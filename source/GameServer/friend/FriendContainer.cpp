#include "../mt_player/mt_player.h"
#include "../mt_server/mt_server.h"
#include "../mt_guid/mt_guid.h"
#include "mt_friend_manager.h"
#include "../mt_cache/mt_shm_manager.h"
#include "active_model.h"
#include "../mt_chat/mt_chatmessage.h"
#include "utils.h"
#include "FriendContainer.h"


namespace Mt
{
	FriendContainer::FriendContainer(MtPlayer& player)
		:player_(player.weak_from_this())
	{
		reset();
		
	}

	FriendContainer::~FriendContainer()
	{
		reset();
	}
	void FriendContainer::lua_reg(lua_State* ls)
	{
		ls;
	}
	void FriendContainer::reset()
	{
		friendlist_.clear();
		m_backlist.clear();
	}

	bool FriendContainer::isFriend(uint64 playerguid)
	{
		auto it = std::find_if(std::begin(friendlist_), std::end(friendlist_), [=](auto& iter)
		{
			return iter->friend_guid() == playerguid;
		});
		if (it == std::end(friendlist_))
		{
			return false;
		}
		return true;
	}
	bool FriendContainer::isBlack(uint64 playerguid)
	{
		if (std::find(m_backlist.begin(), m_backlist.end(), playerguid) != m_backlist.end())
		{
			return true;
		}
		return false;
	}
	Packet::ResultCode FriendContainer::apply_add_friend(
		uint32 player_max_friend_count, uint64 playerguid,
		const Packet::PlayerBasicInfo* info)//请求添加好友
	{
		ZXERO_ASSERT(playerguid != INVALID_GUID);
		if (player_.expired()) {
			return Packet::ResultCode::InvalidPlayer;
		}
		auto player = player_.lock();
		if (info == nullptr)
		{
			return Packet::ResultCode::UnknownError;
		}
		if (friendlist_.size() >= player_max_friend_count)
		{
			return Packet::ResultCode::FRIEND_FULL;
		}

		if (playerguid == player->Guid())
		{
			return Packet::ResultCode::ADD_SELF_FRIEND;
		}

		if (isFriend(playerguid))
		{
			return Packet::ResultCode::PLAYER_IS_FRIEND;
		}
		auto friend_info = boost::make_shared<Packet::FriendPlayer>();
		friend_info->set_guid(MtGuid::Instance()(*friend_info));
		friend_info->set_player_guid(player->Guid());
		friend_info->set_friend_guid(playerguid);
		friend_info->set_friendvalue(0);
		friend_info->set_hair(info->hair());
		friend_info->set_init_actor_id(info->init_actor_id());
		friend_info->set_name(info->name());
		friend_info->set_level(info->level());
		friendlist_.push_back(friend_info);//改成单向添加好友
		return Packet::ResultCode::FRIEND_SUCCESS;
	}

	Packet::ResultCode FriendContainer::del_friend_request(uint64 playerguid)//删除好友
	{
		ZXERO_ASSERT(playerguid != INVALID_GUID);
		auto player = player_.lock();
		if (player == nullptr) {
			return Packet::ResultCode::InvalidPlayer;
		}
		if (!isFriend(playerguid))
		{
			return Packet::ResultCode::FRIEND_NOTEXIST;
		}
		for (auto iter = friendlist_.begin(); iter != friendlist_.end();)
		{
			if ((*iter)->friend_guid() == playerguid)
			{
				//标记和删除
				(*iter)->SetDeleted();
				player->Cache2Save((*iter).get());

				iter = friendlist_.erase(iter);

			}
			else
			{
				iter++;
			}
		}
		return Packet::ResultCode::ResultOK;
	}
	Packet::ResultCode FriendContainer::add_black_friend(uint64 playerguid)//把好友移入黑名单
	{
		auto player = player_.lock();
		if (player == nullptr) {
			return Packet::ResultCode::InvalidPlayer;
		}
		if (!isFriend(playerguid))
		{
			return Packet::ResultCode::FRIEND_NOTEXIST;
		}

		for (auto iter = friendlist_.begin(); iter != friendlist_.end();)
		{
			if (playerguid == (*iter)->friend_guid())
			{
				//标记和删除
				(*iter)->SetDeleted();
				player->Cache2Save((*iter).get());

				iter = friendlist_.erase(iter);
			}
			else
			{
				iter++;
			}
		}
		m_backlist.push_back(playerguid);
		return Packet::ResultCode::ResultOK;
	}

	void FriendContainer::refresh_newday()
	{
		//for (auto &f_data : m_friendlist)
		//{
		//	f_data.second.m_b_gift = false;
		//}
	}
	//===netlog===

	void FriendContainer::fill_protocol(Packet::FriendInfotReply &reply)
	{
		auto player = player_.lock();
		if (player == nullptr) {
			LOG_ERROR << "[friend container] player is null";
			return;
		}
		auto friends = reply.mutable_friends();
		fill_friend_protocol(*friends);
		MtFriendManager::Instance().Fill_FriendInfot_PlayerZone(reply, player->Guid());
		auto recommend = reply.mutable_recommends();
		ask_recommend_req(*recommend);
	}

	void FriendContainer::fill_friend_protocol(Packet::FriendListUpdate &friends)
	{
		for (auto& fr_guid : friendlist_)
		{
			auto friendinfo = friends.add_friends();
			friendinfo->set_guid(fr_guid->friend_guid());
			friendinfo->set_init_actor_id(fr_guid->init_actor_id());
			friendinfo->set_hair(fr_guid->hair());
			friendinfo->set_name(fr_guid->name());
			friendinfo->set_level(fr_guid->level());
			friendinfo->set_friendvalue(fr_guid->friendvalue());
			auto player = Server::Instance().FindPlayer(fr_guid->friend_guid());
			if (player != nullptr)
			{
				friendinfo->set_teamid(player->GetTeamID());
				friendinfo->set_online(player->Online());
				friendinfo->set_guildname(player->GetGuildName());
			}
			else
			{
				friendinfo->set_teamid(INVALID_GUID);
				friendinfo->set_online(false);
				friendinfo->set_guildname("");
			}

			auto playerzoneinfo = MtFriendManager::Instance().GetPlayerZone(fr_guid->friend_guid());
			if (playerzoneinfo)
			{
				friendinfo->set_signature(playerzoneinfo->get_zone_signature());
			}
			else
			{
				friendinfo->set_signature("");
			}
		}
	}

	void FriendContainer::ask_recommend_req(Packet::RecommendsUpdate &update)
	{
		auto player = player_.lock();
		if (player == nullptr) {
			LOG_ERROR << "[friend container] player is null";
			return;
		}
		std::vector<uint64> sampleplayer;
		std::vector<uint64> myfriendplayerlist;
		for (auto &myfriend : friendlist_)
		{
			uint64 myfriendguid = myfriend->friend_guid();
			myfriendplayerlist.push_back(myfriendguid);
			auto feiendplayer = Server::Instance().FindPlayer(myfriendguid);
			if (feiendplayer != nullptr)
			{
				for (auto &friendfriend : feiendplayer->Friend_Container()->FriendList())
				{
					uint64 friendfriendguid = friendfriend->friend_guid();
					if (friendfriendguid != player->Guid())
					{
						if (std::find(sampleplayer.begin(), sampleplayer.end(), friendfriendguid) == sampleplayer.end())
						{
							sampleplayer.push_back(friendfriendguid);
						}
					}
				}
			}
		}

		Server::Instance().ask_recommend_req(player->Guid(), sampleplayer, myfriendplayerlist);
		std::vector<uint64> outplayer;
		random_select_form_list(sampleplayer, outplayer, 5);
		for (uint64 playerguid : outplayer)
		{
			auto feiendplayer = Server::Instance().FindPlayer(playerguid);
			if (feiendplayer != nullptr)
			{
				feiendplayer->fill_friendplayerinfo_protocol(*update.add_recommends());
			}
		}
	}

	void FriendContainer::OnNewDay()
	{
	}

	
	void FriendContainer::OnLoad(const boost::shared_ptr<zxero::dbcontext>& dbctx)
	{
		auto player = player_.lock();
		if (player == nullptr) {
			LOG_ERROR << "[friend container] player is null";
			return;
		}
		auto friendlist = ActiveModel::Base<Packet::FriendPlayer>(dbctx).FindAll({ { "player_guid", player->Guid() } });
		for (auto& friend_ : friendlist) { 
			friendlist_.push_back(friend_); 
			friend_->ClearDirty();
		}
		Server::Instance().SendS2GCommonMessage("LoadZoneInfo", {}, { (int64)player->Guid() }, {});
	}

	void FriendContainer::OnCache()
	{
		auto player = player_.lock();
		if (player == nullptr) {
			LOG_ERROR << "[friend container] player is null";
			return;
		}
		//ActiveModel::Base<Packet::FriendPlayer>(dbctx).Delete({ { "player_guid", player_->Guid() } });
		for (auto &friend_ : friendlist_){
			player->Cache2Save(friend_.get());
		}
		player->Cache2Save(zone_data_.get());
		for (auto &message : messageboardlist_)
		{
			player->Cache2Save(message.get());
		}
	}
}
