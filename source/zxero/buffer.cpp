#include "buffer.h"
#include "tcp_socket.h"
#include "network_errno.h"

namespace zxero
{
	size_t buffer::read(network::tcp_socket& sock, int* saved_errno)
	{
#ifndef ZXERO_OS_LINUX
		size_t writable = writable_bytes();
		int n = sock.read(begin() + writer_index_, writable_bytes());
		if ( n < 0 )
		{
			*saved_errno = network::net_errno();
		}
		else
		{
			writer_index_ += n;
			//	如果读到头了，那么尝试扩充64k再读一次
			if (n == (int)writable)
			{
				make_space(64 * 1024);
				int nn = sock.read(begin() + writer_index_, writable_bytes());
				if (nn < 0)
				{
					//	通常认为第二次读失败都是因为E_WOULD_BLOCK了
					*saved_errno = network::net_errno();
				}
				else
				{
					writer_index_ += nn;
					n += nn;
				}
			}
		}

		return n;
#else
		// saved an ioctl()/FIONREAD call to tell how much to read
	  char extrabuf[65536];
	  struct iovec vec[2];
	  const size_t writable = writable_bytes();
	  vec[0].iov_base = begin()+writer_index_;
	  vec[0].iov_len = writable;
	  vec[1].iov_base = extrabuf;
	  vec[1].iov_len = sizeof(extrabuf);
	  // when there is enough space in this buffer, don't read into extrabuf.
	  // by doing this, we read 128k-1 bytes at most
	  const int iovcnt = (writable < sizeof(extrabuf)) ? 2 : 1;
	  const int n = sock.readv(vec, iovcnt);
	  if (n < 0)
	  {
	    *saved_errno = network::net_errno();
	  }
	  else if ((size_t)n <= writable)
	  {
	    writer_index_ += n;
	  }
	  else
	  {
	    writer_index_ = buffer_.size();
	    append(extrabuf, n - writable);
	  }
	  // if (n == writable + sizeof extrabuf)
	  // {
	  //   goto line_30;
	  // }
	  return n;
#endif
	}
}
