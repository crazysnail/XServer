#ifndef ZXERO_GAMESERVER_MT_ZONE_H__
#define ZXERO_GAMESERVER_MT_ZONE_H__

#include "../mt_types.h"


namespace Mt
{
	using namespace zxero;
	class MtScene;
	class MtPlayer;
	
	class MtZone : public boost::noncopyable, public boost::enable_shared_from_this<MtZone>
	{
	public:
		enum class ZoneSize {
			WIDTH = 10000,
			HEIGHT = 10000,
		};
		MtZone(zxero::int32 zone_id, boost::shared_ptr<MtScene> scene_);
		~MtZone();
		bool OnPlayerEnter(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<MtZone> previous_zone = nullptr);
		bool OnPlayerLeave(boost::shared_ptr<MtPlayer> player_);
		bool BroadcastMessage(const google::protobuf::Message& message, const std::vector<uint64>& except_guids = {});
		bool BroadcastMessageRelateZone(const google::protobuf::Message& message, const std::vector<uint64>& except_guids = {});
		int32 GetZoneId();
		void FillZonePlayers(Packet::ScenePlayerList& player_list);
		void EnumPlayer(boost::function<void(boost::shared_ptr<MtPlayer>&)> functor);
		bool OnPlayerReenter(const boost::shared_ptr<MtPlayer>& player);
	private:
		int32 zone_id_;
		boost::shared_ptr<MtScene> scene_;
		std::map<uint64, boost::shared_ptr<MtPlayer>> player_map_;
	};
}

#endif // ZXERO_GAMESERVER_MT_ZONE_H__