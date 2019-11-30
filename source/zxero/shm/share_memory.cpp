
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
		//��ʹ�������Դ����жϹ����ڴ��Ƿ��Ѿ�����
		SMHandle hd =  shmget(key ,Size,IPC_CREAT|IPC_EXCL|0666); //key Ϊ0��ָ�½�һ�鹲���ڴ�[IPC_PRIVATE]�����ָ���˷� 0 key����ʵ�ʴ�����keyֵΪ0�Ĺ����ڴ棬˵��shmgetʧ���ˣ�С�ģ�
		if(hd==-1){//˵���Ѿ���key��Ӧ�Ĺ����ڴ��ˣ�Ҫ���ݴ���
			//ȡ�����еĹ����ڴ�
			hd = shmget(key, Size, IPC_CREAT | 0666);
			if (hd == -1) {//����û��������
				LOG_ERROR << "shmget failed! handle = " << hd << " ,key = " << key << " ,errno: " << errno;
				return hd;
			}			

			//ȡ����Ϣ
			struct shmid_ds buf;
			hd = shmctl(hd, IPC_STAT, &buf);
			if (hd == -1) {
				LOG_ERROR << "handle = " << hd << " ,key = " << key << " ,errno: " << errno;
				return hd;
			}

			//���ҽ���
			if (buf.shm_nattch != 0) {//����û��������
				LOG_ERROR << "buf.shm_nattch != 0,handle = " << hd << " ,key = " << key;
				return hd;
			}

			//˵��������Ĺ����ڴ棬����ǿ���ͷŸ��ڴ�
			hd = shmctl(hd, IPC_RMID, 0);
			if (hd == -1) {
				LOG_ERROR << "shmctl remove shm failed! handle = " << hd << " ,key = " << key << " ,errno: " << errno;
				return hd;
			}

			LOG_TRACE << "shmctl remove shm ok! try recreate handle = " << hd << " ,key = " << key;
			//���·���һ��
			hd = shmget(key, Size, IPC_CREAT | IPC_EXCL | 0666); //key Ϊ0��ָ�½�һ�鹲���ڴ�[IPC_PRIVATE]�����ָ���˷� 0 key����ʵ�ʴ�����keyֵΪ0�Ĺ����ڴ棬˵��shmgetʧ���ˣ�С�ģ�
			if (hd == -1) {//����û��������
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
		////�ڹ����ڴ�ҽӵ�ʱ����ʹ�������Դ����жϹ����ڴ��Ƿ��Ѿ������������û��������г��������Ѿ���������ҽ�
		//SMHandle hd = shmget(key, size, IPC_CREATE | IPC_EXCL);
		//if(-1 != hd){//˵��û�п��õĹ����ڴ�
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