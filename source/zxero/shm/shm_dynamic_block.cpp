#include "shm_dynamic_block.h"
#include "log.h"

namespace share_memory {

	bool shm_dynamic_block::shm_create(sm_key key, uint32 size)
	{
		shm_destroy();

		auto final_size = shm_pre_calc_size(size);

		m_hold = share_memory::CreateShareMem(key, final_size);
		if (m_hold == INVALID_SM_HANDLE){
			LOG_ERROR << "shm_dynamic_block::CreateShareMem error SM_KET =" << key;
			return false;
		}

		char *pheader = share_memory::MapShareMem(m_hold);
		if (!pheader){
			LOG_ERROR << "shm_dynamic_block::MapShareMem error SM_KET =" << key;
			return false;
		}

		m_header = (shm_block_head *)pheader;		
		m_header->m_key = key;					//attach 时比对用
		m_header->m_size = final_size;			//attach 时比对用
		m_all_size = final_size;

		if (!shm_create_init(size)) {
			return false;
		}

		LOG_INFO << "ShareMem::Create ShareMem Ok SM_KET =" << key<<"try create size=" << size <<" final_size="<< final_size;

		return true;
	}

	bool shm_dynamic_block::shm_attach(sm_key key, uint32 size)
	{
		auto final_size = shm_pre_calc_size(size);

		m_hold = share_memory::OpenShareMem(key, final_size);
		if (m_hold == INVALID_SM_HANDLE){
			LOG_INFO << "shm_dynamic_block::shm_attach error SM_KET =" << key;
			return false;
		}

		char *pheader = share_memory::MapShareMem(m_hold);
		if (!pheader) {
			LOG_INFO << "shm_dynamic_block::shm_attach error SM_KET =" << key;
			return false;
		}

		m_header = (shm_block_head *)pheader;
		ZXERO_ASSERT(m_header->m_key == key);
		ZXERO_ASSERT(m_header->m_size == final_size);
		m_all_size = final_size;

		if (!shm_attach_init(final_size)) {
			return false;
		}

		LOG_INFO << "ShareMem::Attach ShareMem Ok SM_KET =" << key << "try create size=" << size <<" final_size=" << final_size;

		return true;
	}

	void shm_dynamic_block::shm_destroy()
	{
		if (m_header){
			share_memory::UnMapShareMem((char *)m_header);
			m_header = 0;
		}
		if (m_hold){
			share_memory::CloseShareMem(m_hold);
			m_hold = 0;
		}

		m_all_size = 0;
	}
	
	bool shm_dynamic_block::dump_to_file(char* file_path)
	{
		ZXERO_ASSERT(file_path);

		FILE* f = fopen(file_path, "wb");
		if (!f)
			return false;
		fwrite(m_header, 1, m_all_size, f);
		fclose(f);

		return true;
	}

	bool shm_dynamic_block::merge_from_file(char* file_path)
	{
		ZXERO_ASSERT(file_path);

		FILE*	f = fopen(file_path, "rb");
		if (!f)
			return false;
		fseek(f, 0L, SEEK_END);
		INT FileLength = ftell(f);
		fseek(f, 0L, SEEK_SET);
		fread(m_header, FileLength, 1, f);
		fclose(f);

		return true;
	}	
}