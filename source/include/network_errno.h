//	双方兼容的错误码
#ifndef _zxero_network_errno_h_
#define _zxero_network_errno_h_

#include "types.h"

namespace zxero
{
	namespace network
	{
		//	返回网络错误码
		int net_errno();

		//	网络错误码定义
		enum net_error_code
		{
#ifdef ZXERO_OS_WINDOWS
			E_WOULD_BLOCK = WSAEWOULDBLOCK,		//	需要阻塞操作
			E_CONN_RESET = WSAECONNRESET,		//	远端重置
			E_CONN_REFUSED = WSAECONNREFUSED,	//	连接被拒绝
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
			E_WOULD_BLOCK = EWOULDBLOCK,		//	需要阻塞操作
			E_CONN_RESET = ECONNRESET,		//	远端重置
			E_CONN_REFUSED = ECONNREFUSED,	//	连接被拒绝
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

