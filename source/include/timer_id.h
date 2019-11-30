//	timer_id，from muduo
#ifndef _zxero_network_timer_id_h_
#define _zxero_network_timer_id_h_


namespace zxero
{
	namespace network
	{
		//	数据成员连带构造函数全部私有，外部不允许操作timer_id的内部内容
		class timer_id
		{
		public:
			timer_id() : timer_(nullptr) {}

			bool operator == (const timer_id& r) const{
				return timer_ == r.timer_;
			}

			bool valid() const {
				return timer_ != nullptr;
			}
		private:
			friend class timer_queue;

			timer_id(timer* tq) :timer_(tq){}

			void reset() {
				timer_ = nullptr;
			}
		private:
			timer* timer_;
		};
        
        //  无效的timer_id
        const static timer_id kInvalidTimerId;
	}
}


#endif	//	#ifndef _zxero_network_timer_id_h_
