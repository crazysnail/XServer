#ifndef ZXERO_GAMESERVER_MT_NPC_H__
#define ZXERO_GAMESERVER_MT_NPC_H__

#include "../mt_types.h"
#include "SceneObject.pb.h"

namespace Mt 
{

	class MtNpc : public boost::noncopyable, public boost::enable_shared_from_this<MtNpc>
	{
	public:
		static void lua_reg(lua_State* ls);
	public:
		MtNpc(	const boost::shared_ptr<Packet::NpcInfo>& npc_info, 
			const boost::shared_ptr<Packet::NpcDynamicConfig> npc_dynamic,
			const boost::shared_ptr<Packet::SceneLayout> pos_config,
			const int32 relate_mission_id=-1 );

		MtNpc();
		virtual ~MtNpc();

		bool IsLock() { 
			return lock_; 
		};
		void Lock(bool lock) { 
			lock_ = lock;
		};

		void OnBigTick(int32 delta) {
			auto lifetime = npc_info_->lifetime() - delta;
			npc_info_->set_lifetime(lifetime);
		}
		bool IsDead() {
			return npc_info_->lifetime() < 0;
		}
		int32 MissionId() { 
			return relate_mission_id_;
		}
		const boost::shared_ptr<Packet::NpcInfo> NpcInfo() {
			return npc_info_;
		}
		const boost::shared_ptr<Packet::NpcDynamicConfig> NpcDynamicConfig() {
			return npc_dynamic_;
		}
		const boost::shared_ptr<Packet::SceneLayout> PositionConfig() {
			return pos_config_;
		}

	private:
		const	boost::shared_ptr<Packet::NpcInfo> npc_info_;
		const	boost::shared_ptr<Packet::NpcDynamicConfig> npc_dynamic_;
		const	boost::shared_ptr<Packet::SceneLayout> pos_config_;
		int32	relate_mission_id_ = -1;
		bool	lock_ = false;
	};
}
#endif // ZXERO_GAMESERVER_MT_NPC_H__
