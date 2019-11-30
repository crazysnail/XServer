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
		//	//	����fd��ռ��0~9λ
		//	E_MASK	=	0x03ff,
		//	//	��ȡfd��ռ��10~19λ,30λΪ0ʱ������ȡfd���ڣ������򲻴��ڣ���ʱ��ȡfd����Ϊ0
		//	R_SHIFT	=	10,
		//	R_NOT_EXIST_FLAG	=	0x40000000,
		//	//	д��fd��ռ��20~29Ϊ,31λΪ0�Ǳ���д��fd���ڣ������򲻴��ڣ���ʱд��fd����Ϊ0
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
			//	������ʱfd�б�
			tmp_read_fd_ = read_fd_;
			tmp_write_fd_ = write_fd_;
			tmp_error_fd_ = error_fd_;
			//	ִ��select����
			timeval timeout;
			timeout.tv_sec = td.seconds();
			timeout.tv_usec = (long)td.fractional_seconds();

			int nfds = channel_map_.size();
			if (nfds) ++nfds;
			int num = 0;
			//	���û��socket�Ļ����ǲ��ܵ���select��
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
				//	ժȡ�����¼�
				fill_active_channels(active_channels);
			}
			else if (num == 0)
			{
				//	û�����¼�
			}
			else
			{
				//	��������������
				//LOG_ERROR << "select_poller [" << this << "] poll failed, error code " << net_errno() ;
			}
			return now;
		}

		//
		void select_poller::update_channel(channel* c)
		{
			assert_in_loop_thread();
			//	ͨ��c��index������֪��c������fd_set�е�λ��
			int idx = c->index();
			if (idx == -1)
			{
				ZXERO_ASSERT(channel_map_.size() + 1 < fd_set_t::FD_SIZE);
				ZXERO_ASSERT(channel_map_.find(c->fd()) == channel_map_.end());
				//	��fd
				bool revent = (c->events() & channel::k_read_event) != 0;
				bool wevent = (c->events() & channel::k_write_event) != 0;
								
				FdIndex fi = { 0 };
				fi.error_fd_index = error_fd_.size();
				
				//	һ�����ڵĴ��󼯺�
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
				
				//	���ø�������
				c->index(*(int32*)&fi);

				LOG_DEBUG << "select_poller [" << this << "] add channel " << c << " of fd " << fd ;
				//	����ӳ��
				channel_map_.insert(std::make_pair(fd, c));
			}
			else
			{
				socket_t fd = c->fd();

				//	����֮
				ZXERO_ASSERT(channel_map_.find(fd) != channel_map_.end());
				ZXERO_ASSERT(channel_map_[fd] == c);

				//	��ȡfd����
				int index = c->index();
				FdIndex fi = *(FdIndex*)&index;

				//	��ǰ�Ƿ��ж�д�¼�
				bool revent = (c->events() & channel::k_read_event) != 0;
				bool wevent = (c->events() & channel::k_write_event) != 0;
			
				//	
				if (revent)
				{
					//	�������¼���ԭ��û�¼�
					if (fi.read_not_exist)
					{
						fi.read_fd_index = read_fd_.size();
						fi.read_not_exist = 0;
						read_fd_.set(fd);
					}
				}
				else
				{
					//	ԭ�����¼�������û�¼�
					if (!fi.read_not_exist)
					{
						read_fd_.clr_by_idx(fi.read_fd_index);

						//	������Ҫ���±�����λ�õ�fd������
						if (fi.read_fd_index < read_fd_.size())
						{
							channel* nc = fd2channel(read_fd_[fi.read_fd_index]);
							int nidx = nc->index();
							FdIndex nfi = *(FdIndex*)&nidx;
							//	�滻�µ�fd����
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
					//	�������¼���ԭ��û�¼�
					if (fi.write_not_exist)
					{
						fi.write_fd_index = write_fd_.size();
						fi.write_not_exist = 0;
						write_fd_.set(fd);
					}
				}
				else
				{
					//	ԭ�����¼�������û�¼�
					if (!fi.write_not_exist)
					{
						write_fd_.clr_by_idx(fi.write_fd_index);

						//	����������λ�õ�fd������
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

				//	����index
				c->index(*(int32*)&fi);
			}
		}

		//
		void select_poller::remove_channel(channel* c)
		{
			assert_in_loop_thread();

			//	�Ƴ�����fd
			int idx = c->index();
			FdIndex fi = *(FdIndex*)&idx;

			//	�Ƴ�error
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

			//	�Ƴ�read
			if (!fi.read_not_exist)
			{
				read_fd_.clr_by_idx(fi.read_fd_index);

				//	������Ҫ���±�����λ�õ�fd������
				if (fi.read_fd_index < read_fd_.size())
				{
					channel* nc = fd2channel(read_fd_[fi.read_fd_index]);
					int nidx = nc->index();
					FdIndex nfi = *(FdIndex*)&nidx;
					//	�滻�µ�fd����
					nfi.read_fd_index = fi.read_fd_index;
					nc->index(*(int*)&nfi);
				}
			}

			//	�Ƴ�write
			if (!fi.write_not_exist)
			{
				write_fd_.clr_by_idx(fi.write_fd_index);

				//	����������λ�õ�fd������
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

			//	��������
			c->index(-1);

			//	�Ƴ�ӳ��
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

			//	��������read_fd, ��䵽set��ȥ
			for (uint32 i = 0; i < tmp_read_fd_.size(); ++i)
			{
				socket_t fd = tmp_read_fd_[i];
				LOG_DEBUG << "select_poller [" << this << "] active read fd " << fd << " at " << i ;
				if ( fd <= 0 )
					continue;
				channel* c = fd2channel(fd);
				ZXERO_ASSERT(fd != INVALID_SOCKET);
				ZXERO_ASSERT(c);

				//	���ó�ʼ�¼�Ϊ���¼�
				c->revents(channel::k_read_event);

				//	����ӳ���
				channel_set_.insert(c);
			}

			//	��������write_fd, ��䵽set��ȥ
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
					//	�ϲ�д�¼�
					c->revents(c->revents() | channel::k_write_event);
				}
			}

			//	��������error_fd, ��䵽set��ȥ
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
					//	�ϲ������¼�
					c->revents(c->revents() | channel::k_error_event | channel::k_close_event);
				}
			}

			//	�����Ͽ���Ϊvector
			active_channels.assign(channel_set_.begin(), channel_set_.end());
		}
	}
}

#endif //	#ifdef ZXERO_OS_WINDOWS
