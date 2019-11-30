#ifndef ZXERO_GAMESERVER_MT_SCENE_H__
#define ZXERO_GAMESERVER_MT_SCENE_H__


#include "../mt_types.h"
#include "date_time.h"
#include "../mt_arena/geometry.h"
#include "../mt_battle/mt_battle_ground.h"
#include "../base/mt_lua_proxy.h"

namespace Mt
{
	using namespace zxero;
	class MtZone;
	class MtPlayer;
	class MtNpc;
	class MtRaid;
	class LogicArea;
	struct RaidHelper;
	class MtLuaCallProxy;
	enum ProfAddrType
	{
		PLAYER_COPY_TICK = 0,
		PLAYER_TICK,
		RAIDS_TICK,
		BATTLE_TICK,
		PLAYER_MSG_TICK,
		SCENE_BIG_TICK,
		SCENE_PLAYER_BIG_TICK,
		SCENE_PLAYER_BIG_TICK_1,
		SCENE_PLAYER_BIG_TICK_2,
		SCENE_PLAYER_BIG_TICK_3,
		SCENE_PLAYER_BIG_TICK_4,
		SCENE_PLAYER_BIG_TICK_5,
		SCENE_PLAYER_BIG_TICK_6,
		SCENE_RAID_BIG_TICK,
		SCENE_NPC_BIG_TICK,
		SCENE_RAID_DES_BIG_TICK,
		SCENE_SEC_TICK,
		SCENE_MIN_TICK,
		MAX_TICK_COUNT,
	};
	const int32 SCENE_TICK_HISTROY = 20;
	class MtScene : public boost::noncopyable, public boost::enable_shared_from_this<MtScene>
	{
	public:
		static void lua_reg(lua_State* ls);
	public:
		MtScene(const boost::shared_ptr<Config::Scene>& scene_config, network::event_loop* event_loop_);
		MtScene() = default;
		virtual ~MtScene();
		void DispatcherInit();
		void LuaInit();
		bool BroadcastNotify(const std::string& content);
		virtual bool BroadcastMessage(const google::protobuf::Message& message, const std::vector<uint64>& except_guids = {});
		bool BroadcastMessageRelatedZones(const boost::shared_ptr<MtZone> centre_zone, 
			const google::protobuf::Message& message, const std::vector<uint64>&except_guids = {});
		virtual bool OnPlayerEnterBegin(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<Packet::PlayerEnterScene>& message);
		virtual	bool OnPlayerEnter(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<Packet::PlayerEnterScene>& message, bool full_sync);
		virtual bool OnPlayerEnterEnd(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<Packet::PlayerEnterScene>& message);
		virtual bool OnPlayerReenter(const boost::shared_ptr<MtPlayer>& player);
		virtual bool OnPlayerLeave(const boost::shared_ptr<MtPlayer>& player);
		virtual LogicArea GetInitLogicArea(const boost::shared_ptr<Packet::PlayerEnterScene>& message);
		virtual	LogicArea GetPlayerLogicArea(const boost::shared_ptr<MtPlayer>& player);
		virtual LogicArea GetCurrentLogicArea(const boost::shared_ptr<MtPlayer>& player);
		virtual	void OnTick();
		uint64 FrameElapseTime() const { return frame_elapseTime_; }
		virtual void OnBigTick();
		virtual void OnSecondTick();
		virtual void OnMinTick();

		virtual void OnMonthTriger(int32 month);
		virtual void OnWeekTriger(int32 week);
		virtual void OnDayTriger(int32 day);
		virtual void OnHourTriger(int32 hour);
		virtual void OnMinTriger(int32 min);
		
		// G2S Game to Scene 消息相关 start [11/1/2017 Ryan]
		/**
		* \brief 给场景发送一个待处理消息
		* \param player 指定player
		* - 需要检查是否在场景中.
		* - 如果为nullptr, 则广播给所有玩家
		*/
		void DispatchMessage(const boost::shared_ptr<google::protobuf::Message>& msg,
			const boost::shared_ptr<MtPlayer>& player,
			int32 source);
		
		void ExecuteMessage(const boost::shared_ptr<google::protobuf::Message>& msg,
			const boost::shared_ptr<MtPlayer>& player);

		void BroadcastExcuteMessage(const boost::shared_ptr<google::protobuf::Message>& msg);

		void LuaDispatchMessage(const google::protobuf::Message* msg);
		void LuaExecuteMessage(const google::protobuf::Message* msg);

		void SendS2SCommonMessage(const std::string &name, const std::vector<int32>& int32values, const std::vector<int64>& int64values, const std::vector<std::string>& stringvalues);


		bool OnCacheAll(const boost::shared_ptr<MtPlayer>& player,
			const boost::shared_ptr<G2S::CacheAllNotify>&, int32 /*source*/);
		
		bool OnXLuaReg(const boost::shared_ptr<MtPlayer>&,
			const boost::shared_ptr<G2S::XLuaRegNotify>&, int32 /*source*/);
		bool OnLuaLoad(const boost::shared_ptr<MtPlayer>&,
			const boost::shared_ptr<G2S::LuaLoadNotify>& msg, int32 /*source*/);
		bool OnPlayerOnline(const boost::shared_ptr<MtPlayer>& player,
			const boost::shared_ptr<G2S::PlayerOnline>& msg, int32 /*source*/);
		bool OnPlayerDisconnect(const boost::shared_ptr<MtPlayer>& player,
			const boost::shared_ptr<Packet::PlayerDisconnect>& msg, 
			int32 /*source*/);
		// G2S Game to Scene 消息相关 end [11/1/2017 Ryan]
		int32 SceneId() const;
		void OnTeamFollowMove(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<LogicArea> logic_area,const uint32 time_flag);
		void OnTeamFollowStop(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<LogicArea> logic_area);
		bool OnSyncPlayerTeamData(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<G2S::SyncPlayerTeamData>& msg, int32 /*source*/);
		bool OnResetPlayerTeamData(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<G2S::ResetPlayerTeamData>& msg, int32 /*source*/);

		static bool IsRaidScene(const int32 sceneid);
		static bool IsDynamicNpc(const int32 npcid);
		boost::shared_ptr<MtPlayer> FindPlayer(uint64 guid);
		boost::shared_ptr<MtPlayer> FindPlayerBySceneServer(uint64 guid);
		boost::shared_ptr<MtZone> GetZoneById(zxero::int32 zone_id);
		std::vector<boost::shared_ptr<MtZone>> GetRelatedZone(const boost::shared_ptr<MtZone>& centre_zone);
		void QueueInLoop(boost::function<void()> functor);
		boost::shared_ptr<MtZone> GetZoneByPos(const Packet::Position& pos);
		bool ValidPos(const Packet::Position& pos);
		boost::shared_ptr<MtDataCell> GetSceneData() { return scene_cell_; }
		const boost::shared_ptr<Config::Scene>& Config() { return config_; }
		bool OnLootPlayer(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<Packet::LootPlayer>& msg, int32 /*source*/);
		//Message Handlers start
		bool UnRegistedMessage(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<google::protobuf::Message>& message, int32 /*source*/);
		bool UnRegistedInnerMessage(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<google::protobuf::Message>& message, int32 /*source*/);
		bool OnClientMove(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<Packet::ClientPlayerMove>& message, int32 /*source*/);
		bool OnClientStop(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<Packet::ClientPlayerStop>& message, int32 /*source*/);
		//void OnClientClickMove(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<Packet::ClientClickMove>& message, int32 /*source*/);
		bool OnChangeHair(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<Packet::ChangeHair>& message, int32 /*source*/);
		bool OnPlayerEnterScene(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<Packet::PlayerEnterScene>& message, int32 /*source*/);
		bool OnPlayerChangeScene(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<Packet::PlayerChangeSceneRequest>& message, int32 /*source*/);
		bool OnClientEnterSceneOk(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<Packet::ClientEnterSceneOk>& message, int32 /*source*/);
		bool OnSessoinChanged(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<Packet::SessionChanged>& msg, int32 /*source*/);
		//void OnPlayerEnterRaid(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<Packet::PlayerEnterRaid>& message, int32 /*source*/);
		bool OnPlayerLeaveScene(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<Packet::PlayerLeaveScene>& message, int32 /*source*/);
		bool OnActorList(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<Packet::ActorList>& message, int32 /*source*/);
		bool OnChatMessage(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<Packet::ChatMessage>& message, int32 /*source*/);
		//void OnEnterBattleGround(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<Packet::EnterBattleGround>& message, int32 /*source*/);
		bool ExitBattleGround(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<Packet::ExitBattleGround>& message, int32 /*source*/);
		bool OnPlayerStartHook(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<Packet::PlayerStartHook>& message, int32 /*source*/);
		bool OnPlayerEndHook(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<Packet::PlayerEndHook>& message, int32 /*source*/);
		bool OnChangeHookInfo(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<Packet::ChangeHookInfo>& message, int32 /*source*/);
		bool OnDelMessageReq(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<Packet::DelMessageReq>& message, int32 /*source*/);
		//挑战挂机boss
		bool OnCaptureStageToScene(const boost::shared_ptr<MtPlayer>& player,
			const boost::shared_ptr<Packet::CaptureStageToScene>& msg,
			int source);
		bool OnCollectTax(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<Packet::CollectAllTax>& message, int32 /*source*/);
		bool OnChallengeBoss(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<Packet::ChallengeBossReq>& message, int32 /*source*/);
		bool OnConstantChallengeBoss(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<Packet::ConstantChallengeBoss>& message, int32 /*source*/);
		bool OnUpdateFollowState(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<Packet::UpdateFollowState>& message, int32 /*source*/);
		bool OnPlayerReachBattlePos(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<Packet::PlayerReachBattlePos>& message, int32 /*source*/);
		bool OnPlayerContainerRequet(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<Packet::PlayerContainerRequest>& message, int32 /*source*/);
		bool OnActorAddEquipRequest(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<Packet::ActorAddEquipRequest>& message, int32 /*source*/);
		bool OnActorDelEquipRequest(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<Packet::ActorDelEquipRequest>& message, int32 /*source*/);
		bool OnBattleGroundMoveFinish(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<Packet::ActorMoveToTargetFinish>& msg, int32 /*source*/);
		bool OnBattleGroundSetControlType(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<Packet::SetControlType>& message, int32 /*source*/);
		bool OnBattleGroundClientActorMove(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<Packet::ClientActorMove>& message, int32 /*source*/);
		bool OnBattleGroundClientActorStop(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<Packet::ClientActorStop>& message, int32 /*source*/);
		bool OnBattleGroundClientActorUseSkill(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<Packet::ClientActorUseSkill>& message, int32 /*source*/);
		//竞技场
		bool OnArenaPanelOpen(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<Packet::ArenaPanelOpen>& message, int32 /*source*/);
		bool OnArenaPanelClose(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<Packet::ArenaPanelClose>& message, int32 /*source*/);
		bool OnArenaRefreshTarget(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<Packet::ArenaRefreshTarget>& message, int32 /*source*/);
		bool OnArenaChallengePlayer(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<Packet::ArenaChallengePlayer>& message, int32 /*source*/);
		bool OnArenaBuyChallengeCount(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<Packet::ArenaBuyChallengeCount>& msg, int32 /*source*/);

		//
		bool OnReNameRequest(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<Packet::ReNameRequest>& message, int32 /*source*/);
		//friend
		bool OnSendGiftReq(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<Packet::SendGiftReq>& message, int32 /*source*/);
		/**
		* \brief 试炼场玩家刷新完毕
		*/
		bool OnPlayerTrialInfoLoad(const uint64 player_guid);
		bool OnTrialPlayerLoadDone(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<DB::PlayerTrialInfo>& msg, int32 /*source*/);
	
		//阵容
		bool OnActorFormationPull(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<Packet::ActorFormationPull>& message, int32 /*source*/);
		bool OnSetActorFight(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<Packet::SetActorFightReq>& message, int32 /*source*/);
		bool CheckFormation(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<Packet::SetActorFightReq>& message);
		
		//升级,升星,升阶 start
		bool OnUpgradeLevel(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<Packet::UpgradeLevelReq>& message, int32 /*source*/);
		bool OnUpgradeColor(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<Packet::UpgradeColorReq>& message, int32 /*source*/);
		bool OnUpgradeStar(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<Packet::UpgradeStarReq>& message, int32 /*source*/);
		//升级,升星,升阶 end
		//技能升级
		bool OnUpgradeSKill(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<Packet::UpgradeSkillReq>& message, int32 /*source*/);
		//装备相关 装备强化 装备附魔 装备镶嵌 装备继承 start
		//装备强化
		bool OnEnhenceEquip(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<Packet::EnhenceEquipReq>& message, int32 /*source*/);
		//装备附魔开孔
		bool OnAddEnchantSlot(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<Packet::AddEnchantSlotReq>& message, int32 /*source*/);
		//装备附魔 
		bool OnEnchantEquip(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<Packet::EnchantEquipReq>& message, int32 /*source*/);
		//装备镶嵌, 宝石镶嵌
		bool OnInsetGem2Equip(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<Packet::InsetEquipReq>& message, int32 /*source*/);
		//装备镶嵌卸载
		bool OnTakeOutGem(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<Packet::TakeOutGemReq>& msg, int32 /*source*/);
		//装备继承
		bool OnEquipInherit(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<Packet::EquipInheritReq>& message, int32 /*source*/);
		//装备批量附魔
		bool OnBatchEnchantEquip(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<Packet::BatchEnchantEquipReq>& message, int32 /*source*/);
		//装备批量附魔确认
		bool OnBatchEnchantEquipConfirm(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<Packet::BatchEnchantEquipConfirmReq>& message, int32 /*source*/);
		//装备修理
		bool OnFixEquipReq(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<Packet::FixEquipReq>& message, int32 /*source*/);
		//装备相关 装备强化 装备附魔 装备镶嵌 装备继承 end
		//宝石合成 git test
		bool OnGemFusionReq(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<Packet::GemFusionReq>& message, int32 /*source*/);
		//装备分解
		bool OnDismantleEquipReq(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<Packet::DismantleEquipReq>& message, int32 /*source*/);
		//
		bool OnHeroFragmentFusion(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<Packet::FragmentToHeroReq>& message, int32 /*source*/);
		//装备幻化
		bool OnEquipMagicalReq(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<Packet::EquipMagicalReq>& message, int32 /*source*/);
		//移动物品
		bool OnMoveItemReq(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<Packet::MoveItemRequest>& message, int32 /*source*/);
		//扩容
		bool OnExtendContainerReq(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<Packet::ExtendContainerRequest>& message, int32 /*source*/);
		//npc对话
		bool OnNpcSessionReq(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<Packet::NpcSession>& message, int32 /*source*/);
		//nonpc对话
		bool OnNoNpcSessionReq(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<Packet::NoNpcSession>& message, int32 /*source*/);
		//玩家npc对话
		bool OnPlayerNpcSessionReq(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<Packet::PlayerNpcSession>& message, int32 /*source*/);
		//
		bool OnSyncMissionReq(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<Packet::SyncMissionRequest>& message, int32 /*source*/);
		bool OnClickChatMissionReq(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<Packet::ClickChatMissionReq>& message, int32 /*source*/);
		//
		bool OnCommonRequest(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<Packet::CommonRequest>& message, int32 /*source*/);
		bool OnLuaRequestPacket(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<Packet::LuaRequestPacket>& message, int32 /*source*/);
		//player opy info
		bool OnPlayerOperateInfoReq(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<Packet::PlayerOperateInfoReq>& message, int32 /*source*/);
		// WORLDBOSS
		bool OnWorldBossInfoReq(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<Packet::WorldBossInfoReq>& message, int32 /*source*/);
		bool OnWorldBossHpReq(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<Packet::WorldBossHpReq>& message, int32 /*source*/);
		bool OnLeaveWorldBossReq(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<Packet::LeaveWorldBossReq>& message, int32 /*source*/);
		bool OnWorldBossRelivenow(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<Packet::WorldBossRelivenow>& message, int32 /*source*/);
		bool OnWorldBossGoldInspire(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<Packet::WorldBossGoldInspire>& message, int32 /*source*/);
		bool OnWorldBossDiamondInspire(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<Packet::WorldBossDiamondInspire>& message, int32 /*source*/);
		bool OnWorldBossStatisticsReq(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<Packet::WorldBossStatisticsReq>& message, int32 /*source*/);
		bool OnSetWorldBossAutoFightReq(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<Packet::SetWorldBossAutoFightReq>& message, int32 /*source*/); 
		//Message Handler end
		bool OnRobotPlayerInOk(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<Packet::RobotPlayerInOk>& msg, int32 /*source*/);
		//G2SMessage
		bool OnG2SCommonMessage(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<G2S::G2SCommonMessage>& message, int32 /*source*/);
		bool OnG2SSyncPlayerGuildData(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<G2S::SyncPlayerGuildData>& message, int32 /*source*/);
		bool OnG2SCodeCheckDone(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<G2S::G2SCodeCheckDone>& message, int32 /*source*/);
		//S2SMessage
		bool OnS2SCommonMessage(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<S2S::S2SCommonMessage>& message, int32 /*source*/);
		/**
		* \brief 这个方法比较特殊, 请勿调用!!!
		* - 服务器正常退出时, 玩家的退出逻辑时在主线程执行的
		* - 主线的fflua_t本来是没有注册任何场景相关的方法的
		* - 所有在最后阶段, copy了一份场景的fflua,用于执行退出逻辑
		*/
		ff::fflua_t* Lua() const { return lua_; }
		int32 AddNpc(const int32 pos_id, const int32 index, const int32 lifetime);
		void DelNpc(const int32 series_id);
		void LockNpc(const int32 series_id, const bool lock);
		int32 AddRaid( const RaidHelper &helper);
		boost::shared_ptr<MtRaid> GetRaid(int32 rt_id) ;
		void DelRaid(const int32 rt_id);
		void DelNpcByType(const std::string src_type);		
		boost::shared_ptr<MtNpc> FindNpc(const int32 series_id);
		const std::map<int32, boost::shared_ptr<MtNpc>>& GetNpcList() { return npc_map_;  }
		void PlayerLeaveScene(const boost::shared_ptr<MtPlayer>& player);
		Packet::Position SpawnPos(int32 index);
		bool HandlePlayerMessage(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<google::protobuf::Message>& msg, int32 source);
		bool HandleInnerMessage(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<google::protobuf::Message>& msg, int32 source);
		void OnShutDown();
		
		MtLuaCallProxy* GetLuaCallProxy();
		MtBattleGroundManager& GetBattleManager();
		//
		/**
		* \brief 改变玩家的位置后, 通知一下客户端刷新
		* \param player
		* \return bool
		*/
		virtual bool TransportByPosIndex(const boost::shared_ptr<MtPlayer>& player, int32 posindex);
		virtual bool Transport(const boost::shared_ptr<MtPlayer>& player, const Packet::Position & init_pos);
	protected:
		void __regist_player_2_scene(const boost::shared_ptr<MtPlayer>& player);
		void __on_player_leave(const boost::shared_ptr<MtPlayer>& player);
		void __after_player_regist_2_scene(const boost::shared_ptr<MtPlayer>& player);
		void OnNpcRefreshTime(const int32 delta);
		void OutputTickProf() const;
		const boost::shared_ptr<Config::Scene> config_;
		std::map<uint64, boost::shared_ptr<MtPlayer>> player_map_;
		std::map<int32, boost::shared_ptr<MtRaid>> raids_;//当前场景创建的副本表
	private:
		std::map<int32, boost::shared_ptr<MtZone>> zone_map_;
		network::event_loop* loop_;
		int32 zone_count_;
		std::unique_ptr<message_dispatcher<MtScene, MtPlayer>> client_message_dispatcher_;
		std::unique_ptr<message_dispatcher<MtScene, MtPlayer>> inner_message_dispatcher_;
		uint64 last_tick_;
		uint64 frame_elapseTime_ = 0;
		uint64 last_big_tick_;
		bool tick_player_map_locked_ = false;
		boost::shared_ptr<Rect<int32>> scene_area_;
		std::map<int32, boost::shared_ptr<MtNpc>> npc_map_;		   //动态npc表
		
		int32 width_;
		int32 height_;
		point_xy<int32> fix_offset_;

		int64 raid_count_ = 0;//副本计数
		ff::fflua_t* lua_ = nullptr;
		mutable std::mutex raid_mutex_;

		boost::shared_ptr<MtDataCell> scene_cell_;

		MtLuaCallProxy lua_call_;
		MtBattleGroundManager battle_manager_;
		int32 ticks_[SCENE_TICK_HISTROY][ProfAddrType::MAX_TICK_COUNT];
		int32 tick_index_ = 0;
		int32 big_tick_index_ = 0;
		int32 sec_tick_index_ = 0;
		int32 min_tick_index_ = 0;

	};

}

#endif // ZXERO_GAMESERVER_MT_SCENE_H__