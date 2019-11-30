#include "network_errno.h"

namespace zxero
{
	namespace network
	{
		int net_errno()
		{
#ifdef ZXERO_OS_WINDOWS
			errno = ::WSAGetLastError();
			return errno;
#else
			return errno;
#endif
		}
	}
}
