#ifndef MTONLINE_GAMESERVER_MT_BATTLE_REPLY_MANAGER_H__
#define MTONLINE_GAMESERVER_MT_BATTLE_REPLY_MANAGER_H__
#include "../mt_types.h"
#include "../mt_server/mt_server.h"
#include <list>
#include <BattleReply.pb.h>
#include <S2GMessage.pb.h>

namespace Mt
{

	class MtBattleReplyManager : public boost::noncopyable
	{
	public:
		static void lua_reg(lua_State* ls);
		static MtBattleReplyManager& Instance();
		MtBattleReplyManager();
		bool OnCacheAll();
		int32 OnLoad() { return 0; }
		void LoadReplyAndSend(uint64 reply_guid, uint64 player_guid);
		bool OnCG_GetBattleReply(const boost::shared_ptr<client_session>& session,
			const boost::shared_ptr<Packet::CG_GetBattleReply>& msg, int32 /*source*/);
		bool OnSaveBattleReply(const boost::shared_ptr<MtPlayer>&,
			const boost::shared_ptr<S2G::SaveBattleReply>& msg, int32 /*source*/);
		//数据加载完毕后的回调
		void OnBattleReplyLoad(const boost::shared_ptr<Packet::BattleReply>& reply,
			uint64 player_guid);
		void ClearLog();
	private:
		const uint32 max_runtime_reply_ = 100 * 5;
		std::map<uint64, boost::shared_ptr<Packet::BattleReply>> replys_;
	};
}

#endif // MTONLINE_GAMESERVER_MT_BATTLE_REPLY_MANAGER_H__