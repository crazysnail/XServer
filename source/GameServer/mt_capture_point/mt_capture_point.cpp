#include "mt_capture_point.h"
#include "module.h"
#include "../data/data_manager.h"
#include "../mt_server/mt_server.h"
#include "../base/mt_db_load_work.h"
#include "../mt_cache/mt_shm_manager.h"
#include "../mt_player/mt_player.h"
#include "../mt_guild/mt_guild_manager.h"
#include "../mt_guid/mt_guid.h"
#include "../mt_stage/mt_scene_stage.h"
#include "active_model.h"
#include <GuildCapturePoint.pb.h>
#include <S2GMessage.pb.h>
#include <SceneStageConfig.pb.h>


namespace Mt
{
MtCapturePointManager* __mt_capture_point_manager = nullptr;

void MtCapturePointManager::lua_reg(lua_State* ls)
{
	ff::fflua_register_t<MtCapturePointManager, ctor()>(ls, "MtCapturePointManager")
		.def(&MtCapturePointManager::FindCapturePoint, "FindCapturePoint")
		.def(&MtCapturePointManager::FindGuildCapturePoint,"FindGuildCapturePoint")
		.def(&MtCapturePointManager::RelateCapturePoint, "RelateCapturePoint")
		.def(&MtCapturePointManager::UpdateStrongHold, "UpdateStrongHold")
		.def(&MtCapturePointManager::AllChapterId, "AllChapterId")
		.def(&MtCapturePointManager::OnCaptureBattleEnd, "OnCaptureBattleEnd")
		.def(&MtCapturePointManager::FindBattleLog, "FindBattleLog")
		.def(&MtCapturePointManager::FindGuildBattleLog, "FindGuildBattleLog")
		.def(&MtCapturePointManager::CreateBattleLog, "CreateBattleLog")
		.def(&MtCapturePointManager::CapturePointBossGroupId, "CapturePointBossGroupId")
		.def(&MtCapturePointManager::CapturePointGuardBossId, "CapturePointGuardBossId")
		.def(&MtCapturePointManager::GetHookUserCount, "GetHookUserCount")
		.def(&MtCapturePointManager::LoadPlayerAndActor, "LoadPlayerAndActor")
		;
	ff::fflua_register_t<>(ls)
		.def(&MtCapturePointManager::Instance, "LuaMtCapturePointManager");
}

REGISTER_MODULE(MtCapturePointManager)
{
	require("data_manager");
	register_load_function(module_base::STAGE_LOAD, 
		boost::bind(&MtCapturePointManager::OnLoad, __mt_capture_point_manager));

}
MtCapturePointManager& MtCapturePointManager::Instance()
{
	return *__mt_capture_point_manager;
}

bool MtCapturePointManager::OnCacheAll()
{
	std::set<google::protobuf::Message *> msg_set;
	for (auto& pair : capture_points_) {
		msg_set.insert(pair.second.get());
	}
	for (auto& pair : guild_db_) {
		msg_set.insert(pair.second.get());
	}
	for (auto& pair : battle_logs_) {
		msg_set.insert(pair.second.get());
	}
	MtShmManager::Instance().Cache2Shm(msg_set, "MtCapturePointManager");
	return true;
}

int32 MtCapturePointManager::OnLoad()
{
	auto table = data_manager::instance()->get_table("scene_stage");
	if (table == nullptr) {
		ZXERO_ASSERT(false) << "cannot find table scene stage";
		return -1;
	}
	for (auto i = 0; i < table->get_rows(); ++i) {
		auto row = table->find_row(i);
		auto group_id = row->getInt("group");
		auto can_guild_canture = row->getInt("can_guild_capture") == 1;
		auto capture_boss = row->getInt("guild_capture_boss");
		auto guard_boss_id = row->getInt("guard_monster_id");
		if (can_guild_canture) {
			capture_point_monster_groups_[group_id].push_back(capture_boss);
			capture_point_bosses_[group_id].push_back(guard_boss_id);
		}
	}
	for (auto& pair : capture_point_monster_groups_) {
		if (pair.second.size() > 5) {
			std::vector<int32> monsters(pair.second.end() - 5, pair.second.end());
			pair.second.swap(monsters);
		}
	}
	for (auto& pair : capture_point_bosses_) {
		if (pair.second.size() > 5) {
			std::vector<int32> monsters(pair.second.end() - 5, pair.second.end());
			pair.second.swap(monsters);
		}
	}
	return 0;
}

CapturePointDB* MtCapturePointManager::FindCapturePoint(int32 chapter_id) const
{
	auto it = capture_points_.find(chapter_id);
	if (it == capture_points_.end())
		return nullptr;
	return it->second.get();
}

bool MtCapturePointManager::OnLoadAll(
	std::vector<boost::shared_ptr<CapturePointDB>>& cps,
	std::vector<boost::shared_ptr<GuildCapturePointDB>>& guild_cps,
	std::vector<boost::shared_ptr<GuildCaptureBattleLogDB>>& logs)
{
	for (auto& cp : cps) {
		capture_points_[cp->chapter_id()] = cp;
	}
	for (auto& capture_point : guild_cps) {
		guild_db_[capture_point->guid()] = capture_point;
	}
	for (auto& log : logs) {
		battle_logs_[log->guid()] = log;
	}
	for (auto& pair : capture_point_monster_groups_) {
		auto chapter_id = pair.first;
		if (capture_points_.find(chapter_id) == capture_points_.end()) {
			//初始化占点
			auto cp = boost::make_shared<CapturePointDB>();
			try
			{
				thread_lua->call<void>(script_id_, "OnCapturePointInit", chapter_id, cp);
				if (cp->chapter_id() > 0) {
					capture_points_[cp->chapter_id()] = cp;
				}
			}
			catch (ff::lua_exception& e)
			{
				LOG_ERROR << "[capture_point] init,lua:" << e.what();
			}
		}
	}
/*
	for (auto& pair : capture_points_) {
		try
		{
			thread_lua->call<void>(script_id_, "OnServerStartCheckCp", cp);
		}
		catch (ff::lua_exception& e)
		{
			LOG_ERROR << "[capture_point] server start,lua:" << e.what();
		}
	}*/
	return 0;
}

GuildCapturePointDB* MtCapturePointManager::FindGuildCapturePoint(uint64 guild_guid) const
{
	auto it = guild_db_.find(guild_guid);
	if (it == guild_db_.end())
		return nullptr;
	return it->second.get();
}

std::vector<int32> MtCapturePointManager::AllChapterId() const
{
	std::vector<int32> chapter_ids;
	chapter_ids.reserve(capture_point_monster_groups_.size());
	for (auto& pair : capture_point_monster_groups_)
		chapter_ids.push_back(pair.first);
	return chapter_ids;
}

void MtCapturePointManager::OnLoadPlayerAndActorDone(uint64 attacker_guid,
	uint64 defender_guid, int32 chapter_id, bool load_success)
{
	try
	{
		thread_lua->call<void>(script_id_, "OnLoadPlayerAndActorDone",
			attacker_guid, defender_guid, chapter_id, load_success);
	}
	catch (ff::lua_exception&)
	{
	}

}

void MtCapturePointManager::LoadPlayerAndActor(uint64 attacker_guid, 
	uint64 defender_guid,
	int32 chapter_id,
	std::set<uint64>& player_guids)
{
	auto work = new LoadCapturePlayerAndActor(attacker_guid, 
		defender_guid, chapter_id, player_guids);
	Server::Instance().AddDbLoadTask(work);
}

void MtCapturePointManager::OnStrongHoldUpdateDone(uint64 guild_guid,
	Packet::CG_UpdateStrongHold& data)
{
	try
	{
		thread_lua->call<void>(script_id_, "OnStrongHoldUpdateDone", guild_guid,
			&data);
	}
	catch (ff::lua_exception&)
	{
	}

}

void MtCapturePointManager::UpdateStrongHold(uint64 guild_guid,
	const Packet::CG_UpdateStrongHold* data)
{
	auto capture_point = FindGuildCapturePoint(guild_guid);
	if (capture_point == nullptr)
	{
		auto new_capture_point = boost::make_shared<Packet::GuildCapturePointDB>();
		new_capture_point->SetInitialized();
		new_capture_point->set_guid(guild_guid);
		for (int i = 0; i < 5; ++i) {
			auto sh = new_capture_point->add_strong_holds();
			for (int j = 0; j < 5; ++j) {
				sh->add_heros();
			}
			sh->SetInitialized();
		}
		guild_db_.insert({ new_capture_point->guid(), new_capture_point });
	}
	Server::Instance().AddDbLoadTask(new UpdateStrongHoldWork(guild_guid, data));
}

MtCapturePointManager::MtCapturePointManager()
{
	__mt_capture_point_manager = this;
}

void MtCapturePointManager::OnCaptureBattleEnd(uint64 attacker_guid,
	uint64 defender_guid,
	uint64 log_guid,
	bool defeat,
	int32 battle_index,
	uint64 reply_guid)
{
	try
	{
		thread_lua->call<void>(script_id_, "OnCaptureBattleEnd", attacker_guid,
			defender_guid, log_guid, defeat, battle_index, reply_guid);
	}
	catch (ff::lua_exception&)
	{
	}

}
GuildCaptureBattleLogDB* MtCapturePointManager::CreateBattleLog()
{
	auto log = boost::make_shared<GuildCaptureBattleLogDB>();
	for (int i = 0; i < 5; ++i) {
		auto detail = log->add_details();
		for (int j = 0; j < 5; ++j) {
			detail->add_attacker_heros();
			detail->add_defender_heros();
		}
	}
	log->SetInitialized();
	log->set_guid(MtGuid::Instance()(*log));
	battle_logs_[log->guid()] = log;
	return log.get();
}

GuildCaptureBattleLogDB* MtCapturePointManager::FindBattleLog(uint64 guid)
{
	if (battle_logs_.find(guid) != battle_logs_.end()) {
		return battle_logs_[guid].get();
	} else {
		return nullptr;
	}
}

std::vector<uint64> MtCapturePointManager::FindGuildBattleLog(uint64 guild_guid)
{
	std::vector<std::pair<uint64, int32>> log_and_time;//需要根据创建时间排序
	for (auto& log : battle_logs_) {
		if (log.second->attacker_guid() == guild_guid
			|| log.second->defender_guid() == guild_guid) {
			log_and_time.push_back({ log.first, log.second->battle_time() });
		}
	}
	std::sort(log_and_time.begin(), log_and_time.end(),
		[](auto& l, auto& r) {return l.second > r.second; });
	std::vector<uint64> log_guids;
	std::transform(log_and_time.begin(), log_and_time.end(),
		std::back_inserter(log_guids), 
		[](auto& pair) {return pair.first; });
	return log_guids;
}

int32 MtCapturePointManager::CapturePointBossGroupId(int32 chapter_id, int32 index) const
{
	if (capture_point_monster_groups_.find(chapter_id) == capture_point_monster_groups_.end())
		return 0;
	else
		return capture_point_monster_groups_.at(chapter_id)[index];
}

int32 MtCapturePointManager::CapturePointGuardBossId(int32 chapter_id, int32 index) const
{
	if (capture_point_bosses_.find(chapter_id) == capture_point_bosses_.end())
		return 0;
	else
		return capture_point_bosses_.at(chapter_id)[index];
}

void MtCapturePointManager::OnTick()
{
	for (auto& pair : capture_points_) {
		try
		{
			thread_lua->call<void>(script_id_, "OnTick", pair.second);
		}
		catch (ff::lua_exception& e)
		{
			LOG_ERROR << "[CapturePoint] lua:" << e.what();
		}
	}
}

void MtCapturePointManager::OnBigTick()
{
	hook_user_count_.clear();
	for (auto& pair : hook_distributes_) {
		for (int i = 0; i < pair.second.stage_id_size() && i < pair.second.player_count_size(); ++i) {
			auto stage_config = MtSceneStageManager::Instance().GetConfig(pair.second.stage_id(i));
			if (stage_config != nullptr) {
				hook_user_count_[stage_config->group()] += pair.second.player_count(i);
			}
		}
	}
}

int32 MtCapturePointManager::GetHookUserCount(int32 chapter_id) const
{
	if (hook_user_count_.find(chapter_id) != hook_user_count_.end()) {
		return hook_user_count_.at(chapter_id);
	} else {
		return 0;
	}
}

bool MtCapturePointManager::OnHookStageDistributeUpdate(
	const boost::shared_ptr<MtPlayer>& /*player*/,
	const boost::shared_ptr<S2G::HookStageDistributeUpdate>& message,
	int32 /*source*/)
{
	auto& data = hook_distributes_[message->scene_id()];
	data.CopyFrom(*message);
	return true;
}

CapturePointDB* MtCapturePointManager::RelateCapturePoint(uint64 guild_guid)
{
	for (auto& pair : capture_points_) {
		if (pair.second->capture_guild_guid() == guild_guid
			|| pair.second->challenger_guild_guid() == guild_guid) {
			return pair.second.get();
		}
	}
	return nullptr;
}

}