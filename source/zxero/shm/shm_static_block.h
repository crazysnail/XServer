#ifndef _SHARE_MEM_STATIC_BLOCK_H_
#define _SHARE_MEM_STATIC_BLOCK_H_

#include "types.h"
#include "share_memory.h"

//��С�̶���Ԥ���乲���ڴ��
//Ч�ʸߣ�����С��ҪԤ��ָ��

namespace share_memory
{
	struct sm_head
	{
		sm_key			m_Key;
		uint32			m_Size;
		int32			m_AllocCount;	//��ʱ�ķ��������attach������
		int32			m_CurFreeIndex; //��ʱ��freeindex��attach������
		uint32			m_HeadVer;		//�����̰汾
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
		init_clearall = 1,		//���ģʽ
		init_loadall = 2,		//����dumpģʽ
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
		 *	����ShareMem ���ʶ���(�´���)
		 *
		 *  SM_KEY	key		���ʼ�ֵ
		 *
		 *	uint32		Size	�����������ֽڸ���
		 *
		 */
		bool	Create(sm_key key, uint32	Size);
		/*
		 *	���ٶ���
		 */
		void	Destory();

		/*
		 *		����ShareMem ���ʶ���(�����´���)
		 *		SM_KEY	key		���ʼ�ֵ
		 *
		 *		uint32		Size	�����������ֽڸ���
		 *
		 */
		bool	Attach(sm_key, uint32 Size);
		/*
		 *		ȡ������(������)
		 */
		bool	Detach();

		/*
		 *	 ���������ָ��
		 */
		char*	GetDataPtr()
		{
			return m_pDataPtr;
		}

		/*
		 *	��� ��СΪtSize �ĵ�tIndex ��smu������
		 */
		char*	GetTypePtr(uint32 tSize, uint32 tIndex);
		/*
		 *	����������ܴ�С
		 */
		uint32	GetSize()
		{
			return m_Size;
		}

		/*
		 *	���key��Ӧ�����ݿ��С����ֹ��sm����ʱ��С���ػ����̴�С��һ�£���������д��
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

		//����
		int32					m_CmdArg;
	private:

		//ShareMemory	�ڴ��С
		uint32				m_Size;
		// ShareMemory  ����ָ��
		char*				m_pDataPtr;
		// ShareMemory	�ڴ�ͷָ��
		char*				m_pHeader;
		// ShareMemory	���	
		SMHandle			m_hold;

	};

}



#endif