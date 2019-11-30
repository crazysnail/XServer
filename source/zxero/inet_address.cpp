#include "inet_address.h"
#include <stdio.h>
#include <string.h>
namespace zxero
{
	namespace network
	{
		inet_address::inet_address(uint16 port)
		{
			memset(&addr_, 0, sizeof(addr_));
			addr_.sin_family = AF_INET;
#ifdef ZXERO_OS_WINDOWS
			addr_.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
#else
			addr_.sin_addr.s_addr = htons(INADDR_ANY);
#endif
			addr_.sin_port = htons(port);
		}

		inet_address::inet_address(const std::string& ip, uint16 port)
		{
			memset(&addr_, 0, sizeof(addr_));
			addr_.sin_family = AF_INET;
#ifdef ZXERO_OS_WINDOWS
			addr_.sin_addr.S_un.S_addr = inet_addr(ip.c_str());
#else
			inet_pton(AF_INET, ip.c_str(), &addr_.sin_addr.s_addr );
#endif
			addr_.sin_port = htons(port);
		}

		inet_address::inet_address(const struct sockaddr_in& addr)
			:addr_(addr)
		{

		}

		std::string inet_address::ip() const
		{
			
#ifdef ZXERO_OS_WINDOWS
			return inet_ntoa(addr_.sin_addr);
#else
			char buff[32] = { 0 };
			inet_ntop(AF_INET, &addr_.sin_addr, buff, 32);
			return buff;
#endif
		}

		std::string inet_address::ip_port() const
		{
			char buff[32] = { 0 };
			zprintf( buff, sizeof(buff), "%s:%u", ip().c_str(), ntohs(addr_.sin_port) );
			return buff;
		}

		const struct sockaddr_in& inet_address::sock_addr_inet() const
		{
			return addr_;
		}

		void inet_address::sock_addr_inet(const struct sockaddr_in& addr)
		{
			addr_ = addr;
		}

		bool inet_address::operator == (const inet_address& r) const
		{
			return addr_.sin_port == r.addr_.sin_port
#ifdef ZXERO_OS_WINDOWS
				&& addr_.sin_addr.S_un.S_addr == r.addr_.sin_addr.S_un.S_addr;
#else
				&& addr_.sin_addr.s_addr == r.addr_.sin_addr.s_addr;
#endif
		}
	}
}
