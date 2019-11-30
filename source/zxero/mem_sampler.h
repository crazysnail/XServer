
/********************************************************************
	created:	2013/12/25
	filename: 	MemorySanpler.h
	author:		jingang.li(jingang.li@chukong-inc.com)
	
	purpose:	for memory trace

	Copyright (c) 2013 Chukong Technologies, Inc.
*********************************************************************/

#ifndef MEMORY_SAMPLER_H
#define MEMORY_SAMPLER_H


//#include "mem_debugger.h"
#define  USE_MEMEORY_SAMPLER 0

#if  USE_MEMEORY_SAMPLER && WIN32

const float DUMP_MEM_TRACE_ACCOUNT  = 15.0f;

class MemorySampler
{
public:
	MemorySampler();
	~MemorySampler();

	virtual bool init();

	static MemorySampler* getInstance();
	
	void dumpUnfreed();
	void dumpAccount(float dt = 0.0f);
	void startSample( float time = DUMP_MEM_TRACE_ACCOUNT );
	void endSample();

private:
	float m_sampleTime;
	int m_sampleCount;
};



//////////////////////////////
#if !REDEFINE_PLACEMENT_NEW
#ifdef _MEM_DEBUG
inline void * __cdecl operator new(size_t size, const char *file, int line)
{
	void *ptr = (void *)dbg_malloc( size, file, line );
	uint32 addr = (uint32)ptr;
	return(ptr);
}

inline void __cdecl operator delete(void *p)
{
	if( p == nullptr )
		return;

	uint32 addr = (uint32)p;
	dbg_free(p, nullptr, -1);
};

inline void *__cdecl operator new[](size_t size,const char *file, int line)
{	
	return (operator new(size, file, line));
}

inline void __cdecl operator delete[](void *p)
{	
	operator delete(p);
}

#define DEBUG_NEW new(__FILE__, __LINE__)
#define DEBUG_NEW_A new[](__FILE__, __LINE__)


#else

#define DEBUG_NEW new

#endif

#define new DEBUG_NEW
#pragma warning(disable:4291)

#endif //!REDEFINE_PLACEMENT_NEW


#endif //USE_MEMEORY_SAMPLER

#endif //MEMORY_SAMPLER_H