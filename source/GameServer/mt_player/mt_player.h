#ifndef ZXERO_GAMESERVER_MT_PLAYER_H__
#define ZXERO_GAMESERVER_MT_PLAYER_H__
#include "../mt_types.h"
#include "ItemAndEquip.pb.h"
#include <BattleField.pb.h>
#include <ActorBasicInfo.pb.h>
#include "../mt_battle/mt_battle_object.h"
#include "../mt_arena/mt_arena.h"
#include "FriendMail.pb.h"
#include "ChatMessage.pb.h"
#include "../mt_arena/geometry.h"
#include "../mt_skill/mt_simple_impact.h"
#include <BattleReply.pb.h>
#include "object_pool.h"

namespace Mt
{
	class MtNpc;
	class MtRaid;
	class GuildInfo;
	class MtSimpleImpactSet;
	class MtPlayerStageProxy;
	enum eLogoutStatus
	{
		eInitStatus = -1,
		eOffLine = 0,
		eCanLogout =1,
		eOnLine =2
	};

	using namespace zxero;
	struct MtRobot
	{
		MtRobot() = delete;
		MtRobot(const Packet::PlayerBasicInfo& basic_info);
		uint64 Guid() const { return basic_info_.guid(); }
		Packet::PlayerBasicInfo basic_info_;
		Packet::ActorFullInfo actor_full_;
	};
	class MtPlayer : public boost::noncopyable, public boost::enable_shared_from_this<MtPlayer>
	{
	public:
		static void lua_reg(lua_State* ls);
		MtPlayer();
		MtPlayer(const boost::shared_ptr<Packet::PlayerDBInfo>& player_data, const boost::shared_ptr<Packet::AccountModel>& account);
		//methods definein scene object start
		zxero::uint64 Guid() const;
		/*
				virtual void FillSceneObjectMessage(Packet::SceneObject& message) const override{

				}*/
		bool Online() const {
			return session_ != nullptr;
		}
		virtual ~MtPlayer();;
		virtual void OnTick(uint64 elapseTime);
		virtual void OnBigTick(uint64 elapseTime);
		virtual void OnCache2Shm( );
		void OnRobotPlayerInOk(const boost::shared_ptr<Packet::RobotPlayerInOk>& msg);
		bool CreateRobotTeam(int32 group);
		bool ResetRobotPos();
		bool DismissTeam();
		bool OnRobotMove(const boost::shared_ptr<Packet::ClientPlayerMove>& message);
		void FillRobotPlayer(Packet::ScenePlayerList& list);
		int32 CurrentRobotCount() const;
		bool BroadCastMessage(const google::protobuf::Message& msg);
		bool BroadCastSceneMessage(const google::protobuf::Message& msg);
		bool SendMessage(const google::protobuf::Message& message);
		void SendLogMessage(const std::string& msg);
		bool LuaSendMessage(const google::protobuf::Message* message);
		bool HandleMessage();
		bool DispatchMessage(const boost::shared_ptr<google::protobuf::Message>& msg);
		bool LuaDispatchMessage(const google::protobuf::Message* msg);
		bool ExecuteMessage(const boost::shared_ptr<google::protobuf::Message>& msg);
		bool LuaExecuteMessage(const google::protobuf::Message* msg);
		Packet::PlayerBasicInfo* GetScenePlayerInfo() const;
		const boost::shared_ptr<Packet::PlayerDBInfo>& GetDBPlayerInfo() const;
		void FillScenePlayerInfo(Packet::ScenePlayerInfo& msg);
		//只能场景线程调用
		void SendScenePlayerInfoToServer();
		float Direction();
		google::protobuf::RepeatedPtrField<Packet::RTHC>* GetRealTimeHookData();
		void Direction(float dir);
		uint32 GetMoveSpeed();
		void SetMoveSpeed(uint32 move_speed);
		void ResetSpeed();
		void UpdateSimpleImpact();
		int32 BattleScore() const;
		int32 PowerCount();
		int32 MaxBattleScore() { return PowerCount(); }
		void UpdateBattleScore(bool init=false);
		zxero::int32 GetSceneId();
		void SetSceneId(int32 scene_id);
		const Packet::Position& Position();
		bool AddActor(const boost::shared_ptr<MtActor>& actor,bool is_init=false);
		bool LoadActor(const boost::shared_ptr<MtActor>& actor);
		void Position(const Packet::Position& pos);

		void OnLeaveDaysTriger(int32 day);
		void OnMonthTriger(int32 month);
		void OnWeekTriger(int32 week);
		void OnDayTriger(int32 day, bool Interday);
		void OnHourTriger(int32 hour);
		void OnMinTriger(int32 min);

		const boost::shared_ptr<MtScene>& Scene() {	return scene_; }
		const boost::shared_ptr<MtZone>& Zone();
		void Zone(const boost::shared_ptr<MtZone>& zone);
		void SetScene(const boost::shared_ptr<MtScene>& scene);
		void Session(const boost::shared_ptr<client_session>& session);
		const boost::shared_ptr<client_session> Session();
		void OnBattleEnd(const boost::shared_ptr<MtBattleGround>& battle_ground,bool force_end);
		bool OnUpdateFollowPath();
		void OnResetFollowPath();		

		//ReConnected和DisConnected对应于重新连线和临时断线，意味着数据不需要重新加载和卸载
		void ReConnected();
		void LoginTimeCheck();
		void DisConnected();
		bool IsDisConnected() const {
			return disconnect_;
		}
		//OnLogin和OnLogOut对应于正式登陆和正式退出，意味着数据的重新加载和完全卸载		
		void OnLogin();
		/**
		* \brief 切换场景完毕后同步数据
		* \return void
		*/
		void EnterSceneSync();
		void OnLogout();
		void SendLogoutLog();
		void OnLeaveScene();
		eLogoutStatus LogoutStatus() const {
			return logout_status_;
		}
		void LogoutStatus(eLogoutStatus status);

		uint32 CacheStamp() const {
			return cache_stamp_;
		}
		//同步数据到客户端
		void RefreshData2Clinet();
		//
		boost::shared_ptr<MtActor> MainActor() const;
		int32 PlayerLevel() const;
		void OnLevelUpTo(const int32 level);
		std::vector<boost::shared_ptr<MtActor>> OtherActors() const;
		const std::vector<boost::shared_ptr<MtActor>>& Actors() const;
		//单人阵容出战
		const std::vector<boost::shared_ptr<MtActor>> BattleActor(Packet::ActorFigthFormation form);
		//队伍阵容混合出战
		const std::vector<boost::shared_ptr<MtActor>> TeamFormBattleActor(Packet::ActorFigthFormation form) ;
		//队伍主角出战
		const std::vector<boost::shared_ptr<MtActor>> TeamMainBattleActor() ;
		Packet::BattleGroundType GetCurBattleType();
		Packet::BattleGroundStatus GetCurBattleStatus();
		boost::shared_ptr<MtBattleGround> BattleGround() { return battle_ground_; }
		std::vector<int32> AllInsetGems();
		std::map<int32, int32> EnchantSlotEquips(bool effect);
		void QuitBattle();
		void QuitHookBattle();
		//开始模拟挂机
		bool StartHookBattle(int32 stage_id, int32 group_id, int32 insight, int32 send_start_notify);
		MtBattleGround* HookBattle() {
			return hook_battle_.get();
		}
		bool Moving() const;
		void StopMove();
		bool EnterBattleGround(MtBattleGround* battle_ground);
		void EnterRaid(const boost::shared_ptr<MtRaid>& raid);
		void LeaveRaid();
		void GoingLeaveScene();
		void SetPlayerStageDbInfo(const boost::shared_ptr<Packet::PlayerStageDB>& data);
		Packet::PlayerStageDB* GetPlayerStageDB() const;
		void OnReSetHookData() const;
		const boost::shared_ptr<MtRaid> GetRaid() const;
		bool AddHero(zxero::int32 config_id, Packet::ActorType type,
			const std::string& source, bool init_equip = false,
			Packet::ActorStar star = Packet::ActorStar::ONE,
			Packet::ActorColor color = Packet::ActorColor::WHILE,
			int32 level = 1);

		boost::shared_ptr<MtActor> FindActorByCfgId(int32 config_id);
		boost::shared_ptr<MtActor> FindActorByGuid(uint64 guid);
		const boost::shared_ptr<MtContainer> FindContainer(const Packet::ContainerType type);
		void SetContainer(const Packet::ContainerType type,const boost::shared_ptr<MtContainer>& container);
		void OnCache();
		void OnServerCache();
		bool AddItem(const boost::shared_ptr<MtItemBase>& item, Config::ItemAddLogType type, const int64 source_id = 0);
		bool DelItems(const std::vector<boost::shared_ptr<MtItemBase>>& items, Config::ItemDelLogType type, const int64 source_id = 0);
		bool AddItemById(const int32 config_id, const int32 count, 
			Config::ItemAddLogType type,
			Packet::NotifyItemList *nlist=nullptr,
			const int64 source_id = 0,
			Packet::ActorFigthFormation formation = Packet::ActorFigthFormation::AFF_NORMAL_HOOK);

		bool AddItemByIdWithNotify(const int32 config_id, const int32 count,
			Config::ItemAddLogType type, const int64 source_id = 0,
			Packet::ActorFigthFormation formation = Packet::ActorFigthFormation::AFF_NORMAL_HOOK);

		bool AddItemByIdWithoutNotify(const int32 config_id, const int32 count,
			Config::ItemAddLogType type, const int64 source_id = 0,
			Packet::ActorFigthFormation formation = Packet::ActorFigthFormation::AFF_NORMAL_HOOK);

		bool DelItemById(const int32 config_id, const int32 count, Config::ItemDelLogType type, const int64 source_id = 0);
		int32 GetBagItemCount(const int32 config_id);

		bool MoveItem(const boost::shared_ptr<MtItemBase>& item, const Packet::ContainerType desType);
		bool BagLeftCapacity(const Packet::ContainerType layer, const int32 check_count);
		bool EquipAndItemBagLeftCheck(const std::map<int32,int32>& items);
		const boost::shared_ptr<MtItemBase> FindItemByGuid(const Packet::ContainerType layer,zxero::uint64 guid);
		const boost::shared_ptr<MtItemBase> FindItemByConfigId(const Packet::ContainerType layer,int32 config_id);

		void fill_hyper_iteminfo(std::string message, Packet::MessageHyperItemInfo &item);
		const boost::shared_ptr<MtActor> FindActor(uint64 guid) const;
		bool ActorFull() const;
		void SetMovePath(boost::shared_ptr<Packet::ClientClickMove> path) {
			move_path_ = path;
		}
		bool ClickMoveProcessDone(uint64 elapseTime);
		Packet::Camp GetCamp() {
			return player_data_->camp();
		}
		void SetTeamArrangeType(Packet::ArrangeType type);
		Packet::ArrangeType GetTeamArrangeType();
		void CheckAcrDrop(int32 actype);
		//
		const boost::shared_ptr<MtMarket> FindMarket(Config::MarketType type) const;
		void InitMount();
		void InitMarket();
		void InitActivity();
		bool AddMarket(const boost::shared_ptr<MtMarket> &market);
		bool AddActivity(const boost::shared_ptr<MtActivity> &activity);
		boost::shared_ptr<MtActivity> GetActivity(const int32 type);
		Packet::ResultCode ActivityCheck(const int32 type);

		int32 GetPlayerToken(const Packet::TokenType token) const;
		//friend
		boost::shared_ptr<FriendContainer> Friend_Container() { return friend_; }
		void SetFriendContainer(const boost::shared_ptr<FriendContainer>& container){ friend_ = container;}
		void FillPlayerOperateInfo(Packet::PlayerOperateInfo &info);
		static void FillPlayerOperateInfoBySceneInfo(Packet::PlayerOperateInfo &info,uint64 playerid,const Packet::ScenePlayerInfo* spi, Packet::GuildPosition guildposition);
		void fill_friend_protocol(Packet::FriendInfotReply &reply);
		void fill_recommend_protocol(Packet::RecommendsUpdate &reply);
		void fill_friendplayerinfo_protocol(Packet::FriendInfo &info);
		void send_friendlistupdate();
		void NewMessageNotify();
		Packet::ResultCode add_friend(uint64 playerguid,const Packet::PlayerBasicInfo* targetinfo);
		Packet::ResultCode del_friend(uint64 playerguid);
		
		void OnFriendMessageReq(Packet::FriendMessageType type, uint64 targetguid, std::string chatmessage,int32 chattime);
		void send_friendsystemmessage(std::string chatmessage);
		void set_zone_signature(std::string signature);
		void fill_ViewZoneInfoEquip(Packet::ViewZoneReply& msg);
		bool check_MessagePlayerInfo(boost::shared_ptr<Packet::MessagePlayerInfo> info);
		void update_MessagePlayerInfo(const Packet::PlayerBasicInfo* targetinfo);
		void OnSendGiftGiveReq(const boost::shared_ptr<MtPlayer>& sourceplayer,  const boost::shared_ptr<Packet::SendGiftReq>& message);
		//
		MtSimpleImpactSet* GetSimpleImpactSet() { return &simple_impacts_; }
	
		const boost::shared_ptr<MessageContainer> GetMessageContainer() { return message_; }
		void SetMessageContainer(const boost::shared_ptr<MessageContainer>& container)
		{
			message_ = container;
		}
		void add_message(boost::shared_ptr<Packet::MessageInfo> mail_, MAIL_OPT_TYPE opt_type);
		Packet::ResultCode del_message(uint64 messageid);
		void del_notify_playerinfo(uint64 guid);
		//
		bool FixActorFormations();
		void CopyActorFormation(const Packet::ActorFigthFormation source_type, const Packet::ActorFigthFormation target_type);
		boost::shared_ptr<Packet::ActorFormation> GetActorFormation(const Packet::ActorFigthFormation type);
		void AddActorFormation(boost::shared_ptr<Packet::ActorFormation>& formation);	
		int32 GetActorFormationPower(const Packet::ActorFigthFormation type);
		const std::vector<boost::shared_ptr<Packet::ActorFormation>>& ActorFormation() const;
		void UpdateFormationLastHpSet(int32 formation,int32 lasthp);
		bool IsEmptyFormation(Packet::ActorFigthFormation type);

		bool FirstStageReward(const int32 stage_id);
		void ResetActivityData();
		void ResetTrialFieldData();
		void ResetTrialGBOSSData();
		void ActivityListRefresh();
		void ActivityListModify();
		//试炼场
		void SetPlayerNpc(const boost::shared_ptr<DB::PlayerTrialInfo> trialinfo);
		void ResetPlayerNpcLastHp();
		void ResetPlayerActorLastHp(Packet::LastHpType type);
		void RefreshPlayerNpc(bool dirty);
		void DelPlayerNpc(const uint64 target_guid);
		void DelPlayerNpcByIndex(const int32 index);
		DB::TrialTarget* GetPlayerNpc(const uint64 target_guid);
		DB::TrialTarget* GetPlayerNpcByIndex(const int32 index);
		bool CheckRefreshNextWave();
		void SavePlayerLastHp(const uint64 actor_guid, Packet::LastHpType type,const int32 hp);
		void SavePlayerNpcLastHp(const uint64 target_guid, const uint64 actor_guid, const int32 hp);
		//
		void DecBossChallengeCount(int32 stage_id, int32 count);
		int32 GetBossChallengeCount(int32 stage_id);	
		//
		bool GoTo(int32 sceneid,std::vector<int32> pos);
		bool GoRaid(int32 sceneid,int32 script_id, int32 raidid,int32 camp,std::vector<int32> pos);
		void GoRaidByPosIndex(int32 sceneid, int32 script_id, int32 raidid, int32 camp, int32 posindex);
		bool CanGo();
		//
		int32 AddNpc(const int32 pos_id, const int32 index, const int32 lifetime);
		void AddOtherNpc(const int32 group, const int32 lifetime);
		bool AddSceneNpc(const int32 pos_id, const int32 index, const int32 lifetime);//跨线程添加场景npc接口
		void DelNpc(const int32 series_id);
		void LockNpc(const int32 series_id, const bool lock);
		void DelNpcByType(const std::string src_type);
		void DelNpcByGroup(const int32 group);
		std::vector<boost::shared_ptr<MtNpc>> GetSceneNpcs(const int32 sceneid, const std::string &tag);
		std::vector<boost::shared_ptr<MtNpc>> GetTagNpcs(const std::string &tag);

		//数据同步接口
		void Refresh2ClientNpcList();
		void Refresh2ClientMissionList();
		void Refresh2ClientStageInfo();
		void Refresh2ClientActivityList();		
		void Refresh2ClientLuaFiles(bool dirty);
		void Refresh2ClientOperateConfig(int32 actibity_id);
		void Refresh2ClientDataCellList();
		void Refresh2ClientActorList();		
		void Refresh2ClientImpactList();
		void Refresh2ClientBattleFeild();
		int OnLuaFunCall_0(const int32 script_id, const std::string &func_name);
		int OnLuaFunCall_1(const int32 script_id, const std::string &func_name, const int32 param1);
		int OnLuaFunCall_n(const int32 script_id, const std::string &func_name, const std::map<std::string, int32> params);
		int OnLuaFunCall_n_64(const int32 script_id, const std::string &func_name, const std::map<std::string, uint64> params);
		int OnLuaFunCall_x(const std::string &func_name, const std::map<std::string, int32> params);
		int OnLuaFunCall_c(const int32 script_id, const std::string &func_name, const std::vector<int32> int32param, const std::vector<uint64> int64param, const std::vector<std::string> stringparam);
		int OnLuaFunCall_OnMessageHandler(const std::string& funcname, google::protobuf::Message *message);
		//
		boost::shared_ptr<MtArrangeProxy>& GetArrangeProxy();
		//mission begin
		boost::shared_ptr<MtMissionProxy>& GetMissionProxy();
		MtMission* GetMissionById(int32 missionid);
		//missionend
		MtPlayerStageProxy* GetPlayerStageProxy();
		void OnMissionRaid(const int32 mission_id);
		void OnNpcOptionReply(const int32 npc_index, const std::string msg, const std::string str_param, const int32 int_param);
		void OnJoinBattleRoom(const int32 scene_id, const int32 script_id);		
		boost::shared_ptr<MtScene> OnRaidNpc2Scene(const int32 sceneid);
		//
		Packet::ResultCode CanChat(Packet::ChatChannel channel);
		void SetChatChannelCD(Packet::ChatChannel channel,int32 cdtime);
		Packet::ResultCode SendClientNotify(const std::string & content, const int32 channel, const int32 show_type);
		void SendCommonResult(Packet::ResultOption option, Packet::ResultCode code);
		void SendCommonReply(const std::string &name, const std::vector<int32>& int32values, const std::vector<uint64>& int64values,const std::vector<std::string>& stringvalues);
		void BroadCastCommonReply(const std::string &name, const std::vector<int32>& int32values, const std::vector<uint64>& int64values, const std::vector<std::string>& stringvalues);
		void SendLuaPacket(const std::string &name, const std::vector<int32>& int32values, const std::vector<uint64>& int64values, const std::vector<std::string>& stringvalues);
		void BroadCastLuaPacket(const std::string &name, const std::vector<int32>& int32values, const std::vector<uint64>& int64values, const std::vector<std::string>& stringvalues);

		//校验位置id
		bool ValidPositionById(const int32 posid );
		bool ValidPosition(const Packet::Position& pos, const int32 dist);
		//
		void UpdateContainer(const Packet::ContainerType type);
		bool InLootProtect();
		bool BeLoot(uint64 looter_guid);
		boost::shared_ptr<MtNpc> FindNpc(const int32 series_id);
	
		bool PlayerCanHook(int32 scene_stage_id);
		bool CanAutoHook();
		MtBattleGroundManager* GetBattleManager();
		void AddChallengReq(uint64 guid);
		bool CheckChallengReq(uint64 guid) const;
		void EndGuildBoss(int32 damage);
		void UpdateGuildBossDamage(int32 opt,int32 damage);
		void EndWorldBoss();
		void OnWorldBossDamage(int32 damage);	
		void OnSendWorldBossBattleReply();
		void OnSendWorldBossHpUpdate();
		bool CheckGuildBattleQualify();		
		void SendGuildBattleStage(int32 stage,bool dialog = true);
		
		Packet::PlayerRunTimeStatus& RunTimeStatus();
		void BroadCastRunTimeStatus();
		const std::deque<Packet::Position>& GetFollowPath() { return follow_path_; };

		void SetOnHook(bool on_hook);
		void SetOnHookOption(const Packet::HookOption &option);
		const Packet::HookOption & GetOnHookOption() {
			return player_db_->hook_option();
		}
		void SetInBattle(bool in_battle);
		bool GetInBattle();
		
		boost::shared_ptr<MtCellContainer> DataCellContainer() const {
			return datacell_container_;
		}
		void SetDataCellContainer(const boost::shared_ptr<MtCellContainer>& container) {
			datacell_container_ = container;
		}
		const std::string& Name() const;
		void SetName(const std::string &name);
		void OfflineHookReward();
 
		//帮会修炼
		void OnGetPractice();
		boost::shared_ptr<Packet::PracticeInfo> GetPracticeInfo(int32 id);
		std::vector<boost::shared_ptr<Packet::PracticeInfo>>& GetAllPracticeInfo();
		/**
		* \brief 公会修炼所有技能等级之和
		* \return zxero::int32
		*/
		int32 GuildPracticeLevelSum() const;
		void AddPracticeInfo(boost::shared_ptr<Packet::PracticeInfo> p);
		void AddPracticeExp(int32 id,int32 type,int32 guildlevel,int32 allcontribution);
		int32 GetPracticeProperty(Packet::Property pro);
		uint32 GetAllCharge() {
			return player_db_->charge_all();
		}
		void AddAllCharge(uint32 rmb) {
			player_db_->set_charge_all(GetAllCharge()+rmb);
		}
		uint64 GetTeamID();
		uint64 GetTeamLeader();
		Packet::PlayerTeamStatus GetTeamStatus();
		bool IsTeamGroup();
		bool IsTeamLeader();
		int32 TeamMemberSize() const;
		std::vector<uint64> GetTeamMember(bool follow_state = false);
		void OnSyncPlayerTeamData(const boost::shared_ptr<G2S::SyncPlayerTeamData>& msg);
		/**
		* \brief 玩家的队伍等级
		* \retval - 0, 没有队伍
		* \retval - >0, 实际等级
		*/
		int32 TeamLevel();
		void CleanTeamData();
		void ResetTeamData();
		void PlayerAutoMatch(int32 purpose, bool setauto);
		void BroadcastTeamEx(bool follow_state, const google::protobuf::Message& msg);
		void BroadcastTeam(const std::vector<uint64>& filter_list, const google::protobuf::Message& msg);

		void OnChargeOk( const std::string& productid, const std::string& ip, const std::string & order_id, int32 price, int32 count, int32 charge_times, int32 extra_count);
		void MqGenerateOrderID(std::string ProductId, std::string Extra);
		//
		const Packet::BattleFeildProxy* GetBattleFeildProxy();
		void SetBattleFeildProxy(const Packet::BattleFeildProxy & proxy);
	
		//////////////////////////////////////////
		int32 GetParams32(const std::string &key) {
			auto iter = params_32_.find(key);
			if (iter == params_32_.end()) {
				return INVALID_INT32;
			}
			return iter->second;
		}
		int64 GetParams64(const std::string &key) {
			auto iter = params_64_.find(key);
			if (iter == params_64_.end()) {
				return INVALID_INT64;
			}
			return iter->second;
		}
		void SetParams32(const std::string &key, int32 value) {
			params_32_[key] = value;
		}
		void SetParams64(const std::string &key, int64 value) {
			params_64_[key] = value;
		}
		/////////////////////////////////////////////

		int32 GetWorldBossReliveCd()
		{
			return worldboss_relivecd_;
		}
		int64 AddWorldBossDamage(int32 damage)
		{
			worldboss_hp_ += damage;
			return worldboss_hp_;
		}
		
		int64 GetWorldBossDamage()
		{
			return worldboss_hp_;
		}
		void AddWorldBossActorDamage(uint64 guid, int32 damage);
		const std::map<uint64, int32>& GetWorldBossActorDamage();
		/**
		* \brief 玩家内容初始化, 这个会在有了场景后再调用
		* \return void
		*/
		void CheckInit();
		// guild
		void OnCreateGuild(std::string createname, std::string createnotice);
		void UpdateGuildInfo(std::string guildname,uint64 guildid, std::string log_from);
		uint64 GetGuildID();
		std::string GetGuildName();
		void SetGuildName(std::string name);
		MtPlayerGuildProxy* GetPlayerGuildProxy();
		void InitActorEquip();
		void InitDataCell();
		void OnArenaBattleDone(bool win, MtBattleGround* battle_ground);
		const boost::shared_ptr<DB::PlayerTrialInfo> GetTrialInfo() {
			return cur_trialinfo_;
		}

		void ChangeSceneOk() { is_change_ok_ = true; };
		void Cache2Save(google::protobuf::Message * msg);

		//ud = 0 ，不展示； ud=1 展示且分解； ud=2 展示不分解
		void AddItemNotify(const int32 config_id, const int32 ammout, Packet::NotifyItemList *nlist, Config::ItemAddLogType type, int32 ud=2);

		Packet::ResultCode BuyMarketItem(const Config::MarketType type, const int32 index, const int32 count, const bool isauto = false);
		Packet::ResultCode MarketReq(const Config::MarketType type);
		Packet::ResultCode ItemLockReq(const int64 guid, bool lock);
		Packet::ResultCode SetGuideStep(const int32 group, const int32 step);
		Packet::ResultCode ExpPointOption(const int32 lock_or_get);
		Packet::ResultCode GoldOperation(const bool is_take, const int32 count);
		Packet::ResultCode UseItem(const int64 guid, const int64 actor_guid);
		Packet::ResultCode UseItemByCfgId(const int32 config_id, const int32 count);
		Packet::ResultCode DestroyItem(const int64 guid);
		Packet::ResultCode SellItem(const int64 guid);
		Packet::ResultCode ActivityBoxReward(const int32 index);
		Packet::ResultCode OnDrawCard(const Config::DrawType draw_type);
		Packet::ResultCode OnInspire(int32 type);
		Packet::ResultCode OnWorldBossRelive();
		Packet::ResultCode OnHellRelive();
		Packet::ResultCode OnRepairAllEquip();

		//答题
		void RecordPicResult(const int32 index, const int32 result);
		std::map<int32, int32>& GetPicResult() { return pic_group_result_; }
		bool CheckPicResult();
		bool ValidAnswerIndex(const std::string& type, const int32 index);
		Packet::ResultCode OnResetAnswer(const std::string& type);
		Packet::ResultCode OnBeginAnswer(const std::string& type, const int32 count);
		Packet::ResultCode OnNextAnswer(const std::string& type);
		Packet::ResultCode OnConfirmAnswer(const std::string& type, const int32 index);
		bool IsLastAnswer(const std::string& type);

		void SetArenaTarget( const boost::shared_ptr<ArenaMemUser> & target ) {
			curr_arena_target_ = target;
		}
		const boost::shared_ptr<ArenaMemUser> & GetArenaTarget() {
			return curr_arena_target_;
		}

		std::string PlayerBibase();
		void UpdateWebPlayerInfo(std::string type = "PlayerInfo",bool Interday = false);
		void DBLog(std::string type,std::string log, int32 ud);
		//CDK
		void CheckCDKCode(std::string cdkcode);

		void EquipDurableCheck();
		void SetPlayerSetting(const boost::shared_ptr<Packet::PlayerSetting>& setting);
		Packet::PlayerSetting* GetPlayerSetting() const;
		bool AutoDismantleEquip(const int32 config_id,const int32 count,bool force=false, Packet::NotifyItemList *nlist = nullptr);
		//
		void EnableMsgCounter(bool enable) { enable_msg_counter_ = enable; };
		void DumpMsgCounterInfo(bool clean);
		void UpdateMsgCounter(const google::protobuf::Message& message);
		void SetChangeSceneCD(int64 cd) {
			change_scenecd_ = cd;
		}
		int64 GetChangeSceneCD() {
			return change_scenecd_;
		}

		bool ServerLevelCheck( const int32 level);
		void AddPlayerStage(int32 stage_id);
		void LoadPlayerStage(const std::vector<boost::shared_ptr<Packet::PlayerStage>>& stages);
		const std::vector<boost::shared_ptr<Packet::PlayerStage>>& GetPlayerStages() const;
		int32 GetPlayerMaxStageId() const;
		Packet::PlayerStage* GetPlayerStage(int32 stage_id) const;
		void SendStatisticsResult(const int32 statistics, bool reset);
		void StatisticsResolve(const int32 item_id, const int32 count, Config::ItemAddLogType type, const int64 source_id);
		void ResetStatistics(const int32 statistics);
		std::vector<boost::shared_ptr<MtMarket>>& GetAllMarkets() {
			return markets_;
		}
		void OnActorDead(MtBattleGround* battle_ground, MtActor* actor);

		const boost::shared_ptr<Packet::AccountModel> GetAccount() const { return account_; }
		void OnBigTick_1(uint64 elapseTime);
		void OnBigTick_2(uint64 elapseTime);
		void OnBigTick_3(uint64 elapseTime);
		void OnBigTick_4(uint64 elapseTime);
		void OnBigTick_5(uint64 elapseTime);
		void OnBigTick_6(uint64 elapseTime);
		std::string GetIP();
protected:
		Packet::ResultCode DrawCrystalOne(Packet::NotifyItemList &list);
		Packet::ResultCode DrawGoldOne(Packet::NotifyItemList &list);
		Packet::ResultCode DrawCrystalTen(Packet::NotifyItemList &list);
		Packet::ResultCode DrawGoldTen(Packet::NotifyItemList &list);	

		bool AddItemEx(const int32 config_id, const int32 count, Config::ItemAddLogType type, const int64 source_id = 0);
		void SycnAnswerCache();
		void LoadAnswerCache();
		void ExpPointReward();
		bool DrawCardInternal(std::vector<boost::shared_ptr<Config::DrawCardConfig>> &draw_pool, const int32 all_weight, Packet::NotifyItemList *list, const int32 draw_cost);
		void OnNpcRefreshTime(const int32 delta);
		
		//添加东西通过AddItemById来
		bool AddToken(const Packet::TokenType token, const int32 add_count,
			Config::ItemAddLogType type, const int64 source_id = 0,
			Packet::ActorFigthFormation formation = Packet::ActorFigthFormation::AFF_NORMAL_HOOK);
		//删除代币通过delitembyid来
		bool CostToken(const Packet::TokenType token, const int32 cost_count, Config::ItemDelLogType type, const int64 source_id = 0);
		bool AddCard(const int32 config_id, const int32 count, Config::ItemAddLogType type, Packet::NotifyItemList *nlist = nullptr, const int64 source_id = 0);


private:		
		std::map<std::string, int32> params_32_;
		std::map<std::string, int64> params_64_;
		std::map<int32, std::map<int32, int32>> statistics_data_;
		Packet::PlayerBasicInfo * player_data_;
		boost::shared_ptr<MtActor> main_actor_;
		const boost::shared_ptr<Packet::PlayerDBInfo> player_db_;
		boost::shared_ptr<MtScene> scene_;
		boost::shared_ptr<MtZone> zone_;
		mutable	std::mutex session_mutex_;
		std::recursive_mutex mail_mutex_;
		boost::shared_ptr<client_session> session_;
		boost::shared_ptr<ArenaMemUser> curr_arena_target_;
		boost::shared_ptr<MtBattleGround> battle_ground_;
		boost::shared_ptr<MtBattleGround> hook_battle_;
		boost::shared_ptr<MtRaid> raid_; //副本
		std::vector<boost::shared_ptr<MtActor>> actors_;
		std::map<Packet::ContainerType,boost::shared_ptr<MtContainer>> containers_;
		std::vector<std::shared_ptr<PlayerAi>> player_ai_;
		boost::shared_ptr<DB::PlayerTrialInfo> cur_trialinfo_;
		boost::shared_ptr<Packet::ClientClickMove> move_path_ = nullptr;
		std::vector<boost::shared_ptr<MtMarket>>  markets_;
		std::map<int32, boost::shared_ptr<MtActivity>>  activities_;
		std::map<int32, boost::shared_ptr<MtNpc>> npc_map_;
		boost::shared_ptr<FriendContainer> friend_;
		boost::shared_ptr<MessageContainer> message_;
		boost::shared_ptr<Packet::PlayerRunTimeStatus> runtime_status_;
		boost::shared_ptr<MtCellContainer> datacell_container_;
		std::vector< boost::shared_ptr<Packet::PracticeInfo>> guild_practice_;
		boost::shared_ptr<MtMissionProxy> mission_proxy_;		
		boost::shared_ptr<MtArrangeProxy> arrange_proxy_;
		boost::shared_ptr<MtPlayerStageProxy> player_stage_proxy_;
		boost::shared_ptr<MtPlayerGuildProxy> player_guild_proxy_;
		int32 chatcd_[Packet::ChatChannel::MAX];		
		bool team_mission_confirm = false;
		mutable	std::mutex save_mutex_;
		mutable std::mutex team_mutex_;
		mutable std::mutex bfproxy_mutex_;
		mutable std::vector<boost::shared_ptr<Packet::ActorFormation>> actor_formations_; //上阵数据
		std::deque<Packet::Position> follow_path_;
		int32 challenge_boss_counter_ = 0;
		bool disconnect_ = true;
		eLogoutStatus logout_status_ = eLogoutStatus::eInitStatus;	//real logic offline
		uint64 m_refresh_time;	//refresh time
		Packet::BattleFeildProxy bf_proxy_;
		int32 worldboss_relivecd_;
		int64 change_scenecd_ = 0;
		int64 worldboss_hp_;
		std::map<uint64, int32> worldboss_actordamage_;
		MtPlayerSimpleImpactSet simple_impacts_;
		std::vector<int32> text_answers_;
		std::vector<int32> pic_answers_;
		std::map<int32,int32> pic_group_result_;
		uint32 cache_stamp_{ 0 };
		bool is_change_ok_ = true;		
		uint64 text_answers_timer_;
		std::queue<std::pair<int32, boost::shared_ptr<google::protobuf::Message>>> client_msgs_;
		std::mutex client_msgs_mutext_;
		std::vector<uint64> pvp_fight_challengers_;
		Packet::ArrangeType arrange_type_ = Packet::ArrangeType::AT_Roll;
		boost::shared_ptr<Packet::PlayerSetting> settings_;
		uint32 login_time_ = 0;
		boost::shared_ptr<Packet::PlayerStageDB> player_stage_db_;
		std::vector<boost::shared_ptr<Packet::PlayerStage>> stages_;
		bool enable_msg_counter_ = false;
		std::map<std::string, int32> message_counter_;
		boost::shared_ptr<Packet::AccountModel> account_;
		std::vector<MtRobot> robots_;
	};
}
#endif // ZXERO_GAMESERVER_MT_PLAYER_H__