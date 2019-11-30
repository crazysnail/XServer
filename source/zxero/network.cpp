#include "network.h"
#include "types.h"

namespace zxero
{
	namespace network
	{
		socket_t	g_invalid_socket = INVALID_SOCKET;

#ifdef ZXERO_OS_WINDOWS
		int startup()
		{
			WSAData wd;
			memset(&wd, 0, sizeof(wd));
			return WSAStartup(MAKEWORD(2, 2), &wd);
		}

		void shutdown()
		{
			WSACleanup();
		}
#else
		int startup()
		{
			return 0;
		}

		void shutdown()
		{
		}
#endif
	}
}


