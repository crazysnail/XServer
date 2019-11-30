//
//  client_protocol_dispatcher.h
//  GameServer
//
//  Created by zXero on 14-4-7.
//  Copyright (c) 2014年 zxero. All rights reserved.
//
//  客户端协议分发器
//  

#ifndef __GameServer__client_protocol_dispatcher__
#define __GameServer__client_protocol_dispatcher__

#include "types.h"
#include "event_loop.h"
#include "date_time.h"
#include "EnterScene.pb.h"
#include <boost/noncopyable.hpp>
#include <boost/function.hpp>

namespace Mt
{
	using namespace zxero;
	using namespace zxero::network;
	class Server;
	class client_session;

	template <typename TARGET>
	class message_handler : boost::noncopyable
	{
	public:
		virtual ~message_handler() {}
		
		/**
		* \brief 执行消息对应的逻辑
		* \prama source 消息来源 参考MessageSource
		* \retval true 继续执行
		* \retval false 下个tick再执行
		*/
		virtual bool on_message(const boost::shared_ptr<TARGET>&,
			const boost::shared_ptr<google::protobuf::Message>&,
			int32 /*source*/) = 0;
	};

	template<typename TARGET, typename T>
	class message_handler_instance : public message_handler<TARGET>
	{
	public:
		typedef boost::function<bool (const boost::shared_ptr<TARGET>&,
			boost::shared_ptr<T>&,
			int32 /*source*/)> handle_function;
		typedef T message_type;
		explicit message_handler_instance(const handle_function& handle)
			:handle_(handle)
		{
		}

		virtual bool on_message(const boost::shared_ptr<TARGET>& tar,
			const boost::shared_ptr<google::protobuf::Message>& msg,
			int32 source) override
		{
			return handle_(tar, *(boost::shared_ptr<T>*)&msg, source);
		}

	public:
		handle_function handle_;
	};
	
	template <typename TARGET, typename OWNER_CLASS>
	struct thread_checker
	{
		bool operator()(OWNER_CLASS* owner,
			const boost::shared_ptr<TARGET>& obj,
			const boost::shared_ptr<google::protobuf::Message>& msg)
		{
			if (obj != nullptr && obj->Scene() != nullptr && obj->Scene().get() != owner
				&& msg->GetDescriptor() != Packet::PlayerEnterScene::descriptor())
				return false;
			return true;
		}
	};
	template <typename TARGET>
	struct thread_checker<TARGET, Mt::Server>
	{
		bool operator()(Mt::Server* /*owner*/,
			const boost::shared_ptr<TARGET>& /*obj*/, 
			const boost::shared_ptr<google::protobuf::Message>& /*msg*/)
		{
			return true;
		}
	};


	template <typename OWNER, typename TARGET>
	class message_dispatcher : boost::noncopyable
	{
	public:
		typedef boost::function<bool (const boost::shared_ptr<TARGET>&,
			const boost::shared_ptr<google::protobuf::Message>&,
			int32)> handler_type;
		message_dispatcher(OWNER* owner, event_loop* loop)
			:owner_(owner), loop_(loop) {}
		void default_handle(bool (OWNER::*handle)(
			const boost::shared_ptr<TARGET>&,
			const boost::shared_ptr<google::protobuf::Message>&,
			int32))
		{
			default_handler_ = boost::bind(handle, owner_, _1, _2, _3);
		}
		bool direct_handle_message(const boost::shared_ptr<TARGET>& tar,
			const boost::shared_ptr<google::protobuf::Message>& msg,
			int32 source) 
		{
			return on_message(tar, msg, source);
		}
		void handle_message(const boost::shared_ptr<TARGET>& tar,
			const boost::shared_ptr<google::protobuf::Message>& msg,
			int32 source)
		{
			loop_->run_in_loop(boost::bind(&message_dispatcher::on_message, this, tar, msg, source));
		}
		template <typename Message>
		message_dispatcher& add_handler(bool (OWNER::*handler)(
			const boost::shared_ptr<TARGET>&, 
			const boost::shared_ptr<Message>& msg,
			int32))
		{
			static_assert(std::is_base_of<google::protobuf::Message, Message>::value,
				"Message should inherit from google::protobuf::Message");
			ZXERO_ASSERT(handler_map_.find(Message::descriptor()) == handler_map_.end())
				<< " duplicate message handler";
			typename message_handler_instance<TARGET, Message>::handle_function cb
				= boost::bind(handler, owner_, _1, _2, _3);
			auto h = boost::make_shared<message_handler_instance<TARGET, Message>>(cb);
			handler_map_[Message::descriptor()] = h;
			return *this;
		}
		template <typename Message, typename SINGTON>
		message_dispatcher& add_handler(bool (SINGTON::*handler)(
			const boost::shared_ptr<TARGET>&,
			const boost::shared_ptr<Message>& msg,
			int32))
		{
			static_assert(std::is_base_of<google::protobuf::Message, Message>::value,
				"Message should inherit from google::protobuf::Message");
			ZXERO_ASSERT(handler_map_.find(Message::descriptor()) == handler_map_.end())
				<< " duplicate message handler";
			typename message_handler_instance<TARGET, Message>::handle_function cb
				= boost::bind(handler, &SINGTON::Instance(), _1, _2, _3);
			auto h = boost::make_shared<message_handler_instance<TARGET, Message>>(cb);
			handler_map_[Message::descriptor()] = h;
			return *this;
		}
	private:
		bool on_message(const boost::shared_ptr<TARGET>& tar,
			const boost::shared_ptr<google::protobuf::Message>& msg,
			int32 source)
		{
			auto it = handler_map_.find(msg->GetDescriptor());
			if (it != handler_map_.end()) {
				return it->second->on_message(tar, msg, source);
			} else {
				return default_handler_(tar, msg, source);
			}
		}
		OWNER* owner_;
		event_loop* loop_;
		std::map<const google::protobuf::Descriptor*, 
			boost::shared_ptr<message_handler<TARGET>>> handler_map_;
		handler_type default_handler_;
	};
/*
	template <typename TARGET, typename OWNER_CLASS>
	class object_protocal_dispatcher : boost::noncopyable
	{
	public:
		typedef boost::function<void(const boost::shared_ptr<TARGET>&, const boost::shared_ptr<google::protobuf::Message>&, zxero::timestamp_t)> call_back_type;
		object_protocal_dispatcher(OWNER_CLASS* owner, event_loop* loop, const call_back_type& default_cb) :loop_(loop), default_callback_(default_cb), owner_(owner)   {}
		void dispatch_message(const boost::shared_ptr<TARGET>& obj, const boost::shared_ptr<google::protobuf::Message>& msg, zxero::timestamp_t t)
		{
			if (loop_->is_in_loop_thread()) {
				auto iter = callback_map_.find(msg->GetDescriptor());
				if (iter != callback_map_.end())
				{
					if (checker_(owner_, obj, msg))
						iter->second->on_message(obj, msg, t);
					else
						return;
				}
				else
				{
					default_callback_(obj, msg, t);
				}
			}
			else {
				loop_->run_in_loop(boost::bind(&object_protocal_dispatcher<TARGET, OWNER_CLASS>::dispatch_message, this, obj, msg, t));
			}
		}
		template <typename Message>
		object_protocal_dispatcher& reg(void (OWNER_CLASS::*handler)(const boost::shared_ptr<client_session>&, const boost::shared_ptr<Message>& msg, zxero::timestamp_t))
		{
			static_assert(std::is_base_of<google::protobuf::Message, Message>::value, "Message should inherit from google::protobuf::Message");
			typename message_handler_instance<client_session, Message>::handle_function cb
				= boost::bind(handler, owner_, _1, _2, _3);
			register_handler<Message>(cb);
			return *this;
		}
	private:
		template <typename MESSAGE_TYPE>
		void register_handler(const typename message_handler_instance<TARGET, MESSAGE_TYPE>::handle_function& cb)
		{
			auto pcb = boost::make_shared<message_handler_instance<TARGET, MESSAGE_TYPE>>(cb);
			callback_map_[MESSAGE_TYPE::descriptor()] = pcb;
		}
		event_loop* loop_;
		std::map<const google::protobuf::Descriptor*, boost::shared_ptr<message_handler<TARGET>>> callback_map_;
		call_back_type default_callback_;
		OWNER_CLASS* owner_ = nullptr;
		thread_checker<TARGET, OWNER_CLASS> checker_;
	};
        
	template <typename OWNER_CLASS>
	class client_protocol_dispatcher : boost::noncopyable
	{
	public:
		typedef boost::function<void(const boost::shared_ptr<client_session>&, const boost::shared_ptr<google::protobuf::Message>&, zxero::timestamp_t)> client_protocol_callback_t;
		client_protocol_dispatcher(OWNER_CLASS* owner,
			event_loop* loop, const client_protocol_callback_t& default_cb)
			:owner_(owner), loop_(loop), default_callback_(default_cb) {}

		void on_client_message(const boost::shared_ptr<tcp_connection> &conn, const boost::shared_ptr<google::protobuf::Message> &msg, zxero::timestamp_t t)
		{
			loop_->run_in_loop(boost::bind(&client_protocol_dispatcher::on_client_message_in_loop, this, conn, msg, t));
		}
		template <typename Message>
		client_protocol_dispatcher& reg(void (OWNER_CLASS::*handler)(const boost::shared_ptr<client_session>&, const boost::shared_ptr<Message>& msg, zxero::timestamp_t))
		{
			typename message_handler_instance<client_session, Message>::handle_function cb
				= boost::bind(handler, owner_, _1, _2, _3);
			register_handler<Message>(cb);
			return *this;
		}
	private:
		void on_client_message_in_loop(const boost::shared_ptr<tcp_connection> &conn, const boost::shared_ptr<google::protobuf::Message> &msg, zxero::timestamp_t t);
		template< typename Message >
		void register_handler(const typename message_handler_instance<client_session, Message>::handle_function& cb)
		{
			static_assert(std::is_base_of<google::protobuf::Message, Message>::value, "_ITYPE should inherit from google::protobuf::Message");
			typedef typename message_handler_instance<client_session, Message>::message_type msg_t;
			auto pcb = boost::make_shared< message_handler_instance<client_session, Message> >(cb);
			callback_map_[msg_t::descriptor()] = pcb;
		}
		OWNER_CLASS* owner_;
		event_loop* loop_;
		typedef std::map<const google::protobuf::Descriptor*, boost::shared_ptr<message_handler<client_session>>> callback_map;
		callback_map callback_map_;
		client_protocol_callback_t default_callback_;
	};*/
        
}   //  namespace Mt

#endif /* defined(__GameServer__client_protocol_dispatcher__) */
