#ifndef ZXERO_GAMESERVER_MT_BATTLE_GROUND_H__
#define ZXERO_GAMESERVER_MT_BATTLE_GROUND_H__

#include "../mt_types.h"

#include "BattleGroundInfo.pb.h"
#include <BattleExpression.pb.h>
#include <vector>

using Mt::TrapImpl::Trap;
namespace Mt
{
	struct Statistics {//掉落统计
		void Clear()
		{
			player_exp_ = 0;
			actor_exp_ = 0;
			damage_ = 0;
			kill_monsters_ = 0;
			self_dead_times_ = 0;
			boss_count_ = 0;
			items_.clear();
			damages_.clear();
		}
		int32 player_exp_ = 0;
		int32 actor_exp_ = 0;
		int32 damage_ = 0;
		int32 kill_monsters_ = 0;
		int32 self_dead_times_ = 0;
		int32 boss_count_ = 0;
		std::map<int32, int32> items_; //物品掉落
		std::map<uint64, int32> damages_;//伤害统计
		std::map<uint64, int32> cures_; //治疗统计
	};
	namespace Battle
	{
		const int32 BATTLE_GROUND_DEFAULT_SCRIPT_ID = 1001;
		const int32 BATTLE_GROUND_PVE_HOOK_SCRIPT_ID = 1002;
		class BaseObject;
		/*!
		* \class BattleGround
		*
		* \brief 战场负责执行战场内所有战斗逻辑, 分发战斗表现消息给客户端\n
		* 所有需要修改的逻辑通过脚本来实现, 战场自己只实现最简单的战斗流程\n
		* 流程如下
		* -# 创建战斗, 设置scene_battle_group_id, type[pve,pvp], script_id
		* -# 初始化流程. 1 创建攻击者, 防御者 2 初始化攻击者和防御者的位置
		* -# 战场一开始处于等待状态, 通过调用StartBattle即可开始
		* \author Ryan
		* \date 30/03/2017
		*/
		class  BattleGround : public boost::noncopyable, public boost::enable_shared_from_this<BattleGround>
		{
		public:
			/**
			* \brief 创建战场对象
			* \param creator 创建该战场的玩家
			* \param target pvp 目标玩家, 可能是空
			* \usage 创建战斗并绑定脚本 ---> 开始
			* - auto bg = make_shared<BattleGround>(creator, nullptr);
			* - bg->BindBattleTypeAndScriptId(any_type, 1);
			* - ZXERO_ASSERT(bg->Init());
			* - ZXERO_ASSERT(bg->StartBattle());
			*/
			BattleGround(const boost::shared_ptr<MtPlayer>& creator, const boost::shared_ptr<MtPlayer>& target);
			/**
			* \brief 设置当前战斗的场景站位信息
			* \param battle_group_id
			* \return bool
			*/
			bool SetSceneBattleGroup(int32 battle_group_id);
			/**
			* \brief 设置战斗类型,站位,脚本id
			* \param type 战斗类型 [pve, pvp]
			* \param battle_group_id 战斗的场景站位信息
			* \param script_id 绑定脚本号
			* \retval true 成功
			*/
			bool BindBattleTypeAndScriptId(Packet::BattleGroundType type, int32 battle_group_id, int32 script_id = BATTLE_GROUND_DEFAULT_SCRIPT_ID);

			/**
			* \brief 清空所有内部对象和重置所有状态
			* \return void
			*/
			void Clear();
			/**
			* \brief 初始化第一个阵营的战斗角色
			* \retval true 初始化成功
			* \retval false 初始化失败
			*/
			bool InitSideOneObjects();
			/**
			* \brief 初始化第二个阵营的战斗角色
			* \retval true 初始化成功
			* \retval false 初始化失败
			*/
			bool InitSideTwoObjects();
			/**
			* \brief 设置战场内各阵营角色的站位
			* \return bool
			*/
			bool InitPosition();
			/**
			* \brief 初始化战场数据
			* \return bool
			*/
			bool Init();

			/**
			* \brief 重置战场数据
			* \return bool
			*/
			bool RefreshBattle();

			/**
			* \brief 战场tick事件
			* \param elapseTime
			*/
			void OnTick(uint64 elapseTime);
			/**
			* \brief 添加一个战场相关对象到战场中
			* \param side 阵营编号
			* -	1 可以理解为攻击方
			* - 2 可以理解为防御方
			* \param object
			* \retval true 添加成功
			* \retval false 添加失败
			*/
			bool AddObject(int32 camp, const boost::shared_ptr<BaseObject>& object);
			/**
			* \brief 设置战场为战斗状态, 开始进行计算
			* \return bool
			*/
			bool StarBattle();
			/**
			* \brief 向战场创建者以及所有观众发送战场消息
			* \param msg 消息内容
			*/
			void BroadCastMessage(const google::protobuf::Message& msg) const;
			/**
			* \brief 当前帧耗时. 用于向Behaviac里的对象传递帧耗时
			* \return google::protobuf::uint64
			*/
			uint64 FrameElapseTime() const;
			/**
			* \brief 战斗结束
			* \param winner_camp 胜利方的阵营编号, [1,2]
			*/
			void SendBattleEndMessage(int32 winner_camp);
			/**
			* \brief 检测战斗是否结束
			* \retval 0 未结束
			* \retval 1 阵营1胜利
			* \retval 2 阵营2胜利
			*/
			int32 GetWinnerSide();
			/**
			* \brief 检测某一阵营是否全部阵亡
			* \param camp
			* \return bool
			*/
			bool AllObjectDead(int32 camp);
			/**
			* \brief 给胜利一方发放奖励
			* \param winner_camp [1,2]
			* \param packet_id 掉落包id
			* \return bool
			*/
			bool SendWinnerReward(int32 winner_camp, int32 packet_id);
			/**
			* \brief 重置阵营一的对象
			* \return bool
			*/
			bool RefreshSideOneObjects();
			/**
			* \brief 重置阵营二的对象
			* \return bool
			*/
			bool RefreshSideTwoObjects();
			/**
			* \brief 从战斗开始到现在已经过去的时间,单位毫秒
			* \return int32
			*/
			uint64 ElapseTime() const { return brief_.elapse_time(); }

		private:
			/**
			* \brief 多线程场景, 每次从创建者身上获取lua_state
			* \return ff::fflua_t*
			*/
			boost::shared_ptr<MtPlayer> creator_;
			boost::shared_ptr<MtPlayer> target_;
			std::vector<boost::shared_ptr<BaseObject>> side_1_objects_;
			std::vector<boost::shared_ptr<BaseObject>> side_2_objects_;
			std::vector<std::function<void(void)>> delay_commands_;
			std::vector<boost::weak_ptr<MtPlayer>> audiences_; //观众
			boost::shared_ptr<Statistics> statistics_;
			Packet::BattleGroundStatus status_ = Packet::BattleGroundStatus::WAIT;
			boost::shared_ptr<Config::SceneBattleGroup> scene_battle_group_; //战斗站位点数据
			int32 script_id_ = 0;

			bool initialized_ = false;
			Packet::BattleGroundBrief brief_;
		};
	}
	#define  MAX_COPYSCENE_DATA_PARAM 20
	class BattleCommand;
	typedef boost::function<bool(const boost::shared_ptr<MtActor>&)> ActorSelector;
	//战场, 组织敌我双方战斗的地方
	class MtBattleGround : public boost::noncopyable, public boost::enable_shared_from_this<MtBattleGround>
	{
	public:
		static void lua_reg(lua_State* ls);
	public:
		MtBattleGround();;
		MtBattleGround(MtPlayer* create_player, MtPlayer*  target_player);
		virtual ~MtBattleGround();

		bool Init();
		void SetFightStatus();
		bool OnTick(zxero::uint64 elapseTime);
		void ProcessEvents();
		bool operator==(const boost::shared_ptr<MtBattleGround>& rhs) = delete;
	
		void RefreshBattle(int32 wait_mill_second = 0);
		void RefreshBattleGroup();
		void PlayerRaidStage();
		//
		uint64 CreateBattleReply();
		Packet::BattleReply* GetBattleReply();
		void SaveBattleReply() const;
		bool AddStat2Reply();
		int32 GenMoveSerial();
		bool TimeOut() const;
		bool MoveByClient();
		void UpdateBattleEarning();
		int32 GetStatisticsDamage();
		void OnMonsterDead(const MonsterBattleActor& monster, MtActor* killer);
		void OnDamageRefix(Packet::ActorOnDamage& damage_info, MtActor* killer);
		void OnDamage(const Packet::ActorOnDamage& damage_info);
		void OnCure(Packet::ActorOnCure& cure_info);
		Packet::Stat* GetStat() { return &stat_; }
		void OnActorCopyDead(MtActor* dead_one, MtActor* killer);			 //有角色死亡了
		Packet::Position RelativePosition(uint64 actor_guid, int32 index, bool opponent = false);//返回相对位置的坐标
		int32 GetAllDeadCampID();																 //得到所有死去的阵营,阵营ID 0-还没有 1-attackers 2-defenders
		
		bool SendCommand(const boost::shared_ptr<BattleCommand> command);
		void BroadCastMessage(const google::protobuf::Message& message);
		void LogMessage(const google::protobuf::Message& msg);
		void FillBattleGroundInfo(Packet::BattleGroundInfo& info);
		void OnPlayerReachPos();
		void OnBattleEnd(bool force_end, uint64 source);
		void BattleEnterNotify();
		/**
		* \brief 挂机类型的战斗有效率上线, 刷新时可能需要等待
		* \return int32 等待时长(毫秒)
		*/
		int32 GetRefreshWaitTime() const;
		void BattleOverNotify(int32 winnercamp);
		void BattleGroupDrop(const int32 monster_group_id);
		void PushHookStatisticsToStageManager();
		void SetHookPause(bool pause);
		//
		Packet::BattleGroundStatus GetStatus() { return status_; }
		void SetStatus(Packet::BattleGroundStatus status) { status_ = status; }
		int32 GetScriptId() { return script_id_; }
		void SetScriptId( int32 script_id ) { script_id_ = script_id; }

		//brief 相关
		Packet::BattleGroundBrief& Brief();
		uint64 TimeStamp()	{ return brief_.elapse_time(); }
		Packet::BattleGroundType BattleType() { return brief_.type(); }
		int32 GetSceneId() { return brief_.scene_id(); }
		int32 GetStageId() { return brief_.stage_id(); }
		uint64 Guid() { return brief_.guid(); }

		/////////////////////////////////////////////
		bool CreateAttackerPlayerBattleActors(bool is_team, bool is_main_actor, Packet::ActorFigthFormation form);
		bool CreateDefenderPlayerBattleActors(bool is_team, bool is_main_actor, Packet::ActorFigthFormation form);
		bool CleanDeadBattleActors();//清理一下参战角色[剔除初始血量为0的角色]
		bool CreateDefenderFromStageCaptain();
		bool CreateDefenderFromArena();
		bool CreateDefenderFromHell();
		void CreateDefenderMonster(const std::map<int32, int32> monster_info);
		void InitAttackerFromActorWithoutPlayer(MtActor* actor);
		void InitDefenderFromActorWithoutPlayer(MtActor* actor);
		void TransformAttackers2BattleActor();
		void TransformDefenders2BattleActor();
		//
		void AddPlayer(const boost::shared_ptr<MtPlayer>& player);
		void AddMonster(int32 side, int32 config_id, int32 pos_index = 0);
		void AddActor(uint64 teammate_guid, const boost::shared_ptr<MtActor>& newbie);
		void AddActor(int32 side, const boost::shared_ptr<MtActor>& newbie);
		void AddTrap(const boost::shared_ptr<Trap>& trap);
		void DelActor(uint64 guid);	//删掉一个战斗actor
		void DeleteActorNow(uint64 guid); //仅在event中调用, 避免迭代器失效
		MtActor* FindActor(uint64 guid);	
		std::vector<MtActor*> Enemies(MtActor& self,ActorSelector selector = ActorSelector());	//找到战场内自己的所有敌人		
		std::vector<MtActor*> TeamMates(MtActor& self,ActorSelector selector = ActorSelector());//找到战场内自己的友军
		bool IsTeamMate(const MtActor& lhs, const MtActor& rhs);
		std::vector<MtActor*> AllActors(ActorSelector selector = ActorSelector());

		/////////////////////////////////////////////////
		std::vector<boost::shared_ptr<MtActor>>& GetAttackers() {
			return attackers_;
		}
		std::vector<boost::shared_ptr<MtActor>>& GetDefenders() {
			return defenders_;
		}
		std::vector<boost::shared_ptr<MtPlayer>> GetAttackerPlayers();
		std::vector<boost::shared_ptr<MtPlayer>> GetDefenderPlayers();

		MtPlayer* GetCreator() {
			return creator_;
		}
		MtPlayer* GetTarget() {
			return target_;
		}	
		int32 GetParams32(const std::string &key) {
			auto iter = params_32_.find(key);
			if (iter == params_32_.end()) {
				return -1;
			}
			return iter->second;
		}
		int64 GetParams64(const std::string &key) {
			auto iter = params_64_.find(key);
			if (iter == params_64_.end()) {
				return -1;
			}
			return iter->second;
		}
		void SetParams32(const std::string &key, int32 value) {
			params_32_[key] = value;
		}
		void SetParams64(const std::string &key, int64 value) {
			params_64_[key] = value;
		}

		void PushEvent(const boost::shared_ptr<Event>& e);

		//遍历战场内所有actor, then apply functor
		void EnumAllActor(const boost::function<void(const boost::shared_ptr<MtActor>& actor)>& functor);
		void SetClientInsight(bool insight);
		bool GetClientInsight() const { return client_insight_; }
	protected:

		//团队分配逻辑
		bool OnCheckArrange();
		bool OnNextBattle();
		void EnumActors(const std::vector<boost::shared_ptr<MtActor>>& actors,	const boost::function<void(const boost::shared_ptr<MtActor>& actor)>& functor);
		void InitAttackerPos();
		void InitDefenderPos();
		void InitPosAndDir();
		void InitDirection();
		void EquipDurableCost();
		void AddBossChallengeProgress(int32 scene_stage_id, int32 count);
		void ClearAttacker();
		void ClearDefender();
		void CheckLackeysBuff(std::vector<boost::shared_ptr<MtActor>> &actor_list);
		bool IsAttacker(uint64 guid);
	private:
		 
		std::vector<boost::shared_ptr<MtActor>>	attackers_;
		std::vector<boost::shared_ptr<MtActor>>	defenders_;
		std::vector<boost::shared_ptr<MtActor>> new_attackers_;
		std::vector<boost::shared_ptr<MtActor>> new_defenders_;
		MtPlayer* creator_ = nullptr;			//发起战斗的玩家
		MtPlayer* target_ = nullptr;   //pvp目标
		uint64 fight_start_time_ = 0;
		std::list<boost::shared_ptr<BattleCommand>> commands_; //需要执行各类指令
		Packet::BattleGroundBrief brief_;
		Config::SceneBattleGroup* scene_battle_group_ = nullptr;
		std::vector<boost::shared_ptr<MtPlayer>> players_;
		std::vector<boost::shared_ptr<Trap>> traps_;
		boost::shared_ptr<Statistics> statistics_;
		Packet::BattleGroundStatus status_ = Packet::BattleGroundStatus::END;
		int64 waiter_ = 0;
		int32 winnercamp_ = 0;//0-none 1- attackers_win 2-defenders_win	
		int32 IntParam_[MAX_COPYSCENE_DATA_PARAM];
		std::vector<boost::shared_ptr<Event>> events_;
		boost::shared_ptr<Packet::BattleReply> reply_;
		bool client_insight_ = true;
		int32 battle_time_ = 0;
		//uint64 leave_time_ = 0;
		int32 move_serial_ = 0;
		bool sim_pause_ = false;
		Packet::StatisticInfo battle_drops_;
		int32 monster_kills_ = 0;
		int32 script_id_ = 0;

		std::map<std::string, int32> params_32_;
		std::map<std::string, int64> params_64_;
		Packet::Stat stat_;
	};


	class MtBattleGroundManager 
	{
	public:
		static void lua_reg(lua_State* ls);
		const boost::shared_ptr<MtBattleGround> CreateHookBattle(
			MtPlayer* player, std::map<std::string, int32> params);
		const boost::shared_ptr<MtBattleGround> CreateFrontBattle(
			MtPlayer * player, MtPlayer* target,
			std::map<std::string, int32> params);
		const boost::shared_ptr<MtBattleGround> CreatePureBattle();
		void OnTick(uint64 elapseTime);
		int32 BattleCount() const { return battles_.size(); }
	private:
		std::list<boost::shared_ptr<MtBattleGround>> battles_;
	};


}
#endif // ZXERO_GAMESERVER_MT_BATTLE_GROUND_H__
