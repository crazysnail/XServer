#ifndef __static_mail_manger_h__
#define __static_mail_manger_h__
#include "../mt_types.h"
#include "FriendMail.pb.h"
#include "G2SMessage.pb.h"
namespace Mt
{
	struct mail_data
	{
		int32 id;
		std::string content;
		std::string title;
		item_helper rewards_[MAX_MAIL_REWARD_COUNT];
	};
	class message;
	struct s_mail;
	class static_message_manager
	{
	public:
		static void lua_reg(lua_State* ls);
	public:
		static_message_manager();
		~static_message_manager();
	public:
		//	模块加载和卸载
		static static_message_manager& Instance();
		//static
		int32 on_load();
		int32 on_unload();
		int32 on_release();
		void OnCacheAll();
		//
		void process_tick();

		mail_data* get_mail_data(int32 cfgid);

		bool check_auto_del(uint64 starttime, uint64 now_second);

		void send_static_mail(int32 cfgid, const boost::shared_ptr<MtPlayer>& player_);
		//db
		/*void on_db_load(dbcontext_ptr& dbctx);
		static void db_delete_mail(dbcontext_ptr& dbctx);
		static void db_save_mail(dbcontext_ptr& dbctx, s_mail* m_);*/

		uint64 generate_guid();
		void set_serial(int64 max_serial);

		void lua_create_message(uint64 target_id, int32 opt_type,
			std::string title, std::string content, std::map<int32, int32> item_map);

		//player // catch create
		boost::shared_ptr<Packet::MessageInfo> create_message( uint64 targetid, boost::shared_ptr<MtPlayer> targetplayer,  MAIL_OPT_TYPE opt_type,
 			std::string title, std::string content, google::protobuf::RepeatedPtrField<Packet::ItemCount>* items=nullptr,uint64 sourceid=INVALID_GUID,int32 chattime = -1);
		void player_catch_mails_g(MtPlayer* p);
		void mail_payback_g(const std::vector<uint64>& allplayer, const std::string& title, const std::string& content, google::protobuf::RepeatedPtrField<Packet::ItemCount> &items);

		bool gs2s_add_mail_to_player_s(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<G2S::GS2SAddMailToPlayer>& msg, int32 /*source*/);

		void player_catch_worldchat_g(MtPlayer* p);
		bool s2g_worldchat_g(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<S2G::WorldChat>& msg, int32 /*source*/);
	private:
		std::map<int32,mail_data*> m_mail_data;
		 
		std::vector<boost::shared_ptr<Packet::MessageInfo>> offline_mail;

		//缓存10个世界聊天
		std::list<boost::shared_ptr<Packet::ChatMessage>> tempworldchat_;
		//unzxero::mutex m_mutex;
		uint64 m_mail_serial;
	};
}
#endif