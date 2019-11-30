#include "mt_arena.h"
#include <mutex>
#include "module.h"
#include <SceneCommon.pb.h>
#include <BattleReply.pb.h>
#include <ActorConfig.pb.h>
#include "active_model.h"
#include "../mt_player/mt_player.h"
#include "../data/data_manager.h"
#include "../mt_battle/mt_battle_ground.h"
#include "../mt_chat/mt_word_manager.h"
#include "../base/mt_db_load_work.h"
#include "../mail/static_message_manager.h"
#include "../mt_data_cell/mt_data_cell.h"
#include "../mt_data_cell/mt_data_container.h"
#include "../mt_player/mt_player.h"
#include "../mt_guid/mt_guid.h"
#include "../mt_actor/mt_actor.h"
#include "../ranklist/top_ranklist.h"
#include "../base/mt_timer.h"
#include "random_generator.h"
#include "../mt_scene/mt_scene_manager.h"
#include "../mt_cache/mt_shm_manager.h"

namespace Mt
{
	static MtArenaManager* __mt_arena_manager = nullptr;
	REGISTER_MODULE(MtArenaManager)
	{
		require("MtActorConfig");
		require("data_manager");
		register_load_function(module_base::STAGE_LOAD, boost::bind(&MtArenaManager::OnLoad, __mt_arena_manager));
		register_option("skip_arena_robot", "do not load arena robot, default is false",
			boost::program_options::value<bool>()->default_value(false),
			opt_delegate<bool>([](bool skip) {
			if (skip)
				MtArenaManager::Instance().SkipArenaRobot(skip);
		}));
	}

	void MtArenaManager::lua_reg(lua_State* ls)
	{
		ff::fflua_register_t<MtArenaManager, ctor()>(ls, "MtArenaManager")
			.def(&MtArenaManager::OnChallengePlayer, "OnChallengePlayer");

		ff::fflua_register_t<>(ls)
			.def(&MtArenaManager::Instance, "LuaMtArenaManager");
	}


	MtArenaManager::MtArenaManager()
	{
		__mt_arena_manager = this;
	}

	MtArenaManager& MtArenaManager::Instance()
	{
		return *__mt_arena_manager;
	}

	void MtArenaManager::SkipArenaRobot(bool skip)
	{
		skip_ = skip;
	}

	void MtArenaManager::AddLogToUsers(
		const boost::shared_ptr<DB::ArenaUser>& challenger,
		const boost::shared_ptr<DB::ArenaUser>& target,
		uint64 reply_guid, bool win)
	{
		static const int32 MAX_LOG_COUNT = 10;
		auto rank_change = 0;
		if (challenger == nullptr || target == nullptr)
			return;
		if (win && challenger->rank() > target->rank())
			 rank_change = challenger->rank() - target->rank();
		if (target->is_robot() == false) {
			Packet::ArenaLog new_log;
			ArenaUser2ArenaPlayer(challenger, *new_log.mutable_player());
			new_log.set_replay_guid(reply_guid);
			new_log.set_rank_change(-rank_change);
			new_log.set_win(!win);
			if (new_log.IsInitialized() && target->mutable_log()
				&& target->mutable_log()->mutable_logs()){
				auto logs = target->mutable_log()->mutable_logs();
				Packet::ArenaLog* new_log_ptr = nullptr;
				if (logs->size() < MAX_LOG_COUNT) {
					new_log_ptr = logs->Add();
				} else {
					for (auto i = 0; i < MAX_LOG_COUNT - 1; ++i) {
						logs->SwapElements(i, i + 1);
					}
					new_log_ptr = logs->Mutable(MAX_LOG_COUNT - 1);
				}
				new_log_ptr->CopyFrom(new_log);
			}
			

		}
		Packet::ArenaLog new_log;
		ArenaUser2ArenaPlayer(target, *new_log.mutable_player());
		new_log.set_replay_guid(reply_guid);
		new_log.set_rank_change(rank_change);
		new_log.set_win(win);
		if (new_log.IsInitialized() && challenger->mutable_log()
			&& challenger->mutable_log()->mutable_logs()) {
			auto logs = challenger->mutable_log()->mutable_logs();
			Packet::ArenaLog* new_log_ptr = nullptr;
			if (logs->size() < MAX_LOG_COUNT) {
				new_log_ptr = logs->Add();
			} else {
				for (auto i = 0; i < MAX_LOG_COUNT - 1; ++i) {
					logs->SwapElements(i, i + 1);
				}
				new_log_ptr = logs->Mutable(MAX_LOG_COUNT - 1);
			}
			new_log_ptr->CopyFrom(new_log);
		}
	}

	int32 MtArenaManager::OnLoad()
	{
		auto arena_robot_config = data_manager::instance()->get_table("arena_robot_config");
		ZXERO_ASSERT(arena_robot_config != nullptr);
		auto arena_rank_reward = data_manager::instance()->get_table("arena_rank_reward");
		ZXERO_ASSERT(arena_rank_reward != nullptr);
		for (auto i = 0; i < arena_rank_reward->get_rows(); ++i) {
			auto row = arena_rank_reward->find_row(i);
			std::string rank_string = row->getString("ranks");
			std::vector<int32> ranks;
			utils::split_string(rank_string.c_str(), "|", ranks);
			ZXERO_ASSERT(!ranks.empty());
			if (ranks.size() == 1) ranks.push_back(ranks.front());
			RankReward reward;
			reward.rank_0_ = ranks[0];
			reward.rank_1_ = ranks[1];
			reward.honor_ = row->getInt("honor");
			reward.crystal_ = row->getInt("crystal");
			reward.gold_ = row->getInt("gold");
			if (ranks[0] == 10001) ranks[1] = std::numeric_limits<int32>::max();
			rank_reward_list_.push_back(reward);
		}
		return 0;
	}

	void MtArenaManager::DrawDailyArenaReward(std::vector<std::pair<int32, uint64>>& guids)
	{
		for (auto pair : guids) {
			LOG_INFO << "[arena] player:" << pair.second << " draw arena reward";
			auto reward = FindRankReward(pair.first);
			std::string reward_content = (boost::format("arena_daily_reward_content|%1%") % pair.first).str();
			google::protobuf::RepeatedPtrField<Packet::ItemCount> items;
			if (reward.gold_ > 0) {
				auto item = items.Add();
				item->set_itemid(Packet::TokenType::Token_Gold);
				item->set_itemcount(reward.gold_);
			}
			if (reward.crystal_ > 0) {
				auto item = items.Add();
				item->set_itemid(Packet::TokenType::Token_Crystal);
				item->set_itemcount(reward.crystal_);
			}
			if (reward.honor_ > 0) {
				auto item = items.Add();
				item->set_itemid(Packet::TokenType::Token_Honor);
				item->set_itemcount(reward.honor_);
			}
			static_message_manager::Instance().create_message(
				pair.second,
				nullptr,
				MAIL_OPT_TYPE::ARENA_ADD,
				"arena_daily_reward_title",
				reward_content,
				&items);

		}
	}

	MtArenaManager::RankReward MtArenaManager::FindRankReward(int32 rank)
	{
		RankReward result;
		for (auto& reward : rank_reward_list_) {
			if (reward.rank_0_ <= rank && rank <= reward.rank_1_) {
				result = reward;
				break;
			}
		}
		return result;
	}

	void MtArenaManager::OnOpenArenaPanel(const boost::shared_ptr<MtPlayer>& player)
	{
		std::lock_guard<std::recursive_mutex> lock(arena_user_map_lock_);
		auto it = arena_users_map_.find(player->Guid());
		if (it == std::end(arena_users_map_)) {
			auto user = boost::make_shared<ArenaMemUser>();
			user->status_ = ArenaMemUser::LoadStatus::START_LOADING;
			user->panel_open_ = true;
			user->last_reload_tick_ = MtTimerManager::Instance().CurrentTime();
			arena_users_map_.insert(std::make_pair(player->Guid(), user));
			Server::Instance().AddDbLoadTask( new LoadPlayerArenaMainData(player, nullptr));
		}
		else {
			if (it->second->status_ == ArenaMemUser::LoadStatus::LOADED) {
				SendDataToClient(player);
			} else if (it->second->status_ == ArenaMemUser::LoadStatus::NEED_RELOAD) {
				it->second->status_ = ArenaMemUser::LoadStatus::START_LOADING;
				Server::Instance().AddDbLoadTask(new LoadPlayerArenaMainData(player, it->second->user_));
			} else if (it->second->status_ == ArenaMemUser::LoadStatus::START_LOADING) {
				player->SendCommonResult(Packet::ResultOption::ArenaOp, Packet::ResultCode::Wait_Loading);
			} else {
				LOG_WARN << "Arena User Load Error:" << player->Guid() << ".status:" << (int32)it->second->status_;
			}
		}
	}

	void MtArenaManager::OnBuyChallengeCount(const boost::shared_ptr<MtPlayer>& player)
	{
		LOG_INFO << "[arena] player:" << player->Guid() << " buy challengecount begin";
		std::lock_guard<std::recursive_mutex> lock(arena_user_map_lock_);
		auto it = arena_users_map_.find(player->Guid());
		if (it != std::end(arena_users_map_)) {
			it->second->last_reload_tick_ = MtTimerManager::Instance().CurrentTime() - (uint64)seconds(11).total_milliseconds();
		}
		OnRefreshPlayerArenaMainData(player);
		LOG_INFO << "[arena] player:" << player->Guid() << " buy challengecount end";
	}

	void MtArenaManager::OnRefreshPlayerArenaMainData(const boost::shared_ptr<MtPlayer>& player)
	{
		std::lock_guard<std::recursive_mutex> lock(arena_user_map_lock_);
		auto it = arena_users_map_.find(player->Guid());
		if (it == std::end(arena_users_map_)) {
			player->SendCommonResult(Packet::ResultOption::ArenaOp, Packet::ResultCode::Invalid_Request);
			return;
		}
		switch (it->second->status_)
		{
		case ArenaMemUser::LoadStatus::LOADED:
			//正常数据有10秒钟cooldown
			if ((MtTimerManager::Instance().CurrentTime() - it->second->last_reload_tick_) > (uint64)seconds(10).total_milliseconds()) {
				it->second->status_ = ArenaMemUser::LoadStatus::START_LOADING;
				it->second->last_reload_tick_ = MtTimerManager::Instance().CurrentTime();
				Server::Instance().AddDbLoadTask(new LoadPlayerArenaMainData(player, it->second->user_));
				LOG_INFO << "[arena] player:" << player->Guid() << "refresh arena info";
			} else {
				LOG_INFO << "[arena] player:" << player->Guid() << "load arena info cooldown";
				player->SendCommonResult(Packet::ResultOption::ArenaOp, Packet::ResultCode::CoolDownLimit);
			}
			break;
		case ArenaMemUser::LoadStatus::NEED_RELOAD:
			//需要重新处理的 
			LOG_INFO << "[arena] player:" << player->Guid() << "start load arena info";
			it->second->status_ = ArenaMemUser::LoadStatus::START_LOADING;
			Server::Instance().AddDbLoadTask(new LoadPlayerArenaMainData(player, it->second->user_));
			break;
		case ArenaMemUser::LoadStatus::START_LOADING:
			LOG_INFO << "[arena] player:" << player->Guid() << "load arena info ongoing";
			player->SendCommonResult(Packet::ResultOption::ArenaOp, Packet::ResultCode::Wait_Loading);
			break;
		default:
			break;
		}
	}

	void MtArenaManager::OnLoadPlayerArenaMainDataDone(
		boost::shared_ptr<DB::ArenaUser>& self,
		std::vector<boost::shared_ptr<DB::ArenaUser>>& targets)
	{
		LOG_INFO << "[arena] player:" << self->player_guid() << " load data done:" << targets.size();
		std::vector<int32> ranks;
		std::transform(targets.begin(), targets.end(), std::back_inserter(ranks),
			boost::bind(&DB::ArenaUser::rank, _1));
		for (auto& arena_user : targets) {
			std::lock_guard<std::recursive_mutex> lock(arena_user_map_lock_);
			auto memory_user_it = arena_users_map_.find(arena_user->player_guid());
			if (memory_user_it != std::end(arena_users_map_))
				continue;
			auto memory_user = boost::make_shared<ArenaMemUser>();
			memory_user->status_ = ArenaMemUser::LoadStatus::LOADED;
			memory_user->user_ = arena_user;
			memory_user->panel_open_ = false;
			arena_users_map_.insert({ arena_user->player_guid(), memory_user });
		}
		std::vector<uint64> guids;
		for (auto rank : ranks) {
			for (auto& pair : arena_users_map_) {
				auto& user = pair.second->user_;
				if (user != nullptr && user->rank() == rank)
					guids.push_back(pair.first);
			}
		}
		std::lock_guard<std::recursive_mutex> lock(arena_user_map_lock_);
		auto it = arena_users_map_.find(self->player_guid());
		if (it != std::end(arena_users_map_)) {
			it->second->status_ = ArenaMemUser::LoadStatus::LOADED;
			it->second->user_ = self;
			it->second->target_guids_ = guids;
		}
		auto player = Server::Instance().FindPlayer(self->player_guid());
		if (player) {
			SendDataToClient(player);
		}
	}

	bool MtArenaManager::ArenaUser2ArenaPlayer(
		const boost::shared_ptr<DB::ArenaUser>& db_user,
		Packet::ArenaPlayer& arena_player)
	{
		if (db_user == nullptr)
			return false;
		arena_player.set_rank(db_user->rank());
		arena_player.set_yesterday_rank(db_user->yesterday_rank());
		arena_player.set_guid(db_user->player_guid());
		arena_player.set_in_battle(false);
		if (db_user->is_robot()) {
			return ArenaRobot2ArenaPlayer(*db_user, arena_player);
		} else {
			auto player = Server::Instance().FindPlayer(db_user->player_guid());
			if (player == nullptr) {
				ZXERO_ASSERT(player != nullptr);
				return false;
			}
			Packet::ArenaPlayerInfo& player_info = *arena_player.mutable_player_info();
			player_info.set_icon(player->GetScenePlayerInfo()->icon());
			player_info.set_level(player->PlayerLevel());
			player_info.set_name(player->Name());
			player_info.set_score(player->BattleScore());
			auto actors = player->BattleActor(Packet::ActorFigthFormation::AFF_PVP_ARENA);
			auto score = 0;
			for (auto& actor : actors) {
				score += actor->DbInfo()->score();
				actor->SerializationToMessage(*arena_player.add_actors());
			}
			return true;
		}
	}

	void MtArenaManager::PackArenaMessage(uint64 player_guid, Packet::ArenaMainData& msg)
	{
		auto self_it = arena_users_map_.find(player_guid);
		if (self_it == std::end(arena_users_map_)) {
			return;
		}
		auto self = self_it->second->user_;
		std::vector<boost::shared_ptr<ArenaMemUser>> arena_targets;
		std::for_each(std::begin(self_it->second->target_guids_), std::end(self_it->second->target_guids_), [&](auto guid) {
			std::lock_guard<std::recursive_mutex> lock(arena_user_map_lock_);
			auto target_it = arena_users_map_.find(guid);
			if (target_it != std::end(arena_users_map_)) {
				arena_targets.push_back(target_it->second);
			}
		});
		msg.set_last_reload_tick(self_it->second->last_reload_tick_);
		msg.mutable_logs()->CopyFrom(self->log().logs());
		ArenaUser2ArenaPlayer(self, *msg.mutable_my_self());
		for (std::size_t i = 0; i < arena_targets.size(); ++i) {
			Packet::ArenaPlayer player_data;
			if (ArenaUser2ArenaPlayer(arena_targets[i]->user_, player_data)) {
				msg.add_targets()->CopyFrom(player_data);
			} else {
				LOG_ERROR << "ArenaUser2ArenaPlayer fails:" << arena_targets[i]->user_->player_guid();
			}
		}
		return;
	}

	void MtArenaManager::SendDataToClient(const boost::shared_ptr<MtPlayer>& player)
	{
		Packet::ArenaMainData msg;
		PackArenaMessage(player->Guid(), msg);
		player->SendMessage(msg);
	}

	void MtArenaManager::OnChallengePlayer(uint64 src_guid, uint64 target_guid, int32 battle_group_index )
	{
		LOG_INFO << "[arena] challenge player:" << src_guid << " target_guid: " << target_guid << " idx:" << battle_group_index;
		auto player = Server::Instance().FindPlayer(src_guid);
		if (player == nullptr) {
			return;
		}
		LOG_INFO << "[arena] challenge2 player:" << src_guid << " target_guid: " << target_guid << " idx:" << battle_group_index;
		std::lock_guard<std::recursive_mutex> lock(arena_user_map_lock_);
		auto target_it = arena_users_map_.find(target_guid);
		auto self_it = arena_users_map_.find(player->Guid());
		if (self_it == std::end(arena_users_map_) || target_it == std::end(arena_users_map_)
			|| self_it->second->user_ == nullptr || target_it->second->user_ == nullptr) {
			return player->SendCommonResult(Packet::ResultOption::ArenaOp, Packet::ResultCode::Wait_Loading);
		}
		auto target_mem_user = target_it->second;
		auto self_mem_user = self_it->second;
		if (target_it->second->in_battle == true || target_it->second->user_ == nullptr) {
			return player->SendCommonResult(Packet::ResultOption::ArenaOp, Packet::ResultCode::Target_Busy);
		}
		boost::shared_ptr<MtPlayer> target_player = nullptr;
		auto target_arena_user = target_it->second->user_;

		if ( !target_arena_user->is_robot()) {
			target_player = Server::Instance().FindPlayer(target_arena_user->player_guid());
			if (target_player == nullptr) {
				player->SendCommonResult(Packet::ResultOption::ArenaOp, Packet::ResultCode::Wait_Loading);
				return;
			}
		}

		player->SetArenaTarget(target_it->second);
		auto battle = player->GetBattleManager()->CreateFrontBattle(player.get(),nullptr,
		{
			{ "script_id", 1012 },
			{ "group_id", battle_group_index }

		});
		if (battle != nullptr) {			
			target_mem_user->in_battle = true;
			self_mem_user->in_battle = true;
		}
		LOG_INFO << "[arena] challenge3 player:" << src_guid << " target_guid: " << target_guid << " idx:" << battle_group_index;
	}

	void MtArenaManager::OnChallengeDone(
		const boost::shared_ptr<MtPlayer>& challenger, 
		const boost::shared_ptr<ArenaMemUser>& target,
		uint64 reply_guid,bool win)
	{
		if (target == nullptr || target->user_ == nullptr) {
			LOG_ERROR << "[arena] OnChallengeDone player:" << challenger->Guid() << " challenge done, target nullptr";
			return;
		}
		if (target->user_) {
			LOG_INFO << "[arena] OnChallengeDone player:" << challenger->Guid() << " target_guid: " << target->user_->player_guid() << " challenge done";
		} else {
			LOG_INFO << "[arena] OnChallengeDone player:" << challenger->Guid() << " challenge done";
		}
		boost::shared_ptr<ArenaMemUser> challenge_arena_user;
		{
			std::lock_guard<std::recursive_mutex> lock(arena_user_map_lock_);
			auto it = arena_users_map_.find(challenger->Guid());
			if (it != arena_users_map_.end())
				challenge_arena_user = it->second;
		}
		if (challenge_arena_user == nullptr) {
			LOG_ERROR << "[arena] OnChallengeDone player:" << challenger->Guid() << " challenge done, self nullptr";
			return;
		}
		target->in_battle = false;
		challenge_arena_user->in_battle = false;
		auto challenger_arena_db_user = challenge_arena_user->user_;
		auto target_db_user = target->user_;
		AddLogToUsers(challenger_arena_db_user, target_db_user, reply_guid, win);
		auto old_rank = challenger_arena_db_user->rank();
		int32 new_rank = target_db_user->rank();
		if (win && old_rank > new_rank) {
			challenge_arena_user->status_ = ArenaMemUser::LoadStatus::NEED_RELOAD;
			challenger_arena_db_user->set_rank(new_rank);
			target_db_user->set_rank(old_rank);
			std::string targetname = "MtOnline";
			int32 targetlevel = 1;
			int32 targetactorid = 1;
			if (target_db_user->is_robot())
			{
				auto robotconfig = MtArenaManager::Instance().FindRobotPlayer(*target_db_user);
				if (robotconfig != nullptr)
				{
					targetname = robotconfig->name_;
					targetactorid = robotconfig->ConfigId();
					targetactorid = robotconfig->level_;
				}
			}
			auto target_user = Server::Instance().FindPlayer(target_db_user->player_guid());
			if (target_user)
			{
				targetname = target_user->Name();
				targetlevel = target_user->PlayerLevel();
				targetactorid = target_user->GetScenePlayerInfo()->init_actor_id();
				target->status_ = ArenaMemUser::LoadStatus::NEED_RELOAD;
				target->target_guids_.clear();
				if (target->panel_open_) {
					target->last_reload_tick_ = MtTimerManager::Instance().CurrentTime() - (uint64)seconds(11).total_milliseconds();
					OnRefreshPlayerArenaMainData(target_user);
				}
			}
			MtTopRankManager::Instance().OnPlayerArenaChanged(challenger,
				new_rank, challenger->BattleScore(), old_rank,
				target_db_user->score(), target_db_user->player_guid(), 
				targetname, targetactorid, targetlevel);
		}
		challenge_arena_user->last_reload_tick_ = MtTimerManager::Instance().CurrentTime() - (uint64)seconds(11).total_milliseconds();
		OnRefreshPlayerArenaMainData(challenger);
		ChallengeReward(challenger, win);
	}

	
	void MtArenaManager::ChallengeReward(const boost::shared_ptr<MtPlayer>& player, bool win)
	{
		LOG_INFO << "[arena] ChallengeReward player:" << player->Guid() 
			<< " level" << player->PlayerLevel() << " win:" << win;
		auto reward_config = MtActorConfig::Instance().GetLevelData(player->PlayerLevel());
		int32 arena_exp = reward_config->arena_exp();
		int32 arena_gold = reward_config->arena_gold();
		int32 arena_honor = reward_config->arena_lose_honor();
		if (win)
			arena_honor = reward_config->arena_win_honor();
		bool first_five_challenge = player->DataCellContainer()->get_data_32(
			Packet::CellLogicType::NumberCell,
			Packet::NumberCellIndex::ArenaBuyTimesToday) == 0;
		if (first_five_challenge) {
			player->AddItemByIdWithNotify(Packet::TokenType::Token_Gold, arena_gold, Config::ItemAddLogType::AddType_Activity,	7);
			player->AddItemByIdWithNotify(Packet::TokenType::Token_Exp, arena_exp, Config::ItemAddLogType::AddType_Activity,	7);
		}
		player->AddItemByIdWithNotify(Packet::TokenType::Token_Honor, arena_honor, Config::ItemAddLogType::AddType_Activity,	7);
	}

	void MtArenaManager::OnCloseArenaPanel(const boost::shared_ptr<MtPlayer>& player)
	{
		auto it = arena_users_map_.find(player->Guid());
		if (it != std::end(arena_users_map_)) {
			it->second->panel_open_ = false;
		}
	}

	void MtArenaManager::OnNewDay()
	{
	}

	void MtArenaManager::OnSave(boost::shared_ptr<zxero::dbcontext>& dbctx)
	{
		for (auto& ele : arena_users_map_) {
			if (ele.second->user_)
				ActiveModel::Base<DB::ArenaUser>(dbctx).Save(ele.second->user_);
		}
	}
	void MtArenaManager::OnCacheAll( )
	{
		std::lock_guard<std::recursive_mutex> lock(arena_user_map_lock_);
		std::set<google::protobuf::Message *> msg_set;
		for (auto& ele : arena_users_map_) {
			auto db_user = ele.second->user_.get();
			if (db_user)
				msg_set.insert(db_user);
		}

		MtShmManager::Instance().Cache2Shm(msg_set, "MtSceneStageManager");

	}

	void MtArenaManager::ArenaActor2ActorFullInfo(const RobotActor& actor,
		Packet::ActorFullInfo& full_actor)
	{
		uint64 actor_guid = MtGuid::Instance().GenGuid<Packet::ActorBasicInfo>();
		auto actor_config = MtActorConfig::Instance().GetBaseConfig(actor.actor_config_id_);
		full_actor.SetInitialized();
		Packet::ActorBasicInfo& actor_info = *full_actor.mutable_actor();
		//init basic info
		actor_info.set_guid(actor_guid);
		actor_info.set_player_guid(0);
		actor_info.set_race(actor_config->race);
		actor_info.set_profession(actor_config->profession);
		actor_info.set_talent(actor_config->talent);
		actor_info.set_level(actor.level_);
		actor_info.set_type(Packet::ActorType::Hero);
		actor_info.set_color(Packet::ActorColor::GREEN_0);
		actor_info.set_star(Packet::ActorStar::ONE);
		actor_info.set_client_res_id(actor.actor_config_id_);
		actor_info.set_actor_config_id(actor.actor_config_id_);
		actor_info.mutable_battle_info()->MergeFrom(actor.battle_info_);
		actor_info.set_score(MtActorConfig::BattleInfo2Score(actor_info.battle_info()));
		//init skill
		auto actor_basic = MtActorConfig::Instance().GetBaseConfig(actor.actor_config_id_);
		if (actor_basic) {
			for (auto skill_id : actor_basic->skills) {
				auto skill = full_actor.add_skills();
				skill->set_actor_guid(actor_guid);
				skill->set_guid(0);
				skill->set_skill_id(skill_id);
				skill->set_skill_level(1);
			}
			if (actor_basic->green_skill > 0) {
				auto skill = full_actor.add_skills();
				skill->set_actor_guid(actor_guid);
				skill->set_guid(0);
				skill->set_skill_id(actor_basic->green_skill);
				skill->set_skill_level(1);
			}
		}
		//init equip
		for (auto equip_id : actor.equip_ids_) {
			auto equip = full_actor.add_equips();
			equip->SetInitialized();
			equip->set_actor_guid(actor_guid);
			equip->set_config_id(equip_id);
			equip->set_durable(100);
			equip->set_enhence_level(1);
		}
	}

	bool MtArenaManager::ArenaRobot2ArenaPlayer(DB::ArenaUser& db_info, Packet::ArenaPlayer& player_info)
	{
		if (robot_player_map_.find(db_info.robot_id()) == robot_player_map_.end())
			return false;
		Packet::ArenaPlayerInfo* info = player_info.mutable_player_info();
		RobotPlayer* robot_player = robot_player_map_[db_info.robot_id()].get();
		info->set_icon(robot_player->icon_);
		info->set_level(robot_player->level_);
		info->set_name(robot_player->name_);
		info->set_score(robot_player->score_);
		player_info.set_rank(db_info.rank());
		player_info.set_yesterday_rank(db_info.yesterday_rank());
		for (RobotActor& actor : robot_player->actors_) {
			Packet::ActorFullInfo& full_actor = *player_info.add_actors();
			ArenaActor2ActorFullInfo(actor, full_actor);
		}
		return true;
	}

	void MtArenaManager::OnCheckInit(boost::shared_ptr<zxero::dbcontext>& dbctx)
	{
		auto arena_robot_config = data_manager::instance()->get_table("arena_robot_config");
		if (!skip_) {
			LOG_INFO << "[arena] OnCheckInit" << " load arena robot";
			boost::shared_ptr<RobotPlayer> robot_player = boost::make_shared<RobotPlayer>();
			for (auto i = 0; i < arena_robot_config->get_rows(); ++i) {
				auto row = arena_robot_config->find_row(i);
				auto name = (boost::format("Robot_%1%") % i).str();
				RobotActor actor;
				actor.LoadFrom(*row);
				if (robot_player->rank_ == 0 || robot_player->rank_ == actor.rank_) {
					robot_player->PushBack(actor);
				} else {
					robot_player = boost::make_shared<RobotPlayer>();
					robot_player->PushBack(actor);
				}
				robot_player_map_[robot_player->rank_] = robot_player;
				if (i % 1000 == 0)
					LOG_INFO << "Load Arena Robot:" << i;
			}
		}
		auto count = ActiveModel::Base<DB::ArenaUser>(dbctx).Count({});
		if (count == 0) {//need init
			LOG_INFO << "[arena] OnCheckInit" << " create arena robot";
			for (auto& pair : robot_player_map_){
				const RobotPlayer& robot_player_info = *pair.second;
				DB::ArenaUser arena_user;
				arena_user.set_rank(robot_player_info.rank_);
				arena_user.set_yesterday_rank(robot_player_info.rank_);
				arena_user.set_player_guid(MtGuid::Instance().GenGuid<DB::ArenaUser>());
				arena_user.set_is_robot(true);
				arena_user.set_robot_id(robot_player_info.rank_);
				arena_user.set_updated_at(now_second());
				arena_user.set_score(robot_player_info.score_);
				arena_user.mutable_log();
				arena_user.mutable_trial_copy();
				ActiveModel::Base<DB::ArenaUser>(dbctx).Create(arena_user);
				LOG_INFO << "Create Arena Robot, rank:" << arena_user.rank();
			}

			std::set<uint64> loadplayersceneinfo;
			MtTopRankManager::Instance().OnLoadArena(dbctx, loadplayersceneinfo);
		}
	}

	const boost::shared_ptr<DB::ArenaUser> MtArenaManager::FindArenaUser(uint64 guid)
	{
		auto it = arena_users_map_.find(guid);
		if (it == std::end(arena_users_map_)) {
			return nullptr;
		}
		if (it->second->status_ == ArenaMemUser::LoadStatus::LOADED) {
			return it->second->user_;
		}
		return nullptr;
	}


	bool MtArenaManager::TargetBusy(uint64 guid)
	{
		auto it = arena_users_map_.find(guid);
		if (it == arena_users_map_.end() || it->second->in_battle)
			return true;
		return false;
	}

	void MtArenaManager::OnPlayerScoreChanged(const boost::shared_ptr<MtPlayer>& player, int32 score)
	{
		
		boost::shared_ptr<Mt::ArenaMemUser> arena_user;
		{
			std::lock_guard<std::recursive_mutex> lock(arena_user_map_lock_);
			auto it = arena_users_map_.find(player->Guid());
			if (it != arena_users_map_.end()) {
				arena_user = it->second;
			}
		}
		if (arena_user != nullptr && arena_user->user_ != nullptr) {
			arena_user->user_->set_score(score);
			arena_user->user_->mutable_trial_copy()->clear_actors();
			auto battle_actors = player->BattleActor(Packet::ActorFigthFormation::AFF_PVP_GUARD);
			for (auto& actor : battle_actors) {
				actor->SerializationToMessage(*arena_user->user_->mutable_trial_copy()->add_actors());
			}
		}
	}

	const MtArenaManager::RobotPlayer* MtArenaManager::FindRobotPlayer(DB::ArenaUser& arene_user)
	{
		if (arene_user.is_robot()) {
			auto it = robot_player_map_.find(arene_user.robot_id());
			if (it == robot_player_map_.end())
				return nullptr;
			else
				return it->second.get();
		}
		return nullptr;
	}

	void MtArenaManager::RobotActor::LoadFrom(data_row& row)
	{
		actor_config_id_ = row.getInt("actor_config_id");
		rank_ = row.getInt("rank");
		level_ = row.getInt("level");
		battle_info_.set_hp(row.getInt("hp"));
		battle_info_.set_physical_attack(row.getInt("physical_attack"));
		battle_info_.set_physical_armor(row.getInt("physical_armor"));
		battle_info_.set_magic_attack(row.getInt("magic_attack"));
		battle_info_.set_magic_armor(row.getInt("magic_armor"));
		battle_info_.set_accurate(row.getInt("accurate"));
		battle_info_.set_critical(row.getInt("critical"));
		battle_info_.set_dodge(row.getInt("dodge"));
		battle_info_.set_toughness(row.getInt("toughness"));
		battle_info_.set_move_speed(row.getInt("move_speed"));
		score_ = MtActorConfig::BattleInfo2Score(battle_info_);
		auto base_config = MtActorConfig::Instance().GetBaseConfig(actor_config_id_);
		if (base_config) {
			icon_ = base_config->icon;
		}
		for (auto i = 0; i < 5; ++i) {
			auto skill_id = row.getInt((boost::format("skill_%1%") % i).str().c_str());
			if (skill_id > 0) {
				skill_ids_.push_back(skill_id);
			}
		}
		for (auto i = 0; i < 8; ++i) {
			auto equip_id = row.getInt((boost::format("equip_%1%") % i).str().c_str());
			if (equip_id > 0) {
				equip_ids_.push_back(equip_id);
			}
		}
	}


	void MtArenaManager::RobotPlayer::PushBack(const RobotActor& actor)
	{
		if (rank_ == 0) {
			rank_ = actor.rank_;
			icon_ = actor.icon_;
			level_ = actor.level_;
			auto config = MtActorConfig::Instance().GetBaseConfig(actor.actor_config_id_);
			name_ = MtWordManager::Instance()->RandomName(config->gender);
		}
		score_ += actor.score_;
		actors_.push_back(actor);
	}

}