#include "select_poller.h"
#include "channel.h"
#include "event_loop.h"
#include "log.h"
#include <set>

#ifdef ZXERO_OS_WINDOWS

namespace zxero
{
	namespace network
	{
		
		//enum FdSetIndexShiftMask
		//{
		//	//	错误fd，占用0~9位
		//	E_MASK	=	0x03ff,
		//	//	读取fd，占用10~19位,30位为0时表明读取fd存在，否则则不存在，此时读取fd索引为0
		//	R_SHIFT	=	10,
		//	R_NOT_EXIST_FLAG	=	0x40000000,
		//	//	写入fd，占用20~29为,31位为0是表明写入fd存在，否则则不存在，此时写入fd索引为0
		//	W_SHIFT	=	20,
		//	W_NOT_EXIST_FLAG	=	0x80000000,
		//};
#ifdef ZXERO_OS_WINDOWS
#pragma warning(disable:4201)
#endif
		union FdIndex
		{
			int32 idx;
			struct 
			{
				uint32 error_fd_index	: 10;
				uint32 read_fd_index	: 10;
				uint32 write_fd_index	: 10;
				uint32 read_not_exist : 1;
				uint32 write_not_exist : 1;
			};
		};
#ifdef ZXERO_OS_WINDOWS
#pragma warning(default:4201)
#endif
		select_poller::select_poller(event_loop* loop)
			:poller(loop)
		{
			ZXERO_ASSERT(sizeof(FdIndex) == sizeof(int32));
			LOG_TRACE << "select_poller ctor[" << this << "]";
		}

		select_poller::~select_poller()
		{
			LOG_TRACE << "select_poller dtor[" << this << "]";
		}

		timestamp_t select_poller::poll(time_duration td, channel_list_t& active_channels)
		{
			//	拷贝临时fd列表
			tmp_read_fd_ = read_fd_;
			tmp_write_fd_ = write_fd_;
			tmp_error_fd_ = error_fd_;
			//	执行select操作
			timeval timeout;
			timeout.tv_sec = td.seconds();
			timeout.tv_usec = (long)td.fractional_seconds();

			int nfds = channel_map_.size();
			if (nfds) ++nfds;
			int num = 0;
			//	如果没有socket的话，是不能调用select的
			if (nfds)
			{
				num = select(nfds, tmp_read_fd_, tmp_write_fd_, tmp_error_fd_, &timeout);
			}
			else
			{
				//LOG_WARN<<"thread::sleep";
				boost::this_thread::sleep(td);
			}
			
			timestamp_t now(microsec_clock::local_time());

			if (num > 0)
			{
				//	摘取所有事件
				fill_active_channels(active_channels);
			}
			else if (num == 0)
			{
				//	没发生事件
			}
			else
			{
				//	发生其他错误了
				//LOG_ERROR << "select_poller [" << this << "] poll failed, error code " << net_errno() ;
			}
			return now;
		}

		//
		void select_poller::update_channel(channel* c)
		{
			assert_in_loop_thread();
			//	通过c的index，可以知道c在三个fd_set中的位置
			int idx = c->index();
			if (idx == -1)
			{
				ZXERO_ASSERT(channel_map_.size() + 1 < fd_set_t::FD_SIZE);
				ZXERO_ASSERT(channel_map_.find(c->fd()) == channel_map_.end());
				//	新fd
				bool revent = (c->events() & channel::k_read_event) != 0;
				bool wevent = (c->events() & channel::k_write_event) != 0;
								
				FdIndex fi = { 0 };
				fi.error_fd_index = error_fd_.size();
				
				//	一定存在的错误集合
				socket_t fd = c->fd();
				error_fd_.set(fd);

				if (revent)
				{
					fi.read_fd_index = read_fd_.size();
					read_fd_.set(fd);
				}
				else
					fi.read_not_exist = 1;

				if (wevent)
				{
					fi.write_fd_index = write_fd_.size();
					write_fd_.set(fd);
				}
				else
					fi.write_not_exist = 1;
				
				//	设置复合索引
				c->index(*(int32*)&fi);

				LOG_DEBUG << "select_poller [" << this << "] add channel " << c << " of fd " << fd ;
				//	加入映射
				channel_map_.insert(std::make_pair(fd, c));
			}
			else
			{
				socket_t fd = c->fd();

				//	更新之
				ZXERO_ASSERT(channel_map_.find(fd) != channel_map_.end());
				ZXERO_ASSERT(channel_map_[fd] == c);

				//	获取fd索引
				int index = c->index();
				FdIndex fi = *(FdIndex*)&index;

				//	当前是否有读写事件
				bool revent = (c->events() & channel::k_read_event) != 0;
				bool wevent = (c->events() & channel::k_write_event) != 0;
			
				//	
				if (revent)
				{
					//	现在有事件，原先没事件
					if (fi.read_not_exist)
					{
						fi.read_fd_index = read_fd_.size();
						fi.read_not_exist = 0;
						read_fd_.set(fd);
					}
				}
				else
				{
					//	原先有事件，现在没事件
					if (!fi.read_not_exist)
					{
						read_fd_.clr_by_idx(fi.read_fd_index);

						//	这里需要更新被调整位置的fd的索引
						if (fi.read_fd_index < read_fd_.size())
						{
							channel* nc = fd2channel(read_fd_[fi.read_fd_index]);
							int nidx = nc->index();
							FdIndex nfi = *(FdIndex*)&nidx;
							//	替换新的fd索引
							nfi.read_fd_index = fi.read_fd_index;
							nc->index(*(int*)&nfi);
						}

						//
						fi.read_fd_index = 0;
						fi.read_not_exist = 1;
					}
				}

				//
				if (wevent)
				{
					//	现在有事件，原先没事件
					if (fi.write_not_exist)
					{
						fi.write_fd_index = write_fd_.size();
						fi.write_not_exist = 0;
						write_fd_.set(fd);
					}
				}
				else
				{
					//	原先有事件，现在没事件
					if (!fi.write_not_exist)
					{
						write_fd_.clr_by_idx(fi.write_fd_index);

						//	调整被调整位置的fd的索引
						if (fi.write_fd_index < write_fd_.size())
						{
							channel* nc = fd2channel(write_fd_[fi.write_fd_index]);
							int nidx = nc->index();
							FdIndex nfi = *(FdIndex*)&nidx;
							//
							nfi.write_fd_index = fi.write_fd_index;
							nc->index(*(int*)&nfi);
						}

						//
						fi.write_fd_index = 0;
						fi.write_not_exist = 1;
					}
				}

				//	更新index
				c->index(*(int32*)&fi);
			}
		}

		//
		void select_poller::remove_channel(channel* c)
		{
			assert_in_loop_thread();

			//	移除所有fd
			int idx = c->index();
			FdIndex fi = *(FdIndex*)&idx;

			//	移除error
			{
				error_fd_.clr_by_idx(fi.error_fd_index);
				if (fi.error_fd_index < error_fd_.size())
				{
					channel* nc = fd2channel(error_fd_[fi.error_fd_index]);
					int nidx = nc->index();
					FdIndex nfi = *(FdIndex*)&nidx;
					//
					nfi.error_fd_index = fi.error_fd_index;
					nc->index(*(int*)&nfi);
				}
			}

			//	移除read
			if (!fi.read_not_exist)
			{
				read_fd_.clr_by_idx(fi.read_fd_index);

				//	这里需要更新被调整位置的fd的索引
				if (fi.read_fd_index < read_fd_.size())
				{
					channel* nc = fd2channel(read_fd_[fi.read_fd_index]);
					int nidx = nc->index();
					FdIndex nfi = *(FdIndex*)&nidx;
					//	替换新的fd索引
					nfi.read_fd_index = fi.read_fd_index;
					nc->index(*(int*)&nfi);
				}
			}

			//	移除write
			if (!fi.write_not_exist)
			{
				write_fd_.clr_by_idx(fi.write_fd_index);

				//	调整被调整位置的fd的索引
				if (fi.write_fd_index < write_fd_.size())
				{
					channel* nc = fd2channel(write_fd_[fi.write_fd_index]);
					int nidx = nc->index();
					FdIndex nfi = *(FdIndex*)&nidx;
					//
					nfi.write_fd_index = fi.write_fd_index;
					nc->index(*(int*)&nfi);
				}
			}

			//	重置索引
			c->index(-1);

			//	移除映射
			channel_map_.erase(c->fd());
		}

		//
		channel* select_poller::fd2channel(socket_t fd)
		{
			if (channel_map_.find(fd) != channel_map_.end())
				return channel_map_[fd];
			
			return nullptr;
		}

		//
		void select_poller::fill_active_channels(channel_list_t& active_channels)
		{
			typedef std::set<channel*>	channel_set;
			//	
			channel_set	channel_set_;

			//	遍历所有read_fd, 填充到set中去
			for (uint32 i = 0; i < tmp_read_fd_.size(); ++i)
			{
				socket_t fd = tmp_read_fd_[i];
				LOG_DEBUG << "select_poller [" << this << "] active read fd " << fd << " at " << i ;
				if ( fd <= 0 )
					continue;
				channel* c = fd2channel(fd);
				ZXERO_ASSERT(fd != INVALID_SOCKET);
				ZXERO_ASSERT(c);

				//	设置初始事件为读事件
				c->revents(channel::k_read_event);

				//	加入映射表
				channel_set_.insert(c);
			}

			//	遍历所有write_fd, 填充到set中去
			for (uint32 i = 0; i < tmp_write_fd_.size(); ++i)
			{
				socket_t fd = tmp_write_fd_[i];
				LOG_DEBUG << "select_poller [" << this << "] active write fd " << fd << " at " << i ;
				if ( fd <= 0 )
					continue;
				channel* c = fd2channel(fd);
				ZXERO_ASSERT(fd != INVALID_SOCKET);
				ZXERO_ASSERT(c);

				//	
				if (channel_set_.find(c) == channel_set_.end())
				{
					c->revents(channel::k_write_event);
					channel_set_.insert(c);
				}
				else
				{
					//	合并写事件
					c->revents(c->revents() | channel::k_write_event);
				}
			}

			//	遍历所有error_fd, 填充到set中去
			for (uint32 i = 0; i < tmp_error_fd_.size(); ++i)
			{
				socket_t fd = tmp_error_fd_[i];
				LOG_DEBUG << "select_poller [" << this << "] active error fd " << fd << " at " << i ;
				if ( fd <= 0 )
					continue;
				channel* c = fd2channel(fd);
				ZXERO_ASSERT(fd != INVALID_SOCKET);
				ZXERO_ASSERT(c);

				//	
				if (channel_set_.find(c) == channel_set_.end())
				{
					c->revents(channel::k_error_event | channel::k_close_event);
					channel_set_.insert(c);
				}
				else
				{
					//	合并错误事件
					c->revents(c->revents() | channel::k_error_event | channel::k_close_event);
				}
			}

			//	将集合拷贝为vector
			active_channels.assign(channel_set_.begin(), channel_set_.end());
		}
	}
}

#endif //	#ifdef ZXERO_OS_WINDOWS
