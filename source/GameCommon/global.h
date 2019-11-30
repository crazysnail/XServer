//	����ͷ�ļ���ʡ���°Ѹð��Ķ�����һ��
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
    
    //  ������ö�ٲ���
    enum server_param
    {
        //  ��Ϸ������idλ��
        kGameServerIdBit = 8,
        kGameServerNumber = 1 << kGameServerIdBit,
        
        //  �ͻ��˻Ự����λ��
        kClientSessionIndexBit = 14,
        kClientSessionNumber = 1 << kClientSessionIndexBit,
        
        //  �ͻ��˻Ựħ����λ��
        kClientSessionMagicBit = 10,
        kClientSessionMagicMask = (1 << kClientSessionMagicBit) - 1,
        
    };
    
	//  �ͻ��˻ػ�id������ʱΨһid
	union client_session_id
	{
		//  32λΨһ��ʶ��
		uint32 csid;

		//  �ֶα�ʾ
		struct
		{
			//  10λħ��������������ظ��߼�������Ϊ0
			uint32  magic_number : 10;
			//  8λ��Ϸ������id�����256����Ϸ������������������������������Ƶ�
			uint32  game_server_id : 8;
			//  14λ�����������ڱ�ʾ��ǰ��ռ�õĻỰ, ���16384���ͻ�������
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

	//  ��Ч�ĻỰid
	const static client_session_id kClientSessionInvalidId(0);

	//  ��������������ڹ����̳߳�ʼ��ʱ�ᴴ��
	typedef boost::any work_context_t;

	//
}

#endif	//#ifndef _game_sotcc_common_h_

