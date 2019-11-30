#include "tcp_connection.h"
#include "event_loop.h"
#include "channel.h"
#include "tcp_socket.h"
#include "network_errno.h"
#include "log.h"

namespace zxero
{
	namespace network
	{
		void default_connection_callback(const tcp_connection_ptr& conn)
		{
			LOG_INFO << conn->local_address().ip_port() << " -> "
				<< conn->peer_address().ip_port() << " is "
				<< (conn->connected() ? "UP" : "DOWN")
				;
		}

		void default_message_callback(const tcp_connection_ptr& conn, buffer* buf, timestamp_t overtime)
		{
			overtime;
			LOG_INFO << conn << "read data size " << buf->readable_bytes() ;
			buf->retrieve_all();
		}

		////
		tcp_connection::tcp_connection(event_loop* loop, const std::string& name, tcp_socket_ptr& sock)
			:loop_(loop)
			, name_(name)
			, state_(kConnecting)
			, socket_()
			, channel_(new channel(loop, sock->fd()))
			, local_address_(sock->local_address())
			, peer_address_(sock->peer_address())
		{
			socket_.swap(sock);
			channel_->read_callback(bind(&tcp_connection::handle_read, this, _1));
			channel_->write_callback(bind(&tcp_connection::handle_write, this));
			channel_->close_callback(bind(&tcp_connection::handle_close, this));
			channel_->error_callback(bind(&tcp_connection::handle_error, this));

			LOG_TRACE << "tcp_connection ctor[" << name_ << "] at " << this << " fd = " << socket_->fd() ;
			socket_->keep_alive(true);
			no_delay(true);
		}

		tcp_connection::~tcp_connection()
		{
			LOG_TRACE << "tcp_connection dtor[" << name_ << "] at " << this << " fd = " << socket_->fd() ;
		}

		event_loop* tcp_connection::get_loop() const
		{
			return loop_;
		}

		const std::string& tcp_connection::name() const
		{
			return name_;
		}

		const inet_address& tcp_connection::local_address() const
		{
			return local_address_;
		}

		const inet_address& tcp_connection::peer_address() const
		{
			return peer_address_;
		}

		bool tcp_connection::connected() const
		{
			return state_ == kConnected;
		}

		//	zxero: 目前这个逻辑是有问题的
		void tcp_connection::send(const void* message, size_t len)
		{
			if (state_ == kConnected)
			{
				if (loop_->is_in_loop_thread())
				{
					send_in_loop(message, len);
				}
				else
				{
					loop_->run_in_loop(boost::bind(&tcp_connection::send_in_loop, shared_from_this(), std::string((const char*)message, len)));
				}
			}
            else
            {
                LOG_ERROR << "tcp_connection [" << this << "] send message " << message << " failed";
            }
		}

		void tcp_connection::send(const std::string& message)
		{
			send(message.c_str(), message.size());
		}

		void tcp_connection::send(buffer* message)
		{
			if (state_ == kConnected)
			{
				if (loop_->is_in_loop_thread())
				{
					send_in_loop(message->peek(), message->readable_bytes());
					message->retrieve_all();
				}
				else
				{
					loop_->run_in_loop(boost::bind(&tcp_connection::send_in_loop, shared_from_this(), message->retrieve_all_as_string()));
				}
			}
		}

		void tcp_connection::send_in_loop(const std::string& msg)
		{
			send_in_loop(msg.c_str(), msg.size());
		}

		void tcp_connection::send_in_loop(const void* msg, size_t len)
		{
			loop_->assert_in_loop_thread();
			int nwrote = 0;
			int nlogwrote = 0;
			int error = net_errno();
			size_t remaining = len;
			bool faultError = false;
			if (state_ == kDisconnected)
			{
				LOG_WARN << "tcp_connection [" << this << "] disconected, give up writing" ;
				return;
			}
			if (output_buffer_.size() >= 10 * 1024 * 1024)
			{
				LOG_ERROR << "#send data#buffer to big.[" << output_buffer_.size() << "]. force close";
				force_close();
				return;
			}
			timestamp_t start_write = now();
			if (!channel_->writing() && output_buffer_.readable_bytes() == 0)
			{
				nwrote = socket_->write(msg, len);
				nlogwrote = nwrote;
				if (nwrote >= 0)
				{
					remaining = len - nwrote;
					if (remaining == 0 && write_complete_callback_) 
					{
						loop_->queue_in_loop(boost::bind(write_complete_callback_, shared_from_this()));
					}
				}
				else
				{
					error = net_errno();
					nwrote = 0;
					if (error == E_WOULD_BLOCK) {
						LOG_INFO << "#send data#E_WOULD_BLOCK" << this << "[" << remaining << "][" << len << "]";
					} else {
						if (error == E_PIPE)
						{
							LOG_ERROR << "#send data#E_PIPE ERROR[" << remaining << "][" << len << "]";
						} else {
							LOG_ERROR << "#send data#ERROR[" << error << "][" << remaining << "][" << len << "]. froce close";
							force_close();
						}
						faultError = true;
					}
				}
			}
			timestamp_t end_write = now();
			int32 writetimes = (int32)((end_write-start_write).total_milliseconds());
			ZXERO_ASSERT(remaining <= len) << this;
			if (!faultError && remaining > 0)
			{
				output_buffer_.append((const char*)msg + nwrote, remaining);
				if (!channel_->writing())
					channel_->enable_writing();
			}
			int32 appendtimes = (int32)((now()-end_write).total_milliseconds());
			if ( (writetimes+appendtimes) > 1000)
			{
				LOG_WARN<<"#send data#["<<remaining<<"]["<<len<<"]["<<writetimes<<"]["<<appendtimes<<"]["<<nlogwrote<<"]["<<error<<"]";
			}
		}

		int hexCharToInt(char c)  
		{   
			if (c >= '0' && c <= '9') return (c - '0');  
			if (c >= 'A' && c <= 'F') return (c - 'A' + 10);  
			if (c >= 'a' && c <= 'f') return (c - 'a' + 10);  
			return 0;  
		}  

		char* hexstringToBytes(std::string s)  
		{           
			int sz = s.length();  
			char *ret = new char[sz/2];  
			for (int i=0 ; i <sz ; i+=2) {  
				ret[i/2] = (char) ((hexCharToInt(s.at(i)) << 4)  
					| hexCharToInt(s.at(i+1)));  
			}  
			return ret;  
		}  

		std::string bytestohexstring(char* bytes,int bytelength)  
		{  
			std::string str("");  
			std::string str2("0123456789abcdef");   
			for (int i=0;i<bytelength;i++) {  
				int b;  
				b = 0x0f&(bytes[i]>>4);  
				//char s1 = str2.at(b);  
				str.append(1,str2.at(b));            
				b = 0x0f & bytes[i];  
				str.append(1,str2.at(b));  
				//char s2 = str2.at(b);  
			}  
			return str;  
		}  

		void byteToHexStr(unsigned char* byte_arr, int arr_len, std::string* hexstr)
		{
			for (int i=0;i<arr_len; ++i)
			{

				char hex1;

				char hex2;

				/*借助C++支持的unsigned和int的强制转换，把unsigned char赋值给int的值，那么系统就会自动完成强制转换*/

				int value=byte_arr[i];

				int S=value/16;

				int Y=value % 16;

				//将C++中unsigned char和int的强制转换得到的商转成字母

				if (S>=0&&S<=9)

					hex1=(char)(48+S);

				else

					hex1=(char)(55+S);

				//将C++中unsigned char和int的强制转换得到的余数转成字母

				if (Y>=0&&Y<=9)

					hex2=(char)(48+Y);

				else

					hex2=(char)(55+Y);

				//最后一步的代码实现，将所得到的两个字母连接成字符串达到目的

				*hexstr=*hexstr+hex1+hex2;

			}
		}

		

		////
		void tcp_connection::shutdown()
		{
			if (state_ == kConnected)
			{
				state(kDisconnecting);
				loop_->run_in_loop(bind(&tcp_connection::shutdown_in_loop, shared_from_this()));
			}
		}

		void tcp_connection::shutdown_in_loop()
		{
			loop_->assert_in_loop_thread();
			if (!channel_->writing())
			{
				socket_->shutdown_write();
			}
		}

		////
		void tcp_connection::force_close()
		{
			if (state_ == kConnected)
			{
				state(kDisconnecting);
				loop_->run_in_loop(bind(&tcp_connection::force_close_in_loop, shared_from_this()));
			}
		}

		void tcp_connection::force_close_in_loop()
		{
			loop_->assert_in_loop_thread();
			if (state_ == kConnected || state_ == kDisconnecting)
			{
				handle_close();
			}
		}

		////
		void tcp_connection::no_delay(bool on)
		{
			socket_->no_delay(on);
		}

		////
		void tcp_connection::state(StateE s)
		{
			state_ = s;
		}

		////
		void tcp_connection::context(const boost::any& context)
		{
			context_ = context;
		}
		const boost::any& tcp_connection::context() const
		{
			return context_;
		}
		boost::any& tcp_connection::context()
		{
			return context_;
		}

		////
		void tcp_connection::connection_callback(const connection_callback_t& cb) {
			connection_callback_ = cb;
		}
		void tcp_connection::message_callback(const message_callback_t& cb) {
			message_callback_ = cb;
		}
		void tcp_connection::write_complete_callback(const write_complete_callback_t& cb) {
			write_complete_callback_ = cb;
		}
		void tcp_connection::close_callback(const close_callback_t& cb) {
			close_callback_ = cb;
		}

		////
		void tcp_connection::connect_established()
		{
			loop_->assert_in_loop_thread();
			ZXERO_ASSERT(state_ == kConnecting) << this;
			state(kConnected);
			//	这个tie是绑定的意思，确保channel_不会使用一个空旋指针
			channel_->tie(shared_from_this());
			channel_->enable_reading();

			connection_callback_(shared_from_this());
		}

		void tcp_connection::connect_destroyed()
		{
			loop_->assert_in_loop_thread();
			if (state_ == kConnected)
			{
				state(kDisconnected);
				channel_->disable_all();

				connection_callback_(shared_from_this());
			}
			channel_->remove();
		}

		////
		void tcp_connection::handle_read(timestamp_t overtime)
		{
			loop_->assert_in_loop_thread();
			timestamp_t time_tick = now();
			int saved_errno = 0;
			int n = input_buffer_.read(*socket_.get(), &saved_errno);
			if (n > 0)
			{
				message_callback_(shared_from_this(), &input_buffer_, overtime);
			}
			else if (n == 0)
			{
				handle_close();
			}
			else
			{
				errno = saved_errno;
				handle_error();
			}
			int32 ticktimes = (int32)((now()-time_tick).total_milliseconds());
			if ( ticktimes > 1000)
			{
				LOG_TRACE<<"#READTICK#["<<n<<"]["<<input_buffer_.readable_bytes()<<"]["<<ticktimes<<"]";
			}
		}

		void tcp_connection::handle_write()
		{
			loop_->assert_in_loop_thread();
			if (channel_->writing())
			{
				int n = socket_->write(output_buffer_.peek(), output_buffer_.readable_bytes());
				int error = net_errno();
				if (n > 0)
				{
					output_buffer_.retrieve(n);
					if (output_buffer_.readable_bytes() == 0)
					{
						channel_->disable_writing();
						if (write_complete_callback_)
						{
							loop_->queue_in_loop(boost::bind(write_complete_callback_, shared_from_this()));
						}
						if (state_ == kDisconnecting)
						{
							shutdown_in_loop();
						}
					}
				}
				else
				{
					LOG_ERROR << "tcp_connection [" << this << "] handle_write n = " << n << " with state " << state_ << error;
				}
			}
			else
			{
				//	...	
			}
		}

		void tcp_connection::handle_close()
		{
			loop_->assert_in_loop_thread();
			ZXERO_ASSERT(state_ == kConnected || state_ == kDisconnecting) << this << " state is " << state_;
			state(kDisconnected);
			channel_->disable_all();
			
			tcp_connection_ptr guard_this(shared_from_this());
			connection_callback_(guard_this);
			close_callback_(guard_this);
		}

		void tcp_connection::handle_error()
		{
			loop_->assert_in_loop_thread();
			//	
			LOG_ERROR << "tcp_connection [" << this << "] handle_error errno = " << errno << " with state " << state_ ;
			//	这是暂时处理，对此还是需要细节对待一下的
			//	zXero: 如果这里不改成kConnected，handle_close的时候就一定会断言失败。。。
			//	zXero: 确保所有的error_event默认就有close_event存在
			//state(kConnected);
			//handle_close();

			//	zXero: 安全处理，确保已经连接的socket发生错误时可以被关闭
			//	JD:同步XX代码 handle error 同时处理 kConnecting error
			if (state_ == kConnected || state_ == kDisconnecting)
			{
				if (state_ == kDisconnecting)
				{
					LOG_WARN <<"tcp_connection ["<< this <<"] handle_error errno =" << errno << "close with state" << state_;
				}
				handle_close();
			}

		}
	}
}
