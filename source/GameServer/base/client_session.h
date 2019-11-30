//
//  client_session.h
//  GameServer
//
//  Created by zXero on 14-4-7.
//  Copyright (c) 2014年 zxero. All rights reserved.
//
//  GameSession
//

#ifndef __GameServer__client_session__
#define __GameServer__client_session__

#include "session.h"
#include "../mt_types.h"

namespace Mt
{

    //  客户端状态
    enum client_session_state
    {
        //  授权协议已收到，正在验证
        kVerifing = kNextAviableState,
        //  验证通过后, 可能会排队
        kInQueue,
		// 尚未进入场景
		kBeforEnteringScene,
        //  玩家数据获取成功后，就可以正式开始游戏了
        kGaming,
		//	离开阶段
		kLeaving,
    };
        
	using zxero::network::tcp_connection;
	using zxero::network::protobuf_codec;
    class client_session : public session
    {
    public:
        client_session( const boost::shared_ptr<tcp_connection>& conn, protobuf_codec& codec);
        ~client_session();
            
    public:
            
        //  切换会话状态
        virtual bool state( uint32 s );
            
        //  绑定player
        void bind_player(const boost::shared_ptr<MtPlayer>& p)
		{
            player_ = p;
        }
            
        //  获取player
		boost::shared_ptr<MtPlayer>& player() {
            return player_;
        }            

		void set_account(const boost::shared_ptr<Packet::AccountModel> &account) { account_ = account; }
		const boost::shared_ptr<Packet::AccountModel> get_account() const {return account_;}
		void set_logininfo(const boost::shared_ptr<Packet::LoginRequest> &logininfo) { logininfo_ = logininfo; }
		const boost::shared_ptr<Packet::LoginRequest> get_logininfo() const { return logininfo_; }
		
		void reset_frame_packet() { frame_packets_ = 0; }
		void incr_frame_packet() { frame_packets_++; }
		int32 get_frame_packet() const {  return frame_packets_; }
		
		std::string get_ip();
    private:
        //  当前玩家
        boost::shared_ptr<MtPlayer> player_;
		boost::shared_ptr<Packet::AccountModel> account_;
		boost::shared_ptr<Packet::LoginRequest> logininfo_;
		int32 frame_packets_ = 0;
    };  //  client_session
        
    //typedef boost::shared_ptr<client_session> client_session_ptr;
}   //  namespace Mt


#endif /* defined(__GameServer__client_session__) */
