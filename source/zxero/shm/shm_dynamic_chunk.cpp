#include "shm_dynamic_chunk.h"
#include "shm_dynamic_buddy_block.h"
#include "shm_dynamic_fast_block.h"
#include "utils.h"
#include "log.h"

namespace share_memory
{
	shm_dynamic_chunk::shm_dynamic_chunk()
	{		
	}

	shm_dynamic_chunk::~shm_dynamic_chunk()
	{
		for (auto child : m_block_set) {
			child.second->shm_destroy();
			delete child.second;
		}
		m_block_set.clear();

		m_init_key = 0;
		m_max_block_count  = 0;
		m_block_size = 0;
		m_cur_block = nullptr;
	}

	bool shm_dynamic_chunk::shm_init(block_type btype,sm_key init_key, uint32 max_block_count, uint32 block_size, bool create_init)
	{
		m_init_key = init_key;
		m_max_block_count = max_block_count;
		m_block_size = block_size;
		m_btype = btype;

		if (create_init) {
			m_cur_block = shm_create();
			if (m_cur_block == nullptr) {
				ZXERO_ASSERT(false) << "shm_dynamic_chunk create_init failed!";
				return false;
			}
		}

		return true;
	}
	bool shm_dynamic_chunk::shm_attach(sm_key init_key, sm_key key, uint32 size)
	{
		if (size <= 0)
			return false;

		if (m_init_key != init_key || m_block_size != size) {
			ZXERO_ASSERT(false) << "attach param not match with init param!";
			return false;
		}

		if (m_block_set.size() >= m_max_block_count) {
			ZXERO_ASSERT(false) << "shm_dynamic_chunk::shm_attach failed! more block! key=" << key << " size=" << size << " block_count" << m_max_block_count;
			return false;
		}

		if (m_block_set.find(key) != m_block_set.end()) {
			ZXERO_ASSERT(false) << "shm_dynamic_chunk::load_init failed! repeated key! key=" << key << " size=" << size;
			return false;
		}

		shm_dynamic_block *block_ptr = nullptr;
		if (m_btype == buddy_block) {
			block_ptr = new shm_dynamic_buddy_block();
		}
		else {
			block_ptr = new shm_dynamic_fast_block();
		}
		if (block_ptr == nullptr)
			return false;

		bool ret = block_ptr->shm_attach(key, size);
		if (!ret){
			ZXERO_ASSERT(ret) << "shm_dynamic_chunk::load_init failed! attach error! key="<<key<<" size="<<size;
			return false;
		}
		
		m_block_set.insert({ key,block_ptr });

		m_cur_block = block_ptr;

		return true;
	}

	shm_dynamic_block* shm_dynamic_chunk::shm_create()
	{		
		if (m_block_size <= 0)
			return nullptr;

		if (m_block_set.size() >= m_max_block_count) {
			ZXERO_ASSERT(false) << "shm_dynamic_chunk::create_init failed! more block! block_count"<<m_max_block_count;
			return nullptr;
		}

		sm_key key = m_init_key + m_block_set.size();
		uint32 size = m_block_size;

		if (m_block_set.find(key) != m_block_set.end()) {
			ZXERO_ASSERT(false) << "shm_dynamic_chunk::create_init failed! repeated key! key=" << key << " size=" << size;
			return nullptr;
		}

		shm_dynamic_block *block_ptr = nullptr;
		if (m_btype == buddy_block) {
			block_ptr = new shm_dynamic_buddy_block();
		}
		else {
			block_ptr = new shm_dynamic_fast_block();
		}
		if (block_ptr == nullptr) {
			return nullptr;
		}

		bool ret = false;
#if ZXERO_OS_WINDOWS
		ret = block_ptr->shm_attach(key, size);
		if (ret) {//必须检查key是否冲突
			ZXERO_ASSERT(false) << "shm_dynamic_chunk::create_init failed! attach error! key=" << key << " size=" << size;
			return nullptr;
		}
#endif
		ret = block_ptr->shm_create(key, size);
		if (!ret)
		{
			ZXERO_ASSERT(false) << "shm_dynamic_chunk::create_init failed! create error! key=" << key << " size=" << size;
			return nullptr;
		}

		m_block_set.insert({ key,block_ptr });

		// 写入预分配的配置文件，方便守护进程进行加载 [7/18/2015 SEED]
		FILE *fp = fopen(SM_SIZE_FILE, "a");
		if (fp != nullptr) {
			fprintf(fp, "%d,%llu,%u,%u,%llu\n", (int)m_btype, m_init_key, m_max_block_count, m_block_size, key);
			fclose(fp);
		}
		return block_ptr;
	}

	bool shm_dynamic_chunk::shm_destroy()
	{
		for (auto child : m_block_set) {
			child.second->shm_destroy();
		}
		return true;
	}

	bool shm_dynamic_chunk::shm_reset()
	{
		for (auto child : m_block_set) {
			child.second->shm_reset();
		}
		return true;
	}

	bool shm_dynamic_chunk::dump_to_file(sm_key key, char* file_path)
	{
		if (m_block_set.find(key) == m_block_set.end()) {
			return false;
		}
		return m_block_set[key]->dump_to_file(file_path);
	}

	bool shm_dynamic_chunk::dump_all(char* file_path)
	{
		for (auto child : m_block_set) {
			char out_file[256] = { 0 };
			zprintf(out_file, 256, "%sshm_%d.block", file_path, (int32)child.first);
			if (!child.second->dump_to_file(out_file)) {
				ZXERO_ASSERT(false) << "shm_dynamic_chunk::dump_to_file failed! key=" << child.first;
			}
		}
		return true;
	}

	bool shm_dynamic_chunk::merge_from_file(sm_key key, char* file_path)
	{
		if (m_block_set.find(key) == m_block_set.end()) {
			return false;
		}
		return m_block_set[key]->merge_from_file(file_path);
	}

	shm_dynamic_block* shm_dynamic_chunk::align_block(uint32 size)
	{
		shm_dynamic_block* ok_block = nullptr;
		for (auto child : m_block_set) {
			ok_block = child.second;
			char* new_addr = ok_block->shm_new(size);
			if (new_addr != nullptr) {
				return ok_block;
			}
		}

		return shm_create();
	}

	char* shm_dynamic_chunk::shm_new(uint32 size)
	{
		//profiler timer("shm_new");

		char *new_addr = nullptr;
		if (m_cur_block == nullptr) {
			m_cur_block = align_block(size);
			if (m_cur_block == nullptr) {
				ZXERO_ASSERT(false) << "shm_dynamic_chunk::align_block failed! cant create more shm block!";
				return nullptr;
			}
			new_addr = m_cur_block->shm_new(size);
		}
		else {
			new_addr = m_cur_block->shm_new(size);
			if (new_addr == nullptr) {
				//当前的block不够用了
				auto new_block = align_block(size);
				if (new_block == nullptr) {
					//真的分配不出来了
					ZXERO_ASSERT(false) << "shm_dynamic_chunk::align_block failed! cant create more shm block!";
					return nullptr;
				}
				m_cur_block = new_block;
				new_addr = m_cur_block->shm_new(size);
			}
		}
		return new_addr;
	}

	void shm_dynamic_chunk::shm_delete(char* address)
	{
		//profiler timer("shm_delete");

		for (auto child : m_block_set) {
			if (child.second->shm_delete(address)) {
				address = nullptr;
				return;
			}
		}
		ZXERO_ASSERT(false) << "shm_delete error! address not in shm! " << address << "/"<<(void *)address << "/" <<m_block_set.size();
	}

	uint32 shm_dynamic_chunk::shm_alloced(std::vector<char *> &alloced_address)
	{
		for (auto child : m_block_set) {
			child.second->shm_alloced(alloced_address);
		}
		return alloced_address.size();
	}

	char* shm_dynamic_chunk::read_from(uint32 read_size)
	{
		read_size;
		//if (read_size < 0 || read_size >= m_block_ptr->GetUsedSize()) {
		//	ZXERO_ASSERT(false) << "share_mem_chunk::CreateInit MemToObj failed! read_size=" << read_size;
		//	return nullptr;
		//}

		//char *p = new char(read_size);
		//memcpy(p, m_read_anchor, read_size);
		//m_read_anchor = m_read_anchor + read_size;

		//return p;
		return nullptr;
	}

}

