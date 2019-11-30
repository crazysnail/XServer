#include <iostream>
#include "date_time.h"
#include "mem_debugger.h"


#define  CONSOLE_DUMP 0
#define  OUTPUT_DUMP 0
#define  FILE_DUMP 1

namespace zxero
{

#undef malloc
#undef free
#undef new
#undef delete

	mem_debugger::mem_debugger()
	{
		s_allocSize = 0;
		s_freeSize = 0;
		s_sampleCount = 0;
		s_sampleTime = 0;
		s_isBegin = false;

		init_debugger();
	}

	mem_debugger::~mem_debugger()
	{
		release_debugger();
	}

	void mem_debugger::init_debugger()
	{
		s_isBegin = true;
		s_allocMap = new mem_map();
		s_recycleMap = new mem_map();
		s_accountNewMap = new account_map();
		s_accountDelMap = new account_map();
	}

	mem_debugger* mem_debugger::instance()
	{
		static mem_debugger sg_data_manager;
		return &sg_data_manager;
	}

	void mem_debugger::release_debugger()
	{
		mem_map::iterator iter;
		for(iter = s_allocMap->begin(); iter != s_allocMap->end(); ++iter)
		{
			free(iter->second);
		}

		for(iter = s_recycleMap->begin(); iter != s_recycleMap->end(); ++iter)
		{
			free(iter->second);
		}

		account_map::iterator accountIter;
		line_map::iterator lineIter;

		for(accountIter = s_accountNewMap->begin(); accountIter != s_accountNewMap->end(); ++accountIter)
		{
			for( lineIter = accountIter->second->begin(); lineIter != accountIter->second->end(); ++lineIter )
			{
				free(lineIter->second);
			}

			line_map *line = accountIter->second;
			line->clear();
			delete line;
		}

		for(accountIter = s_accountDelMap->begin(); accountIter != s_accountDelMap->end(); ++accountIter)
		{
			for( lineIter = accountIter->second->begin(); lineIter != accountIter->second->end(); ++lineIter )
			{
				free(lineIter->second);
			}

			line_map *line = accountIter->second;
			line->clear();
			delete line;
		}

		s_recycleMap->clear();
		s_allocMap->clear();
		s_accountNewMap->clear();
		s_accountDelMap->clear();

		delete s_allocMap;
		delete s_recycleMap;		
		delete s_accountNewMap;
		delete s_accountDelMap;
	}
	void mem_debugger::add_anchor(void * addr, uint32 asize, const char *fname, uint32 lnum)
	{
		unzxero::auto_lock autolock(m_mutex);

		if( s_allocMap == nullptr )
			return;

		alloc_info *info;
		info = (alloc_info *)malloc(sizeof(alloc_info));
		info->reset();
		info->m_address = addr;
		strncpy(info->m_fileName, fname, MAX_DIR_PATH);
		info->m_lineNum = lnum;
		info->m_size = asize;
		s_allocMap->insert(std::make_pair(addr,info));

		my_string name(fname);
		account_map::iterator fIter = s_accountNewMap->find(name);
		if( fIter != s_accountNewMap->end() )
		{
			const auto &iter = fIter->second->find(lnum);
			if(iter != fIter->second->end())
			{
				line_account *lineInfo = iter->second;
				lineInfo->m_times++;
				lineInfo->m_accountSize += info->m_size;
			}
			else
			{
				line_account *lineInfo = (line_account *)malloc(sizeof(line_account));
				lineInfo->reset();
				lineInfo->m_times = 1;
				lineInfo->m_accountSize += info->m_size;
				fIter->second->insert(std::make_pair(lnum,lineInfo));
			}
		}
		else
		{
			line_map *line = new line_map();
			line_account *lineInfo = (line_account *)malloc(sizeof(line_account));
			lineInfo->reset();
			lineInfo->m_times = 1;
			lineInfo->m_accountSize += info->m_size;

			line->insert(std::make_pair(lnum,lineInfo));
			s_accountNewMap->insert(std::make_pair(name,line));
		}

		s_allocSize += asize;
	}
	void mem_debugger::remove_anchor(void * addr)
	{
		unzxero::auto_lock autolock(m_mutex);

		if( s_allocMap == nullptr || s_allocMap->empty() )
			return;

		mem_map::iterator iter = s_allocMap->find(addr);
		if( iter != s_allocMap->end() )
		{
			alloc_info* info = iter->second;

			//account delete info
			my_string name(info->m_fileName);
			int lnum = info->m_lineNum;
			account_map::iterator fIter = s_accountDelMap->find(name);
			if( fIter != s_accountDelMap->end() )
			{
				const auto &sub_iter = fIter->second->find(lnum);
				if(sub_iter != fIter->second->end())
				{
					line_account *lineInfo = sub_iter->second;
					lineInfo->m_times++;
					lineInfo->m_accountSize += info->m_size;
				}
				else
				{
					line_account *lineInfo = (line_account *)malloc(sizeof(line_account));
					lineInfo->reset();
					lineInfo->m_times = 1;
					lineInfo->m_accountSize += info->m_size;
					fIter->second->insert(std::make_pair(lnum,lineInfo));
				}
			}
			else
			{
				line_map *line = new line_map();
				line_account *lineInfo = (line_account *)malloc(sizeof(line_account));
				lineInfo->reset();
				lineInfo->m_times = 1;
				lineInfo->m_accountSize += info->m_size;

				line->insert(std::make_pair(lnum,lineInfo));
				s_accountDelMap->insert(std::make_pair(name,line));
			}


			s_freeSize += info->m_size;
			free(info);		
			s_allocMap->erase(iter);

			return;
		}

		return;
	}



	void mem_debugger::dump_account( float dt)
	{
		s_sampleTime = (uint64)dt;

		dump_new_account();
		dump_del_account();
		
		s_sampleCount++;
	}

	void mem_debugger::dump_new_account()
	{
		if( s_accountNewMap == nullptr || s_accountNewMap->empty() )
			return;

		account_map::iterator iter;
		line_map::iterator lIter;
		char buf[1024]={0};

		//sample info
#if CONSOLE_DUMP
		zprintf(buf,sizeof(buf),"SamepleId=%d,Sample Time=%llu seconds \n", m_sampleCount, s_sampleTime );
		zprintf(buf,sizeof(buf),"Current Alloc Memory Size =%d byte (new Size[%d] - del Size[%d]) \n", s_allocSize-s_freeSize, s_allocSize,s_freeSize );
		printf(buf);
#endif
#if OUTPUT_DUMP
		zprintf(buf,sizeof(buf),"SamepleId=%d,Sample Time=%llu seconds \n", m_sampleCount, s_sampleTime );
		zprintf(buf,sizeof(buf),"Current Alloc Memory Size =%d byte (new Size[%d] - del Size[%d]) \n", s_allocSize-s_freeSize, s_allocSize,s_freeSize );
		CCLog(buf);
#endif

#if FILE_DUMP
		zprintf(buf,sizeof(buf),"debug_memory_new_sample_%d.txt",s_sampleCount );
		FILE *fp = fopen(buf, "a");
		if(s_sampleTime == 0 )
			s_sampleTime = now_second();

		fprintf(fp, "SamepleId=%d,Sample Time=%llu seconds \n", s_sampleCount, s_sampleTime );
		//fprintf(fp, "Current Alloc Memory Size =%d byte (new Size[%d] - del Size[%d]) \n", s_allocSize-s_freeSize, s_allocSize,s_freeSize );
		fprintf(fp, "Current Alloc Memory Size =%.2fkb (new Size[%.2f]kb - del Size[%.2f]kb) \n", (s_allocSize-s_freeSize)/1024.0f, s_allocSize/1024.0f,s_freeSize/1024.0f);
		fclose(fp);
#endif


		for(iter = s_accountNewMap->begin(); iter != s_accountNewMap->end(); ++iter)
		{

			//file name
#if CONSOLE_DUMP
			zprintf(buf,sizeof(buf),"File=%s\n", iter->first.c_str() );
			printf(buf);
#endif
#if OUTPUT_DUMP
			zprintf(buf,sizeof(buf),"File=%s\n", iter->first.c_str() );
			CCLog(buf);
#endif

#if FILE_DUMP
			FILE *sub_fp = fopen(buf, "a");
			fprintf(sub_fp, "File=%s\n", iter->first.c_str() );
			fclose(sub_fp);
#endif

			//account file new info
			for(lIter = iter->second->begin(); lIter != iter->second->end(); ++lIter )
			{
#if CONSOLE_DUMP
				zprintf(buf,sizeof(buf),"Line=%d, mem times=%d, account size=%d byte!\n", lIter->first, lIter->second->m_times,lIter->second->m_accountSize );
				printf(buf);
#endif

#if OUTPUT_DUMP
				zprintf(buf,sizeof(buf),"Line=%d, mem times=%d, account size=%d byte!\n", lIter->first, lIter->second->m_times,lIter->second->m_accountSize );
				CCLog(buf);
#endif

#if FILE_DUMP
				FILE *dump_fp = fopen(buf, "a");
				//fprintf(fp, "Line=%d, mem new times=%d, account size=%d byte!\n", lIter->first, lIter->second->m_times,lIter->second->m_accountSize );
				fprintf(dump_fp, "Line=%d, mem times=%d, account size=%.2fkb!\n", lIter->first, lIter->second->m_times,(lIter->second->m_accountSize)/1024.0f );
				fclose(dump_fp);
#endif
			}


		}
	}

	void mem_debugger::dump_del_account()
	{
		if( s_accountDelMap == nullptr || s_accountDelMap->empty() )
			return;

		account_map::iterator iter;
		line_map::iterator lIter;
		char buf[1024]={0};

		//sample info
#if CONSOLE_DUMP
		zprintf(buf,sizeof(buf),"SamepleId=%d,Sample Time=%llu seconds \n", m_sampleCount, s_sampleTime );
		zprintf(buf,sizeof(buf),"Current Alloc Memory Size =%d byte (new Size[%d] - del Size[%d]) \n", s_allocSize-s_freeSize, s_allocSize,s_freeSize );
		printf(buf);
#endif
#if OUTPUT_DUMP
		zprintf(buf,sizeof(buf),"SamepleId=%d,Sample Time=%llu seconds \n", m_sampleCount, s_sampleTime );
		zprintf(buf,sizeof(buf),"Current Alloc Memory Size =%d byte (new Size[%d] - del Size[%d]) \n", s_allocSize-s_freeSize, s_allocSize,s_freeSize );
		CCLog(buf);
#endif

#if FILE_DUMP
		zprintf(buf,sizeof(buf),"debug_memory_del_sample_%d.txt",s_sampleCount );
		FILE *fp = fopen(buf, "a");
		fprintf(fp, "SamepleId=%d,Sample Time=%llu seconds \n", s_sampleCount, s_sampleTime );
		//fprintf(fp, "Current Alloc Memory Size =%d byte (new Size[%d] - del Size[%d]) \n", s_allocSize-s_freeSize, s_allocSize,s_freeSize );
		fprintf(fp, "Current Alloc Memory Size =%.2fkb (new Size[%.2fkb] - del Size[%.2fkb]) \n", (s_allocSize-s_freeSize)/1024.0f, s_allocSize/1024.0f,s_freeSize/1024.0f);
		fclose(fp);
#endif

		for(iter = s_accountDelMap->begin(); iter != s_accountDelMap->end(); ++iter)
		{

			//file name
#if CONSOLE_DUMP
			zprintf(buf,sizeof(buf),"File=%s\n", iter->first.c_str() );
			printf(buf);
#endif
#if OUTPUT_DUMP
			zprintf(buf,sizeof(buf),"File=%s\n", iter->first.c_str() );
			CCLog(buf);
#endif

#if FILE_DUMP
			FILE *sub_fp = fopen(buf, "a");
			fprintf(sub_fp, "File=%s\n", iter->first.c_str());
			fclose(sub_fp);
#endif

			//account file new info
			for(lIter = iter->second->begin(); lIter != iter->second->end(); ++lIter )
			{
#if CONSOLE_DUMP
				zprintf(buf,sizeof(buf),"Line=%d, mem times=%d, account size=%d byte!\n", lIter->first, lIter->second->m_times,lIter->second->m_accountSize );
				printf(buf);
#endif

#if OUTPUT_DUMP
				zprintf(buf,sizeof(buf),"Line=%d, mem times=%d, account size=%d byte!\n", lIter->first, lIter->second->m_times,lIter->second->m_accountSize );
				CCLog(buf);
#endif

#if FILE_DUMP
				FILE *dump_fp = fopen(buf, "a");
				//fprintf(fp, "Line=%d, mem del times=%d, account size=%d byte!\n", lIter->first, lIter->second->m_times,lIter->second->m_accountSize );
				fprintf(dump_fp, "Line=%d, mem times=%d, account size=%.2fkb!\n", lIter->first, lIter->second->m_times,(lIter->second->m_accountSize)/1024.0f );
				fclose(dump_fp);
#endif
			}


		}
	}

	void mem_debugger::dump_unfreed( const char * fileName )
	{
		//ZXERO_ASSERT( fileName != nullptr );
		if( s_allocMap == nullptr )
			return;

		mem_map::iterator iter;
		uint32 totalSize = 0;
		char buf[1024]={0};
		for(iter = s_allocMap->begin(); iter != s_allocMap->end(); ++iter)
		{
			alloc_info *pInfo = iter->second;
#if CONSOLE_DUMP
			zprintf(buf,sizeof(buf),"%s:LINE=%d, ADDRESS=%d, unfreed BTYE=%d!\n", pInfo->m_fileName, pInfo->m_lineNum, pInfo->m_address, pInfo->m_size );
			printf(buf);
#endif

#if OUTPUT_DUMP
			zprintf(buf,sizeof(buf),"%s:LINE=%d, ADDRESS=%d, unfreed BTYE=%d!\n", pInfo->m_fileName, pInfo->m_lineNum, pInfo->m_address, pInfo->m_size );
			CCLog(buf);
#endif

#if FILE_DUMP
			FILE *fp = fopen(fileName, "a");
			//FILE *fp = fopen("debug_memory.txt", "a");
			fprintf(fp, "%s:LINE=%d, ADDRESS=%d, unfreed=%.2f!\n", pInfo->m_fileName, pInfo->m_lineNum, (long)pInfo->m_address, (pInfo->m_size)/1024.0f );
			fclose(fp);
#endif
			totalSize += pInfo->m_size;

		}

#if CONSOLE_DUMP
		zprintf(buf,sizeof(buf),"Total not free: %d bytes\n", totalSize);
		printf(buf);
#endif

#if OUTPUT_DUMP
		zprintf(buf,sizeof(buf),"Total not free: %d bytes\n", totalSize);
		CCLog(buf);
#endif

#if FILE_DUMP
		//FILE *fp = fopen("debug_memory.txt", "a");
		FILE *fp = fopen(fileName, "a");
		fprintf(fp, "Total not free: %d bytes\n", totalSize);
		fclose(fp);
#endif

		dump_account();
	} 


}

/************************************************************************/

#if REDEFINE_PLACEMENT_NEW

void* operator new( size_t size ) throw ( std::bad_alloc )
{
	//return malloc( size );//, "debugger", -1 );

	//pos = getFileLine(1);
	//dbgprint(( DMEMORY, "%s:%d: new( %d ) [%p]", pos.file, pos.line, size, ret ));

	void *ptr = (void *)malloc(size);
	uint32 addr = (uint32)ptr;
	mem_debugger::instance()->add_anchor(addr, (uint32)size, "debugger", -1 );
	return(ptr);
}

void* operator new[](size_t size) throw (std::bad_alloc )
{
	return (operator new(size));
}

void operator delete( void* ptr ) throw ()
{
	//free( ptr );//, "debugger", -1 );
	//return;

	//pos = getFileLine(2);
	//dbgprint(( DMEMORY, "%s:%d: delete [%p]", pos.file, pos.line, ptr ));

	if( ptr == nullptr )
		return;

	uint32 addr = (uint32)ptr;
	mem_debugger::instance()->remove_anchor(addr);
	free(ptr);
}

void operator delete[]( void * ptr )
{
	operator delete(ptr);
}
#endif // REDEFINE_PLACEMENT_NEW