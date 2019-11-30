#ifndef _SHARE_MEM_STATIC_BLOCK_H_
#define _SHARE_MEM_STATIC_BLOCK_H_

#include "types.h"
#include "share_memory.h"

//大小固定的预分配共享内存块
//效率高，但大小需要预先指定

namespace share_memory
{
	struct sm_head
	{
		sm_key			m_Key;
		uint32			m_Size;
		int32			m_AllocCount;	//当时的分配计数，attach后有用
		int32			m_CurFreeIndex; //当时的freeindex，attach后有用
		uint32			m_HeadVer;		//最后存盘版本
		sm_head()
		{
			m_Key = 0;
			m_Size = 0;
			m_AllocCount = -1;
			m_CurFreeIndex = -1;
			m_HeadVer = 0;
		}
	};

	enum	sm_init_mode
	{
		init_clearall = 1,		//清除模式
		init_loadall = 2,		//载入dump模式
	};

	class shm_static_block
	{

	public:
		shm_static_block()
		{
			m_pDataPtr = 0;
			m_hold = 0;
			m_Size = 0;
			m_pHeader = 0;
		}
		~shm_static_block() {};
		/*
		 *	创建ShareMem 访问对象(新创建)
		 *
		 *  SM_KEY	key		访问键值
		 *
		 *	uint32		Size	访问数据区字节个数
		 *
		 */
		bool	Create(sm_key key, uint32	Size);
		/*
		 *	销毁对象
		 */
		void	Destory();

		/*
		 *		附着ShareMem 访问对象(不是新创建)
		 *		SM_KEY	key		访问键值
		 *
		 *		uint32		Size	访问数据区字节个数
		 *
		 */
		bool	Attach(sm_key, uint32 Size);
		/*
		 *		取消附着(不销毁)
		 */
		bool	Detach();

		/*
		 *	 获得数据区指针
		 */
		char*	GetDataPtr()
		{
			return m_pDataPtr;
		}

		/*
		 *	获得 大小为tSize 的第tIndex 个smu的数据
		 */
		char*	GetTypePtr(uint32 tSize, uint32 tIndex);
		/*
		 *	获得数据区总大小
		 */
		uint32	GetSize()
		{
			return m_Size;
		}

		/*
		 *	检查key对应的数据块大小，防止从sm加载时大小和守护进程大小不一致，导致数据写乱
		 */
		bool    SizeCheck(sm_key key, uint32 Size);

		bool	DumpToFile(char* FilePath);
		bool	MergeFromFile(char* FilePath);

		uint32	GetHeadVer();
		void	SetHeadVer(uint32 ver);

		int32	GetAllocCount();
		void	SetAllocCount(int32 count);

		int32	GetFreeIndex();
		void	SetFreeIndex(int32 index);

		//命令
		int32					m_CmdArg;
	private:

		//ShareMemory	内存大小
		uint32				m_Size;
		// ShareMemory  数据指针
		char*				m_pDataPtr;
		// ShareMemory	内存头指针
		char*				m_pHeader;
		// ShareMemory	句柄	
		SMHandle			m_hold;

	};

}



#endif