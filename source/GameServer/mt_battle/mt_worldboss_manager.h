#ifndef WORLDBOSS_MANAGER_H__
#define WORLDBOSS_MANAGER_H__

#include "../mt_types.h"
#include "../ranklist/guilduserdamage_ranklist.h"

namespace Mt
{
	struct Statistics;
	class MtWorldBossManager : public boost::noncopyable
	{
	public:
		static void lua_reg(lua_State* ls);
	public:
		static MtWorldBossManager& Instance();
		int32	OnLoad();
		bool OnLoadAll(boost::shared_ptr<zxero::dbcontext>& dbctx);
		void OnSaveAll(boost::shared_ptr<zxero::dbcontext>& dbctx);
		void OnCacheAll( );
		MtWorldBossManager();
		~MtWorldBossManager();
	public:
		void OnCreateBoss(int32 seriesid,int32 sceneid);
		void OnBattleEndDamageBoss(boost::shared_ptr<MtPlayer> player_, int32 damage);
		void OnEndBoss(bool killboss);
		const boost::shared_ptr<Packet::WorldBossInfo> GetCurWorldBossInfo();
		bool WorldBossDead();
		void OnWorldBossStatisticsReq(boost::shared_ptr<MtPlayer> player_);
		int32 BossSeriesid()
		{
			return boss_seriesid_;
		}
		uint64 BossAllHp()
		{
			return allhp_;
		}
		bool IsWorldBossScene(int sceneid);
	public:
		const boost::shared_ptr<Packet::WorldBossConfig> GetCurWorldBossConfig();
	private:
		boost::shared_ptr<Packet::WorldBossInfo> GetWorldBossInfo(int32 bossid);
	private:
		std::vector<boost::shared_ptr<Packet::WorldBossInfo>> bosslist_;
		std::vector<boost::shared_ptr<Packet::WorldBossConfig>> bossconfiglist_;
		ranklist_top<worldbossuserdamage, -1> ranklist_;
		uint64 killerguild = INVALID_GUID;
		uint64 allhp_ = 0;
		int32 boss_seriesid_ = -1;
	};
}

#endif
