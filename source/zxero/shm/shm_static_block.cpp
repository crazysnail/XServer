#include "shm_static_block.h"
#include "log.h"

namespace share_memory {

	bool	shm_static_block::Create(sm_key key, uint32 Size)
	{
		if (m_CmdArg == init_loadall)
		{
			return false;
		}
		else
		{
			Destory();
			LOG_TRACE << "ShareMem::Close ShareMemory key =" << key;
		}

		m_hold = share_memory::CreateShareMem(key, Size);
		if (m_hold == INVALID_SM_HANDLE)
		{
			LOG_TRACE << "ShareMem::Create ShareMem Error SM_KET =" << key;
			return false;
		}
		m_pHeader = share_memory::MapShareMem(m_hold);

		if (!m_pHeader)
		{
			LOG_TRACE << "ShareMem::Map ShareMem Error SM_KET =" << key;//<< " errorno: " << GetLastError();
			return false;
		}

		m_pDataPtr = m_pHeader + sizeof(sm_head);
		((sm_head*)m_pHeader)->m_Key = key;		//attach 时比对用
		((sm_head*)m_pHeader)->m_Size = Size;		//attach 时比对用

		m_Size = Size;

		LOG_TRACE << "ShareMem::Create ShareMem Ok SM_KET =" << key;

		// 写入预分配的配置文件，方便守护进程进行加载 [7/18/2015 SEED]
		FILE *fp = fopen(SM_SIZE_FILE, "a");
		if (fp != nullptr) {
			fprintf(fp, "%lld,%d\n", key, Size);
			fclose(fp);
		}

		return true;
	}

	char* shm_static_block::GetTypePtr(uint32 tSize, uint32 tIndex)
	{
		ZXERO_ASSERT(tSize > 0);
		ZXERO_ASSERT(tSize*tIndex < m_Size);
		if (tSize <= 0 || tIndex >= m_Size)
			return nullptr;
		return m_pDataPtr + tSize*tIndex;
	}

	bool    shm_static_block::SizeCheck(sm_key key, uint32 Size)
	{
		key, Size;
		return false;
	}

	void	shm_static_block::Destory()
	{
		if (m_pHeader)
		{
			share_memory::UnMapShareMem(m_pHeader);
			m_pHeader = 0;
		}
		if (m_hold)
		{
			share_memory::CloseShareMem(m_hold);
			m_hold = 0;
		}

		m_Size = 0;
	}

	bool shm_static_block::Attach(sm_key key, uint32 Size)
	{
		m_hold = share_memory::OpenShareMem(key, Size);
		if (m_hold == INVALID_SM_HANDLE)
		{
			LOG_TRACE << "ShareMem::Attach ShareMem Error SM_KET = " << key;
			return false;
		}

		if (m_CmdArg != init_loadall)
		{
			// share memory 没关闭就走了创建sm引起的Assert！[7/23/2015 SEED]
			ZXERO_ASSERT(false) << "ShareMem::incorect init mode! key = " << key;
			return true;
		}

		m_pHeader = share_memory::MapShareMem(m_hold);

		if (!m_pHeader)
		{
			LOG_TRACE << "ShareMem::Map ShareMem Error SM_KET = " << key;
			return false;
		}
		m_pDataPtr = m_pHeader + sizeof(sm_head);
		ZXERO_ASSERT(((sm_head*)m_pHeader)->m_Key == key);
		ZXERO_ASSERT(((sm_head*)m_pHeader)->m_Size == Size);
		m_Size = Size;
		LOG_TRACE << "ShareMem::Attach ShareMem OK SM_KET = " << key;

		return true;
	}

	bool shm_static_block::DumpToFile(char* FilePath)
	{
		ZXERO_ASSERT(FilePath);

		FILE* f = fopen(FilePath, "wb");
		if (!f)
			return false;
		fwrite(m_pHeader, 1, m_Size, f);
		fclose(f);

		return true;
	}

	bool shm_static_block::MergeFromFile(char* FilePath)
	{
		ZXERO_ASSERT(FilePath);

		FILE*	f = fopen(FilePath, "rb");
		if (!f)
			return false;
		fseek(f, 0L, SEEK_END);
		INT FileLength = ftell(f);
		fseek(f, 0L, SEEK_SET);
		fread(m_pHeader, FileLength, 1, f);
		fclose(f);

		return true;
	}

	void shm_static_block::SetHeadVer(uint32 ver)
	{
		((sm_head*)m_pHeader)->m_HeadVer = ver;
	}

	uint32 shm_static_block::GetHeadVer()
	{
		uint32 ver = ((sm_head*)m_pHeader)->m_HeadVer;
		return ver;
	}

	int32	shm_static_block::GetAllocCount()
	{
		int32 count = ((sm_head*)m_pHeader)->m_AllocCount;
		return count;
	}
	void	shm_static_block::SetAllocCount(int32 count)
	{
		((sm_head*)m_pHeader)->m_AllocCount = count;
	}

	int32	shm_static_block::GetFreeIndex()
	{
		int32 index = ((sm_head*)m_pHeader)->m_CurFreeIndex;
		return index;
	}
	void	shm_static_block::SetFreeIndex(int32 index)
	{
		((sm_head*)m_pHeader)->m_CurFreeIndex = index;
	}
}