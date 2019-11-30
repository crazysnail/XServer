#ifndef __MAIL_CONTAINER_H__
#define __MAIL_CONTAINER_H__

#include "FriendMail.pb.h"


namespace Mt
{
	using namespace zxero;
	class MessageContainer
	{
	public:
		MessageContainer(){}
		MessageContainer(MtPlayer& player);
		~MessageContainer();
	public:
		static void lua_reg(lua_State* ls);

		void clear_mails();
		//add
		void add_mail(boost::shared_ptr<Packet::MessageInfo> mail_,MAIL_OPT_TYPE opt_type);
		//delete
		void delete_mail(uint64 id,MAIL_OPT_TYPE opt_type);
		//get
		boost::shared_ptr<Packet::MessageInfo> get_mail(uint64 playerguid,uint64 id);
		//size
		int32 get_message_size();

		void fill_mail_protocol(Packet::MessagesUpdate& m);
		//
		void send_notify_playerinfos(MtPlayer* player);
		//
		void process_tick();
		//
		void add_notify_playerinfo(boost::shared_ptr<Packet::MessageListPlayerInfo> info);
		//
		void del_notify_playerinfo(uint64 guid);
		boost::shared_ptr<Packet::MessageListPlayerInfo> get_notify_playerinfo(uint64 guid);
		const std::vector<boost::shared_ptr<Packet::MessageListPlayerInfo>>& notify_playerinfo() const;
		const std::vector<boost::shared_ptr<Packet::FriendMessageInfo>>& offline_messageinfo() const;
		void delete_offlinemessage();
		void add_offlinemessage(boost::shared_ptr<Packet::FriendMessageInfo> offlinemsg);
	public:
		void OnLoad(const boost::shared_ptr<zxero::dbcontext>& dbctx);
		void OnSave(const boost::shared_ptr<zxero::dbcontext>& dbctx);
		void OnCache();
	private:
		std::vector<boost::shared_ptr<Packet::MessageInfo>> m_mails;
		std::vector<boost::shared_ptr<Packet::MessageListPlayerInfo>> m_notify_playerinfo;
		std::vector<boost::shared_ptr<Packet::FriendMessageInfo>> m_offline_messageinfo; //³¡¾°Ïß³Ì
		const boost::weak_ptr<MtPlayer> player_;
	};
}
#endif