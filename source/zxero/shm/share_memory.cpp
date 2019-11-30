
#include "share_memory.h"
#include "log.h"
#if ZXERO_OS_LINUX
#include <sys/types.h> 
#include <sys/ipc.h> 
#include <sys/shm.h> 
#include <errno.h>
#elif ZXERO_OS_WINDOWS
#include <WinBase.h>
#endif

namespace share_memory
{
	SMHandle CreateShareMem(sm_key key,UINT Size)
	{
	#if ZXERO_OS_LINUX
		//先使用排他性创建判断共享内存是否已经创建
		SMHandle hd =  shmget(key ,Size,IPC_CREAT|IPC_EXCL|0666); //key 为0是指新建一块共享内存[IPC_PRIVATE]，如果指定了非 0 key，但实际创建出key值为0的共享内存，说明shmget失败了，小心！
		if(hd==-1){//说明已经有key对应的共享内存了，要做容错处理
			//取得已有的共享内存
			hd = shmget(key, Size, IPC_CREAT | 0666);
			if (hd == -1) {//彻底没法处理了
				LOG_ERROR << "shmget failed! handle = " << hd << " ,key = " << key << " ,errno: " << errno;
				return hd;
			}			

			//取得信息
			struct shmid_ds buf;
			hd = shmctl(hd, IPC_STAT, &buf);
			if (hd == -1) {
				LOG_ERROR << "handle = " << hd << " ,key = " << key << " ,errno: " << errno;
				return hd;
			}

			//检查挂接数
			if (buf.shm_nattch != 0) {//彻底没法处理了
				LOG_ERROR << "buf.shm_nattch != 0,handle = " << hd << " ,key = " << key;
				return hd;
			}

			//说明存在脏的共享内存，可以强制释放该内存
			hd = shmctl(hd, IPC_RMID, 0);
			if (hd == -1) {
				LOG_ERROR << "shmctl remove shm failed! handle = " << hd << " ,key = " << key << " ,errno: " << errno;
				return hd;
			}

			LOG_TRACE << "shmctl remove shm ok! try recreate handle = " << hd << " ,key = " << key;
			//重新分配一次
			hd = shmget(key, Size, IPC_CREAT | IPC_EXCL | 0666); //key 为0是指新建一块共享内存[IPC_PRIVATE]，如果指定了非 0 key，但实际创建出key值为0的共享内存，说明shmget失败了，小心！
			if (hd == -1) {//彻底没法处理了
				LOG_ERROR << "handle = " << hd << " ,key = " << key << " ,errno: " << errno;
				return hd;
			}
			return hd;
		}
		return hd;	

	#elif ZXERO_OS_WINDOWS
		char keybuf[64] ={0};
		zprintf(keybuf,sizeof(keybuf),"%lld",key);
		return  CreateFileMapping( (HANDLE)0xFFFFFFFFFFFFFFFF, NULL, PAGE_READWRITE, 0, Size, keybuf);
	#endif
	}

	SMHandle OpenShareMem(sm_key key,UINT Size)
	{
		Size;

	#if ZXERO_OS_LINUX
		////在共享内存挂接的时候，先使用排他性创建判断共享内存是否已经创建，如果还没创建则进行出错处理，若已经创建，则挂接
		//SMHandle hd = shmget(key, size, IPC_CREATE | IPC_EXCL);
		//if(-1 != hd){//说明没有可用的共享内存
		//	LOG_ERROR << "OpenShareMem failed! no match shm handle = " << hd << " ,key = " << key << " ,errno: " << errno;
		//	return -1
		//}
		SMHandle hd =  shmget(key , Size, 0);
		if(hd==-1){
			LOG_ERROR<<"handle = "<<hd<<" ,key = "<<key<<" ,errno: "<<errno;
		}		
		return hd;
	#elif ZXERO_OS_WINDOWS
		char keybuf[64]={0};
		zprintf(keybuf,sizeof(keybuf),"%lld",key);
		return OpenFileMapping( FILE_MAP_ALL_ACCESS, TRUE, keybuf);
	#endif
		//return SMHandle(-1);
	}

	char*	MapShareMem(SMHandle handle)
	{
	#if ZXERO_OS_LINUX
		void *addr = shmat(handle,0,0);
		if (addr == (void *)-1){
			return nullptr;
		}
		return (char*)addr;	
	#elif ZXERO_OS_WINDOWS
		return (char *)MapViewOfFile(handle, FILE_MAP_ALL_ACCESS, 0, 0, 0);
	#endif
	}


	void UnMapShareMem(char* MemoryPtr)
	{
	#if ZXERO_OS_LINUX
		  shmdt(MemoryPtr);
	#elif ZXERO_OS_WINDOWS
		 UnmapViewOfFile(MemoryPtr);
	#endif
	}


	void CloseShareMem(SMHandle handle)
	{
	#if ZXERO_OS_LINUX
		shmctl(handle,IPC_RMID,0); 
	#elif ZXERO_OS_WINDOWS
		CloseHandle(handle);
	#endif
	}
}