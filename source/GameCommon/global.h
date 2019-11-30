//	公共头文件，省点事把该包的都包在一起
#ifndef _game_sotcc_common_h_
#define _game_sotcc_common_h_

#include "types.h"
//#include "Account.pb.h"
#include "log.h"

namespace zxero
{
	class log_message;
}
namespace Mt
{
	class server_session;
	typedef boost::shared_ptr<server_session> server_session_ptr;
}
namespace game
{
    using namespace zxero;
	using namespace network;
    
    //  服务器枚举参数
    enum server_param
    {
        //  游戏服务器id位数
        kGameServerIdBit = 8,
        kGameServerNumber = 1 << kGameServerIdBit,
        
        //  客户端会话索引位数
        kClientSessionIndexBit = 14,
        kClientSessionNumber = 1 << kClientSessionIndexBit,
        
        //  客户端会话魔术数位数
        kClientSessionMagicBit = 10,
        kClientSessionMagicMask = (1 << kClientSessionMagicBit) - 1,
        
    };
    
	//  客户端回话id，运行时唯一id
	union client_session_id
	{
		//  32位唯一标识符
		uint32 csid;

		//  分段表示
		struct
		{
			//  10位魔术数，避免快速重复逻辑，不会为0
			uint32  magic_number : 10;
			//  8位游戏服务器id，最多256个游戏服务器，所以最大在线人数是有限制的
			uint32  game_server_id : 8;
			//  14位索引数，用于标示当前被占用的会话, 最多16384个客户端连接
			uint32  index : 14;
		};

		explicit client_session_id(uint32 id = 0) :csid(id) {}

		client_session_id(uint32 m, uint32 gsid, uint32 idx)
			:magic_number(m)
			, game_server_id(gsid)
			, index(idx)
		{

		}

		operator uint32() const {
			return csid;
		}

		client_session_id& operator=(const uint32& v)
		{
			csid = v;
			return *this;
		}

		bool invalid() const
		{
			return csid == 0;
		}
	};

	//  无效的会话id
	const static client_session_id kClientSessionInvalidId(0);

	//  工作环境，这个在工作线程初始化时会创建
	typedef boost::any work_context_t;

	//
}

#endif	//#ifndef _game_sotcc_common_h_

