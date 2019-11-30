#include "mt_scene_stage.h"
#include "module.h"
#include "../data/data_manager.h"
#include "active_model.h"
#include "../base/client_session.h"
#include "../mt_guid/mt_guid.h"
#include "../mt_server/mt_server.h"
#include "../mt_player/mt_player.h"
#include "../mt_actor/mt_actor.h"
#include "../base/mt_db_load_work.h"
#include "../mt_scene/mt_scene_manager.h"
#include "../mt_skill/mt_skill.h"
#include "../mt_item/mt_item_package.h"
#include "../mt_cache/mt_shm_manager.h"
#include "../mt_data_cell/mt_data_container.h"
#include "../mt_config/mt_config.h"
#include "../mail/static_message_manager.h"
#include "../mt_chat/mt_chatmessage.h"
#include "../mt_item/mt_item.h"
#include <SceneStageConfig.pb.h>
#include <BaseConfig.pb.h>
#include <S2GMessage.pb.h>
#include "../mt_scene/mt_scene.h"
#include <SceneStage.pb.h>

#include <boost/format.hpp>

namespace Mt
{
static MtSceneStageManager* __mt_scene_stage_manager = nullptr;

void MtSceneStageManager::lua_reg(lua_State* ls)
{
	ff::fflua_register_t<MtSceneStageManager, ctor()>(ls, "MtSceneStageManager")
		.def(&MtSceneStageManager::GetConfig, "GetConfig")
		.def(&MtSceneStageManager::DefaultStageId, "DefaultStageId")
		.def(&MtSceneStageManager::GetNextStageId, "GetNextStageId")
		.def(&MtSceneStageManager::GetGroupSceneStageByGroupId, "GetGroupSceneStageByGroupId")
		;
	ff::fflua_register_t<>(ls)
		.def(&MtSceneStageManager::Instance, "LuaMtSceneStageManager");
}
REGISTER_MODULE(MtSceneStageManager)
{
	require("data_manager");
	register_load_function(module_base::STAGE_LOAD, boost::bind(&MtSceneStageManager::OnLoad, __mt_scene_stage_manager));

}
zxero::int32 MtSceneStageManager::OnLoad()
{
	auto table = data_manager::instance()->get_table("scene_stage");
	for (auto i = 0; i < table->get_rows(); ++i) {
		auto row = table->find_row(i);
		auto config = boost::make_shared<Config::SceneStageConfig>();
		FillMessageByRow(*config, *row);
		for (auto j = 1; j <= 5; ++j) {
			auto col_name = (boost::format("monster%1%") % j).str();
			auto value = row->getInt(col_name.c_str());
			if (value > 0) {
				config->add_normal_monsters(value);
			}
			col_name = (boost::format("boss%1%") % j).str();
			value = row->getInt(col_name.c_str());
			if (value > 0) {
				config->add_bosses(value);
			}
		}
		configs_.insert({ config->id(), config });
		if (first_group_stage_map_.find(config->group()) == first_group_stage_map_.end()) {
			first_group_stage_map_[config->group()] = config->id();
		}
		bool no_pre_stage = std::all_of(config->pre_stages().begin(),
			config->pre_stages().end(), [](auto& id) {return id == -1; });
		if (alliance_default_stage_ == 0 && config->camp() == 1 && no_pre_stage)
			alliance_default_stage_ = config->id();
		if (horde_default_stage_ == 0 && config->camp() == 2 && no_pre_stage)
			horde_default_stage_ = config->id();
	}
	return 0;
}


void MtSceneStageManager::OnDbConnected() {
	Server::Instance().AddDbLoadTask(new SceneStageLoader());
}

MtSceneStageManager::MtSceneStageManager()
{
	__mt_scene_stage_manager = this;
}

MtSceneStageManager& MtSceneStageManager::Instance()
{
	return *__mt_scene_stage_manager;
}
/*

void MtSceneStageManager::OnLoadPlayerSceneStage(const std::vector<boost::shared_ptr<Packet::PlayerSceneStage>>& player_scene_stages)
{
	std::lock_guard<std::recursive_mutex> l(player_scene_stage_mutex_);
	for (auto& db_info : player_scene_stages) {
		if (player_scene_stages_.find({ db_info->player_guid(), db_info->stage_id() }) != player_scene_stages_.end()) {
			player_scene_stages_[{db_info->player_guid(), db_info->stage_id()}]->CopyFrom(*db_info);
		} else {
			player_scene_stages_.insert({ { db_info->player_guid(), db_info->stage_id()}, db_info });
		}
		db_info->ClearDirty();
	}
}*/
void MtSceneStageManager::InitSceneStageInfo(const boost::shared_ptr<dbcontext>& dbctx)
{
	for (auto& config : configs_) {
		auto scene_stage = ActiveModel::Base<Packet::SceneStageInfo>(dbctx).Create({ { "stage_id", config.first} });
		scene_stages_.insert({scene_stage->stage_id(), scene_stage});
	}
}

void MtSceneStageManager::OnLoadSceneStageInfo(const std::vector<boost::shared_ptr<Packet::SceneStageInfo>>& stages)
{
	for (auto& db_info : stages) {
		scene_stages_.insert({ db_info->stage_id(), db_info });
		db_info->ClearDirty();
	}
}

/*
void MtSceneStageManager::OnLoadSceneStageNew(const std::vector<boost::shared_ptr<Packet::SceneStageNew>>& stages)
{
	for (auto& db_info : stages) {
		scene_stages_new_.insert({ db_info->stage_id(), db_info });
		db_info->ClearDirty();
	}
}*/

const Config::SceneStageConfig* MtSceneStageManager::GetConfig(zxero::int32 stage_id) const
{
	auto it = configs_.find(stage_id);
	if (it == std::end(configs_)) {
		return nullptr;
	}
	else {
		return (it->second).get();
	}
}


bool MtSceneStageManager::OnCacheAll( )
{
	std::set<google::protobuf::Message *> msg_set;
	for (auto& info : scene_stages_){
		msg_set.insert(info.second.get());
	}		

/*
	for (auto& info : scene_stages_new_){
		msg_set.insert(info.second.get());
	}
	{
		std::lock_guard<std::recursive_mutex> l(player_scene_stage_mutex_);
		for (auto& info : player_scene_stages_) {
			msg_set.insert(info.second.get());
		}
	}*/
	MtShmManager::Instance().Cache2Shm(msg_set, "MtSceneStageManager");

	return true;
}

/*
void MtSceneStageManager::UpdateCapturedStage(const boost::shared_ptr<MtPlayer>& player)
{
	LOG_INFO << "[stage] player:" << player->Guid() << "try update capture stage";
	for (auto& stage : scene_stages_new_) {
		if (stage.second->captain_guid() == player->Guid()) {
			LOG_INFO << "[stage] player:" << player->Guid() << "update capture stage:" << stage.second->stage_id();
			auto guards = player->BattleActor(Packet::ActorFigthFormation::AFF_STAGE_GUARD);
			auto captain_info = stage.second->mutable_captain();
			captain_info->mutable_player_info()->CopyFrom(*player->GetScenePlayerInfo());
			auto guard_formation = captain_info->mutable_guard_formation();
			guard_formation->clear_actors();
			for (auto& actor : guards) {
				actor->SerializationToMessage(*guard_formation->add_actors());
			}
		}
	}
}*/
/*

bool MtSceneStageManager::PlayerCaptureStage(zxero::int32 stage_id, bool success,  MtPlayer* player, const std::vector<boost::shared_ptr<MtActor>>& formation)
{
	formation;
	auto scene_stage = MutableSceneStageInfo(stage_id);
	LOG_INFO << "[stage] player:" << player->Guid() << " capture stage:" << stage_id << " success:" << success;
	if (success) {
		for (auto& stage : scene_stages_new_) {
			if (stage.second->captain_guid() == player->Guid()) {
				stage.second->set_captain_guid(INVALID_GUID);
			}
		}
		auto notify = boost::make_shared<Packet::ClientSceneStageInfo>();
		auto old_captain_guid = scene_stage->captain_guid();
		scene_stage->mutable_challengers()->mutable_names()->Clear();
		scene_stage->set_captain_guid(player->Guid());
		auto captain_info = scene_stage->mutable_captain();
		captain_info->mutable_player_info()->CopyFrom(*player->GetScenePlayerInfo());
		auto guard_formation = captain_info->mutable_guard_formation();
		guard_formation->mutable_actors()->Clear();
		auto guard_actors = player->BattleActor(Packet::ActorFigthFormation::AFF_STAGE_GUARD);
		for (auto& actor : guard_actors) {
			actor->SerializationToMessage(*guard_formation->add_actors());
		}
		SceneStageInfo2ClientInfo(scene_stage, *notify, player->Guid());
		player->SendMessage(*notify);
		if (old_captain_guid != INVALID_GUID) {//通知原来的占领者, 关卡被占领了
			auto old_captain = Server::Instance().FindPlayer(old_captain_guid);
			if (old_captain) {
				old_captain->SendMessage(*notify);
			}
		}
	}
	else {
		if (scene_stage->captain_guid() == INVALID_GUID) {
			return true;
		}
		const int32 max_challenger_name_log_count = 6;
		auto player_name = player->Name();
		auto challenger_names = scene_stage->mutable_challengers();
		if (challenger_names->names_size() == 6) {
			for (auto i = 0; i < max_challenger_name_log_count - 1; ++i) {
				*challenger_names->mutable_names(i) = *challenger_names->mutable_names(i + 1);
			}
			*challenger_names->mutable_names(max_challenger_name_log_count - 1) = player_name;
		}
		else {
			challenger_names->add_names(player_name);
		}
		auto notify = boost::make_shared<Packet::GuardStageSuccess>();
		notify->set_name(player->Name());
		notify->set_stage_id(scene_stage->stage_id());
		auto old_captain = Server::Instance().FindPlayer(scene_stage->captain_guid());
		if (old_captain) {
			old_captain->SendMessage(*notify);
		}
	}
	return true;
}
*/

/*

void MtSceneStageManager::UpdateSceneStageOnHookRank(int32 stage_id, 
	uint64 player_guid, const std::string& player_name,
	int32 total_time, int32 total_income)
{
	auto run_time_rank_it = run_time_on_hook_rank_users_.find(stage_id);
	if (run_time_rank_it == std::end(run_time_on_hook_rank_users_)) {
		run_time_on_hook_rank_users_.insert({ stage_id, boost::make_shared<Packet::OnHookRankUserRuntime>() });
		run_time_rank_it = run_time_on_hook_rank_users_.find(stage_id);
	}
	//更新排行榜 start
	auto& ranks = run_time_rank_it->second;
	const auto max_user_ranks = 15;
	auto user_rank_it = std::find_if(std::begin(*ranks->mutable_users()),
		std::end(*ranks->mutable_users()), 
		boost::bind(&Packet::OnHookRankUser::player_guid, _1) == player_guid);
	Packet::OnHookRankUser* rank = nullptr;
	if (user_rank_it != std::end(*ranks->mutable_users()))
		rank = &*user_rank_it;
	else {
		if (ranks->users_size() < max_user_ranks)
			rank = ranks->add_users();
		else
			rank = &*(ranks->mutable_users()->rbegin());
	}
	rank->set_player_guid(player_guid);
	rank->set_name(player_name);
	rank->set_seconds(total_time);
	rank->set_income_per_min(total_income);
	std::sort(std::begin(*ranks->mutable_users()),
		std::end(*ranks->mutable_users()), [](auto& lhs, auto& rhs) {
		return lhs.seconds() > rhs.seconds();
	});
	//更新排行榜 end
}

bool MtSceneStageManager::PlayerRaidStage(zxero::int32 stage_id,
	int32 time_cost, int32 formation_score, 
	MtPlayer* player,
	const std::vector<boost::shared_ptr<MtActor>>& formation)
{
	LOG_INFO << "[stage] player:" << player->Guid() << " raid stage:" << stage_id;
	auto config_it = configs_.find(stage_id);
	if (config_it == std::end(configs_)) {
		ZXERO_ASSERT(false)<<"invalid stage config data! stage id= "<<stage_id;
		return false;
	}
	auto offline_config = player->GetPlayerOfflineHookData();
	if (offline_config->hook_datas_size() > 0) {
		auto last_hook_data = offline_config->mutable_hook_datas(offline_config->hook_datas_size() - 1);
		if (last_hook_data->stage_id() == 0) {
			last_hook_data->set_stage_id(stage_id);
		}
	}
	auto scene_stage_info = MutableSceneStageInfo(stage_id);
	if (scene_stage_info == nullptr) {
		return false;
	}
		
	if ((formation_score != -1 && scene_stage_info->best_score() < formation_score)
		|| scene_stage_info->best_score() == 0) {
		auto best_formation = scene_stage_info->mutable_best_formation();
		best_formation->mutable_actors()->Clear();
		best_formation->set_player_name(player->Name());
		scene_stage_info->set_best_score(formation_score);
		for (auto& actor : formation) {
			actor->SerializationToMessage(*best_formation->add_actors());
		}
	}
	if ((time_cost != -1 && scene_stage_info->fast_time_cost() > time_cost)
		|| scene_stage_info->fast_time_cost() == 0) {
		auto fast_formation = scene_stage_info->mutable_fast_formation();
		fast_formation->set_player_name(player->Name());
		fast_formation->mutable_actors()->Clear();
		scene_stage_info->set_fast_time_cost(time_cost);
		for (auto& actor : formation) {
			actor->SerializationToMessage(*fast_formation->add_actors());
		}
	}
	auto player_stage_data = MutablePlayerSceneStageInfo(player->Guid(), stage_id);
	if (player_stage_data == nullptr) {
		return false;
	}
	if (player_stage_data->raid()) {
		return true;
	}
	player_stage_data->set_raid(true);
	player_stage_data->set_challenge_boss_progress_num(0);

	int32 count = calc_challenge_boss_count(player, config_it->second->raid_count(), config_it->second->rush_count());
	player_stage_data->set_challenge_boss_progress_den(count);

	Packet::PlayerStageDataChangedNotify notify;
	notify.mutable_stage()->CopyFrom(*player_stage_data);
	notify.set_promot(true);
	player->SendMessage(notify);
	//
	player->OnLuaFunCall_x("xPlayerRaidStage",
	{
		{ "value", stage_id }
	});
	LOG_INFO << "[stage] player:" << player->Guid() << " raid stage:" << stage_id << "success";
	return true;
}*/
	/*
zxero::uint64 MtSceneStageManager::StageCaptainGuid(int32 / *stage_id* /) const
{
	return INVALID_GUID;
}

void MtSceneStageManager::GmPlayerRaidAllStage(const boost::shared_ptr<MtPlayer>& player)
{
	std::vector<boost::shared_ptr<MtActor>> formation;
	formation.push_back(player->MainActor());
	for (auto& ele : configs_) {
		PlayerRaidStage(ele.first, -1, -1, player.get(), formation);
	}
}

bool MtSceneStageManager::IncBossChallengeCount(const boost::shared_ptr<MtPlayer>& player, int32 stage_id, int32 count)
{
	auto stage_info = MutablePlayerSceneStageInfo(player->Guid(), stage_id);
	if (stage_info == nullptr) {
		return false;
	}

	stage_info->set_challenge_boss_time(count);

	Packet::PlayerStageDataChangedNotify notify;
	notify.mutable_stage()->CopyFrom(*stage_info);
	player->SendMessage(notify);
	return true;
}

bool MtSceneStageManager::DecBossChallengeCount(const boost::shared_ptr<MtPlayer>& player, int32 stage_id, int32 count)
{
	auto stage_info =  MutablePlayerSceneStageInfo(player->Guid(), stage_id);
	if (stage_info == nullptr) {
		return false;
	}

	if (stage_info->challenge_boss_time() < count) {
		return false;
	}
	stage_info->set_challenge_boss_time(stage_info->challenge_boss_time() - count);
	if (stage_info->raid() == false) {
		stage_info->set_challenge_boss_progress_num(0); //如果没有通关,清空进度
	}
	Packet::PlayerStageDataChangedNotify notify;
	notify.mutable_stage()->CopyFrom(*stage_info);
	player->SendMessage(notify);
	return true;
}*/
/*

int32 MtSceneStageManager::calc_challenge_boss_count(const MtPlayer* player, const int32 cur_count, int32 rush_count )
{
	if (player != nullptr) {
		auto container = player->DataCellContainer();
		if (container != nullptr) {
			if( container->check_bit_data(Packet::BitFlagCellIndex::MonthCardFlag) 
				||	container->check_bit_data(Packet::BitFlagCellIndex::LifeCardFlag)) {

				auto config = MtConfigDataManager::Instance().FindConfigValue("card_wave_value");
				if (config != nullptr && config->value1()>0) {
					rush_count -= config->value1();
				}				
			}
		}
	}
	auto old = cur_count;
	if (old <= 20) {
		old += 2;			
	}
	else if (old < rush_count) {
		old += 1;
	}

	if(old>= rush_count){
		old = rush_count;
	}
	return old;
}*/
/*

bool MtSceneStageManager::AddBossChallengeProgress(const MtPlayer* player, int32 stage_id, int32 count)
{
	auto result = false;
	auto config_it = configs_.find(stage_id);
	if ( config_it != std::end(configs_)) {
		Packet::PlayerStageDataChangedNotify notify;
		auto stage_data = MutablePlayerSceneStageInfo(player->Guid(), stage_id);
		if (stage_data->raid()) {

			int32 old_count = stage_data->challenge_boss_progress_den();
			if (old_count <= config_it->second->raid_count()) {
				stage_data->set_challenge_boss_progress_den(config_it->second->raid_count());
			}
			auto num = stage_data->challenge_boss_progress_num() + count;
			if (num >= stage_data->challenge_boss_progress_den()) {

				auto challenge_count = num / stage_data->challenge_boss_progress_den();
				auto new_num = num % stage_data->challenge_boss_progress_den();
				stage_data->set_challenge_boss_time(stage_data->challenge_boss_time() + challenge_count);
				stage_data->set_challenge_boss_progress_num(new_num);

				int32 new_cout = calc_challenge_boss_count(player, old_count, config_it->second->rush_count());
				stage_data->set_challenge_boss_progress_den(new_cout);

				result = true;
			} else {
				stage_data->set_challenge_boss_progress_num(num);
			}
		}
		else {//未通关, 最多只有一次挑战boss机会
			stage_data->set_challenge_boss_progress_den(config_it->second->raid_count());
			auto num = stage_data->challenge_boss_progress_num() + count;
			if (num >= config_it->second->raid_count()) {
				stage_data->set_challenge_boss_time(1);
				stage_data->set_challenge_boss_progress_num(config_it->second->raid_count());
			}
			else {
				stage_data->set_challenge_boss_progress_num(num);
			}
		}
		notify.mutable_stage()->CopyFrom(*stage_data);
		player->SendMessage(notify);
	}
	return result;
}
*/

/*
bool MtSceneStageManager::PlayerHasSceneStageInfo(uint64 player_guid, int32 scene_stage_id)
{
	std::lock_guard<std::recursive_mutex> l(player_scene_stage_mutex_);
	return player_scene_stages_.find({ player_guid, scene_stage_id }) != std::end(player_scene_stages_);
}*/

/*
Packet::PlayerSceneStage MtSceneStageManager::RunTimePlayerSceneStageInfo(uint64 player_guid, int32 scene_stage_id)
{
	auto config_it = configs_.find(scene_stage_id);
	if (config_it != std::end(configs_)) {
		std::lock_guard<std::recursive_mutex> l(player_scene_stage_mutex_);
		auto it = player_scene_stages_.find({ player_guid, scene_stage_id });
		if (it == std::end(player_scene_stages_)) {
			Packet::PlayerSceneStage new_info;
			new_info.set_guid(INVALID_GUID);
			new_info.set_stage_id(scene_stage_id);
			new_info.set_player_guid(player_guid);
			new_info.set_raid(false);
			new_info.set_challenge_boss_time(0);
			new_info.set_challenge_boss_progress_num(0);
			new_info.set_challenge_boss_progress_den(config_it->second->raid_count());
			return new_info;
		}
	}
	ZXERO_ASSERT(false);
	return Packet::PlayerSceneStage();
}*/
/*

boost::shared_ptr<Packet::PlayerSceneStage> MtSceneStageManager::MutablePlayerSceneStageInfo(uint64 player_guid, int32 scene_stage_id)
{
	auto config_it = configs_.find(scene_stage_id);
	if (config_it != std::end(configs_)) {
		std::lock_guard<std::recursive_mutex> l(player_scene_stage_mutex_);
		if (PlayerHasSceneStageInfo(player_guid, scene_stage_id) == false) {
			auto new_info = boost::make_shared<Packet::PlayerSceneStage>();
			auto tmp_info = RunTimePlayerSceneStageInfo(player_guid, scene_stage_id);
			new_info->CopyFrom(tmp_info);
			new_info->set_guid(MtGuid::Instance()(*new_info));
			player_scene_stages_.insert({ { player_guid, scene_stage_id }, new_info });
			return new_info;
		}
		else {
			return player_scene_stages_.find({ player_guid, scene_stage_id })->second;
		}
	}
	else {
		ZXERO_ASSERT(false);
		return nullptr;
	}
}*/

zxero::int32 MtSceneStageManager::DefaultStageId(int32 camp) const
{
	if (Packet::Camp(camp) == Packet::Camp::Alliance)
		return alliance_default_stage_;
	if (Packet::Camp(camp) == Packet::Camp::Horde)
		return horde_default_stage_;
	ZXERO_ASSERT(false);
	return 0;
}
/*

std::vector<boost::shared_ptr<MtActor>> MtSceneStageManager::StageCaptain(int32 stage_id)
{
	std::vector<boost::shared_ptr<MtActor>> ret;
	auto it = scene_stages_new_.find(stage_id);
	if (it == std::end(scene_stages_new_) || it->second->captain_guid() == INVALID_GUID
		|| it->second->captain().has_guard_formation()  == false
		|| it->second->captain().guard_formation().actors_size() == 0) {
		return ret;
	}
	else {
		for (auto& full_actor_info : it->second->captain().guard_formation().actors()) {
			ret.push_back(MtActor::CreateBattleCopyFromMsg(full_actor_info));
			auto actor = boost::make_shared<MtActor>(full_actor_info);
		}
		return ret;
	}
}*/

int32 MtSceneStageManager::CollectTax(const boost::shared_ptr<MtPlayer>& player, int32 stage_id, int32 gold)
{
	auto stage_config = MtSceneStageManager::Instance().GetConfig(stage_id);
	auto scene_stage_info = GetSceneStageInfo(stage_id);
	if (stage_config == nullptr ||  scene_stage_info == nullptr) {
		return 0;
	}

	if (player != nullptr) {
		auto container = player->DataCellContainer();
		if (container != nullptr) {
			if (container->check_bit_data(Packet::BitFlagCellIndex::MonthCardFlag )
				||	container->check_bit_data(Packet::BitFlagCellIndex::LifeCardFlag )){
				return 0;
			}
		}
	}
	auto tax = 0;
	if (scene_stage_info->captain_guid() != INVALID_GUID && scene_stage_info->tax_rate() > 0) {
		tax = int32(scene_stage_info->tax_rate() / 100.f *  gold);
		auto final_tax = scene_stage_info->tax() + tax;
		scene_stage_info->set_tax(final_tax);
		if (final_tax > stage_config->tax_broadcast_mark() && !scene_stage_info->tax_broadcasted()) {
			std::stringstream msg;
			scene_stage_info->set_tax_broadcasted(true);
			msg << "ac_notify_031|" << player->Name() << "|" << stage_config->name() << "|" << MtItemManager::Instance().GetItemName(Packet::TokenType::Token_Gold) << "|" << final_tax;
			send_run_massage(msg.str());
		}
	}
	auto group_scene_stage = GetGroupSceneStageByGroupId(stage_config->group());
	if (group_scene_stage && group_scene_stage->guild_tax_rate() > 0) {
		int32 guild_tax = int32(group_scene_stage->guild_tax_rate() / 100.f * gold);
		tax += guild_tax;
		auto final_tax = group_scene_stage->guild_tax() + guild_tax;
		group_scene_stage->set_guild_tax(final_tax);
		if (final_tax > stage_config->guild_tax_broadcat_mark() && !group_scene_stage->guild_tax_broadcasted()){
			group_scene_stage->set_guild_tax_broadcasted(true);
			std::string str = (boost::format("ac_notify_046|capture_point_name_%1%|%2%|%3%") 
				% stage_config->group() 
				% MtItemManager::Instance().GetItemName(Packet::TokenType::Token_GuildMoney) 
				% final_tax).str();
			send_run_massage(str);
		}
	}	
	return tax;
}


/*
void MtSceneStageManager::LoadPlayerStages(uint64 player_guid) const
{
	Server::Instance().AddDbLoadTask(new PlayerStagesLoadWork(player_guid));
}

void MtSceneStageManager::OnLoadPlayerStages(const boost::shared_ptr<Packet::PlayerStages>& data)
{
	if (player_stages_.find(data->guid()) == player_stages_.end()) {
		player_stages_[data->guid()] == data;
	}
}*/
/*

Packet::PlayerStages* MtSceneStageManager::FindPlayerStages(uint64 player_guid) const
{
	if (player_stages_.find(player_guid) == player_stages_.end()) {
		return nullptr;
	}
	return player_stages_.at(player_guid).get();
}*/

bool MtSceneStageManager::OnPlayerHookDataUpdate(
	const boost::shared_ptr<MtPlayer>& /*player*/,
	const boost::shared_ptr<S2G::PlayerHookDataUpdate>& message,
	int32 /*source*/)
{
	int32 stage_id = message->stage_id();
	auto info = GetSceneStageInfo(stage_id);
	if (info == nullptr) {
		LOG_INFO << "[scene_stage] player capture stage cannot find stage" << stage_id;
		return false;
	}
	//最快阵容
	if (info->fast_time_cost() == 0 || info->fast_time_cost() > message->time_cost()) {
		info->set_fast_time_cost(message->time_cost());
		info->mutable_fast_formation()->CopyFrom(message->form());
	}
	//最高评分
	if (info->best_score() < message->score()) {
		info->set_best_score(message->score());
		info->mutable_best_formation()->CopyFrom(message->form());
	}
	//更新时长排行榜 start
	const static int32 MAX_RANK_USER = 15;
	auto hook_users = info->mutable_ranks()->mutable_users();
	auto rank_user_it = std::find_if(hook_users->begin(),
		hook_users->end(), boost::bind(&Packet::HookRankUser::player_guid, _1) == message->guid());
	Packet::HookRankUser* user = nullptr;
	if (rank_user_it != hook_users->end()) {//自己在排行中,更新
		user = &*rank_user_it;
	} else {
		//自己不在榜内. 人数未满直接加入
		if (hook_users->size() < MAX_RANK_USER) {
			user = hook_users->Add();
		} else {//如果比最后一名时间长, 替换最后一个
			Packet::HookRankUser& last_user = *(hook_users->rbegin());
			if (last_user.seconds() < message->hook_time()) {
				user = &last_user;
			}
		}
	}
	if (user != nullptr) {
		user->set_player_guid(message->guid());
		user->set_name(message->name());
		user->set_seconds(message->hook_time());
		user->set_income_per_min(message->income_per_min());
		user->set_exp_pre_min(message->exp_pre_min());
		std::sort(hook_users->begin(), hook_users->end(),
			[](auto& lhs, auto& rhs) {
			return lhs.seconds() > rhs.seconds();
		});
		if (hook_users->size() > MAX_RANK_USER) {
			hook_users->RemoveLast();
		}
	}
	return true;
	//更新时长排行榜 end
}

void MtSceneStageManager::ClearPlayerOldCaptrueStage(const boost::shared_ptr<MtPlayer>& player)
{
	for (auto& scene_stage : scene_stages_) {
		if (scene_stage.second->captain_guid() == player->Guid()) {
			scene_stage.second->set_captain_guid(INVALID_GUID);
			scene_stage.second->mutable_captain()->Clear();
			scene_stage.second->mutable_challengers()->Clear();
			player->SendMessage(*scene_stage.second);
		}
	}
}

bool MtSceneStageManager::OnPlayerCaptureStage(
	const boost::shared_ptr<MtPlayer>& player, 
	const boost::shared_ptr<S2G::PlayerCaptureStatge>& message,
	int32 /*source*/)
{
	int32 stage_id = message->stage_id();
	auto info = GetSceneStageInfo(stage_id);
	auto config = GetConfig(stage_id);
	if (info == nullptr || config == nullptr) {
		LOG_ERROR << "[scene_stage] player capture stage cannot find stage" << stage_id;
		return false;
	}
	LOG_INFO << "[scene_stage] player:" << player->Guid()
		<< ",captrue stage:" << stage_id << ",win:" << message->win_battle();
	if (message->win_battle()) {
		ClearPlayerOldCaptrueStage(player);
		if (message->has_player_info() == false) {
			LOG_INFO << "[scene_stage] player capture stage win battle has no player_info";
			return false;
		}
		auto old_captain_guid = info->captain_guid();
		info->set_captain_guid(message->player_info().guid());
		info->mutable_captain()->mutable_player_info()->CopyFrom(message->player_info());
		info->mutable_captain()->mutable_actors()->CopyFrom(message->actors());
		info->mutable_challengers()->Clear();
		player->SendMessage(*info);		
		std::string lost_content = (boost::format("scene_stage_lost_notify_content|%1%|%2%")
			% player->Name() % config->name()).str();
		auto old_captain = Server::Instance().FindPlayer(old_captain_guid);
		static_message_manager::Instance().create_message(old_captain_guid,
			old_captain, MAIL_OPT_TYPE::PVP_ADD,
			"scene_stage_lost_notify_title",lost_content);
		if (old_captain) {//pvp
			old_captain->SendMessage(*info);
			std::stringstream msg;
			msg << "ac_notify_033|" << player->Name() << "|" << old_captain->Name() << "|" << config->name();
			send_run_massage(msg.str());
		}
		else {//pve
			std::stringstream msg;
			msg << "ac_notify_034|" << player->Name() << "|monster_name_" << config->monster_id() << "|" << config->name();
			send_run_massage(msg.str());
		}
	} else {
		const static int32 MAX_CHALLENGER_LOGS = 6;
		auto challenger_names = info->mutable_challengers();
		if (challenger_names->names_size() == 6) {
			for (auto i = 0; i < MAX_CHALLENGER_LOGS - 1; ++i) {
				*challenger_names->mutable_names(i) = *challenger_names->mutable_names(i + 1);
			}
			*challenger_names->mutable_names(MAX_CHALLENGER_LOGS - 1) = message->player_info().name();
		} else {
			challenger_names->add_names(message->player_info().name());
		}
		auto notify = boost::make_shared<Packet::GuardStageSuccess>();
		notify->set_name(player->Name());
		notify->set_stage_id(stage_id);
		auto old_captain = Server::Instance().FindPlayer(info->captain_guid());
		if (old_captain) {
			old_captain->SendMessage(*notify);
		}
	}
	return true;
}

bool MtSceneStageManager::OnPlayerUpdateCaptureForm(
	const boost::shared_ptr<MtPlayer>& player,
	const boost::shared_ptr<S2G::PlayerUpdateCaptureForm>& message,
	int32 /*source*/)
{
	auto player_guid = message->player_info().guid();
	for (auto& pair : scene_stages_) {
		if (pair.second->captain_guid() == player_guid) {
			LOG_INFO << "[scene_stage] player update capture stage_id: " << pair.first
				<< " captian:" << pair.second->captain_guid() << " player:" << player_guid;
			pair.second->mutable_captain()->mutable_player_info()->CopyFrom(message->player_info());
			pair.second->mutable_captain()->mutable_actors()->CopyFrom(message->actors());
			player->SendMessage(*pair.second);
		}
	}
	return true;
}

bool MtSceneStageManager::OnUpdateStageTax(const boost::shared_ptr<client_session>& session,
	const boost::shared_ptr<Packet::UpdateStageTax>& message, int32 /*source*/)
{
	if (session == nullptr || session->player() == nullptr)
	{
		return false;
	}
	auto player = session->player();
	auto stage_info = GetSceneStageInfo(message->stage_id());
	if (stage_info == nullptr || stage_info->captain_guid() != player->Guid()
		|| message->new_tax() < 0 || message->new_tax() > 5) {
		player->SendCommonResult(Packet::ResultOption::SCENE_STAGE, Packet::ResultCode::InvalidConfigData);
		return true;
	}
	LOG_INFO << "[scene_stage] update tax,player:" << player->Guid()
		<< ",stage:" << stage_info->stage_id() << ",tax:" << message->new_tax();
	stage_info->set_tax_rate(message->new_tax());
	player->SendMessage(*stage_info);

	auto config = GetConfig(message->stage_id());
	if (config != nullptr) {
		std::stringstream msg;
		msg << "ac_notify_032|" << player->Name() << "|" << config->name() << "|" << message->new_tax();
		send_run_massage(msg.str());
	}

	return true;
}

bool MtSceneStageManager::OnGetSceneStageInfo(
	const boost::shared_ptr<client_session>& session,
	const boost::shared_ptr<Packet::GetSceneStageInfo>& message, int32 /*source*/)
{
	if (session == nullptr || session->player() == nullptr) {
		return false;
	}
	auto player = session->player();
	for (auto stage_id : message->stage_ids()) {
		auto scene_stage_info = MtSceneStageManager::Instance().GetSceneStageInfo(stage_id);
		if (scene_stage_info != nullptr) {
			player->SendMessage(*scene_stage_info);
		} else {
			player->SendCommonResult(Packet::ResultOption::SCENE_STAGE, Packet::ResultCode::InvalidConfigData);
		}
	}
	return true;
}

bool MtSceneStageManager::OnCollectTax(const boost::shared_ptr<client_session>& session, const boost::shared_ptr<Packet::CollectAllTax>& message, int32 /*source*/)
{
	if (session == nullptr || session->player() == nullptr)
	{
		return false;
	}
	auto player = session->player();
	message->set_tax(0);
	auto stage_info = MtSceneStageManager::Instance().GetSceneStageInfo(message->stage_id());
	if (stage_info == nullptr || stage_info->captain_guid() != player->Guid()
		|| stage_info->tax() == 0) {
		player->SendCommonResult(Packet::ResultOption::SCENE_STAGE, Packet::ResultCode::SCENE_STAGE_NO_TAX);
		return true;
	}
	auto origin_tax = stage_info->tax();
	message->set_tax(origin_tax);
	stage_info->set_tax(0);
	stage_info->set_tax_broadcasted(false);
	LOG_INFO << "[scene_stage] player collect, player:" << player->Guid()
		<<",tax:" << message->tax();
	player->ExecuteMessage(message);
	player->SendMessage(*stage_info);

	return true;
}

bool MtSceneStageManager::OnCaptureStage(
	const boost::shared_ptr<client_session>& session,
	const boost::shared_ptr<Packet::CaptureStage>& message, int32 /*source*/)
{
	if (session == nullptr || session->player() == nullptr) {
		return false;
	}
	auto player = session->player();
	auto stage_id = message->stage_id();
	auto scene_stage_info = GetSceneStageInfo(stage_id);
	auto config = GetConfig(stage_id);
	if (scene_stage_info == nullptr || config == nullptr) {
		LOG_WARN << "[scene_stage] capture stage null target. player:"
			<< player->Guid() << " stage_id:" << stage_id;
	}
	if (!config->can_captrue()) {
		return false;
	}
	auto msg = boost::make_shared<Packet::CaptureStageToScene>();
	msg->mutable_client_req()->CopyFrom(*message);
	if (scene_stage_info->captain_guid() != INVALID_GUID) {
		msg->mutable_actors()->CopyFrom(scene_stage_info->captain().actors());
		msg->set_boss_group_id(0);
	} else {
		msg->set_boss_group_id(config->guard_id());
	}
	player->ExecuteMessage(msg);
	return true;
}

Packet::SceneStageInfo* MtSceneStageManager::GetSceneStageInfo(int32 stage_id) const
{
	auto it = scene_stages_.find(stage_id);
	if (it == scene_stages_.end()) {
		return nullptr;
	}
	return it->second.get();
}

zxero::int32 MtSceneStageManager::GetStageDefaultDen(int32 stage_id) const
{
	auto config = GetConfig(stage_id);
	if (config == nullptr) {
		LOG_ERROR << "[MtSceneStageManager] no stage:" << stage_id;
		return 40;
	} else {
		return config->raid_count();
	}
}

int32 MtSceneStageManager::GetNextStageId(int32 stage_id) const
{
	for (auto& config : configs_) {
		auto& pre_stages = config.second->pre_stages();
		if (std::any_of(pre_stages.begin(), pre_stages.end(), [=](auto id) {return id == stage_id; })) {
			return config.second->id();
		}
	}
	return 0;
}

Packet::SceneStageInfo* MtSceneStageManager::GetGroupSceneStageByGroupId(int32 group_id) const
{
	auto group_it = first_group_stage_map_.find(group_id);
	if (group_it == first_group_stage_map_.end()) {
		return nullptr;
	} else {
		return GetSceneStageInfo(group_it->second);
	}
}

}