#include "shm_dynamic_fast_block.h"
#include "log.h"

namespace share_memory {

	uint32	shm_dynamic_fast_block::shm_pre_calc_size(uint32 size)
	{
		//block类型的shm没有额外开销
		return size;
	}
	bool shm_dynamic_fast_block::shm_create_init(uint32 size)
	{
		size;
		m_data_ptr=(char*)m_header + sizeof(shm_block_head);
		return true;
	}
	bool shm_dynamic_fast_block::shm_attach_init(uint32 size)
	{
		size;
		m_data_ptr = (char*)m_header + sizeof(shm_block_head);
		return true;
	}
	char* shm_dynamic_fast_block::shm_new(uint32 size)
	{
		if (size <= 0 || size >= m_all_size-m_use_size) {
			ZXERO_ASSERT(false) << "tSize=" << size << "key=" << m_header->m_key;
			return nullptr;
		}			
		
		char* ptr = m_data_ptr + m_use_size;
		m_use_size += size;
		return ptr;
	}

	bool shm_dynamic_fast_block::shm_delete(char* address)
	{
		ZXERO_ASSERT(false) << "unsuppoted!" << address;
		return false;
	}
	uint32 shm_dynamic_fast_block::shm_alloced(std::vector<char *>& alloced_address)
	{
		ZXERO_ASSERT(false) << "unsuppoted!";
		return alloced_address.size();
	}
	void shm_dynamic_fast_block::shm_reset()
	{
		m_use_size = 0;
	}
	uint32	shm_dynamic_fast_block::get_use_size() 
	{ 
		return m_use_size;
	}
	uint32	shm_dynamic_fast_block::get_free_size() 
	{ 
		return m_all_size - m_use_size;
	}
}