#include "ClientMove.pb.h"	
#include "mt_zone.h"
#include "mt_scene.h"
#include "../mt_player/mt_player.h"
#include "log.h"
#include <boost/range/algorithm/set_algorithm.hpp>

namespace Mt
{
		const static int32 MAX_ZONE_SYNC_PLAYERS = 50;
		MtZone::~MtZone()
		{
			player_map_.clear();
		}

		MtZone::MtZone(int32 zone_id, boost::shared_ptr<MtScene> scene)
			:zone_id_(zone_id), scene_(scene)
		{

		}

		bool MtZone::OnPlayerReenter(const boost::shared_ptr<MtPlayer>& player)
		{
			player_map_.erase(player->Guid());
			auto near_zones = scene_->GetRelatedZone(shared_from_this());
			Packet::ScenePlayerList msg;
			std::for_each(std::begin(near_zones), std::end(near_zones), [&](auto& zone) {
				zone->FillZonePlayers(msg);
			});
			player->ResetRobotPos();
			player->FillRobotPlayer(msg);
			player_map_.insert(std::make_pair(player->Guid(), player));
			player->SendMessage(msg);
			return true;
		}

		bool MtZone::OnPlayerEnter(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<MtZone> previous_zone /*= nullptr*/)
		{
			ZXERO_ASSERT(player_map_.find(player->Guid()) == player_map_.end()) << "player enter same zone";
			auto current_zones = scene_->GetRelatedZone(this->shared_from_this());
			std::vector<boost::shared_ptr<MtZone>> previous_zones;
			if (previous_zone != nullptr) {
				previous_zones = scene_->GetRelatedZone(previous_zone);
				ZXERO_ASSERT(previous_zone->OnPlayerLeave(player)) << "Leave Zone error, no such player " << player->Guid() << ":" << player->Name();
			}
			std::vector<boost::shared_ptr<MtZone>> new_zones;
			boost::set_difference(current_zones, previous_zones, std::back_inserter(new_zones),
				[](auto& left, auto& right) {return left->GetZoneId() < right->GetZoneId(); });
			std::vector<boost::shared_ptr<MtZone>> old_zones;
			boost::set_difference(previous_zones, current_zones, std::back_inserter(old_zones),
				[](auto& left, auto& right) {return left->GetZoneId() < right->GetZoneId(); });


			std::vector<uint64> dummy;
			//通知附近区域, 有玩家上线了
			Packet::ScenePlayerInfo msg;
			player->FillScenePlayerInfo(msg);
			boost::for_each(new_zones, boost::bind(&MtZone::BroadcastMessage, _1, boost::ref(msg), boost::cref(dummy)));

			//取周边区域玩家信息, 发给进入player_
			Packet::ScenePlayerList player_lists;
			boost::for_each(new_zones, boost::bind(&MtZone::FillZonePlayers, _1, boost::ref(player_lists)));
			if (previous_zone == nullptr) {//初次进入场景, 构造一下机器人
				player->ResetRobotPos();
				player->FillRobotPlayer(player_lists);
			}
			player->SendMessage(player_lists);

			//通知老的区域, 玩家离开了
			Packet::PlayerLeaveZone leave_notify;
			leave_notify.set_guid(player->Guid());
			boost::for_each(old_zones, boost::bind(&MtZone::BroadcastMessage, _1, boost::ref(leave_notify), boost::cref(dummy)));

			//通知玩家, 删除上个区域的玩家
			auto cb = [&](const boost::shared_ptr<MtPlayer>& scene_player) {
				leave_notify.set_guid(scene_player->Guid());
				player->SendMessage(leave_notify);
			};
			std::for_each(std::begin(old_zones), std::end(old_zones), [&](auto& zone) {
				zone->EnumPlayer(cb);
			});

			player_map_.insert(std::make_pair(player->Guid(), player));
			player->Zone(shared_from_this());
			return true;
		}

		bool MtZone::OnPlayerLeave(boost::shared_ptr<MtPlayer> player_)
		{
			if (player_map_.find(player_->Guid()) == player_map_.end())
				return false;
			
			Packet::PlayerLeaveZone leave_notify;
			leave_notify.set_guid(player_->Guid());
			player_->SendMessage(leave_notify);

			player_map_.erase(player_->Guid());
			player_->Zone(nullptr);

			return true;
		}

		bool MtZone::BroadcastMessage(const google::protobuf::Message& message, const std::vector<uint64>& except_guids)
		{
			for (auto it : player_map_) {
				if (std::any_of(std::begin(except_guids), std::end(except_guids), [&](auto guid) {return guid == it.first; })) {
					continue;
				}
				it.second->SendMessage(message);
			}
			return true;
		}

		zxero::int32 MtZone::GetZoneId()
		{
			return zone_id_;
		}

		bool MtZone::BroadcastMessageRelateZone(const google::protobuf::Message& message_, const std::vector<uint64>& except_guids)
		{
			return scene_->BroadcastMessageRelatedZones(this->shared_from_this(), message_, except_guids);
		}

		void MtZone::FillZonePlayers(Packet::ScenePlayerList& player_list)
		{
			if (player_list.player_list_size() >= MAX_ZONE_SYNC_PLAYERS) {
				return;
			}
			std::for_each(std::begin(player_map_), std::end(player_map_), [&](auto it) {
				if (player_list.player_list_size() >= MAX_ZONE_SYNC_PLAYERS) {
					return;
				}
				auto list_ele = player_list.add_player_list();
				it.second->FillScenePlayerInfo(*list_ele);
			});
			return;
		}

		void MtZone::EnumPlayer(boost::function<void(boost::shared_ptr<MtPlayer>&)> functor) {
			std::for_each(std::begin(player_map_), std::end(player_map_), [&](auto it) {
				functor(it.second);
			});
		}
}
