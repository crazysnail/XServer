//	timer_id��from muduo
#ifndef _zxero_network_timer_id_h_
#define _zxero_network_timer_id_h_


namespace zxero
{
	namespace network
	{
		//	���ݳ�Ա�������캯��ȫ��˽�У��ⲿ���������timer_id���ڲ�����
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
        
        //  ��Ч��timer_id
        const static timer_id kInvalidTimerId;
	}
}


#endif	//	#ifndef _zxero_network_timer_id_h_
