#include "tcp_socket.h"
#include "inet_address.h"
#include "network_errno.h"
#include "log.h"
#include <boost/make_unique.hpp>

#ifdef ZXERO_OS_WINDOWS
#include <mstcpip.h>
#else
#include <unistd.h>
#include <fcntl.h>
#endif // ZXERO_OS_WINDOWS

namespace zxero
{
	namespace network
	{
		tcp_socket::tcp_socket(socket_t sock)
			:sock_(sock == INVALID_SOCKET ? socket(AF_INET, SOCK_STREAM, IPPROTO_TCP) : sock)
		{
			nonblocking(true);
			LOG_TRACE << "tcp_socket ctor[" << this << "] at " << sock_;
		}

		tcp_socket::~tcp_socket()
		{
			socket_close();
			LOG_TRACE << "tcp_socket dtor[" << this << "]";
		}

		socket_t tcp_socket::fd() const
		{
			return sock_;
		}

		int tcp_socket::error()
		{
			int optval = 0;
			socklen_t optlen = sizeof optval;
			if (getsockopt(sock_, SOL_SOCKET, SO_ERROR, (char*)&optval, &optlen) < 0)
			{
				return net_errno();
			}
			else
				return optval;
		}

		void tcp_socket::bind(const inet_address& addr)
		{
			int ret = ::bind(sock_, (const sockaddr*)&addr.sock_addr_inet(), sizeof(sockaddr_in));
			if (ret < 0)
			{
				LOG_FATAL << "tcp_socket [" << this << "] bind error, error code " << net_errno();
			}
		}

		void tcp_socket::listen()
		{
			int ret = ::listen(sock_, SOMAXCONN);
			if (ret < 0)
			{
				LOG_FATAL << "tcp_socket [" << this << "] listen error, error code " << net_errno();
			}
		}

		std::unique_ptr<tcp_socket> tcp_socket::accept(inet_address* peer_address)
		{
			sockaddr_in inaddr;
			socklen_t inaddrlen = sizeof(inaddr);
			socket_t sock = ::accept(sock_, (sockaddr*)(peer_address ? &(peer_address->sock_addr_inet()) : &inaddr), &inaddrlen);
			if (sock == INVALID_SOCKET)
			{
				LOG_ERROR << "tcp_socket [" << this << "] accept failed, error code " << net_errno();
				//	handler error
				return nullptr;
			}
			return boost::make_unique<tcp_socket>(sock);
		}

		int tcp_socket::connect(const inet_address& addr)
		{
			return ::connect(sock_, (const sockaddr*)&addr.sock_addr_inet(), sizeof(sockaddr_in));
		}

		void tcp_socket::socket_close()
		{
			if (sock_ != INVALID_SOCKET)
			{
#ifdef ZXERO_OS_WINDOWS
				closesocket(sock_);
#else
				close(sock_);
#endif
			}
			LOG_TRACE << "tcp_socket close[" << this << "]";
		}

		int tcp_socket::read(void* buf, int buf_len)
		{
			return ::recv(sock_, (char*)buf, buf_len, 0);
		}

		int tcp_socket::write(const void* buf, int buf_len)
		{
#ifdef ZXERO_OS_LINUX
			return ::send(sock_, (const char*)buf, buf_len, MSG_DONTWAIT);
#else
			return ::send(sock_, (const char*)buf, buf_len, 0);
#endif
		}



#ifdef ZXERO_OS_LINUX
		int tcp_socket::readv(const struct iovec *iov, int iovcnt)
		{
			return ::readv(sock_, iov, iovcnt);
		}
#endif

		void tcp_socket::shutdown_write()
		{
			int ret = ::shutdown(sock_, 
#ifdef ZXERO_OS_WINDOWS
				SD_SEND
#else
				SHUT_WR
#endif
				);
			if (ret < 0)
			{
				LOG_ERROR << "tcp_socket [" << this << "] shutdown_write failed, error code " << net_errno() ;
			}
		}

		inet_address tcp_socket::local_address() const
		{
			sockaddr_in addr;
			socklen_t addrlen = sizeof(addr);
			int ret = getsockname(sock_, (sockaddr*)&addr, &addrlen);
			if (ret < 0)
			{
				LOG_ERROR << "tcp_socket [" << this << "]  get local_address failed, error code " << net_errno() ;
			}
			return addr;
		}

		inet_address tcp_socket::peer_address() const
		{
			sockaddr_in addr;
			socklen_t addrlen = sizeof(addr);
			int ret = getpeername(sock_, (sockaddr*)&addr, &addrlen);
			if (ret < 0)
			{
				LOG_ERROR << "tcp_socket [" << this << "]  get peer_address failed, error code " << net_errno() ;
			}
			return addr;
		}

		bool tcp_socket::self_connect() const
		{
			return local_address() == peer_address();
		}

		void tcp_socket::no_delay(bool on)
		{
			int optval = on ? 1 : 0;
			int ret = setsockopt(sock_, IPPROTO_TCP, TCP_NODELAY, (const char*)&optval, sizeof(optval));
			if (ret < 0)
			{
				LOG_ERROR << "tcp_socket [" << this << "] set no_delay failed, error code " << net_errno() ;
			}
		}

		void tcp_socket::reuse_address(bool on)
		{
			int optval = on ? 1 : 0;
			int ret = setsockopt(sock_, SOL_SOCKET, SO_REUSEADDR, (const char*)&optval, sizeof(optval));
			if (ret < 0)
			{
				LOG_ERROR << "tcp_socket [" << this << "] set reuse_address failed, error code " << net_errno() ;
			}
		}

		void tcp_socket::keep_alive(bool on)
		{
			int optval = on ? 1 : 0;
			int ret = setsockopt(sock_, SOL_SOCKET, SO_KEEPALIVE, (const char*)&optval, sizeof(optval));
			if (ret < 0)
			{
				LOG_ERROR << "tcp_socket [" << this << "] set keep_alive failed, error code " << net_errno() ;
			}
			if (!on)
				return;
#ifdef ZXERO_OS_WINDOWS
			tcp_keepalive alive_in, alive_out;
			alive_in.keepalivetime = 10000;  // 10s
			alive_in.keepaliveinterval = 20000; //20s  
			alive_in.onoff = true;
			unsigned long ulBytesReturn = 0;
			ret = WSAIoctl(sock_, SIO_KEEPALIVE_VALS, &alive_in, sizeof(alive_in),
				&alive_out, sizeof(alive_out), &ulBytesReturn, NULL, NULL);
			if (ret < 0)
			{
				LOG_ERROR << "tcp_socket " << this << " set keep_alive time fails, errod code" << net_errno();
			}
#else
			int sockopt_tcp_keep_idle = 10;
			int sockopt_tcp_keep_intvl = 20;
			int sockopt_tcp_keep_cnt = 5;
			if (setsockopt(sock_, IPPROTO_TCP, TCP_KEEPIDLE, (char*)&(sockopt_tcp_keep_idle), sizeof(sockopt_tcp_keep_idle)))
			{
				LOG_ERROR << "tcp_socket " << this << " set keep_alive sockopt_tcp_keep_idle, errod code" << net_errno();
			}
			if (setsockopt(sock_, IPPROTO_TCP, TCP_KEEPINTVL, (char*)&(sockopt_tcp_keep_intvl), sizeof(sockopt_tcp_keep_intvl)))
			{
				LOG_ERROR << "tcp_socket " << this << " set keep_alive sockopt_tcp_keep_intvl, errod code" << net_errno();
			}
			if (setsockopt(sock_, IPPROTO_TCP, TCP_KEEPCNT, (char*)&(sockopt_tcp_keep_cnt), sizeof(sockopt_tcp_keep_cnt)))
			{
				LOG_ERROR << "tcp_socket " << this << " set keep_alive sockopt_tcp_keep_cnt, errod code" << net_errno();
			}
#endif // ZXERO_OS_WINDOWS

		}

		void tcp_socket::nonblocking(bool on)
		{
#ifdef ZXERO_OS_WINDOWS
			u_long nonblock = on ? 1 : 0;
			int ret = ioctlsocket( sock_, FIONBIO, &nonblock );
			if (ret < 0)
			{
				LOG_ERROR << "tcp_socket [" << this << "] set nonblocking failed, error code " << net_errno() ;
			}

#else
			int flags = fcntl(sock_, F_GETFL, 0);
			flags |= O_NONBLOCK;
			int ret = fcntl(sock_, F_SETFL, flags);
			if (ret < 0)
			{
				LOG_ERROR << "tcp_socket [" << this << "] set nonblocking failed, error code " << net_errno() ;
			}
#endif
		}

		int tcp_socket::send_buff()
		{
			int size = 0;
			socklen_t optlen = sizeof(size);
			int ret = getsockopt(sock_, SOL_SOCKET, SO_SNDBUF, (char*)&size, &optlen);
			if (ret < 0)
			{
				LOG_ERROR << "tcp_socket [" << this << "] get send_buff failed, error code " << net_errno() ;
			}
			return size;
		}

		int tcp_socket::recv_buff()
		{
			int size = 0;
			socklen_t optlen = sizeof(size);
			int ret = getsockopt(sock_, SOL_SOCKET, SO_RCVBUF, (char*)&size, &optlen);
			if (ret < 0)
			{
				LOG_ERROR << "tcp_socket [" << this << "] get recv_buff failed, error code " << net_errno() ;
			}
			return size;
		}

		void tcp_socket::send_buff(int size)
		{
			int ret = setsockopt(sock_, SOL_SOCKET, SO_SNDBUF, (const char*)&size, sizeof(size));
			if (ret < 0)
			{
				LOG_ERROR << "tcp_socket [" << this << "] set send_buff failed, error code " << net_errno() ;
			}
		}

		void tcp_socket::recv_buff(int size)
		{
			int ret = setsockopt(sock_, SOL_SOCKET, SO_RCVBUF, (const char*)&size, sizeof(size));
			if (ret < 0)
			{
				LOG_ERROR << "tcp_socket [" << this << "] set recv_buff failed, error code " << net_errno() ;
			}
		}
	}
}
