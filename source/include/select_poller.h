//	基于select的poller
#ifndef _zxero_network_select_poller_h_
#define _zxero_network_select_poller_h_

#include "types.h"

#ifdef ZXERO_OS_WINDOWS

#include "fd_set.h"
#include "date_time.h"
#include "poller.h"

namespace zxero
{
	namespace network
	{
		class select_poller : public poller
		{
		public:
			select_poller(event_loop* loop);
			~select_poller();

			//	返回完成poll操作的时间点
			virtual timestamp_t poll(time_duration timeoutms, channel_list_t& active_channels);

			//
			virtual void update_channel(channel* c);
			virtual void remove_channel(channel* c);

		private:
			//
			void fill_active_channels(channel_list_t& active_channels);
			channel* fd2channel(socket_t fd);
		private:
			//	注意：这里最大为1024，不要超过这个数，否则会出错
			typedef fd_set<1024>	fd_set_t;
			typedef std::map< socket_t, channel* >	channel_map_t;

			//	这是一个比较合适的值
			//	缓存要select的原始fd
			fd_set_t	read_fd_;
			fd_set_t	write_fd_;
			fd_set_t	error_fd_;

			//	执行select操作的临时fd
			fd_set_t	tmp_read_fd_;
			fd_set_t	tmp_write_fd_;
			fd_set_t	tmp_error_fd_;

			//	
			channel_map_t channel_map_;
		};
	}
}

#endif	//	#ifdef ZXERO_OS_WINDOWS

#endif	//	#ifndef _zxero_network_select_poller_h_

