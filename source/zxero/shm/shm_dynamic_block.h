#ifndef _SHARE_MEM_DYNAMIC_BLOCK_H_
#define _SHARE_MEM_DYNAMIC_BLOCK_H_

#include "types.h"
#include <vector>
#include "share_memory.h"

namespace share_memory
{
	struct shm_block_head
	{
		sm_key			m_key;
		uint32			m_size;
		uint32			m_ud_size;
		shm_block_head()
		{
			m_key = 0;
			m_size = 0;
			m_ud_size = 0;
		}
	};

	class shm_dynamic_block
	{

	public:
		shm_dynamic_block()
		{
			m_hold = 0;
			m_all_size = 0;
			m_header = 0;
			m_data_ptr = 0;
		}
		virtual ~shm_dynamic_block() {};

		bool shm_create(sm_key key, uint32 size);	
		bool shm_attach(sm_key key, uint32 size);
		void shm_destroy();
		bool dump_to_file(char* file_path);
		bool merge_from_file(char* file_path);
		shm_block_head*	get_head() { return m_header; }
		char* get_data_ptr() { return m_data_ptr; }
		uint32	get_all_size() { return m_all_size; }
			
		virtual uint32	shm_pre_calc_size(uint32 size) = 0;
		virtual bool	shm_create_init(uint32 size) = 0;
		virtual bool	shm_attach_init(uint32 size) = 0;
		virtual char*	shm_new(uint32 size) = 0;	
		virtual bool	shm_delete(char* address) = 0;
		virtual void	shm_reset() = 0;
		virtual uint32	get_use_size() = 0;
		virtual uint32	get_free_size() = 0;
		virtual uint32	shm_alloced(std::vector<char *>& alloced_address) = 0;

	protected:

		//ShareMemory	内存大小
		uint32				m_all_size;
		// ShareMemory	内存头指针
		shm_block_head*		m_header;
		// ShareMemory	句柄	
		SMHandle			m_hold;
		//数据指针
		char*				m_data_ptr;

	};

}



#endif