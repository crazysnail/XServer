#include "mt_copy_scene.h"
#include "../mt_player/mt_player.h"
#include "mt_raid.h"
#include "../mt_team/mt_team_manager.h"
#include "mt_logic_area.h"
#include <EnterScene.pb.h>
#include <S2GMessage.pb.h>
#include <boost/make_shared.hpp>

namespace Mt
{
	bool MtCopyScene::BroadcastMessage(const google::protobuf::Message& message, const std::vector<uint64>& except_guids)
	{
		except_guids;
		for (auto raid : raids_)
		{
			raid.second->BroadCastMessage(message);
		}
		return true;
	}

	bool MtCopyScene::OnPlayerEnterBegin(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<Packet::PlayerEnterScene>& message)
	{
		if (!message->has_rtid() && !message->has_script_id() && !message->has_portal_pos()) {
			ZXERO_ASSERT(false) << "invalid enter raid scene message! scene id=" << message->scene_id();
			return false;
		}
		int32 rtid = message->rtid();
		//直接新建副本
		if (rtid == INVALID_GUID) {

			RaidHelper helper;
			helper.scene_id_ = SceneId();
			helper.portal_pos_ = message->portal_pos();
			helper.script_id_ = message->script_id();
			helper.portal_scene_id_ = message->pre_scene_id();

			rtid = AddRaid(helper);
			//透传给队员进副本用！！有点奇怪，但先这样用吧
			message->set_rtid(rtid);
		}
		auto raid = GetRaid(rtid);
		if (raid == nullptr) {
			ZXERO_ASSERT(false) << "invalid raid rtid = " << message->rtid() << " player guid=" << player->Guid();
			OnPlayerLeave(player);
			return false;
		}
		return true;
	}

	bool MtCopyScene::OnPlayerEnterEnd(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<Packet::PlayerEnterScene>& message)
	{
		if (player->GetTeamID() != INVALID_GUID) {//同步给队友
			auto member_change_scene = boost::make_shared<S2G::TeamMemberChangeScene>();
			member_change_scene->set_scene_id(player->GetSceneId());
			member_change_scene->set_script_id(message->script_id());
			member_change_scene->set_rtid(message->rtid());
			member_change_scene->set_camp(message->camp());
			if (message->has_init_pos()) {
				member_change_scene->mutable_init_pos()->CopyFrom(message->init_pos());
			}
			else {
				//取队长位置
				member_change_scene->mutable_init_pos()->CopyFrom(player->Position());
			}
			Server::Instance().SendMessage(member_change_scene, player);
		}

		return true;
	}

	bool MtCopyScene::Transport(const boost::shared_ptr<MtPlayer>& player, const Packet::Position & init_pos)
	{
		player->Direction(0.f);
		player->SetMoveSpeed(4000);
		player->GetScenePlayerInfo()->set_player_status(Packet::PlayerSceneStatus::IDEL);
		player->Position(init_pos);

		if (!ValidPos(player->Position())) {
			ZXERO_ASSERT(false) << "invalid position! scene id=" << config_->id() << " player guid=" << player->Guid();
			player->Position(config_->spawn_pos(0));
		}

		if (player->GetRaid()) {
			player->GetRaid()->SyncInfo(player);
		}

		if (player->GetTeamID() != INVALID_GUID) {//同步给队友
			auto member_change_scene = boost::make_shared<S2G::TeamMemberChangeScene>();
			member_change_scene->set_scene_id(player->GetSceneId());
			member_change_scene->mutable_init_pos()->CopyFrom(init_pos);
			Server::Instance().SendMessage(member_change_scene, player);
		}

		return true;
	}

	LogicArea MtCopyScene::GetInitLogicArea( const boost::shared_ptr<Packet::PlayerEnterScene>& message )
	{
		auto raid = GetRaid(message->rtid());
		if( raid != nullptr  && raid->Scene().get() == this){
			return LogicArea(raid);
		}
		else {
			return LogicArea();
		}
	}

	LogicArea MtCopyScene::GetPlayerLogicArea(const boost::shared_ptr<MtPlayer>& player)
	{
		if (player == nullptr) {
			return LogicArea();
		}
		auto raid = player->GetRaid();
		if (raid != nullptr  && raid->Scene().get() == this) {
			return LogicArea(raid);
		}
		else {
			return LogicArea();
		}
	}

	Mt::LogicArea MtCopyScene::GetCurrentLogicArea(const boost::shared_ptr<MtPlayer>& player)
	{
		return GetPlayerLogicArea(player);
	}

}