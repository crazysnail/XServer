#ifndef MTONLINE_GAMESERVER_MT_ITEM_H__
#define MTONLINE_GAMESERVER_MT_ITEM_H__
#include "../mt_types.h"
#include <ItemAndEquip.pb.h>
#include <map>

namespace Mt
{
	const static int32 MAX_SUIT_ATTR_COUNT = 4;					//套装最大附加属性数
	
	enum MtItemType
	{
		INVALID_ITEM = 0,
		NORMAL_ITEM = 1,
		GEM_ITEM = 2,
		EQUIP_ITEM = 3,
	};

	class MtItemManager : public boost::noncopyable
	{
	public:
		static void lua_reg(lua_State* ls);
	public:
		static MtItemManager& Instance();
		MtItemManager();
		zxero::int32 OnLoad();

		//物品具体分类
		const Config::ItemType GetItemType(int32 config_id);
		//物品名称
		std::string GetItemName(int32 config_id);
		//容器归属类型判定
		Packet::ContainerType CheckOwnerContainer(const int32 config_id);
		//物品实例分类
		const MtItemType GetItemInstanceType(const int32 config_id);

		const int32 GetMaxBlcokCount(const int32 config_id);

		const boost::shared_ptr<Config::ItemConfig> GetItemConfig(int32 id)
		{
			if (item_configs_.find(id) == std::end(item_configs_))
				return nullptr;
			return item_configs_[id];
		}
		const boost::shared_ptr<Config::EquipmentConfig> GetEquipmentConfig(int32 id)
		{
			if (equip_configs_.find(id) == std::end(equip_configs_))
				return nullptr;
			return equip_configs_[id];
		}
		const boost::shared_ptr<Config::GemConfig> GetGemConfig(int32 id) {
			if (gem_configs_.find(id) == std::end(gem_configs_)) {
				return nullptr;
			}
			return gem_configs_[id];
		}
		const boost::shared_ptr<Config::GemCostConfig> GetGemCostConfig(int32 level) {
			if (gem_cost_configs_.find(level) == std::end(gem_cost_configs_)) {
				return nullptr;
			}
			return gem_cost_configs_[level];
		}

		const boost::shared_ptr<Config::EquipmentSuitConfig> GetEquipmentSuitConfig(int32 id){
			if (suit_configs_.find(id) == std::end(suit_configs_))
				return nullptr;
			return suit_configs_[id];
		}

		const boost::shared_ptr<Config::EnhanceMasterConfig> GetEnhanceMasterConfig(int32 level) {
			if (enhance_configs_.find(level) == std::end(enhance_configs_))
				return nullptr;
			return enhance_configs_[level];
		}

		int32 GetEquipBookIndex(int32 config_id) {
			auto iter = equip_book_indexs_.find(config_id);
			if (iter == std::end(equip_book_indexs_))
				return -1;
			return iter->second;
		}

		std::vector<int32> GetSuitEquips(int32 suidId) {			
			if (suit_equip_set_.find(suidId) == std::end(suit_equip_set_)) {
				std::vector<int32> temp;
				return temp;
			}
			return suit_equip_set_[suidId];
		}

		const boost::shared_ptr<MtItemBase> CreateItem(
			MtPlayer* player, zxero::int32 config_id, zxero::int32 count = 1);
		int32 GetSuitCount(int32 suitId);
		boost::shared_ptr<Packet::SimpleImpactConfig> ImpactType2Config(const int32 impact_type);

		//
		void LoadItemTB(bool reload = false);
		void LoadEquipTB(bool reload = false);
		void LoadGemTB(bool reload = false);
		void LoadSimpleImpacktTB(bool reload = false);
		void LoadBookTB(bool reload = false);
	private:
		void CalcSuitEquipCount( const boost::shared_ptr<Config::EquipmentConfig> &equip_config );
	private:
		std::map<zxero::int32, boost::shared_ptr<Config::ItemConfig>> item_configs_;
		std::map<zxero::int32, boost::shared_ptr<Config::EquipmentConfig>> equip_configs_;
		std::map<zxero::int32, boost::shared_ptr<Config::GemConfig>> gem_configs_;
		std::map<zxero::int32, boost::shared_ptr<Config::GemCostConfig>> gem_cost_configs_;
		std::map<zxero::int32, boost::shared_ptr<Config::EquipmentSuitConfig>> suit_configs_;
		std::map<zxero::int32, boost::shared_ptr<Config::EnhanceMasterConfig>> enhance_configs_;
		std::map<int32, std::vector<int32>> suit_equip_set_;
		std::map<int32, boost::shared_ptr<Packet::SimpleImpactConfig>> simple_impacts_;
		std::map<int32, int32> equip_book_indexs_;

	};

	class MtItemBase : public boost::noncopyable, public boost::enable_shared_from_this<MtItemBase>
	{
	public:
		static void lua_reg(lua_State* ls);
	public:
		MtItemBase(const boost::shared_ptr<MtContainer>& container);
		MtItemBase() {}
		virtual ~MtItemBase() {}
		const boost::shared_ptr<MtContainer> Container();
		virtual void AfterCreate() const {}
		virtual MtItemType Type() const = 0;
		virtual void OnSave(const boost::shared_ptr<zxero::dbcontext>& dbctx) = 0;
		virtual google::protobuf::Message* DbInfo() const = 0;
		virtual google::protobuf::Message* Config() const = 0;
		virtual void OnCache( ) = 0;
		virtual int32 Count() const = 0;
		virtual bool CanOverlay() const { return false; }
		virtual int32 MaxBlockCount() const  { return 1;}
		virtual int32 ConfigId() const = 0;
		virtual std::string GetName() const = 0;
		virtual std::string GetColor() const = 0;
		virtual int32 AddCount(zxero::int32 count) = 0;
		virtual int32 DecCount(zxero::int32 count) = 0;
		virtual bool OnAdd2Container(const boost::shared_ptr<MtContainer>& container);
		virtual void FillMessage(Packet::ClientItemInfo& info) const = 0;
		virtual void SetContainerGuid(zxero::uint64 guid) = 0;
		virtual uint64 Guid() const = 0;
		virtual void RefreshData2Client() const;
		virtual void EnableLock(bool lock) = 0;
		virtual bool IsLock() const = 0;
		virtual void GatherGemId(std::vector<int32>& /*result*/) const {}
		virtual std::string GenHyperStr();
	protected:
		boost::weak_ptr<MtContainer> container_;
	};

	class MtNormalItem : public MtItemBase
	{
	public:
		static void lua_reg(lua_State* ls);
	public:
		MtNormalItem(const boost::shared_ptr<MtContainer>& container, const boost::shared_ptr<Packet::NormalItem>& db_info)
			:MtItemBase(container),db_info_(db_info) {}
		MtNormalItem(const boost::shared_ptr<Packet::NormalItem>& db_info)
			:db_info_(db_info) {}
		MtNormalItem() {};
		virtual MtItemType Type() const override { return MtItemType::NORMAL_ITEM; }
		virtual void OnSave(const boost::shared_ptr<zxero::dbcontext>& dbctx) override;
		virtual void OnCache( ) override;
		virtual zxero::int32 Count() const override;
		virtual bool CanOverlay() const override;
		virtual int32 MaxBlockCount() const override;
		virtual zxero::int32 ConfigId() const override;	
		virtual std::string GetName() const;
		virtual std::string GetColor() const;
		virtual bool OnAdd2Container(const boost::shared_ptr<MtContainer>& container) override;
		virtual zxero::int32 AddCount(zxero::int32 count) override;
		virtual zxero::int32 DecCount(zxero::int32 count) override;
		virtual void FillMessage(Packet::ClientItemInfo& info) const override;
		
		virtual zxero::uint64 Guid() const override;
		virtual void SetContainerGuid(zxero::uint64 guid) override;
		virtual Packet::ResultCode OnUse(const boost::shared_ptr<MtActor> actor_target =nullptr);
		virtual Packet::NormalItem* DbInfo() const override;
		Config::ItemConfig* Config() const;
		virtual void EnableLock(bool lock);
		virtual bool IsLock() const;
		virtual void RefreshData2Client() const;
		
	private:
		const boost::shared_ptr<Packet::NormalItem> db_info_;
	};

	class MtEquipItem : public MtItemBase
	{
	public:
		static void lua_reg(lua_State* ls);
	public:
		MtEquipItem(const boost::shared_ptr<MtContainer>& container, const boost::shared_ptr<Packet::EquipItem>& db_info)
			:MtItemBase(container), db_info_(db_info) {	 }
		MtEquipItem(const boost::shared_ptr<Packet::EquipItem>& db_info)
			: db_info_(db_info) {  }
		MtEquipItem() {};
		virtual MtItemType Type() const override { return MtItemType::EQUIP_ITEM; }
		virtual void OnSave(const boost::shared_ptr<zxero::dbcontext>& dbctx) override;
		virtual void OnCache( ) override;
		virtual zxero::int32 Count() const override;
		virtual zxero::int32 ConfigId() const override;
		virtual std::string GetName() const override;
		virtual std::string GetColor() const override;
		int32 EnhenceLevel() const;
		void SetEnhenceLevel(int32 level);
		Config::EquipmentConfig* Config() const;
		virtual zxero::int32 AddCount(zxero::int32 /*count*/);
		virtual zxero::int32 DecCount(zxero::int32 /*count*/);
		bool SameType(const boost::shared_ptr<MtEquipItem>& rhs);
		void ActorGuid(zxero::uint64 guid);
		zxero::uint64 ActorGuid() const;
		uint64 GetBindActorGuid() const;
		void SetBindActorGuid(uint64 guid);
		Config::EquipmentSlot Slot() const;
		virtual void AfterCreate() const override { UpdateEquipScore(); }
		void Slot(Config::EquipmentSlot slot);
		virtual bool OnAdd2Container(const boost::shared_ptr<MtContainer>& container) override;
		virtual void FillMessage(Packet::ClientItemInfo& info) const override;
		void FillEquipInfo(Packet::EquipItem& info) const;
		virtual zxero::uint64 Guid() const override;
		void EquipEffect(attribute_set& changes) const;
		void UpdateEquipScore() const;
		virtual void SetContainerGuid(zxero::uint64 guid) override;
		bool StripGem(int32 config_id, int32 index);
		bool InsetGem(int32 config_id, int32 index);
		bool CanInsetGem(int32 index);
		int32 GemCount() const;
		bool HasGem(int32 config_id, int32 index);
		virtual Packet::EquipItem* DbInfo() const override;
		/**
		* \brief 已经拥有的附魔类型
		* \return std::vector<std::string>
		*/
		std::vector<std::string> OldEnchantsTypes() const;
		std::vector<Packet::EquipItem_EquipEnchant*> OpenedEnchants(const std::vector<int32>& slot_ids,bool effect = false);
		Packet::EquipItem_EquipEnchant* Enchant(int32 index);
		int32 OpenEnchantSlotSize() const;
		int32 GetMagicalResId() const;
		bool InheritFrom(const MtEquipItem& old_equip, MtPlayer& player);
		void MagicalFrom(const int32 magical_res_id);
		void CacheBatchEnchantInfo(const Packet::BatchEnchantEquipReply& info);
		bool HasBatchEnchantCache() const;
		void ApplyBatchEnchantCache(int32 index);
		bool AutoRefixEquipDurable(bool simulate /*= false*/);
		bool CheckRace(const Packet::Race r);
		/**
		* \brief 消耗装备耐久值
		* \param cost
		* \param simulate 默认false, 
		* \retval true 装备已损坏
		* \retval false 装备还能用
		*/
		bool DurableCost(real32 cost, bool simulate = false);
		bool CheckProf(const Packet::Professions p);
		bool CheckTalent(const Packet::Talent t);
		virtual void EnableLock(bool lock);
		virtual void GatherGemId(std::vector<int32>& result) const override;
		virtual bool IsLock() const;
		virtual void RefreshData2Client() const;
	private:
		const boost::shared_ptr<Packet::EquipItem> db_info_;
		boost::shared_ptr<Packet::BatchEnchantEquipReply> batch_enchant_cache_;
	};

	class MtGemItem : public MtItemBase
	{
	public:
		static void lua_reg(lua_State* ls);
	public:
		MtGemItem(const boost::shared_ptr<MtContainer>& container, const boost::shared_ptr<Packet::GemItem>& db_info)
			:MtItemBase(container), db_info_(db_info){}
		MtGemItem(const boost::shared_ptr<Packet::GemItem>& db_info)
			:db_info_(db_info) {}
		MtGemItem() {};
		virtual MtItemType Type() const override { return MtItemType::GEM_ITEM; }
		virtual void OnSave(const boost::shared_ptr<zxero::dbcontext>& dbctx) override;
		virtual void OnCache( ) override;
		virtual zxero::int32 Count() const override;
		virtual bool CanOverlay() const override;
		virtual int32 MaxBlockCount() const override;
		virtual zxero::int32 ConfigId() const override;
		virtual std::string GetName() const override;
		virtual std::string GetColor() const override;
		virtual zxero::int32 AddCount(zxero::int32 count)  override;
		virtual zxero::int32 DecCount(zxero::int32 /*count*/) override;
		virtual void FillMessage(Packet::ClientItemInfo& info) const  override;
		void FillGemInfo(Packet::GemItem& gem_info);
		virtual zxero::uint64 Guid() const override;
		virtual void SetContainerGuid(zxero::uint64 guid) override;
		uint64 EquipGuid() const;
		void EquipGuid(uint64 equip_guid);
		Config::GemConfig* Config() const;
		virtual void EnableLock(bool lock);
		virtual bool IsLock() const;
		virtual Packet::GemItem* DbInfo() const override;
		virtual void RefreshData2Client() const;
	private:
		boost::shared_ptr<Packet::GemItem> db_info_;
	};



}

#endif // MTONLINE_GAMESERVER_MT_ITEM_H__
