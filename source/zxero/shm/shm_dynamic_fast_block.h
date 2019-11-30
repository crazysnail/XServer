#ifndef _SHARE_MEM_DYNAMIC_FAST_BLOCK_H_
#define _SHARE_MEM_DYNAMIC_FAST_BLOCK_H_

#include "types.h"
#include "share_memory.h"
#include "shm_dynamic_block.h"

namespace share_memory
{

	class shm_dynamic_fast_block:public shm_dynamic_block
	{
	public:
		shm_dynamic_fast_block() {
			m_use_size = 0;
		};
		virtual ~shm_dynamic_fast_block() {};

		uint32	shm_pre_calc_size(uint32 size);
		bool	shm_create_init(uint32 size);
		bool	shm_attach_init(uint32 size);
		char*	shm_new(uint32 size);
		bool	shm_delete(char* address);
		void	shm_reset();
		uint32	shm_alloced(std::vector<char *>& alloced_address);
		uint32	get_use_size();
		uint32	get_free_size();

	private:
		//ShareMemory	内存使用大小
		uint32				m_use_size;
	};
}
#endif