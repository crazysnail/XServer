#include "../mt_types.h"
#include <set>
#include <map>
using Packet::GuildCapturePointDB;
using Packet::GuildCaptureBattleLogDB;
using Packet::CapturePointDB;

namespace Mt
{
	class MtCapturePointManager : public boost::noncopyable
	{
	public:
		static MtCapturePointManager& Instance();
		static void lua_reg(lua_State* ls);
		MtCapturePointManager();
		int32 OnLoad();
		bool OnCacheAll();
		bool OnLoadAll(std::vector<boost::shared_ptr<CapturePointDB>>& cps,
			std::vector<boost::shared_ptr<GuildCapturePointDB>>& guild_cps,
			std::vector<boost::shared_ptr<GuildCaptureBattleLogDB>>& logs);
		/**
		* \brief 根据chapter_id找到对应的领土数据
		* \param chapter_id
		* \return GuildCapturePointDB*
		*/
		CapturePointDB* FindCapturePoint(int32 chapter_id) const;
		/**
		* \brief 目标公会的占点配置
		* \param guild_guid
		* \return GuildCapturePointDB*
		*/
		GuildCapturePointDB* FindGuildCapturePoint(uint64 guild_guid) const;
		/**
		* \brief 所有章节id
		*/
		std::vector<int32> AllChapterId() const;

		/**
		* \brief 更新该公会的占点数据, 校验在lua层做
		* \param guild_guid
		* \param data
		* \return void
		*/
		void UpdateStrongHold(uint64 guild_guid, const Packet::CG_UpdateStrongHold* data);
		/**
		* \brief 公会已占领或者正在争夺中的cp
		* \return CapturePointDB*
		*/
		CapturePointDB* RelateCapturePoint(uint64 guild_guid);
		void OnCaptureBattleEnd(uint64 attacker_guid,
			uint64 defender_guid,
			uint64 log_guid,
			bool defeat,
			int32 battle_index,
			uint64 reply_guid);
		bool OnHookStageDistributeUpdate(const boost::shared_ptr<MtPlayer>& /*player*/,
			const boost::shared_ptr<S2G::HookStageDistributeUpdate>& message,
			int32 /*source*/);
		int32 GetHookUserCount(int32 chapter_id) const;
		/**
		* \brief 加载完对应的herodata后, 通知capture_manager, 设置完成了
		* \param guild_guid
		* \param data
		* \return void
		*/
		void OnStrongHoldUpdateDone(uint64 guild_guid, Packet::CG_UpdateStrongHold& data);
		GuildCaptureBattleLogDB* FindBattleLog(uint64 guid);
		std::vector<uint64> FindGuildBattleLog(uint64 guild_guid);
		GuildCaptureBattleLogDB* CreateBattleLog();
		void OnTick();
		void OnBigTick();
		bool AddCapturedChapter(int32 scene_id, uint64 guid);
		bool RemoveCaptureChapter(int32 scene_id, uint64 guid);
		int32 CapturePointBossGroupId(int32 chapter_id, int32 index) const;
		int32 CapturePointGuardBossId(int32 chapter_id, int32 index) const;
		void LoadPlayerAndActor(uint64 attacker_guid, uint64 defender_guid,
			int32 chapter_id, std::set<uint64>& player_guids);
		void OnLoadPlayerAndActorDone(uint64 attacker_guid,
			uint64 defender_guid, int32 chapter_id, bool load_success);
	private:
		std::map<uint64, boost::shared_ptr<GuildCapturePointDB>> guild_db_;
		std::map<int32, boost::shared_ptr<CapturePointDB>> capture_points_;
		std::map<uint64, boost::shared_ptr<GuildCaptureBattleLogDB>> battle_logs_;
		std::map<int32, uint64> captured_chapters_;
		std::map<uint32, std::vector<int32>> capture_point_monster_groups_; //<chapter_id, <monster_group_ids>>公会占点怪物组配置
		std::map<uint32, std::vector<int32>> capture_point_bosses_; //<chapter_id, <monster_ids>>公会占点boss_id配置
		std::map<int32, S2G::HookStageDistributeUpdate> hook_distributes_; //<stage_id,hook_distribute>
		std::map<int32, int32> hook_user_count_; //<group_id,user_count>
		int32 script_id_ = 1025; //公会占点战斗脚本
	};
}
