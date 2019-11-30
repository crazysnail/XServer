#ifndef _SMU_MANAGER_H_
#define _SMU_MANAGER_H_


#include "shm_static_block.h"

//ShareMemory chunk
template<typename T>

class shm_static_chunk
{	
public:
	shm_static_chunk( )
	{
		m_pRefObjPtr	= nullptr;	
		m_hObj			= nullptr;
		m_nMaxSize		= 0;
		m_AllocCount	= 0;
		m_CurFreeObjIndex = -1;
	}
	~shm_static_chunk()
	{	
		delete m_pRefObjPtr;
		delete []m_hObj;
	}

	bool LoadInit( sm_key key, uint32 Size )
	{
		ZXERO_ASSERT( Size != 0 );
		ZXERO_ASSERT( m_pRefObjPtr == nullptr );

		m_pRefObjPtr = new shm_static_block();

		ZXERO_ASSERT(m_pRefObjPtr);

		if(!m_pRefObjPtr)
			return false;

		m_pRefObjPtr->m_CmdArg = init_loadall;
		//bool ret = m_pRefObjPtr->Attach(key,sizeof(T)*nMaxCount+sizeof(sm_head));
		//ע�⣬�����Size�Ѿ�������smhead ���ݿ�Ĵ�С
		bool ret = m_pRefObjPtr->Attach(key,Size);
		if(!ret)
		{
			LOG_TRACE << "ShareMem::Setup SMU load init block fail!";
			ZXERO_ASSERT(ret);
			return ret;
		}

		m_nMaxSize		= (Size-sizeof(sm_head))/sizeof(T);
		//��ԭ����
		m_AllocCount	= m_pRefObjPtr->GetAllocCount();
		m_CurFreeObjIndex = m_pRefObjPtr->GetFreeIndex();
		ZXERO_ASSERT(m_AllocCount >= 0 && m_AllocCount <= m_nMaxSize );// �Ų������� [7/23/2015 SEED]
		ZXERO_ASSERT(m_CurFreeObjIndex >= 0 && m_CurFreeObjIndex <= m_nMaxSize );// �Ų������� [7/23/2015 SEED]
		//
		m_hObj	= new T* [m_nMaxSize];

		for( int32 i = 0; i < m_nMaxSize; i++ )
		{
			m_hObj[i] = reinterpret_cast<T*>(m_pRefObjPtr->GetTypePtr(sizeof(T),i));
			if ( m_hObj[i] == nullptr )
			{
				ZXERO_ASSERT( m_hObj[i] != nullptr );
				return false;
			}
		}
		m_key = key;
		return true;
	}

	bool CreateInit( uint32 nMaxCount, sm_key key )
	{
		ZXERO_ASSERT( nMaxCount != 0 );
		ZXERO_ASSERT( m_pRefObjPtr == nullptr );

		m_pRefObjPtr = new shm_static_block();

		ZXERO_ASSERT(m_pRefObjPtr);
		
		if(!m_pRefObjPtr)
			return false;

		m_pRefObjPtr->m_CmdArg = init_clearall;

		bool ret = false;
#if ZXERO_OS_WINDOWS
		ret = m_pRefObjPtr->Attach(key,sizeof(T)*nMaxCount+sizeof(sm_head));
		if( ret )
		{
			return false;			
		}
#endif
		ret = m_pRefObjPtr->Create(key,sizeof(T)*nMaxCount+sizeof(sm_head));		
		if(!ret)
		{
			LOG_TRACE << "ShareMem::Setup SMU create init block fail!";
			ZXERO_ASSERT(ret);
			return false;
		}

		m_nMaxSize		= nMaxCount;
		m_AllocCount	= 0;
		m_CurFreeObjIndex = 0;
		m_pRefObjPtr->SetAllocCount(m_AllocCount); //��¼count
		m_pRefObjPtr->SetFreeIndex(m_CurFreeObjIndex); //��¼index
		m_hObj			= new T* [m_nMaxSize];

		for( int32 i = 0; i < m_nMaxSize; i++ )
		{
			m_hObj[i] = reinterpret_cast<T*>(m_pRefObjPtr->GetTypePtr(sizeof(T),i));
			if ( m_hObj[i] == nullptr )
			{
				ZXERO_ASSERT( m_hObj[i] != nullptr );
				return false;
			}

			new(m_hObj[i])T();

			//����nextfree��ϵ [9/16/2015 SEED]
			//�����Ȳ��������һ���ڵ��next_free_indexԽ�����⣬��������ʱ���������Ǹ�ʱ��˵���Ѿ�û�п����ڴ��ˣ�
			m_hObj[i]->set_sharemem_id(-1);	
			m_hObj[i]->set_next_free_id(i+1);	
		}
		m_key = key;
		return true;
	}

	bool Finalize()
	{
		ZXERO_ASSERT(m_pRefObjPtr);
		m_pRefObjPtr->Destory();
		return true;
	}

	void ClearChunk()
	{
		for( int32 i=0; i<m_nMaxSize; i++ )
		{
			m_hObj[i]->reset();
			m_hObj[i]->set_sharemem_id(-1);	
			m_hObj[i]->set_next_free_id(i+1);	
		}
		m_AllocCount = 0;
		m_CurFreeObjIndex = 0;

		m_pRefObjPtr->SetAllocCount(m_AllocCount); //��¼count
		m_pRefObjPtr->SetFreeIndex(m_CurFreeObjIndex); //��¼index
	}

	T* NewObj( void )
	{
		if ( m_AllocCount >= m_nMaxSize || m_AllocCount < 0 )
		{
			LOG_ERROR<<"shm_static_chunk memory out! key="<<m_key<<" tail index="<<m_AllocCount<<" chunk size="<<m_nMaxSize;
			ZXERO_ASSERT(false);
			return nullptr;
		}

		if ( m_CurFreeObjIndex >= m_nMaxSize || m_CurFreeObjIndex<0)
		{
			LOG_ERROR<<"shm_static_chunk memory out! key="<<m_key<<" tail index="<<m_AllocCount<<" chunk size="<<m_nMaxSize;
			ZXERO_ASSERT(false);
			return nullptr;
		}
		
		T *pObj = m_hObj[m_CurFreeObjIndex];				
		if(pObj==nullptr)
		{
			LOG_ERROR<<"shm_static_chunk new obj is null! key="<<m_key<<" tail index="<<m_AllocCount<<" chunk size="<<m_nMaxSize;
			ZXERO_ASSERT(false);
			return nullptr;
		}

		//�����Լ���smid
		m_hObj[m_CurFreeObjIndex]->set_sharemem_id(m_CurFreeObjIndex);
		//���·����obj��nextfreeindex��Ϊ�´η����m_CurFreeObjIndex
		int32 nextFreeIndex = m_hObj[m_CurFreeObjIndex]->get_next_free_id();
		ZXERO_ASSERT(nextFreeIndex >= 0 );// �Ų������� [7/23/2015 SEED]
		m_CurFreeObjIndex = nextFreeIndex;
		m_AllocCount++;

		m_pRefObjPtr->SetAllocCount(m_AllocCount); //��¼count
		m_pRefObjPtr->SetFreeIndex(m_CurFreeObjIndex); //��¼index

		//�����·����obj
		return pObj;
	}

	void DeleteObj( int32 iIndex )
	{
		ZXERO_ASSERT(iIndex<m_nMaxSize);
		if(m_hObj[iIndex] == nullptr)
			return;

		m_AllocCount--;
		ZXERO_ASSERT(m_AllocCount >= 0);// �Ų������� [7/23/2015 SEED]
		if( m_AllocCount < 0 )
			m_AllocCount = 0;

		m_hObj[iIndex]->reset();
		m_hObj[iIndex]->set_sharemem_id(-1);

		//����ǰfreeobj������Ϊ�ͷ�obj����һ��freeboj����
		m_hObj[iIndex]->set_next_free_id(m_CurFreeObjIndex);
		//���ͷŵ�index��Ϊ��ǰ��freeobj����
		m_CurFreeObjIndex = iIndex;

		//m_CurFreeObjIndex = m_hObj[iIndex]->get_next_free_id();

		m_pRefObjPtr->SetAllocCount(m_AllocCount); //��¼count
		m_pRefObjPtr->SetFreeIndex(m_CurFreeObjIndex); //��¼index

	}

	T*	GetObj(int32 iIndex)
	{
		ZXERO_ASSERT(iIndex<m_nMaxSize);
		ZXERO_ASSERT(iIndex>=0);

		if(iIndex<0 ||iIndex>=m_nMaxSize)
			return nullptr;

		return m_hObj[iIndex];
	}

	int32	GetChunkMaxSize()
	{
		return m_nMaxSize;
	}

	int32	GetChunkSize()
	{
		return m_AllocCount;
	}

	void SetChunkSize( int32 index )
	{
		m_AllocCount = index;
	}

	sm_key	GetKey()
	{
		return m_key;
	}

	bool	DumpToFile(char* FilePath)
	{
		if(!m_pRefObjPtr)
		{
			ZXERO_ASSERT(m_pRefObjPtr);
			return false;
		}

		return m_pRefObjPtr->DumpToFile(FilePath);
	}
	bool	MergeFromFile(char* FilePath)			
	{
		if(!m_pRefObjPtr)
		{
			ZXERO_ASSERT(m_pRefObjPtr);
			return false;
		}
		return m_pRefObjPtr->MergeFromFile(FilePath);
	}

	uint32	GetHeadVer()
	{
		ZXERO_ASSERT(m_pRefObjPtr);
		return m_pRefObjPtr->GetHeadVer();
	}
	void	SetHeadVer(uint32 ver)
	{
		ZXERO_ASSERT(m_pRefObjPtr);
		return m_pRefObjPtr->SetHeadVer(ver);
	}	

	void	DumpUseInfo()
	{
		LOG_TRACE<<"share_mem chunk key="<<m_key<<" use info="<<m_AllocCount<<"/"<<m_nMaxSize;
	}

private:
	T				**m_hObj;			//�������SMU����
	int32			m_nMaxSize;			//�������
	int32			m_AllocCount;		//��ǰʹ��ƫ��
	int32			m_CurFreeObjIndex;	//��ǰ���õ�obj�±�
	shm_static_block*	m_pRefObjPtr;	//����SMObject
	sm_key			m_key;				//��Ӧ��ShareMemory Key	
};

//--------------------------------------------------------------------
//��Ԫ��
#define MAX_MANAGER_SMU		5000
#define MAX_SM_OBJ_NUM		10

template<typename T>
class chunk_obj_manager
{
	
	// smu ����
	T				*m_chunk_obj[MAX_MANAGER_SMU];
	// smu ����
	int32			m_nCount;

public:

	chunk_obj_manager():
	  m_nCount(0)
	{
		memset(m_chunk_obj,0,sizeof(T*)*MAX_MANAGER_SMU);
	}

	~chunk_obj_manager()
	{}
	
	//��ʼ������
	void Init()
	{
		m_nCount	= 0;
		return true;
	}
	
	bool HeartBeat(uint32 uTime=0);

	/*
	 *	���һ��SMUPool ���������ݵ�������
	 */

	bool	add_obj(T*		pSMU)
	{
		ZXERO_ASSERT( m_nCount < MAX_MANAGER_SMU );
		if( m_nCount >= MAX_MANAGER_SMU )
			return false;

		m_chunk_obj[m_nCount] = pSMU;
		m_chunk_obj->SetSMUID( m_nCount );
		m_nCount ++;
		return true;
	}
	/*
	 *	ɾ��һ��SMUPool ���������ݵ�������
	 */
	bool	del_obj(T* pSMU)
	{
		uint32 uID = pSMU->GetSMUID();
		
		ZXERO_ASSERT(uID < (uint32)m_nCount );
		if (uID >= (uint32)m_nCount )
			return false;

		ZXERO_ASSERT( m_chunk_obj[uID] == pSMU  );
		if ( m_chunk_obj[uID] != pSMU )
			return false;

		m_chunk_obj[uID] = m_chunk_obj[m_nCount-1];
		m_chunk_obj[uID]->SetSMUID(uID); //���¸ı�����
		pSMU->SetSMUID( -1 );
		m_nCount--;
		
		return true;
	}
	/*
	 *	����SMUId ����SMU
	 */
	T*	get_chunk(SMUID_t	SMUId)
	{
		ZXERO_ASSERT(SMUId<m_nCount);
		if( SMUId>=m_nCount )
			return nullptr ;
		return m_chunk_obj[SMUId];		
	}
};

#endif