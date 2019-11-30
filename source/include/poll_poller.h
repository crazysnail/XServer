//	linux об╣д poll poller , from muduo
#ifndef _zxero_network_poll_poller_h_
#define _zxero_network_poll_poller_h_

#include "types.h"
#include "poller.h"
#include <boost/container/map.hpp>

struct pollfd;

namespace zxero
{
	namespace network
	{
		class poll_poller : public poller
		{
		public:
			poll_poller(event_loop* loop);
			virtual ~poll_poller();
			
		public:
			//
			virtual timestamp_t poll(time_duration timeoutms, channel_list_t& active_channels);
			
			//
			virtual void update_channel(channel* channel);
			
			//
			virtual void remove_channel(channel* channel);
			
		private:
			void fill_active_channels(int num_events, channel_list_t& active_channels) const;
			
			typedef std::vector<pollfd> poll_fd_list_t;
			typedef boost::container::map<socket_t, channel*> channel_map_t;
			
			poll_fd_list_t pollfds_;
			channel_map_t channels_;
		};
	}
}

#endif	//	#ifndef _zxero_network_poll_poller_h_
