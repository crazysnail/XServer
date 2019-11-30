#ifndef MTONLINE_GAMESERVER_MT_LOGIC_AREA_H__
#define MTONLINE_GAMESERVER_MT_LOGIC_AREA_H__
#include "../mt_types.h"
#include "mt_zone.h"
#include "mt_raid.h"
#include <EnterScene.pb.h>
namespace Mt
{
	class MtRaid;
	class LogicArea
	{
	public:
		enum AreaType {
			INVALID,
			ZONE,
			RAID,
		};
		LogicArea() :zone_(nullptr), raid_(nullptr), type_(AreaType::INVALID) {}
		LogicArea(const boost::shared_ptr<MtZone>& zone) 
			:zone_(zone),type_(AreaType::ZONE){}
		LogicArea(const boost::shared_ptr<MtRaid>& raid) 
			:raid_(raid), type_(AreaType::RAID) {}
		bool BroadCastMessage(const google::protobuf::Message& msg, const std::vector<uint64>& except_guids = {}) const
		{
			if (type_ == AreaType::ZONE && zone_) {
				return zone_->BroadcastMessageRelateZone(msg, except_guids);
			}
			if (type_ == AreaType::RAID && raid_) {
				return raid_->BroadCastMessage(msg, except_guids);
			}
			return false;
		}
		bool OnPlayerReenter(const boost::shared_ptr<MtPlayer>& player) const
		{
			if (type_ == AreaType::ZONE && zone_) {
				return zone_->OnPlayerReenter(player);
			}
			if (type_ == AreaType::RAID && raid_) {
				raid_->SyncInfo(player);
				return true;
			}
			return false;
		}
		bool OnPlayerLeave(const boost::shared_ptr<MtPlayer>& player)
		{
			if (type_ == AreaType::ZONE && zone_) {
				return zone_->OnPlayerLeave(player);
			}
			if (type_ == AreaType::RAID && raid_) {
				raid_->OnLeave(player);
				return true;
			}
			return false;
		}
		bool OnPlayerEnter(const boost::shared_ptr<MtPlayer>& player,
			const LogicArea* previous_area,
			const boost::shared_ptr<Packet::PlayerEnterScene>& message) const
		{
			if (type_ == AreaType::ZONE && zone_) {
				if (previous_area == nullptr || previous_area->Empty()) {
					return zone_->OnPlayerEnter(player);
				} else {
					return zone_->OnPlayerEnter(player, previous_area->zone_);
				}
			}
			if (type_ == AreaType::RAID && raid_) {
				if (message == nullptr) {
					return false;
				}
				int32 camp = -1;
				if (message->has_camp()) {
					camp = message->camp();
				}
				raid_->OnEnter(player, camp);
				return true;
			}
			return false;
		}
		bool EqualTo(const LogicArea& rhs) const
		{
			switch (type_)
			{
			case AreaType::ZONE:
				return zone_ != nullptr && rhs.zone_ != nullptr && zone_->GetZoneId() == rhs.zone_->GetZoneId();
			case AreaType::RAID:
				return true;
			default:
				break;
			}
			return false;
		}
		bool Empty() const
		{
			switch (type_)
			{
			case AreaType::ZONE:
				return zone_ == nullptr;
			case AreaType::RAID:
				return raid_ == nullptr;
			default:
				break;
			}
			return true;
		}
	private:
		const boost::shared_ptr<MtZone> zone_;
		const boost::shared_ptr<MtRaid> raid_;
		const AreaType type_ = AreaType::INVALID;
	};
}
#endif // MTONLINE_GAMESERVER_MT_LOGIC_AREA_H__
