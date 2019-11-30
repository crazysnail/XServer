#include "mt_mission.h"
#include "mt_mission_proxy.h"
#include "module.h"
#include "../data/data_manager.h"
#include "../mt_player/mt_player.h"
#include "../mt_server/mt_server.h"
#include "../base/mt_db_save_work.h"
#include "../mt_cache/mt_shm_manager.h"
#include "active_model.h"
#include <ItemAndEquip.pb.h>
#include <ItemConfig.pb.h>
#include <MarketConfig.pb.h>
#include <ActivityPacket.pb.h>
#include "MissionConfig.proto.fflua.h"
#include "MissionConfig.proto.fflua.h"
#include "../mt_guid/mt_guid.h"
#include "utils.h"


#include "../zxero/mem_debugger.h"

namespace Mt
{

	static MtMissionManager* __mission_manager = nullptr;

	REGISTER_MODULE(MtMissionManager)
	{
		require("data_manager");
		register_load_function(module_base::STAGE_LOAD, boost::bind(&MtMissionManager::OnLoad, __mission_manager));
	}

	void MtMissionManager::lua_reg(lua_State* ls)
	{
		ff::fflua_register_t<MtMissionManager, ctor()>(ls, "MtMissionManager")
			.def(&MtMissionManager::FindMissionAddOn, "FindMissionAddOn")
			.def(&MtMissionManager::GetCarbonTinyThreeMission,"GetCarbonTinyThreeMission")
			.def(&MtMissionManager::GetCarbonBigThreeMission, "GetCarbonBigThreeMission")
			.def(&MtMissionManager::FindMission, "FindMission")
			.def(&MtMissionManager::FindMissionEx, "FindMissionEx")
			.def(&MtMissionManager::GenCommonRewardMission, "GenCommonRewardMission")
			.def(&MtMissionManager::GenNextExtraRewardMission, "GenNextExtraRewardMission")
			.def(&MtMissionManager::GenArchaeologyMission,"GenArchaeologyMission")
			.def(&MtMissionManager::GenCorpsMission,"GenCorpsMission")
			.def(&MtMissionManager::GenCarbonTinyThreeMission, "GenCarbonTinyThreeMission")
			.def(&MtMissionManager::GenCarbonBigThreeMission, "GenCarbonBigThreeMission")
			.def(&MtMissionManager::GenGuildMission, "GenGuildMission")
			.def(&MtMissionManager::GetLastMissionId, "GetLastMissionId")			
			.def(&MtMissionManager::GetMissionIndex, "GetMissionIndex")
			.def(&MtMissionManager::FindTarget, "FindTarget")
			.def(&MtMissionManager::GetLevelMissions,"GetLevelMissions")
			.def(&MtMissionManager::GetAllTarget,"GetAllTarget")
			.def(&MtMissionManager::Type2Mission, "Type2Mission")
			.def(&MtMissionManager::FindStoryStage, "FindStoryStage")
			;

		ff::fflua_register_t<>(ls)
			.def(&MtMissionManager::Instance, "LuaMtMissionManager");
	}

	bool MtMissionManager::LoadPlayerMission( const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<zxero::dbcontext>& dbctx)
	{
		auto missions = ActiveModel::Base<Packet::Mission>(dbctx).FindAll({ { "player_guid", player->Guid() } });
	
		for (auto& child : missions) {
			//脏数据
			if (child->state() == Packet::MissionState::MissionFinish
				|| child->state() == Packet::MissionState::MissionDrop) {
				child->SetDeleted();
				player->Cache2Save(child.get());
				continue;
			}

			int32 id = (int32)child->guid();
			auto mission_config = FindMission(id);
			if (!mission_config){
				LOG_ERROR << "invalid mission config data! player guid=" << player->Guid() << " mission id=" << id;
				continue;
			}

			boost::shared_ptr<Config::MissionExConfig>mission_config_ex = nullptr;
			if (mission_config->finish_type() == Config::FinishType::KillMonster) {
				mission_config_ex = FindMissionEx(mission_config->param(0));
				if (mission_config_ex==nullptr)	{
					LOG_ERROR << "invalid mission config ex! guid" << player->Guid() << " mission ex id=" << mission_config->param(0);
					continue;
				}
			}
			
			auto mission = MtMissionManager::Instance().CreateMission(player->Guid(), mission_config.get(), mission_config_ex.get());
			if (mission != nullptr) {
				auto db = mission->MissionDB();
				db->CopyFrom(*child);
				player->GetMissionProxy()->LoadMission(mission);
			}			
		}

		return true;
	}
	

	MtMissionManager::MtMissionManager()
	{
		__mission_manager = this;
	}
	MtMissionManager::~MtMissionManager()
	{
		mission_pool_.release();
	}

	MtMissionManager& MtMissionManager::Instance()
	{
		return *__mission_manager;
	}

	int32 MtMissionManager::OnLoad()
	{
		auto table = data_manager::instance()->get_table("mission");
		for (auto i = 0; i < table->get_rows(); ++i)
		{
			auto config = boost::make_shared<Config::MissionConfig>();
			auto row = table->find_row(i);			
			FillMessageByRow(*config, *row);

			if (!config->is_open()) {
				continue;
			}
			mission_configs_.insert({ config->id(), config });
		}

		table = data_manager::instance()->get_table("mission_ex");
		for (auto i = 0; i < table->get_rows(); ++i)
		{
			auto config = boost::make_shared<Config::MissionExConfig>();
			auto row = table->find_row(i);
			FillMessageByRow(*config, *row);
			mission_configs_ex_.insert({ config->index(), config });
		}
		table = data_manager::instance()->get_table("battle_plot");
		for (auto i = 0; i < table->get_rows(); ++i) {
			auto stage = boost::make_shared<Config::StoryStageConfig>();
			auto row = table->find_row(i);
			FillMessageByRow(*stage, *row);
			story_stages_.insert({ row->getInt("id"), stage });
		}

		//建立触发任务表
		std::vector<int32> triggers;
		for (auto child:mission_configs_){
			triggers.clear();
			for (auto triger : mission_configs_){
				for (int32 i = 0; i < triger.second->pre_id_size(); i++){
					if (triger.second->pre_id(i) == child.first){
						triggers.push_back(triger.first);
					}
				}
			}
			mission_add_on_.insert({ child.first, triggers });
		}

		
		std::map<int32,int32> reward_missions;
		std::map<int32, int32> extra_missions;
		for (auto child : mission_configs_)	{	

			//建立赏金任务索引表
			if (child.second->type() == Config::MissionType::RewardMission)	{
				reward_missions.clear();
				int32 circle = child.second->circle();
				if (circle >= 0) {
					if (common_reward_mission_.find(circle) == common_reward_mission_.end()) {
						reward_missions.insert({ child.first,child.second->rate() });
						common_reward_mission_.insert({ circle , reward_missions });
						common_reward_weight_.insert({ circle , child.second->rate() });
					}
					else {
						common_reward_mission_[circle].insert({ child.first, child.second->rate() });
						//计算每组的总权重
						common_reward_weight_[circle] += child.second->rate();
					}
				}
			}

			//附加赏金任务表
			if (child.second->type() == Config::MissionType::ExtraRewardMission) {
				extra_missions.clear();
				int32 mission_thread = child.second->group();
				if (mission_thread >= 0){
					if (extra_reward_mission_.find(mission_thread) == extra_reward_mission_.end()) {
						extra_missions.insert({ child.second->circle(),child.first });
						extra_reward_mission_.insert({ mission_thread , extra_missions });
						extra_reward_weight_.insert({ mission_thread , child.second->rate() });
					}
					else {
						extra_reward_mission_[mission_thread].insert({ child.second->circle(),child.first });
						extra_reward_weight_[mission_thread] = child.second->rate();
					}
				}	
			}
		
			//考古任务索引表
			if (child.second->type() == Config::MissionType::ArchaeologyMission) {
				archaeology_mission_.push_back(child.first);
			}

			//暮光军团任务索引表
			if (child.second->type() == Config::MissionType::CorpsMission) {
				corps_mission_.push_back(child.first);
			}

			if (child.second->type() == Config::MissionType::TinyThreeCarbonMission) {
				auto iter = tinythree_carbon_mission_.find(child.second->group());
				if (iter == tinythree_carbon_mission_.end()) {
					std::vector<int32> temp;
					temp.push_back(child.first);
					tinythree_carbon_mission_.insert({ child.second->group(),temp });
				}
				else {
					if (std::find(tinythree_carbon_mission_[child.second->group()].begin(), 
						tinythree_carbon_mission_[child.second->group()].end(), child.first) != 
						tinythree_carbon_mission_[child.second->group()].end()) {
						ZXERO_ASSERT(false);
					}
					else {
						tinythree_carbon_mission_[child.second->group()].push_back(child.first);
					}
				}
			}

			if (child.second->type() == Config::MissionType::BigThreeCarbonMission) {
				auto iter = bigthree_carbon_mission_.find(child.second->group());
				if (iter == bigthree_carbon_mission_.end()) {
					std::vector<int32> temp;
					temp.push_back(child.first);
					bigthree_carbon_mission_.insert({ child.second->group(),temp });
				}
				else {
					if (std::find(bigthree_carbon_mission_[child.second->group()].begin(), 
						bigthree_carbon_mission_[child.second->group()].end(), child.first) !=
						bigthree_carbon_mission_[child.second->group()].end()) {
						ZXERO_ASSERT(false);
					}
					else {
						bigthree_carbon_mission_[child.second->group()].push_back(child.first);
					}
				}
			}
					

			if (child.second->type() == Config::MissionType::UnionMission) {
				auto iter = guild_carbon_mission_.find(child.second->circle());
				if (iter == guild_carbon_mission_.end()) {
					std::vector<int32> temp;
					temp.push_back(child.first);
					guild_carbon_mission_.insert({ child.second->circle(),temp });
				}
				else {
					if (std::find(guild_carbon_mission_[child.second->circle()].begin(),
						guild_carbon_mission_[child.second->circle()].end(), child.first) !=
						guild_carbon_mission_[child.second->circle()].end()) {
						ZXERO_ASSERT(false);
					}
					else {
						guild_carbon_mission_[child.second->circle()].push_back(child.first);
					}
				}
			}

			if (child.second->type() == Config::MissionType::LevelMission) {
				if (level_mission_.find(child.first) != level_mission_.end()) {
					ZXERO_ASSERT(false) << "repeated mission ,id=" << child.first;					
				}
				level_mission_.insert({ child.first,child.second });
			}			
		}	

		//计算附加赏金任务每组的总权重
		for (auto child : extra_reward_weight_) {
			extra_reward_all_weight_ += child.second;
		}		

		//目标奖励
		std::set<int32, less<int32>> group;
		table = data_manager::instance()->get_table("target_reward");
		for (auto i = 0; i < table->get_rows(); ++i)
		{
			auto config = boost::make_shared<Config::TargetConfig>();
			auto row = table->find_row(i);
			FillMessageByRow(*config, *row);
			if (!config->switch_open())
				continue;


			target_configs_.insert({ config->index(), config });
			group.insert(config->max_level());
		}

		//分段
		int32 i = 0; 
		for (auto child : group) {
			for (auto target : target_configs_) {
				if (child == target.second->max_level()) {
					target.second->set_group(i);
				}
			}
			i++;
		}		

		return 0;
	}

	boost::shared_ptr<Config::TargetConfig> MtMissionManager::FindTarget(int32 index)
	{
		if (target_configs_.find(index) == target_configs_.end())
			return nullptr;

		return target_configs_[index];
	}

	std::vector<boost::shared_ptr<Config::TargetConfig>> MtMissionManager::GetAllTarget(const int32 camp,const int32 level)
	{
		std::vector<boost::shared_ptr<Config::TargetConfig>> temp;
		for (auto child : target_configs_) {
			if( child.second->camp() != -1 && child.second->camp() != camp )
				continue;
			if( child.second->min_level() > level )
				continue;
			temp.push_back(child.second);
		}
		return temp;
	}

	int32 MtMissionManager::GenCarbonTinyThreeMission(int32 group, int32 mission_id)
	{
		auto iter = tinythree_carbon_mission_.find(group);
		if (iter == tinythree_carbon_mission_.end()) {
			return -1;
		}

		if (tinythree_carbon_mission_[group].empty()){
			return  -1;
		}

		if (mission_id < 0) {
			return tinythree_carbon_mission_[group][0];
		}
		else {
			int32 next_id = mission_id + 1;
			if (std::find(tinythree_carbon_mission_[group].begin(), tinythree_carbon_mission_[group].end(), next_id) != tinythree_carbon_mission_[group].end()) {
				return next_id;
			}
			else {
				// 表名当前组已经做完了 [4/12/2017 SEED]
				return -2;
			}
		}
		return -1;
	}

	std::vector<int32> MtMissionManager::GetCarbonTinyThreeMission(int32 group)
	{
		auto iter = tinythree_carbon_mission_.find(group);
		if (iter != tinythree_carbon_mission_.end()) {
			return iter->second;
		}
		std::vector<int32> temp;
		return temp;
	}
	int32 MtMissionManager::GenCarbonBigThreeMission(int32 group, int32 mission_id)
	{
		auto iter = bigthree_carbon_mission_.find(group);
		if (iter == bigthree_carbon_mission_.end()) {
			return -1;
		}

		if (bigthree_carbon_mission_[group].empty()) {
			return  -1;
		}

		if (mission_id < 0) {
			return bigthree_carbon_mission_[group][0];
		}
		else {
			int32 next_id = mission_id + 1;
			if (std::find(bigthree_carbon_mission_[group].begin(), bigthree_carbon_mission_[group].end(), next_id) != bigthree_carbon_mission_[group].end()) {
				return next_id;
			}
			else {
				// 表名当前组已经做完了 [4/12/2017 SEED]
				return -2;
			}
		}
		return -1;

	}
	
	std::vector<int32> MtMissionManager::GetCarbonBigThreeMission(int32 group)
	{
		auto iter = bigthree_carbon_mission_.find(group);
		if (iter != bigthree_carbon_mission_.end()) {
			return iter->second;
		}
		std::vector<int32> temp;
		return temp;
	}

	boost::shared_ptr<Config::MissionConfig> MtMissionManager::FindMission(int32 config_id)
	{
		auto it = mission_configs_.find(config_id);
		if (it == mission_configs_.end()) {
			return nullptr;
		}
		return it->second;
	}

	boost::shared_ptr<Config::MissionExConfig> MtMissionManager::FindMissionEx(int32 index)
	{
		auto it = mission_configs_ex_.find(index);
		if (it == mission_configs_ex_.end()) {
			return nullptr;
		}
		return it->second;
	}

	std::vector<int32> MtMissionManager::FindMissionAddOn(int32 config_id)
	{		
		auto it = mission_add_on_.find(config_id);
		if (it == mission_add_on_.end()) {
			std::vector<int32> temp;
			return temp;
		}
		return it->second;
	}

	std::vector<int32> MtMissionManager::GetLevelMissions(const int32 lastid, const int32 level, const int32 camp)
	{
		std::vector<int32> temp;
		for (auto child : level_mission_) {
			if (child.first > lastid 
				&& child.second->open_level() <= level 
				&& ( camp == child.second->group() || child.second->group() == -1 )) {
				temp.push_back(child.first);
			}
		}
		return temp;
	}

	int32 MtMissionManager::GetMissionIndex(int32 mission_id)
	{
		if (mission_id < 10000)
			return 0;
		return mission_id / 10000 - 1;
	}
			
	int32 MtMissionManager::GenCommonRewardMission(int32 circle)
	{
		auto iter = common_reward_mission_.find(circle);
		if (iter == common_reward_mission_.end()){
			return -1;
		}

		auto iter_weight = common_reward_weight_.find(circle);
		if (iter_weight == common_reward_weight_.end()) {
			return -1;
		}

		//根据每一组的总权重随机出一个值
		int32 weight_seed = rand_gen->intgen(1, iter_weight->second);
		int32 weight_base = 0;
		
		for (auto child : iter->second)
		{
			weight_base += child.second;
			if ( weight_seed <= weight_base) {
				//命中
				return child.first;
			}
		}

		return -1;
	}
	int32 MtMissionManager::GenNextExtraRewardMission(int32 mission_id)
	{		
		if (mission_id <= 0) {
			int32 mission_thread = 1;
			//起始任务要随机
			int32 weight_seed = rand_gen->intgen(1, extra_reward_all_weight_);
			int32 weight_base = 0;
			for (auto child : extra_reward_weight_)	{
				weight_base += child.second;
				if ( weight_seed <= weight_base ) {
					//命中
					mission_thread = child.first;
					break;
				}
			}

			auto iter = extra_reward_mission_.find(mission_thread);
			if (iter != extra_reward_mission_.end()) {
				if(iter->second.find(1) != iter->second.end()){ 
					return iter->second[1];
				}
			}			
			return -1;
		}
		else{
			auto config = mission_configs_.find(mission_id);
			if (config == mission_configs_.end())
				return -1;

			int32 mission_thread = config->second->group();
			int32 mission_circel = config->second->circle();

			for (auto child : extra_reward_mission_){
				if (child.first == mission_thread) {
					if (child.second.find(mission_circel + 1) != child.second.end()){
						return child.second[mission_circel + 1];
					}
				}
			}

			return -1;
		}
	}
	//
	int32 MtMissionManager::GenArchaeologyMission()
	{
		if (archaeology_mission_.empty())
			return -1;

		int32 seed= rand_gen->intgen(0, archaeology_mission_.size()-1);
		return archaeology_mission_[seed];
	}
	//
	int32 MtMissionManager::GenCorpsMission()
	{
		if (corps_mission_.empty())
			return -1;

		int32 seed = rand_gen->intgen(0, corps_mission_.size() - 1);
		return corps_mission_[seed];
	}
	int32 MtMissionManager::GenGuildMission(int32 circle)
	{
		auto missionlist = guild_carbon_mission_.find(circle);
		if (missionlist == guild_carbon_mission_.end())
			return -1;

		int32 allrate = 0;
		for (auto missionid : missionlist->second)
		{
			auto mission = FindMission(missionid);
			if (mission != nullptr)
			{
				allrate += mission->rate();
			}
		}
		int32 seed = rand_gen->intgen(0, allrate);
		int32 seedrate = 0;
		for (auto missionid : missionlist->second)
		{
			auto mission = FindMission(missionid);
			if (mission != nullptr)
			{
				seedrate += mission->rate();
				if (seedrate >= seed)
				{
					return mission->id();
				}
			}
		}
		return -1;
	}
	int32 MtMissionManager::GetLastMissionId(Config::MissionType mission_type, int32 group)
	{
		int32 last_mission_id = -1;
		for (auto child : mission_configs_) {
			if (child.second->type() == mission_type && child.second->group() == group) {
				if (child.first > last_mission_id) {
					last_mission_id = child.first;
				}
			}
		}
		return last_mission_id;
	}

	boost::shared_ptr<Config::MissionConfig> MtMissionManager::Type2Mission(Config::MissionType mission_type)
	{
		for (auto child : mission_configs_) {
			if (child.second->type() == mission_type) {
				return child.second;
			}
		}
		return nullptr;
	}

	MtMission* MtMissionManager::CreateMission(const uint64 player_guid, const Config::MissionConfig* config, const Config::MissionExConfig* ex_config)
	{
		if (config == nullptr || player_guid == INVALID_GUID)
			return nullptr;

		MtMission* mission = mission_pool_.create();
		if (mission == nullptr){
			ZXERO_ASSERT(false) << "new mission error! playerid :" << player_guid;
			return nullptr;
		}
		mission->reset();

		//mission->SetData(player_guid);
		mission->SetConfigData(config);
		mission->SetExConfigData(ex_config);

		Packet::Mission* db = mission->MissionDB();
		db->set_guid(config->id());
		db->set_player_guid(player_guid);
		db->set_state(Packet::MissionState::MissionTodo);

		for (int32 i = 0; i <= Packet::MissionDB_Param_MAX; i++) {
			db->add_param(-1);
		}

		return mission;
	}

	void MtMissionManager::RecycleMission(MtMission* ptr)
	{
		mission_pool_.recycle(ptr);
	}

	Config::StoryStageConfig* MtMissionManager::FindStoryStage(int32 id) const
	{
		auto it = story_stages_.find(id);
		if (it == story_stages_.end()) {
			return nullptr;
		} else {
			return it->second.get();
		}
	}

	//------------------------------------------------------
	void MtMission::lua_reg(lua_State* ls)
	{
		ff::fflua_register_t<MtMission, ctor( )>(ls, "MtMission")
			.def(&MtMission::Config, "Config")
			.def(&MtMission::MissionDB, "MissionDB")
			.def(&MtMission::ExConfig, "ExConfig");
	}

	void MtMission::reset()
	{
		for (int32 i=0; i<mission_data_.param_size(); i++){
			mission_data_.set_param(i, -1);
		}
		mission_data_.set_guid(INVALID_GUID);
		mission_data_.set_player_guid(INVALID_GUID);
		mission_data_.set_state(Packet::MissionState::MissionTodo);

		mission_config_ = nullptr;
		mission_ex_config_ = nullptr;
	}

	void MtMission::OnSave(const boost::shared_ptr<zxero::dbcontext>& dbctx)
	{
		ActiveModel::Base<Packet::Mission>(dbctx).Save(mission_data_);
	}
		
	void MtMission::FillMessage(Packet::MissionUpdateReply& message)
	{
		auto  mission = message.mutable_mission();
		mission->CopyFrom(mission_data_);
	}

}
