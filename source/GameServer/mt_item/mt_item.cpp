#include "mt_item.h"
#include "mt_container.h"
#include "module.h"
#include "../data/data_manager.h"
#include "../mt_guid/mt_guid.h"
#include "../mt_player/mt_player.h"
#include "../mt_actor/mt_actor_config.h"
#include "../mt_actor/mt_actor.h"
#include "../mt_upgrade/mt_upgrade.h"
#include "../mt_cache/mt_shm_manager.h"
#include "../mt_data_cell/mt_data_container.h"
#include "../mt_config/mt_config.h"
#include "active_model.h"
#include <ItemAndEquip.pb.h>
#include <EquipmentConfig.pb.h>
#include <ItemConfig.pb.h>
#include <SceneCommon.pb.h>
#include <SimpleImpact.pb.h>
#include <BaseConfig.pb.h>
#include <Upgrades.pb.h>
#include <boost/algorithm/string.hpp>
#include "utils.h"
namespace Mt
{
	static MtItemManager* __mt_item_mamager = nullptr;

	void MtItemManager::lua_reg(lua_State* ls)
	{
		ff::fflua_register_t<MtItemManager, ctor()>(ls, "MtItemManager")
			.def(&MtItemManager::GetItemConfig, "GetItemConfig")
			.def(&MtItemManager::GetItemName,"GetItemName")
			.def(&MtItemManager::GetEquipmentConfig, "GetEquipmentConfig")
			.def(&MtItemManager::GetGemConfig,"GetGemConfig")
			.def(&MtItemManager::GetGemCostConfig, "GetGemCostConfig")
			.def(&MtItemManager::ImpactType2Config, "ImpactType2Config")			
			.def(&MtItemManager::GetSuitEquips,"GetSuitEquips")
			.def(&MtItemManager::LoadItemTB, "LoadItemTB")
			.def(&MtItemManager::LoadEquipTB, "LoadEquipTB")
			.def(&MtItemManager::LoadGemTB, "LoadGemTB")
			.def(&MtItemManager::LoadSimpleImpacktTB, "LoadSimpleImpacktTB")
			.def(&MtItemManager::LoadBookTB, "LoadBookTB")
			.def(&MtItemManager::GetEquipBookIndex,"GetEquipBookIndex")
			;
		ff::fflua_register_t<>(ls)
			.def(&MtItemManager::Instance, "LuaItemManager");
	}

	REGISTER_MODULE(MtItemManager)
	{
		require("data_manager");
		register_load_function(module_base::STAGE_LOAD, boost::bind(&MtItemManager::OnLoad, __mt_item_mamager));
	}

	MtItemManager::MtItemManager()
	{
		__mt_item_mamager = this;
	}

	MtItemManager& MtItemManager::Instance()
	{
		return *__mt_item_mamager;
	}

	zxero::int32 MtItemManager::OnLoad()
	{
		LoadItemTB();
		LoadEquipTB();
		LoadGemTB();
		LoadSimpleImpacktTB();
		LoadBookTB();
		return 0;
	}

	void MtItemManager::LoadBookTB(bool reload)
	{
		equip_book_indexs_.clear();
		data_table* table = nullptr;
		if (reload) {
			table = data_manager::instance()->on_reload_file("equip_book");
		}
		else {
			table = data_manager::instance()->get_table("equip_book");
		}
		for (auto i = 0; i < table->get_rows(); ++i) {
			auto row = table->find_row(i);
			int32 index = row->getInt("index");
			int32 id = row->getInt("id");

			if (equip_book_indexs_.find(index) != equip_book_indexs_.end()) {
				ZXERO_ASSERT(false) << "equip_book data repeated! index=" << index << " id=" << id;
			}
			equip_book_indexs_.insert({ index, id });
		}
	}

	void MtItemManager::LoadItemTB( bool reload )
	{
		item_configs_.clear();
		data_table* table = nullptr;
		if (reload) {
			table = data_manager::instance()->on_reload_file("items");
		}
		else {
			table = data_manager::instance()->get_table("items");
		}
		for (auto i = 0; i < table->get_rows(); ++i)
		{
			auto config = boost::make_shared<Config::ItemConfig>();
			auto row = table->find_row(i);
			FillMessageByRow(*config, *row);
			item_configs_.insert({ config->id(), config });
		}
	}
	void MtItemManager::LoadEquipTB(bool reload)
	{
		equip_configs_.clear();
		suit_equip_set_.clear();
		suit_configs_.clear();
		enhance_configs_.clear();

		data_table* table = nullptr;
		if (reload) {
			table = data_manager::instance()->on_reload_file("equipments");
		}
		else {
			table = data_manager::instance()->get_table("equipments");
		}
		for (auto i = 0; i < table->get_rows(); ++i)
		{
			auto config = boost::make_shared<Config::EquipmentConfig>();
			auto row = table->find_row(i);
			FillMessageByRow(*config, *row);
			equip_configs_.insert({ config->id(), config });

			//套装处理
			CalcSuitEquipCount(config);
		}

		//套装
		if (reload) {
			table = data_manager::instance()->on_reload_file("equipment_suit");
		}
		else {
			table = data_manager::instance()->get_table("equipment_suit");
		}
		for (auto i = 0; i < table->get_rows(); ++i) {
			auto config = boost::make_shared<Config::EquipmentSuitConfig>();
			auto row = table->find_row(i);
			FillMessageByRow(*config, *row);
			suit_configs_.insert({ config->id(), config });
		}

		//强化大师
		if (reload) {
			table = data_manager::instance()->on_reload_file("equip_enhence_master");
		}
		else {
			table = data_manager::instance()->get_table("equip_enhence_master");
		}
		for (auto i = 0; i < table->get_rows(); ++i) {
			auto config = boost::make_shared<Config::EnhanceMasterConfig>();
			auto row = table->find_row(i);
			FillMessageByRow(*config, *row);
			enhance_configs_.insert({ config->level(), config });
		}

	}

	void MtItemManager::LoadGemTB(bool reload)
	{
		gem_configs_.clear();
		gem_cost_configs_.clear();

		data_table* table = nullptr;
		if (reload) {
			table = data_manager::instance()->on_reload_file("gems");
		}
		else {
			table = data_manager::instance()->get_table("gems");
		}
		for (auto i = 0; i < table->get_rows(); ++i) {
			auto config = boost::make_shared<Config::GemConfig>();
			auto row = table->find_row(i);
			FillMessageByRow(*config, *row);
			gem_configs_.insert({ config->id(), config });
		}

		//
		if (reload) {
			table = data_manager::instance()->on_reload_file("gems_cost");
		}
		else {
			table = data_manager::instance()->get_table("gems_cost");
		}
		for (auto i = 0; i < table->get_rows(); ++i) {
			auto config = boost::make_shared<Config::GemCostConfig>();
			auto row = table->find_row(i);
			FillMessageByRow(*config, *row);
			gem_cost_configs_.insert({ config->level(), config });
		}

	}
	void MtItemManager::LoadSimpleImpacktTB(bool reload)
	{
		//impact
		simple_impacts_.clear();

		data_table* table = nullptr;
		if (reload) {
			table = data_manager::instance()->on_reload_file("simple_impact");
		}
		else {
			table = data_manager::instance()->get_table("simple_impact");
		}
		for (auto i = 0; i < table->get_rows(); ++i) {
			auto config = boost::make_shared<Packet::SimpleImpactConfig>();
			auto row = table->find_row(i);
			FillMessageByRow(*config, *row);
			auto impact_type = config->impact_type();
			if (simple_impacts_.find(impact_type) != simple_impacts_.end()) {
				ZXERO_ASSERT(false) << "repeated impact_type =" << impact_type;
				continue;
			}
			simple_impacts_[impact_type] = config;
		}
	}

	boost::shared_ptr<Packet::SimpleImpactConfig> MtItemManager::ImpactType2Config(const int32 impact_type)
	{
		auto config = simple_impacts_.find(impact_type);
		if (config != simple_impacts_.end()) {
			return config->second;
		}
		return nullptr;
	}

	void MtItemManager::CalcSuitEquipCount(const boost::shared_ptr<Config::EquipmentConfig> &equip_config)
	{
		int32 suit_id = -1;
		int32 equip_id = equip_config->id();
		if (equip_config->suit_group_id() > 0)
		{
			suit_id = equip_config->suit_group_id();
			auto iter = suit_equip_set_.find(suit_id);
			if (iter != suit_equip_set_.end())
			{
				auto equip = std::find(iter->second.begin(), iter->second.end(), equip_id);
				if (equip == iter->second.end())
				{//不同id计算进套装
					iter->second.push_back(equip_id);
				}
			}
			else
			{
				std::vector<int32> suit;
				suit.push_back(equip_id);
				suit_equip_set_.insert(std::make_pair(suit_id, suit));
			}
		}
	}

	int32 MtItemManager::GetSuitCount(int32 suitId)
	{
		const auto & iter = suit_equip_set_.find(suitId);
		if (iter == suit_equip_set_.end())
			return 0;
		else
			return iter->second.size();
	}

	const Config::ItemType MtItemManager::GetItemType(int32 config_id)
	{
		if (item_configs_.find(config_id) == std::end(item_configs_))
			return Config::ItemType::ItemTypeInvalid;
		return item_configs_[config_id]->type();
	}
	std::string MtItemManager::GetItemName(int32 config_id)
	{
		MtItemType type = GetItemInstanceType(config_id);
		switch (type)
		{
		case Mt::MtItemType::INVALID_ITEM:
			return "";
			break;
		case Mt::MtItemType::NORMAL_ITEM:
			return "item_name_"+boost::lexical_cast<std::string>(config_id);
			break;
		case Mt::MtItemType::GEM_ITEM:
			return "gem_name_" + boost::lexical_cast<std::string>(config_id);
			break;
		case Mt::MtItemType::EQUIP_ITEM:
			return "equip_name_" + boost::lexical_cast<std::string>(config_id);
			break;
		default:
			break;
		}
		return "";
	}
	Packet::ContainerType MtItemManager::CheckOwnerContainer(const int32 config_id)
	{
		auto type = GetItemType(config_id);
		if (type == Config::ItemType::CURRENCY) {
			return Packet::ContainerType::Layer_TokenBag;
		}
		else if (type == Config::ItemType::MISSION) {
			return Packet::ContainerType::Layer_MissionBag;
		}
		else {
			if (config_id < 20000000) {
				return Packet::ContainerType::Layer_EquipBag;
			}
			else {
				return Packet::ContainerType::Layer_ItemBag;
			}
		}
	}

	//物品实例分类
	const MtItemType MtItemManager::GetItemInstanceType(const int32 config_id)
	{
		if (config_id < 20000000) {
			return MtItemType::EQUIP_ITEM;
		}
		else if (config_id < 30000000) {
			return MtItemType::NORMAL_ITEM;
		}
		else  if (config_id < 40000000) {
			return MtItemType::GEM_ITEM;
		}
		else {
			return MtItemType::INVALID_ITEM;
		}
	}


	const int32 MtItemManager::GetMaxBlcokCount(const int32 config_id)
	{
		auto type = GetItemInstanceType(config_id);
		if (type == MtItemType::NORMAL_ITEM) {
			auto config = GetItemConfig(config_id);
			if (config == nullptr) {
				return 1;
			}
			return config->block_max_count();
		}
		else if (type == MtItemType::EQUIP_ITEM) {
			return 1;
		}
		else if (type == MtItemType::GEM_ITEM) {
			auto config = GetGemConfig(config_id);
			if (config == nullptr) {
				return 1;
			}
			return config->block_max_count();
		}
		else {
			return 1;
		}
	}

	const boost::shared_ptr<MtItemBase> MtItemManager::CreateItem(
		MtPlayer* player, zxero::int32 config_id, zxero::int32 count /*= 1*/)
	{
		if (player == nullptr)
			return nullptr;

		auto item_config = GetItemConfig(config_id);
		if (item_config) {
			auto db_info = boost::make_shared<Packet::NormalItem>();
			db_info->set_guid(MtGuid::Instance()(*db_info));
			db_info->set_config_id(config_id);
			db_info->set_count(count);
			db_info->set_param(-1);
			int32 cd = item_config->cool_down() / 1000;
			db_info->set_cool_down(cd);
			db_info->set_life_time(item_config->life_time());
			db_info->set_lock_flag(0);

			if (item_config->script_id() >= 0) {
				try {
					int32 lua_ret = thread_lua->call<int32>(item_config->script_id(), "LuaCreateItem", player, db_info.get(), item_config);
					if (lua_ret != 0)
					{
						LOG_ERROR << "CreateItem failed, player guid=" << player->Guid() << " config_id=" << config_id << " error code=" << lua_ret;
						return nullptr;
					}
				}
				catch (ff::lua_exception& e) {
					e;
					player->SendCommonResult(Packet::ResultOption::Default_Opt, Packet::ResultCode::LuaScriptException);
					return nullptr;
				}
			}

			auto ret =  boost::make_shared<MtNormalItem>(db_info);
			ret->AfterCreate();
			return ret;
		}
		auto equip_config = GetEquipmentConfig(config_id);
		if (equip_config)
		{
			auto db_info = boost::make_shared<Packet::EquipItem>();
			db_info->set_guid(MtGuid::Instance()(*db_info));

			Packet::EquipItem_EquipEnchant enchant;
			for (int32 i = 0; i < 4; i++)
			{
				auto e = db_info->add_enchants();
				e->set_effect_type(""); 
				e->set_effect_value(0);
				e->set_max_value(0);
				e->set_client_lock(false);
				if( i==0)//第一个附魔槽默认解锁
					e->set_slot_open(true);
				else
					e->set_slot_open(false);
			}			

			db_info->set_config_id(config_id);
			db_info->set_bind_actor_guid(INVALID_GUID);
			db_info->set_durable(float(equip_config->init_durable()));
			db_info->set_enhence_level(0);
			db_info->set_lock_flag(0);

			for (int32 i = 0; i < 3; i++)
			{
				db_info->add_gems(0);
			}

			db_info->set_actor_guid(INVALID_GUID);
			db_info->set_slot(Config::EquipmentSlot::EQUIPMENT_SLOT_INVALID);
			db_info->set_magical_res_id(-1);
			auto ret = boost::make_shared<MtEquipItem>(db_info);
			ret->AfterCreate();
			return ret;
		}

		auto gem_config = GetGemConfig(config_id);
		if (gem_config) {
			auto db_info = boost::make_shared<Packet::GemItem>();
			db_info->set_guid(MtGuid::Instance()(*db_info));
			db_info->set_config_id(config_id);
			db_info->set_equip_guid(INVALID_GUID);
			db_info->set_count(count);
			db_info->set_lock_flag(0);
			auto ret = boost::make_shared<MtGemItem>(db_info);
			ret->AfterCreate();
			return ret;
		}
		return nullptr;
	}

	///////////////////////////////////////////////////////////////////////////////////////////
	void MtItemBase::lua_reg(lua_State* ls)
	{
		ff::fflua_register_t<MtItemBase, int()>(ls, "MtItemBase").
			def(&MtItemBase::DbInfo, "DbInfo").
			def(&MtItemBase::Config, "Config").
			def(&MtItemBase::Container, "Container").
			def(&MtItemBase::ConfigId, "ConfigId").
			def(&MtItemBase::DecCount, "DecCount").
			def(&MtItemBase::AddCount, "AddCount").
			def(&MtItemBase::RefreshData2Client, "RefreshData2Client");
	}
	void MtNormalItem::lua_reg(lua_State* ls)
	{
		ff::fflua_register_t<MtNormalItem, ctor()>(ls, "MtNormalItem", "MtItemBase")
			.def(&MtNormalItem::Container, "Container")
			.def(&MtNormalItem::DbInfo, "DbInfo")
			.def(&MtNormalItem::Config, "Config")
			.def(&MtNormalItem::RefreshData2Client, "RefreshData2Client")
			;		
	}
	void MtEquipItem::lua_reg(lua_State* ls)
	{
		ff::fflua_register_t<MtEquipItem, ctor()>(ls, "MtEquipItem", "MtItemBase")
			.def(&MtEquipItem::Container, "Container")
			.def(&MtEquipItem::DbInfo, "DbInfo")
			.def(&MtEquipItem::Config, "Config")
			.def(&MtEquipItem::StripGem, "StripGem")
			.def(&MtEquipItem::SetEnhenceLevel,"SetEnhenceLevel")
			.def(&MtEquipItem::InsetGem, "InsetGem")
			.def(&MtEquipItem::RefreshData2Client,"RefreshData2Client")
			;
	}
	void MtGemItem::lua_reg(lua_State* ls)
	{
		ff::fflua_register_t<MtGemItem, ctor()>(ls, "MtGemItem", "MtItemBase")
			.def(&MtGemItem::Container, "Container")
			.def(&MtGemItem::DbInfo, "DbInfo")
			.def(&MtGemItem::Config, "Config")
			.def(&MtGemItem::RefreshData2Client, "RefreshData2Client")
			;
	}

	/////////////////////
	bool MtItemBase::OnAdd2Container(const boost::shared_ptr<MtContainer>& container)
	{
		if (!container_.expired())
		{
			container_.lock()->RemoveItem(shared_from_this(),false);
		}
		container_ = container;
		SetContainerGuid(container_.lock()->Guid());
		return true;
	}

	void MtItemBase::RefreshData2Client() const
	{
		if (container_.expired()) {
			LOG_ERROR << "[mt item base] contianer nullptr";
			return;
		}
		Packet::PlayerContainerRefreshItemNotify notify;
		notify.set_container_type(container_.lock()->Type());
		auto info = notify.add_items();
		FillMessage(*info);
		auto player = container_.lock()->Player().lock();
		if (player) {
			player->SendMessage(notify);
		}
	}

	std::string MtItemBase::GenHyperStr()
	{
		Packet::ClientItemInfo notifymsg;
		FillMessage(notifymsg);
		char temp[1024] = { 0 };
		notifymsg.SerializePartialToArray(temp, notifymsg.ByteSize());
		std::string outchat = "<a item=" + utils::bytestohexstring(temp, notifymsg.ByteSize()) + ">["+GetName()+"%"+ GetColor()+"]</a>";
		return outchat;
	}

	MtItemBase::MtItemBase(const boost::shared_ptr<MtContainer>& container) 
		:container_(container)
	{
	}

	const boost::shared_ptr<MtContainer> MtItemBase::Container()
	{
		return container_.lock();
	}

	Packet::ResultCode MtNormalItem::OnUse(const boost::shared_ptr<MtActor> actor_target)
	{		
		if (container_.expired()) {
			LOG_ERROR << "[MtNormalItem] contianer empty";
			return Packet::ResultCode::UnknownError;
		}
		auto player = container_.lock()->Player().lock();
		if (player == nullptr) {
			return Packet::ResultCode::UnknownError;
		}
	
		auto config = MtItemManager::Instance().GetItemConfig(db_info_->config_id());
		if (config == nullptr) {
			return Packet::ResultCode::InvalidConfigData;
		}

		if (config->type() != Config::ItemType::MISSION && 
			config->type() != Config::ItemType::AUTOMEDIC) {
			if (!config->can_use()) {
				return Packet::ResultCode::CantUseItem;
			}
		}

		int32 bigcd = config->cool_down() / 1000;
		if (bigcd >= BIG_TICK&&db_info_->cool_down()>0) {// 超过10s cd的道具才做服务器cd校验 [3/25/2017 SEED]
			return Packet::ResultCode::CoolDownLimit;
		}
		//使用判定
		if (player->PlayerLevel() < config->min_level_limit() || player->PlayerLevel() > config->max_level_limit()) {
			return Packet::ResultCode::LevelLimit;
		}

		try {
			if (config->script_id() >= 0) {
				//各自的脚本走道具扣除的流程更靠谱些
				int32 lua_ret = thread_lua->call<int32>(config->script_id(), "LuaUseItem", player, db_info_, config, actor_target.get());
				if (lua_ret != 0) {
					return (Packet::ResultCode)lua_ret;
				}
			}
			else {//没有脚本直接扣除
				if (!player->DelItemById(ConfigId(), 1, Config::ItemDelLogType::DelType_UseItem, Guid())) {
					return Packet::ResultCode::CostItemNotEnough;
				}
			}
			//任务检测脚本
			player->OnLuaFunCall_x("xOnUseItem",
			{
				{ "value", db_info_->config_id() }
			});
		}
		catch (ff::lua_exception& e) {e;
			return Packet::ResultCode::LuaScriptException;
		}

		//
		db_info_->set_cool_down(config->cool_down() / 1000);

		return Packet::ResultCode::ResultOK;
	}


	Packet::NormalItem* MtNormalItem::DbInfo() const
	{
		return db_info_.get();
	}

	void MtNormalItem::OnSave(const boost::shared_ptr<zxero::dbcontext>& dbctx)
	{
		ZXERO_ASSERT(container_.expired() == false);
		ActiveModel::Base<Packet::NormalItem>(dbctx).Save(*db_info_);
	}

	void MtNormalItem::OnCache( )
	{
		if (container_.expired()) {
			ZXERO_ASSERT(false);
			return;
		}
		auto player = container_.lock()->Player().lock();
		if (player) {
			player->Cache2Save(db_info_.get());
		} else {
			ZXERO_ASSERT(false);
		}
	}

	int32 MtNormalItem::Count() const
	{
		return db_info_->count();
	}

	bool MtNormalItem::CanOverlay() const
	{
		return MaxBlockCount() >1;
	}

	int32 MtNormalItem::ConfigId() const
	{
		return db_info_->config_id();
	}

	std::string MtNormalItem::GetName() const
	{
		std::string name = "item_name_" + std::to_string(ConfigId());
		return name;
	}
	std::string MtNormalItem::GetColor() const
	{
		auto cfg = Config();
		if (cfg)
		{
			if (cfg->quality() == 1)
				return "#FFFFFFFF";
			else if (cfg->quality() == 2)
				return "#5AB935FF";
			else if (cfg->quality() == 3)
				return "#546EF0FF";
			else if (cfg->quality() == 4)
				return "#AB33FFFF";
			else
				return "#EA7F28FF";
		}
		return "#FFFFFFFF";
	}
	Config::ItemConfig* MtNormalItem::Config() const
	{
		return MtItemManager::Instance().GetItemConfig(ConfigId()).get();
	}


	int32 MtNormalItem::MaxBlockCount() const
	{
		return MtItemManager::Instance().GetItemConfig(db_info_->config_id())->block_max_count();
	}

	void MtNormalItem::RefreshData2Client() const
	{
		MtItemBase::RefreshData2Client();
	}
	bool MtNormalItem::OnAdd2Container(const boost::shared_ptr<MtContainer>& container)
	{
		if (MtItemBase::OnAdd2Container(container)) {
			db_info_->set_container_guid(container->Guid());
			return true;
		}
		else {
			return false;
		}
	}

	zxero::int32 MtNormalItem::AddCount(zxero::int32 count)
	{
		if (container_.expired()) {
			LOG_ERROR << "[MtNormalItem] contianer empty";
			return 0;
		}
		db_info_->set_count(db_info_->count() + count);
		Packet::PlayerContainerRefreshItemNotify notify;
		notify.set_container_type(container_.lock()->Type());
		FillMessage(*(notify.add_items()));
		auto player = container_.lock()->Player().lock();
		if (player) {
			player->SendMessage(notify);
			return db_info_->count();
		} else {
			ZXERO_ASSERT(false);
			return 0;
		}
	}


	zxero::int32 MtNormalItem::DecCount(zxero::int32 count)
	{
		if (count <= 0 || db_info_->count() < count) {
			LOG_ERROR << "[normal item] dec count dismatch."
				<< " config:" << ConfigId() << ",curr_count:" << Count()
				<< ",dec_count:" << count;
			return db_info_->count();
		}
		else {
			if (container_.expired()) {
				LOG_ERROR << "[MtNormalItem] contianer empty";
				return 0;
			}
			db_info_->set_count(db_info_->count() - count);
			int32 now_count = db_info_->count();
			if (Count() > 0) {
				Packet::PlayerContainerRefreshItemNotify notify;
				notify.set_container_type(container_.lock()->Type());
				FillMessage(*(notify.add_items()));
				auto player = container_.lock()->Player().lock();
				if (player) {
					player->SendMessage(notify);
				} else {
					ZXERO_ASSERT(false);
				}
			} else {
				container_.lock()->RemoveItem(shared_from_this(), true);
			}
			return now_count;
		}
	}

	void MtNormalItem::FillMessage(Packet::ClientItemInfo& info) const
	{
		info.mutable_normal_item()->CopyFrom(*db_info_);
	}
	
	zxero::uint64 MtNormalItem::Guid() const
	{
		return db_info_->guid();
	}

	void MtNormalItem::SetContainerGuid(zxero::uint64 guid)
	{
		db_info_->set_container_guid(guid);
	}
	void MtNormalItem::EnableLock(bool lock) {
		db_info_->set_lock_flag(lock ? 1 : 0);
	}
	bool MtNormalItem::IsLock() const {
		return db_info_->lock_flag() ? true : false;
	};

	void MtEquipItem::EnableLock(bool lock) {
		db_info_->set_lock_flag(lock ? 1 : 0);
	}

	void MtEquipItem::GatherGemId(std::vector<int32>& result) const
	{
		for (auto id : db_info_->gems()) {
			if (id != 0) result.push_back(id);
		}
	}

	bool MtEquipItem::IsLock() const {
		return db_info_->lock_flag() ? true : false;
	};

	void MtEquipItem::RefreshData2Client() const
	{
		MtItemBase::RefreshData2Client();
	}

	void MtGemItem::EnableLock(bool lock) {
		db_info_->set_lock_flag(lock ? 1 : 0);
	}
	bool MtGemItem::IsLock() const {
		return db_info_->lock_flag() ? true : false;
	};
	void MtEquipItem::OnSave(const boost::shared_ptr<zxero::dbcontext>& dbctx)
	{
		if (container_.expired()) {
			ZXERO_ASSERT(false);
			return;
		}
		ActiveModel::Base<Packet::EquipItem>(dbctx).Save(*db_info_);
	}

	void MtEquipItem::OnCache( )
	{
		if (container_.expired()) {
			ZXERO_ASSERT(false);
			return;
		}
		auto player = container_.lock()->Player().lock();
		if (player) {
			player->Cache2Save(db_info_.get());
		} else {
			ZXERO_ASSERT(false);
		}
	}

	bool MtEquipItem::SameType(const boost::shared_ptr<MtEquipItem>& rhs)
	{
		return Config()->type() == rhs->Config()->type();
	}

	void MtEquipItem::ActorGuid(zxero::uint64 guid)
	{
		db_info_->set_actor_guid(guid);
		UpdateEquipScore();
	}

	zxero::uint64 MtEquipItem::ActorGuid() const
	{
		return db_info_->actor_guid();
	}


	zxero::uint64 MtEquipItem::GetBindActorGuid() const
	{
		if (db_info_) {
			return db_info_->bind_actor_guid();
		}
		return INVALID_GUID;
	}

	void MtEquipItem::SetBindActorGuid(uint64 guid)
	{
		db_info_->set_bind_actor_guid(guid);
	}

	Config::EquipmentSlot MtEquipItem::Slot() const
	{
		return db_info_->slot();
	}

	void MtEquipItem::Slot(Config::EquipmentSlot slot)
	{
		db_info_->set_slot(slot);
	}

	bool MtEquipItem::OnAdd2Container(const boost::shared_ptr<MtContainer>& container)
	{
		if (MtItemBase::OnAdd2Container(container)) {
			db_info_->set_container_guid(container->Guid());
			return true;
		}
		else {
			return false;
		}
	}

	void MtEquipItem::FillMessage(Packet::ClientItemInfo& info) const
	{
		attribute_set effect;
		EquipEffect(effect);
		info.mutable_equip_item()->CopyFrom(*db_info_);
		
	}
	void MtEquipItem::FillEquipInfo(Packet::EquipItem& info) const
	{
		info.CopyFrom(*db_info_);
	}

	zxero::uint64 MtEquipItem::Guid() const
	{
		return db_info_->guid();
	}

	void MtEquipItem::EquipEffect(attribute_set& changes) const
	{
		MtActorConfig::Instance().EquipEffect(*db_info_, changes);
	}

	/**
	* \brief 更新装备评分
	* \return void
	*/
	void MtEquipItem::UpdateEquipScore() const 
	{
		attribute_set attr;
		EquipEffect(attr);
		Packet::BattleInfo battle_info;
		MtActorConfig::BattleInfoHelper(attr, battle_info);
		if (ActorGuid() != INVALID_GUID && container_.expired() == false
			&& container_.lock()->Player().expired() == false) {
			auto actor = container_.lock()->Player().lock()->FindActorByGuid(ActorGuid());
			if (actor) {
				MtActorConfig::Instance().LevelOne2LevelTwo(actor->DbInfo()->talent(), battle_info);
			}
		} else {
			MtActorConfig::Instance().LevelOne2LevelTwo(Packet::Talent::Non_Talent, battle_info);
		}
		db_info_->set_score(MtActorConfig::BattleInfo2Score(battle_info));
	}

	void MtEquipItem::SetContainerGuid(zxero::uint64 guid)
	{
		db_info_->set_container_guid(guid);
	}

	bool MtEquipItem::StripGem(int32 config_id, int32 index)
	{
		if (HasGem(config_id, index)) {
			db_info_->set_gems(index, 0);
			UpdateEquipScore();
			return true;
		}
		return false;
	}

	bool MtEquipItem::InsetGem(int32 config_id, int32 index)
	{
		if (config_id < 0) {
			return false;
		}
		if (CanInsetGem(index)) {
			db_info_->set_gems(index, config_id);
			UpdateEquipScore();
			return true;
		}
		return false;
	}


	bool MtEquipItem::CanInsetGem(int32 index)
	{
		auto config = Config();
		if (config == nullptr) return false;
		if (index > db_info_->gems_size() - 1) return false;
		if (index > config->gem_slot_count() - 1) return false;
		bool has_loc = config->gem_slot_count() - std::count_if(db_info_->gems().begin(), db_info_->gems().end(),
			[](int32 config_id) {return config_id != 0; }) > 0;
		bool empty = db_info_->gems(index) == 0;
		return has_loc && empty;
	}


	zxero::int32 MtEquipItem::GemCount() const
	{
		return std::count_if(db_info_->gems().begin(), db_info_->gems().end(),
			[](auto id) {return id > 0; });
	}

	bool MtEquipItem::HasGem(int32 config_id, int32 index)
	{
		if (config_id == 0) {
			return db_info_->gems(index) > 0;
		} else {
			return db_info_->gems(index) == config_id;
		}
	}

	Packet::EquipItem* MtEquipItem::DbInfo() const
	{
		return db_info_.get();
	}


	std::vector<std::string> MtEquipItem::OldEnchantsTypes() const
	{
		std::vector<std::string> ret;
		for (int32 i = 0; i < db_info_->enchants().size(); ++i) {
			if (db_info_->enchants(i).effect_type().empty() == false) {
				ret.push_back(db_info_->enchants(i).effect_type());
			}
		}
		return ret;
	}

	std::vector<Packet::EquipItem_EquipEnchant*> MtEquipItem::OpenedEnchants(const std::vector<int32>& slot_ids, bool effect)
	{
		std::vector<Packet::EquipItem_EquipEnchant*> result;
		for (int32 i=0; i<db_info_->enchants().size(); i++ )
		{
			if (db_info_->enchants(i).slot_open()
				&& std::find(std::begin(slot_ids), std::end(slot_ids), i) != std::end(slot_ids)) {
				if (effect && db_info_->enchants(i).effect_value() <= 0) {
					continue;
				}
				result.push_back(db_info_->mutable_enchants(i));
			}
		}
		return result;
	}

	Packet::EquipItem_EquipEnchant* MtEquipItem::Enchant(int32 index)
	{
		for (int32 i = 0; i < db_info_->enchants().size(); i++)
		{
			if (i == index) {
				return db_info_->mutable_enchants(i);
			}
		}
		return nullptr;
	}

	zxero::int32 MtEquipItem::OpenEnchantSlotSize() const
	{
		auto i = 0;
		for (auto child: db_info_->enchants())
		{			
			if (child.slot_open()) {
				i++;
			}
		}
		return i;
	}

	bool MtEquipItem::InheritFrom(const MtEquipItem& old_equip, MtPlayer& player)
	{
		static const int32 MAX_GEM_COUNT = 3;
		if (Config() == nullptr || old_equip.Config() == nullptr) {
			LOG_ERROR << "[equip_inherit] " << Guid() << "," << DbInfo()->config_id()
				<< " <- " << old_equip.Guid() << "," << old_equip.DbInfo()->config_id()
				<< " config error";
			return false;
		}
		if (db_info_->gems_size() != old_equip.db_info_->gems_size()
			|| db_info_->gems_size() != MAX_GEM_COUNT) {
			LOG_ERROR << "[equip_inherit] " << Guid() << "," << DbInfo()->gems_size()
				<< " <- " << old_equip.Guid() << "," << DbInfo()->gems_size()
				<< " gems_size error";
			return false;
		}
		//宝石
		int32 gem_count = GemCount() + old_equip.GemCount();
		if (player.BagLeftCapacity(Packet::ContainerType::Layer_ItemBag, gem_count) == false) {
			player.SendClientNotify("op_notify_002", -1, -1);
			return false;
		}
		std::sort(db_info_->mutable_gems()->begin(), db_info_->mutable_gems()->begin(), std::greater<int32>());
		std::sort(old_equip.db_info_->mutable_gems()->begin(), old_equip.db_info_->mutable_gems()->begin(), std::greater<int32>());
		int32 max_index = std::minmax(Config()->gem_slot_count(), old_equip.Config()->gem_slot_count()).first;
		for (int32 i = 0; i < max_index; ++i) {
			int32 temp = db_info_->gems(i);
			db_info_->set_gems(i, old_equip.db_info_->gems(i));
			old_equip.db_info_->set_gems(i, temp);
		}
		for (auto& gem_id : (*old_equip.db_info_->mutable_gems())) {
			if (gem_id > 0) {
				player.AddItemById(gem_id, 1, Config::ItemAddLogType::AddType_TakeOutGem);
				gem_id = 0;
			}
		}
		//强化等级
		{
			auto self_enhence_level = db_info_->enhence_level();
			db_info_->set_enhence_level(old_equip.db_info_->enhence_level());
			old_equip.db_info_->set_enhence_level(self_enhence_level);
		}
		//附魔
		db_info_->mutable_enchants()->Swap(old_equip.db_info_->mutable_enchants());
		//幻化不允许继承
		//{
		//	auto self_magical_res_id = db_info_->magical_res_id();
		//	db_info_->set_magical_res_id(old_equip.db_info_->magical_res_id());
		//	old_equip.db_info_->set_magical_res_id(self_magical_res_id);
		//}
		RefreshData2Client();
		old_equip.RefreshData2Client();
		return true;
	}

	int32 MtEquipItem::GetMagicalResId() const
	{ 
		int32 id = db_info_->magical_res_id(); 
		if (id <= 0)
		{
			auto config_ = MtItemManager::Instance().GetEquipmentConfig(db_info_->config_id());
			id = config_->res();
		}
		return id;
	}

	void MtEquipItem::MagicalFrom(const int32 magical_res_id)
	{
		if (magical_res_id <= 0)
			return;
		db_info_->set_magical_res_id(magical_res_id);
		RefreshData2Client();
	}

	void MtEquipItem::CacheBatchEnchantInfo(const Packet::BatchEnchantEquipReply& info)
	{
		auto cache = boost::make_shared<Packet::BatchEnchantEquipReply>();
		cache->CopyFrom(info);
		batch_enchant_cache_ = cache;
	}

	bool MtEquipItem::HasBatchEnchantCache() const
	{
		return batch_enchant_cache_ != nullptr;
	}

	void MtEquipItem::ApplyBatchEnchantCache(int32 index)
	{
		if (batch_enchant_cache_) {
			auto all_slots = OpenedEnchants({ 0,1,2,3 });
			for (int i = 0; i < batch_enchant_cache_->previews_size(); ++i) {
				if (i == index) {
					auto& preview = batch_enchant_cache_->previews(i);
					for (auto j = 0u; j < all_slots.size(); ++j) {
						auto slot = all_slots[j];
						slot->set_effect_type(preview.elements(j).effect_type());
						slot->set_effect_value(preview.elements(j).effect_value());
						slot->set_max_value(preview.elements(j).max_value());
					}
				}
			}
			batch_enchant_cache_ = nullptr;
		}
	}

	bool MtEquipItem::CheckRace(const Packet::Race r)
	{
		auto config = Config();
		if (config == nullptr)
			return false;

		if (config->race_limit().size() <= 0)
			return true;

		if (config->race_limit(0)==0)
			return true;

		for (auto i = 0; i <config->race_limit().size(); i++)
		{
			if (r == config->race_limit(i))
				return true;
		}

		return false;
	}

	bool MtEquipItem::DurableCost(real32 cost, bool simulate /*= false*/)
	{
		auto defer = Defer([&]() { if (!simulate) RefreshData2Client(); });
		auto new_durable = DbInfo()->durable() - cost;
		if (new_durable < 0.f) 
			new_durable = 0.f;
		DbInfo()->set_durable(new_durable);
		if (abs(new_durable) <= 0.00001/* && AutoRefixEquipDurable(simulate) == false*/) {
			return false;
		} else {
			return true;
		}
	}

	bool MtEquipItem::AutoRefixEquipDurable(bool simulate /*= false*/)
	{
		if (container_.expired()){
			ZXERO_ASSERT(false);
			return false;
		}
		auto player = container_.lock()->Player().lock();
		if (player == nullptr) {
			ZXERO_ASSERT(false);
			return false;
		}
		if (player->GetOnHookOption().auto_refix()) {
			auto init_durable = Config()->init_durable();
			auto limit = player->GetOnHookOption().auto_equip_durable();
			if (init_durable <= 0 || limit <= 0) {
				return false;
			}
			auto rate = db_info_->durable() / (float)init_durable;
			auto lrate = ((limit > 50) ? 50.0 : (float)limit) / 100.0;		
			if ( rate <= lrate ) {
				int32 cost = MtUpgradeConfigManager::Instance().EquipFixCost(*this);

				//卡福利
				auto container = player->DataCellContainer();
				if (container != nullptr) {
					if (container->check_bit_data(Packet::BitFlagCellIndex::MonthCardFlag)   ) {
						auto config = MtConfigDataManager::Instance().FindConfigValue("card_refix_value");
						if (config != nullptr && config->value1() > 0) {
							cost = (int32)(cost*(1.0 - (float)(config->value1())/100.0));
						}						
					}
				}

				if (player->DelItemById(Packet::TokenType::Token_Gold, cost, Config::ItemDelLogType::DelType_Refix, Guid())) {
					db_info_->set_durable((float)init_durable);
					if (!simulate) {
						RefreshData2Client();
					}
					return true;
				}
			}
		}
		return false;
	}

	bool MtEquipItem::CheckProf(const Packet::Professions p)
	{
		auto config = Config();
		if (config == nullptr)
			return false;

		if (config->prof_limit().size() <= 0)
			return true;

		if (config->prof_limit(0) == 0)
			return true;

		for (auto i = 0; i < config->prof_limit().size(); i++)
		{
			if (p == config->prof_limit(i))
				return true;
		}

		return false;
	}
	bool MtEquipItem::CheckTalent(const Packet::Talent t)
	{
		auto config = Config();
		if (config == nullptr)
			return false;

		if (config->talent_limit().size() <= 0)
			return true;

		if (config->talent_limit(0) == 0)
			return true;

		for (auto i = 0; i < config->talent_limit().size(); i++)
		{
			if (t == config->talent_limit(i))
				return true;
		}

		return false;
	}
	zxero::int32 MtEquipItem::Count() const
	{
		return 1;
	}

	zxero::int32 MtEquipItem::ConfigId() const
	{
		return db_info_->config_id();
	}
	std::string MtEquipItem::GetName() const
	{
		std::string name = "equip_name_" + std::to_string(ConfigId());
		return name;
	}

	std::string MtEquipItem::GetColor() const
	{
		auto cfg = Config();
		if (cfg)
		{
			if (cfg->color() == Config::EquipmentColor::EQUIP_WHITE)
				return "#FFFFFFFF";
			else if (cfg->color() == Config::EquipmentColor::EQUIP_GREEN)
				return "#5AB935FF";
			else if (cfg->color() == Config::EquipmentColor::EQUIP_BLUE)
				return "#546EF0FF";
			else if (cfg->color() == Config::EquipmentColor::EQUIP_PURPLE)
				return "#AB33FFFF";
			else if (cfg->color() == Config::EquipmentColor::EQUIP_ORANGE)
				return "#EA7F28FF";
		}
		return "#FFFFFFFF";
	}
	zxero::int32 MtEquipItem::EnhenceLevel() const
	{
		return db_info_->enhence_level();
	}

	void MtEquipItem::SetEnhenceLevel(int32 level)
	{
		if (container_.expired()) {
			ZXERO_ASSERT(false);
			return;
		}
		auto player = container_.lock()->Player().lock();
		if (player) {
			player->OnLuaFunCall_x("xEnhenceLevel",
			{
				{ "value",level }
			});
		} else {
			ZXERO_ASSERT(false);
		}

		db_info_->set_enhence_level(level);
		UpdateEquipScore();
	}

	Config::EquipmentConfig* MtEquipItem::Config() const
	{
		return MtItemManager::Instance().GetEquipmentConfig(ConfigId()).get();
	}

	zxero::int32 MtEquipItem::AddCount(zxero::int32 /*count*/)
	{
		ZXERO_ASSERT(false);
		return 1;
	}

	zxero::int32 MtEquipItem::DecCount(zxero::int32 /*count*/)
	{
		ZXERO_ASSERT(false);
		return 1;
	}

	void MtGemItem::OnSave(const boost::shared_ptr<zxero::dbcontext>& dbctx)
	{
		if (container_.expired()) {
			ZXERO_ASSERT(false);
			return;
		}
		ActiveModel::Base<Packet::GemItem>(dbctx).Save(*db_info_);
	}

	void MtGemItem::OnCache( )
	{
		if (container_.expired()) {
			ZXERO_ASSERT(false);
			return;
		}
		auto player = container_.lock()->Player().lock();
		if (player) {
			player->Cache2Save(db_info_.get());
		} else {
			ZXERO_ASSERT(false);
		}
	}

	zxero::int32 MtGemItem::Count() const
	{
		return db_info_->count();
	}

	bool MtGemItem::CanOverlay() const
	{
		return MaxBlockCount()>1;
	}

	int32 MtGemItem::MaxBlockCount() const
	{
		return MtItemManager::Instance().GetGemConfig(db_info_->config_id())->block_max_count();
	}


	zxero::int32 MtGemItem::ConfigId() const
	{
		return db_info_->config_id();
	}
	std::string MtGemItem::GetName() const
	{
		std::string name = "gem_name_" + std::to_string(ConfigId());
		return name;
	}
	std::string MtGemItem::GetColor() const
	{
		auto cfg = Config();
		if (cfg)
		{
			if (cfg->quality() == 1)
				return "#FFFFFFFF";
			else if (cfg->quality() == 2)
				return "#5AB935FF";
			else if (cfg->quality() == 3)
				return "#546EF0FF";
			else if (cfg->quality() == 4)
				return "#AB33FFFF";
			else
				return "#EA7F28FF";
		}
		return "#FFFFFFFF";
	}

	zxero::int32 MtGemItem::AddCount(zxero::int32 count)
	{
		if (container_.expired()) {
			ZXERO_ASSERT(false);
			return 0;
		}
		auto player = container_.lock()->Player().lock();
		if (player == nullptr) {
			ZXERO_ASSERT(false);
			return 0;
		}
		db_info_->set_count(db_info_->count() + count);
		Packet::PlayerContainerRefreshItemNotify notify;
		notify.set_container_type(container_.lock()->Type());
		FillMessage(*(notify.add_items()));
		player->SendMessage(notify);
		return db_info_->count();
	}

	zxero::int32 MtGemItem::DecCount(zxero::int32 count)
	{
		if (container_.expired()) {
			ZXERO_ASSERT(false);
			return 0;
		}
		auto player = container_.lock()->Player().lock();
		if (player == nullptr) {
			ZXERO_ASSERT(false);
			return 0;
		}
		if (count <= 0 || db_info_->count() < count) {
			ZXERO_ASSERT(false);
			return db_info_->count();
		} else {
			db_info_->set_count(db_info_->count() - count);
			int32 now_count = db_info_->count();
			if (Count() > 0) {
				Packet::PlayerContainerRefreshItemNotify notify;
				notify.set_container_type(container_.lock()->Type());
				FillMessage(*(notify.add_items()));
				player->SendMessage(notify);
			} else {
				container_.lock()->RemoveItem(shared_from_this(), true);
			}
			return now_count;
		}
	}

	void MtGemItem::FillMessage(Packet::ClientItemInfo& info) const
	{
		info.mutable_gem_item()->CopyFrom(*db_info_);
	}

	void MtGemItem::FillGemInfo(Packet::GemItem& gem_info)
	{
		gem_info.CopyFrom(*db_info_);
	}

	zxero::uint64 MtGemItem::Guid() const
	{
		return db_info_->guid();
	}

	void MtGemItem::SetContainerGuid(zxero::uint64 guid)
	{
		db_info_->set_container_guid(guid);
	}

	zxero::uint64 MtGemItem::EquipGuid() const
	{
		return db_info_->equip_guid();
	}

	void MtGemItem::EquipGuid(uint64 equip_guid)
	{
		db_info_->set_equip_guid(equip_guid);
	}

	Config::GemConfig* MtGemItem::Config() const
	{
		return MtItemManager::Instance().GetGemConfig(ConfigId()).get();
	}

	Packet::GemItem* MtGemItem::DbInfo() const
	{
		return db_info_.get();
	}

	void MtGemItem::RefreshData2Client() const
	{
		MtItemBase::RefreshData2Client();
	}
}