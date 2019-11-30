#ifndef MT_BATTLE_ROOM_MANAGER_H__
#define MT_BATTLE_ROOM_MANAGER_H__
#include "../mt_types.h"
#include "object_pool.h"
#include "date_time.h"

#include <BattleField.pb.h>

namespace Mt
{
	struct RaidHelper;
	class MtBattleField;
	class MtRaid;

	class MtBattleFieldManager : public boost::noncopyable
	{
	public: 
		static void lua_reg(lua_State* ls);
	public:
		bool OnJoinBattleRoom(const boost::shared_ptr<client_session>& session, const boost::shared_ptr<Packet::JoinBattleRoom>& message, int32 /*source*/);
		bool OnQuitBattleRoom(const boost::shared_ptr<client_session>& session, const boost::shared_ptr<Packet::LeaveBattleRoom>& message, int32 /*source*/);
		bool OnQuitBattleField(const boost::shared_ptr<client_session>& session, const boost::shared_ptr<Packet::LeaveBattleField>& message, int32 /*source*/);
		bool OnAgreeEnterBattleField(const boost::shared_ptr<client_session>& session, const boost::shared_ptr<Packet::AgreeEnterBattleField>& message, int32 /*source*/);
		bool OnDisAgreeEnterBattleField(const boost::shared_ptr<client_session>& session, const boost::shared_ptr<Packet::DisAgreeEnterBattleField>& message, int32 /*source*/);
		bool OnFlagOprate(const boost::shared_ptr<client_session>& session, const boost::shared_ptr<Packet::FlagOprate>& message, int32 /*source*/);
		bool OnBattleFieldFightTarget(const boost::shared_ptr<client_session>& session, const boost::shared_ptr<Packet::BattleFieldFightTarget>& message, int32 /*source*/);
		
		bool OnDestroyRoom(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<S2G::DestroyRoom>& message, int32 /*source*/);
		//bool OnBuffPoint(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<S2G::BuffPoint>& message, int32 /*source*/);
		
		void OnCreatBattleResult(MtPlayer * player, const uint64 target_guid,const bool isok);
		void OnRaidBattleOver(MtPlayer * winner, MtPlayer * losser);		
		bool LeaveBattleField(MtPlayer* player);
		void EnterBattleField(MtPlayer* player);
	public:
		static MtBattleFieldManager& Instance();
		MtBattleFieldManager();
		~MtBattleFieldManager();
		zxero::int32 OnLoad();
		
		void OnSecondTick(int32 elapse_second);
		void OnBigTick(uint64 elapseTime);
		void JoinBattleRoom(MtPlayer * player,const int32 scene_id, const int32 script_id);
		void BroadcastToRoom(uint64 rtid, const google::protobuf::Message& msg);
		void TryEnterBattleField(MtPlayer * player);
		Packet::ResultCode Room2BattleFieldRaid( MtBattleField* room, MtScene * scene, MtPlayer * player);
		MtBattleField * FindRoomById(const uint64 rtid);
		Packet::ResultCode OnDoDropFlag(MtPlayer * player);
		Packet::ResultCode OnDoPickFlag(MtPlayer * player);
		Packet::ResultCode OnDoCancelFlag(MtPlayer * player);
		const boost::shared_ptr<Packet::BFConfig> FindBfConfig(const int32 level);
	private:
		MtBattleField * JoinRoom(const RaidHelper& helper, MtPlayer * player);
		MtBattleField * CreateRoom(const RaidHelper& helper,const int32 max_lv,const int32 min_lv);
		MtBattleField * FindRoom(const int32 level);
		bool QuitBattleField(MtPlayer* player);
	private:
		std::map<int64, MtBattleField*> room_map_;
		std::map<int32, int32> level_config_;
		std::map<int32, boost::shared_ptr<Packet::BFConfig>> bf_config_;
		std::mutex mutex_;
		object_pool_ex<MtBattleField> room_pool_{ 200 };
	};
}
#endif // MT_BATTLE_ROOM_MANAGER_H__
