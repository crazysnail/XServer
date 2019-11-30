#ifndef SHM_DYNAMIC_CHUNK_H
#define SHM_DYNAMIC_CHUNK_H


#include "types.h"
#include "share_memory.h"

using namespace std;
namespace	share_memory
{
	class shm_dynamic_block;
	enum block_type {
		fast_block=0,
		buddy_block=1,
	};
	class shm_dynamic_chunk
	{
	public:	
		shm_dynamic_chunk();
		~shm_dynamic_chunk();
		bool shm_init(block_type btype,sm_key init_key, uint32 max_block_count, uint32 block_size, bool create_init=false);	
		bool shm_attach(sm_key init_key, sm_key key, uint32 size);
		bool shm_destroy();
		bool shm_reset();
		char* shm_new(uint32 size);
		void shm_delete(char* addr);
		uint32 shm_alloced(std::vector<char *> &alloced_address);

		bool dump_to_file(sm_key key, char* file_path);
		bool dump_all(char* file_path);
		bool merge_from_file(sm_key key, char* file_path);
	
		char* read_from(uint32 read_size);


		uint32 block_count() { return m_block_set.size(); }
		uint32 block_size() { return m_block_size; }
		uint32 max_block_count() { return m_max_block_count; }
		sm_key init_key() { return m_init_key; }

	private:
		shm_dynamic_block* shm_create();
		shm_dynamic_block* align_block(uint32 size);

	private:
		std::map<sm_key, shm_dynamic_block*> m_block_set; //共享内存集合
		uint32 m_max_block_count;
		uint32 m_block_size;
		sm_key m_init_key;
		block_type m_btype;
		shm_dynamic_block* m_cur_block;					//当前可用的block
	};
}

#endif