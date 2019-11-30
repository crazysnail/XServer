#include <cmath>
#include "shm_dynamic_buddy_block.h"
#include "log.h"

#define LEFT_LEAF(index) ((index) * 2 + 1)
#define RIGHT_LEAF(index) ((index) * 2 + 2)
#define PARENT(index) ( ((index) + 1) / 2 - 1)

#define IS_POWER_OF_2(x) (!((x)&((x)-1)))
#define MAX(a, b) ((a) > (b) ? (a) : (b))

namespace share_memory {

	const static uint32 TREE_NODE_SIZE = sizeof(uint32); //每个树节点自身内存占用
	const static uint32 MIN_FRAG = 2*sizeof(uint32); //固定
	const static uint32 MINI_BLOCK_SIZE = MIN_FRAG*8;//每个树节点管理的内存大小，可调，需要是2的n次幂

	/*head_ptr[shm_block_head]|buddy_start_address[tree_size]|m_data_ptr*///-->//[size]
	//ex/*head_ptr[shm_block_head]|buddy_start_address[final_data_size]|m_data_ptr*///-->//[size]

	uint32	shm_dynamic_buddy_block::shm_pre_calc_size(uint32 size)
	{	
		int node_count = int(size / MINI_BLOCK_SIZE);
		int tree_size = node_count * 2 * TREE_NODE_SIZE;
		return sizeof(shm_block_head)+tree_size+ size;
	}

	bool shm_dynamic_buddy_block::shm_create_init(uint32 size)
	{
		char *buddy_start_address = (char*)m_header + sizeof(shm_block_head);
		int tree_node_count = int(size / MINI_BLOCK_SIZE);	
		if (!tree_new(buddy_start_address, tree_node_count)) {
			ZXERO_ASSERT(false) << "shm_dynamic_buddy_block::shm_create_init failed! size" << size;
			return false;
		}

		int tree_size = tree_node_count * 2 * TREE_NODE_SIZE;
		m_header->m_ud_size = tree_size;
		m_data_size = size;
		m_data_ptr = buddy_start_address + tree_size;

		return true;
	}

	bool shm_dynamic_buddy_block::shm_create_init_ex(uint32 size)
	{	
		uint32 tree_size = cal_tree_size_ex(size);
		uint32 data_size = size - sizeof(shm_block_head) - tree_size;
		//2n次幂对齐
		uint32 final_data_size = floor_align_power_2(data_size);

		char *buddy_start_address = (char*)m_header + sizeof(shm_block_head);
		int tree_node_count = int(final_data_size / MINI_BLOCK_SIZE);
		if (!tree_new(buddy_start_address, tree_node_count)) {
			ZXERO_ASSERT(false) << "shm_dynamic_buddy_block::shm_create_init failed! size" << size;
			return false;
		}
		
		m_header->m_ud_size = tree_size;
		m_data_size = final_data_size;
		m_data_ptr = buddy_start_address + tree_size;

		return true;
	}

	bool shm_dynamic_buddy_block::shm_attach_init(uint32 size)
	{
		uint32 data_size = size - sizeof(shm_block_head) - m_header->m_ud_size;
		char *buddy_start_address = (char*)m_header + sizeof(shm_block_head);
		int node_count = int(data_size / MINI_BLOCK_SIZE);
		if (!tree_attach(buddy_start_address, node_count)) {
			ZXERO_ASSERT(false) << "shm_dynamic_buddy_block::shm_attach_init failed! size" << size;
			return false;
		}

		m_data_size = data_size;
		m_data_ptr = buddy_start_address+ m_header->m_ud_size;

		return true;
	}
	bool shm_dynamic_buddy_block::shm_attach_init_ex(uint32 size)
	{
		uint32 data_size = size - m_header->m_ud_size - sizeof(shm_block_head);
		//2n次幂对齐
		uint32 final_data_size = floor_align_power_2(data_size);

		char *buddy_start_address = (char*)m_header + sizeof(shm_block_head);
		int tree_node_count = int(final_data_size / MINI_BLOCK_SIZE);
		if (!tree_attach(buddy_start_address, tree_node_count)){
			ZXERO_ASSERT(false) << "shm_dynamic_buddy_block::shm_attach_init failed! size" << size;
			return false;
		}

		m_data_size = final_data_size;
		m_data_ptr = buddy_start_address + m_header->m_ud_size;

		return true;
	}

	char* shm_dynamic_buddy_block::shm_new(uint32 size)
	{
		int tree_node_count = size / MINI_BLOCK_SIZE;
		tree_node_count = (size % MINI_BLOCK_SIZE == 0 ? tree_node_count : tree_node_count + 1);

		int node_offset = tree_alloc(tree_node_count);
		if (node_offset == -1 ){
			LOG_WARN << "tree_alloc failed!";
			return nullptr;
		}

		return m_data_ptr + (node_offset * MINI_BLOCK_SIZE);
	}

	bool shm_dynamic_buddy_block::shm_delete(char * address)
	{
		int offset = address - m_data_ptr;
		if (offset < 0 || offset > m_data_size)
		{
			LOG_WARN<<"free address out of range: " << address<<"/"<< (void *)address << "/" <<m_data_ptr << "/" << m_data_size << "/" << offset;
			return false;
		}

		if (offset % MINI_BLOCK_SIZE != 0)
		{
			ZXERO_ASSERT(false) << "free address alignment error: " << address;
			return false;
		}

		int offset_count = offset / MINI_BLOCK_SIZE;
		tree_free(offset_count);
		return true;
	}

	void shm_dynamic_buddy_block::shm_reset()
	{
		std::vector<char *> alloced_address;
		shm_alloced( alloced_address);
		for (auto child : alloced_address) {
			shm_delete(child);
		}
		return;
	}

	uint32 shm_dynamic_buddy_block::shm_alloced(std::vector<char *>& alloced_address)
	{
		std::vector<alloced> alloced_offset;
		tree_alloced(alloced_offset);

		for (auto ao : alloced_offset){
			char * address = m_data_ptr + (ao.start * MINI_BLOCK_SIZE);
			alloced_address.push_back(address);
		}

		return alloced_address.size();
	}
	
	//------------------------------------------------------
	int shm_dynamic_buddy_block::fixsize(int size)
	{
		size |= size >> 1;
		size |= size >> 2;
		size |= size >> 4;
		size |= size >> 8;
		size |= size >> 16;
		return size + 1;
	}

	uint32 shm_dynamic_buddy_block::cal_tree_size_ex(uint32 size)
	{		
		//小心别越界
		if(MIN_FRAG*size>0xffffffff){
			ZXERO_ASSERT(false) << "memory out of range！size=" << size;
			return 0;
		}
		//1gb，64byte MINI_BLOCK_SIZE，128mb
		//(size-final_size-sizeof(shm_block_head))/MINI_BLOCK_SIZE = final_size/MIN_FRAG
		uint32 final_size = (uint32)std::ceil(MIN_FRAG*(size - sizeof(shm_block_head)) / (MIN_FRAG + MINI_BLOCK_SIZE));
		return final_size;
	}

	//向下取整
	uint32 shm_dynamic_buddy_block::floor_align_power_2(uint32 value)
	{
		//2的n次幂对齐
		if (IS_POWER_OF_2(value)) {
			return value;
		}
		else {
			int i = 0, n = 0;
			while (i++ < 31) {
				if (value&(1 << i)) n = i;
			}

			if (n <= 1) {
				ZXERO_ASSERT(false) << "init shm size is too small!";
				return 0;
			}
	
			return (uint32)std::pow(2, n - 1);
		}
	}

	//构建叶子节点为1的二叉树
	//				  8			 longest[0]=16	
	//				/   \
	//			  4      4		 longest[1-2]=8	
	//			 / \     / \
	//			2  2     2  2	 longest[3-6]=4	
	//		   /\  /\   /\  /\
	//		  1 1 1 1  1 1  1 1  longest[7-14]=2	
	bool shm_dynamic_buddy_block::tree_new(char * buff, int node_count)
	{
		if (node_count < 1 || !IS_POWER_OF_2(node_count))
			return false;

		self = (struct memhead*)buff;
		self->node_count = node_count;
		int tree_node_count = node_count * 2;

		for (int i = 0; i < 2 * node_count - 1; ++i)
		{
			//第2的n次幂个i就进行二分
			if (IS_POWER_OF_2(i + 1))
				tree_node_count /= 2;
			self->longest[i] = tree_node_count;
		}
		return true;
	}

	bool shm_dynamic_buddy_block::tree_attach(char * buff, int node_count)
	{
		if (node_count < 1 || !IS_POWER_OF_2(node_count))
			return false;
		
		self = (struct memhead*)buff;

		if (node_count != self->node_count)
		{
			ZXERO_ASSERT(false);
			return false;
		}

		return true;
	}
	
	int shm_dynamic_buddy_block::tree_alloc(int node_count)
	{
		int index = 0;
		int node_size;
		int node_offset = 0;

		if (self == NULL)
			return -1;

		if (node_count <= 0)
			node_count = 1;
		else if (!IS_POWER_OF_2(node_count)) {
			//向上按照2的n次幂取整
			node_count = fixsize(node_count);
		}

		//分配数溢出
		if (self->longest[index] < node_count)
			return -1;

		//2×树遍历
		for (node_size = self->node_count; node_size != node_count; node_size /= 2)
		{
			if (self->longest[LEFT_LEAF(index)] >= node_count)
				index = LEFT_LEAF(index);
			else
				index = RIGHT_LEAF(index);
		}

		self->longest[index] = 0;//已经分配标记
		node_offset = (index + 1) * node_size - self->node_count;

		//回溯处理父节点
		while (index)
		{
			index = PARENT(index);
			self->longest[index] = MAX(self->longest[LEFT_LEAF(index)], self->longest[RIGHT_LEAF(index)]);
		}

		return node_offset;
	}
	
	void shm_dynamic_buddy_block::tree_free(int offset_count)
	{
		if (self == nullptr || offset_count < 0 || offset_count >= self->node_count) {
			ZXERO_ASSERT(false) << "invalid offset!";
			return;
		}

		int left_longest, right_longest = 0;
		int node_size = 1;
		int index = offset_count + self->node_count - 1;

		for (; self->longest[index]; index = PARENT(index))
		{
			node_size *= 2;
			if (index == 0)
				return;
		}

		self->longest[index] = node_size;

		while (index)
		{
			index = PARENT(index);
			node_size *= 2;

			left_longest = self->longest[LEFT_LEAF(index)];
			right_longest = self->longest[RIGHT_LEAF(index)];

			if (left_longest + right_longest == node_size)
				self->longest[index] = node_size;
			else
				self->longest[index] = MAX(left_longest, right_longest);
		}
	}
	
	int shm_dynamic_buddy_block::tree_size(int offset_count)
	{
		if (self == nullptr || offset_count < 0 || offset_count >= self->node_count) {
			ZXERO_ASSERT(false) << "invalid offset!";
			return -1;
		}
		int node_size = 1;
		for (int index = offset_count + self->node_count - 1; self->longest[index]; index = PARENT(index))
			node_size *= 2;

		return node_size;
	}

	void shm_dynamic_buddy_block::tree_dump()
	{
		char canvas[512];
		int node_size, offset;

		if (self == nullptr) {
			printf("buddy2_dump: (struct buddy2*)self == NULL");
			return;
		}

		if (self->node_count > 256) {
			printf("buddy2_dump: (struct buddy2*)self is too big to dump");
			return;
		}

		memset(canvas, '_', sizeof(canvas));
		node_size = self->node_count * 2;

		for (int i = 0; i < 2 * self->node_count - 1; ++i)
		{
			if (IS_POWER_OF_2(i + 1))
				node_size /= 2;

			if (self->longest[i] == 0)
			{
				if (i >= self->node_count - 1)
				{
					canvas[i - self->node_count + 1] = '*';
				}
				else if (self->longest[LEFT_LEAF(i)] && self->longest[RIGHT_LEAF(i)])
				{
					offset = (i + 1) * node_size - self->node_count;

					for (int j = offset; j < offset + node_size; ++j)
						canvas[j] = '*';
				}
			}
		}
		canvas[self->node_count] = '\0';
		puts(canvas);
	}

	int shm_dynamic_buddy_block::tree_alloced(std::vector<alloced> & all_saved)
	{
		int node_size, offset;
		int count = 0;

		node_size = self->node_count * 2;

		for (int i = 0; i < 2 * self->node_count - 1; ++i)
		{
			if (IS_POWER_OF_2(i + 1))
				node_size /= 2;

			if (self->longest[i] == 0)
			{
				if (i >= self->node_count - 1)
				{
					alloced al;
					al.start = i - self->node_count + 1;
					al.end = i - self->node_count + 1;
					all_saved.push_back(al);
					count++;
				}
				else if (self->longest[LEFT_LEAF(i)] && self->longest[RIGHT_LEAF(i)])
				{
					offset = (i + 1) * node_size - self->node_count;

					alloced al;
					al.start = offset;
					al.end = offset + node_size - 1;
					all_saved.push_back(al);
					count++;
				}
			}
		}

		return count;
	}

	
}


