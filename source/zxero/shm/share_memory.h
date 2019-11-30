#ifndef _SHAREMEM_API_H_
#define _SHAREMEM_API_H_

#include "types.h"

using namespace zxero;

#include <vector>
#include <map>

typedef int16	ID_t;
typedef uint16	SMUID_t;
typedef	uint64	sm_key;
typedef uint32	ObjID_t;			//场景中固定的所有OBJ拥有不同的ObjID_t
typedef uint32  UINT;
typedef int32   INT;

#if defined(ZXERO_OS_LINUX)
typedef		int		SMHandle;
#elif defined(ZXERO_OS_WINDOWS)
typedef		void*	SMHandle;
#endif


#ifdef	ZXERO_OS_WINDOWS
#define INVALID_SM_HANDLE	 ((void*)0)
#elif	 ZXERO_OS_LINUX
#define INVALID_SM_HANDLE	 -1
#endif


#define SM_SIZE_FILE "./share_mem_size.conf"

namespace	share_memory
{

	/*创建ShareMem 内存区
	 *	
	 *	key   创建ShareMem 的关键值
	 *
	 *  Size  创建大小
	 *
	 *	返回 对应ShareMem保持值
	 */
	SMHandle CreateShareMem(sm_key key,UINT Size);
	/*打开ShareMem 内存区
	 *	
	 * key   打开ShareMem 的关键值
	 * 
	 * Size  打开大小
	 *
	 * 返回  对应ShareMem 保持值
	 */
	SMHandle OpenShareMem(sm_key key,UINT Size);
	
	/*映射ShareMem 内存区
	 *	
	 *	handle 映射ShareMem 的保持值
	 *
	 *  返回 ShareMem 的数据指针
	 */
	char*	MapShareMem(SMHandle handle);
	
	/*关闭映射 ShareMem 内存区
	 *
	 *	MemoryPtr			ShareMem 的数据指针
	 *	
	 *  
	 */	
	void	UnMapShareMem(char* MemoryPtr);
	
	/*	关闭ShareMem
	 * 	handle  需要关闭的ShareMem 保持值
	 */
	void	CloseShareMem(SMHandle handle);


}


#endif

