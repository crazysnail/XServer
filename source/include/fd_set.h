#ifndef _zxero_network_fd_set_h_
#define _zxero_network_fd_set_h_

#include "types.h"

namespace zxero
{
	namespace network
	{
		extern socket_t	g_invalid_socket;

#ifdef ZXERO_OS_WINDOWS
		template< int FD_COUNT >
		class fd_set
		{
		private:
			uint32			fdc;
			socket_t		fds[FD_COUNT];

		public:
			enum {
				FD_SIZE = FD_COUNT,
			};

		public:
			fd_set()
				:fdc(0)
			{
				memset(fds, 0, sizeof(fds));
			}

			//	简单的将fd_set模板，变为fd_set*或fd_set本身
			operator ::fd_set* ()
			{
				return (::fd_set*)this;
			}

			operator ::fd_set& ()
			{
				return *(::fd_set*)this;
			}

			//
			socket_t& operator[] (size_t idx)
			{
				if (idx < fdc)
					return fds[idx];
				return g_invalid_socket;
			}

			const socket_t& operator[] (size_t idx) const
			{
				if (idx < fdc)
					return fds[idx];
				return g_invalid_socket;
			}

			//
			uint32 size() const
			{
				return fdc;
			}

		public:
			//	不同于原本的移除前移操作，而是直接把最后一个fd移到要被移除的位置
			//	返回被更新的fd的索引, 如果没有被移除的fd，则返回-1
			int clr(socket_t sock)
			{
				if (sock == INVALID_SOCKET)
					return -1;
				for (uint32 i = 0; i < fdc; ++i)
				{
					if (fds[i] == sock)
					{
						--fdc;
						if (i != fdc)
						{
							fds[i] = fds[fdc];
						}
						return i;
					}
				}
				return -1;
			}

			//
			void clr_by_idx(uint32 idx)
			{
				if (idx < fdc)
				{
					--fdc;
					if (idx != fdc)
					{
						fds[idx] = fds[fdc];
					}
				}
			}

			//
			void set(socket_t sock)
			{
				if (sock == INVALID_SOCKET)
					return;

				if (!is_set(sock) && fdc < FD_COUNT)
				{
					fds[fdc++] = sock;
				}
			}

			//
			void zero()
			{
				fdc = 0;
			}

			//
			bool is_set(socket_t sock)
			{
				for (uint32 i = 0; i < fdc; ++i)
				{
					if (fds[i] == sock)
						return true;
				}
				return false;
			}
		};
#else
		//	linux 下的 fd_set 不是这么设计的
#endif

		typedef fd_set<8>		fd_set_8;
		typedef fd_set<16>		fd_set_16;
		typedef fd_set<32>		fd_set_32;
		typedef fd_set<64>		fd_set_64;
		typedef fd_set<256>		fd_set_256;
		typedef fd_set<512>		fd_set_512;
		typedef fd_set<1024>	fd_set_1024;
		typedef fd_set<2028>	fd_set_2048;
	}	//	namespace network
}	//	namespace zxero

#endif //	#ifndef _zxero_network_fd_set_h_
