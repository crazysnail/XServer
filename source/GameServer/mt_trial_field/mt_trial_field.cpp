#include "mt_trial_field.h"
#include "TrialFieldDB.pb.h"
#include <SceneCommon.pb.h>
#include <S2GMessage.pb.h>
#include "module.h"
#include "active_model.h"
#include "../mt_player/mt_player.h"
#include "../mt_actor/mt_actor.h"
#include "../mt_server/mt_server.h"
#include "../base/mt_db_load_work.h"
#include "../mt_cache/mt_shm_manager.h"

namespace Mt
{
	static TrialFieldManager* __trial_field_manager = nullptr;
	REGISTER_MODULE(TrialFieldManager)
	{
		require("data_manager");
		register_load_function(module_base::STAGE_LOAD, boost::bind(&TrialFieldManager::OnLoad, __trial_field_manager));
	}

	TrialFieldManager& TrialFieldManager::Instance()
	{
		return *__trial_field_manager;
	}

	TrialFieldManager::TrialFieldManager()
	{
		__trial_field_manager = this;
	}

	int32 TrialFieldManager::OnLoad()
	{
		return 0;
	}

	void TrialFieldManager::OnSaveAll(const boost::shared_ptr<dbcontext>& dbctx)
	{
		for (auto& ele : trial_info_map_) {
			auto trial_data = ele.second;
			if (trial_data.info_)
				ActiveModel::Base<DB::PlayerTrialInfo>(dbctx).Save(*trial_data.info_);
		}		
	}

	void TrialFieldManager::OnCacheAll()
	{
		std::set<google::protobuf::Message *> msg_set;
		for (auto& ele : trial_info_map_) {
			msg_set.insert(ele.second.info_.get());
		}
		MtShmManager::Instance().Cache2Shm(msg_set, "TrialFieldManager");
	}


	bool TrialFieldManager::OnRefreshTarget(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<S2G::RefreshTarget>& message, int32 /*source*/)
	{
		if (player == nullptr || message == nullptr) {
			return true;
		}

		RefreshTarget(player, message->wave());
		return true;
	}

	void TrialFieldManager::RefreshTarget(const boost::shared_ptr<MtPlayer>& player, int32 wave)
	{
		if (player == nullptr)
			return;
		LOG_INFO << "[trial_field] player:" << player->Guid() << " refresh target, wave:" << wave;
		auto it = trial_info_map_.find(player->Guid());
		if (it == std::end(trial_info_map_)) {
			Server::Instance().AddDbLoadTask(new LoadTrialPlayerInfoWork(player, wave, player->PowerCount()));
			LOG_INFO << "[trial_field] player:" << player->Guid() << " refresh target, wave:" << wave
				<< " create load task";
		/*	TrialData trial_data{ TrialData::LoadStatus::LOGING, nullptr };
			trial_info_map_[player->Guid()] = trial_data;
			trial_data.info_ = LoadDataFromMemory(player);
			if (trial_data.info_ == nullptr) {
				Server::Instance().AddDbLoadTask(new LoadTrialPlayerInfoWork(player));
				return;
			}*/
		} else {
			auto trial_data = it->second;
			if (trial_data.status_ == TrialData::LoadStatus::LOGING) {
				LOG_INFO << "[trial_field] player:" << player->Guid() << " refresh target, wave:" << wave
					<< " data load ongoing";
				player->SendCommonResult(Packet::ResultOption::Hell_Opt, Packet::ResultCode::Wait_Loading);
				return;
			}
			auto trial_player = trial_data.info_;
			auto all_passed = std::all_of(trial_player->target().targets().begin(),
				trial_player->target().targets().end(),
				boost::bind(&DB::TrialTarget::passed, _1) == true);
			if (trial_data.wave_ != wave || all_passed) {
				LOG_INFO << "[trial_field] player:" << player->Guid() << " refresh target, wave:" << wave
					<< " all target passed, reload";
				trial_player->mutable_target()->clear_targets();
				Server::Instance().AddDbLoadTask(new LoadTrialPlayerInfoWork(player, wave, player->PowerCount()));
				/*auto trial_player_from_mem = LoadDataFromMemory(player);
				if (trial_player_from_mem == nullptr) {
					Server::Instance().AddDbLoadTask(new RefreshTrialPlayerInfoWork(player,trial_player));
					return;
				} else {
					trial_data.info_ = trial_player_from_mem;
					trial_info_map_[player->Guid()] = trial_data;
					Server::Instance().OnTrialDataLoadDone(player, trial_data.info_);
				}*/
			}
			else {
				//如果还有没挑战完成的，继续上次挑战数据
				Server::Instance().OnTrialDataLoadDone(player, trial_player);
				LOG_INFO << "[trial_field] player:" << player->Guid() << " refresh target, wave:" << wave
					<< " not all target passwd";
			}
		}
	}

	void TrialFieldManager::OnDataLoad(int32 wave, 
		boost::shared_ptr<DB::PlayerTrialInfo>& trial_player,
		const boost::shared_ptr<MtPlayer>& player)
	{
		if (trial_player == nullptr)
			return;
		LOG_INFO << "[trial_field] OnDataLoad player:" << trial_player->guid() << " data load done";
		for (auto& target : trial_player->target().targets()) {
			LOG_INFO << "[trial_field] OnDataLoad player:" << trial_player->guid()
				<< "," << target.player().guid()
				<< "," << target.player().actor_config_id();
		}
		trial_info_map_[player->Guid()] = TrialData{ TrialData::LoadStatus::DONE, wave, trial_player };
		Server::Instance().OnTrialDataLoadDone(player, trial_player);
	}

	/*boost::shared_ptr<DB::PlayerTrialInfo> TrialFieldManager::LoadDataFromMemory(const boost::shared_ptr<MtPlayer>& player)
	{
		LOG_INFO << "[trial_field] LoadDataFromMemory player:" << player->Guid();
		auto online_players = Server::Instance().OnlinePlayer();
		if (online_players.size() > 3) {
			auto i = 0;
			auto trial_info = boost::make_shared<DB::PlayerTrialInfo>();
			trial_info->set_guid(player->Guid());
			auto target_info = trial_info->mutable_target();
			for (auto it = std::begin(online_players); it != std::end(online_players) && i < 3; ++it) {
				auto target = target_info->add_targets();
				target->set_passed(false);
				target->set_posid(-1);
				target->set_rewarded(false);
				if (it->first != player->Guid()) {
					++i;
					target->mutable_player()->CopyFrom(*it->second->GetScenePlayerInfo());
					auto target_actors = it->second->BattleActor(Packet::ActorFigthFormation::AFF_PVP_GUARD);
					for (auto& actor : target_actors) {
						actor->SerializationToMessage(*target->add_actors());
					}
				}
			}
			return trial_info;
		} else {
			return nullptr;
		}
	}*/

	boost::shared_ptr<DB::PlayerTrialInfo> TrialFieldManager::PlayerTrialInfo(const boost::shared_ptr<MtPlayer>& player)
	{
		auto it = trial_info_map_.find(player->Guid());
		if (it == std::end(trial_info_map_)) {
			return nullptr;
		}
		return it->second.info_;
	}

}