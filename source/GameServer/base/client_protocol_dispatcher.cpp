//
//  client_protocol_dispatcher.cpp
//  GameServer
//
//  Created by zXero on 14-4-7.
//  Copyright (c) 2014年 zxero. All rights reserved.
//

#include "LoginPacket.pb.h"
#include "date_time.h"
#include "client_protocol_dispatcher.h"
#include "client_session.h"
#include "tcp_connection.h"

namespace Mt {
using namespace zxero;
         
/*
template <typename OWNER>
void client_protocol_dispatcher<OWNER>::on_client_message_in_loop(const boost::shared_ptr<tcp_connection> &conn, const boost::shared_ptr<google::protobuf::Message> &msg, zxero::timestamp_t t)
{
	//  客户端会话
	boost::shared_ptr<client_session> session_;

	//  获取会话环境
	boost::any& c = conn->context();
	if (c.empty())
	{
		LOG_ERROR << "connection [" << conn << "] has no context for protocol: " << msg->GetTypeName();
		conn->force_close();
		return;
	} else
	{
		//  安全第一
		try
		{
			session_ = boost::any_cast<boost::shared_ptr<client_session>>(c);
		}
		catch (boost::bad_any_cast& e)
		{
			LOG_ERROR << "connection [" << conn << "] have invalid context: " << e.what();
			conn->force_close();
			return;
		}
		ZXERO_ASSERT(session_) << "connection [" << conn << "] get context failed";
	}

	//  验证状态处理消息
	switch (session_->get_state()) {
	case kUnauthorization:
		if (msg->GetDescriptor() != Packet::LoginRequest::descriptor()) {
			LOG_ERROR << "kUnauthorization connection [" << conn << "] receive  unauthorization protocol : " << msg->GetTypeName();
			conn->force_close();
			return;
		}
		break;
		/ *           case kVerifing:
		{
		if (msg->GetDescriptor() != Packet::PlayerLoginRequest::descriptor()) {
		LOG_ERROR << "kVerifing connection [" << conn << "] receive  unauthorization protocol : " << msg->GetTypeName();
		conn->force_close();
		return;
		}
		}
		break;* /
	case kInQueue:
	case kDisconnected:
	case kDisconnecting:
		//  不处理不合时宜的协议，直接跳过
	{
		LOG_WARN << "connection [" << conn << "] receive invalid protocol : " << msg->GetTypeName() << " on state " << session_->get_state();
		return;
	}break;
	default:
		break;
	}   //  switch (session_->state())

		//  将消息分发出去
	callback_map::iterator iter = callback_map_.find(msg->GetDescriptor());
	try
	{
		if (iter != callback_map_.end())
		{
			iter->second->on_message(session_, msg, t);
		} else
		{
			default_callback_(session_, msg, t);
		}
	}
	catch (std::exception& e)
	{
		LOG_ERROR << e.what();
	}
}
*/

}   //  namespace Mt