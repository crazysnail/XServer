//b.cpp
#include <iostream>
#include <Windows.h>
#include <tchar.h>
using namespace std;

#define BUF_SIZE 256
TCHAR szName[]=TEXT("Global[0]");    //ָ��ͬһ�鹲���ڴ������

int main()
{
	HANDLE hMapFile;
	LPCTSTR pBuf;

	hMapFile = CreateFileMapping(
		INVALID_HANDLE_VALUE,    // use paging file
		NULL,                    // default security
		PAGE_READWRITE,          // read/write access
		0,                       // maximum object size (high-order DWORD)
		BUF_SIZE,                // maximum object size (low-order DWORD)
		szName);                 // name of mapping object

	if (hMapFile == NULL)
	{
		_tprintf(TEXT("Could not create file mapping object (%d).\n"),
			GetLastError());
		return 1;
	}
	printf("handle=%d",hMapFile);

	pBuf = (LPTSTR) MapViewOfFile(hMapFile,   // handle to map object
		FILE_MAP_ALL_ACCESS, // read/write permission
		0,
		0,
		BUF_SIZE);

	if (pBuf == NULL)
	{
		_tprintf(TEXT("Could not map view of file (%d).\n"),
			GetLastError());

		CloseHandle(hMapFile);

		return 1;
	}
	//��main��ʼ���ˣ�A B process����һ�������ǻ�ȡ��Ϊ"Global\\MyFileMappingObject"�Ĺ����ڴ��ָ��
	printf("buf=%d",pBuf);
	//���´��룬B��ͣд�����ڴ�pBuf
	while(1)
	{
		TCHAR s[BUF_SIZE];
		cout<<"B process: plz input sth. to be transfered to A process."<<endl;
		cin>>s;
		memcpy((PVOID)pBuf, s, BUF_SIZE);
	}
}