//	ipv4 from muduo
#ifndef _zxero_network_inet_address_h_
#define _zxero_network_inet_address_h_

#include "types.h"
#include <string>

namespace zxero
{
	namespace network
	{
		class inet_address
		{
		public:
			//	for server address
			explicit inet_address(uint16 port);

			//	for common address
			inet_address(const std::string& ip, uint16 port);

			//	..
			inet_address(const struct sockaddr_in& addr);

		public:
			std::string ip() const;
			std::string ip_port() const;

			const struct sockaddr_in& sock_addr_inet() const;
			void sock_addr_inet(const struct sockaddr_in& addr);

		public:
			bool operator == (const inet_address& r) const;

		private:
			struct sockaddr_in addr_;
		};

		//	增加inet_address的输出方法
	}
}


#endif	//	#ifndef _zxero_network_inet_address_h_

