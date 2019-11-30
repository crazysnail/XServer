#include "mt_market.h"
#include "module.h"
#include "../data/data_manager.h"
#include "../mt_player/mt_player.h"
#include "../mt_server/mt_server.h"
#include "../base/mt_db_save_work.h"
#include "../mt_cache/mt_shm_manager.h"
#include "../mt_guild/mt_guild_manager.h"
#include "../mt_guild/mt_player_guild_proxy.h"
#include "active_model.h"
#include <ItemAndEquip.pb.h>
#include <ItemConfig.pb.h>
#include <MarketConfig.pb.h>
#include <MarketConfig.proto.fflua.h>
#include "../mt_guid/mt_guid.h"
#include "utils.h"

namespace Mt
{
	void MtMarketManager::lua_reg(lua_State* ls)
	{
		ff::fflua_register_t<MtMarketManager, ctor()>(ls, "MtMarketManager")
			.def(&MtMarketManager::FindMarket, "FindMarket")
			.def(&MtMarketManager::FindItem, "FindItem")
			.def(&MtMarketManager::FindItemByItemId, "FindItemByItemId")
			.def(&MtMarketManager::FindItemByItemIdEx, "FindItemByItemIdEx")
			.def(&MtMarketManager::FindChargeItem, "FindChargeItem")
			.def(&MtMarketManager::FindChargeIndex, "FindChargeIndex")
			.def(&MtMarketManager::FindChargeProductId, "FindChargeProductId")			
			.def(&MtMarketManager::LoadMarketTB, "LoadMarketTB")
			.def(&MtMarketManager::LoadMarketItemTB, "LoadMarketItemTB")
			.def(&MtMarketManager::LoadChargeStoreTB, "LoadChargeStoreTB")
			.def(&MtMarketManager::LoadChargeListTB, "LoadChargeListTB")
			.def(&MtMarketManager::LuaBuyCheck, "BuyCheck")
			;

		ff::fflua_register_t<>(ls)
			.def(&MtMarketManager::Instance, "LuaMtMarketManager");
	}


	static MtMarketManager* __market_manager = nullptr;
	REGISTER_MODULE(MtMarketManager)
	{
		require("data_manager");
		register_load_function(module_base::STAGE_LOAD, boost::bind(&MtMarketManager::OnLoad, __market_manager));
	}

	bool MtMarketManager::LoadPlayerMarket(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<zxero::dbcontext>& dbctx)
	{
		for (int32 i = 0; i < Config::MarketType::MarketTypeMax; i++)
		{
			boost::shared_ptr<Config::MarketConfig> config = FindMarket(i);
			if (config == nullptr) {
				continue;
			}


			if (ActiveModel::Base<Packet::Market>(dbctx).CheckExist({ {"player_guid", player->Guid()},{"market_type", i } }) == true) {
				auto market_db = ActiveModel::Base<Packet::Market>(dbctx).FindOne({ { "player_guid", player->Guid() },{ "market_type", i } });
				auto market = boost::make_shared<MtMarket>(*player, market_db);
				if (!player->AddMarket(market)) {
					ZXERO_ASSERT(false) << "AddMarket failed! player guid=" << player->Guid() << " market type=" << market->GetType();
				}
				market->Enable(config->open() == 1, true);

			}
			else {
				auto market_db = boost::make_shared<Packet::Market>();
				market_db->set_market_type((Config::MarketType)i);
				market_db->set_player_guid(player->Guid());
				market_db->set_refresh_cd(config->refresh_cd());
				market_db->set_guid(i);
				market_db->set_market_items("");
				market_db->set_buys_count("");
				auto market = boost::make_shared<MtMarket>(*player, market_db);				
				if (!player->AddMarket(market)) {
					ZXERO_ASSERT(false) << "AddMarket failed! player guid=" << player->Guid() << " market type=" << market->GetType();
				}
				market->Enable(config->open() == 1, true);
			}
		}

		return true;
	}
	

	MtMarketManager::MtMarketManager()
	{
		__market_manager = this;
	}

	MtMarketManager& MtMarketManager::Instance()
	{
		return *__market_manager;
	}

	void MtMarketManager::LoadMarketTB(bool reload)
	{
		market_configs_.clear();
		data_table* table = nullptr;
		if (reload) {
			table = data_manager::instance()->on_reload_file("market");
		}
		else {
			table = data_manager::instance()->get_table("market");
		}

		for (auto i = 0; i < table->get_rows(); ++i)
		{
			auto config = boost::make_shared<Config::MarketConfig>();
			auto row = table->find_row(i);
			FillMessageByRow(*config, *row);

			if (market_configs_.find((int32)config->market_id()) != market_configs_.end()) {
				ZXERO_ASSERT(false) << "repeated market id! id=" << config->market_id();
				continue;
			}

			market_configs_.insert({ (int32)config->market_id(), config });
		}

	}
	void MtMarketManager::LoadMarketItemTB(bool reload)
	{
		item_configs_.clear();
		data_table* table = nullptr;
		if (reload) {
			table = data_manager::instance()->on_reload_file("market_item");
		}
		else {
			table = data_manager::instance()->get_table("market_item");
		}

		for (auto i = 0; i < table->get_rows(); ++i)
		{
			auto config = boost::make_shared<Config::MarketItemConfig>();
			auto row = table->find_row(i);
			FillMessageByRow(*config, *row);

			if (item_configs_.find(config->id()) != item_configs_.end()) {
				ZXERO_ASSERT(false) << "repeated market item id! id=" << config->id();
				continue;
			}
			item_configs_.insert({ config->id(), config });
		}
	}
	void MtMarketManager::LoadChargeStoreTB(bool reload)
	{
		charge_configs_.clear();
		data_table* table = nullptr;
		if (reload) {
			table = data_manager::instance()->on_reload_file("charge_store");
		}
		else {
			table = data_manager::instance()->get_table("charge_store");
		}
		for (auto i = 0; i < table->get_rows(); ++i)
		{
			auto config = boost::make_shared<Config::ChargeItemConfig>();
			auto row = table->find_row(i);
			FillMessageByRow(*config, *row);

			if (charge_configs_.find(config->id()) != charge_configs_.end()) {
				ZXERO_ASSERT(false) << "repeated charge item id! id=" << config->id();
				continue;
			}
			charge_configs_.insert({ config->id(), config });
		}
	}
	void MtMarketManager::LoadChargeListTB(bool reload)
	{
		charge_lists_.clear();
		data_table* table = nullptr;
		if (reload) {
			table = data_manager::instance()->on_reload_file("charge_list");
		}
		else {
			table = data_manager::instance()->get_table("charge_list");
		}

		for (auto i = 0; i < table->get_rows(); ++i){
			auto config = boost::make_shared<Config::ChargeList>();
			auto row = table->find_row(i);
			FillMessageByRow(*config, *row);

			charge_lists_[config->channel_id()][config->product_id()] = config->product_index();
/*			auto iter = charge_lists_.find(config->channel_id());
			if (iter != charge_lists_.end()) {				
				charge_lists_[config->channel_id()][config->product_id()] = config->product_index();
			}
			else {
				charge_lists_.insert({ config->channel_id(), { config->product_id(),config->product_index() } });
			}*/			
		}
	}

	zxero::int32 MtMarketManager::OnLoad()
	{		
		
		LoadMarketTB();
		LoadMarketItemTB();
		LoadChargeStoreTB();
		LoadChargeListTB();		

		return 0;
	}

	const boost::shared_ptr<Config::MarketConfig> MtMarketManager::FindMarket(int32 config_id)
	{
		auto it = market_configs_.find(config_id);
		if (it == market_configs_.end()) {
			return nullptr;
		}
		return it->second;
	}

	const boost::shared_ptr<Config::MarketItemConfig> MtMarketManager::FindItem(int32 config_id)
	{
		auto it = item_configs_.find(config_id);
		if (it == item_configs_.end()) {
			return nullptr;
		}
		return it->second;
	}

	const boost::shared_ptr<Config::MarketItemConfig> MtMarketManager::FindItemByItemId(int32 config_id)
	{
		for (auto child : item_configs_) {
			if (child.second->item_id() == config_id) {
				return child.second;
			}
		}
		return nullptr;
	}



	const boost::shared_ptr<Config::ChargeItemConfig> MtMarketManager::FindChargeItem(int32 cfgid)
	{
		auto iter = charge_configs_.find(cfgid);
		if (iter != charge_configs_.end()){
			return iter->second;
		}
		else{
			return nullptr;
		}
	}

	int32 MtMarketManager::FindChargeIndex(const std::string &channel_id, const std::string& product_id)
	{
		auto iter = charge_lists_.find(channel_id);
		if (iter != charge_lists_.end()) {
			auto iter2 = iter->second.find(product_id);
			if (iter2 != iter->second.end()) {
				return iter2->second;
			}
		}
		return -1;
	}

	std::string MtMarketManager::FindChargeProductId(const std::string &channel_id, int32 index)
	{
		auto iter = charge_lists_.find(channel_id);
		if (iter != charge_lists_.end()) {
			for (auto child : iter->second) {
				if (child.second == index)
				{
					return child.first;
				}
			}
		}
		return "";
	}
	
	void MtMarketManager::GenMarketItem(const Config::MarketType market_type,const boost::shared_ptr<MtPlayer>& player, std::vector<int>& results)
	{
		auto config = FindMarket(market_type);
		if (config == nullptr)
		{
			ZXERO_ASSERT(false) << "invalid market data ! market id=" << market_type;
			return;
		}

		if (config->type() == 0) {
			GenStaticMarketItem(config, player, results);
		}
		else {
			for (int32 i = 0; i < Config::MarketItemGroup::CountGroupMax; i++){
				GenRandomMarketItem(config, (const Config::MarketItemGroup)i, player, results);
			}
		}		
	}

	const boost::shared_ptr<Config::MarketItemConfig> MtMarketManager::FindItemByItemIdEx(const Config::MarketType market_type, const int32 config_id)
	{
		for (auto child : item_configs_) {
			if (child.second->item_id() == config_id && (Config::MarketType)(child.second->market_id()) == market_type) {
				return child.second;
			}
		}
		return nullptr;
	}

	bool MtMarketManager::LuaBuyCheck(const Config::MarketType market_type, const int32 config_id, MtPlayer* player, const int32 buy)
	{
		if (player == nullptr) {
			return false;
		}
		
		auto market = player->FindMarket(market_type);
		if (market == nullptr) {
			return false;
		}
		auto mitem = FindItemByItemIdEx(market_type, config_id);
		if (mitem == nullptr) {
			return false;
		}

		return BuyCheck(market, mitem, player, buy);

	}

	bool MtMarketManager::BuyCheck(const boost::shared_ptr<MtMarket> market,
		const boost::shared_ptr<Config::MarketItemConfig> item, 
		MtPlayer* player,
		const int32 buy)
	{
		if (player == nullptr) {
			return false;
		}
		if (market == nullptr || item == nullptr || player == nullptr)
		{
			player->SendClientNotify("InvalidConfigData", -1, -1);
			return false;
		}
		
		auto check_level = player->PlayerLevel();
		if (market->GetConfig()->market_id() == Config::MarketType::MarketType_Union) {//工会商店
			auto guildid = player->GetGuildID();
			if (guildid != INVALID_GUID) {
				check_level = player->GetPlayerGuildProxy()->GetBuildLevel(Packet::GuildBuildType::GUILD_SHOP);
			}
			else {
				return false;
			}
		}
		if (item->open() != 1
			|| item->open_level() > check_level
			|| item->off_level() <= check_level
			|| item->market_id() != market->GetConfig()->market_id()) {
			player->SendClientNotify("LevelLimit", -1, -1);
			return false;
		}
	
		//当前商店是否有这个道具
		auto buy_count = market->ItemCheck(item->id());
		if (buy_count < 0) {
			player->SendClientNotify("InvalidConfigData", -1, -1);
			return false;
		}

		auto limit_count = item->day_limit_count();
		if (limit_count > 0 && (buy_count + buy) > item->day_limit_count()) {
			player->SendClientNotify("ac_notify_018", -1, -1);
			return false;
		}

		return true;
	}

	void MtMarketManager::GenStaticMarketItem(const boost::shared_ptr<Config::MarketConfig> config, const boost::shared_ptr<MtPlayer>& player, std::vector<int>& results)
	{
		player;
		//策划需要无视等级看到东西
		//auto check_level = player->PlayerLevel();
		//if (config->market_id() == Config::MarketType::MarketType_Union) {//工会商店
		//	auto guildid = player->GetGuildID();
		//	if (guildid != INVALID_GUID) {
		//		check_level = player->GetPlayerGuildProxy()->GetBuildLevel(Packet::GuildBuildType::GUILD_SHOP);
		//	}
		//	else{
		//		return;
		//	}
		//}
		for (auto & iter : item_configs_) {
			if (iter.second->market_id() != config->market_id()
				|| iter.second->open() != 1
/*				|| iter.second->open_level() > check_level
				|| iter.second->off_level() <= check_level*/)
			{
				continue;
			}

			results.push_back(iter.second->id());
		}
	}
	
	void MtMarketManager::GenRandomMarketItem(const boost::shared_ptr<Config::MarketConfig> config, const Config::MarketItemGroup group, const boost::shared_ptr<MtPlayer>& player, std::vector<int>& results)
	{
		int32 weight = 0;
		std::vector<boost::shared_ptr<Config::MarketItemConfig>> items;

		auto check_level = player->PlayerLevel();
		if (config->market_id() == Config::MarketType::MarketType_Union) {//工会商店
			auto guildid = player->GetGuildID();
			if (guildid != INVALID_GUID) {
				check_level = player->GetPlayerGuildProxy()->GetBuildLevel(Packet::GuildBuildType::GUILD_SHOP);
			}
			else {
				check_level = 1;

			}
		}

		for (auto & iter : item_configs_)
		{
			if (iter.second->market_id() != config->market_id()
				|| iter.second->open() != 1
				|| iter.second->open_level() > check_level
				|| iter.second->off_level() <= check_level
				|| iter.second->count_group() != group)
			{
				continue;
			}

			weight += iter.second->gen_weight();
			items.push_back(iter.second);
		}

		if (items.size() <= 0)
		{
			return;
		}
		
		std::vector<int32> group_ids;
		for (auto guid : config->count_group())
		{
			group_ids.push_back(guid);
		}

		if ((uint32)group < 0 || (uint32)group >= group_ids.size())
		{
			ZXERO_ASSERT(false) << "invalid group type !" << group;
			return;
		}

		//gen
		int32 base_weight = 0;
		int32 seed = 0;
		if (weight <= 0) {
			ZXERO_ASSERT(false) << "invalid weight value=" << weight;
			return;
		}
		for (int32 i = 0; i < group_ids[group]; i++){
			seed = (int32)rand_gen->intgen(1,weight);
			base_weight = 0;
			for (uint32 k = 0; k < items.size(); k++){
				base_weight += items[k]->gen_weight();
				if (seed <= base_weight){
					results.push_back(items[k]->id());
					//权重移除
					weight -= items[k]->gen_weight();
					//抽牌移除
					items.erase(items.begin()+k);
					break;
				}
			}
		}
	}

	//------------------------------------------------------

	void MtMarket::lua_reg(lua_State* ls)
	{
		ff::fflua_register_t<MtMarket, ctor()>(ls, "MtMarket")
			.def(&MtMarket::GetType, "GetType")
			.def(&MtMarket::UpdateItem, "UpdateItem")
			.def(&MtMarket::ItemCheck, "ItemCheck")
			.def(&MtMarket::Refresh, "Refresh")
			.def(&MtMarket::IsStatic, "IsStatic")
			.def(&MtMarket::Enable,"Enable")
			.def(&MtMarket::EnableItem, "EnableItem")
			.def(&MtMarket::GetConfig, "GetConfig")
			;
	}

	MtMarket::MtMarket(MtPlayer& player, const boost::shared_ptr<Packet::Market>& db_info)
		: player_(player.weak_from_this())
		, db_info_(db_info)
	{	
		GenItems();
	}

	MtMarket::MtMarket(const MtMarket& rhs)
		: player_(rhs.player_)
		, db_info_(rhs.db_info_)
	{
		GenItems();
	}

	void MtMarket::GenItems()
	{
		auto player = player_.lock();
		if (player == nullptr) {
			ZXERO_ASSERT(false);
			return;
		}
	
		std::vector<int32> ids;
		std::vector<int32> buys;
		//int转换
		utils::split_string(db_info_->market_items().c_str(), "|", ids);
		utils::split_string(db_info_->buys_count().c_str(), "|", buys);

		//不能早于split
		Refresh(true);

		if (ids.size() == buys.size()) {
			//检查配置数据是否有变动
			for (auto &child : items_) {
				for (uint32 i = 0; i < ids.size(); i++) {
					if (child.first == ids[i]) {
						child.second = buys[i];
						break;
					}
				}
			}
			SycnSaveCache();
		}
		else {
			ZXERO_ASSERT(false) << "market data invalid ! type=" << db_info_->market_type() << " player guid=" << player->Guid();
		}
	}

	bool MtMarket::UpdateItem(const int32 index, const int32 buy_count )
	{
		auto player = player_.lock();
		if (player == nullptr) {
			ZXERO_ASSERT(false);
			return false;
		}
		if (items_.find(index) == items_.end()) {
			return false;
		}
		
		items_[index] += buy_count;

		SycnSaveCache();

		Packet::UpdatePlayerMarketItem notify;
		notify.set_market_type( db_info_->market_type());
		notify.set_market_item_id(index);
		notify.set_buy_count(items_[index]);
		player->SendMessage(notify);

		return true;
	}
	
	void MtMarket::OnCache( ) {
		auto player = player_.lock();
		if (player == nullptr) {
			ZXERO_ASSERT(false);
			return;
		}
		player->Cache2Save(db_info_.get());
	}

	void MtMarket::SycnSaveCache()
	{

		if (GetType() == Config::MarketType::MarketType_Equip) {
			//不需要存储
			return;
		}
		//更新存储缓存
		std::stringstream idresult;
		std::stringstream buyresult;

		std::vector<int32> ids;
		std::vector<int32> buys;
		for (auto child : items_) {
			ids.push_back(child.first);
			buys.push_back(child.second);
		}
		std::copy(ids.begin(), ids.end(), std::ostream_iterator<int>(idresult, "|"));
		db_info_->set_market_items(idresult.str());

		std::copy(buys.begin(), buys.end(), std::ostream_iterator<int>(buyresult, "|"));
		db_info_->set_buys_count(buyresult.str());
	}

	void MtMarket::FillMessage(Packet::PlayerMarketReply& message)
	{
		message.set_market_type(db_info_->market_type());
		for (auto& item : items_)
		{
			message.add_market_items(item.first);
			message.add_buy_counts(item.second);
		}
		message.set_refresh_cd(db_info_->refresh_cd());

	}

	int32 MtMarket::ItemCheck(const int32 coinfig_id)
	{		
		if (items_.find(coinfig_id) == items_.end()) {
			return -1;
		}
		return items_[coinfig_id];
	}

	bool MtMarket::Enable(bool enable, bool init)
	{		
		auto player = player_.lock();
		if (player == nullptr) {
			ZXERO_ASSERT(false);
			return false;
		}
		enable_ = enable;
		if (init) {
			return true;
		}
		if (enable) {			
			Refresh(true);
			player->SendLuaPacket("OpenMarket", { GetType() }, {}, {});			
		}
		else {
			Clear();
			player->SendLuaPacket("CloseMarket", { GetType() }, {}, {});			
		}
		return true;
	}

	const boost::shared_ptr<Config::MarketConfig> MtMarket::GetConfig()
	{
		return MtMarketManager::Instance().FindMarket((int32)db_info_->market_type());
	}

	bool MtMarket::EnableItem(int32 index, bool enable)
	{
		auto item_config = MtMarketManager::Instance().FindItem(index);
		if (item_config == nullptr) {
			return false;
		}

		if (enable) {
			items_[item_config->id()] = 0;
		}
		else {
			auto iter = items_.find(item_config->id());
			if (iter != items_.end()) {
				items_.erase(iter);
			}
		}

		SycnSaveCache();
		RefreshData2Client();

		return true;

	}

	void MtMarket::Clear()
	{
		items_.clear();
		SycnSaveCache();
		RefreshData2Client();
	}

	int64 MtMarket::GetRefreshCD() 
	{
		auto config = GetConfig();
		if (config == nullptr) {
			return -1;
		}
		return config->refresh_cd();
	}

	void MtMarket::OnRefreshTime(const int32 delta)
	{
		if (GetRefreshCD() < 0) {
			return;
		}

		int64 tick = db_info_->refresh_cd();
		tick -= (int64)delta;
		if(tick <= 0){
			Refresh();
		}
		else {
			db_info_->set_refresh_cd(tick);
		}	
	}

	void MtMarket::ResetBuyCount()
	{
		for (auto& child : items_) {
			child.second = 0;
		}
		SycnSaveCache();
		RefreshData2Client();
	}

	bool MtMarket::IsOpen()
	{
		auto config = GetConfig();
		if (config == nullptr) {
			return true;
		}
		return (config->open() == 1);
	}

	bool MtMarket::IsStatic()
	{
		auto config = GetConfig();
		if (config == nullptr) {
			return true;
		}
		return (config->type() == 0);
	}

	void MtMarket::Refresh(bool force_clean)
	{
		auto player = player_.lock();
		if (player == nullptr) {
			ZXERO_ASSERT(false);
			return;
		}
		if (!IsOpen()) {
			return;
		}
		std::vector<int32> temp;
		if (IsStatic()){
			if (force_clean) {
				items_.clear();
			}
			MtMarketManager::Instance().GenMarketItem(db_info_->market_type(), player, temp);
			bool dirty = false;
			if (items_.size()>60 && Config::MarketType::MarketType_Equip != GetType() ) {
				ZXERO_ASSERT(false) << "market item is more! type = "<< db_info_->guid() << " player guid=" << player->Guid();
				return;
			}
			for (auto child : temp) {
				if (items_.find(child) == items_.end()) {					
					items_.insert({ child,0 });
					dirty = true;
				}
			}
			if (dirty) {
				SycnSaveCache();
				RefreshData2Client();
			}
		}
		else {
			db_info_->set_refresh_cd(GetRefreshCD());
			MtMarketManager::Instance().GenMarketItem(db_info_->market_type(), player, temp);

			items_.clear();
			for (auto child : temp) {
				items_.insert({ child,0 });
			}
			SycnSaveCache();
			RefreshData2Client();
		}
	}

	void MtMarket::RefreshData2Client()
	{
		auto player = player_.lock();
		if (player == nullptr) {
			ZXERO_ASSERT(false);
			return;
		}
		if (enable_) 
		{
			Packet::PlayerMarketReply message;
			FillMessage(message);
			player->SendMessage(message);
		}
	}
}