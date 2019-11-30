#ifndef _SHAREMEM_API_H_
#define _SHAREMEM_API_H_

#include "types.h"

using namespace zxero;

#include <vector>
#include <map>

typedef int16	ID_t;
typedef uint16	SMUID_t;
typedef	uint64	sm_key;
typedef uint32	ObjID_t;			//�����й̶�������OBJӵ�в�ͬ��ObjID_t
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

	/*����ShareMem �ڴ���
	 *	
	 *	key   ����ShareMem �Ĺؼ�ֵ
	 *
	 *  Size  ������С
	 *
	 *	���� ��ӦShareMem����ֵ
	 */
	SMHandle CreateShareMem(sm_key key,UINT Size);
	/*��ShareMem �ڴ���
	 *	
	 * key   ��ShareMem �Ĺؼ�ֵ
	 * 
	 * Size  �򿪴�С
	 *
	 * ����  ��ӦShareMem ����ֵ
	 */
	SMHandle OpenShareMem(sm_key key,UINT Size);
	
	/*ӳ��ShareMem �ڴ���
	 *	
	 *	handle ӳ��ShareMem �ı���ֵ
	 *
	 *  ���� ShareMem ������ָ��
	 */
	char*	MapShareMem(SMHandle handle);
	
	/*�ر�ӳ�� ShareMem �ڴ���
	 *
	 *	MemoryPtr			ShareMem ������ָ��
	 *	
	 *  
	 */	
	void	UnMapShareMem(char* MemoryPtr);
	
	/*	�ر�ShareMem
	 * 	handle  ��Ҫ�رյ�ShareMem ����ֵ
	 */
	void	CloseShareMem(SMHandle handle);


}


#endif

