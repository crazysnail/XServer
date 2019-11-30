#include "mt_battle_reply_manager.h"
#include "module.h"
#include "../base/mt_db_load_work.h"
#include "../base/client_session.h"
#include "../mt_cache/mt_shm_manager.h"
#include "../mt_player/mt_player.h"

namespace Mt
{
	static MtBattleReplyManager* __mt_arena_manager = nullptr;

	REGISTER_MODULE(MtBattleReplyManager)
	{
		register_load_function(module_base::STAGE_LOAD, boost::bind(&MtBattleReplyManager::OnLoad, __mt_arena_manager));

	}

	MtBattleReplyManager::MtBattleReplyManager()
	{
		__mt_arena_manager = this;
	}

	void MtBattleReplyManager::lua_reg(lua_State* ls)
	{
		ff::fflua_register_t<MtBattleReplyManager, ctor()>(ls, "MtBattleReplyManager")
			.def(&MtBattleReplyManager::LoadReplyAndSend, "LoadReplyAndSend")
			;
		ff::fflua_register_t<>(ls)
			.def(&MtBattleReplyManager::Instance, "LuaMtBattleReplyManager");
	}

	void MtBattleReplyManager::LoadReplyAndSend(uint64 reply_guid, uint64 player_guid)
	{
		Server::Instance().AddDbLoadTask(new LoadBattleReply(reply_guid, player_guid));
	}

	bool MtBattleReplyManager::OnCG_GetBattleReply(
		const boost::shared_ptr<client_session>& session,
		const boost::shared_ptr<Packet::CG_GetBattleReply>& msg, int32 /*source*/)
	{
		auto p = session->player();
		if (p == nullptr) {
			return true;
		}
		if (msg->guid() == INVALID_GUID) {
			p->SendClientNotify("battle_reply_invalid_guid", -1, -1);
			return true;
		}
		auto it = replys_.find(msg->guid());
		if (it != replys_.end()) {
			p->SendMessage(*(it->second));
			return true;
		}
		LOG_INFO << "[battle_reply] reply:" << msg->guid()
			<< ",not found in memory, try load and send later";
		LoadReplyAndSend(msg->guid(), p->Guid());
		return true;
	}

	bool MtBattleReplyManager::OnSaveBattleReply(
		const boost::shared_ptr<MtPlayer>&,
		const boost::shared_ptr<S2G::SaveBattleReply>& msg, int32 /*source*/)
	{
		if (msg->reply().ByteSize() > std::pow(2,24)) {
			LOG_WARN << "[battle reply] guid:" << msg->reply().guid()
				<< ",type:" << msg->reply().type()
				<< ",scene_id:" << msg->reply().scene_id()
				<< " too big, cannot save";
			return true;
		}
		auto reply = boost::make_shared<Packet::BattleReply>(msg->reply());
		OnBattleReplyLoad(reply, INVALID_GUID);
		return true;
	}

	MtBattleReplyManager& MtBattleReplyManager::Instance()
	{
		return *__mt_arena_manager;
	}

	void MtBattleReplyManager::OnBattleReplyLoad(const boost::shared_ptr<Packet::BattleReply>& reply, uint64 player_guid)
	{
		boost::shared_ptr<MtPlayer> player;
		if (player_guid != INVALID_GUID) {
			player = Server::Instance().FindPlayer(player_guid);
		}
		if (reply == nullptr) {
			if (player != nullptr) {
				player->SendClientNotify("battle_reply_out_of_date", -1, -1);
			}
			return;
		}
		if (player != nullptr) {
			player->SendMessage(*reply);
		}
		auto it = replys_.find(reply->guid());
		if (it != replys_.end()) {
			replys_.erase(it);
			LOG_INFO << "[battle_reply] reply in memory,erase. guid:" << reply->guid();
		}
		if (replys_.size() >= max_runtime_reply_) {
			replys_.erase(replys_.begin());
		}
		replys_[reply->guid()] = reply;
	}

	bool MtBattleReplyManager::OnCacheAll()
	{
		
		std::set<google::protobuf::Message *> msg_set;
		for (auto& pair : replys_) {
			msg_set.insert(pair.second.get());
		}
		MtShmManager::Instance().Cache2Shm(msg_set, "MtBattleReplyManager");
		return true;
	}

	void MtBattleReplyManager::ClearLog()
	{
		replys_.clear();
	}

}