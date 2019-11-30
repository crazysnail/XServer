#include "mt_npc.h"
#include "../mt_guid/mt_guid.h"
#include "../mt_player/mt_player.h"
#include "../mt_guid/mt_guid.h"

namespace Mt
{
	
	void MtNpc::lua_reg(lua_State* ls)
	{
		ff::fflua_register_t<MtNpc, ctor()>(ls, "MtNpc")
			.def(&MtNpc::MissionId, "MissionId")
			.def(&MtNpc::NpcInfo, "NpcInfo")
			.def(&MtNpc::NpcDynamicConfig, "NpcDynamicConfig")
			.def(&MtNpc::PositionConfig, "PositionConfig");
	}

	MtNpc::MtNpc()
	{

	}
	MtNpc::~MtNpc()
	{

	}

	MtNpc::MtNpc(const boost::shared_ptr<Packet::NpcInfo>& npc_info,
		const boost::shared_ptr<Packet::NpcDynamicConfig> npc_dynamic,
		const boost::shared_ptr<Packet::SceneLayout> pos_config,
		const int32 relate_mission_id)
		: npc_info_(npc_info)
		, npc_dynamic_(npc_dynamic)
		, pos_config_(pos_config)
		, relate_mission_id_(relate_mission_id)
	{
	}

	

}