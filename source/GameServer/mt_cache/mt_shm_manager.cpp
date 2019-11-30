#include "mt_shm_manager.h"
#include "active_model.h"
#include "../mt_migrate/mt_migrate.h"
#include "module.h"
#include "app.h"
#include "dbcontext.h"
#include "dbhelper.h"
#include <thread>
#include "utils.h"
#include <pb2json.h>
#include <S2GMessage.pb.h>

#include "../base/mt_db_save_work.h"
#include "../mt_config/mt_config.h"
#include "../mt_player/mt_player.h"

namespace Mt
{

	static MtShmManager* sg_dsm_manager = nullptr;
	REGISTER_MODULE(MtShmManager)
	{
		require("Server");
		register_load_function(STAGE_LOAD, boost::bind(&MtShmManager::OnLoad, sg_dsm_manager));
		register_unload_function(STAGE_UNLOAD, boost::bind(&MtShmManager::OnUnload, sg_dsm_manager));
	}


	void MtShmManager::lua_reg(lua_State* ls)
	{
		ff::fflua_register_t<MtShmManager, ctor()>(ls, "MtShmManager")
			.def(&MtShmManager::LuaCache2Shm, "Cache2Shm")
			.def(&MtShmManager::Shm2Save, "Shm2Save")
			.def(&MtShmManager::DumpShm, "DumpShm")
			.def(&MtShmManager::DumpInfo, "DumpInfo")
			.def(&MtShmManager::ResetShm, "ResetShm")
			;
		ff::fflua_register_t<>(ls)
			.def(&MtShmManager::Instance, "LuaMtShmManager");
	}

	MtShmManager::MtShmManager()
	{
		m_write_cache = 0;
		m_read_cache = 1;

		m_save_msg_mark = 0;
		sg_dsm_manager = this;
	}

	MtShmManager::~MtShmManager()
	{
		//OnRelease();// 重复释放sm会报assert [7/17/2015 SEED]
	}

	int32 MtShmManager::OnRelease()
	{
		//清除shm文件
		FILE *fp = fopen(SM_SIZE_FILE, "w");
		if (fp != nullptr) {
			fclose(fp);
		}
		//////////////////////////////////////////////////////////////////////////
		for (int32 i = 0; i < 2; i++)
		{
			m_shm_chunk[i].shm_destroy();
		}
		return 0;
	}

	MtShmManager& MtShmManager::Instance()
	{
		return *sg_dsm_manager;
	}

	bool MtShmManager::LoadShmFile()
	{
		LOG_INFO << "#MtShmManager#,LoadShmFile begin!";

		char StrLine[1024] = { 0 };
		FILE *fp = fopen(SM_SIZE_FILE, "r");

		struct init_node {
			share_memory::block_type btype; sm_key init_key; uint32 max_block; uint32 block_size; sm_key cur_key;
		};

		std::vector<init_node> init_node_list;

		if (fp != nullptr) {
			while (!feof(fp)) {
				memset(StrLine, 0, 1024);
				fgets(StrLine, 1024, fp);  //读取一行
				std::vector<int32> temp;
				zxero::utils::split_string(StrLine, ",", temp);
				if (strcmp(StrLine, "") == 0) {
					break;
				}

				if (temp.size() != 5) {
					ZXERO_ASSERT(false) << "#MtShmManager#,invalid shm size file!";
					break;
				}
				//初始数据
				init_node node;
				node.btype = (share_memory::block_type)temp[0];
				node.init_key = temp[1];
				node.max_block = temp[2];
				node.block_size = temp[3];
				node.cur_key = temp[4];
				init_node_list.push_back(node);
			}
			fclose(fp);
		}

		if (init_node_list.empty() || init_node_list.size() < 2) {
			return false;
		}

		//用前两行来初始化
		for (uint32 i = 0; i < 2; ++i) {
			if (!m_shm_chunk[i].shm_init(
				init_node_list[i].btype,
				init_node_list[i].init_key,
				init_node_list[i].max_block,
				init_node_list[i].block_size)) {
				ZXERO_ASSERT(false) << "#MtShmManager#,invalid shm size file!";
				return false;
			}
		}

		//根据基偶数attach
		for (uint32 k = 0; k < init_node_list.size(); k++) {
			if (!m_shm_chunk[k % 2].shm_attach(init_node_list[k].init_key, init_node_list[k].cur_key, init_node_list[k].block_size)) {
				ZXERO_ASSERT(false) << "#MtShmManager#,invalid shm size file!";
				return false;
			}
		}

		LOG_INFO << "#MtShmManager#,LoadShmFile end!";
		return true;
	}

	int32 MtShmManager::OnLoad()
	{
		return 0;
	}

	void MtShmManager::ShmInit()
	{
		//
		if (!LoadShmFile()) {

			//清除shm文件
			FILE *fp = fopen(SM_SIZE_FILE, "w");
			if (fp != nullptr) {
				fclose(fp);
			}

			const int32 server_id = MtConfig::Instance().server_id_;
			const int32 max_block_count = 4;
			for (int32 i = 0; i < 2; i++) {
				if (!m_shm_chunk[i].shm_init(
					share_memory::block_type::buddy_block,
					server_id*SHM_KEY_BASE_OFFSET + i * max_block_count,
					max_block_count,
					MtConfig::Instance().sm_block_size_ * 1024 * 1024,
					true)) {
					//这种情况说明是彻底没救了,不允许启动！
					EXIT();
				}
			}
			LOG_INFO << "[#MtShmManager#] ShmInit ok! db load start!";
		}
		else {
			if (!ShmRecoverCheck2()) {//还原失败就重置shm,彻底回档！
				for (int32 i = 0; i < 2; i++) {
					m_shm_chunk[i].shm_reset();
				}
				LOG_INFO << "[#MtShmManager#] ShmRecover failed! shm reback start!";
			}
			else {
				LOG_INFO << "[#MtShmManager#] ShmRecover ok! shm normal start!";
			}
		}
	}
	bool MtShmManager::ShmRecoverCheck2()
	{
		//输出当前shm
		DumpShm();

		LOG_INFO << "[#MtShmManager#] ShmRecoverCheck2 begin!";
		//-------------------------------------------------------------
		//m_shm_head是第一块sm数据，里面存放了oncache的时间戳，可以用来校验数据的完整性以及数据的最新性
		//先确认最新缓存
		std::vector<char *> alloced_buff_0;
		m_shm_chunk[0].shm_alloced(alloced_buff_0);
		std::vector<char *> alloced_buff_1;
		m_shm_chunk[1].shm_alloced(alloced_buff_1);

		shm_object* shm_obj_0 = nullptr;
		shm_object* shm_obj_1 = nullptr;

		for (auto child : alloced_buff_0) {
			auto ptr = (shm_object*)child;
			if (ptr != nullptr && strcmp(ptr->name, "Packet.ShmHead") == 0) {
				shm_obj_0 = ptr;
				break;
			}
		}

		for (auto child : alloced_buff_1) {
			auto ptr = (shm_object*)child;
			if (ptr != nullptr && strcmp(ptr->name, "Packet.ShmHead") == 0) {
				shm_obj_1 = ptr;
				break;
			}
		}

		if (alloced_buff_0.empty() && alloced_buff_1.empty()) {
			LOG_INFO << "[#MtShmManager#]empty share memory data!";
			return true;
		}
		boost::scoped_ptr<Packet::ShmHead> cache_0;
		boost::scoped_ptr<Packet::ShmHead> cache_1;
		int32 init_write_index = -1;

		if (shm_obj_0 != nullptr) {
			cache_0.reset((Packet::ShmHead*)MtShmManager::GetMessageByName(shm_obj_0->name));
			if (cache_0 != nullptr) {
				cache_0->ParseFromArray(shm_obj_0->data, shm_obj_0->length);
			}
		}

		if (shm_obj_1 != nullptr) {
			cache_1.reset((Packet::ShmHead*)MtShmManager::GetMessageByName(shm_obj_1->name));
			if (cache_1 != nullptr) {
				cache_1->ParseFromArray(shm_obj_1->data, shm_obj_1->length);
			}
		}

		//最新的初始化为write缓存
		if (cache_0 != nullptr && cache_1 != nullptr) {
			if (cache_0->cache_time() >= cache_1->cache_time()) {
				init_write_index = 0;
				LOG_INFO << "[#MtShmManager#] init data with cache0 time = " << cache_0->cache_time();
			}
			else {
				init_write_index = 1;
				LOG_INFO << "[#MtShmManager#] init data with cache1 time = " << cache_1->cache_time();
			}
		}
		else if (cache_0 != nullptr) {
			init_write_index = 0;
			LOG_INFO << "[#MtShmManager#] init data with cache0 time = " << cache_0->cache_time();
		}
		else if (cache_1 != nullptr) {
			init_write_index = 1;
			LOG_INFO << "[#MtShmManager#] init data with cache1 time = " << cache_1->cache_time();
		}
		else {
			ZXERO_ASSERT(false) << "[#MtShmManager#]bad share memory data! init failed!";
			return false;
		}

		if (init_write_index == -1) {//没有任何有用的shm
			return false;
		}

		m_write_cache = init_write_index;
		m_read_cache = init_write_index == 0 ? 1 : 0;

		LOG_INFO << "[#MtShmManager#] ShmRecoverCheck2 end! m_write_cache=" << m_write_cache << " m_read_cache=" << m_read_cache;
		return true;
	}

	void MtShmManager::ShmRecoverSave()
	{
		LOG_INFO << "[#MtShmManager#] ShmRecoverSave begin!";
		ReMarkSaved();
		FastSave(m_read_cache);

		//再交换一次shm，保证write shm也完成存储
		SwapCache();
		ReMarkSaved();
		FastSave(m_read_cache);

		LOG_INFO << "[#MtShmManager#] ShmRecoverSave end!";
	}

	void MtShmManager::ResetShm()
	{
		for (int32 i = 0; i < 2; i++) {
			//
			m_msg_shm_addr[i].clear();
			m_shm_chunk[i].shm_reset();
			m_cache_player[i].clear();
		}
		m_used_size = 0;
		m_max_msg_size = 0;
	}
	void MtShmManager::DumpInfo()
	{
		for (int32 i = 0; i < 2; i++) {
			std::vector<char *> alloced_buff;
			m_shm_chunk[i].shm_alloced(alloced_buff);
			LOG_INFO << "chunk index=" << i << ", buff size=" << alloced_buff.size();
			LOG_INFO << "chunk index=" << i << ", block count=" << m_shm_chunk[i].block_count();
			LOG_INFO << "chunk index=" << i << ", block size=" << m_shm_chunk[i].block_size();
			LOG_INFO << "chunk index=" << i << ", init key=" << m_shm_chunk[i].init_key();
			LOG_INFO << "chunk index=" << i << ", max block count=" << m_shm_chunk[i].max_block_count();
			LOG_INFO << "chunk index=" << i << ", used size =" << m_used_size;
			LOG_INFO << "chunk index=" << i << ", player count =" << m_cache_player[i].size();
			LOG_INFO << "chunk index=" << i << ", max msg size=" << m_max_msg_size;
		}
	}
	void MtShmManager::DumpShm()
	{
		LOG_INFO << "DumpShm begin!";
		FILE *fp = fopen("ShmMsg.txt", "w+");
		Json::FastWriter writer;
		if (fp != nullptr) {
			for (int32 i = 0; i < 2; i++) {
				std::vector<char *> alloced_buff;
				m_shm_chunk[i].shm_alloced(alloced_buff);
				LOG_INFO << "chunk index=" << i << ", buff size=" << (int)alloced_buff.size();

				for (auto ab : alloced_buff) {
					shm_object * head = (shm_object*)ab;
					LOG_INFO << "save shm addr=" << (int*)ab << ", name =" << head->name << ", key =" << head->key << ",  save Dirty" << (int)head->Dirty() << ",  save Delete=" << (int)head->Deleted();

					boost::scoped_ptr<google::protobuf::Message> msg(MtShmManager::GetMessageByName(head->name));
					if (msg != nullptr) {
						msg->ParseFromArray(head->data, head->length);
						Json::Value jvalue;
						FormatToJson(jvalue, *msg);
						std::string out2 = writer.write(jvalue);
						fprintf(fp, "%s:%s", head->name, out2.c_str());
					}
				}
			}
			fclose(fp);
		}

		DumpInfo();
		LOG_INFO << "DumpShm end!";
	}

	int32 MtShmManager::OnUnload()
	{
		OnRelease();
		return 0;
	}

	static int times = 0;
	void MtShmManager::Shm2Save()
	{
		LOG_INFO << "OnSaveShmBegin !";
		while (!m_cache_done) {//必须等
			//防止save线程在没cache完的情况下交换了双缓冲,破坏shm
			std::this_thread::sleep_for(std::chrono::milliseconds(50));
			times++;
		}
		// 如果写线程在写，些微卡一下存储线程，这个可以忍 [2/2/2015 SEED]
		// 把swap cache提前。解决第二次存储是默认数据的问题和保证每次存在都是最新数据
		SwapCache();
		ReMarkSaved();
		FastSave(m_read_cache);

		LOG_INFO << "OnSaveShmEnd ! wait times=" << times;
		times = 0;

		return;
	}

	void MtShmManager::SwapCache()
	{
		std::unique_lock<std::mutex> lock(m_swap_mutex);

		m_write_cache ^= 1;
		m_read_cache ^= 1;
	}

	int32 MtShmManager::MarkMsgSaved()
	{
		std::unique_lock<std::mutex> lock(m_mark_mutex);

		m_save_msg_mark++;
		return m_save_msg_mark;
	}

	void MtShmManager::SingleSave(int32 cache_index)
	{
		std::vector<char *> alloced_buff;
		int count = m_shm_chunk[cache_index].shm_alloced(alloced_buff);

		///
		for (auto ab : alloced_buff) {
			if (ab == nullptr) {
				count--;
				continue;
			}

			shm_object * head = (shm_object*)ab;
			boost::scoped_ptr<google::protobuf::Message> message(MtShmManager::GetMessageByName(head->name));
			if (message == nullptr) {
				count--;
				continue;
			}

			//存储线程n对n个work存储,支持断线重连
			Server::Instance().AddDbSaveTask(new MessageSaveWork(head));
		}
		do {
			LOG_INFO << "#FastSave#" << m_save_msg_mark << "/" << count;
			std::this_thread::sleep_for(std::chrono::milliseconds(100));

		} while (m_save_msg_mark < count);


		LOG_INFO << "#SingleSave# done" << m_save_msg_mark << "/" << count << "m_write_cache=" << m_write_cache << ": m_read_cache" << m_read_cache;
	}

	void MtShmManager::SetSave(int32 cache_index)
	{		
		std::vector<char *> alloced_buff;
		int32 msg_count = m_shm_chunk[cache_index].shm_alloced(alloced_buff);
		int32 set_count = 0;
		if (msg_count != 0) {
			uint32 unit_count = 10;
			set_count = msg_count / unit_count +1;			
			std::list<shm_object*> msg_set;
			for (auto ab : alloced_buff) {
				if (ab == nullptr) {
					continue;
				}

				shm_object * head = (shm_object*)ab;
				boost::scoped_ptr<google::protobuf::Message> message(MtShmManager::GetMessageByName(head->name));
				if (message == nullptr) {
					continue;
				}

				msg_set.push_back(head);

				if (msg_set.size() >= unit_count) {
					//存储线程n对n个work存储,支持断线重连
					Server::Instance().AddDbSaveTask(new MessageSetSaveWork(msg_set));
					msg_set.clear();
				}

			}

			//存储线程n对n个work存储,支持断线重连
			Server::Instance().AddDbSaveTask(new MessageSetSaveWork(msg_set));
			msg_set.clear();

			do {
				LOG_INFO << "#FastSave#" << m_save_msg_mark << "/" << set_count;
				std::this_thread::sleep_for(std::chrono::milliseconds(100));

			} while (m_save_msg_mark < set_count);

		}

		LOG_INFO << "#SetSave# done" << m_save_msg_mark << "/" << set_count << " msg_count="<< msg_count<< " m_write_cache=" << m_write_cache << ": m_read_cache" << m_read_cache;
	}

	void MtShmManager::FastSave(int32 cache_index)
	{
		try{
		
			SetSave(cache_index);

			//处理离线玩家
			auto msg = boost::make_shared<S2G::CheckOffLine>();
			for (auto child : m_cache_player[cache_index]) {				
				msg->add_guid(child.first);
				msg->add_stamp(child.second);
			}
			Server::Instance().SendMessage(msg);

			//
			m_msg_shm_addr[cache_index].clear();
			m_shm_chunk[cache_index].shm_reset();
			m_cache_player[cache_index].clear();
			m_used_size = 0;
			m_max_msg_size = 0;

		}
		catch (sql::SQLException& e){
			do {//关服时候有存储异常就不让关服
				LOG_ERROR<<"[#sharemem_manager] fast save failed! server can not shut down!"<<e.what();
				std::this_thread::sleep_for(std::chrono::seconds(10));
			} while (1);
		}	

		LOG_INFO << "#FastSave# done";
	}

	void MtShmManager::Cache2Shm(std::set<google::protobuf::Message *> &msg_set, const std::string &tag)
	{
		tag;
		if (msg_set.empty()) {
			return;
		}
		for (auto msg : msg_set) {
			Cache2Shm(msg);
		}
	}

	void MtShmManager::OnCacheShmBegin()
	{
		m_head_msg.set_cache_time(MtTimerManager::Instance().CurrentTime());
		LOG_INFO << "OnCacheShmBegin set_cache_time=" << m_head_msg.cache_time() << m_write_cache << ": m_read_cache" << m_read_cache << " done:" << m_cache_done;
		Cache2Shm(&m_head_msg);
	}

	void MtShmManager::OnCacheShmEnd()
	{
		LOG_INFO << "OnCacheShmEnd set_cache_time=" << m_head_msg.cache_time() << m_write_cache << ": m_read_cache" << m_read_cache << " done:" << m_cache_done;
		Server::Instance().CheckNormalSave();

		LOG_INFO << "all shm used size =" << m_used_size << "byte! msg count=" << m_cache_player[m_write_cache].size()<<" max msg size="<< m_max_msg_size;
	}

	void MtShmManager::Cache2Shm(google::protobuf::Message *msg, const uint64 guid, const uint32 cache_stamp)
	{
		if (msg == nullptr)
			return;
		
		if (!msg->Dirty() && !msg->Deleted()) {
			return;
		}

		std::unique_lock<std::mutex> lock(m_cache_mutex);//各个场景的竞争锁

		m_cache_done = false;//保证不被save线程打断

		 //脏数据cache时，如果没有数据，需要begin来记录头数据
		if (m_cache_player[m_write_cache].empty()) {
			m_head_msg.set_cache_time(MtTimerManager::Instance().CurrentTime());
			Cache2ShmInternal(&m_head_msg);
		}

		if (Cache2ShmInternal(msg)){
			//记录guid和时间戳
			if (guid != INVALID_GUID) {
				auto iter = m_cache_player[m_write_cache].find(guid);
				if (iter != m_cache_player[m_write_cache].end()) {
					m_cache_player[m_write_cache][guid] = cache_stamp;
				}
				else {
					m_cache_player[m_write_cache].insert({ guid, cache_stamp });
				}
			}
		}
		
		m_cache_done = true;//保证不被打断

	}		

	bool MtShmManager::Cache2ShmInternal( google::protobuf::Message * msg)
	{
		if (msg == nullptr) {
			return false;
		}

		UniqueValue key = MtMigrate::Instance().GetMsgUniqueValue(*msg);
		if (key.IsInvalid()) {
			return false;
		}

		bool mark_cached = false;
		if (m_msg_shm_addr[m_write_cache].find(key) != m_msg_shm_addr[m_write_cache].end()) {
			char *buff_address = m_msg_shm_addr[m_write_cache][key];
			if (buff_address != nullptr) {
				m_shm_chunk[m_write_cache].shm_delete(buff_address);
			}
			mark_cached = true;
		}
		
		int msg_size = msg->ByteSize();
		int all_size = msg_size + sizeof(shm_object);
		char* buff_address = m_shm_chunk[m_write_cache].shm_new(all_size);			
		if (buff_address == nullptr) {
			ZXERO_ASSERT(false) << "shm_dynamic_chunk::shm_new failed! cant create more shm block!";
			return false;
		}
		shm_object * obj = (shm_object *)buff_address;

		//初始化一下
		obj->init();

		zprintf(obj->name, 64, "%s", msg->GetDescriptor()->full_name().c_str());
		obj->length = msg_size;
		if (msg->Dirty()) 
			obj->SetDirty();
		if (msg->Deleted()) 
			obj->SetDeleted();

		//查bug
		obj->key = key.value;

		msg->SerializeToArray(obj->data, msg_size);

		m_used_size += all_size;
		if (all_size > m_max_msg_size) {
			m_max_msg_size = all_size;
		}
		////查问题用
		//for (auto child : m_msg_shm_addr[m_write_cache]) {
		//	if (child.second == buff_address && key != child.first) {
		//		ZXERO_ASSERT(false) << "shm new error, addr conflict! key=" << key.name << " addr=" << buff_address;
		//	}
		//}
		////以上查问题用
		//////
		if (mark_cached) {
			m_msg_shm_addr[m_write_cache][key] = buff_address;
		}
		else {
			m_msg_shm_addr[m_write_cache].insert({ key, buff_address });
		}

		msg->ClearDirty();
		msg->ClearDeleted();

		return true;
	}

	google::protobuf::Message * MtShmManager::GetMessageByName(std::string name)
	{
		const google::protobuf::Descriptor * desc = google::protobuf::DescriptorPool::generated_pool()->FindMessageTypeByName(name);
		if (desc == nullptr)
		{
			LOG_FATAL << "can not create message for: " << name;
			return nullptr;
		}

		google::protobuf::Message* message = google::protobuf::MessageFactory::generated_factory()->GetPrototype(desc)->New();
		return message;
	}

}	//	namespace Mt


