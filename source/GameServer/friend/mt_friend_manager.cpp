#include <FriendMail.pb.h>
#include "mt_friend_manager.h"
#include "../mt_player/mt_player.h"
#include "utils.h"
#include "module.h"
#include "active_model.h"
#include "../base/client_session.h"
#include "../mt_cache/mt_shm_manager.h"
#include "../mt_guid/mt_guid.h"

namespace Mt
{
	static MtFriendManager* __mt_friend_manager = nullptr;
	REGISTER_MODULE(MtFriendManager)
	{
		require("data_manager");
		//register_load_function(module_base::STAGE_LOAD, boost::bind(&MtGuildManager::OnLoad, boost::ref(MtGuildManager::Instance())));
		
	}

	void MtFriendManager::lua_reg(lua_State* ls)
	{
		ff::fflua_register_t<MtFriendManager, ctor()>(ls, "MtFriendManager")
			.def(&MtFriendManager::GetPlayerZone, "GetPlayerZone");
		ff::fflua_register_t<>(ls)
			.def(&MtFriendManager::Instance, "LuaMtFriendManager");
	}
	MtFriendManager& MtFriendManager::Instance()
	{
		return *__mt_friend_manager;
	}

	MtFriendManager::MtFriendManager()
	{
		__mt_friend_manager = this;
	}

	MtFriendManager::~MtFriendManager()
	{
	}

	int32 MtFriendManager::OnLoad()
	{
		return 0;
	}

	void MtFriendManager::OnNewDay()
	{
		for (auto info : playerzone_m_)
		{
			int32 celldata = info.second->zone_data_->celldata();
			utils::bit_reset(celldata, Packet::ZoneCellIndex::AddPopularity);
			info.second->zone_data_->set_celldata(celldata);
		}
	}

	void MtFriendManager::Fill_ViewZoneInfo_BaseInfo(Packet::ViewZoneReply& msg, const Packet::PlayerBasicInfo* baseinfo)
	{
		if (baseinfo == nullptr)
			return;
		auto vzbi = msg.mutable_base_info();
		if (vzbi)
		{
			vzbi->set_player_guid(baseinfo->guid());
			vzbi->set_player_name(baseinfo->name());
			vzbi->set_player_hair(baseinfo->hair());
			vzbi->set_init_actor_id(baseinfo->init_actor_id());
			vzbi->set_level(baseinfo->level());
			vzbi->set_guildname(baseinfo->guildname());
		}
	}
	void MtFriendManager::Fill_ViewZoneInfo_Player(Packet::ViewZoneReply& msg, MtPlayer* player, const PlayerZone* zoneinfo)
	{
		if (player == nullptr || zoneinfo == nullptr)
			return;
		Fill_ViewZoneInfo_BaseInfo(msg, player->GetScenePlayerInfo());
		player->fill_ViewZoneInfoEquip(msg);
		zoneinfo->fill_zone_protocal(msg);
	}
	void MtFriendManager::Fill_ViewZoneInfo_SceneInfo(Packet::ViewZoneReply& msg, const Packet::ScenePlayerInfo* sceneinfo, const PlayerZone* zoneinfo)
	{
		if (sceneinfo == nullptr || zoneinfo == nullptr)
			return;
		Fill_ViewZoneInfo_BaseInfo(msg, &sceneinfo->basic_info());
		for (auto equip : sceneinfo->main_equips())
		{
			msg.add_main_equips()->CopyFrom(equip);
		}
		zoneinfo->fill_zone_protocal(msg);
	}
	void MtFriendManager::Fill_FindFriendInfo_SceneInfo(Packet::FindPlayerInfo& msg, const Packet::PlayerBasicInfo* baseinfo)
	{
		if (baseinfo == nullptr )
			return;
		msg.set_guid(baseinfo->guid());
		msg.set_name(baseinfo->name());
		msg.set_hair(baseinfo->hair());
		msg.set_init_actor_id(baseinfo->init_actor_id());
		msg.set_level(baseinfo->level());
	}
	void MtFriendManager::Fill_FriendMessage(Packet::FriendMessageInfo& msg, Packet::FriendMessageType type, uint64 receive_guid, uint64 send_guid, std::string content, int32 chat_time)
	{
		msg.set_type(type);
		msg.set_receive_guid(receive_guid);
		msg.set_send_guid(send_guid);
		msg.set_content(content);
		msg.set_chat_time(chat_time);
		msg.set_datetime(now_second());
	}
	void MtFriendManager::Fill_MessagePlayerInfo_MessageList(Packet::MessagePlayerInfo& msg, boost::shared_ptr<Packet::MessageListPlayerInfo> list)
	{
		uint64 playerguid = list->msg_player_guid();
		msg.set_source_guid(playerguid);
		msg.set_source_name(list->msg_player_name());
		msg.set_source_hair(list->msg_player_hair());
		msg.set_level(list->msg_player_level());

		auto zone = MtFriendManager::Instance().GetPlayerZone(playerguid);
		if (zone)
		{
			msg.set_signature(zone->get_zone_signature());
		}
		else
		{
			msg.set_signature("");
		}

		auto notify_player_ = Server::Instance().FindPlayer(playerguid);
		if (notify_player_ != nullptr)
		{
			MtFriendManager::Fill_MessagePlayerInfo_BaseInfo(msg, notify_player_->GetScenePlayerInfo());
			msg.set_teamid(notify_player_->GetTeamID());
			msg.set_online(notify_player_->Online());
		}
		else
		{
			msg.set_teamid(INVALID_GUID);
			msg.set_online(false);
			auto notify_player_scene = Server::Instance().FindPlayerSceneInfo(playerguid);
			if (notify_player_scene)
			{
				MtFriendManager::Fill_MessagePlayerInfo_BaseInfo(msg, &notify_player_scene->basic_info());
			}
		}
	}
	void MtFriendManager::Fill_MessagePlayerInfo_BaseInfo(Packet::MessagePlayerInfo& msg, const Packet::PlayerBasicInfo* baseinfo)
	{
		if (baseinfo == nullptr)
			return;
		msg.set_init_actor_id(baseinfo->init_actor_id());
		msg.set_havenew(true);
		msg.set_level(baseinfo->level());
		msg.set_guildname(baseinfo->guildname());
	}
	void MtFriendManager::Fill_FriendInfot_PlayerZone(Packet::FriendInfotReply &reply, uint64 playerid)
	{
		auto playerzoneinfo = MtFriendManager::Instance().GetPlayerZone(playerid);
		if (playerzoneinfo)
		{
			playerzoneinfo->fill_friendinfo_protocol(reply);
		}
		else
		{
			auto zi = reply.mutable_zoneinfo();
			zi->set_guid(playerid);
			zi->set_signature("");
			zi->set_popularity(0);
			zi->set_celldata(0);
		}
	}
	void MtFriendManager::OnCacheAll()
	{
		for (auto zone : playerzone_m_)
		{
			zone.second->OnCacheAll();
		}

		std::set<google::protobuf::Message *> msg_set;
		for (auto msg : offlinefriendmessage)
		{
			msg_set.insert(msg.get());
		}

		for (auto info : offlinemessagelistinfo)
		{
			msg_set.insert(info.get());
		}
		
		MtShmManager::Instance().Cache2Shm(msg_set, "FriendMessage");
		offlinefriendmessage.clear();
		offlinemessagelistinfo.clear();
	}
	
	boost::shared_ptr<PlayerZone> MtFriendManager::GetPlayerZone(uint64 playerid)
	{
		auto info = playerzone_m_.find(playerid);
		if (info != playerzone_m_.end())
		{
			return info->second;
		}
		return nullptr;
	}
	void MtFriendManager::AddZoneInfo(uint64 playerid, boost::shared_ptr<PlayerZone> info)
	{
		auto getinfo = GetPlayerZone(playerid);
		if (getinfo == nullptr)
		{
			playerzone_m_.insert(std::make_pair(playerid, info));
		}
		//else
		//{
		//	getinfo->zone_data_->CopyFrom(*info->zone_data_.get());
		//	getinfo->zone_data_->ClearDirty();
		//	getinfo->messageboardlist_.clear();
		//	for (auto message : info->messageboardlist_)
		//	{
		//		getinfo->messageboardlist_.push_back(message);
		//		message->ClearDirty();
		//	}
		//}
	}

	void MtFriendManager::AddFriendMessageInfo(const boost::shared_ptr<Packet::FriendMessageInfo>& msg)
	{
		if (msg)
		{
			offlinefriendmessage.push_back(msg);
		}
	}

	boost::shared_ptr<Packet::MessageListPlayerInfo> MtFriendManager::CreateMessageListPlayerInfo(uint64 playerid, const Packet::PlayerBasicInfo* targetinfo)
	{
		if (targetinfo == nullptr)
			return nullptr;
		boost::shared_ptr<Packet::MessageListPlayerInfo> info = boost::make_shared<Packet::MessageListPlayerInfo>();
		if (info)
		{
			info->set_player_guid(playerid);
			info->set_msg_player_guid(targetinfo->guid());
			info->set_msg_player_hair(targetinfo->hair());
			info->set_msg_player_level(targetinfo->level());
			info->set_msg_player_name(targetinfo->name());
		}
		return info;
	}
	void MtFriendManager::AddMessageListPlayerInfo(const boost::shared_ptr<Packet::MessageListPlayerInfo>& info)
	{
		if (info)
		{
			auto it = std::find_if(std::begin(offlinemessagelistinfo), std::end(offlinemessagelistinfo), [=](auto& iter)
			{
				return (iter->player_guid() == info->player_guid() && iter->msg_player_guid() == info->msg_player_guid());
			});
			if (it == std::end(offlinemessagelistinfo))
			{
				offlinemessagelistinfo.push_back(info);
			}
		}
	}

	bool MtFriendManager::OnFriendMessageReq(const boost::shared_ptr<client_session>& session, const boost::shared_ptr<Packet::FriendMessageReq>& req, int32 /*source*/)
	{
		auto player = session->player();
		if (player == nullptr) {
			ZXERO_ASSERT(false);
			return true;
		}
		uint64 targetguid = req->player_guid();
		std::string chatmessage = req->chat_message();
		int32 chattime = req->chat_time();

		player->OnFriendMessageReq(Packet::FriendMessageType::FriendM, targetguid, chatmessage, chattime);
		return true;
	}
	//PlayerZone
	void PlayerZone::lua_reg(lua_State* ls)
	{
		ff::fflua_register_t<PlayerZone, ctor()>(ls, "PlayerZone")
			.def(&PlayerZone::GetZoneInfo, "GetZoneInfo")
			.def(&PlayerZone::UpdateSign, "UpdateSign")
			.def(&PlayerZone::AddMessageBoard, "AddMessageBoard");
	}

	PlayerZone::PlayerZone()
	{
		zone_data_ = boost::make_shared<Packet::ZoneInfo>();
		zone_data_->set_signature("");
		zone_data_->set_guid(INVALID_GUID);
		zone_data_->set_popularity(0);
		zone_data_->set_celldata(0);
		for (int32 i = 0; i < 4; ++i)
		{
			zone_data_->add_signid(0);
		}
	}
	PlayerZone::~PlayerZone()
	{

	}
	void PlayerZone::OnCacheAll()
	{
		std::set<google::protobuf::Message *> msg_set;
		msg_set.insert(zone_data_.get());
		//
		for (auto board : messageboardlist_) {
			msg_set.insert(board.get());
		}
		MtShmManager::Instance().Cache2Shm(msg_set, "PlayerZone");
	}
	void PlayerZone::fill_friendinfo_protocol(Packet::FriendInfotReply &reply)
	{
		reply.mutable_zoneinfo()->CopyFrom(*zone_data_);
		for (auto msg : messageboardlist_)
		{
			auto rm = reply.add_msgs();
			rm->CopyFrom(*msg.get());
		}
	}
	void PlayerZone::fill_zone_protocal(Packet::ViewZoneReply &reply) const
	{
		reply.mutable_zoneinfo()->CopyFrom(*zone_data_.get());
		for (auto msg : messageboardlist_)
		{
			auto rm = reply.add_msgs();
			rm->CopyFrom(*msg.get());
		}
	}
	boost::shared_ptr<Packet::ZoneInfo> PlayerZone::GetZoneInfo()
	{
		return zone_data_;
	}

	std::string PlayerZone::get_zone_signature()
	{
		return zone_data_->signature();
	}

	void PlayerZone::set_zone_signature(std::string signature)
	{
		zone_data_->set_signature(signature);
	}

	void PlayerZone::UpdateSign(MtPlayer* player)
	{
		if (player && zone_data_)
		{
			Packet::UpdateSign update;
			for (auto id : zone_data_->signid())
			{
				update.add_id(id);
			}
			player->SendMessage(update);
		}
	}
	void PlayerZone::AddMessageBoard(uint64 owerplayerid, MtPlayer* targetplayer, std::string replyplayer, std::string message)
	{
		if (targetplayer == nullptr )
			return;
		if (messageboardlist_.size() > 100)
		{
			return;
		}
		boost::shared_ptr<Packet::MessageBoard> m = boost::make_shared<Packet::MessageBoard>();
		if (m == nullptr)
			return;
		m->set_guid(MtGuid::Instance()(*m));
		m->set_owerid(owerplayerid);
		m->set_playerid(targetplayer->Guid());
		m->set_playername(targetplayer->Name());
		m->set_playerhair(targetplayer->GetScenePlayerInfo()->hair());
		m->set_playerlevel(targetplayer->PlayerLevel());
		m->set_replyplayer(replyplayer);
		m->set_message(message);
		m->set_date(MtTimerManager::Instance().CurrentDate());
		messageboardlist_.push_back(m);
		if (targetplayer->Guid() == owerplayerid) //如果回复自己
		{
			Packet::AddMessageBoard add;
			add.mutable_addmsg()->CopyFrom(*m.get());
			add.set_self(true);
			targetplayer->SendMessage(add);
		}
		else//回复他人，给自己回复一个false 给他人回复一个true
		{
			Packet::AddMessageBoard add;
			add.mutable_addmsg()->CopyFrom(*m.get());
			add.set_self(false);
			targetplayer->SendMessage(add);

			int32 celldata = zone_data_->celldata();
			bool c = utils::bit_check(celldata, Packet::ZoneCellIndex::AddPopularity);
			if (c == false)
			{
				utils::bit_set(celldata, Packet::ZoneCellIndex::AddPopularity);
				zone_data_->set_celldata(celldata);
				zone_data_->set_popularity((zone_data_->popularity() + 1));
			}

			auto owerplayer = Server::Instance().FindPlayer(owerplayerid);
			if (owerplayer)
			{
				Packet::AddMessageBoard sadd;
				sadd.mutable_addmsg()->CopyFrom(*m.get());
				sadd.set_self(true);
				owerplayer->SendMessage(sadd);
				owerplayer->SendCommonReply("UpdatePopularity", { zone_data_->popularity() }, {}, {});
			}
		}
	}
}