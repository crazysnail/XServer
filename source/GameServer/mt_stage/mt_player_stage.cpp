#include "mt_player_stage.h"
#include "mt_scene_stage.h"
#include "module.h"
#include "../mt_item/mt_container.h"
#include "../data/data_manager.h"
#include "../mail/static_message_manager.h"
#include "active_model.h"
#include "../mt_guid/mt_guid.h"
#include "../mt_server/mt_server.h"
#include "../mt_battle/mt_battle_ground.h"
#include "../mt_item/mt_item.h"
#include "../mt_monster/mt_monster_manager.h"
#include "../mt_player/mt_player.h"
#include "../mt_actor/mt_actor.h"
#include "../base/mt_db_load_work.h"
#include "../mt_scene/mt_scene_manager.h"
#include "../mt_skill/mt_skill.h"
#include "../mt_item/mt_item_package.h"
#include "../mt_cache/mt_shm_manager.h"
#include "../mt_data_cell/mt_data_container.h"
#include "../mt_config/mt_config.h"
#include <SceneStageConfig.pb.h>
#include <BaseConfig.pb.h>
#include "../mt_scene/mt_scene.h"
#include <SceneStage.pb.h>
#include <S2GMessage.pb.h>

#include <boost/format.hpp>

namespace Mt
{

bool MtPlayerStageProxy::CanRaidStage(int32 stage_id)
{
	auto config = MtSceneStageManager::Instance().GetConfig(stage_id);
	if (config == nullptr)
		return false;
	for (auto pre_stage_id : config->pre_stages()) {
		auto previous_config = MtSceneStageManager::Instance().GetConfig(pre_stage_id);
		if (previous_config == nullptr || IsStageRaid(previous_config->id())) {
			return true;
		}
	}
	return false;
}

bool MtPlayerStageProxy::IsStageRaid(zxero::int32 stage_id)
{
	if (player_.expired())
		return false;
	auto stage_info = player_.lock()->GetPlayerStage(stage_id);
	if (stage_info == nullptr) {
		return false;
	} else {
		return stage_info->raid();
	}
}

zxero::int32 MtPlayerStageProxy::CalcChallengeBossCount(const int32 cur_count, int32 rush_count)
{
	if (player_.expired())
		return rush_count;
	auto container = player_.lock()->DataCellContainer();
	if (container != nullptr) {
		if (container->check_bit_data(Packet::BitFlagCellIndex::MonthCardFlag)
			|| container->check_bit_data(Packet::BitFlagCellIndex::LifeCardFlag)) {

			auto config = MtConfigDataManager::Instance().FindConfigValue("card_wave_value");
			if (config != nullptr && config->value1() > 0) {
				rush_count -= config->value1();
			}
		}
	}
	auto old = cur_count;
	if (old <= 20) {
		old += 2;
	} else if (old < rush_count) {
		old += 1;
	}

	if (old >= rush_count) {
		old = rush_count;
	}
	return old;
}

bool MtPlayerStageProxy::AddBossChallengeProgress(int32 stage_id, int32 count)
{
	auto result = false;
	if (player_.expired())
		return false;
	auto stage_info = player_.lock()->GetPlayerStage(stage_id);
	if (stage_info == nullptr) {
		return false;
	} else {
		auto config = MtSceneStageManager::Instance().GetConfig(stage_id);
		if (config != nullptr) {
			Packet::PlayerStage& stage_data = *stage_info;
			//if (stage_data.raid()) {
				int32 old_count = stage_data.den();
				if (old_count <= config->raid_count()) {
					stage_data.set_den(config->raid_count());
				}
				auto num = stage_data.num() + count;
				if (num >= stage_data.den()) {

					auto challenge_count = num / stage_data.den();
					auto new_num = num % stage_data.den();
					AddStageBossToEffi(stage_data.id(), challenge_count);
					stage_data.set_boss(stage_data.boss() + challenge_count);
					stage_data.set_num(new_num);
					int32 new_cout = CalcChallengeBossCount(old_count, config->rush_count());
					stage_data.set_den(new_cout);

					result = true;
				} else {
					stage_data.set_num(num);
				}
			//} else {//未通关, 最多只有一次挑战boss机会
			//	stage_data.set_den(config->raid_count());
			//	auto num = stage_data.num() + count;
			//	if (num >= config->raid_count()) {
			//		stage_data.set_boss(1);
			//		stage_data.set_num(config->raid_count());
			//	} else {
			//		stage_data.set_num(num);
			//	}
			//}
		}
		return result;
	}	
}

void MtPlayerStageProxy::PlayerCaptrueStage(int32 stage_id, bool win)
{
	if (player_.expired())
		return;
	auto msg = boost::make_shared<S2G::PlayerCaptureStatge>();
	msg->set_stage_id(stage_id);
	msg->set_win_battle(win);
	msg->mutable_player_info()->CopyFrom(player_.lock()->GetDBPlayerInfo()->basic_info());
	if (win) {
		auto actors = player_.lock()->BattleActor(Packet::ActorFigthFormation::AFF_STAGE_CAPTURE);
		for (auto& a : actors) {
			a->SerializationToMessage(*msg->add_actors());
		}
	}
	Server::Instance().SendMessage(msg, player_.lock());
}

void MtPlayerStageProxy::UpdateCapturedStage()
{
	if (player_.expired())
		return;
	auto msg = boost::make_shared<S2G::PlayerUpdateCaptureForm>();
	auto actors = player_.lock()->BattleActor(Packet::ActorFigthFormation::AFF_STAGE_CAPTURE);
	if (actors.empty())
		return;
	msg->mutable_player_info()->CopyFrom(player_.lock()->GetDBPlayerInfo()->basic_info());
	for (auto& a : actors) {
		a->SerializationToMessage(*msg->add_actors());
	}
	Server::Instance().SendMessage(msg, player_.lock());
}

void MtPlayerStageProxy::SyncHookData2Server()
{
	if (player_.expired())
		return;
	auto player_stage = player_.lock()->GetPlayerStageDB();
	auto stage_id = player_stage->current_stage();
	auto it = std::find_if(player_stage->configs().begin(),
		player_stage->configs().end(),
		boost::bind(&Packet::RTHC::stage_id, _1) == stage_id);
	if (it == player_stage->configs().end()) {
		return;
	}
	const Packet::RTHC& config = (*it);
	if (config.wave_mill_sec() <= 0) {
		return;
	}
	auto msg = boost::make_shared<S2G::PlayerHookDataUpdate>();
	msg->set_stage_id(stage_id);
	msg->set_guid(player_.lock()->Guid());
	msg->set_name(player_.lock()->Name());
	msg->set_time_cost(int32(config.wave_mill_sec()/1000.f));
	msg->set_hook_time(int32(config.hook_time()/ 1000.f));
	msg->set_income_per_min(int32( real32(config.wave_gold()) / config.wave_mill_sec() * 1000 * 60));
	msg->set_exp_pre_min(int32(real32(config.wave_exp()) / config.wave_mill_sec() * 1000 * 60));
	msg->set_score(player_.lock()->BattleScore());
	msg->mutable_form()->set_player_name(player_.lock()->Name());
	msg->mutable_form()->set_player_guid(player_.lock()->Guid());
	auto actors = player_.lock()->BattleActor(Packet::ActorFigthFormation::AFF_NORMAL_HOOK);
	for (auto& a : actors) {
		a->SerializationToMessage(*(msg->mutable_form()->add_actors()));
	}
	Server::Instance().SendMessage(msg, player_.lock());
}

bool MtPlayerStageProxy::OnPlayerRaidStage(int32 stage_id)
{
	auto config = MtSceneStageManager::Instance().GetConfig(stage_id);
	if (config == nullptr) {
		LOG_ERROR << "[player_stage] player raid stage config null:" << stage_id;
		return false;
	}
	if (player_.expired())
		return false;
	LOG_INFO << "[player_stage] player:" << player_.lock()->Guid() << " raid stage:" << stage_id;
	auto player_stage = player_.lock()->GetPlayerStage(stage_id);
	if (player_stage == nullptr) {
		LOG_ERROR << "[player_stage] player raid stage player data null:" << stage_id;
		return false;
	}
	if (player_stage->raid()) {
		return true;
	}
	player_stage->set_raid(true);
	auto next_stage_id = MtSceneStageManager::Instance().GetNextStageId(player_stage->id());
	player_.lock()->AddPlayerStage(next_stage_id);
	SyncSpecStageToClient(stage_id);
	SyncSpecStageToClient(next_stage_id);
	player_.lock()->OnLuaFunCall_x("xPlayerRaidStage",
	{
		{ "value", stage_id }
	});
	LOG_INFO << "[player_stage] player:" << player_.lock()->Guid() << " raid stage:" << stage_id << "success";
	player_.lock()->DBLog("RaidStage", boost::lexical_cast<string>(stage_id), stage_id);
	return true;
}

void MtPlayerStageProxy::GmPlayerRaidAllStage()
{
	auto stages = player_.lock()->GetPlayerStages();
	int32 last_stage = 0;
	for (auto& stage : stages) {
		last_stage = stage->id();
	}
	while (true) {
		last_stage = MtSceneStageManager::Instance().GetNextStageId(last_stage);
		if (last_stage > 0) {
			player_.lock()->AddPlayerStage(last_stage);
		} else {
			break;
		}
	}
	stages = player_.lock()->GetPlayerStages();
	for (auto& stage : stages) {
		stage->set_raid(true);
	}
	SyncSpecStageToClient(-1);
}

bool MtPlayerStageProxy::IncBossChallengeCount(int32 stage_id, int32 count)
{
	if (player_.expired())
		return false;
	auto player_stage = player_.lock()->GetPlayerStage(stage_id);
	if (player_stage == nullptr) {
		return false;
	} else {
		player_stage->set_boss(player_stage->boss() + count);
	}
	AddStageBossToEffi(stage_id, count);
	SyncEffe2Client();
	SyncSpecStageToClient(stage_id);
	return true;
}

void MtPlayerStageProxy::AddStageBossToEffi(int32 stage_id, int32 count)
{
	if (efficient_ == nullptr) return;
	auto it = std::find(efficient_->boss_stage_ids().begin(),
		efficient_->boss_stage_ids().end(), stage_id);
	if (it != efficient_->boss_stage_ids().end()) {
		auto index = std::distance(efficient_->boss_stage_ids().begin(), it);
		efficient_->set_boss_count(index, efficient_->boss_count(index) + count);
	} else {
		efficient_->add_boss_stage_ids(stage_id);
		efficient_->add_boss_count(count);
	}
}

bool MtPlayerStageProxy::DecBossChallengeCount(int32 stage_id, int32 count)
{
	if (player_.expired())
		return false;
	auto player_stage = player_.lock()->GetPlayerStage(stage_id);
	if (player_stage == nullptr) {
		return false;
	} else {
		int32 new_count = player_stage->boss() - count;
		if (new_count < 0) new_count = 0;
		player_stage->set_boss(new_count);
	}
	SyncSpecStageToClient(stage_id);
	return true;
}

void MtPlayerStageProxy::lua_reg(lua_State* ls)
{
	ff::fflua_register_t<MtPlayerStageProxy, ctor()>(ls, "MtPlayerStageProxy")
		.def(&MtPlayerStageProxy::PlayerCaptrueStage, "PlayerCaptrueStage")
		.def(&MtPlayerStageProxy::CanRaidStage, "CanRaidStage")
		.def(&MtPlayerStageProxy::IsStageRaid, "IsStageRaid")
		.def(&MtPlayerStageProxy::SyncEffe2Client, "SyncEffe2Client")
		.def(&MtPlayerStageProxy::CanRushStage, "CanRushStage")
		.def(&MtPlayerStageProxy::SyncSpecStageToClient, "SyncSpecStageToClient")
		.def(&MtPlayerStageProxy::ResetTo, "ResetTo")
		.def(&MtPlayerStageProxy::FirstNeedCalcId, "FirstNeedCalcId")
		.def(&MtPlayerStageProxy::AddBossChallengeProgress, "AddBossChallengeProgress")
		.def(&MtPlayerStageProxy::UpdateCapturedStage, "UpdateCapturedStage")
		.def(&MtPlayerStageProxy::ClearEfficentLog, "ClearEfficentLog")
		.def(&MtPlayerStageProxy::OnPlayerRaidStage, "OnPlayerRaidStage")
		.def(&MtPlayerStageProxy::GetPlayerStage, "GetPlayerStage")
		;
}

Packet::PlayerStage* MtPlayerStageProxy::GetPlayerStage(int32 stage_id)
{
	if (player_.expired())
		return nullptr;
	auto player_stage = player_.lock()->GetPlayerStage(stage_id);
	return player_stage;
}

bool MtPlayerStageProxy::CanAutoHook()
{
	if (player_.expired())
		return false;
	if (default_stage_ != nullptr) {
		return default_stage_->raid();
	}
	auto default_stage_id = player_.lock()->GetCamp() == Packet::Camp::Alliance ? 60101 : 60111;
	auto default_stage = player_.lock()->GetPlayerStage(default_stage_id);
	if (default_stage != nullptr) {
		default_stage_ = default_stage;
		return default_stage_->raid();
	}
	return false;
}

//int32 MtPlayerStageProxy::MakeTax(int32 stage_id, int32 gold)
//{
//	if (player_.expired())
//		return 0;
//	auto container = player_.lock()->DataCellContainer();
//	if (container != nullptr) {
//	if (container->check_bit_data(Packet::BitFlagCellIndex::MonthCardFlag)
//		||	container->check_bit_data(Packet::BitFlagCellIndex::LifeCardFlag )){
//		return 0;
//		}
//	}
//	auto scene_stage_info = MtSceneStageManager::Instance().GetSceneStageInfo(stage_id);
//	if (scene_stage_info == nullptr 
//		|| scene_stage_info->captain_guid() == INVALID_GUID
//		|| scene_stage_info->tax_rate() <= 0
//		|| scene_stage_info->captain_guid() == player_.lock()->Guid()) {
//		return 0;
//	}
//	auto tax = int32(scene_stage_info->tax_rate() / 100.f *  gold);
//	scene_stage_info->set_tax(scene_stage_info->tax() + tax); //tax涉及多线程访问问题,可能会漏加, 但应该不影响
//	return tax;
//}

uint64 MtPlayerStageProxy::GenerateReward(uint64 offline_mill_sec,
	Packet::RTHC& stage_config, bool last_stage) 
{
	auto left_hook_time = stage_config.total_time() - stage_config.hook_time();
	if (last_stage) { left_hook_time = offline_mill_sec;}

	int32 stage_id = stage_config.stage_id();
	if (stage_id == 0 || left_hook_time == 0) {
		return offline_mill_sec;
	}
	if (stage_config.calc_done() == false || stage_config.wave_mill_sec() == 0) {
		LOG_ERROR << "[player_stage] player:" << player_.lock()->Guid()
			<< ", stage calc not done, stage_id:" << stage_id
			<< ". reset to default freq 1/30.";
		stage_config.set_calc_done(true);
		stage_config.set_wave_mill_sec(30 * 1000);
	}
	auto scene_stage_config = MtSceneStageManager::Instance().GetConfig(stage_id);
	if (scene_stage_config == nullptr || scene_stage_config->normal_monsters_size() == 0) {
		LOG_ERROR << "[player_stage] null stage config:" << stage_id;
		return offline_mill_sec;
	}
	std::map<MtItemPackage*, int32> drops;
	uint64 stage_offline_seconds = std::minmax(offline_mill_sec, left_hook_time).first;
	offline_mill_sec -= stage_offline_seconds;
	stage_config.set_hook_time(stage_config.hook_time() + stage_offline_seconds);
	int32 wave = int32(stage_offline_seconds / stage_config.wave_mill_sec());
	int32 stage_dead_count = 0;
	if (stage_config.dead_mill_sec() > 0) {
		stage_dead_count = int32(stage_offline_seconds / stage_config.dead_mill_sec());
	}
	auto actors = player_.lock()->BattleActor(Packet::ActorFigthFormation::AFF_NORMAL_HOOK);
	std::for_each(actors.begin(), actors.end(),
		boost::bind(&MtActor::BattleDurableCost, _1, wave, true));
	if (stage_config.wave_kill() > 0) {
		for (auto i = 0; i < wave; ++i) {
			auto monster_group_id = scene_stage_config->normal_monsters(rand_gen->intgen(0, scene_stage_config->normal_monsters_size() - 1));
			auto monster_group_random_drop = MtMonsterManager::Instance().MonsterGroupRandomDrop(monster_group_id);
			for (auto& drop : monster_group_random_drop) {
				drops[drop.get()] = drops[drop.get()] + 1;
			}
			AddBossChallengeProgress(stage_id, 1);
		}
	}
	std::map<int32, int32> drop_items;
	for (auto& drop_pair : drops) {
		for (int i = 0; i < drop_pair.second; ++i) {
			drop_pair.first->GenItems(drop_items);
		}
	}
	ItemDrops(drop_items);
	auto tax_collector = [&](int gold) {
		return MtSceneStageManager::Instance().CollectTax(player_.lock(), stage_id, gold);
	};
	int32 stage_gold = wave * stage_config.wave_gold();
	stage_gold -= tax_collector(stage_gold);
	int32 stage_exp = wave * stage_config.wave_exp();
	int32 stage_exp_extra = 0;
	int32 monster_kills = wave * stage_config.wave_kill();
	int32 exp_point = player_.lock()->GetPlayerToken(Packet::TokenType::Token_ExpPoint);
	if (monster_kills > exp_point) {
		if (player_.lock()->DelItemById(Packet::TokenType::Token_ExpPoint, exp_point, Config::ItemDelLogType::DelType_OfflineHook, 0)) {
			float ratio = exp_point / float(monster_kills);
			stage_exp_extra = int32(stage_exp * ratio);
		}
	} else {
		if (player_.lock()->DelItemById(Packet::TokenType::Token_ExpPoint, monster_kills, Config::ItemDelLogType::DelType_OfflineHook, 0)) {
			stage_exp_extra = stage_exp;
		}
	}
	player_.lock()->AddItemById(Packet::TokenType::Token_Gold, stage_gold,
		Config::ItemAddLogType::AddType_Hook, nullptr, 0);
	player_.lock()->AddItemById(Packet::TokenType::Token_FormationExp, stage_exp + stage_exp_extra,
		Config::ItemAddLogType::AddType_Hook, nullptr, 0);
	std::map<MtActor*, int32> actor_deads;
	for (auto j = 0; j < stage_dead_count; ++j) {
		if (actors.empty()) break;
		auto dead_actor = actors[rand_gen->uintgen(0, actors.size() - 1)];
		actor_deads[dead_actor.get()] = actor_deads[dead_actor.get()] + 1;
	}
	for (auto& pair : actor_deads) {
		pair.first->DeadDurableCost(pair.second, true);
	}
	player_.lock()->OnLuaFunCall_x("xOnMonsterDead",
	{
		{ "monster_id", -1 },
		{ "stage_id", stage_config.stage_id() },
		{ "scene_id", scene_stage_config->scene_id() },
		{ "monster_class", -1 },
		{ "battle_type", Packet::BattleGroundType::PVE_HOOK },
		{ "monster_count" , monster_kills },
	});
/*
	if (stage_boss_count > 0) {
		AddStageBossToEffi(stage_id, stage_boss_count);
	}*/
	efficient_->set_battle_times(efficient_->battle_times() + wave);
	efficient_->set_total_exp(efficient_->total_exp() + stage_exp);
	efficient_->set_total_gold(efficient_->total_gold() + stage_gold);
	efficient_->set_total_exp_extra(efficient_->total_exp_extra() + stage_exp_extra);
	efficient_->set_monster_kills(efficient_->monster_kills() + monster_kills);
	efficient_->set_dead_times(efficient_->dead_times() + stage_dead_count);
	return offline_mill_sec;
}


int32 MtPlayerStageProxy::CheckDismantleEquip(const int32 id, const int32 count)
{
	if (player_.expired())
		return 0;

	auto player = player_.lock();

	auto equip_bag = player->FindContainer(Packet::ContainerType::Layer_EquipBag);
	if (equip_bag == nullptr) {
		LOG_ERROR << "player:" << player->Guid() << "has no equip bag";
		return 0;
	}	
	int32 dismantle = 0;
	//如果开启自动分解，优先处理
	if (player->AutoDismantleEquip(id, count)) {
		dismantle = count;
	}
	else{
		if (equip_bag->LeftSize() == 0) {
			dismantle = count;
		}
		else if (equip_bag->LeftSize() >= count) {
			player->AddItemById(id, count, Config::ItemAddLogType::AddType_OfflineHook);
		}
		else {
			player->AddItemById(id, equip_bag->LeftSize(), Config::ItemAddLogType::AddType_OfflineHook);
			dismantle = count - equip_bag->LeftSize();
		}
		//背包满就强制分解
		player->AutoDismantleEquip(id, dismantle,true);
	}
	return dismantle;
}

void MtPlayerStageProxy::ItemDrops(const std::map<int32,int32>& drop_items)
{
	if (player_.expired())
		return;
	//profiler timer("add_drop");
	int32 green_count = 0;//优秀装备
	int32 green_dismantle_count = 0;//优秀装备分解数量
	int32 blue_count = 0; //精良装备
	int32 blue_dismantle_count = 0;//优秀装备分解数量
	int32 purple_count = 0; //史诗装备
	auto player = player_.lock();

	google::protobuf::RepeatedPtrField<Packet::ItemCount> purple_items;
	for (auto& item_pair : drop_items) {
		if (item_pair.first != Packet::TokenType::Token_Gold
			&& item_pair.first != Packet::TokenType::Token_Exp
			&& item_pair.first != Packet::TokenType::Token_Hero_Exp) {
			auto config = MtItemManager::Instance().GetEquipmentConfig(item_pair.first);
			if (config != nullptr) {//装备
				if (config->color() == Config::EquipmentColor::EQUIP_GREEN){
					green_count += item_pair.second;
					green_dismantle_count += CheckDismantleEquip(item_pair.first, item_pair.second);
				}
				if (config->color() == Config::EquipmentColor::EQUIP_BLUE)	{
					blue_count += item_pair.second;				
					blue_dismantle_count += CheckDismantleEquip(item_pair.first, item_pair.second);
				}
				if (config->color() == Config::EquipmentColor::EQUIP_PURPLE){
					purple_count += item_pair.second;
					auto item = purple_items.Add();
					item->set_itemid(item_pair.first);
					item->set_itemcount(item_pair.second);
				}
			} else {//材料
				AddItemToEffi(item_pair.first, item_pair.second);
				player->AddItemById(item_pair.first, item_pair.second, Config::ItemAddLogType::AddType_OfflineHook);
			}
		}
	}
	if (purple_items.size() > 0) {
		static_message_manager::Instance().create_message(
			player->Guid(),
			nullptr,
			MAIL_OPT_TYPE::OFFLINE_HOOK_ADD,
			"offline_hook_purple_title",
			"offline_hook_purple_content",
			&purple_items);
	}
	efficient_->set_green_equip_count(green_count + efficient_->green_equip_count());
	efficient_->set_green_equip_dismantle(green_dismantle_count + efficient_->green_equip_dismantle());
	efficient_->set_blue_equip_count(blue_count + efficient_->blue_equip_count());
	efficient_->set_blue_equip_count_dismantle(blue_dismantle_count + efficient_->blue_equip_count_dismantle());
	efficient_->set_purple_equip_count(purple_count + efficient_->purple_equip_count());
}

void MtPlayerStageProxy::OnPlayerOnline(int32 offline_sec)
{
	uint64 offline_mill_seconds = offline_sec * 1000;
	if (player_.expired())
		return;
	auto player = player_.lock();
	if (efficient_->off_time() > 0) {
		SyncEffe2Client();
		return;
	}
	auto level = player->PlayerLevel();
	auto player_db = player->GetDBPlayerInfo();
	auto player_stages = player->GetPlayerStageDB();
	if (offline_mill_seconds <= 0 || player->CanAutoHook() == false)
		return;
	if (offline_mill_seconds > uint64(hours(7 * 24).total_milliseconds())) {
		offline_mill_seconds = hours(7 * 24).total_milliseconds();
	}
	const uint64 origin_offline_seconds = offline_mill_seconds;
	efficient_->Clear();
	efficient_->SetInitialized();
	efficient_->set_off_time(offline_mill_seconds);
	for (int32 i = 0; i < player_stages->configs_size(); ++i) {
		auto& config = (*player_stages->mutable_configs(i));
		LOG_INFO << "[player_stage] player:" << player->Guid()
			<< ",hook. stage:" << config.stage_id()
			<< ",done:" << config.calc_done()
			<< ",total_time:" << config.total_time()
			<< ",hook_time:" << config.hook_time()
			<< ",wave_mill_sec:" << config.wave_mill_sec()
			<< ",wave_exp:" << config.wave_exp()
			<< ",wave_gold:" << config.wave_gold()
			<< ",dead_mill_sec" << config.dead_mill_sec()
			<< ",left:" << offline_mill_seconds;
		bool last_stage = (i == player_stages->configs_size() - 1);
		offline_mill_seconds = GenerateReward(offline_mill_seconds, config, last_stage);
		LOG_INFO << "[player_stage] player:" << player->Guid()
			<< ",hook. stage:" << config.stage_id()
			<< ",done:" << config.calc_done()
			<< ",total_time:" << config.total_time()
			<< ",hook_time:" << config.hook_time()
			<< ",wave_mill_sec:" << config.wave_mill_sec()
			<< ",wave_exp:" << config.wave_exp()
			<< ",wave_gold:" << config.wave_gold()
			<< ",dead_mill_sec" << config.dead_mill_sec()
			<< ",left:" << offline_mill_seconds
			<< ",total_monster:" << efficient_->monster_kills()
			<< ",total_gold" << efficient_->total_gold()
			<< ",total_exp" << efficient_->total_exp();
		SyncSpecStageToClient(config.stage_id());
		if (offline_mill_seconds == 0) break;
	}
	efficient_->set_battle_time_costs(0);
	efficient_->set_up_level(player->PlayerLevel() - level);
	SyncEffe2Client();
}

MtPlayerStageProxy::MtPlayerStageProxy(boost::weak_ptr<MtPlayer> p) : player_(p)
{
	efficient_ = boost::make_shared<Packet::RealTimeHookEfficient>();
	efficient_->SetInitialized();
}

void MtPlayerStageProxy::ResetStageTime(int32 stage_id)
{
	if (player_.expired())
		return;
	auto player_stages = player_.lock()->GetPlayerStageDB();
	for (auto& config : (*player_stages->mutable_configs())) {
		if (config.stage_id() == stage_id) {
			auto total_time = config.total_time();
			config.Clear();
			config.SetInitialized();
			config.set_total_time(total_time);
			config.set_stage_id(stage_id);
			break;
		}
	}
}

void MtPlayerStageProxy::ClearEfficentLog()
{
	if (efficient_ == nullptr || player_.expired())
		return;
	auto player = player_.lock();
	efficient_->Clear();
	auto player_stages = player->GetPlayerStageDB();
	for (auto& config : (*player_stages->mutable_configs())) {
		if (config.stage_id() == player_stages->current_stage()){
			efficient_->mutable_config()->CopyFrom(config);
			break;
		}
	}
	if (efficient_->has_config() == false)
		return;
	efficient_->SetInitialized();
	SyncEffe2Client();
}

void MtPlayerStageProxy::OnMonsterDead(int32 count /*= 1*/)
{
	efficient_->set_monster_kills(efficient_->monster_kills() + count);
}

void MtPlayerStageProxy::OnItemDrop(int32 id, int32 count)
{
	if (id == Packet::TokenType::Token_Gold) {
		return OnAddGold(count);		
	}
	if (id == Packet::TokenType::Token_Exp) {
		return OnAddExp(count);
	}
	if (id == Packet::TokenType::Token_Hero_Exp) {//特殊用法, 为了不增加无效的TokenType,Token_Hero_Exp在这里代替双倍经验
		return OnAddExtraExp(count);
	}
	AddItemToEffi(id, count);
}

void MtPlayerStageProxy::AddItemToEffi(int32 id, int32 count)
{
	if (efficient_ == nullptr) return;
	auto it = std::find(efficient_->item_ids().begin(),
		efficient_->item_ids().end(), id);
	if (it == efficient_->item_ids().end()) {
		efficient_->add_item_ids(id);
		efficient_->add_item_counts(count);
	} else {
		int32 index = std::distance(efficient_->item_ids().begin(), it);
		efficient_->set_item_counts(index, efficient_->item_counts(index) + count);
	}
}

void MtPlayerStageProxy::OnAddExp(int32 exp)
{
	efficient_->set_total_exp(efficient_->total_exp() + exp);
}

void MtPlayerStageProxy::OnAddExtraExp(int32 exp)
{
	efficient_->set_total_exp_extra(efficient_->total_exp_extra() + exp);
}

void MtPlayerStageProxy::OnAddGold(int32 gold)
{
	efficient_->set_total_gold(efficient_->total_gold() + gold);
}

bool MtPlayerStageProxy::GiveRealTimeHookItem(uint64 elapse_millseconds)
{
	if (player_.expired()) return false;
	auto player = player_.lock();
	Packet::RTHC* stage_config = nullptr;
	auto realtime_hook_data = player->GetPlayerStageDB()->mutable_configs();
	for (auto i = 0; i < realtime_hook_data->size(); ++i) {
		stage_config = realtime_hook_data->Mutable(i);
		if (stage_config->stage_id() > 0 && stage_config->calc_done() == true) {
			auto left_time = stage_config->total_time() - stage_config->hook_time();
			if (left_time > 0 || i == realtime_hook_data->size() - 1) {
				break;
			}
		}
	}
	if (stage_config == nullptr || stage_config->stage_id() == 0
		|| stage_config->calc_done() == false) {
		return false;
	}

	if (stage_config->wave_mill_sec() > elapse_millseconds) {//单局战斗时间未到
		return false;
	}

	int32 stage_id = stage_config->stage_id();
	player->GetPlayerStageDB()->set_current_stage(stage_id);
	auto actors = player->BattleActor(Packet::ActorFigthFormation::AFF_NORMAL_HOOK);
	stage_config->set_hook_time(stage_config->hook_time() + stage_config->wave_mill_sec());
	if (stage_config->hook_time() >= stage_config->total_time()) {
		stage_config->set_hook_time(stage_config->total_time());
	}
	if (stage_config->dead_mill_sec() > 0) {
		dead_timer_ += stage_config->wave_mill_sec();
		if (dead_timer_ > stage_config->dead_mill_sec()) {
			dead_timer_ -= stage_config->dead_mill_sec();
			if (actors.empty() == false) {
				auto dead_actor = actors[rand_gen->uintgen(0, actors.size() - 1)];
				dead_actor->DeadDurableCost(1, false);
			}
			efficient_->set_dead_times(efficient_->dead_times() + 1);
		}
	}
	std::for_each(actors.begin(), actors.end(),
		boost::bind(&MtActor::BattleDurableCost, _1, 1, false));
	auto scene_stage_config = MtSceneStageManager::Instance().GetConfig(stage_id);
	if (scene_stage_config == nullptr || scene_stage_config->normal_monsters_size() == 0)
		return false;
	auto monster_group_id = scene_stage_config->normal_monsters(rand_gen->intgen(0, scene_stage_config->normal_monsters_size() - 1));
	auto monster_group_random_drop = MtMonsterManager::Instance().MonsterGroupRandomDrop(monster_group_id);
	auto group_monster_count = MtMonsterManager::Instance().MonsterCount(monster_group_id);
	if (stage_config->wave_kill() == group_monster_count) {
		std::map<int32, int32> drop_items;
		for (auto& drop : monster_group_random_drop) {
			drop->GenItems(drop_items);
		}
		ItemDrops(drop_items);

		player->OnLuaFunCall_x("xOnMonsterDead",
		{
			{ "monster_id", -1 },
			{ "stage_id", stage_id },
			{ "scene_id", scene_stage_config->scene_id() },
			{ "monster_class", -1 },
			{ "battle_type", Packet::BattleGroundType::PVE_HOOK },
			{ "monster_count" , group_monster_count },
		});
		efficient_->set_monster_kills(efficient_->monster_kills() + group_monster_count);
		AddBossChallengeProgress(stage_id, 1);
	}
	
	auto exp_point = player->GetPlayerToken(Packet::TokenType::Token_ExpPoint);
	if (exp_point > group_monster_count) {
		player->DelItemById(Packet::TokenType::Token_ExpPoint, group_monster_count, Config::ItemDelLogType::DelType_OfflineHook, 0);
	} else {
		player->DelItemById(Packet::TokenType::Token_ExpPoint, exp_point, Config::ItemDelLogType::DelType_OfflineHook, 0);
	}
	efficient_->set_battle_times(efficient_->battle_times() + 1);
	int32 total_gold = stage_config->wave_gold();
	auto tax_collector = [&](int gold) {
		return MtSceneStageManager::Instance().CollectTax(player, stage_id, gold);
	};
	total_gold -= tax_collector(total_gold);
	int32 total_exp = stage_config->wave_exp();
	int32 give_exp = total_exp;
	player->AddItemByIdWithNotify(Packet::TokenType::Token_Gold, total_gold,
		Config::ItemAddLogType::AddType_Hook);
	player->AddItemByIdWithNotify(Packet::TokenType::Token_FormationExp, give_exp,
		Config::ItemAddLogType::AddType_Hook);
	if (exp_point > 0) {
		player->AddItemByIdWithNotify(Packet::TokenType::Token_FormationExp, give_exp,
			Config::ItemAddLogType::AddType_Hook);
		efficient_->set_total_exp_extra(efficient_->total_exp_extra() + give_exp);
	}
	efficient_->set_total_exp(efficient_->total_exp() + total_exp);
	efficient_->set_total_gold(efficient_->total_gold() + total_gold);
	efficient_->set_off_time(efficient_->off_time() + stage_config->wave_mill_sec());
	SyncEffe2Client();
	return true;
}

void MtPlayerStageProxy::OnActorDead(MtActor* actor)
{
	actor;
	efficient_->set_dead_times(efficient_->dead_times() + 1);
}

void MtPlayerStageProxy::OnBattleEnd(MtBattleGround* battle)
{
	battle->UpdateBattleEarning();
	uint64 time_cost = battle->Brief().elapse_time();
	efficient_->set_battle_times(efficient_->battle_times() + 1);
	efficient_->set_battle_time_costs(0);
	//efficient_->set_battle_time_costs(efficient_->battle_time_costs() + time_cost);
	efficient_->set_off_time(efficient_->off_time() + time_cost);
	auto current_rthc = CurrentRTHC();
	if (current_rthc == nullptr) {
		LOG_ERROR << "player:" << player_.lock()->Guid()
			<< " current_rthc null, stage_id:" << player_.lock()->GetPlayerStageDB()->current_stage();
		return;
	}
	if (current_rthc->stage_id() != battle->Brief().stage_id()) {//应该不会发生这种情况
		LOG_INFO << "player:" << player_.lock()->Guid()
			<< " hook battle curr stage_id:" << battle->Brief().stage_id()
			<< ", need:" << current_rthc->stage_id();
		ZXERO_ASSERT(false);
		return;
	}
	UpdateRTHCData(time_cost);
	if (current_rthc->calc_done() && battle->GetClientInsight() == false){
		LOG_INFO << "[player_stage] player:" << player_.lock()->Guid()
			<< " calc done try end hook battle:" << battle->Brief().stage_id();
		player_.lock()->Scene()->QueueInLoop([=]() {
			if (player_.expired()) return;
			player_.lock()->QuitHookBattle();
		});
	}
}

Packet::RTHC* MtPlayerStageProxy::CurrentRTHC()
{
	if (player_.expired())
		return nullptr;
	auto player_stages = player_.lock()->GetPlayerStageDB();
	auto current_stage_id = player_stages->current_stage();
	if (current_stage_id == 0)
		return nullptr;
	auto it = std::find_if(player_stages->mutable_configs()->begin(),
		player_stages->mutable_configs()->end(),
		boost::bind(&Packet::RTHC::stage_id, _1) == current_stage_id);
	if (it == player_stages->mutable_configs()->end())
		return nullptr;
	return &*it;
}

bool MtPlayerStageProxy::UpdateRTHCData(uint64 battle_time)
{
	if (CurrentRTHC() == nullptr || player_.expired()) return false;
	auto current_rthc = CurrentRTHC();
	current_rthc->set_hook_time(current_rthc->hook_time() + battle_time);
	if (current_rthc->hook_time() > current_rthc->total_time()) {
		current_rthc->set_hook_time(current_rthc->total_time());
	}
	LOG_INFO << "[player stage] battle end, player:" << player_.lock()->Guid()
		<< ", stage:" << current_rthc->stage_id()
		<< ",hook_time:" << current_rthc->hook_time()
		<< ",battle_time:" << battle_time;
	auto stage_config = MtSceneStageManager::Instance().GetConfig(current_rthc->stage_id());
	if (stage_config == nullptr) {
		LOG_ERROR << "[player stage] stage_config:" << current_rthc->stage_id();
		return true;
	}
	if (current_rthc->calc_done() == false) {
		auto player = player_.lock();
		auto actor = player->BattleActor(Packet::ActorFigthFormation::AFF_NORMAL_HOOK);
		auto score = std::accumulate(actor.begin(), actor.end(), 0, [](auto sum, auto& a) {return sum + a->Score(); });
		current_rthc->SetInitialized();
/*
		current_rthc->set_main_actor_exp_freq(float(efficient_->total_exp()) / efficient_->off_time());
		current_rthc->set_hero_actor_exp_freq(current_rthc->main_actor_exp_freq());
		current_rthc->set_kill_monster_freq(float(efficient_->monster_kills()) / efficient_->off_time());
		current_rthc->set_battle_time_freq(float(efficient_->battle_times()) / efficient_->off_time());
		current_rthc->set_dead_freq(float(efficient_->dead_times()) / efficient_->off_time());
		current_rthc->set_gold_freq(float(efficient_->total_gold()) / efficient_->off_time());*/
/*
		if (current_rthc->main_actor_exp_freq() * 60 > stage_config->max_exp_pre_min()) {
			current_rthc->set_main_actor_exp_freq(stage_config->max_exp_pre_min() / 60.f);
			current_rthc->set_hero_actor_exp_freq(stage_config->max_exp_pre_min() / 60.f);
		}
		if (current_rthc->gold_freq() * 60 > stage_config->max_gold_pre_min()) {
			current_rthc->set_gold_freq(stage_config->max_gold_pre_min() / 60.f);
		}*/
		current_rthc->set_wave_exp(efficient_->total_exp() / efficient_->battle_times());
		current_rthc->set_wave_gold(efficient_->total_gold() / efficient_->battle_times());
		current_rthc->set_wave_mill_sec(efficient_->off_time() / efficient_->battle_times());
		current_rthc->set_wave_kill(efficient_->monster_kills() / efficient_->battle_times());
		current_rthc->set_relate_score(score);
		
		if (current_rthc->hook_time() >= uint64(minutes(10).total_milliseconds())) {
			//经验, 金币帽子
			int32 exp_freq = int32(current_rthc->wave_exp() * 1000.f * 60 / current_rthc->wave_mill_sec());
			int32 gold_freq = int32(current_rthc->wave_gold() * 1000.f * 60 / current_rthc->wave_mill_sec());
			int64 new_wave_time = 0;
			if (exp_freq > stage_config->max_exp_pre_min()) {
				new_wave_time = int64(current_rthc->wave_exp() * 1000.f * 60 / stage_config->max_exp_pre_min());
				current_rthc->set_wave_mill_sec(new_wave_time);
			}
			if (gold_freq > stage_config->max_gold_pre_min()) {
				auto temp = new_wave_time;
				new_wave_time = int64(current_rthc->wave_gold() * 1000.f * 60 / stage_config->max_gold_pre_min());
				if (new_wave_time < temp) {
					new_wave_time = temp;
				}
				current_rthc->set_wave_mill_sec(new_wave_time);
			}
			if (efficient_->dead_times() > 0) {
				current_rthc->set_dead_mill_sec(efficient_->off_time() / efficient_->dead_times());
			}
			LOG_INFO << "[playar_stage] plaeyr:" << player->Guid()
				<< ",rthc calc done, stage_id:" << current_rthc->stage_id()
				<< ",wave_mill_sec:" << current_rthc->wave_mill_sec()
				<< ",dead_mill_sec:" << current_rthc->dead_mill_sec()
				<< ",wave_exp:" << current_rthc->wave_exp()
				<< ",wave_gold:" << current_rthc->wave_gold();
			current_rthc->set_calc_done(true);
		}
	}
	return true;
}

void MtPlayerStageProxy::RecalcRealTimeHookData()
{
	if (player_.expired() || efficient_->IsInitialized() == false)
		return;
	auto player_stages = player_.lock()->GetPlayerStageDB();
	for (auto& config : (*player_stages->mutable_configs())) {
		auto total_time = config.total_time();
		auto stage_id = config.stage_id();
		config.Clear();
		config.SetInitialized();
		config.set_total_time(total_time);
		config.set_hook_time(0);
		config.set_stage_id(stage_id);
	}
	SyncEffe2Client();
}

void MtPlayerStageProxy::ResetTo(int32 stage_id)
{
	if (player_.expired())
		return;
	auto player = player_.lock();
	auto& configs = *(player->GetPlayerStageDB()->mutable_configs());
	for (auto& config : configs) {
		config.Clear();
		config.SetInitialized();
	}
	auto last_config = configs.Mutable(configs.size() - 1);
	last_config->set_stage_id(stage_id);
	last_config->set_total_time(hours(24).total_milliseconds());
	player->GetPlayerStageDB()->set_current_stage(FirstNeedCalcId());
	LOG_INFO << "[player_stage] player:" << player->Guid() << " real time hook reset to:" << stage_id;
	ClearEfficentLog();
}

void MtPlayerStageProxy::SyncEffe2Client()
{
	auto current_rthc = CurrentRTHC();
	if (current_rthc == nullptr) return;
	auto player = player_.lock();
	efficient_->mutable_config()->CopyFrom(*current_rthc);
	efficient_->set_level_up_need_seconds(player->MainActor()->LevelUpNeedExp());
	player->SendMessage(*efficient_);
	SyncSpecStageToClient(current_rthc->stage_id());
}

int32 MtPlayerStageProxy::FirstNeedCalcId()
{
	if (player_.expired()) return 0;
	auto& configs = *(player_.lock()->GetPlayerStageDB()->mutable_configs());
	for (auto& config : configs) {
		if (config.stage_id() > 0 && config.calc_done() == false) {
			return config.stage_id();
		}
	}
	return 0;
}

void MtPlayerStageProxy::SyncSpecStageToClient(int32 stage_id)
{
	if (player_.expired()) return;
	auto player = player_.lock();
	auto stage_db = player->GetPlayerStageDB();
	auto player_stages = player->GetPlayerStages();
	Packet::PlayerStages stage_to_client;
	stage_to_client.set_player_guid(stage_db->player_guid());
	stage_to_client.set_current_stage(stage_db->current_stage());
	stage_to_client.mutable_configs()->CopyFrom(stage_db->configs());
	if (stage_id == -1) {
		auto& stages = player_.lock()->GetPlayerStages();
		for (auto& stage : stages) {
			stage_to_client.add_stages()->CopyFrom(*stage);
		}
	} else {
		auto stage = player_.lock()->GetPlayerStage(stage_id);
		if (stage != nullptr) {
			stage_to_client.add_stages()->CopyFrom(*stage);
		}
	}

	player->SendMessage(stage_to_client);
}

void MtPlayerStageProxy::SetCaptureStageTempInfo(int32 stage_id,
	const std::vector<Packet::ActorFullInfo>& actors)
{
	capture_stage_info_ = std::make_tuple(stage_id, actors);
}

void MtPlayerStageProxy::ClearCaptureStageTempInfo()
{
	decltype(capture_stage_info_) dummy;
	capture_stage_info_.swap(dummy);
}

std::tuple<int32, std::vector<Packet::ActorFullInfo>> MtPlayerStageProxy::GetCaptureStageTempInfo()
{
	return capture_stage_info_;
}


}