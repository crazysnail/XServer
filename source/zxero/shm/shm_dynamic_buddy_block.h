#ifndef _SHM_DYNAMIC_BUDDY_BLOCK_H__
#define _SHM_DYNAMIC_BUDDY_BLOCK_H__

#include "types.h"
#include "share_memory.h"
#include "shm_dynamic_block.h"

namespace share_memory {

	class shm_dynamic_buddy_block :public shm_dynamic_block
	{
	public:
		shm_dynamic_buddy_block() {};
		virtual ~shm_dynamic_buddy_block() {};
		struct memhead
		{
			int node_count;
			int longest[1];
		};

		struct alloced
		{
			int start;
			int end;
		};

		uint32	shm_pre_calc_size(uint32 size);
		//���⿪�������ӿڣ����һ��treesize����ҪԤ�����С��
		bool	shm_create_init(uint32 size);
		bool	shm_attach_init(uint32 size);
		//����Ӧ�����ӿڣ��޶��⿪��������ҪԤ�����С��
		bool	shm_create_init_ex(uint32 size);
		bool	shm_attach_init_ex(uint32 size);
		char*	shm_new(uint32 size);
		bool	shm_delete(char* address);
		void	shm_reset();		
		uint32	shm_alloced(std::vector<char *>& alloced_address);
		uint32	get_use_size() { return 0; };
		uint32	get_free_size() { return 0; };

	private:
		uint32 cal_tree_size_ex(uint32 size);
		uint32 floor_align_power_2(uint32 value);
		bool tree_new(char * buff, int node_count);
		bool tree_attach(char * buff, int node_count);
		int tree_alloc(int node_count);
		void tree_free(int offset_count);
		int tree_size(int offset_count);
		int	tree_alloced(std::vector<alloced> & all_saved);
		void tree_dump();
		int fixsize(int size);

	private:
		int32 m_data_size;
		struct memhead * self;

	};
}

#endif//_SHM_DYNAMIC_BUDDY_BLOCK_H__
