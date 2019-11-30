#ifndef MTGUILD_MANAGER_H__
#define MTGUILD_MANAGER_H__

#include "../mt_types.h"
#include "../ranklist/guilduserdamage_ranklist.h"

namespace Mt
{
#define MAX_GUILD_ID 999999999
	class GuildInfo {
	public:
		static void lua_reg(lua_State* ls);
	public:
		GuildInfo();
		~GuildInfo();
		bool OnSaveAll(boost::shared_ptr<zxero::dbcontext>& dbctx);
		bool OnCacheAll( );
	public:
		void OnSecondTick(int32 elapseTime);
		uint64 GetGuildId();
		void SetGuildName(std::string name, MtPlayer* player);
		std::string GetGuildName();
		boost::shared_ptr<Packet::GuildUser> GetGuildUser(uint64 playerguid);
		boost::shared_ptr<Packet::GuildUser> GetGuildUserByPosition(int32 position);
		int32 GetGuildBattleUserSize() const;
		int32 GetGuildUserSize() const;
		int32 GetActiveGuildUserSize();
		int32 GetGuildUserSizeLessLoginDay(int32 day);
		int32 GetGuildUserMaxSize();
		bool IsFullGuildUser();
		void DeleteUser(uint64 playerid);
		void InitGuildUser(MtPlayer* player_, boost::shared_ptr<Packet::GuildUser> user, Packet::GuildPosition position,  uint64 guildid);
		void UpdateGuildUser(MtPlayer* player_, boost::shared_ptr<Packet::GuildUser> user);
		void G2SSyncGuildUserData(MtPlayer* player_, Packet::GuildUser* guilduser);
		void SendGuildUserList(MtPlayer* player);
		void AddUserContribution(uint64 playerid,int32 add);
		Packet::ResultCode AddGuild(boost::shared_ptr<MtPlayer> player_);
		void AddGuildUser(boost::shared_ptr<Packet::GuildUser> guilduser);
		Packet::ResultCode ApplyGuild(boost::shared_ptr<MtPlayer> player_);
		int32 AddGuildMoney_S(int32 money);
		void AddGuildMoney(int32 money, MtPlayer* player);
		void AddGuildWeekBonus(int32 money);
		int32 GetGuildMoney();
		int32 GetGuildMaintain();
		boost::shared_ptr<Packet::GuildCDKInfo> GetGuildCDKInfoByPlayer(uint64 playerguid,int32 configid);
		boost::shared_ptr<Packet::GuildCDKInfo> GetGuildCDKInfoById(uint64 cdkguid);
		void DeleteGuildCDKInfoById(uint64 cdkguid);
		bool HaveGetCDKInfo(boost::shared_ptr<Packet::GuildCDKInfo> cdkinfo,uint64 playerguid);
		void AddCDKCount(boost::shared_ptr<MtPlayer> player_, boost::shared_ptr<Packet::GuildCDKInfo> cdk,int32 count);
		uint32 GetBuildLevelCDTime(int32 buildtype);
		int32 GetBuildLevel(int32 buildtype) const;
		void SettBuildLevel(int32 buildtype,int32 buildlevel,uint32 cdtime);
		void SendApplyList(MtPlayer* player_);
		void SendCDKList(boost::shared_ptr<MtPlayer> player_);
		void OnGuildWetCopyDamage(int32 copyid, guilduserdamage &userdamage);
		int32 GetWetCopyTimes(uint64 playerid , int32 copyid);
		void AddWetCopytimes(MtPlayer* player_,  int32 copyid);
		boost::shared_ptr<Packet::GuildWetCopyInfo> GetGuildWetCopyInfo(int32 copyid);
		void SetGuildWetCDTime(int32 copyid,int32 cdtime);
		boost::shared_ptr<Packet::GuildWarehouse> GetWarehouse(int32 index);
		void AddWarehouseRecord(int32 itemid,std::string playername);
		void AddWarehouse(int32 itemid);
		void DelWarehouse(int32 index);
		void AddSpoilItem(int32 itemid, int32 copyid,MtPlayer* player);
		boost::shared_ptr<Packet::GuildSpoilItem> GetSpoilItemByIndex(int32 index);
		void SendSpoilItem(MtPlayer* player_, int32 copyid);
		void SendUpdateSpoilItem(MtPlayer* player_, int32 index);
		void SpoilItemJumpPlayer(MtPlayer* player_,int32 index);
		std::vector<boost::shared_ptr<Packet::GuildSpoilItem>> GetSpoilItem();

		void AddRecord(std::string record);
		void SendRecord(MtPlayer* player_);

		boost::shared_ptr<Packet::GuildImpeach> GetGuildImpeach();
		void SendGuildImpeach(MtPlayer* player_);
		void ResetGuildImpeach();

		void OnWeekBonus();
		void OnGuilDBossReward(int32 rankfix,int32 monsterid);
		void OnHourTriger(int32 hour);
		void OnDayTriger(int32 day);
		void OnMainTain();
		void BroadcastUser(const google::protobuf::Message& msg);
		void BroadG2SCommonMessage(const std::string &name, const std::vector<int32>& int32values, const std::vector<int64>& int64values, const std::vector<std::string>& stringvalues);
		uint64 ChieftainGuid() const;
		Packet::GuildDBInfo* GetDBInfo() const;
	public:
		float GetBattleWinRate() const;
		float GetBattleRoundWinRate() const;
		bool CanenterBattle();
	public:
		boost::shared_ptr<Packet::GuildDBInfo> guilddb;
		std::vector<boost::shared_ptr<Packet::GuildUser>> guilduserlist;
		std::vector<Packet::ApplyGuildPlayerInfo> applyplayerlist;
		uint64 newcdkguid;
		std::vector<boost::shared_ptr<Packet::GuildCDKInfo>> cdklist;
		ranklist_top<guildbossuserdamage, -1> guildranklist_;
		std::map<int32, ranklist_top<guilduserdamage, 10>> guildwetcopyranklist_;
		std::vector<boost::shared_ptr<Packet::GuildWetCopyInfo>> wetcopyinfo_;
		std::map<int32,int32> wetcopycdtime_;
		int32 warehouseindex_ = 0;
		std::vector<boost::shared_ptr<Packet::GuildWarehouse>> warehouse_;
		std::vector<boost::shared_ptr<Packet::GuildWarehouseRecord>> warehouse_record_;
		int32 guildspoilindex_ = 0;
		std::vector<boost::shared_ptr<Packet::GuildSpoilItem>> guildspoil_item_;
		std::vector<boost::shared_ptr<Packet::GuildRecord>> guildrecord_;

		boost::shared_ptr<Packet::GuildImpeach> impeadch_;
		bool tickdelete = false;
		int32 bossalldamage = 0;
	};

	class GuildBattle
	{
	public:
		static void lua_reg(lua_State* ls);
	public:
		GuildBattle();
		~GuildBattle() {};
		void OnCacheAll();
		void OnDeletedAll();
		boost::shared_ptr<Packet::GuildBattleInfo> GetBattleInfo();
		boost::shared_ptr<Packet::GuildBattlePlayerInfo> GetPlayerinfo(uint64 playerid);
		int32 GetPlayerCountByid(uint64 guildid);
		int32 CreateGuildBattleRaid(int32 sceneid, int32 script_id, int32 npcid, int32 oldsceneid);
		int32 GetGuildBattleRaidID(int32 sceneid);
		int32 GetAttackerScore();
		int32 GetDefenderScore();
		int32 GetAttackerCount();
		int32 GetDefenderCount();
		uint64 GetAttackerGuid();
		uint64 GetDefenderGuid();
		uint64 GetWinGuild();
		void  OnWin(int32 camp);
		const boost::shared_ptr<MtRaid> GetGuildBattleRaid();
		const boost::shared_ptr<Packet::GuildBattlePlayerInfo> GetGuildBattlePlayer(uint64 playerid);
		bool EnterPlayer(uint64 playerid,std::string playername, int32 mobility, uint64 guildid,int32 initid);
		void LeavePlayer(uint64 playerid);
		void PlayerOnGuildWarEnd(MtPlayer* player, bool win);
		void AddLeavePlayerExp(MtPlayer* player,bool win);
		void UpdatePlayerList();
		void UpdatePlayerByGuid(uint64 playerid);
		void SendGuildBattleInfo(MtPlayer* player);
		void BroadCastMessage(const google::protobuf::Message& msg);
		void UpdateBattleStage(Packet::GuildBattle_Stage stage);
		
		void OnSecondTick();

		void SortPlayerRank();

	private:
		boost::shared_ptr<Packet::GuildBattleInfo> dbinfo;
		std::vector<boost::shared_ptr<Packet::GuildBattlePlayerInfo>> playerlist;
		boost::shared_ptr<MtRaid> raid_;
	};
	class MtGuildManager : public boost::noncopyable
	{
	public:
		static void lua_reg(lua_State* ls);
	public:
		static MtGuildManager& Instance();
		int32	OnLoad();
		MtGuildManager();
		~MtGuildManager();
		
		void OnSaveAll(boost::shared_ptr<zxero::dbcontext>& dbctx);
		void OnCacheAll( );
		void Cache2Save(google::protobuf::Message * msg,std::string MessageName);
		bool OnLoadAll(boost::shared_ptr<zxero::dbcontext>& dbctx);

		void OnSecondTick(uint32 elapseTime);
		void OnWeekTriger(int32 cur_day);
		void OnHourTriger(int32 hour);
		void OnDayTriger(int32 day);
		void OnMinTriger(int32 min);
		void OnSecondTriger(int32 second);
		void SetLastDate(uint32 last_date) { last_date_ = last_date; }
		uint32 GetLastDate() { return last_date_; }
		void SetSerial(uint64 max_serial);
	private:
		uint64 GenerateGuid();
	public:
		bool OnGuildChatMessage(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<S2G::GuildChat>& msg, int32 /*source*/);
		bool OnS2GEndGuildBoss(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<S2G::EndGuildBoss>& msg, int32 /*source*/);

		bool OnCreateGuildReq(const boost::shared_ptr<client_session>& session, const boost::shared_ptr<Packet::CreateGuildReq>& req, int32 /*source*/);
		void CreateGuild(MtPlayer* player_, std::string createname,std::string createnotice);
		bool OnFindGuildReq(const boost::shared_ptr<client_session>& session, const boost::shared_ptr<Packet::FindGuildReq>& req, int32 /*source*/);
		bool OnApplyGuildReq(const boost::shared_ptr<client_session>& session, const boost::shared_ptr<Packet::ApplyGuildReq>& message, int32 /*source*/);
		bool OnOneKeyApplyGuildReq(const boost::shared_ptr<client_session>& session, const boost::shared_ptr<Packet::OneKeyApplyGuildReq>& message, int32 /*source*/);
		bool OnAgreeApplyGuildReq(const boost::shared_ptr<client_session>& session, const boost::shared_ptr<Packet::AgreeApplyGuildReq>& message, int32 /*source*/);
		bool OnApplyGuildPlayerListReq(const boost::shared_ptr<client_session>& session, const boost::shared_ptr<Packet::ApplyGuildPlayerListReq>& message, int32 /*source*/);
		bool OnClearApplyGuildPlayerListReq(const boost::shared_ptr<client_session>& session, const boost::shared_ptr<Packet::ClearApplyGuildPlayerListReq>& message, int32 /*source*/);
		bool OnInviteyAddGuildReq(const boost::shared_ptr<client_session>& session, const boost::shared_ptr<Packet::InviteyAddGuildReq>& message, int32 /*source*/);
		bool OnInviteyAgreeGuildReq(const boost::shared_ptr<client_session>& session, const boost::shared_ptr<Packet::InviteyAgreeGuildReq>& message, int32 /*source*/);
		bool OnSetGuildNameRR(const boost::shared_ptr<client_session>& session, const boost::shared_ptr<Packet::SetGuildNameRR>& message, int32 /*source*/);
		bool OnSetGuildNoticeRR(const boost::shared_ptr<client_session>& session, const boost::shared_ptr<Packet::SetGuildNoticeRR>& message, int32 /*source*/);
		bool OnSetGuildIconRR(const boost::shared_ptr<client_session>& session, const boost::shared_ptr<Packet::SetGuildIconRR>& message, int32 /*source*/);
		
		void SendGuildUserPosition(MtPlayer* player_, uint64 targetguid, int32 position);
		//领取红利
		bool OnGetGuildBonusReq(const boost::shared_ptr<client_session>& session, const boost::shared_ptr<Packet::GetGuildBonusReq>& message, int32 /*source*/);
		bool OnCreateGuildCDKReq(const boost::shared_ptr<client_session>& session, const boost::shared_ptr<Packet::CreateGuildCDKReq>& message, int32 /*source*/);
		bool OnGetGuildCDKReq(const boost::shared_ptr<client_session>& session, const boost::shared_ptr<Packet::GetGuildCDKReq>& message, int32 /*source*/);
		bool OnGetGuildCDKListReq(const boost::shared_ptr<client_session>& session, const boost::shared_ptr<Packet::GetGuildCDKListReq>& message, int32 /*source*/);
		
		bool OnAddGuildMoney(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<S2G::AddGuildMoney>& msg, int32 /*source*/);

		bool OnBuildLevelUpReq(const boost::shared_ptr<client_session>& session, const boost::shared_ptr<Packet::BuildLevelUpReq>& message, int32 /*source*/);
		bool OnPlayerPracticeReq(const boost::shared_ptr<client_session>& session, const boost::shared_ptr<Packet::PlayerPracticeReq>& message, int32 /*source*/);
		bool OnGetPracticeReq(const boost::shared_ptr<client_session>& session, const boost::shared_ptr<Packet::GetPracticeReq>& message, int32 /*source*/);
		bool OnGetGuildWetCopyInfosReq(const boost::shared_ptr<client_session>& session, const boost::shared_ptr<Packet::GetGuildWetCopyInfosReq>& message, int32 /*source*/);
		bool SetGuildWetCopyStageMonster(Packet::GuildWetCopyInfo* wetcopy,Config::GuildWetCopyStageConfig* stage);
		bool InitGuildWetCopyMonster(boost::shared_ptr<Packet::GuildWetCopyInfo> wetcopy);
		bool OnOpenGuildWetCopyReq(const boost::shared_ptr<client_session>& session, const boost::shared_ptr<Packet::OpenGuildWetCopy>& message, int32 /*source*/);
		bool OnResetGuildWetCopyReq(const boost::shared_ptr<client_session>& session, const boost::shared_ptr<Packet::ResetGuildWetCopy>& message, int32 /*source*/);
		void SendGuildWetCopy(MtPlayer* player, Packet::GuildWetCopyInfo* wetcopy);
		bool OnGetGuildInfoReq(const boost::shared_ptr<client_session>& session, const boost::shared_ptr<Packet::GetGuildInfoReq>& message, int32 /*source*/);
		bool OnGetGuildListReq(const boost::shared_ptr<client_session>& session, const boost::shared_ptr<Packet::GetGuildListReq>& message, int32 /*source*/);
		bool OnGetGuildUserListInfoReq(const boost::shared_ptr<client_session>& session, const boost::shared_ptr<Packet::GetGuildUserListInfoReq>& message, int32 /*source*/);
		void OnGetGuildList(boost::shared_ptr<MtPlayer> player_);
		void OnGetGuildInfo(MtPlayer* player_);
		bool OnGetGuildBossCompetitionReq(const boost::shared_ptr<client_session>& session, const boost::shared_ptr<Packet::GetGuildBossCompetitionReq>& message, int32 /*source*/);
		void FillGuildUserDamage(guilduserdamage& damage, MtPlayer* player_, GuildInfo* guild,int32 copyid, int32 damagevalue, Packet::ActorFigthFormation formation);
		bool OnGetGuildUserDamageReq(const boost::shared_ptr<client_session>& session, const boost::shared_ptr<Packet::GetGuildUserDamageReq>& message, int32 /*source*/);
		bool OnGetServerUserDamageReq(const boost::shared_ptr<client_session>& session, const boost::shared_ptr<Packet::GetServerUserDamageReq>& message, int32 /*source*/);
		void OnGetGuildCopyDamage(MtPlayer* player_,int32 copyid);
		void OnGetGuildCopyServerDamage(MtPlayer* player_);
		void OnGetGuildBossUserDamage(MtPlayer* player_);
		void OnGetGuildWarehouseInfos(boost::shared_ptr<MtPlayer> player_);
		bool OnGuildWarehouseReceiveReq(const boost::shared_ptr<client_session>& session, const boost::shared_ptr<Packet::GuildWarehouseReceiveReq>& message, int32 /*source*/);
		bool OnGuildWarehouseSetReceivePlayerReq(const boost::shared_ptr<client_session>& session, const boost::shared_ptr<Packet::GuildWarehouseSetReceivePlayerReq>& message, int32 /*source*/);
		bool OnGuildBattleFightTarget(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<Packet::GuildBattleFightTarget>& msg, int32 /*source*/);
		void GenGuildBattleList();
		void InsertGuildBattleList(boost::shared_ptr<GuildInfo> a, boost::shared_ptr<GuildInfo> b);
		void GuildBattleRoundReward();
		void ClearGuildBattleRoundScore();
		void ClearGuildBattleScore();
		bool GetGuildBattleList(
			const boost::shared_ptr<client_session>& session,
			const boost::shared_ptr<Packet::GuildBattleListReq>& message, 
			int32 /*source*/);
		bool SendGuildBattleWeekRankList(
			const boost::shared_ptr<client_session>& session,
			const boost::shared_ptr<Packet::GuildBattleWeekRankReq>& message,
			int32 /*source*/);
		bool SendGuildBattleAllRankList(
			const boost::shared_ptr<client_session>& session,
			const boost::shared_ptr<Packet::GuildBattleAllRankReq>& message,
			int32 /*source*/);
		bool OnGetGuildWarehouseInfosReq(
			const boost::shared_ptr<client_session>& session,
			const boost::shared_ptr<Packet::GuildWarehouseReq>& message,
			int32 /*source*/);
		bool OnGetGuildWarehouseRecordReq(
			const boost::shared_ptr<client_session>& session,
			const boost::shared_ptr<Packet::GuildWarehouseRecordReq>& message,
			int32 /*source*/);

		bool SetGuildBattleWeekRank(uint64 guid, std::string guildname, int32 level, int32 score);
		bool SetGuildBattleAllRank(uint64 guid, std::string guildname, int32 level,
			int32 battlewintimes, int32  battletimes);
		boost::shared_ptr<GuildBattle> GetGuildBattle(uint64 guild);
		int32 GetGuildBattleStage() {
			return battle_stage_;
		}
		void SetGuildBattleStage(int32 stage);
	public:
		//只能主线程使用
		boost::shared_ptr<Packet::GuildUser> GetGuildUser(uint64 guildid,uint64 playerid);
		int32 GetGuildMemberSize(const boost::shared_ptr<GuildInfo> guild_);
		void AddLeaveUser(boost::shared_ptr<Packet::GuildUser> user);
		boost::shared_ptr<Packet::GuildUser> GetLeaveUser(uint64 playerid);
		void DelLeaveUser(uint64 playerid);
		boost::shared_ptr<GuildInfo> GetGuildByid(uint64 guid);
		boost::shared_ptr<GuildInfo> GetGuildByName(std::string name);
		int32 GetPlayerGuildPosition(uint64 playerguid, GuildInfo* guild);

		void BroadcastGuild(uint64 guildid, const google::protobuf::Message& msg);
	public:
		boost::shared_ptr<Config::GuildBuildLevelConfig> GetBuildLevelConfig(int32 buildtype, int32 buildlevel);
		boost::shared_ptr<Config::GuildLevelFixConfig> GetGuildLevelFixConfig(int32 level);
		boost::shared_ptr<Config::GuildPracticeLevelConfig> GetPracticeLevelConfig(int32 level);
		boost::shared_ptr<Config::GuildPracticeConfig> GetPracticeConfig(int32 id);
		boost::shared_ptr<Config::GuildCDKConfig> GetGuildCDKConfig(int32 id);
		boost::shared_ptr<Config::GuildWetCopyConfig> GetGuildWetCopyConfig(int32 id);
		boost::shared_ptr<Config::GuildPositionConfig> GetGuildGuildPositionConfig(int32 id);
		boost::shared_ptr<Config::GuildDonateConfig> GetGuildDonateConfig(int32 type);
		const boost::shared_ptr<Config::GuildWetCopyStageConfig> GetGuildWetCopyFristStage(int32 copyid);
		const boost::shared_ptr<Config::GuildWetCopyStageConfig> GetGuildWetCopyStage(int32 stageid);
		bool InGuildWetCopyScene(int32 sceneid);
		bool NeedPracticeLevelUp(int32 level,int32 exp);
		bool AddPracticeLevel(boost::shared_ptr<Packet::PracticeInfo> practice);
	public:
		void EndGuildBoss(uint64 guildid, uint64 playerid,int32 damage);
		void OnStartGuildBoss();
		void OnEndGuildBoss();
		int32 GetGuildBossStatus();

		void EndGuildWetCopy(MtPlayer* player_, GuildInfo* guild,int32 copyid, int32 damage);
		void testplayers(boost::shared_ptr<MtPlayer> player_);
		void testplayer(MtPlayer* player_);
	private:
		void OnWeekBonus();
		void OnMainTain();

	private:
		std::vector<boost::shared_ptr<GuildInfo>> guildlist_;	//所有的帮会管理
		std::vector<boost::shared_ptr<Packet::GuildUser>> leaveuserlist;	//所有曾经有过帮会但是现在没有帮会的人
		std::vector<boost::shared_ptr<Config::GuildBuildLevelConfig>> guildbuild_;
		std::vector<boost::shared_ptr<Config::GuildLevelFixConfig>> guildlevelfix_;
		std::map<int32, boost::shared_ptr<Config::GuildPracticeConfig>> guildpractice_;
		std::map<int32, boost::shared_ptr<Config::GuildPracticeLevelConfig>> guildpracticelevel_;
		std::map<int32, boost::shared_ptr<Config::GuildCDKConfig>> guildcdk_;
		std::map<int32, boost::shared_ptr<Config::GuildWetCopyConfig>> guildwetcopy_;
		std::vector<boost::shared_ptr<Config::GuildWetCopyStageConfig>> guildwetcopystage_;
		std::vector<boost::shared_ptr<Config::GuildPositionConfig>> guildposition_;
		std::vector<boost::shared_ptr<Config::GuildDonateConfig>> guilddonate_;
		int32 bossstatus_ = 0;
		ranklist_top<guildalldamage, -1> serverranklist_;
		ranklist_top<guilduserdamage, 50> serverwetcopyranklist_;
		boost::shared_mutex  guildbattle_shared_mutex_;
		std::vector<boost::shared_ptr<GuildBattle>> guildbattlepairlist_;
		Packet::GuildBattle_Stage battle_stage_ = Packet::GuildBattle_Stage::GuildBattle_Stage_None;
		uint32 last_date_;
		uint64 guid_serial_ = INVALID_GUID;
	public:
		ranklist_top<guildbattleallrankinfo, 50> gballranklist_;
		ranklist_top<guildbattleweekrankinfo, 50> gbweekranklist_;
		int32 WetCopySpoilApplySize = 0;
		int32 WetCopyMonsterSzie = 0;
	};
}

#endif