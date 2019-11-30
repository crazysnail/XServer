#ifndef MTFRIEND_MANAGER_H__
#define MTFRIEND_MANAGER_H__

#include "../mt_types.h"
namespace Mt
{
	class PlayerZone
	{
	public:
		static void lua_reg(lua_State* ls);
	public:
		PlayerZone();
		~PlayerZone();
		void OnCacheAll();
	public:
		void fill_friendinfo_protocol(Packet::FriendInfotReply &reply);
		void fill_zone_protocal(Packet::ViewZoneReply &reply) const ;
		boost::shared_ptr<Packet::ZoneInfo> GetZoneInfo();
		std::string get_zone_signature();
		void set_zone_signature(std::string signature);
		void UpdateSign(MtPlayer* player);
		void AddMessageBoard(uint64 owerplayerid, MtPlayer* targetplayer, std::string replyplayer, std::string message);
	public:
		boost::shared_ptr<Packet::ZoneInfo> zone_data_;
		std::list<boost::shared_ptr<Packet::MessageBoard>> messageboardlist_;
	};

	class MtFriendManager : public boost::noncopyable
	{
	public:
		static void lua_reg(lua_State* ls);
	public:
		static MtFriendManager& Instance();
		int32	OnLoad();
		MtFriendManager();
		~MtFriendManager();

		void OnCacheAll();
		void OnNewDay();
	public:
		static void Fill_ViewZoneInfo_BaseInfo(Packet::ViewZoneReply& msg, const Packet::PlayerBasicInfo* baseinfo);
		static void Fill_ViewZoneInfo_Player(Packet::ViewZoneReply& msg, MtPlayer* player, const PlayerZone* zoneinfo);
		static void Fill_ViewZoneInfo_SceneInfo(Packet::ViewZoneReply& msg, const Packet::ScenePlayerInfo* sceneinfo, const PlayerZone* zoneinfo);
		static void Fill_FindFriendInfo_SceneInfo(Packet::FindPlayerInfo& msg, const Packet::PlayerBasicInfo* baseinfo);
		static void Fill_FriendMessage(Packet::FriendMessageInfo& msg, Packet::FriendMessageType type, uint64 receive_guid, uint64 send_guid, std::string content, int32 chat_time);
		static void Fill_MessagePlayerInfo_MessageList(Packet::MessagePlayerInfo& msg,boost::shared_ptr<Packet::MessageListPlayerInfo> list);
		static void Fill_MessagePlayerInfo_BaseInfo(Packet::MessagePlayerInfo& msg, const Packet::PlayerBasicInfo* baseinfo);
		void Fill_FriendInfot_PlayerZone(Packet::FriendInfotReply &reply,uint64 playerid);
	public:
		bool OnFriendMessageReq(const boost::shared_ptr<client_session>& session, const boost::shared_ptr<Packet::FriendMessageReq>& req, int32 /*source*/);

		boost::shared_ptr<PlayerZone> GetPlayerZone(uint64 playerid);
		void AddZoneInfo(uint64 playerid,boost::shared_ptr<PlayerZone>);
		void AddFriendMessageInfo(const boost::shared_ptr<Packet::FriendMessageInfo>& msg);
		boost::shared_ptr<Packet::MessageListPlayerInfo> CreateMessageListPlayerInfo(uint64 playerid, const Packet::PlayerBasicInfo* targetinfo);
		void AddMessageListPlayerInfo(const boost::shared_ptr<Packet::MessageListPlayerInfo>& info);
	private:
		std::map<uint64, boost::shared_ptr<PlayerZone>> playerzone_m_;
		std::vector<boost::shared_ptr<Packet::FriendMessageInfo>> offlinefriendmessage;
		std::vector<boost::shared_ptr<Packet::MessageListPlayerInfo>> offlinemessagelistinfo;
	};
}
#endif