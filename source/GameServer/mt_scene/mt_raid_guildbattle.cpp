#include "ClientMove.pb.h"
#include "../mt_player/mt_player.h"
#include "mt_raid_guildbattle.h"

namespace Mt
{
	MtRaidGuildBattle::MtRaidGuildBattle(const RaidHelper & helper, const int32 rtid) :MtRaid( helper,  rtid)
	{

	}
	void MtRaidGuildBattle::SyncInfo(const boost::shared_ptr<MtPlayer>& player)
	{
		//���뿪�ڽ���
		Packet::PlayerLeaveZone leave_notify;
		leave_notify.set_guid(player->Guid());
		BroadCastMessage(leave_notify);

		//֪ͨ�������������, �����˼���
		Packet::ScenePlayerInfo msg;
		player->FillScenePlayerInfo(msg);
		BroadCastMessage(msg);

		//ȡ�����˵�����
		Packet::ScenePlayerList player_lists;
		std::for_each(std::begin(attackers_), std::end(attackers_), [&](auto& team_member) {
			if (team_member->Guid() != player->Guid())
				team_member->FillScenePlayerInfo(*player_lists.add_player_list());
		});

		std::for_each(std::begin(defenders_), std::end(defenders_), [&](auto& team_member) {
			if (team_member->Guid() != player->Guid())// && team_member->IsTeamLeader()
				team_member->FillScenePlayerInfo(*player_lists.add_player_list());
		});
		player->SendMessage(player_lists);
	}
}
