
#ifndef MTONLINE_GAMESERVER_MT_RAID_H__
#define MTONLINE_GAMESERVER_MT_RAID_H__
#include "../mt_types.h"
#include <Base.pb.h>
#include <SceneCommon.pb.h>

namespace Mt
{
	struct RaidHelper {
		int32 scene_id_;
		Packet::Position portal_pos_;
		int32 script_id_;
		int32 portal_scene_id_;		
	};
	class MtNpc;

	class MtRaid : public boost::noncopyable, public boost::enable_shared_from_this<MtRaid>
	{
	public:
		static void lua_reg(lua_State* ls);
	public:
		MtRaid() {};
		MtRaid(const RaidHelper & helper, const int32 rtid);
		virtual ~MtRaid() {};

		void OnTick(zxero::uint64 elapseTime);
		void OnBigTick(zxero::uint64 elapseTime);
		void OnSecondTick();
		void OnMinTick();
		const boost::shared_ptr<MtPlayer> GetOwner() const;
		bool AddPlayer(const boost::shared_ptr<MtPlayer> player, const int32 camp);
		bool DelPlayer(const boost::shared_ptr<MtPlayer> player);
		void OnEnter(const boost::shared_ptr<MtPlayer> player, const int32 camp);
		void OnLeave(const boost::shared_ptr<MtPlayer> player);
		bool BroadCastMessage(const google::protobuf::Message& msg, const std::vector<uint64>& except_guid = {});
		void BroadCastLuaPacket(const std::string &name, const std::vector<int32>& int32values, const std::vector<uint64>& int64values, const std::vector<std::string>& stringvalues);
		void OnDestroy(); //销毁副本
		uint32 RegenBeginDate();

		bool CanDestroy() { 
			return state_== Packet::RaidState::ToDestroy; 
		}
		Packet::RaidState GetState() {
			return state_;
		}
		void SetState(const Packet::RaidState state) {
			state_ = state;
		}
		const int32 GetOldSceneId() {
			return old_scene_id_;
		}
		int32 GetScriptId() {
			return script_id_;
		}
		const Packet::Position & GetPortalPos() {
			return old_portal_pos_;
		}
		uint32 GetBeginDate() { 
			return begin_date_;
		};
		const boost::shared_ptr<MtScene>& Scene() const {
			return scene_;
		}
		int32 RuntimeId() const {
			return runtime_id_;
		}

		const std::vector<boost::shared_ptr<MtPlayer>>& GetAttackers() const{
			return attackers_;
		}
		const std::vector<boost::shared_ptr<MtPlayer>>& GetDefenders() const{
			return defenders_;
		}

		void OnClientEnterOk(const boost::shared_ptr<MtPlayer>& player);
		bool IsEmpty(const int32 camp);

		//给lua用的一个很疼的接口
		Packet::BattleFieldState GetBattleFieldState();
		void OnRaidBattleOver(const uint64 winner, const uint64 losser);
		virtual void SyncInfo(const boost::shared_ptr<MtPlayer>& player);

		boost::shared_ptr<MtNpc> FindNpc(const int32 series_id);
		int32 AddNpc(const int32 pos_id, const int32 index,const int32 lifetime);
		void DelNpc(const int32 series_id);
		void PlayerEnterAddNpc(MtPlayer* player);
		void PlayerLeaveDelNpc(MtPlayer* player);
		const std::map<int32, boost::shared_ptr<MtNpc>>& GetNpcList() { return npc_map_; }
		
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
		void SetParams64(const std::string &key, int32 value) {
			params_64_[key] = value;
		}

		void OnGenBuff();
		void OnBuffPoint(MtPlayer * player, const int32 posindex);
		/////////////////////////////////////////////

		void SetRaidTimer(int32 index, int32 second, int32 times);
		boost::shared_ptr<CMyTimer> GetRaidTimer(int32 index);

	private:
		void OnRaidTimer(int32 index);
		void OnNpcRefreshTime(const int32 delta);

	protected:
		boost::shared_ptr<MtScene> scene_;

		//普通副本只有attackers，对抗pvp地图会存在defenders
		std::vector<boost::shared_ptr<MtPlayer>> attackers_;
		std::vector<boost::shared_ptr<MtPlayer>> defenders_;

		Packet::Position  old_portal_pos_;
		int32 script_id_;
		int32 old_scene_id_;	
		uint32 begin_date_;
		int32 runtime_id_;
		Packet::RaidState state_ = Packet::RaidState::RaidInit;

		std::map<int32, boost::shared_ptr<MtNpc>> npc_map_;		   //动态npc表

		std::map<int32, boost::shared_ptr<CMyTimer>> raidtimer_;

		std::map<std::string, int32> params_32_;
		std::map<std::string, int64> params_64_;

		std::map<int32, uint32> buff_list_;

	};
	
}
#endif // MTONLINE_GAMESERVER_MT_RAID_H__
