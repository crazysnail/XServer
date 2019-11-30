#ifndef _SHARE_MEM_MANAGER_H_
#define _SHARE_MEM_MANAGER_H_

#include "../mt_types.h"
#include "../../zxero/shm/shm_dynamic_chunk.h"
#include "dbcontext.h"
#include <unordered_map>  
#include <Base.pb.h>

namespace std
{
template <>
struct hash<Mt::UniqueValue>
{
	size_t operator()(const Mt::UniqueValue& ct) const
	{
		size_t seed = 0;
		boost::hash_combine(seed, boost::hash_value(ct.name));
		boost::hash_combine(seed, boost::hash_value(ct.value));
		return seed;

		// Compute individual hash values without boost
		// http://stackoverflow.com/a/1646913/126995
		//size_t seed = 17;
		//seed = seed * 31 + std::hash_value(ct.name);
		//seed = seed * 31 + std::hash_value(ct.value);
		//return seed;
	}
};
}

namespace Mt
{

	const static int32 SHM_KEY_BASE_OFFSET = 1024;			//shm键值偏移参数,建议>=1024

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
			name[64] = { 0 };
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

	//	缓存管理器
	class MtShmManager : public boost::noncopyable
	{
	public:
		static void lua_reg(lua_State* ls);
	public:
		MtShmManager();
		virtual ~MtShmManager();
	public:
		static MtShmManager& Instance();

	public:
		
		int32 OnLoad();
		int32 OnUnload();
		int32 OnRelease();		

		void ShmInit();
		void LuaCache2Shm(google::protobuf::Message *msg) {
			Cache2Shm(msg);
		}
		void Cache2Shm(google::protobuf::Message *msg, const uint64 guid = INVALID_GUID, const uint32 cache_stamp_ = 0);		
		void Cache2Shm( std::set<google::protobuf::Message *> &msg_set,const std::string &tag);
		void Shm2Save();
		int32 MarkMsgSaved();	//每个work结束调用
		void OnCacheShmBegin();
		void OnCacheShmEnd();

		static google::protobuf::Message * GetMessageByName(std::string name);
		void ShmRecoverSave();
		void DumpShm();
		void DumpInfo();
		void ResetShm();

	private:
		//无需校验cache的完整性
		bool ShmRecoverCheck2();
		bool LoadShmFile();
		void SwapCache();	//变成私有成员函数，不然外部调用。保证每次save的都是最新数据
		void ReMarkSaved(){ m_save_msg_mark = 0;};
		void FastSave(int32 cache_index);		
		bool Cache2ShmInternal( google::protobuf::Message * msg);
		//单个msg作为存储单元
		void SingleSave(int32 cache_index);
		//按照线程数分批存储
		void SetSave(int32 cache_index);
	private:
		std::mutex m_cache_mutex;
		std::mutex m_swap_mutex;
		std::mutex m_mark_mutex;	

		share_memory::shm_dynamic_chunk m_shm_chunk[2];
		bool m_cache_done = false;
		std::map<uint64,uint32> m_cache_player[2];

		//二级缓存,sm存根，逻辑层message注册到sm的东东，sm利用这个来确定哪些message已经进行缓存和持久化
		std::unordered_map <UniqueValue, char*> m_msg_shm_addr[2];

		int32 m_write_cache;
		int32 m_read_cache;
		int32 m_save_msg_mark;
		Packet::ShmHead m_head_msg;		
		
		//统计用
		int32 m_used_size;
		int32 m_max_msg_size;
	};
}
#endif	//	#ifndef sharemem_manager
