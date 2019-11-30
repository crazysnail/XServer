#include "poll_poller.h"
#include "event_loop.h"
#include "network_errno.h"
#include "channel.h"
#include "log.h"

namespace zxero
{
	namespace network
	{
		poll_poller::poll_poller(event_loop* loop)
		  : poller(loop)
		{
			LOG_TRACE << "poll_poller ctor[" << this << "]";
		}
		
		poll_poller::~poll_poller()
		{
			LOG_TRACE << "poll_poller dtor[" << this << "]";
		}
		
		timestamp_t poll_poller::poll(time_duration timeoutMs, channel_list_t& active_channels)
		{
		  // XXX pollfds_ shouldn't change
		  int numEvents = ::poll(&*pollfds_.begin(), pollfds_.size(), (int)timeoutMs.total_milliseconds());
		  int savedErrno = errno;
		  timestamp_t current_time(now());
		  if (numEvents > 0)
		  {
		    //LOG_TRACE << "poll_poller [" << this << "] " << numEvents << " events happended";
		   	//std::cout << numEvents << " events happended" << std::endl;
		    fill_active_channels(numEvents, active_channels);
		  }
		  else if (numEvents == 0)
		  {
		    //LOG_TRACE << " nothing happended";
		  }
		  else
		  {
		    if (savedErrno != E_INTR)
		    {
		      errno = savedErrno;
		      //LOG_SYSERR << "poll_poller::poll()";
		    }
		  }
		  return current_time;
		}
		
		void poll_poller::fill_active_channels(int numEvents, channel_list_t& active_channels) const
		{
		  for (poll_fd_list_t::const_iterator pfd = pollfds_.begin();
		      pfd != pollfds_.end() && numEvents > 0; ++pfd)
		  {
		    if (pfd->revents > 0)
		    {
		      --numEvents;
		      channel_map_t::const_iterator ch = channels_.find(pfd->fd);
		      ZXERO_ASSERT(ch != channels_.end());
		      channel* channel = ch->second;
		      ZXERO_ASSERT(channel->fd() == pfd->fd);
		      channel->revents(pfd->revents);
		      // pfd->revents = 0;
		      active_channels.push_back(channel);
		    }
		  }
		}
		
		void poll_poller::update_channel(channel* channel)
		{
		  poller::assert_in_loop_thread();
		  //LOG_TRACE << "fd = " << channel->fd() << " events = " << channel->events();
		  if (channel->index() < 0)
		  {
		    // a new one, add to pollfds_
			if (channels_.find(channel->fd()) != channels_.end())
			{
				ZXERO_ASSERT(false) << "have channel "<<channel->fd();
				return;
			}
		    struct pollfd pfd;
		    pfd.fd = channel->fd();
		    pfd.events = static_cast<short>(channel->events());
		    pfd.revents = 0;
		    pollfds_.push_back(pfd);
		    int idx = static_cast<int>(pollfds_.size())-1;
		    channel->index(idx);
		    channels_[pfd.fd] = channel;
		  }
		  else
		  {
		    // update existing one

			if (channels_.find(channel->fd()) == channels_.end())
			{
				ZXERO_ASSERT(false) << "no channel "<<channel->fd();
				return;
			}
			if (channels_[channel->fd()] != channel)
			{
				ZXERO_ASSERT(false) << "channel diff "<<channel->fd();
				return;
			}
		    int idx = channel->index();
			if (idx < 0 || idx >= static_cast<int>(pollfds_.size()))
			{
				ZXERO_ASSERT(false)<<"channel index error "<<channel->fd()<< " idx " << idx << " pollfds_.size() " << pollfds_.size();
				return;
			}
		    struct pollfd& pfd = pollfds_[idx];
			if (pfd.fd != channel->fd() && pfd.fd != (-channel->fd()-1))
			{
				ZXERO_ASSERT(false)<<"channel fd diff  pfd.fd "<< " pfd.fd " << pfd.fd << " channel->fd " << channel->fd() << " idx " << idx << " pollfds_.size() " << pollfds_.size();
				return;
			}
		    pfd.events = static_cast<short>(channel->events());
		    pfd.revents = 0;
		    if (channel->nonevent())
		    {
		      // ignore this pollfd
		      pfd.fd = -channel->fd()-1;
		    }
		  }
		}
		
		void poll_poller::remove_channel(channel* channel)
		{
		  poller::assert_in_loop_thread();
		  //LOG_TRACE << "fd = " << channel->fd();
		  if (channels_.find(channel->fd()) == channels_.end())
		  {
			  ZXERO_ASSERT(false)<<"no channel "<<channel->fd();
			  return;
		  }
		  if (channels_[channel->fd()] != channel)
		  {
			  ZXERO_ASSERT(false) << "channel diff "<<channel->fd();
			  return;
		  }
		  if (channel->nonevent() == false)
		  {
			  ZXERO_ASSERT(false)<<"channel "<<channel->fd() <<" no event";
			  //return;
		  }
		  
		  int idx = channel->index();
		  if (idx < 0 || idx >= static_cast<int>(pollfds_.size()))
		  {
			   ZXERO_ASSERT(false)<<"channel index error "<<channel->fd()<< " idx " << idx << " pollfds_.size() " << pollfds_.size();
			   return;
		  }
		 
		  const struct pollfd& pfd = pollfds_[idx]; (void)pfd;
		  ZXERO_ASSERT(pfd.fd == -channel->fd()-1 && pfd.events == channel->events());
		  size_t n = channels_.erase(channel->fd());
		  if (n != 1)
		  {
			  ZXERO_ASSERT(false)<<"channel index erase more "<<channel->fd()<< " idx " << idx << " pollfds_.size() " << pollfds_.size();
			  return;
		  }
		  (void)n;
		  if (pollfds_.empty())
		  {
			  ZXERO_ASSERT(false)<<"pollfds_ empty "<<channel->fd()<< " idx " << idx << " pollfds_.size() " << pollfds_.size() <<" channels_.size() " << channels_.size();
			  return;
		  }
		  if ((size_t)idx == pollfds_.size()-1)
		  {
		    pollfds_.pop_back();
		  }
		  else
		  {
		    socket_t channelAtEnd = pollfds_.back().fd;
		    //boost::swap(pollfds_.begin()+idx, pollfds_.end()-1);
		    pollfds_[idx] = pollfds_.back();
		    if (channelAtEnd < 0)
		    {
				LOG_INFO<<"channelAtEnd fa " << channelAtEnd << " pollfds_.size() " << pollfds_.size() <<" channels_.size() " << channels_.size();
				channelAtEnd = -channelAtEnd-1;
		    }
			auto channels_iter = channels_.find(channelAtEnd);
			if ( channels_iter != channels_.end())
			{
				channels_iter->second->index(idx);
			}
			else
			{
				//LOG
				LOG_ERROR<<"can`t find channels_ id " << channelAtEnd << " pollfds_.size() " << pollfds_.size() <<" channels_.size() " << channels_.size();
			}
		    
		    pollfds_.pop_back();
		  }
		}
	}
}

