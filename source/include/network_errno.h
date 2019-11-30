//	˫�����ݵĴ�����
#ifndef _zxero_network_errno_h_
#define _zxero_network_errno_h_

#include "types.h"

namespace zxero
{
	namespace network
	{
		//	�������������
		int net_errno();

		//	��������붨��
		enum net_error_code
		{
#ifdef ZXERO_OS_WINDOWS
			E_WOULD_BLOCK = WSAEWOULDBLOCK,		//	��Ҫ��������
			E_CONN_RESET = WSAECONNRESET,		//	Զ������
			E_CONN_REFUSED = WSAECONNREFUSED,	//	���ӱ��ܾ�
			E_PIPE = WSAESHUTDOWN,				//
			E_IN_PROGRESS = WSAEINPROGRESS,
			E_INTR = WSAEINTR,
			E_IS_CONN = WSAEISCONN,
			E_AGAIN = WSAEWOULDBLOCK,
			E_ADDR_INUSE = WSAEADDRINUSE,
			E_ADDR_NOTAVAIL = WSAEADDRNOTAVAIL,
			E_NET_UNREACH = WSAENETUNREACH,
			E_ACCES = WSAEACCES,
			E_PERM = WSAEACCES,
			E_AF_NO_SUPPORT = WSAEAFNOSUPPORT,
			E_ALREADY = WSAEALREADY,
			E_BADF = WSAEBADF,
			E_FAULT = WSAEFAULT,
			E_NOT_SOCK = WSAENOTSOCK,
#else
			E_WOULD_BLOCK = EWOULDBLOCK,		//	��Ҫ��������
			E_CONN_RESET = ECONNRESET,		//	Զ������
			E_CONN_REFUSED = ECONNREFUSED,	//	���ӱ��ܾ�
			E_PIPE = EPIPE,				//
			E_IN_PROGRESS = EINPROGRESS,
			E_INTR = EINTR,
			E_IS_CONN = EISCONN,
			E_AGAIN = EWOULDBLOCK,
			E_ADDR_INUSE = EADDRINUSE,
			E_ADDR_NOTAVAIL = EADDRNOTAVAIL,
			E_NET_UNREACH = ENETUNREACH,
			E_ACCES = EACCES,
			E_PERM = EPERM,
			E_AF_NO_SUPPORT = EAFNOSUPPORT,
			E_ALREADY = EALREADY,
			E_BADF = EBADF,
			E_FAULT = EFAULT,
			E_NOT_SOCK = ENOTSOCK,
#endif
		};

	}
}


#endif	//	#ifndef _zxero_network_errno_h_

