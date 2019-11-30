#ifndef MTONLINE_GAMESERVER_MT_MARKET_H__
#define MTONLINE_GAMESERVER_MT_MARKET_H__
#include "../mt_types.h"
#include "ItemAndEquip.pb.h"
#include <vector>


namespace Mt
{

	class MtMarketManager : public boost::noncopyable
	{
	public:
		static void lua_reg(lua_State* ls);
	public:
		static MtMarketManager& Instance();
		MtMarketManager();
		zxero::int32 OnLoad();
		bool LoadPlayerMarket(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<zxero::dbcontext>& dbctx);
		const boost::shared_ptr<Config::MarketConfig> FindMarket(int32 config_id);
		const boost::shared_ptr<Config::MarketItemConfig> FindItem(int32 config_id);
		const boost::shared_ptr<Config::MarketItemConfig> FindItemByItemId(int32 config_id);
		const boost::shared_ptr<Config::ChargeItemConfig> FindChargeItem(int32 config_id);
		const boost::shared_ptr<Config::MarketItemConfig> FindItemByItemIdEx(const Config::MarketType market_type, const int32 config_id);
		int32 FindChargeIndex(const std::string &channel_id, const std::string& product_id);
		std::string FindChargeProductId(const std::string &channel_id, int32 index);
		void GenMarketItem(const Config::MarketType market_type, const boost::shared_ptr<MtPlayer>& player, std::vector<int>& results);
		bool LuaBuyCheck(const Config::MarketType market_type, const int32 config_id, MtPlayer* player, const int32 buy);
		bool BuyCheck(const boost::shared_ptr<MtMarket> market, const boost::shared_ptr<Config::MarketItemConfig> item, MtPlayer* player, const int32 buy);

		void LoadMarketTB(bool reload = false);
		void LoadMarketItemTB(bool reload = false);
		void LoadChargeStoreTB(bool reload = false);
		void LoadChargeListTB(bool reload = false);

	private:
		void GenRandomMarketItem(const boost::shared_ptr<Config::MarketConfig> config, const Config::MarketItemGroup group, const boost::shared_ptr<MtPlayer>& player, std::vector<int>& results );
		void GenStaticMarketItem(const boost::shared_ptr<Config::MarketConfig> config, const boost::shared_ptr<MtPlayer>& player, std::vector<int>& results);

	private:
		std::map<int32, boost::shared_ptr<Config::MarketConfig>> market_configs_;
		std::map<int32, boost::shared_ptr<Config::MarketItemConfig>> item_configs_;
		std::map<int32, boost::shared_ptr<Config::ChargeItemConfig>> charge_configs_;
		std::map<std::string, std::map<std::string,int32>> charge_lists_; //channelid productid index		
	};

	class MtMarket : public boost::noncopyable, public boost::enable_shared_from_this<MtMarket>
	{
	public:
		static void lua_reg(lua_State* ls);
	public:
		MtMarket(MtPlayer& player, const boost::shared_ptr<Packet::Market>& db_info);
		MtMarket(const MtMarket& rhs);
		MtMarket() {};
		~MtMarket() {};

		Config::MarketType GetType() const { return db_info_->market_type(); }
		bool UpdateItem(const int32 index,const int32 buy_count);
		void OnCache( );
		void FillMessage(Packet::PlayerMarketReply& message);
		const boost::shared_ptr<MtPlayer> Player();
		
		const std::map<int32, int32>& Items() { return items_; }
		void SycnSaveCache();
		void OnRefreshTime(const int32 delta);
		void ResetBuyCount();
		void Refresh(bool force_clean = false);
		void RefreshData2Client();
		bool IsOpen();
		bool IsStatic();
		bool Enable(bool enable,bool init =false);
		bool EnableItem(int32 config_id, bool enable);
		int32 ItemCheck(const int32 coinfig_id);
		const boost::shared_ptr<Config::MarketConfig> GetConfig();
	private:
		void GenItems();
		void Clear();
		int64 GetRefreshCD();
	private:
		const boost::weak_ptr<MtPlayer> player_;
		const boost::shared_ptr<Packet::Market> db_info_;
		std::map<int32,int32> items_;
		bool enable_;
	};

}
#endif // MTONLINE_GAMESERVER_MT_MARKET_H__
