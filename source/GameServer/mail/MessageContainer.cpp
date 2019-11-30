#include "date_time.h"
#include "../mt_player/mt_player.h"
#include "../mt_server/mt_server.h"
#include "../friend/FriendContainer.h"
#include "../friend/mt_friend_manager.h"
#include "../mt_guid/mt_guid.h"
#include "../mt_cache/mt_shm_manager.h"
#include "active_model.h"
#include "MessageContainer.h"
#include "static_message_manager.h"

namespace Mt
{
	MessageContainer::MessageContainer(MtPlayer& player)
		:player_(player.weak_from_this())
	{
		m_mails.clear();
		m_notify_playerinfo.clear();
	}

	MessageContainer::~MessageContainer()
	{
		clear_mails();
		m_notify_playerinfo.clear();
	}
	void MessageContainer::lua_reg(lua_State* ls)
	{
		ff::fflua_register_t<MessageContainer, ctor()>(ls, "MessageContainer")
			.def(&MessageContainer::send_notify_playerinfos, "send_notify_playerinfos");

	}
	void MessageContainer::clear_mails()
	{
		//for (auto& child:m_mails)
		//{
		//	//SAFE_DELETE( child.second);
		//}
		m_mails.clear();
	}

	void MessageContainer::OnLoad(const boost::shared_ptr<zxero::dbcontext>& dbctx)
	{
		auto player = player_.lock();
		if (player == nullptr) {
			LOG_ERROR << "[message container] player is null";
			return;
		}
		auto maillist = ActiveModel::Base<Packet::MessageInfo>(dbctx).FindAll({ { "owner_id", player->Guid() } });
		for (auto& mail : maillist) { 
			m_mails.push_back(mail);
			LOG_INFO << "#MAIL# load mail ," << mail->owner_id() << "," << mail->guid();
			mail->ClearDirty();
		}

		auto notify_playerinfolist = ActiveModel::Base<Packet::MessageListPlayerInfo>(dbctx).FindAll({ { "player_guid", player->Guid() } });
		for (auto& notify : notify_playerinfolist) {
			m_notify_playerinfo.push_back(notify); 
			notify->ClearDirty();
		}

		auto offline_messageinfo = ActiveModel::Base<Packet::FriendMessageInfo>(dbctx).FindAll({ { "receive_guid", player->Guid() } });
		for (auto& message : offline_messageinfo) {
			m_offline_messageinfo.push_back(message);
			message->ClearDirty();
		}
	}

	void MessageContainer::OnSave(const boost::shared_ptr<zxero::dbcontext>& dbctx)
	{
		auto player = player_.lock();
		if (player == nullptr) {
			LOG_ERROR << "[message container] player is null";
			return;
		}
		ActiveModel::Base<Packet::MessageInfo>(dbctx).Delete({ { "owner_id", player->Guid() } });
		for (auto &mail_ : m_mails)
		{
			ActiveModel::Base<Packet::MessageInfo>(dbctx).Save(mail_);
		}

		ActiveModel::Base<Packet::MessageListPlayerInfo>(dbctx).Delete({ { "player_guid", player->Guid() } });
		for (auto &notify_ : m_notify_playerinfo)
		{
			ActiveModel::Base<Packet::MessageListPlayerInfo>(dbctx).Save(notify_);
		}
	}

	void MessageContainer::OnCache()
	{
		auto player = player_.lock();
		if (player == nullptr) {
			LOG_ERROR << "[message container] player is null";
			return;
		}
		//ActiveModel::Base<Packet::MessageInfo>(dbctx).Delete({ { "owner_id", player_->Guid() } });
		for (auto &mail_ : m_mails){
			player->Cache2Save(mail_.get());
		}

		//ActiveModel::Base<Packet::MessageListPlayerInfo>(dbctx).Delete({ { "player_guid", player_->Guid() } });
		for (auto &notify_ : m_notify_playerinfo) {
			player->Cache2Save(notify_.get());
		}

		for (auto &notify_ : m_offline_messageinfo) {
			player->Cache2Save(notify_.get());
		}
	}

	void MessageContainer::fill_mail_protocol(Packet::MessagesUpdate& m)
	{
		for (auto& child : m_mails)
		{
			m.add_messages()->CopyFrom(*child);
		}
	}

	void MessageContainer::send_notify_playerinfos(MtPlayer* player)
	{
		if (player)
		{
			Packet::ClientFriendMessageNotify notify;
			for (auto& child : m_notify_playerinfo)
			{
				MtFriendManager::Fill_MessagePlayerInfo_MessageList(*notify.add_notify(), child);
			}
		}
	}

	const std::vector<boost::shared_ptr<Packet::MessageListPlayerInfo>>&  MessageContainer::notify_playerinfo() const
	{
		return m_notify_playerinfo;
	}

	boost::shared_ptr<Packet::MessageListPlayerInfo> MessageContainer::get_notify_playerinfo(uint64 guid)
	{
		auto notify_ =  std::find_if(std::begin(m_notify_playerinfo), std::end(m_notify_playerinfo), [guid](auto &iter) {
			return iter->msg_player_guid() == guid;
		});

		if (notify_ != std::end(m_notify_playerinfo))
		{
			return *notify_;
		}
		return nullptr;
	}
	void MessageContainer::add_notify_playerinfo(boost::shared_ptr<Packet::MessageListPlayerInfo> info)
	{
		if (get_notify_playerinfo(info->msg_player_guid()) == nullptr)
		{
			m_notify_playerinfo.push_back(info);
		}
	}

	void MessageContainer::del_notify_playerinfo(uint64 guid)
	{
		auto player = player_.lock();
		if (player == nullptr) {
			LOG_ERROR << "[message container] player is null";
			return;
		}
		for (auto iter = m_notify_playerinfo.begin(); iter != m_notify_playerinfo.end();) {
			if ((*iter)->msg_player_guid() == guid) {
				//标记和删除
				(*iter)->SetDeleted();
				player->Cache2Save((*iter).get());
				iter = m_notify_playerinfo.erase(iter);
			}
			else {
				++iter;
			}
		}
	}

	const std::vector<boost::shared_ptr<Packet::FriendMessageInfo>>& MessageContainer::offline_messageinfo() const
	{
		return m_offline_messageinfo;
	}
	void MessageContainer::add_offlinemessage(boost::shared_ptr<Packet::FriendMessageInfo> offlinemsg)
	{
		if (offlinemsg)
		{
			m_offline_messageinfo.push_back(offlinemsg);
		}
	}
	void MessageContainer::delete_offlinemessage()
	{
		auto player = player_.lock();
		if (player == nullptr) {
			LOG_ERROR << "[message container] player is null";
			return;
		}
		for (auto msg : m_offline_messageinfo) {
			Packet::AddFriendMessagesReply add;
			add.mutable_messages()->CopyFrom(*msg.get());
			player->SendMessage(add);
			//标记和删除
			msg->SetDeleted();
			player->Cache2Save(msg.get());
		}
		m_offline_messageinfo.clear();
	}
	void MessageContainer::process_tick()
	{
		uint64 nowsecond = now_second();
		std::vector<uint64> del_mails;
		for (auto& child:m_mails)
		{
			bool del_ = static_message_manager::Instance().check_auto_del(child->datetime(), nowsecond);
			if (del_)
			{
				del_mails.push_back(child->guid());
			}
		}

		for (auto& mail_id : del_mails)
		{
			delete_mail(mail_id,MAIL_OPT_TYPE::AUTO_DEL);
		}
	}
	//add
	void MessageContainer::add_mail(boost::shared_ptr<Packet::MessageInfo> mail_,MAIL_OPT_TYPE opt_type)
	{
		if (mail_ == nullptr)
		{
			return ;
		}

		m_mails.push_back(mail_);
		LOG_INFO <<"#MAIL# add mail ,"<<mail_->owner_id() <<","<<mail_->guid() <<","<<opt_type<<",";
	}

	void MessageContainer::delete_mail(uint64 id,MAIL_OPT_TYPE opt_type)
	{
		auto player = player_.lock();
		if (player == nullptr) {
			LOG_ERROR << "[message container] player is null";
			return;
		}
		for (auto iter = m_mails.begin(); iter != m_mails.end();) {
			if ((*iter)->guid() == id) {
				//标记和删除
				(*iter)->SetDeleted();
				player->Cache2Save((*iter).get());
				iter = m_mails.erase(iter);
			}
			else {
				++iter;
			}
		}


		LOG_INFO << "#MAIL# delete mail mailid = " << id << "," << opt_type;
	}

	boost::shared_ptr<Packet::MessageInfo> MessageContainer::get_mail(uint64 playerguid,uint64 id)
	{
		auto mail_iter = std::find_if(std::begin(m_mails), std::end(m_mails), [&id](auto& iter) {
			return iter->guid() == id;
		});

		if (mail_iter != std::end(m_mails))
		{
			return *mail_iter;
		}
		LOG_ERROR<<"#MAIL# get mail:"<<playerguid<<","<<id;
		return nullptr;
	}

	int32 MessageContainer::get_message_size()
	{
		return m_mails.size();
	}
}