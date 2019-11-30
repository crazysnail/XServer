//#include "../GameServer/common.h"
#include "../GameServer/mt_types.h"
#include <pb2json.h>
#include "../zxero/shm/share_memory.h"
#include "../zxero/shm/shm_dynamic_chunk.h"
#include "../include/utils.h"
#include "../include/log.h"
#include "../include/log_file.h"

//#include "../zxero/mem_debugger.h"
//#ifdef ZXERO_OS_WINDOWS
//#include <vld.h> 
//#endif
#include <unordered_map>  

using namespace zxero;

//void test_linux_ini_file()
//{
//	std::string str_test = utils::get_ini_key_string("system_dir", "config_dir", SERVER_INTERNAL_FILE);
//	printf("\n string test:%s", str_test.c_str());
//	int32 int_test = utils::get_ini_key_int("db_thread", "load_count", SERVER_INTERNAL_FILE);
//	printf("\n int test:%d", int_test);
//}



struct init_node {
	share_memory::block_type btype; sm_key init_key; uint32 max_block; uint32 block_size; sm_key cur_key;
};

std::vector<init_node> init_node_list;
share_memory::shm_dynamic_chunk m_shm_chunk[2];

#include <map>
#include <algorithm>
using namespace std;

bool LoadShmFile(std::vector<init_node> &init_node_list)
{
	//{
	//	std::vector<std::pair<uint64, int32>> ptmap;
	//	ptmap.push_back({ 30,39 });
	//	ptmap.push_back({ 40,49 });
	//	ptmap.push_back({ 50,59 });
	//	ptmap.push_back({ 60,60 });
	//	std::sort(std::begin(ptmap), std::end(ptmap), [](auto& lhs, auto& rhs){
	//		return lhs.second > rhs.second;
	//	});
	//}

	char StrLine[1024] = { 0 };
	FILE *fp = fopen(SM_SIZE_FILE, "r");
	if (fp != nullptr) {
		while (!feof(fp)) {
			memset(StrLine, 0, 1024);
			fgets(StrLine, 1024, fp);  //读取一行
			std::vector<int32> temp;
			zxero::utils::split_string(StrLine, ",", temp);
			if (strcmp(StrLine, "") == 0) {
				//ZXERO_ASSERT(false) << "#MtShmManager#,invalid shm size file!";
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


	//int32 max_block = init_node_list[0].max_block;
	//if ((int32)init_node_list.size() != max_block * 2) {
	//	ZXERO_ASSERT(false) << "#MtShmManager#,invalid shm size file!";
	//	return false;
	//}
	if (!init_node_list.empty()&&init_node_list.size() >= 2 ) {
		return true;
	}

	return false;
}

bool InitShm(std::vector<init_node> &init_node_list)
{	
	if (!LoadShmFile(init_node_list))
		return false;
	
	for (uint32 i = 0; i < init_node_list.size(); i++) {
		printf("block=%d, init_key=%llu, max_block=%u, block_size=%u, cur_key=%llu \n"
			, init_node_list[i].btype
			, init_node_list[i].init_key
			, init_node_list[i].max_block
			, init_node_list[i].block_size
			, init_node_list[i].cur_key);
	}

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

	for (uint32 k = 0; k < init_node_list.size(); k++) {
		if (!m_shm_chunk[k%2].shm_attach(init_node_list[k].init_key, init_node_list[k].cur_key, init_node_list[k].block_size)) {
			ZXERO_ASSERT(false) << "#MtShmManager#,invalid shm size file!";
			return false;
		}
	}

	return true;

}

struct shm_object {
	char name[64];
	int status_bits_;
	int length;
	uint64 key;
	char data[1];

	shm_object()
	{
		init();
	}

	void init() {
		name[64] = 0;
		status_bits_ = 0x01;
		length = 0;
		data[1] = 0;
		key = 0;
	}

	inline void SetDirty() { status_bits_ |= 0x01; }
	inline void ClearDirty() { status_bits_ &= ~0x01; }
	inline bool Dirty() const { return (status_bits_ & 0x01) != 0; }
	inline void SetDeleted() { status_bits_ |= 0x02; }
	inline void ClearDeleted() { status_bits_ &= ~0x02; }
	inline bool Deleted() const { return (status_bits_ & 0x02) != 0; }
};

//boost::scoped_ptr<zxero::LogFile> g_logFile;
#include <boost/format.hpp>

void DumpInfo()
{
	for (int32 i = 0; i < 2; i++) {
		std::vector<char *> alloced_buff;
		m_shm_chunk[i].shm_alloced(alloced_buff);
		LOG_INFO << "chunk index=" << i << ", buff size=" << alloced_buff.size();
		LOG_INFO << "chunk index=" << i << ", block count=" << m_shm_chunk[i].block_count();
		LOG_INFO << "chunk index=" << i << ", block size=" << m_shm_chunk[i].block_size();
		LOG_INFO << "chunk index=" << i << ", init key=" << m_shm_chunk[i].init_key();
		LOG_INFO << "chunk index=" << i << ", max block count=" << m_shm_chunk[i].max_block_count();
	}
}

google::protobuf::Message * GetMessageByName(std::string name)
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

int main(int argc, char** argv)
{

	int32 server_group_id = 1;
	uint64 guid = 999999;
	std::string cdstr1 = "dddd";

	boost::format fmt("REPLACE INTO `player_login_log` VALUES('%d','%d','%s','%llu','%d','%s','%s','%s','%s','%d','%s','%s','%s','%s');");
	fmt% server_group_id%server_group_id % cdstr1 % guid % server_group_id % cdstr1
		% cdstr1 % cdstr1 % cdstr1 % server_group_id
		% cdstr1 %cdstr1 % cdstr1 %cdstr1;


	printf(fmt.str().c_str());
	//zxero::log_message::log_level(zxero::LL_INFO);
	//g_logFile.reset(new zxero::LogFile("./log/smlog", 1024 * 1000 * 1000));

	if (!InitShm(init_node_list)) {
		printf("process attach fail !\n");
	}
	else
	{
		printf("process attach ok !\n");
	}

	
	char buffer[128] = { 0 };

	printf("input the process command![1.flush][2.close][3.dump]\n");
	while (true)
	{
		scanf("%s", buffer);
		if (strcmp(buffer, "flush") == 0)
		{
			//防止运行过程中，Gameserver配置成db_load_init导致文件数据被清空，这里在关闭时补一份
			FILE *fpw = fopen(SM_SIZE_FILE, "w+");
			if (fpw != nullptr)
			{
				for (auto &child : init_node_list)
				{
					fprintf(fpw, "%d,%llu,%u,%u,%llu\n", (int)child.btype, child.init_key, child.max_block, child.block_size, child.cur_key);
				}
				fclose(fpw);
			}

			printf("flush ok!");
		}
		else if (strcmp(buffer, "close") == 0)
		{
			for (int32 i=0; i<2; i++)
			{
				m_shm_chunk[i].shm_destroy();
			}
			break;
		}	
		else if (strcmp(buffer, "dump") == 0)
		{

			for (int32 i = 0; i < 2; i++)
			{
				std::vector<char *> alloced_buff;
				m_shm_chunk[i].shm_alloced(alloced_buff);

				for (auto ab : alloced_buff) {
					shm_object * head = (shm_object*)ab;
					printf("save shm addr=%d, name =%s, key =%llu,  save Dirty=%d,  save Delete=%d\n", (int *)ab, head->name, head->key, (int)head->Dirty(), (int)head->Deleted());
				}
			}

			DumpInfo();
			printf("dump ok!");
		}
		else if (strcmp(buffer, "fdump") == 0)
		{
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

						google::protobuf::Message * msg = GetMessageByName(head->name);
						if (msg != nullptr) {
							msg->ParseFromArray(head->data, head->length);
							Json::Value jvalue;
							FormatToJson(jvalue, *msg);
							std::string out2 = writer.write(jvalue);
							fprintf(fp, out2.c_str());
						}
					}
				}
				fclose(fp);
			}

			DumpInfo();
			printf("fdump ok!");
		}
	}
	return 0;
}

