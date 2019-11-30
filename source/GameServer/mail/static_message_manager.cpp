#include "static_message_manager.h"
#include "module.h"
#include "app.h"
#include "../mt_server/mt_server.h"
#include "../data/data_manager.h"
#include "../mt_player/mt_player.h"
#include "../mt_scene/mt_scene.h"
#include "../mt_guid/mt_guid.h"
#include "../mt_cache/mt_shm_manager.h"
#include "../base/mt_db_save_work.h"
#include "S2GMessage.pb.h"

//必须在最后include！-------------------
#include "../zxero/mem_debugger.h"
//------------------------------------
namespace Mt
{
		//DECLARE_REG_PREPARED_STATEMENT(delete_mail_ps);
		//DECLARE_REG_PREPARED_STATEMENT(save_mail_ps);
		//DECLARE_REG_PREPARED_STATEMENT(load_mail_ps);
		using namespace zxero;
		static static_message_manager *sg_static_mail_manager = nullptr;
		REGISTER_MODULE(static_message_manager)
		{
			require("data_manager");

			register_load_function(module_base::STAGE_LOAD, boost::bind(&static_message_manager::on_load, static_message_manager::Instance()));
			register_unload_function(module_base::STAGE_UNLOAD, boost::bind(&static_message_manager::on_unload, static_message_manager::Instance()));
		}

		static_message_manager::static_message_manager()
		{
			m_mail_serial = INVALID_GUID;
			sg_static_mail_manager = this;
		}

		static_message_manager::~static_message_manager()
		{
			on_release();
			m_mail_serial = INVALID_GUID;
		}

		void static_message_manager::lua_reg(lua_State* ls)
		{
			ff::fflua_register_t<static_message_manager, ctor()>(ls, "static_message_manager")
				.def(&static_message_manager::player_catch_mails_g, "player_catch_mails_g")
				.def(&static_message_manager::player_catch_worldchat_g, "player_catch_worldchat_g")
				.def(&static_message_manager::lua_create_message, "create_message")
				;
			ff::fflua_register_t<>(ls)
				.def(&static_message_manager::Instance, "lua_static_message_manager");
		}

		static_message_manager& static_message_manager::Instance()
		{
			return *sg_static_mail_manager;
		}

		int32 static_message_manager::on_load()
		{
			//data_table * config_table = data_manager::instance()->get_table("t_maildata");
			//data_row * result = nullptr;
			//for( int32 i=0; i<config_table->get_rows(); ++i )
			//{
			//	result = config_table->find_row(i);
			//	mail_data * p = SAFE_NEW mail_data();
			//	p->id = result->getInt("id");
			//	p->title = result->getString("title");
			//	p->content = result->getString("content");

			//	for (int32 j =0; j< MAX_MAIL_REWARD_COUNT;++j)
			//	{
			//		char szbuffer[64] ={0};
			//		zprintf(szbuffer,sizeof(buffer),"reward_id_%d",j);
			//		p->rewards_[j].item_id = result->getInt(szbuffer);
			//		zprintf(szbuffer,sizeof(buffer),"reward_count_%d",j);
			//		p->rewards_[j].item_count = result->getInt(szbuffer); 
			//	}
			//	m_mail_data.insert(std::make_pair(p->id, p));
			//}
			return 0;
		}

		int32 static_message_manager::on_unload()
		{
			on_release();
			return 0;
		}

		int32 static_message_manager::on_release()
		{
			for (auto& child : m_mail_data)
			{
				SAFE_DELETE( child.second);
			}
			m_mail_data.clear();

			return 0;
		}
		void static_message_manager::OnCacheAll()
		{
			std::set<google::protobuf::Message *> msg_set;
			for (auto user : offline_mail)
			{
				msg_set.insert(user.get());
			}
			MtShmManager::Instance().Cache2Shm(msg_set, "OffineMessageInfo");
		}
		void static_message_manager::process_tick()
		{
			
		}
		mail_data* static_message_manager::get_mail_data(int32 cfgid)
		{
			auto data = m_mail_data.find(cfgid);
			if (data != m_mail_data.end())
			{
				return data->second;
			}
			LOG_ERROR<<"mail data no cfg :"<<cfgid;
			return nullptr;
		}

		bool static_message_manager::check_auto_del(uint64 starttime, uint64 now_second)
		{
			uint32 leaveTime = (uint32)(now_second - starttime);
			if (leaveTime > AUTO_DEL_MAIL_TIME)
			{
				return true;
			}
			return false;
		}

		void static_message_manager::send_static_mail(int32 cfgid, const boost::shared_ptr<MtPlayer>& player_)
		{
			mail_data* da_da = get_mail_data(cfgid);
			if (da_da == nullptr)
			{
				return;
			}
			boost::shared_ptr<Packet::MessageInfo> mail_ ;
			player_;
			if (mail_ != nullptr)
			{
				//plyaer_->send_mail(mail_,MAIL_OPT_TYPE::PAYBACK_ADD);
			}
		}

		uint64 static_message_manager::generate_guid()
		{
			//unzxero::auto_lock lock(m_mutex);
			m_mail_serial++;
			return m_mail_serial;
		}

		void static_message_manager::set_serial(int64 max_serial)
		{
			m_mail_serial = max_serial;
		}


		void static_message_manager::lua_create_message(uint64 target_id, int32 opt_type, std::string title, std::string content, std::map<int32, int32> item_map)
		{
			google::protobuf::RepeatedPtrField<Packet::ItemCount> items;
			for (auto& item_info : item_map) {
				auto item = items.Add();
				item->set_itemid(item_info.first);
				item->set_itemcount(item_info.second);
			}
			create_message(target_id, nullptr, MAIL_OPT_TYPE(opt_type), title, content, &items);
		}

		//init
		boost::shared_ptr<Packet::MessageInfo> static_message_manager::create_message(uint64 targetid, boost::shared_ptr<MtPlayer> targetplayer, MAIL_OPT_TYPE opt_type,
			std::string title, std::string content, google::protobuf::RepeatedPtrField<Packet::ItemCount>* items ,uint64 sourceid, int32 chattime)
		{
			boost::shared_ptr<Packet::MessageInfo> mail_ = boost::make_shared<Packet::MessageInfo>();
			if (mail_ == nullptr)
			{
				return nullptr;
			}

			mail_->set_guid(MtGuid::Instance()(*mail_));
			if (targetplayer == nullptr)
				targetplayer = Server::Instance().FindPlayer(targetid);

			uint64 starttime = now_second();
			mail_->set_datetime(starttime);
			mail_->set_owner_id(targetid);
			mail_->set_source_guid(sourceid);
			mail_->set_title(title);
			mail_->set_content(content);
			mail_->set_chat_time(chattime);
			uint32 allitemcount = 5;
			if (items != nullptr){
				uint32 itemcount = (*items).size();
				itemcount = itemcount > allitemcount ? allitemcount : itemcount;
				mail_->mutable_items()->CopyFrom(*items);
				allitemcount -= itemcount;
			}
			for (uint32 i=0;i<allitemcount;++i)
			{
				Packet::ItemCount item;
				item.set_itemid(0);
				item.set_itemcount(0);
				mail_->add_items()->CopyFrom(item);
			}

			bool addok = false;
			if (targetplayer != nullptr)
			{
				boost::shared_ptr<G2S::GS2SAddMailToPlayer> addmail = boost::make_shared<G2S::GS2SAddMailToPlayer>();
				if (addmail)
				{
					addmail->mutable_mail()->CopyFrom(*(mail_).get());
					targetplayer->ExecuteMessage(addmail);
				}
				else
				{
					ZXERO_ASSERT(false) << "message no mail message " << targetplayer->Guid();
				}
			}
			if (addok == false)
			{
				if (MAIL_OPT_TYPE::PAYBACK_ADD != opt_type)
				{
					std::vector<boost::shared_ptr<Packet::MessageInfo>> all_db_mail;
					all_db_mail.push_back(mail_);
					Server::Instance().AddDbSaveTask(new SaveOfflineMail(all_db_mail));
				}
			}
			return mail_;
		}

		void static_message_manager::player_catch_mails_g(MtPlayer* p)
		{
			if (p == nullptr)
			{
				ZXERO_ASSERT(false) << "Player null";
				return;
			}
			for (auto mail_iter = offline_mail.begin(); mail_iter != offline_mail.end();)
			{
				if ((*mail_iter)->owner_id() == p->Guid())
				{
					boost::shared_ptr<G2S::GS2SAddMailToPlayer> addmail = boost::make_shared<G2S::GS2SAddMailToPlayer>();
					if (addmail)
					{
						addmail->mutable_mail()->CopyFrom(*(*mail_iter).get());
						p->ExecuteMessage(addmail);
						mail_iter = offline_mail.erase(mail_iter);
					}
					else
					{
						mail_iter++;
					}
				}
				else
				{
					mail_iter++;
				}
			}
		}

		void static_message_manager::mail_payback_g(const std::vector<uint64>& allplayer, const std::string& title, const std::string& content, google::protobuf::RepeatedPtrField<Packet::ItemCount> &items)
		{
			for (auto playerid : allplayer)
			{
				auto player_ = Server::Instance().FindPlayer(playerid);
				boost::shared_ptr<Packet::MessageInfo> mail_ = static_message_manager::Instance().create_message(playerid, player_, MAIL_OPT_TYPE::PAYBACK_ADD, title, content, &items);
				if (player_ == nullptr)
				{
					offline_mail.push_back(mail_);
				}
			}
			LOG_INFO << "mail_payback: "<< offline_mail.size();
			//Server::Instance().AddDbSaveTask(new SaveOfflineMail(all_db_mail));
		}
		bool static_message_manager::gs2s_add_mail_to_player_s(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<G2S::GS2SAddMailToPlayer>& msg, int32 /*source*/)
		{
			if (player==nullptr)
			{
				ZXERO_ASSERT(false)<<"gs2s_add_mail_to_player_s player null "<<msg->mail().owner_id()<<","<< msg->mail().guid();
				return true;
			}
			player->add_message(boost::make_shared<Packet::MessageInfo>(msg->mail()) , (MAIL_OPT_TYPE)(msg->addtype()));
			LOG_INFO << "#MAIL# add gs2s_add_mail_to_player_s ," << msg->mail().owner_id() << "," << msg->mail().guid() << "," << msg->addtype();
			return true;
		}

		void static_message_manager::player_catch_worldchat_g(MtPlayer* p)
		{
			if (p == nullptr)
			{
				return;
			}
			for (auto chatmsg : tempworldchat_)
			{
				Packet::ChatMessage msg;
				msg.CopyFrom(*chatmsg.get());
				p->SendMessage(msg);
			}
		}
		bool static_message_manager::s2g_worldchat_g(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<S2G::WorldChat>& msg, int32 /*source*/)
		{
			player;//player null
			boost::shared_ptr<Packet::ChatMessage> msg_ = boost::make_shared<Packet::ChatMessage>();
			if (msg_ == nullptr) {
				return true;
			}
			msg_->CopyFrom(msg->msg());
			if (tempworldchat_.size() >= 10)
			{
				tempworldchat_.pop_front();
			}
			tempworldchat_.push_back(msg_);
			return true;
		}
		/*
		void static_meesage_manager::on_db_load(dbcontext_ptr& dbctx)
		{
			//try
			//{
			//	auto ps_load = PS(load_mail_ps);
			//	result_set_ptr result(ps_load->executeQuery());
			//	if ( result  )
			//	{
			//		while(result->next())
			//		{  
			//			uint64 s_id = result->getUInt64("tmailid");
			//			uint64 starttime = result->getUInt64("tstarttime");
			//			uint64 ownerid = result->getUInt64("tmail_ownerid");
			//			uint64 fromid = result->getUInt64("tmail_fromid");
			//			std::string fromname = result->getString("tmail_fromname");
			//			std::string title = result->getString("tmail_title");
			//			std::string content = result->getString("tmail_content");
			//			int32 type_ = result->getInt("tmail_type");
			//			item_helper items[MAX_MAIL_REWARD_COUNT];
			//			for (int32 i=0; i<MAX_MAIL_REWARD_COUNT; ++i)
			//			{
			//				char szbuffer[64] ={0};
			//				zprintf(szbuffer,sizeof(szbuffer),"titemid%d",i);
			//				items[i].item_id = result->getInt(szbuffer);
			//				zprintf(szbuffer,sizeof(szbuffer),"titemcount%d",i);
			//				items[i].item_count = result->getInt(szbuffer); 
			//			}
			//			message* mail_ = create_mail(ownerid,fromid,fromname,title,content,items,MAX_MAIL_REWARD_COUNT,type_,s_id,starttime);
			//			if (mail_ != nullptr)
			//			{
			//				add_mail(mail_,MAIL_OPT_TYPE::LOAD_ADD);
			//			}
			//			else
			//			{

			//			}
			//			//mail* mail_ = SAFE_NEW mail(s_id,cfgid,m_ownerid,stime,reward1,reward2,parms);
			//			//ZXERO_ASSERT(mail_);
			//			//add_mail(mail_);
			//		}
			//	}
			//}
			//catch (sql::SQLException& e)
			//{
			//	LOG_ERROR<<"load mail data error"<< e.what(); 
			//	throw e;
			//}
		}
		

		void static_meesage_manager::db_delete_mail(dbcontext_ptr& dbctx)
		{
			//try
			//{
			//	auto ps_mail_delete = PS(delete_mail_ps);
			//	result_set_ptr result(ps_mail_delete->executeQuery());
			//	if (!result)
			//	{
			//		LOG_ERROR<<"delete mail error!";
			//		throw Sotcc::E_UNKOWN_RUNNING_ERROR;
			//	}
			//}
			//catch (sql::SQLException& e)
			//{
			//	LOG_ERROR<<"delete mail error :"<< e.what(); 
			//	throw e;
			//}
		}

		void static_meesage_manager::db_save_mail(dbcontext_ptr& dbctx, s_mail* m_)
		{
			//try
			//{
			//	auto ps_mail_save = PS(save_mail_ps);
			//	ps_mail_save->setUInt64(1,m_->m_mail_id );
			//	ps_mail_save->setUInt64(2,m_->m_starttime);
			//	ps_mail_save->setUInt64(3,m_->m_mail_ownerid );
			//	ps_mail_save->setUInt64(4,m_->m_mail_fromid );
			//	ps_mail_save->setString(5,m_->m_mail_fromname );
			//	ps_mail_save->setInt(6,m_->m_type );
			//	ps_mail_save->setString(7,m_->m_title );
			//	ps_mail_save->setString(8,m_->m_content );
			//	for (int32 i=0;i<MAX_MAIL_REWARD_COUNT;++i)
			//	{
			//		ps_mail_save->setInt( 9+i*2, m_->m_items[i].item_id );
			//		ps_mail_save->setInt( 10+i*2, m_->m_items[i].item_count );
			//	}

			//	result_set_ptr result(ps_mail_save->executeQuery());
			//	if (!result)
			//	{
			//		LOG_ERROR<<"save mail data error playerid:"<<m_->m_mail_ownerid<<" id:"<<m_->m_mail_id;
			//		throw Sotcc::E_UNKOWN_RUNNING_ERROR;
			//	}

			//}
			//catch (sql::SQLException& e)
			//{
			//	LOG_ERROR<<"save mail data error"<< e.what(); 
			//	throw e;
			//}
		}
		*/
}