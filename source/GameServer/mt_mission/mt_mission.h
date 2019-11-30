#ifndef MTONLINE_GAMESERVER_MT_MISSION_H__
#define MTONLINE_GAMESERVER_MT_MISSION_H__
#include "../mt_types.h"
#include <vector>
#include "object_pool.h"
#include "MissionConfig.pb.h"
#include "MissionPacket.pb.h"


namespace Mt
{
	class MtMissionManager : public boost::noncopyable
	{
	public:
		static void lua_reg(lua_State* ls);
	public:
		static MtMissionManager& Instance();
		MtMissionManager();
		~MtMissionManager();
		int32 OnLoad();

		bool LoadPlayerMission( const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<zxero::dbcontext>& dbctx);
		boost::shared_ptr<Config::MissionConfig> FindMission(int32 config_id);
		boost::shared_ptr<Config::MissionExConfig> FindMissionEx(int32 index);
		boost::shared_ptr<Config::TargetConfig> FindTarget(int32 index);
		std::vector<boost::shared_ptr<Config::TargetConfig>> GetAllTarget(const int32 camp,const int32 level);
		std::vector<int32> FindMissionAddOn(int32 config_id);
		std::vector<int32> GetLevelMissions(const int32 lastid, const int32 level,const int32 camp);
		int32 GetMissionIndex(int32 mission_id);
		Config::StoryStageConfig* FindStoryStage(int32 id) const;
		int32 GenCommonRewardMission(int32 circle);
		int32 GenNextExtraRewardMission(int32 mission_id);
		int32 GenCarbonTinyThreeMission(int32 group, int32 mission_id);
		std::vector<int32> GetCarbonTinyThreeMission(int32 group);
		int32 GenCarbonBigThreeMission(int32 group, int32 mission_id);
		std::vector<int32> GetCarbonBigThreeMission(int32 group);
		int32 GenArchaeologyMission();
		int32 GenCorpsMission();
		int32 GenGuildMission(int32 group);
		int32 GetLastMissionId(Config::MissionType mission_type,int32 group );
		boost::shared_ptr<Config::MissionConfig> Type2Mission(Config::MissionType mission_type);

		void RecycleMission(MtMission* ptr);
		MtMission* CreateMission(const uint64 player_guid, const Config::MissionConfig* config,const Config::MissionExConfig* ex_config);

	private:
		std::map<zxero::int32, boost::shared_ptr<Config::MissionConfig>> mission_configs_;		
		std::map<zxero::int32, boost::shared_ptr<Config::TargetConfig>> target_configs_;
		std::map<zxero::int32, boost::shared_ptr<Config::MissionExConfig>> mission_configs_ex_;
		std::map<zxero::int32, std::vector<int32> > mission_add_on_;
		std::map<int32, boost::shared_ptr<Config::StoryStageConfig>>  story_stages_;

		//加速用的赏金任务表
		std::map<int32, std::map<int32, int32>> common_reward_mission_; //<环数，<任务号，权重值>>
		std::map<int32, int32>	common_reward_weight_;//<环数,总权重值>
		//额外赏金任务
		std::map<int32, std::map<int32, int32>> extra_reward_mission_;//<任务线，<环数，任务号>>
		std::map<int32, int32>	extra_reward_weight_;//<任务线,权重值>	
		int32 extra_reward_all_weight_; //附加任务总权重

		//加速用考古任务
		std::vector<int32>	archaeology_mission_;//<id>

		//加速用暮光军团任务
		std::vector<int32>	corps_mission_;//<id>

		//小三环副本起始任务表
		std::map<int32, std::vector<int32>>	tinythree_carbon_mission_;//<group,<任务号>>
		//大三环副本起始任务表
		std::map<int32, std::vector<int32>>	bigthree_carbon_mission_;//<group,<任务号>>

		//帮会起始任务表
		std::map<int32, std::vector<int32>>	guild_carbon_mission_;//<group,<任务号>>
		//等级任务
		std::map<int32, boost::shared_ptr<Config::MissionConfig>>	level_mission_;//<任务号，等级>

		object_pool_ex<MtMission> mission_pool_{10000};
	};

	class MtMission : public boost::noncopyable
	{
	public:
		static void lua_reg(lua_State* ls);
	public:

		MtMission() {
		};
		
		Packet::Mission* MissionDB() { 
			return &mission_data_; 
		}

		const Config::MissionConfig* Config() {
			return mission_config_;
		}
		const Config::MissionExConfig* ExConfig() {
			return mission_ex_config_;
		}

		void OnSave(const boost::shared_ptr<zxero::dbcontext>& dbctx);
		void FillMessage(Packet::MissionUpdateReply& message);
		void reset();

		//void SetData(const uint64 player_guid) {
		//	mission_data_.set_player_guid(player_guid);
		//}
		void SetConfigData(const Config::MissionConfig* mission_config) {
			mission_config_ = mission_config;
		}
		void SetExConfigData(const Config::MissionExConfig* mission_ex_config)	{
			mission_ex_config_ = mission_ex_config;
		}

	private:
		Packet::Mission mission_data_;
		const Config::MissionConfig* mission_config_;
		const Config::MissionExConfig* mission_ex_config_;

	};

}
#endif // MTONLINE_GAMESERVER_MT_MISSION_H__
