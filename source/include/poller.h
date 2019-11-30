//	poller 的接口定义 from muduo
#ifndef _zxero_network_poller_h_
#define _zxero_network_poller_h_

#include "types.h"
#include "date_time.h"
#include <vector>
#include <boost/noncopyable.hpp>



namespace zxero
{
	namespace network
	{
		class poller : boost::noncopyable
		{
			public:
				typedef std::vector<channel*>	channel_list_t;
			
			public:
				poller( event_loop* loop );
				virtual ~poller();
				
			public:
				//
				virtual timestamp_t poll(boost::posix_time::time_duration timeoutms, channel_list_t& active_channels) = 0;
				
				//
				virtual void update_channel(channel* channel) = 0;
				
				//
				virtual void remove_channel(channel* channel) = 0;
				
				//
			public:
				static poller* new_default_poller(event_loop* loop);
				
			public:
				void assert_in_loop_thread();
			
			private:
				event_loop*	owner_loop_;
		};
	}
}


#endif	//	#ifndef _zxero_network_poller_h_

