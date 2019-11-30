#include "../mt_types.h"
#include "../mt_scene/mt_scene.h"
#include "../mt_config/mt_config.h"
#include <random>
#include <Upgrades.pb.h>
#include <ItemConfig.pb.h>
#include <EquipmentConfig.pb.h>
#include <HeroFragment.pb.h>
#include <HeroFragmentConfig.pb.h>
#include <SceneCommon.pb.h>
#include <BaseConfig.pb.h>
#include <ActorConfig.pb.h>
#include "tcp_connection.h"
#include "../base/client_session.h"
#include "../mt_player/mt_player.h"
#include "../mt_actor/mt_actor.h"
#include "../mt_actor/mt_actor_config.h"
#include "../mt_item/mt_container.h"
#include "../mt_data_cell/mt_data_container.h"
#include "../mt_item/mt_item.h"
#include "../mt_upgrade/mt_upgrade.h"
#include "../mt_skill/mt_skill.h"



namespace Mt
{
	bool MtScene::OnUpgradeLevel(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<Packet::UpgradeLevelReq>& message, int32 /*source*/)
	{
		auto actor_it = std::find_if(std::begin(player->Actors()), std::end(player->Actors()), boost::bind(&MtActor::Guid, _1) == message->actor_guid());
		Packet::UpgradeLevelReply reply;
		reply.mutable_req()->CopyFrom(*message);
		if (actor_it == std::end(player->Actors()))
		{
			reply.set_result(Packet::UpgradeLevelReply_Result::UpgradeLevelReply_Result_INVALID_TARGET);
			player->SendMessage(reply);
			return true;
		}
		auto actor = *actor_it;
		if (actor->MainActor()) {
			reply.set_result(Packet::UpgradeLevelReply_Result::UpgradeLevelReply_Result_INVALID_TARGET);
			player->SendMessage(reply);
			return true;
		}
		LOG_INFO << "[level up] player:" << player->Guid()
			<< ",level:" << player->PlayerLevel()
			<< ",actor:" << actor->Guid() << ",level:" << actor->Level()
			<< ", type:" << message->one_key();
		int32 item_count = 0;
		for (auto& item_pair : MtUpgradeConfigManager::Instance().ExpItem()) {
			auto item = player->FindItemByConfigId(Packet::ContainerType::Layer_ItemBag, item_pair.first);
			if (item) item_count += item->Count();
		}
		if (item_count < 1) {
			reply.set_result(Packet::UpgradeLevelReply_Result::UpgradeLevelReply_Result_NO_ENOUGH_ITEM);
			return true;
		}
		if (actor->Level() == player->PlayerLevel() && actor->FullExp()) {
			reply.set_result(Packet::UpgradeLevelReply_Result::UpgradeLevelReply_Result_MAX_LEVEL);
			player->SendMessage(reply);
			return true;
		}
		//目标等级
		int32 target_level = 0;
		if (message->one_key()) {//一键升级, 最多升级至当前玩家等级
			target_level = player->PlayerLevel();
		} else {
			target_level = actor->Level() + 1;
		}
		int32 need_exp = 0;
		//补充当前等级所需经验
		auto config = MtActorConfig::Instance().GetLevelData(actor->Level());
		if (config == nullptr) {
			player->SendClientNotify("InvalidConfigData", -1, -1);
			return true;
		}
		need_exp = config->hero_actor_exp() - actor->DbInfo()->exp();
		for (auto level = actor->Level() + 1; level < target_level; ++level) {
			config = MtActorConfig::Instance().GetLevelData(level);
			if (config == nullptr) {
				player->SendClientNotify("InvalidConfigData", -1, -1);
				return true;
			}
			need_exp += config->hero_actor_exp();
		}
/*
		int32 final_exp = 0;
		config = MtActorConfig::Instance().GetLevelData(target_level);
		if (config == nullptr) {
			player->SendClientNotify("InvalidConfigData", -1, -1);
			return true;
		}
		final_exp = config->hero_actor_exp() - 1;*/
		int32 add_exp = 0;
		std::vector<std::pair<int32, int32>> item_use;
		auto exp_item_configs = MtUpgradeConfigManager::Instance().ExpItem();
		for (auto& item_config : exp_item_configs) {
			auto item = player->FindItemByConfigId(Packet::ContainerType::Layer_ItemBag, item_config.first);
			if (item != nullptr) {
				if (item->Count() * item_config.second >= need_exp) {
					auto need_count = int32(std::ceil(need_exp / float(item_config.second)));
					add_exp += item_config.second * need_count;
					item_use.push_back({ item->ConfigId(), need_count });
				} else {
					add_exp += item->Count() * item_config.second;
					item_use.push_back({ item->ConfigId(), item->Count() });
				}
				need_exp -= add_exp;
				if (need_exp <= 0) break;
			}
		}
		for (auto& use : item_use) {
			auto item = player->FindItemByConfigId(Packet::ContainerType::Layer_ItemBag, use.first);
			if (!player->DelItemById(item->ConfigId(), use.second, Config::ItemDelLogType::DelType_Upgrade_Level, actor->Guid())) {
				reply.set_result(Packet::UpgradeLevelReply_Result::UpgradeLevelReply_Result_NO_ENOUGH_ITEM);
				player->SendMessage(reply);
				return true;
			}
		}
		actor->AddExp(add_exp, false);
		actor->AddExp(0, true);
		reply.set_result(Packet::UpgradeLevelReply_Result::UpgradeLevelReply_Result_SUCCESS);
		LOG_INFO << "[level up] player:" << player->Guid()
			<< ",level:" << player->PlayerLevel()
			<< ",actor:" << actor->Guid() << ",level:" << actor->Level() << ",done";
		player->SendMessage(reply);
		return true;
	}

	bool MtScene::OnUpgradeColor(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<Packet::UpgradeColorReq>& message, int32 /*source*/)
	{
		if (player == nullptr) {
			return true;
		}

		//功能解锁校验
		if (player->OnLuaFunCall_n(100, "CheckFunction", { { "fc_name_3", 999 } }) != Packet::ResultCode::ResultOK) {
			return true;
		}

		auto actor = player->FindActor(message->info().actor_guid());
		if (actor == nullptr) {
			player->SendCommonResult(Packet::ResultOption::Actor_Opt, Packet::ResultCode::NoActor);
			return true;
		}
		LOG_INFO << "[upgrade color] player:" << player->Guid()
			<< ",actor:" << actor->Guid() << ",color:" << actor->DbInfo()->color();
		//目标颜色
		auto up_color = actor->DbInfo()->color() + 1;
		if (up_color > Packet::ActorColor_MAX) {
			player->SendCommonResult(Packet::ResultOption::Actor_Opt, Packet::ResultCode::ColorMax);
			return true;
		}

		if (up_color != message->up_to_color() ) {
			player->SendCommonResult(Packet::ResultOption::Actor_Opt, Packet::ResultCode::InvalidColor);
			return true;
		}

		//检查目标方案需求
		auto upgrade_require = MtUpgradeConfigManager::Instance().UpgradeColorRequirement(actor->DbInfo()->profession(), (Packet::ActorColor)up_color);
		if (upgrade_require == nullptr) {
			player->SendCommonResult(Packet::ResultOption::Actor_Opt, Packet::ResultCode::InvalidConfigData);
			return true;
		}

		if (upgrade_require->level_ > actor->Level()) {
			player->SendCommonResult(Packet::ResultOption::Actor_Opt, Packet::ResultCode::LevelLimit);
			return true;
		}
		//check item  amount
		for (int32 i = 0; i < 3; i++) {
			auto item = player->FindItemByConfigId(Packet::ContainerType::Layer_MissionAndItemBag,upgrade_require->item_id[i]);
			if (item == nullptr || item->Count() < upgrade_require->item_count[i] ) {
				player->SendCommonResult(Packet::ResultOption::Actor_Opt, Packet::ResultCode::CostItemNotEnough);
				return true;
			}
		}

		//扣除物品, 升颜色
		for (int32 i = 0; i < 3; i++) {
			if (!player->DelItemById(upgrade_require->item_id[i], upgrade_require->item_count[i], Config::ItemDelLogType::DelType_Actor, actor->Guid())){
				player->SendCommonResult(Packet::ResultOption::Actor_Opt, Packet::ResultCode::CostItemNotEnough);
				return true;
			}
		}

		Packet::UpgradeColorReply reply;
		reply.set_actor_guid(actor->Guid());
		reply.mutable_prev()->CopyFrom(actor->DbInfo()->battle_info());
		actor->ChangeColor(message->up_to_color());
		reply.set_now_color(actor->DbInfo()->color());
		reply.mutable_curr()->CopyFrom(actor->DbInfo()->battle_info());
		player->SendMessage(reply);
		player->SendCommonResult(Packet::ResultOption::Actor_Opt, Packet::ResultCode::ResultOK);
		LOG_INFO << "[upgrade color] player:" << player->Guid()
			<< ",actor:" << actor->Guid() << ",color:" << actor->DbInfo()->color()
			<< ",done";
		return true;
	}

	bool MtScene::OnUpgradeStar(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<Packet::UpgradeStarReq>& message, int32 /*source*/)
	{
		auto actor = player->FindActor(message->info().actor_guid());
		Packet::UpgradeStarReply reply;
		reply.mutable_req()->CopyFrom(*message);
		if (actor == nullptr) {
			reply.set_result(Packet::UpgradeStarReply_Result::UpgradeStarReply_Result_INVALID_TARGET);
			player->SendMessage(reply);
			return true;
		}
		LOG_INFO << "[upgrade star] player:" << player->Guid()
			<< ",actor:" << actor->Guid() << ",star:" << actor->DbInfo()->star();
		//目标星级
		auto up_to_star = message->up_to_star();
		if (up_to_star != actor->DbInfo()->star() + 1) {
			reply.set_result(Packet::UpgradeStarReply_Result::UpgradeStarReply_Result_INVALID_TARGET);
			player->SendMessage(reply);
			return true;
		}
		if (up_to_star > Packet::ActorStar_MAX) {
			player->SendCommonResult(Packet::ResultOption::Actor_Opt, Packet::ResultCode::MaxLimit);
			return true;
		}
		//获取配置
		auto upgrade_requrie = MtUpgradeConfigManager::Instance().UpgradeStarRequirement(actor->DbInfo()->actor_config_id(), message->up_to_star());
		if (upgrade_requrie == nullptr) {
			reply.set_result(Packet::UpgradeStarReply_Result::UpgradeStarReply_Result_NO_ENOUGH_ITEM);
			player->SendMessage(reply);
			return true;
		}
		//道具数量检查
		auto star_item = player->FindItemByConfigId(Packet::ContainerType::Layer_ItemBag, upgrade_requrie->star_item_id);
		auto soul_item = player->FindItemByConfigId(Packet::ContainerType::Layer_ItemBag, upgrade_requrie->soul_item_id);
		if (player->GetPlayerToken(Packet::TokenType::Token_Gold) < upgrade_requrie->need_gold) {
			player->SendCommonResult(Packet::ResultOption::Upgrade_Relate, Packet::ResultCode::GoldNotEnough);
			return true;
		}
		if (star_item == nullptr || soul_item == nullptr || star_item->Count() < upgrade_requrie->star_item_amount
			|| soul_item->Count() < upgrade_requrie->soul_item_amount) {
			player->SendCommonResult(Packet::ResultOption::Upgrade_Relate, Packet::ResultCode::CostItemNotEnough);
			return true;
		}

		//消耗道具, 升星
		player->DelItemById(upgrade_requrie->soul_item_id, upgrade_requrie->soul_item_amount, Config::ItemDelLogType::DelType_Upgrade_Star);
		player->DelItemById(upgrade_requrie->star_item_id, upgrade_requrie->star_item_amount, Config::ItemDelLogType::DelType_Upgrade_Star);		
		player->DelItemById(Packet::TokenType::Token_Gold, upgrade_requrie->need_gold, Config::ItemDelLogType::DelType_Upgrade_Star);

		reply.mutable_prev()->CopyFrom(actor->DbInfo()->battle_info());
		actor->ChangeStar(message->up_to_star());
		reply.mutable_curr()->CopyFrom(actor->DbInfo()->battle_info());
		reply.set_result(Packet::UpgradeStarReply_Result::UpgradeStarReply_Result_SUCCESS);
		reply.set_now_star(actor->DbInfo()->star());
		player->SendMessage(reply);
		LOG_INFO << "[upgrade star] player:" << player->Guid()
			<< ",actor:" << actor->Guid() << ",star:" << actor->DbInfo()->star()
			<< ",done";
		return true;
	}

	bool MtScene::OnUpgradeSKill(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<Packet::UpgradeSkillReq>& message, int32 /*source*/)
	{
		auto actor = player->FindActor(message->actor_guid());		
		if (actor == nullptr) { 
			player->SendCommonResult(Packet::ResultOption::Skill_Opt, Packet::ResultCode::NoActor);
			return true;
		}		

		auto skill_max_level = MtSkillManager::Instance().MaxSkillLevel() > actor->Level()
			? actor->Level() : MtSkillManager::Instance().MaxSkillLevel();

		if (message->skill_guid_size() != message->up_to_level_size()) {
			player->SendCommonResult(Packet::ResultOption::Skill_Opt, Packet::ResultCode::Invalid_Request);
			return true;
		}
		auto config = MtActorConfig::Instance().GetBaseConfig(actor->ConfigId());
		if (config == nullptr) {
			player->SendCommonResult(Packet::ResultOption::Skill_Opt, Packet::ResultCode::InvalidConfigData);
			return true;
		}
		auto need_gold = 0;
		for (int32 i = 0; i < message->skill_guid_size(); i++) {
			auto skill = actor->FindDBSkill(message->skill_guid(i));
			if (skill == nullptr) {
				player->SendCommonResult(Packet::ResultOption::Skill_Opt, Packet::ResultCode::InvalidSkill);
				return true;
			}
			if (skill->skill_id() == config->star_skill) {
				player->SendClientNotify("xingjijinengbunengshengji", -1, -1);
				return true;
			}
			if (message->up_to_level(i) > skill_max_level) {
				player->SendClientNotify("skill_lv_max", -1, -1);
				return true;
			}
			auto curr_level = skill->skill_level();
			if (curr_level >= skill_max_level) {
				continue;
			}			
			for (auto k = curr_level; k < message->up_to_level(i); ++k) {
				need_gold += MtUpgradeConfigManager::Instance().UpgradeSkillRequirement(k);
			}
			LOG_INFO << "[skill level up] player:" << player->Guid()
				<< ",actor:" << actor->Guid()
				<< ",skill:" << skill->skill_id()
				<< ",level:" << skill->skill_level()
				<< ",up_to:" << message->up_to_level(i)
				<< ",need_gold" << need_gold;
		}
		if (!player->DelItemById(Packet::TokenType::Token_Gold, need_gold, Config::ItemDelLogType::DelType_Upgrade_Level)) {
			player->SendCommonResult(Packet::ResultOption::Skill_Opt, Packet::ResultCode::GoldNotEnough);
			return true;
		}

		for (int32 i = 0; i < message->skill_guid_size(); i++) {
			auto skill = actor->FindDBSkill(message->skill_guid(i));
			if (skill == nullptr) {
				continue;
			}
			skill->set_skill_level(message->up_to_level(i));
			LOG_INFO << "[skill level up] player:" << player->Guid()
				<< ",actor:" << actor->Guid()
				<< ",skill:" << skill->skill_id()
				<< ",level:" << skill->skill_level();
			player->OnLuaFunCall_x("xOnUpgradeSKill",
			{
				{ "value",message->up_to_level(i) }
			});
		}

		actor->OnBattleInfoChanged();
		actor->RefreshSkillData2Client();

		player->SendCommonResult(Packet::ResultOption::Skill_Opt, Packet::ResultCode::ResultOK);
		return true;
	}

	bool MtScene::OnEnhenceEquip(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<Packet::EnhenceEquipReq>& message, int32 /*source*/)
	{
		player; message;
		//已废弃
		return false;

		//Packet::ResultCode code = Packet::ResultCode::ResultOK;
		//std::set<MtEquipItem*> update_list;

		//for (int32 i = 0; i < message->equip_guid_size(); i++)
		//{
		//	auto item = player->FindItemByGuid(Packet::ContainerType::Layer_EquipBag, message->equip_guid(i));
		//	if (item == nullptr) {
		//		item = player->FindItemByGuid(Packet::ContainerType::Layer_Equip ,message->equip_guid(i)); //穿在身上的装备
		//	}
		//	auto equip = boost::dynamic_pointer_cast<MtEquipItem>(item);
		//	if (equip == nullptr) {
		//		code = Packet::ResultCode::InvalidEquip;
		//		continue;
		//	}

		//	int32 curLevel = equip->EnhenceLevel();
		//	if (curLevel >= MtUpgradeConfigManager::Instance().MaxEnhenceLevel()) {
		//		code = Packet::ResultCode::MaxLimit;
		//		continue;
		//	}
		//	int32 delta(0);
		//	if (message->is_max()) {//强化到最高等级
		//		delta = player->PlayerLevel() - curLevel;
		//	} else {//提升一级
		//		delta = 1;
		//		if (curLevel >= player->PlayerLevel())
		//			delta = 0;
		//	}
		//	if (delta == 0) {
		//		code = Packet::ResultCode::MaxLimit;
		//		continue;
		//	}
		//	int32 total_cost = 0;
		//	int32 enhence_level = 0;
		//	for (int32 k = 0; k < delta; k++) {
		//		int32 cost = MtUpgradeConfigManager::Instance().EquipEnhenceCost(curLevel + k);
		//		if (cost == -1) {
		//			code = Packet::ResultCode::InvalidConfigData;
		//			continue;
		//		}
		//		if (player->GetPlayerToken(Packet::TokenType::Token_Gold) < total_cost + cost) break;
		//		total_cost += cost;
		//		enhence_level = k + 1;
		//	}
		//	if (total_cost > 0 && !player->DelItemById(Packet::TokenType::Token_Gold, total_cost, Config::ItemDelLogType::DelType_EnhanceEquip)) {
		//		code = Packet::ResultCode::GoldNotEnough;
		//		continue;;
		//	}
		//	equip->SetEnhenceLevel(equip->EnhenceLevel() + enhence_level);
		//	update_list.insert(equip.get());
		//	code = Packet::ResultCode::ResultOK;
		//	player->OnLuaFunCall_x("xOnEnhenceEquip",
		//	{
		//		{ "value", equip->EnhenceLevel() },
		//	});
		//}
		//
		//uint64 acotr_guid = INVALID_GUID;
		//Packet::PlayerContainerRefreshItemNotify notify;
		//notify.set_option(Packet::ResultOption::Enhance_Equips);
		//notify.set_container_type(Packet::ContainerType::Layer_Equip);
		//for (auto child : update_list) {				
		//	auto info = notify.add_items();
		//	child->FillMessage(*info);
		//	if (child->ActorGuid() != INVALID_GUID) {
		//		acotr_guid = child->ActorGuid();
		//	}
		//}
		//player->SendMessage(notify);
		////一次强化只可能影响一个玩家
		//if (acotr_guid != INVALID_GUID) {
		//	auto actor_it = std::find_if(std::begin(player->Actors()), std::end(player->Actors()), boost::bind(&MtActor::Guid, _1) == acotr_guid);
		//	if (actor_it != std::end(player->Actors())) {
		//		(*actor_it)->OnBattleInfoChanged();
		//		if ((*actor_it)->MainActor()) {
		//			player->OnLuaFunCall_x("xEnhanceMasterLv",
		//			{
		//				{ "value",(*actor_it)->EnhanceMasterLv() }
		//			});
		//		}
		//	}
		//}
		//player->SendCommonResult(Packet::ResultOption::Enhance_Equips, code);
		//return true;
	}

	bool MtScene::OnAddEnchantSlot(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<Packet::AddEnchantSlotReq>& message, int32 /*source*/)
	{
		auto item = player->FindItemByGuid(Packet::ContainerType::Layer_EquipBag, message->equip_guid());
		if (item == nullptr) {
			item = player->FindItemByGuid(Packet::ContainerType::Layer_Equip ,message->equip_guid());//穿在身上的装备
		}
		if (item == nullptr) {
			player->SendCommonResult(Packet::ResultOption::Equip_EnchantSlot, Packet::ResultCode::InvalidEquip);
			return true;
		}
		auto equip = boost::dynamic_pointer_cast<MtEquipItem>(item);
		if (equip == nullptr) {
			player->SendCommonResult(Packet::ResultOption::Equip_EnchantSlot, Packet::ResultCode::InvalidEquip);
			return true;
		}
		int32 index = message->slot_index();
		auto enchant_info = equip->Enchant(index);
		if (enchant_info == nullptr) {
			player->SendCommonResult(Packet::ResultOption::Equip_EnchantSlot, Packet::ResultCode::InvalidSlot);
			return true;
		}
		if (enchant_info->slot_open()) {
			player->SendCommonResult(Packet::ResultOption::Equip_EnchantSlot, Packet::ResultCode::SlotOpened);
			return true;
		}
		if (index< 0 || index >= 4) {
			player->SendCommonResult(Packet::ResultOption::Equip_EnchantSlot, Packet::ResultCode::InvalidSlot);
			return true;
		}
		//////////////////////////////////////////
		auto crystal_config = MtConfigDataManager::Instance().FindConfigValue("enchant_slot_cost");
		if (crystal_config == nullptr) {
			player->SendCommonResult(Packet::ResultOption::Equip_EnchantSlot, Packet::ResultCode::InvalidConfigData);
			return true;
		}
		std::vector<int32> crystals = { crystal_config->value1(),crystal_config->value2(), crystal_config->value3(), crystal_config->value4() };

		////////////////////////////////////////
		auto token_config = MtConfigDataManager::Instance().FindConfigValue("enchant_slot_token");
		if (token_config == nullptr) {
			player->SendCommonResult(Packet::ResultOption::Equip_EnchantSlot, Packet::ResultCode::InvalidConfigData);
			return true;
		}
		std::vector<int32> tokens = { token_config->value1(),token_config->value2(), token_config->value3(), token_config->value4() };

		/////////////////////////////////////优先扣代币
		if (!player->DelItemById(tokens[index], 1, Config::ItemDelLogType::DelType_Draw)) {
			//再扣钻石
			if (!player->DelItemById(Packet::TokenType::Token_Crystal, crystals[index], Config::ItemDelLogType::DelType_Enchant_Slot)) {
				player->SendCommonResult(Packet::ResultOption::Equip_EnchantSlot, Packet::ResultCode::CrystalNotEnough);
				return true;
			}
		}
		LOG_INFO << "[add enchant slot] player:" << player->Guid()
			<< ",equip:" << equip->Guid()
			<< ",config:" << equip->ConfigId()
			<< ",index:" << index;
		enchant_info->set_slot_open(true);
		equip->RefreshData2Client();
		player->SendCommonResult(Packet::ResultOption::Equip_EnchantSlot, Packet::ResultCode::ResultOK);

		player->OnLuaFunCall_x("xOnEnchantSlot",
		{
			{ "value",-1 }
		});

		return true;
	}

	bool MtScene::OnEnchantEquip(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<Packet::EnchantEquipReq>& message, int32 /*source*/)
	{
		auto item = player->FindItemByGuid(Packet::ContainerType::Layer_EquipBag, message->equip_guid());
		if (item == nullptr) {
			item = player->FindItemByGuid(Packet::ContainerType::Layer_Equip, message->equip_guid());//穿在身上的装备
		}
		auto equip = boost::dynamic_pointer_cast<MtEquipItem>(item);
		if (item == nullptr || equip == nullptr) {
			player->SendCommonResult(Packet::ResultOption::Equip_Enchant, Packet::ResultCode::InvalidEquip);
			return true;
		}
		std::vector<int32> lock_slots;
		std::transform(std::begin(message->lock_slots()), std::end(message->lock_slots()), std::back_inserter(lock_slots), [](auto& e) { return e; });
		if (std::is_sorted(std::begin(lock_slots), std::end(lock_slots)) == false) {
			player->SendCommonResult(Packet::ResultOption::Equip_Enchant, Packet::ResultCode::Invalid_Request);
			return true;
		}
		std::vector<int32> origin_ids{ 0,1,2,3 };
		std::vector<int32> slot_ids;
		std::set_difference(std::begin(origin_ids), std::end(origin_ids), std::begin(lock_slots), std::end(lock_slots), std::back_inserter(slot_ids));
		auto old_enchants = equip->OpenedEnchants(slot_ids);
		auto requrie = MtUpgradeConfigManager::Instance().EquipEnchantRequirement(equip->Config()->level_limit());
		if (requrie == nullptr) {
			player->SendCommonResult(Packet::ResultOption::Equip_Enchant, Packet::ResultCode::Invalid_Request);
			return true;
		}
		auto lock_slot_count = message->lock_slots_size();
		if (lock_slot_count >= 4 || lock_slots.size() >= 4) {
			player->SendCommonResult(Packet::ResultOption::Equip_Enchant, Packet::ResultCode::Invalid_Request);
			return true;
		}
		
		auto need_item1_count = requrie->enchant_item_amount() * int32(equip->OpenEnchantSlotSize() - lock_slots.size());
		if (player->GetBagItemCount(requrie->enchant_item_id()) < need_item1_count) {
			player->SendCommonResult(Packet::ResultOption::Equip_Enchant, Packet::ResultCode::CostItemNotEnough);
			return true;
		}

		auto need_item2_count = 0;
		switch (lock_slot_count)
		{
		case 0:
			need_item2_count = 0;
			break;
		case 1:
			need_item2_count = requrie->lock_item_cost_1();
			break;
		case 2:
			need_item2_count = requrie->lock_item_cost_2();
			break;
		case 3:
			need_item2_count = requrie->lock_item_cost_3();
			break;;
		default:
			player->SendCommonResult(Packet::ResultOption::Equip_Enchant, Packet::ResultCode::Invalid_Request);
			return true;
			break;
		}		
		if (player->GetBagItemCount(requrie->lock_item_id()) < need_item2_count) {
			player->SendCommonResult(Packet::ResultOption::Equip_Enchant, Packet::ResultCode::CostItemNotEnough);
			return true;
		}

		//扣道具
		if (!player->DelItemById(requrie->enchant_item_id(), need_item1_count, Config::ItemDelLogType::DelType_Enchant)) {
			//奇异之尘不够
			player->SendCommonResult(Packet::ResultOption::Equip_Enchant, Packet::ResultCode::CostItemNotEnough);
			return true;
		}
		if (need_item2_count > 0) {
			if (!player->DelItemById(requrie->lock_item_id(), need_item2_count, Config::ItemDelLogType::DelType_Enchant)) {
				//魔法精华不够
				player->SendCommonResult(Packet::ResultOption::Equip_Enchant, Packet::ResultCode::CostItemNotEnough);
				return true;
			}
		}
		for (auto& enchant : (*equip->DbInfo()->mutable_enchants())) {
			if (enchant.slot_open())
				enchant.set_client_lock(false);
		}
		for (auto id : lock_slots) {
			auto& enchant = (*equip->DbInfo()->mutable_enchants(id));
			if (enchant.slot_open()) {
				enchant.set_client_lock(true);
			}
		}
		for (auto enchant : old_enchants) {
			auto random_enchant_result = MtUpgradeConfigManager::Instance().RandomEnchantEffect(equip->OldEnchantsTypes());
			enchant->set_effect_type(std::get<0>(random_enchant_result));
			auto effect_value = (int32)ceill(std::get<1>(random_enchant_result) * requrie->enchant_property_factor() / 100);
			std::vector<double> ranges{ 0,40,80,99,100 };
			std::vector<double> weights{ 100,80,19,1 };
			std::piecewise_constant_distribution<> distri(ranges.begin(), ranges.end(), weights.begin());
			auto rand_result = distri(rand_gen->generator()) + 1;
			auto value = int32(std::round(effect_value * rand_result / 100.f));
			if (value == 0) value = 1;
			enchant->set_effect_value(value);
			enchant->set_max_value(effect_value);
		}
		if (equip->ActorGuid() != INVALID_GUID) {
			auto actor = player->FindActor(equip->ActorGuid());
			if (actor) {
				actor->OnBattleInfoChanged();
			}
		}
		LOG_INFO << "[enchant equip] player:" << player->Guid()
			<< ",equip:" << equip->Guid()
			<< ",config:" << equip->ConfigId()
			<< ",slots:" << slot_ids;
		equip->UpdateEquipScore();
		equip->RefreshData2Client();
		player->OnLuaFunCall_x("xOnEnchantEquip",
		{
			{ "value",1 }
		});
				
		player->SendCommonResult(Packet::ResultOption::Equip_Enchant, Packet::ResultCode::ResultOK);
		return true;
	}


	bool MtScene::OnBatchEnchantEquip(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<Packet::BatchEnchantEquipReq>& message, int32 /*source*/)
	{
		auto item = player->FindItemByGuid(Packet::ContainerType::Layer_EquipBag, message->equip_guid());
		if (item == nullptr) {
			item = player->FindItemByGuid(Packet::ContainerType::Layer_Equip, message->equip_guid());//穿在身上的装备
		}
		Packet::BatchEnchantEquipReply reply;
		reply.mutable_req()->CopyFrom(*message);
		auto  message_defer = Defer([&]() { player->SendMessage(reply); });
		if (item == nullptr) {
			player->SendCommonResult(Packet::ResultOption::Equip_Enchant, Packet::ResultCode::InvalidEquip);
			return true;
		}
		auto equip = boost::dynamic_pointer_cast<MtEquipItem>(item);
		if (equip == nullptr) {
			player->SendCommonResult(Packet::ResultOption::Equip_Enchant, Packet::ResultCode::InvalidEquip);
			return true;
		}
		std::vector<int32> lock_slots;
		std::transform(std::begin(message->lock_slots()), std::end(message->lock_slots()), std::back_inserter(lock_slots), [](auto& e) {return e; });
		if (std::is_sorted(std::begin(lock_slots), std::end(lock_slots)) == false) {			
			player->SendCommonResult(Packet::ResultOption::Equip_Enchant, Packet::ResultCode::InvalidSlot);
			return true;
		}
		for (auto id : lock_slots) {
			if (id < 0 || id > 3) {
				player->SendCommonResult(Packet::ResultOption::Equip_Enchant, Packet::ResultCode::InvalidSlot);
				return true;
			}
		}
		auto require = MtUpgradeConfigManager::Instance().EquipEnchantRequirement(equip->Config()->level_limit());

		const int32 batch_count = 10;
		//Check enchant item cost
		auto enchant_cost_item_amount = require->enchant_item_amount() * (int32)(equip->OpenEnchantSlotSize() - lock_slots.size()) * batch_count;
		if (player->GetBagItemCount(require->enchant_item_id())<enchant_cost_item_amount ) {
			player->SendCommonResult(Packet::ResultOption::Equip_Enchant, Packet::ResultCode::CostItemNotEnough);
			return true;
		}
		//Check lock item cost
		auto lock_item_amount = 0;
		if (lock_slots.size() > 0) {
			switch (lock_slots.size())
			{
			case 1:
				lock_item_amount = require->lock_item_cost_1() * batch_count;
				break;
			case 2:
				lock_item_amount = require->lock_item_cost_2() * batch_count;
				break;
			case 3:
				lock_item_amount = require->lock_item_cost_3() * batch_count;
				break;;
			default:
				player->SendCommonResult(Packet::ResultOption::Equip_Enchant, Packet::ResultCode::Invalid_Request);
				return true;
				break;
			}
		}
		if (player->GetBagItemCount(require->lock_item_id()) < lock_item_amount) {
			player->SendCommonResult(Packet::ResultOption::Equip_Enchant, Packet::ResultCode::CostItemNotEnough);
			return true;
		}

		//扣道具
		if (!player->DelItemById(require->enchant_item_id(), enchant_cost_item_amount, Config::ItemDelLogType::DelType_Enchant)) {
			player->SendCommonResult(Packet::ResultOption::Equip_Enchant, Packet::ResultCode::CostItemNotEnough);
			return true;
		}
		if (lock_item_amount > 0) {
			if (!player->DelItemById(require->lock_item_id(), lock_item_amount, Config::ItemDelLogType::DelType_Enchant)) {
				player->SendCommonResult(Packet::ResultOption::Equip_Enchant, Packet::ResultCode::CostItemNotEnough);
				return true;
			}
		}
		for (auto& enchant : (*equip->DbInfo()->mutable_enchants())) {
			if (enchant.slot_open())
				enchant.set_client_lock(false);
		}
		for (auto id : lock_slots) {
			auto& enchant = (*equip->DbInfo()->mutable_enchants(id));
			if (enchant.slot_open()) {
				enchant.set_client_lock(true);
			}
		}
		auto all_enchants = equip->OpenedEnchants({ 0,1,2,3 });
		if (all_enchants.size() <= lock_slots.size()) {
			player->SendCommonResult(Packet::ResultOption::Equip_Enchant, Packet::ResultCode::InvalidSlot);
			return true;
		}
		for (auto i = 0; i <  batch_count; ++i) {
			auto& preview = *reply.add_previews();
			for (auto j = 0u; j < all_enchants.size(); ++j) {
				auto& ele = *preview.add_elements();
				if (std::any_of(lock_slots.begin(), lock_slots.end(), [=](int32 slot) {return slot == int32(j); })) {
					//锁住的槽位
					auto old_enchat = all_enchants[j];
					ele.set_effect_type(old_enchat->effect_type());
					ele.set_effect_value(old_enchat->effect_value());
					ele.set_max_value(old_enchat->max_value());
				} else {
					std::vector<std::string> exclude_list;
					std::transform(preview.elements().begin(),
						preview.elements().end(), std::back_inserter(exclude_list),
						boost::bind(&Packet::EnchantPreviewEle::effect_type, _1));
					auto random_res = MtUpgradeConfigManager::Instance().RandomEnchantEffect(exclude_list);
					auto effect_value = (int32)ceill(std::get<1>(random_res) * require->enchant_property_factor() / 100);
					std::vector<double> ranges{ 0,40,80,99,100 };
					std::vector<double> weights{ 100,80,19,1 };
					std::piecewise_constant_distribution<> distri(ranges.begin(), ranges.end(), weights.begin());
					auto rand_result = distri(rand_gen->generator()) + 1;
					auto value = int32(std::round(effect_value * rand_result / 100.f));
					if (value == 0) value = 1;
					ele.set_effect_type(std::get<0>(random_res));
					ele.set_effect_value(value);
					ele.set_max_value(effect_value);
				}
			}
		}
		LOG_INFO << "[batch enchant equip] player:" << player->Guid()
			<< ",equip:" << equip->Guid()
			<< ",config:" << equip->ConfigId()
			<< ",slots:" << lock_slots;
		equip->CacheBatchEnchantInfo(reply);		

		player->OnLuaFunCall_x("xOnEnchantEquip",
		{
			{ "value",10 }
		});

		player->SendCommonResult(Packet::ResultOption::Equip_Enchant, Packet::ResultCode::ResultOK);
		return true;
	}

	bool MtScene::OnBatchEnchantEquipConfirm(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<Packet::BatchEnchantEquipConfirmReq>& message, int32 /*source*/)
	{
		const int32 batch_count = 10;
		auto item = player->FindItemByGuid(Packet::ContainerType::Layer_EquipBag, message->equip_guid());
		if (item == nullptr) {
			item = player->FindItemByGuid(Packet::ContainerType::Layer_Equip, message->equip_guid());//穿在身上的装备
		}

		if (item == nullptr) {
			player->SendCommonResult(Packet::ResultOption::Equip_Enchant, Packet::ResultCode::InvalidItem);
			return true;
		}
		auto equip = boost::dynamic_pointer_cast<MtEquipItem>(item);
		if (equip == nullptr) {
			player->SendCommonResult(Packet::ResultOption::Equip_Enchant, Packet::ResultCode::InvalidEquip);
			return true;
		}
		if (equip->HasBatchEnchantCache() == false) {
			player->SendCommonResult(Packet::ResultOption::Equip_Enchant, Packet::ResultCode::BatchEnchantFirst);
			return true;
		}
		if (message->confirm_index() < 0 || message->confirm_index() >= batch_count) {
			player->SendCommonResult(Packet::ResultOption::Equip_Enchant, Packet::ResultCode::InvalidIndex);
			return true;
		}
		LOG_INFO << "[batch enchant equip confirm] player:" << player->Guid()
			<< ",equip:" << equip->Guid()
			<< ",config:" << equip->ConfigId()
			<< ",confirm:" << message->confirm_index();
		equip->ApplyBatchEnchantCache(message->confirm_index());
		equip->RefreshData2Client();
		Packet::BatchEnchantEquipConfirmReply reply;
		reply.mutable_req()->CopyFrom(*message);
		reply.set_result(Packet::BatchEnchantEquipConfirmReply_Result::BatchEnchantEquipConfirmReply_Result_SUCCESS);
		player->SendMessage(reply);
		player->SendCommonResult(Packet::ResultOption::Equip_Enchant, Packet::ResultCode::ResultOK);
		return true;
	}

	bool MtScene::OnTakeOutGem(const boost::shared_ptr<MtPlayer>& player,
		const boost::shared_ptr<Packet::TakeOutGemReq>& msg, int32 /*source*/)
	{
		if (msg->equip_guid_size() == 0
			|| msg->equip_guid_size() != msg->gem_config_id_size()
			|| msg->gem_config_id_size() != msg->index_size()) {
			player->SendCommonResult(Packet::ResultOption::Equip_Inset, Packet::ResultCode::Invalid_Request);
			return true;
		}
		if (!player->BagLeftCapacity(Packet::ContainerType::Layer_ItemBag, msg->gem_config_id_size())) {
			player->SendClientNotify("ItemBagIsFull", -1, -1);
			return true;
		}
		std::set<uint64> changed_actor_guids;
		for (auto i = 0; i < msg->equip_guid_size(); ++i) {
			auto base_equip = player->FindItemByGuid(Packet::ContainerType::Layer_EquipBag, msg->equip_guid(i));
			if (base_equip == nullptr) {
				base_equip = player->FindItemByGuid(Packet::ContainerType::Layer_Equip, msg->equip_guid(i));//穿在身上的装备
			}
			auto equip = boost::dynamic_pointer_cast<MtEquipItem>(base_equip);
			if (equip == nullptr || equip->HasGem(msg->gem_config_id(i), msg->index(i)) == false) {
				player->SendCommonResult(Packet::ResultOption::Equip_Inset, Packet::ResultCode::InvalidEquip);
				return true;
			}
		}
		for (auto i = 0; i < msg->equip_guid_size(); ++i) {
			auto base_equip = player->FindItemByGuid(Packet::ContainerType::Layer_EquipBag, msg->equip_guid(i));
			if (base_equip == nullptr) {
				base_equip = player->FindItemByGuid(Packet::ContainerType::Layer_Equip, msg->equip_guid(i));//穿在身上的装备
			}
			auto equip = boost::dynamic_pointer_cast<MtEquipItem>(base_equip);
			LOG_INFO << "[take out gem] player:" << player->Guid()
				<< ",equip:" << equip->Guid()
				<< ",config:" << equip->ConfigId()
				<< ",gem:" << msg->gem_config_id(i)
				<< " ,index:" << msg->index(i);
			if (equip->StripGem(msg->gem_config_id(i), msg->index(i))) {
				player->AddItemById(msg->gem_config_id(i), 1, Config::ItemAddLogType::AddType_TakeOutGem);
			}
			equip->RefreshData2Client();
			if (equip->ActorGuid() != INVALID_GUID) {
				changed_actor_guids.insert(equip->ActorGuid());
			}
		}
		for (auto guid : changed_actor_guids) {
			auto actor = player->FindActor(guid);
			actor->OnBattleInfoChanged();
		}
		player->SendCommonResult(Packet::ResultOption::Equip_Inset, Packet::ResultCode::ResultOK);
		return true;
	}

	bool MtScene::OnInsetGem2Equip(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<Packet::InsetEquipReq>& message, int32 /*source*/)
	{
		if (message->equip_guid_size() == 0 
			|| message->equip_guid_size() != message->gem_config_id_size()
			|| message->gem_config_id_size() != message->index_size()){
			player->SendCommonResult(Packet::ResultOption::Equip_Inset, Packet::ResultCode::Invalid_Request);
			return true;
		}
		std::set<uint64> changed_actor_guids;
		for (auto i = 0; i < message->equip_guid_size(); ++i) {
			auto base_equip = player->FindItemByGuid(Packet::ContainerType::Layer_EquipBag, message->equip_guid(i));
			if (base_equip == nullptr) {
				base_equip = player->FindItemByGuid(Packet::ContainerType::Layer_Equip, message->equip_guid(i));//穿在身上的装备
			}
			auto base_gem = player->FindItemByConfigId(Packet::ContainerType::Layer_ItemBag, message->gem_config_id(i));
			auto equip = boost::dynamic_pointer_cast<MtEquipItem>(base_equip);
			auto gem = boost::dynamic_pointer_cast<MtGemItem>(base_gem);
			if (equip == nullptr) {
				player->SendCommonResult(Packet::ResultOption::Equip_Inset, Packet::ResultCode::InvalidEquip);
				return true;
			}
			if (gem == nullptr) {
				player->SendCommonResult(Packet::ResultOption::Equip_Inset, Packet::ResultCode::InvalidGem);
				return true;
			}
			auto support_locs = gem->Config()->inset_locations();
			if (std::find_if(std::begin(support_locs), std::end(support_locs),
				[&](auto& loc) {return loc == equip->Config()->type(); }) == std::end(support_locs)) {
				player->SendCommonResult(Packet::ResultOption::Equip_Inset, Packet::ResultCode::InvalidEquip);
				return true;
			}
			if (!equip->CanInsetGem(message->index(i))) {
				player->SendCommonResult(Packet::ResultOption::Equip_Inset, Packet::ResultCode::InvalidEquip);
				return true;
			}
		}
		for (auto i = 0; i < message->equip_guid_size(); ++i) {
			auto base_equip = player->FindItemByGuid(Packet::ContainerType::Layer_EquipBag, message->equip_guid(i));
			if (base_equip == nullptr) {
				base_equip = player->FindItemByGuid(Packet::ContainerType::Layer_Equip, message->equip_guid(i));//穿在身上的装备
			}
			auto equip = boost::dynamic_pointer_cast<MtEquipItem>(base_equip);
			if (equip->CanInsetGem(message->index(i)) == false) {
				player->SendCommonResult(Packet::ResultOption::Equip_Inset, Packet::ResultCode::InvalidEquip);
				continue;
			}
			LOG_INFO << "[inset gem] player:" << player->Guid()
				<< ",equip:" << equip->Guid()
				<< ",config:" << equip->ConfigId()
				<< ",gem:" << message->gem_config_id(i);
			if (!player->DelItemById(message->gem_config_id(i), 1, Config::ItemDelLogType::DelType_EmbedGem)) {
				player->SendCommonResult(Packet::ResultOption::Equip_Inset, Packet::ResultCode::GemNotEnough);
				return true;
			}
			equip->InsetGem(message->gem_config_id(i), message->index(i));
			LOG_INFO << "[inset gem] player:" << player->Guid()
				<< ",equip:" << equip->Guid()
				<< ",config:" << equip->ConfigId()
				<< ",gem:" << message->gem_config_id(i)
				<< ",index:" << message->index(i);
			if (equip->ActorGuid() != INVALID_GUID) {
				changed_actor_guids.insert(equip->ActorGuid());
	
			}
			equip->RefreshData2Client();
			player->OnLuaFunCall_x("xOnInsetEquip",
			{
				{ "value",0 },{ "gem_id", message->gem_config_id(i)}
			});
		}
		for (auto guid : changed_actor_guids) {
			auto actor = player->FindActor(guid);
			if (actor) {
				actor->OnBattleInfoChanged();
			}
		}
		player->SendCommonResult(Packet::ResultOption::Equip_Inset, Packet::ResultCode::ResultOK);
		return true;
	}

	bool MtScene::OnEquipInherit(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<Packet::EquipInheritReq>& message, int32 /*source*/)
	{
		auto old_item = player->FindItemByGuid(Packet::ContainerType::Layer_EquipBag ,message->old_equip_guid());
		auto new_item = player->FindItemByGuid(Packet::ContainerType::Layer_EquipBag ,message->new_equip_guid());
		if (old_item == nullptr) {
			old_item = player->FindItemByGuid(Packet::ContainerType::Layer_Equip, message->old_equip_guid());
		}
		if (new_item == nullptr) {
			new_item = player->FindItemByGuid(Packet::ContainerType::Layer_Equip, message->new_equip_guid());
		}
		auto old_equip_item = boost::dynamic_pointer_cast<MtEquipItem>(old_item);
		auto new_equip_item = boost::dynamic_pointer_cast<MtEquipItem>(new_item);
		if (message->old_equip_guid() == message->new_equip_guid()) {
			player->SendCommonResult(Packet::ResultOption::Equip_Inherit, Packet::ResultCode::InvalidEquip);
			return true;
		}
		
		if (new_item == nullptr || old_item == nullptr || old_equip_item == nullptr || new_equip_item == nullptr) {
			player->SendCommonResult(Packet::ResultOption::Equip_Inherit, Packet::ResultCode::InvalidEquip);
			return true;
		}
		LOG_INFO << "[equip_inherit] player:" << player->Guid() << ","
			<< old_equip_item->Guid() << " -> " << new_equip_item->Guid();
		//transfer

		if (new_equip_item->InheritFrom(*old_equip_item, *player) == false)
		{
			LOG_INFO << "[equip_inherit] player:" << player->Guid() << ","
				<< old_equip_item->Guid() << " -> " << new_equip_item->Guid() << " fail";
		}
		auto old_equip_actor = player->FindActor(old_equip_item->ActorGuid());
		auto new_equip_actor = player->FindActor(new_equip_item->ActorGuid());
		if (old_equip_actor) {
			old_equip_actor->OnBattleInfoChanged();
		}
		if (new_equip_actor) {
			new_equip_actor->OnBattleInfoChanged();
		}
		player->SendClientNotify("op_notify_001", -1, -1);
		player->SendCommonResult(Packet::ResultOption::Equip_Inherit, Packet::ResultCode::ResultOK);
		return true;
	}
	bool MtScene::OnGemFusionReq(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<Packet::GemFusionReq>& message, int32 /*source*/)
	{
		player, message;
		/*auto item = player->FindItemByGuid(Packet::ContainerType::Layer_ItemBag, message->gem_guid());
		auto gem = boost::dynamic_pointer_cast<MtGemItem>(item);
		if (gem == nullptr) {
			player->SendCommonResult(Packet::ResultOption::Gem_Compose, Packet::ResultCode::InvalidGem);
			return true;
		}
		if (item->Count() < 3) {
			player->SendCommonResult(Packet::ResultOption::Gem_Compose, Packet::ResultCode::GemNotEnough);
			return true;
		}

		int32 new_gem_count = 1;
		if (message->fusion_all()) {
			new_gem_count = item->Count() / 3;
		}

		if (!player->BagLeftCapacity(Packet::ContainerType::Layer_ItemBag, 1)) {
			player->SendClientNotify("ItemBagIsFull", -1, -1);
			return true;
		}

		//钻石开销
		if (message->cannot_fail()) {
			auto cost = MtItemManager::Instance().GetGemCostConfig(gem->Config()->level());
			if (cost == nullptr) {
				player->SendCommonResult(Packet::ResultOption::Gem_Compose, Packet::ResultCode::InvalidConfigData);
				return true;
			}

			if (!player->DelItemById(cost->cost_item(), cost->cost_count()*new_gem_count, Config::ItemDelLogType::DelType_Gem)) {
				player->SendCommonResult(Packet::ResultOption::Gem_Compose, Packet::ResultCode::CrystalNotEnough);
				return true;
			}
		}

		if (!player->DelItemById(gem->ConfigId(), 3 * new_gem_count, Config::ItemDelLogType::DelType_Gem)) {
			player->SendCommonResult(Packet::ResultOption::Gem_Compose, Packet::ResultCode::GemNotEnough);
			return true;
		}

		auto new_gem_id = gem->Config()->fusion_id();
		if (!player->AddItemByIdWithNotify(new_gem_id, new_gem_count, Config::ItemAddLogType::AddType_Gem, gem->ConfigId())) {
			player->SendClientNotify("ItemBagIsFull", -1, -1);
			return true;
		}

		player->SendCommonResult(Packet::ResultOption::Gem_Compose, Packet::ResultCode::ResultOK);*/
		return true;
		
	}

	bool MtScene::OnDismantleEquipReq(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<Packet::DismantleEquipReq>& message, int32 /*source*/)
	{
		if(player == nullptr )
			return true;

		//10级以前不让分解东西
		if (player->PlayerLevel() < 10) {
			player->SendCommonResult(Packet::ResultOption::Dismantle_Equip, Packet::ResultCode::CantSellItem);
			return true;
		}
		/** 修改了一下
			1 先验证数据 计算分解后所得.
			2 再删除道具.
			3 最后, 按第一部结算结果添加东西
			[11/20/2017 Ryan]
		*/
		Packet::NotifyItemList notify_list;
		std::vector<uint64> delete_items;
		delete_items.reserve(message->equip_guids_size());
		std::map<int32, int32> add_items; //分解后需要增加的<道具id,数量>
		for (auto guid : message->equip_guids()) {
			auto item = player->FindItemByGuid(Packet::ContainerType::Layer_EquipAndItemBag, guid);
			if (item == nullptr || item->IsLock()) {
				player->SendCommonResult(Packet::ResultOption::Dismantle_Equip, Packet::ResultCode::InvalidItem);
				continue;
			}
			auto normal_item = boost::dynamic_pointer_cast<MtNormalItem>(item);
			if (normal_item != nullptr) {
				auto config = normal_item->Config();
				if (config == nullptr || config->can_sale() <= 0 ) {
					player->SendCommonResult(Packet::ResultOption::Dismantle_Equip, Packet::ResultCode::InvalidItem);
					continue;
				}
				add_items[config->can_sale()]+= normal_item->Count() * config->coin();
			} else {
				auto equip = boost::dynamic_pointer_cast<MtEquipItem>(item);
				if (equip == nullptr) {
					continue;
				}
				auto dismantle_config = MtUpgradeConfigManager::Instance().DismantleData(equip->Config());
				if (dismantle_config == nullptr) {
					continue;
				}
				for (auto gem_id : equip->DbInfo()->gems()) {
					if (gem_id > 0)
						add_items[gem_id] += 1;
				}
				for (auto& dismantle_info : dismantle_config->dismantle_items()) {
					add_items[dismantle_info.item_id()] += dismantle_info.item_amount();
				}
			}
			delete_items.push_back(item->Guid());
		}
		if (!player->EquipAndItemBagLeftCheck(add_items)) {
			player->SendClientNotify("ItemBagIsFull", -1, -1);
			return true;
		}
		std::vector<boost::shared_ptr<MtItemBase>> items;
		for (auto item_guid : delete_items) {
			auto item = player->FindItemByGuid(Packet::ContainerType::Layer_EquipAndItemBag, item_guid);
			if (item != nullptr) {
				items.push_back(item);
			}
		}
		if (!player->DelItems(items, Config::ItemDelLogType::DelType_DismantleItem, 0)) {
			player->SendCommonResult(Packet::ResultOption::Dismantle_Equip, Packet::ResultCode::InvalidItem);
			return true;
		}
		for (auto& add_item : add_items) {
			player->AddItemById(add_item.first, add_item.second, Config::ItemAddLogType::AddType_DismantleItem, &notify_list);
		}
		if (notify_list.item_size() > 0) {
			player->SendMessage(notify_list);
		}
		player->SendCommonResult(Packet::ResultOption::Dismantle_Equip, Packet::ResultCode::ResultOK);
		return true;
	}

	bool MtScene::OnEquipMagicalReq(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<Packet::EquipMagicalReq>& message, int32 /*source*/)
	{
		auto src_item = player->FindItemByGuid(Packet::ContainerType::Layer_Equip ,message->src_guid());
		auto des_item = player->FindItemByGuid(Packet::ContainerType::Layer_EquipBag ,message->des_guid());
		if (src_item == nullptr) {
			src_item = player->FindItemByGuid(Packet::ContainerType::Layer_Equip, message->src_guid());
		}
		if (des_item == nullptr) {
			des_item = player->FindItemByGuid(Packet::ContainerType::Layer_Equip, message->des_guid());
		}
		auto src_equip = boost::dynamic_pointer_cast<MtEquipItem>(src_item);
		auto des_equip = boost::dynamic_pointer_cast<MtEquipItem>(des_item);

		if (des_item == nullptr || src_item == nullptr || src_equip == nullptr || des_equip == nullptr) {
			player->SendCommonResult(Packet::ResultOption::Magical_Equip, Packet::ResultCode::InvalidEquip);
			return true;
		}

		//目标装备 条件检查
		auto actor = player->FindActor(src_equip->ActorGuid());
		if (!actor) {
			player->SendCommonResult(Packet::ResultOption::Magical_Equip, Packet::ResultCode::EquipNotOnBody);
			return true;
		}
		
		if (des_equip->Config()->level_limit() > 0) {
			if ( actor->Level() < des_equip->Config()->level_limit()) {
				player->SendCommonResult(Packet::ResultOption::Magical_Equip, Packet::ResultCode::LevelLimit);
				return true;
			}
		}

		if (!des_equip->CheckRace(actor->Race())) {
			player->SendCommonResult(Packet::ResultOption::Magical_Equip, Packet::ResultCode::InvalidRace);
			return true;
		}
		if (!des_equip->CheckProf(actor->Professions())) {
			player->SendCommonResult(Packet::ResultOption::Magical_Equip, Packet::ResultCode::InvalidProfession);
			return true;
		}
		if (!des_equip->CheckTalent(actor->Talent())) {
			player->SendCommonResult(Packet::ResultOption::Magical_Equip, Packet::ResultCode::InvalidTalent);
			return true;
		}
		//
		if (des_equip->Config()->type() != src_equip->Config()->type() && des_equip->Config()->weapon_type() != src_equip->Config()->weapon_type())
		{
			player->SendCommonResult(Packet::ResultOption::Magical_Equip, Packet::ResultCode::InvalidEquipType);
			return true;
		}

		//消耗道具检查
		auto & data = MtConfigDataManager::Instance().FindConfigValue("equip_magical_cost");
		if (data == nullptr) {
			player->SendCommonResult(Packet::ResultOption::Magical_Equip, Packet::ResultCode::InvalidConfigData);
			return true;
		}

		if (!player->DelItemById(data->value1(), data->value2(), Config::ItemDelLogType::DelType_MagicalEquip)) {
			player->SendCommonResult(Packet::ResultOption::Magical_Equip, Packet::ResultCode::CostItemNotEnough);
			return true;
		}

		int32 MagicalResId = des_equip->GetMagicalResId();
		if (!player->DelItems({ des_equip }, Config::ItemDelLogType::DelType_MagicalEquip)) {
			player->SendCommonResult(Packet::ResultOption::Magical_Equip, Packet::ResultCode::RemoveItemFailed);
			return true;
		}
		else{
			src_equip->MagicalFrom(MagicalResId);
			player->OnLuaFunCall_x("xEquipMagic",
			{
				{ "value",1 }
			});
			
			player->SendCommonResult(Packet::ResultOption::Magical_Equip, Packet::ResultCode::ResultOK);
			return true;
		}		
	}

	//移动物品
	bool MtScene::OnMoveItemReq(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<Packet::MoveItemRequest>& message, int32 /*source*/)
	{
		auto dest_type = message->des_container();
		if (dest_type != Packet::ContainerType::Layer_ItemBag 
			&&  dest_type != Packet::ContainerType::Layer_EquipBag 
			&& dest_type != Packet::ContainerType::Layer_Bank) {
			player->SendCommonResult(Packet::ResultOption::Bank_Option, Packet::ResultCode::InvalidContainerType);
			return true;
		}

		auto src_type = message->src_container();
		if (src_type != Packet::ContainerType::Layer_ItemBag 
			&&  src_type != Packet::ContainerType::Layer_EquipBag
			&& src_type != Packet::ContainerType::Layer_Bank) {
			player->SendCommonResult(Packet::ResultOption::Bank_Option, Packet::ResultCode::InvalidContainerType);
			return true;
		}

		auto container = player->FindContainer(src_type);
		if (container == nullptr) {
			player->SendCommonResult(Packet::ResultOption::Bank_Option, Packet::ResultCode::InvalidContainerType);
			return true;
		}

		auto item = container->FindItemByGuid(message->item_guid());
		if (item == nullptr) {
			player->SendCommonResult(Packet::ResultOption::Bank_Option, Packet::ResultCode::InvalidItem);
			return true;
		}

		if (!player->MoveItem(item, dest_type)) {
			player->SendCommonResult(Packet::ResultOption::Bank_Option, Packet::ResultCode::SpaceIsFull);
			return true;
		}

		player->SendCommonResult(Packet::ResultOption::Bank_Option, Packet::ResultCode::ResultOK);
		return true;
	}

	//扩容
	bool MtScene::OnExtendContainerReq(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<Packet::ExtendContainerRequest>& message, int32 /*source*/)
	{
		auto src_type = message->src_container();
		if (src_type != Packet::ContainerType::Layer_ItemBag
			&& src_type != Packet::ContainerType::Layer_EquipBag
			&& src_type != Packet::ContainerType::Layer_Bank) {
			player->SendCommonResult(Packet::ResultOption::Bank_Option, Packet::ResultCode::InvalidContainerType);
			return true;
		}

		auto container = player->FindContainer(src_type);
		if (container == nullptr) {
			player->SendCommonResult(Packet::ResultOption::Bank_Option, Packet::ResultCode::InvalidContainerType);
			return true;
		}

		//消耗检查
		auto const_config = MtConfigDataManager::Instance().FindConfigValue("extend_block_cost");				
		auto cell_container = player->DataCellContainer();
		if (cell_container) {
			if (src_type == Packet::ContainerType::Layer_EquipBag) {
				int32 times = cell_container->get_data_32(Packet::CellLogicType::NumberCell, Packet::NumberCellIndex::CrystalExtendEquipBagTimes);
				int32 cost = const_config->value1();
				if (!container->ExtendCheck(5, 0)) {
					player->SendCommonResult(Packet::ResultOption::Bank_Option, Packet::ResultCode::CountLimit);
					return true;
				}
				if (!player->DelItemById(Packet::TokenType::Token_Gold, cost*(times+1), Config::ItemDelLogType::DelType_ExtendContainerSize)) {
					player->SendCommonResult(Packet::ResultOption::Bank_Option, Packet::ResultCode::GoldNotEnough);
					return true;
				}
				if (!container->ExtendSize(5, 0)) {
					player->SendCommonResult(Packet::ResultOption::Bank_Option, Packet::ResultCode::CountLimit);
					return true;
				}
				cell_container->add_data_32(Packet::CellLogicType::NumberCell, Packet::NumberCellIndex::CrystalExtendEquipBagTimes, 1, true);
			}
			else if (src_type == Packet::ContainerType::Layer_ItemBag) {
				int32 times = cell_container->get_data_32(Packet::CellLogicType::NumberCell, Packet::NumberCellIndex::CrystalExtendItemBagTimes);
				int32 cost = const_config->value2();
				if (!container->ExtendCheck(5, 0)) {
					player->SendCommonResult(Packet::ResultOption::Bank_Option, Packet::ResultCode::CountLimit);
					return true;
				}
				if (!player->DelItemById(Packet::TokenType::Token_Gold, cost*(times + 1), Config::ItemDelLogType::DelType_ExtendContainerSize)) {
					player->SendCommonResult(Packet::ResultOption::Bank_Option, Packet::ResultCode::GoldNotEnough);
					return true;
				}
				if (!container->ExtendSize(5, 0)) {
					player->SendCommonResult(Packet::ResultOption::Bank_Option, Packet::ResultCode::CountLimit);
					return true;
				}
				cell_container->add_data_32(Packet::CellLogicType::NumberCell, Packet::NumberCellIndex::CrystalExtendItemBagTimes, 1, true);
			}
			else if (src_type == Packet::ContainerType::Layer_Bank) {
				int32 times = cell_container->get_data_32(Packet::CellLogicType::NumberCell, Packet::NumberCellIndex::CrystalExtendBankTimes);
				int32 cost = const_config->value3();
				if (!container->ExtendCheck(5, 0)) {
					player->SendCommonResult(Packet::ResultOption::Bank_Option, Packet::ResultCode::CountLimit);
					return true;
				}
				if (!player->DelItemById(Packet::TokenType::Token_Gold, cost*(times+1), Config::ItemDelLogType::DelType_ExtendContainerSize)) {
					player->SendCommonResult(Packet::ResultOption::Bank_Option, Packet::ResultCode::GoldNotEnough);
					return true;
				}
				if (!container->ExtendSize(5, 0)) {
					player->SendCommonResult(Packet::ResultOption::Bank_Option, Packet::ResultCode::CountLimit);
					return true;
				}
				cell_container->add_data_32(Packet::CellLogicType::NumberCell, Packet::NumberCellIndex::CrystalExtendBankTimes, 1, true);
			}
			else {
				player->SendCommonResult(Packet::ResultOption::Bank_Option, Packet::ResultCode::InvalidContainerType);
				return true;
			}
		}

		player->SendCommonResult(Packet::ResultOption::Bank_Option, Packet::ResultCode::ResultOK);
		return true;
	}

	bool MtScene::OnHeroFragmentFusion(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<Packet::FragmentToHeroReq>& message, int32 /*source*/)
	{
		auto item = player->FindItemByGuid(Packet::ContainerType::Layer_ItemBag ,message->item_guid());
		Packet::FragmentToHeroReply reply;
		reply.mutable_req()->CopyFrom(*message);
		auto  message_defer = Defer([&]() { player->SendMessage(reply); });
		if (item == nullptr) {
			reply.set_result(Packet::FragmentToHeroReply_Result::FragmentToHeroReply_Result_NOT_ENOUGH_FRAGMENT);
			return true;
		}
		auto config = MtUpgradeConfigManager::Instance().FragmentConfig(item->ConfigId());
		if (config == nullptr) {
			reply.set_result(Packet::FragmentToHeroReply_Result::FragmentToHeroReply_Result_NOT_ENOUGH_FRAGMENT);
			return true;
		}

		//检查英雄是否已存在
		auto actors = player->Actors();
		auto actor_it = std::find_if(std::begin(actors), std::end(actors), boost::bind(&MtActor::ConfigId, _1) == config->hero_id());
		if (actor_it != std::end(actors)) {
			reply.set_result(Packet::FragmentToHeroReply_Result::FragmentToHeroReply_Result_ALREADY_OWN_TARGET_HERO);
			return true;
		}

		//检查道具数量
		if(!player->DelItemById(item->ConfigId(), config->fragment_amount(),Config::ItemDelLogType::DelType_UseItem)){
			reply.set_result(Packet::FragmentToHeroReply_Result::FragmentToHeroReply_Result_NOT_ENOUGH_FRAGMENT);
			return true;
		}
		
		//添加英雄
		auto ret = player->AddHero(config->hero_id(), Packet::ActorType::Hero, "fusion",true, config->star(), config->color());
		if (ret == false) {
			ZXERO_ASSERT(false) << "add hero failed! hero id =" << config->hero_id() << " guid=" << player->Guid();
		}

		reply.set_result(Packet::FragmentToHeroReply_Result::FragmentToHeroReply_Result_SUCCESS);
		return true;
	}


}

