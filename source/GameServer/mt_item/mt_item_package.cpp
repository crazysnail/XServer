#include "mt_item_package.h"
#include "module.h"
#include "../data/data_manager.h"
#include "../mt_player/mt_player.h"
#include "../mt_actor/mt_actor.h"
#include "../mt_guid/mt_guid.h"
#include "../mt_guild/mt_guild_manager.h"
#include "../mt_item/mt_item.h"
#include "../mt_mission/mt_arrange_proxy.h"

#include <BattleExpression.pb.h>
#include "ItemConfig.pb.h"
#include <PackageConfig.pb.h>
#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>

namespace Mt
{

	void MtItemPackageManager::lua_reg(lua_State* ls)
	{
		ff::fflua_register_t<MtItemPackageManager, ctor()>(ls, "MtItemPackageManager")
			.def(&MtItemPackageManager::GetPackage, "GetPackage")
			.def(&MtItemPackageManager::GetDropGroup, "GetDropGroup")
			.def(&MtItemPackageManager::LoadPackageTB, "LoadPackageTB")
			.def(&MtItemPackageManager::GenDropFromGroup, "GenDropFromGroup")
			;
		
		ff::fflua_register_t<>(ls)
			.def(&MtItemPackageManager::Instance, "LuaMtItemPackageManager");
	}


	static MtItemPackageManager* __mt_item_package_manager = nullptr;
	REGISTER_MODULE(MtItemPackageManager)
	{
		require("data_manager");
		register_load_function(module_base::STAGE_LOAD, boost::bind(&MtItemPackageManager::OnLoad, __mt_item_package_manager));
	}

	void MtItemPackageManager::LoadPackageTB(bool reload) {
		configs_.clear();
		data_table* table = nullptr;
		if (reload) {
			table = data_manager::instance()->on_reload_file("package");
		}
		else {
			table = data_manager::instance()->get_table("package");
		}
		for (auto i = 0; i < table->get_rows(); ++i)
		{
			auto config_info = boost::make_shared<Config::ItemPackageConfig>();
			auto row = table->find_row(i);
			FillMessageByRow(*config_info, *row, "", { { "item_id",[](int32 value) {return value > 0; } } });
			configs_.insert({ config_info->id(), boost::make_shared<MtItemPackage>(config_info) });
			configs_[config_info->id()]->CalcGroupWeight();
		}

		//
		drop_configs_.clear();
		if (reload) {
			table = data_manager::instance()->on_reload_file("drop_group");
		}
		else {
			table = data_manager::instance()->get_table("drop_group");
		}
		for (auto i = 0; i < table->get_rows(); ++i) {
			auto config_info = boost::make_shared<Config::ItemDropGroup>();
			auto row = table->find_row(i);
			FillMessageByRow(*config_info, *row);

			if (drop_configs_.find(config_info->group()) == drop_configs_.end()) {
				auto group = boost::make_shared<MtItemDropGroup>();
				group->items.push_back(config_info);
				group->all_weight += config_info->item().item_weight();
				drop_configs_.insert({ config_info->group(),group });
			}
			else {
				drop_configs_[config_info->group()]->items.push_back(config_info);
				drop_configs_[config_info->group()]->all_weight += config_info->item().item_weight();
			}
		}

	}

	zxero::int32 MtItemPackageManager::OnLoad()
	{
		LoadPackageTB();		
		return 0;
	}

	const Config::ItemPackageElement* MtItemPackageManager::GenDropFromGroup(int32 id)
	{
		auto drop_config = GetDropGroup(id);
		if (drop_config == nullptr) {
			return nullptr;
		}

		int32 seed = rand_gen->intgen(1, drop_config->all_weight);
		int32 base_weight = 0;
		for (uint32 k = 0; k < drop_config->items.size(); k++) {
			auto child = drop_config->items[k]->item();
			base_weight += child.item_weight();
			if ( seed <= base_weight) {
				return drop_config->items[k]->mutable_item();
				break;
			}
		}

		return nullptr;
	}

	boost::shared_ptr<MtItemPackage> MtItemPackageManager::GetPackage(int32 id)
	{
		if (configs_.find(id) == std::end(configs_))
			return nullptr;
		return configs_[id];
	}

	boost::shared_ptr<MtItemDropGroup> MtItemPackageManager::GetDropGroup(int32 id)
	{
		if (drop_configs_.find(id) == std::end(drop_configs_))
			return nullptr;
		return drop_configs_[id];
	}

	MtItemPackageManager& MtItemPackageManager::Instance()
	{
		return *__mt_item_package_manager;
	}

	MtItemPackageManager::MtItemPackageManager()
	{
		__mt_item_package_manager = this;
	}


	void MtItemPackage::lua_reg(lua_State* ls)
	{
		ff::fflua_register_t<MtItemPackage, ctor()>(ls, "MtItemPackage")
			.def(&MtItemPackage::CanAddToPlayer, "CanAddToPlayer")
			.def(&MtItemPackage::AddToPlayerArrange, "AddToPlayerArrange")
			.def(&MtItemPackage::SetMissionId, "SetMissionId")
			.def(&MtItemPackage::ItemCount, "ItemCount")
			.def(&MtItemPackage::LuaGenItems,"GenItems")
			.def(&MtItemPackage::LuaAddToPlayer,"AddToPlayer")
			.def(&MtItemPackage::AddToGuildSpoil,"AddToGuildSpoil")
			;
	}

	bool MtItemPackage::CanAddToPlayer(MtPlayer* player)
	{
		if (player == nullptr)
			return false;

		if (config_info_->type() == Config::ItemPackageType::RANDOM) {
			return player->BagLeftCapacity(Packet::ContainerType::Layer_EquipAndItemBag, 1);
		}
		else if (config_info_->type() == Config::ItemPackageType::STABLE) {
			std::map<int32, int32> temp;
			for (int32 i = 0; i < config_info_->items_size(); i++) {
				auto item = config_info_->items(i);
				if (item.item_id() > 0 && item.item_amount() > 0) {
					temp.insert({ item.item_id(),item.item_amount() });
				}
			}
			return player->EquipAndItemBagLeftCheck(temp);
		}
		return false;
	}

	void MtItemPackage::CalcGroupWeight()
	{
		if (config_info_ == nullptr) {
			return;
		}

		all_group_weight_ = 0;
		for (int32 i = 0; i < config_info_->weight_size(); i++) {
			if (config_info_->weight(i) > 0) {
				all_group_weight_ += config_info_->weight(i);
			}			
		}
	}

	zxero::int32 MtItemPackage::ItemCount()
	{
		switch (config_info_->type())
		{
		case Config::ItemPackageType::RANDOM:
			return 1;
		case Config::ItemPackageType::STABLE:
			return config_info_->items_size();
		default:
			ZXERO_ASSERT(false);
			return 0;
		}
	}

	MtItemPackage::MtItemPackage(const boost::shared_ptr<Config::ItemPackageConfig>& config_info) :config_info_(config_info)
	{

	}

	void MtItemPackage::DropGroup(std::map<int32, int32>& items)
	{
		if (all_group_weight_ <= 0) {
			return;
		}
		int32 seed = rand_gen->intgen(1, all_group_weight_);
		int32 base_weight = 0;
		for (int32 i = 0; i < config_info_->drop_group_size(); i++) {

			if (config_info_->drop_group(i) < 0 || config_info_->weight(i) < 0) 
				continue;

			base_weight += config_info_->weight(i);
			if (seed <= base_weight) {
				auto drop_config = MtItemPackageManager::Instance().GetDropGroup(config_info_->drop_group(i));
				if (drop_config == nullptr)
					continue;

				int32 seed2 = rand_gen->intgen(1, drop_config->all_weight);
				int32 base_weight2 = 0;
				for (uint32 k = 0; k < drop_config->items.size(); k++) {
					auto child = drop_config->items[k]->item();
					base_weight2 += child.item_weight();
					if ( seed2 <= base_weight2 ) {
						items[child.item_id()] += child.item_amount();
						break;
					}
				}
				break;
			}
		}
	}

	std::map<int32, int32> MtItemPackage::LuaGenItems()
	{
		std::map<int32, int32> temp;
		GenItems(temp);
		return temp;
	}

	bool MtItemPackage::GenItems(std::map<int32, int32>& items)
	{
		if (config_info_ == nullptr) 
			return false;
		switch (config_info_->type())
		{
		case Config::ItemPackageType::RANDOM:
		{
			if (config_info_->items_size() > 0) {
				std::vector<int32> weights;
				std::transform(std::begin(config_info_->items()),
					std::end(config_info_->items()),
					std::back_inserter(weights),
					boost::bind(&Config::ItemPackageElement::item_weight, _1));
				boost::random::discrete_distribution<int, double> dist(std::begin(weights), std::end(weights));
				auto index = dist(rand_gen->generator());
				auto item_config = config_info_->items(index);
				items[item_config.item_id()] += item_config.item_amount();
			}
			break;
		}

		case Config::ItemPackageType::DRAW:
		{
			if (config_info_->items_size() > 0) {
				std::vector<Config::ItemPackageElement *> item_pool;
				for (int32 i = 0; i < config_info_->items_size(); i++) {
					item_pool.push_back(config_info_->mutable_items(i));
				}

				if ((int32)item_pool.size() < config_info_->draw_limit()) {
					return false;
				}

				//按照权重进行抽牌处理
				std::vector<int32> weights;
				for (int32 i = 0; i < config_info_->draw_limit(); i++) {
					weights.clear();
					std::transform(std::begin(item_pool), std::end(item_pool), std::back_inserter(weights), boost::bind(&Config::ItemPackageElement::item_weight, _1));
					boost::random::discrete_distribution<int, double> dist(std::begin(weights), std::end(weights));
					auto index = dist(rand_gen->generator());
					auto item_config = *(item_pool[index]);
					item_pool.erase(item_pool.begin() + index);
					items[item_config.item_id()] += item_config.item_amount();
				}
			}
			break;
		}
		case Config::ItemPackageType::STABLE:
		{

			for (auto i = 0; i < config_info_->items_size(); ++i) {
				auto item_config = config_info_->items(i);
				items[item_config.item_id()] += item_config.item_amount();
			}
			break;
		}
		default:
			break;
		}
		DropGroup(items);
		items[Packet::TokenType::Token_Gold] += config_info_->gold();
		items[Packet::TokenType::Token_Exp] += config_info_->player_exp();
		items[Packet::TokenType::Token_Hero_Exp] += config_info_->actor_exp();
		return true;
	}
	bool MtItemPackage::LuaAddToPlayer(MtPlayer* player)
	{
		Packet::NotifyItemList notify;
		auto ret = AddToPlayer(player,nullptr,&notify);
		if (notify.item_size() > 0) {
			player->SendMessage(notify);
		}
		return ret;
	}
	bool MtItemPackage::AddToPlayer(MtPlayer* player,
    Packet::StatisticInfo* result /*nullptr*/, 
    Packet::NotifyItemList* notify /*= nullptr*/,
    std::function<int(int)> tax_collector/*= std::function<int(int)>*/,
	bool from_battle,
	Config::ItemAddLogType type )
	{
		if (config_info_ == nullptr) {
			return false;
		}
		//battle掉落无视背包大小，能放就放
		if (CanAddToPlayer(player) == false && from_battle == false)
			return false;

		std::map<int32, int32> items;
		if (GenItems(items) == false)
			return false;
		for (auto& item_pair : items) {
			if (result) {
				auto item_result = result->add_items();
				item_result->set_item_id(item_pair.first);
				item_result->set_item_amount(item_pair.second);
			}
			switch (item_pair.first)
			{
			case Packet::TokenType::Token_Exp:
			{
				player->AddItemById(Packet::TokenType::Token_Exp, item_pair.second, type, notify, mission_id_);
			}
			break;
			case Packet::TokenType::Token_Hero_Exp:
			{
				auto formation = config_info_->formation();
				auto actors = player->BattleActor(formation);
				for (auto child : actors) {
					if (child->ActorType() != Packet::ActorType::Main) {
						child->AddExp(item_pair.second, true);
					}
				}
			}
			break;
			case Packet::TokenType::Token_Gold:
			{
				int32 gold = item_pair.second;
				if (tax_collector)
					gold -= tax_collector(gold);
				player->AddItemById(Packet::TokenType::Token_Gold, gold,type, notify, mission_id_);
			}
			break;
			default://道具或者装备
			{
				//检查自动分解
				if (!player->AutoDismantleEquip(item_pair.first,item_pair.second, false, notify)) {
					player->AddItemById(item_pair.first, item_pair.second,type, notify, mission_id_);
				}
			}
			break;
			}
		}
		if (notify != nullptr) {
			notify->set_add_type(type);
			notify->set_source_id(config_info_->id());
		}

		if (from_battle) {
			player->SendCommonReply("DropPackageNotify", {}, {}, {});
		}
		return true;
	}

	//掉落至分配背包
	bool MtItemPackage::AddToPlayerArrange(MtPlayer* player)
	{
		if (player == nullptr) {
			return false;
		}
		if (config_info_ == nullptr) {
			return false;
		}

		auto proxy = player->GetArrangeProxy();
		if (proxy == nullptr) {
			return false;
		}

		if(proxy->IsArrangeBagFull()) {
			return false;
		}
		std::map<int32, int32> drop_items;
		if (!GenItems(drop_items)) {
			return false;
		}
		
		//设置统计类型
		proxy->SetMissionId(mission_id_);

		for (auto child : drop_items) {
			proxy->OnAddItem(child.first, child.second);
		}		

		return true;

	}

	bool MtItemPackage::AddToGuildSpoil(GuildInfo* guild, MtPlayer* player, int32 copyid)
	{
		if (guild == nullptr || player == nullptr) {
			return false;
		}
		if (config_info_ == nullptr) {
			return false;
		}
		std::map<int32, int32> drop_items;
		if (!GenItems(drop_items)) {
			return false;
		}
		for (auto child : drop_items) {
			for (int32 i = 0; i < child.second; i++) {
				if (child.first > 0) {
					guild->AddSpoilItem(child.first, copyid, player);
				}
			}
		}
		return true;
	}
}
