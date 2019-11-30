#ifndef MTONLINE_GAMESERVER_MT_CONTAINER_H__
#define MTONLINE_GAMESERVER_MT_CONTAINER_H__
#include "../mt_types.h"
#include "ItemAndEquip.pb.h"
#include <vector>

namespace Mt
{

	class ContainerManager : public boost::noncopyable
	{
	public:
		static ContainerManager& Instance();
		ContainerManager();
		bool LoadPlayerContainer(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<zxero::dbcontext>& dbctx);
	private:
		const zxero::int32 max_equipments = 21 * 5;
		const zxero::int32 init_bag_size = 500;
		
	};

	class MtContainer : public boost::noncopyable, public boost::enable_shared_from_this<MtContainer>
	{
	public:
		static void lua_reg(lua_State* ls);
	public:
		MtContainer(){};
		MtContainer(MtPlayer& player,
			const boost::shared_ptr<Packet::Container>& db_info,
			const Packet::ContainerType type,
			const int32 max_size,
			const int32 max_crystal_open_size,
			const int32 max_item_open_size);
		zxero::uint64 Guid() const;		
		bool AddItem(const boost::shared_ptr<MtItemBase>& item, Config::ItemAddLogType type= Config::ItemAddLogType::AddType_Gm, bool overlay = true, bool ignore_size = false );
		bool AddItemEx(const int32 config_id, const int32 count, Config::ItemAddLogType type = Config::ItemAddLogType::AddType_Gm);
		int32 CheckBlockCost(const int32 config_id, const int32 count);
		void LoadItem(const boost::shared_ptr<MtItemBase>& item);
		bool RemoveItem(const boost::shared_ptr<MtItemBase>& item,bool is_delete=true, bool notify = true);
		void OnSave(const boost::shared_ptr<zxero::dbcontext>& dbctx);
		void OnCache( );
		void FillMessage(Packet::PlayerContainerReply& message);
		boost::weak_ptr<MtPlayer> Player();
		const boost::shared_ptr<MtItemBase> FindItemByGuid(zxero::uint64 guid);
		const boost::shared_ptr<MtItemBase> FindItemByConfigId(int32 config_id);
		std::vector<boost::shared_ptr<MtItemBase>>& Items() { return items_; }
		int32 Size() const { return items_.size(); }
		int32 Capacity() const { return db_info_->max_block();}
		const Packet::ContainerType Type() const { return type_; }
		int32 LeftSize() const { return db_info_->max_block() - items_.size(); }
		void Reset();
		int32 GetItemCount(int32 config_id);
		bool DelItem(int32 config_id, int32 count);
		bool ExtendCheck(const int32 size, const int32 type);
		bool ExtendSize(const int32 size,const int32 type );
		void OnRefreshTime(const int32 delta);
	private:
		boost::weak_ptr<MtPlayer> player_;
		Packet::ContainerType type_;
		boost::shared_ptr<Packet::Container> db_info_;
		std::vector<boost::shared_ptr<MtItemBase>> items_;

		int32 max_size_;  //扩容后能达到的最大空间
		int32 max_crystal_open_size_;  //钻石扩容后能达到的最大空间
		int32 max_item_open_size_;	//道具扩容后能达到的最大空间
	};
}
#endif // MTONLINE_GAMESERVER_MT_CONTAINER_H__
