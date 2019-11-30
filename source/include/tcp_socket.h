//	socket 封装 from moduo
#ifndef _zxero_network_socket_h_
#define _zxero_network_socket_h_

#include "types.h"
#include <memory>	//	for unique_ptr
#include <boost/noncopyable.hpp>

namespace zxero
{
	namespace network
	{
		class tcp_socket : boost::noncopyable
		{
		public:
			explicit tcp_socket(socket_t sock);

			~tcp_socket();

		public:
			socket_t	fd() const;

			int			error();

		public:
			// for server
			void bind(const inet_address& addr);
			void listen();

			//	获取一个tcp连接
			std::unique_ptr<tcp_socket> accept(inet_address* peer_address = nullptr);

		public:
			//	for client
			int connect(const inet_address& addr);
			//
			void socket_close();
		public:
			//	for io
			int read(void* buf, int buf_len);
			int write(const void* buf, int buf_len);

#ifdef ZXERO_OS_LINUX
			int readv(const struct iovec *iov, int iovcnt);
#endif

		public:
			//	写关闭
			void shutdown_write();

		public:
			//	for property
			inet_address local_address() const;
			inet_address peer_address() const;

			//
			bool self_connect() const;

		public:
			//	for op
			//
			void no_delay(bool on);

			//
			void reuse_address(bool on);

			//
			void keep_alive(bool on);

			//	
			void nonblocking(bool on);

			//
			int send_buff();
			void send_buff(int size);

			//
			int recv_buff();
			void recv_buff(int size);

		private:
			const socket_t sock_;
		};
	}
}

#endif	//	#ifndef _zxero_network_socket_h_

