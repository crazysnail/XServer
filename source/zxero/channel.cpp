#include "channel.h"
#include "event_loop.h"
#include "log.h"

namespace zxero
{
	namespace network
	{
#ifdef ZXERO_OS_WINDOWS
		const int channel::k_none_event = 0;
		const int channel::k_read_event = 1;
		const int channel::k_write_event = 2;
		const int channel::k_error_event = 4;
		const int channel::k_close_event = 8;
		const int channel::k_invalid_event = 16;
		const int channel::k_remote_close_event = 32;
#else
		const int channel::k_none_event = 0;
		const int channel::k_read_event = POLLIN | POLLPRI;
		const int channel::k_write_event = POLLOUT;
		const int channel::k_error_event = POLLERR;
		const int channel::k_close_event = POLLHUP;
		const int channel::k_invalid_event = POLLNVAL;
		const int channel::k_remote_close_event = POLLRDHUP;
#endif

		channel::channel(event_loop* loop, socket_t sock)
			:loop_(loop)
			, socket_(sock)
			, events_(0)
			, revents_(0)
			, index_(-1)
			, event_handling_(false)
			, tied_(false)
		{
			LOG_TRACE << "channel ctor[" << this << "] at " << sock;
		}

		channel::~channel()
		{
			ZXERO_ASSERT(!event_handling_);
			LOG_TRACE << "channel dtor[" << this << "] at " << socket_;
		}

		void channel::tie(const boost::shared_ptr<void>& obj)
		{
			tie_ = obj;
			tied_ = true;
		}

		void channel::update()
		{
			loop_->update_channel(this);
		}

		void channel::remove()
		{
			//ZXERO_ASSERT(nonevent()); //[JD]
			loop_->remove_channel(this);
		}

		void channel::handle_event(timestamp_t overtime)
		{
			boost::shared_ptr<void> guard;
			if (tied_)
			{
				guard = tie_.lock();
				if (guard)
				{
					handle_event_with_guard(overtime);
				}
			}
			else
			{
				handle_event_with_guard(overtime);
			}
		}

		void channel::handle_event_with_guard(timestamp_t overtime)
		{
			event_handling_ = true;
			if ((revents_ & k_close_event) && !(revents_ & k_read_event))
			{
				if (close_callback_) close_callback_();
			}

			if (revents_ & (k_error_event | k_invalid_event))
			{
				//LOG_TRACE << "channel [" << this << "] handle error event " << revents_;
				if (error_callback_) error_callback_();
			}

			if (revents_ & (k_read_event | k_remote_close_event))
			{
				if (read_callback_) read_callback_(overtime);
			}
			
			if (revents_ & (k_write_event))
			{
				if (write_callback_) write_callback_();
			}
			event_handling_ = false;
		}

		////
		void channel::read_callback(const read_event_callback_t& cb) { read_callback_ = cb; }
		void channel::write_callback(const event_callback_t& cb) { write_callback_ = cb; }
		void channel::error_callback(const event_callback_t& cb) { error_callback_ = cb; }
		void channel::close_callback(const event_callback_t& cb) { close_callback_ = cb; }

		socket_t channel::fd() const
		{
			return socket_;
		}

		int channel::events() const
		{
			return events_;
		}

		void channel::revents(int revt)
		{
			revents_ = revt;
		}

		int channel::revents() const
		{
			return revents_;
		}

		bool channel::nonevent() const
		{
			return events_ == k_none_event;
		}

		bool channel::writing() const
		{
			return (events_ & k_write_event) != 0;
		}

		void channel::enable_reading()
		{
			events_ |= k_read_event;
			update();
		}

		void channel::enable_writing()
		{
			events_ |= k_write_event;
			update();
		}

		void channel::disable_reading()
		{
			events_ &= ~k_read_event;
			update();
		}

		void channel::disable_writing()
		{
			events_ &= ~k_write_event;
			update();
		}

		void channel::disable_all()
		{
			events_ = k_none_event;
			update();
		}

		int channel::index() const
		{
			return index_;
		}

		void channel::index(int idx)
		{
			index_ = idx;
		}

		event_loop* channel::owner_loop() const
		{
			return loop_;
		}
	}
}

