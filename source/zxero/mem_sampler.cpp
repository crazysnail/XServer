
#include "mem_sampler.h"


#if  USE_MEMEORY_SAMPLER && WIN32


MemorySampler * MemorySampler::getInstance()
{
	static MemorySampler s_debugMemory;
	return &s_debugMemory;
}

MemorySampler::MemorySampler()
{

	m_sampleCount = 0;
	m_sampleTime = DUMP_MEM_TRACE_ACCOUNT;
	//MemoryDebugger::initDebugger();
	init();
}

MemorySampler::~MemorySampler()
{
	MemoryDebugger::releaseDebugger();
	m_sampleCount = 0;
	m_sampleTime = DUMP_MEM_TRACE_ACCOUNT;
}

bool MemorySampler::init()
{
	//startSample();
	return true;
}

void MemorySampler::startSample( float time )
{
	//CCAssert( time > 0.0f, "invalid sample time!");
	m_sampleTime = time;
	//DebugMem::initDebugMemory();	
	//Director::getInstance()->getScheduler()->schedule(schedule_selector(MemorySampler::dumpAccount), this, m_sampleTime, -1, 0, false);
}
void MemorySampler::endSample()
{
	//Director::getInstance()->getScheduler()->unschedule(schedule_selector(MemorySampler::dumpAccount),this);
}


//void DebugMemory::releaseMapData()
//{
//	MemMap::iterator iter;
//	for(iter = DebugMem::s_allocMap.begin(); iter != DebugMem::s_allocMap.end(); ++iter)
//	{
//		delete iter->second;
//	}
//
//	AccountMap::iterator accountIter;
//	LineMap::iterator lineIter;
//
//	for(accountIter = DebugMem::s_accountNewMap.begin(); accountIter != DebugMem::s_accountNewMap.end(); ++accountIter)
//	{
//		for( lineIter = accountIter->second.begin(); lineIter != accountIter->second.end(); ++lineIter )
//		{
//			delete lineIter->second;
//		}
//
//		LineMap &line = accountIter->second;
//		line.clear();
//	}
//
//	for(accountIter = DebugMem::s_accountDelMap.begin(); accountIter != DebugMem::s_accountDelMap.end(); ++accountIter)
//	{
//		for( lineIter = accountIter->second.begin(); lineIter != accountIter->second.end(); ++lineIter )
//		{
//			delete lineIter->second;
//		}
//
//		LineMap &line = accountIter->second;
//		line.clear();
//	}
//
//	DebugMem::s_allocMap.clear();
//	DebugMem::s_accountNewMap.clear();
//	DebugMem::s_accountDelMap.clear();
//}
//#ifdef new
//#undef new
//void DebugMemory::addTraceInfo(uint32 addr, uint32 asize, const char *fname, uint32 lnum)
//{
//	stAllocInfo *info;
//	info = new(stAllocInfo);
//	info->m_address = addr;
//	strncpy(info->m_fileName, fname, MAX_DIR_PATH);
//	info->m_lineNum = lnum;
//	info->m_size = asize;
//	DebugMem::s_allocMap.insert(std::make_pair(addr,info));
//
//	std::string name(fname);
//	AccountMap::iterator fIter = DebugMem::s_accountNewMap.find(name);
//	if( fIter != DebugMem::s_accountNewMap.end() )
//	{
//		LineMap::iterator &lIter = fIter->second.find(lnum);
//		if(lIter != fIter->second.end())
//		{
//			stLineAccount *lineInfo = lIter->second;
//			lineInfo->m_times++;
//			lineInfo->m_accountSize += info->m_size;
//		}
//		else
//		{
//			stLineAccount *lineInfo = new(stLineAccount);
//			lineInfo->m_times = 1;
//			lineInfo->m_accountSize += info->m_size;
//			fIter->second.insert(std::make_pair(lnum,lineInfo));
//		}
//	}
//	else
//	{
//		LineMap line;
//		stLineAccount *lineInfo = new(stLineAccount);
//		lineInfo->m_times = 1;
//		lineInfo->m_accountSize += info->m_size;
//
//		line.insert(std::make_pair(lnum,lineInfo));
//		DebugMem::s_accountNewMap.insert(std::make_pair(name,line));
//	}
//
//	DebugMem::s_allocSize += asize;
//}
//void DebugMemory::removeTraceInfo(uint32 addr)
//{
//
//	MemMap::iterator iter = DebugMem::s_allocMap.find(addr);
//	if( iter != DebugMem::s_allocMap.end() )
//	{
//		stAllocInfo* info = iter->second;
//
//		//account delete info
//		std::string name(info->m_fileName);
//		int lnum = info->m_lineNum;
//		AccountMap::iterator fIter = DebugMem::s_accountDelMap.find(name);
//		if( fIter != DebugMem::s_accountDelMap.end() )
//		{
//			LineMap::iterator &lIter = fIter->second.find(lnum);
//			if(lIter != fIter->second.end())
//			{
//				stLineAccount *lineInfo = lIter->second;
//				lineInfo->m_times++;
//				lineInfo->m_accountSize += info->m_size;
//			}
//			else
//			{
//				stLineAccount *lineInfo = new(stLineAccount);
//				lineInfo->m_times = 1;
//				lineInfo->m_accountSize += info->m_size;
//				fIter->second.insert(std::make_pair(lnum,lineInfo));
//			}
//		}
//		else
//		{
//			LineMap line;
//			stLineAccount *lineInfo = new(stLineAccount);
//			lineInfo->m_times = 1;
//			lineInfo->m_accountSize += info->m_size;
//
//			line.insert(std::make_pair(lnum,lineInfo));
//			DebugMem::s_accountDelMap.insert(std::make_pair(name,line));
//		}
//
//
//		DebugMem::s_freeSize += info->m_size;
//		delete iter->second;
//		DebugMem::s_allocMap.erase(iter);
//	}
//}
//#endif

void MemorySampler::dumpAccount( float dt)
{
	MemoryDebugger::dumpAccount( m_sampleTime );
}


void MemorySampler::dumpUnfreed()
{
	MemoryDebugger::dumpUnfreed("debug_memory.txt");
} 


#endif  //  USE_MEMEORY_SAMPLER