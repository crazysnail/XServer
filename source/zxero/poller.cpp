#include "poller.h"
#include "event_loop.h"
#include "select_poller.h"
#include "poll_poller.h"

namespace zxero
{
	namespace network
	{
		poller::poller(event_loop* loop)
			: owner_loop_(loop)
		{
		}
		
		poller::~poller()
		{
		}
		
		void poller::assert_in_loop_thread()
		{
			owner_loop_->assert_in_loop_thread();
		}
		
		poller* poller::new_default_poller(event_loop* loop)
		{
#ifdef ZXERO_OS_WINDOWS
			return new select_poller(loop);
#else
			return new poll_poller(loop);
#endif
		}
	}
}
