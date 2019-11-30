
/********************************************************************
	created:	2013/12/25
	filename: 	CCDebugMemory.h
	author:		jingang.li(jingang.li@chukong-inc.com)
	
	purpose:	for memory trace, cross platform!

	Copyright (c) 2013 Chukong Technologies, Inc.
	Copyright (c) 2015 KooGame Inc.
*********************************************************************/

#ifndef DEBUG_MEMORY_H
#define DEBUG_MEMORY_H

#include "types.h"
#include "mutex.h"
#include "mem_allocator.h"
#include <map>


namespace zxero
{
	const int MAX_DIR_PATH = 255;

	struct alloc_info
	{
		void * m_address;
		uint32 m_size;
		char m_fileName[MAX_DIR_PATH+1];
		uint32 m_lineNum;
		uint32 m_account;

		alloc_info()
		{
			reset();
		}

		~alloc_info()
		{
			reset();
		}
		void reset()
		{
			m_address = 0;
			m_size = 0;
			memset(m_fileName,0, sizeof(m_fileName));
			m_lineNum = 0;
			m_account = 0;
		}

	};
	struct line_account
	{
		int m_times;
		int m_accountSize;

		line_account()
		{
			reset();
		}
		~line_account()
		{
			reset();
		}

		void reset()
		{
			m_times = 0;
			m_accountSize = 0;
		}
	};

	typedef std::basic_string<char,std::char_traits<char>,unzxero::dbg_allocator<char>> my_string;
	typedef std::map<int,line_account *,std::less<int>,unzxero::dbg_allocator<std::pair<int, line_account*>>> line_map;
	typedef std::map<void *,alloc_info *,std::less<void *>, unzxero::dbg_allocator<std::pair<void *, alloc_info*>>> mem_map;	
	typedef std::map<my_string,line_map *,std::less<my_string>, unzxero::dbg_allocator<std::pair<my_string, line_map*>>> account_map;

	class mem_debugger
	{
	public:
		mem_debugger();
		~mem_debugger();

		void init_debugger();
		void dump_unfreed(const char * fileName );		
		void add_anchor(void * addr, uint32 asize, const char *fname, uint32 lnum);
		void remove_anchor(void * addr);	

		static mem_debugger* instance();

	private:		
		void dump_account(float dt = 0.0f);
		void dump_new_account();
		void dump_del_account();
		void release_debugger();
	private:

		unsigned int s_allocSize;
		unsigned int s_freeSize;
		int s_sampleCount;
		uint64 s_sampleTime;
		bool s_isBegin;

		mem_map* s_allocMap;//.clear();
		mem_map* s_recycleMap;//.clear();
		account_map* s_accountNewMap;//.clear();
		account_map* s_accountDelMap;//.clear();

		unzxero::mutex m_mutex;
	};
}


//#ifndef _MEM_DEBUG
//#define _MEM_DEBUG
//#endif

#ifdef _MEM_DEBUG

inline void * dbg_malloc( size_t size, const char *file, int line)
{
	void *ptr = (void *)malloc(size);
	//void *ptr = ::operator new(size);
	zxero::mem_debugger::instance()->add_anchor(ptr, (zxero::uint32)size, file, line);
	return(ptr);
}

inline void dbg_free( void *ptr, const char *file, int line )
{
	if( ptr == nullptr )
		return;

	zxero::mem_debugger::instance()->remove_anchor(ptr);
	free(ptr);
}

inline void * dbg_calloc( size_t count, size_t size, const char *file, int line)
{
	void *ptr = (void *)malloc(size*count);

	memset(ptr,0,size*count);
	zxero::mem_debugger::instance()->add_anchor(ptr, (zxero::uint32)(size*count), file, line);
	return(ptr);
}


inline void * dbg_realloc( void* ptr, size_t size, const char *file, int line)
{
	dbg_free(ptr, file, line);

	return dbg_malloc( size, file, line );
}
#define malloc(A) dbg_malloc( (A),__FILE__, __LINE__ )
#define calloc(A, B) dbg_calloc( (A), (B),__FILE__, __LINE__  )
#define realloc(A, B) dbg_realloc( (A), (B),__FILE__, __LINE__ )
#define free(A) dbg_free( (A), __FILE__, __LINE__ )

inline void * operator new(size_t size, const char *file, int line)
{
	//void *ptr = (void *)dbg_malloc( size, file, line );
	void *ptr = ::operator new(size);
	zxero::mem_debugger::instance()->add_anchor(ptr, (zxero::uint32)size, file, line);
	return(ptr);
}

inline void operator delete(void *p, const char *file, int line)
{
	if( p == nullptr )
		return;

	dbg_free(p, nullptr, -1);
};

inline void * operator new[](size_t size,const char *file, int line)
{	
	return (operator new(size, file, line));
};

inline void operator delete[](void *p, const char *file, int line)
{	
	operator delete(p, file, line);
};

#endif


#ifdef _MEM_DEBUG
#define SAFE_NEW new(__FILE__, __LINE__)
#define SAFE_NEW_ARRAY new[](__FILE__, __LINE__)
#define SAFE_DELETE(p)           mem_debugger::instance()->remove_anchor(p);\
	do {  delete (p); (p) = nullptr; } while(0)
#define SAFE_DELETE_ARRAY(p)     mem_debugger::instance()->remove_anchor(p);\
	do { if(p) { delete[] (p); (p) = nullptr; } } while(0)
#else
#define SAFE_NEW new
#define SAFE_NEW_ARRY new[]
#define SAFE_DELETE(p)	do {  delete (p); (p) = nullptr; } while(0)
#define SAFE_DELETE_ARRAY(p) do { if(p) { delete[] (p); (p) = nullptr; } } while(0)
#endif
//	/////////////////////
//	#ifdef ZXERO_OS_LINUX
//		#define DEBUG_DEL delete
//		#define DEBUG_DEL_A delete[]
//	#endif
//	//////////////
//#else
//
//	#define DEBUG_NEW new
//
//	/////////////////////
//	#ifdef ZXERO_OS_LINUX
//		#define DEBUG_DEL delete
//	#endif
//	//////////////
//
//#endif
//
//#ifdef ZXERO_OS_WINDOWS
//	#define new DEBUG_NEW
//	#pragma warning(disable:4291)
//	#pragma warning(disable:4345)
//#else
//	#define new DEBUG_NEW
//	#define delete DEBUG_DEL


#endif //DEBUG_MEMORY_H