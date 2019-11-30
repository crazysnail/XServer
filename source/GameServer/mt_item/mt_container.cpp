#include "mt_item.h"
#include "mt_container.h"
#include "module.h"
#include "../mt_guid/mt_guid.h"
#include "../mt_player/mt_player.h"
#include "../mt_server/mt_server.h"
#include "../mt_config/mt_config.h"
#include "../base/mt_db_save_work.h"
#include "../mt_config/mt_config.h"
#include "../mt_cache/mt_shm_manager.h"
#include "active_model.h"
#include <ItemAndEquip.pb.h>
#include <ItemConfig.pb.h>
#include <BaseConfig.pb.h>
#include <SceneCommon.pb.h>
#include "../friend/FriendContainer.h"
#include "../mail/MessageContainer.h"
#include "../mail/static_message_manager.h"
#include "../mt_chat/mt_chatmessage.h"

namespace Mt
{
	static ContainerManager* __container_manager = nullptr;
	REGISTER_MODULE(ContainerManager)
	{
		require("MtItemManager");
	}

	bool ContainerManager::LoadPlayerContainer(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<zxero::dbcontext>& dbctx)
	{
		auto equip_slot_config = MtConfigDataManager::Instance().FindConfigValue("init_equip_block_size");
		auto itembag_config = MtConfigDataManager::Instance().FindConfigValue("init_itembag_size");
		auto equipbag_config = MtConfigDataManager::Instance().FindConfigValue("init_equipbag_size");
		auto missionbag_config = MtConfigDataManager::Instance().FindConfigValue("init_missionbag_size");
		auto bank_config = MtConfigDataManager::Instance().FindConfigValue("init_bank_size");
		auto hook_config = MtConfigDataManager::Instance().FindConfigValue("init_hookbag_size");
		if (equip_slot_config == nullptr || itembag_config == nullptr || equipbag_config == nullptr || missionbag_config == nullptr || bank_config == nullptr ) {
			return false;
		}
		//Equip
		if (ActiveModel::Base<Packet::Container>(dbctx).CheckExist({ { "player_guid", player->Guid() },{ "type", Packet::ContainerType::Layer_Equip } }) == true) {
			auto container_db_info = ActiveModel::Base < Packet::Container>(dbctx).FindOne({ { "player_guid", player->Guid() },{ "type", Packet::ContainerType::Layer_Equip } });
			auto bag = boost::make_shared<MtContainer>(*player, container_db_info, Packet::ContainerType::Layer_Equip, equip_slot_config->value2(),0,0);
			auto equips = ActiveModel::Base<Packet::EquipItem>(dbctx).FindAll({ { "container_guid", bag->Guid() } });
			for (auto& equip : equips)
			{
				bag->LoadItem(boost::make_shared<MtEquipItem>(bag, equip));
			}
			player->SetContainer(Packet::ContainerType::Layer_Equip,bag);
		}
		else {
			auto container_db = boost::make_shared<Packet::Container>();
			container_db->set_guid(MtGuid::Instance()(*container_db));
			container_db->set_player_guid(player->Guid());
			container_db->set_max_block(equip_slot_config->value1());
			container_db->set_type(Packet::ContainerType::Layer_Equip);
			container_db->set_crystal_opened_block(0);
			container_db->set_item_opened_block(0);
			auto bag = boost::make_shared<MtContainer>(*player, container_db, Packet::ContainerType::Layer_Equip, equip_slot_config->value2(),0,0);
			player->SetContainer(Packet::ContainerType::Layer_Equip,bag);
		}

		//ItemBag
		if (ActiveModel::Base<Packet::Container>(dbctx).CheckExist({ {"player_guid", player->Guid()},{"type", Packet::ContainerType::Layer_ItemBag } }) == true) {
			auto container_db_info = ActiveModel::Base<Packet::Container>(dbctx).FindOne({ { "player_guid", player->Guid() },{ "type", Packet::ContainerType::Layer_ItemBag } });
			auto bag = boost::make_shared<MtContainer>(*player, container_db_info, Packet::ContainerType::Layer_ItemBag, itembag_config->value2(), itembag_config->value3(), itembag_config->value4());
			auto items = ActiveModel::Base<Packet::NormalItem>(dbctx).FindAll({ {"container_guid", container_db_info->guid()} });
			for (auto& item : items) {
				bag->LoadItem(boost::make_shared<MtNormalItem>(bag, item));
			}
			auto gem_items = ActiveModel::Base<Packet::GemItem>(dbctx).FindAll({ { "container_guid", container_db_info->guid() } });
			for (auto& item : gem_items) {
				bag->LoadItem(boost::make_shared<MtGemItem>(bag, item));
			}
			player->SetContainer(Packet::ContainerType::Layer_ItemBag,bag);
		} else {
			auto container_db = boost::make_shared<Packet::Container>();
			container_db->set_guid(MtGuid::Instance()(*container_db));
			container_db->set_player_guid(player->Guid());
			container_db->set_max_block(itembag_config->value1());
			container_db->set_type(Packet::ContainerType::Layer_ItemBag);
			container_db->set_crystal_opened_block(0);
			container_db->set_item_opened_block(0);
			auto bag = boost::make_shared<MtContainer>(*player, container_db, Packet::ContainerType::Layer_ItemBag, itembag_config->value2(), itembag_config->value3(), itembag_config->value4());
			player->SetContainer(Packet::ContainerType::Layer_ItemBag, bag);
		}

		//EquipBag
		if (ActiveModel::Base<Packet::Container>(dbctx).CheckExist({ { "player_guid", player->Guid() },{ "type", Packet::ContainerType::Layer_EquipBag } }) == true) {
			auto container_db_info = ActiveModel::Base<Packet::Container>(dbctx).FindOne({ { "player_guid", player->Guid() },{ "type", Packet::ContainerType::Layer_EquipBag } });
			auto bag = boost::make_shared<MtContainer>(*player, container_db_info, Packet::ContainerType::Layer_EquipBag, equipbag_config->value2(), equipbag_config->value3(), equipbag_config->value4());
			auto equip_items = ActiveModel::Base<Packet::EquipItem>(dbctx).FindAll({ { "container_guid", container_db_info->guid() } });
			for (auto& item : equip_items) {
				bag->LoadItem(boost::make_shared<MtEquipItem>(bag, item));
			}
			player->SetContainer(Packet::ContainerType::Layer_EquipBag, bag);
		}
		else {
			auto container_db = boost::make_shared<Packet::Container>();
			container_db->set_guid(MtGuid::Instance()(*container_db));
			container_db->set_player_guid(player->Guid());
			container_db->set_max_block(equipbag_config->value1());
			container_db->set_type(Packet::ContainerType::Layer_EquipBag);
			container_db->set_crystal_opened_block(0);
			container_db->set_item_opened_block(0);
			auto bag = boost::make_shared<MtContainer>(*player, container_db, Packet::ContainerType::Layer_EquipBag, equipbag_config->value2(), equipbag_config->value3(), equipbag_config->value4());
			player->SetContainer(Packet::ContainerType::Layer_EquipBag, bag);
		}

		//MissionBag
		if (ActiveModel::Base<Packet::Container>(dbctx).CheckExist({ { "player_guid", player->Guid() },{ "type", Packet::ContainerType::Layer_MissionBag } }) == true) {
			auto container_db_info = ActiveModel::Base<Packet::Container>(dbctx).FindOne({ { "player_guid", player->Guid() },{ "type", Packet::ContainerType::Layer_MissionBag } });
			auto bag = boost::make_shared<MtContainer>(*player, container_db_info, Packet::ContainerType::Layer_MissionBag, missionbag_config->value2(), missionbag_config->value3(), missionbag_config->value4());
			auto items = ActiveModel::Base<Packet::NormalItem>(dbctx).FindAll({ { "container_guid", container_db_info->guid() } });
			for (auto& item : items) {
				bag->LoadItem(boost::make_shared<MtNormalItem>(bag, item));
			}
			player->SetContainer(Packet::ContainerType::Layer_MissionBag, bag);
		}
		else {
			auto container_db = boost::make_shared<Packet::Container>();
			container_db->set_guid(MtGuid::Instance()(*container_db));
			container_db->set_player_guid(player->Guid());
			container_db->set_max_block(missionbag_config->value1());
			container_db->set_type(Packet::ContainerType::Layer_MissionBag);
			container_db->set_crystal_opened_block(0);
			container_db->set_item_opened_block(0);
			auto bag = boost::make_shared<MtContainer>(*player, container_db, Packet::ContainerType::Layer_MissionBag, missionbag_config->value2(), missionbag_config->value3(), missionbag_config->value4());
			player->SetContainer(Packet::ContainerType::Layer_MissionBag, bag);
		}

		//Bank
		if (ActiveModel::Base<Packet::Container>(dbctx).CheckExist({ { "player_guid", player->Guid() },{ "type", Packet::ContainerType::Layer_Bank } }) == true) {
			auto container_db_info = ActiveModel::Base<Packet::Container>(dbctx).FindOne({ { "player_guid", player->Guid() },{ "type", Packet::ContainerType::Layer_Bank } });
			auto bank = boost::make_shared<MtContainer>(*player, container_db_info, Packet::ContainerType::Layer_Bank, bank_config->value2(), bank_config->value3(), bank_config->value4() );
			auto items = ActiveModel::Base<Packet::NormalItem>(dbctx).FindAll({ { "container_guid", container_db_info->guid() } });
			for (auto& item : items) {
				bank->LoadItem(boost::make_shared<MtNormalItem>(bank, item));
			}
			auto equip_items = ActiveModel::Base<Packet::EquipItem>(dbctx).FindAll({ { "container_guid", container_db_info->guid() } });
			for (auto& item : equip_items) {
				bank->LoadItem(boost::make_shared<MtEquipItem>(bank, item));
			}
			auto gem_items = ActiveModel::Base<Packet::GemItem>(dbctx).FindAll({ { "container_guid", container_db_info->guid() } });
			for (auto& item : gem_items) {
				bank->LoadItem(boost::make_shared<MtGemItem>(bank, item));
			}

			player->SetContainer(Packet::ContainerType::Layer_Bank,bank);
		}
		else {
			auto container_db = boost::make_shared<Packet::Container>();
			container_db->set_guid(MtGuid::Instance()(*container_db));
			container_db->set_player_guid(player->Guid());
			container_db->set_max_block(bank_config->value1());
			container_db->set_type(Packet::ContainerType::Layer_Bank);
			container_db->set_crystal_opened_block(0);
			container_db->set_item_opened_block(0);
			auto bank = boost::make_shared<MtContainer>(*player, container_db, Packet::ContainerType::Layer_Bank, bank_config->value2(), bank_config->value3(), bank_config->value4());
			player->SetContainer(Packet::ContainerType::Layer_Bank,bank);
		}
		//
		auto friend_ = boost::make_shared<FriendContainer>(*player);
		player->SetFriendContainer(friend_);
		auto message_ = boost::make_shared<MessageContainer>(*player);
		player->SetMessageContainer(message_);

		return true;
	}



	ContainerManager::ContainerManager()
	{
		__container_manager = this;
	}

	ContainerManager& ContainerManager::Instance()
	{
		return *__container_manager;
	}


	/////////////////////////////////////////////
	void MtContainer::lua_reg(lua_State* ls)
	{
		ff::fflua_register_t<MtContainer, ctor()>(ls, "MtContainer")
			.def(&MtContainer::ExtendCheck,"ExtendCheck")
			.def(&MtContainer::Items,"Items")
			.def(&MtContainer::ExtendSize, "ExtendSize");
	}

	zxero::uint64 MtContainer::Guid() const
	{
		return db_info_->guid();
	}

	void MtContainer::LoadItem(const boost::shared_ptr<MtItemBase>& item)
	{
		items_.push_back(item);
		item->DbInfo()->ClearDirty();
	}

	bool MtContainer::AddItemEx(const int32 config_id, const int32 count, Config::ItemAddLogType type)
	{
		int32 max_block = MtItemManager::Instance().GetMaxBlcokCount(config_id);
		if (max_block <= 0) {
			ZXERO_ASSERT(false) << "item max_block invalid! config_id=" << config_id;
			max_block = 1;
		}
		int32 left = count;
		int32 delta = 0;
		for (auto child : items_) {
			if (child->ConfigId() != config_id) {
				continue;
			}
			if (!child->CanOverlay()) {
				continue;
			}
			if (max_block <= child->Count()) {
				continue;
			}

			delta = max_block - child->Count();
			if (left > delta) {
				left -= delta;
				child->AddCount(delta);
			}
			else {//叠加完了
				child->AddCount(left);
				return true;
			}
		}
		auto player = player_.lock();
		if (player == nullptr) {
			LOG_ERROR << "[item container] player is null. config_id:" << config_id
				<< ",count:" << count << ",type:" << type;
			return false;
		}
		auto item = MtItemManager::Instance().CreateItem(player.get(), config_id, left);
		if (item == nullptr) {
			LOG_ERROR << "[item container] create item:" << config_id << " fail";
			return false;
		}
		if (item->Count() != left) {//不可叠加的道具, 一次创建多个时, 需要依次添加
			for (auto i = 0; i < left - item->Count(); ++i) {
				auto _item = MtItemManager::Instance().CreateItem(player.get(), config_id, 1);
				if (_item) AddItem(_item, type, false);
			}
		}
		return AddItem(item, type, false);		
	}

	int32 MtContainer::CheckBlockCost(const int32 config_id, const int32 count)
	{
		int32 max_block = MtItemManager::Instance().GetMaxBlcokCount(config_id);
		if (max_block <= 0) {
			ZXERO_ASSERT(false) << "item max_block invalid! config_id=" << config_id;
			max_block = 1;
		}
		int32 left = count;
		int32 delta = 0;
		for (auto child: items_) {
			if (child->ConfigId() != config_id) {
				continue;
			}
			if (!child->CanOverlay()) {
				continue;
			}
			if (max_block <= child->Count()) {
				continue;
			}

			delta = max_block - child->Count();
			if (left > delta) {
				left -= delta;
			}
			else {
				return 0;
			}
		}

		return left == max_block ? 1 : (left / max_block + 1);
	}

	bool MtContainer::AddItem(const boost::shared_ptr<MtItemBase>& item, Config::ItemAddLogType type, bool overlay, bool ignore_size )
	{
		auto player = player_.lock();
		if (player == nullptr) {
			LOG_ERROR << "[container] player is null. item:" << item->ConfigId()
				<< ",type:" << type << ",overlay" << overlay
				<< ",ignore_size" << ignore_size;
			return false;
		}
		if (overlay) {
			for (auto child : items_) {
				if (child->ConfigId() != item->ConfigId()) {
					continue;
				}
				if (child->CanOverlay()) {
					if (child->MaxBlockCount() >= child->Count() + item->Count()) {
						auto count = item->Count();
						item->DecCount(count);
						child->AddCount(count);
						//叠加完了
						return true;
					}
					else {
						auto left_capancity = child->MaxBlockCount() - child->Count();
						item->DecCount(left_capancity);
						child->AddCount(left_capancity);
					}
				}
			}
		}
		if ((zxero::int32)items_.size() >= db_info_->max_block() && !ignore_size ) {
			if (type == Config::ItemAddLogType::AddType_Mission ||
				type == Config::ItemAddLogType::AddType_Arrange || 
				type == Config::ItemAddLogType::AddType_Acr ) {//任务,分配，活动道具 无视背包大小，防止背包满了完成不了任务
				//背包满了发邮件
				google::protobuf::RepeatedPtrField<Packet::ItemCount> items;
				auto item_ = items.Add();
				item_->set_itemid(item->ConfigId());
				item_->set_itemcount(item->Count());
				auto mail = static_message_manager::Instance().create_message(player->Guid(), player, MAIL_OPT_TYPE::FULLBAG_ADD, "BagIsFull", "BagIsFull", &items);
				return true;
			} 
			else {
				return false;
			}
		}
		item->OnAdd2Container(shared_from_this());
		items_.push_back(item);
		Packet::PlayerContainerAddItemNotify notify;
		notify.set_container_type(type_);
		auto item_info = notify.add_items();
		item->FillMessage(*item_info);
		player->SendMessage(notify);

		return true;
	}

	int32 MtContainer::GetItemCount(int32 config_id)
	{
		int32 count = 0;
		for (auto child : items_)
		{
			if (config_id == child->ConfigId())
				count += child->Count();
		}

		return count;
	}
	bool MtContainer::DelItem(int32 config_id, int32 count)
	{
		//数量判定
		if (GetItemCount(config_id) < count)
		{
			return false;
		}

		if (count == 0) {
			return true; //算作合法！
		}
		switch (type_)
		{
		case Packet::ContainerType::Layer_Equip:
		{
			auto item = FindItemByConfigId(config_id);
			return RemoveItem(item);
		}
		break;
		case Packet::ContainerType::Layer_EquipBag:
		{
			auto item = FindItemByConfigId(config_id);
			auto old_count = item->Count();
			if (item != nullptr	&& item->DecCount(count) == old_count)
			{
				//ZXERO_ASSERT(false) << "dec item count fail";
				return false;
			}
		}
		break;
		case Packet::ContainerType::Layer_ItemBag:
		case Packet::ContainerType::Layer_MissionBag:
		{
			int32 loops = 0;
			while (count > 0)
			{
				auto item = FindItemByConfigId(config_id);
				if (item == nullptr) {
					return false;
				}
				auto old_count = item->Count();
				if (old_count >= count) {
					item->DecCount(count);
					count = 0;
				} else {
					count -= old_count;
					item->DecCount(old_count);
				}
				if (loops++ > 10) {
					LOG_ERROR << "[container] del item:" << config_id
						<< ",count:" << count << ",type:" << type_
						<< " loops more than 10 times";
					return false;
				}
			}
		}
		break;
		default: 
		{
			LOG_WARN << "[container] del item:" << config_id
				<< ",count:" << count << ",type:" << type_;
			return false;
		}
		break;
		}
		return true;
	}

	bool MtContainer::ExtendCheck(const int32 size, const int32 type)
	{
		int32 current_size = db_info_->max_block();
		int32 final_size = current_size + size;
		if (final_size > max_size_) {
			return false;
		}
		if (type == 0) {//金币扩展
			auto ccount = db_info_->crystal_opened_block();
			if (ccount + size > max_crystal_open_size_) {
				return false;
			}
		}
		else if (type == 1) {//道具扩展
			auto ccount = db_info_->item_opened_block();
			if (ccount + size > max_item_open_size_) {
				return false;
			}
		}
		
		return true;
	}

	bool MtContainer::ExtendSize(const int32 size, const int32 type)
	{
		auto player = player_.lock();
		if (player == nullptr) {
			LOG_ERROR << "[container] player is null." << ",size" << size
				<< ",type:" << type;
			return false;
		}
		int32 current_size = db_info_->max_block();
		int32 final_size = current_size + size;
		if (final_size > max_size_) {
			return false;
		}
		if (type == 0) {//金币扩展
			auto ccount = db_info_->crystal_opened_block();
			if (ccount + size > max_crystal_open_size_) {
				return false;
			}
			ccount += size;
			db_info_->set_crystal_opened_block(ccount);
		}
		else if (type == 1) {//道具扩展
			auto ccount = db_info_->item_opened_block();
			if (ccount + size > max_item_open_size_) {
				return false;
			}
			ccount += size;
			db_info_->set_item_opened_block(ccount);
		}
		
		Packet::CommonReply reply;
		reply.set_reply_name("extend_size");
		db_info_->set_max_block(final_size);
		reply.add_int32_params(db_info_->type());
		reply.add_int32_params(final_size);

		player->SendMessage(reply);

		return true;
	}

	void MtContainer::OnRefreshTime(const int32 delta)
	{
		std::vector<boost::shared_ptr<MtItemBase>> delete_items;

		for (auto child: items_) {
			if (child->Type() != MtItemType::NORMAL_ITEM) {
				continue;
			}

			auto item = static_cast<MtNormalItem*>(child.get());
			if (item == nullptr) {
				continue;
			}

			//更新冷却
			auto config = MtItemManager::Instance().GetItemConfig(item->ConfigId());
			if (config == nullptr) {
				continue;
			}
			
			bool toclient = false;
			if (config->life_time() > 0){ 
				int32 left_time = item->DbInfo()->life_time();
				if (left_time >= 0) {
					left_time -= delta;
					if (left_time <= 0) {//表示失效
						delete_items.push_back(child);
						//RemoveItem(child);迭代器失效！！！！
						continue;
					}
					item->DbInfo()->set_life_time(left_time);
					toclient = true;
				}				
			}		

			int32 bigcd = config->cool_down() / 1000;			
			if (bigcd >= BIG_TICK ) {// 超过10s cd的道具才做服务器cd校验 [3/25/2017 SEED]
				int32 cool_down = item->DbInfo()->cool_down();
				if (cool_down > 0) {
					cool_down -= delta;
					if (cool_down < 0) {
						cool_down = 0;
					}
					item->DbInfo()->set_cool_down(cool_down);
					toclient = true;
				}
			}
		
			if (toclient) {
				item->RefreshData2Client();
			}
		}

		//清理超时道具
		for (auto child : delete_items) {
			RemoveItem(child);
		}
	}

	void MtContainer::OnSave(const boost::shared_ptr<zxero::dbcontext>& dbctx)
	{
		ActiveModel::Base<Packet::Container>(dbctx).Save(db_info_);
		auto items_copy = items_;
		ActiveModel::Base<Packet::GemItem>(dbctx).Delete({ {"container_guid", this->Guid()} });
		ActiveModel::Base<Packet::NormalItem>(dbctx).Delete({ { "container_guid", this->Guid() } });
		ActiveModel::Base<Packet::EquipItem>(dbctx).Delete({ { "container_guid", this->Guid() } });
		std::for_each(std::begin(items_copy), std::end(items_copy), boost::bind(&MtItemBase::OnSave, _1, dbctx));
	}

	void MtContainer::OnCache()
	{
		auto player = player_.lock();
		if (player == nullptr) {
			LOG_ERROR << "[container] player is null";
			return;
		}
		player->Cache2Save(db_info_.get());

		std::for_each(std::begin(items_), std::end(items_), boost::bind(&MtItemBase::OnCache, _1));
	}

	bool MtContainer::RemoveItem(const boost::shared_ptr<MtItemBase>& item,
		bool is_delete /*= true*/, 
		bool notify /*= true*/)
	{		
		auto player = player_.lock();
		if (player == nullptr) {
			LOG_ERROR << "[container] player is null";
			return false;
		}
		if (item == nullptr) {
			LOG_ERROR << "[container] item is null";
			return false;
		}
		ZXERO_ASSERT(item->Container()->Guid() == Guid());
		ZXERO_ASSERT(std::count_if(std::begin(items_), std::end(items_), boost::bind(&MtItemBase::Guid, _1) == item->Guid()) == 1);
		
		auto dbinfo = item->DbInfo();
		if (dbinfo == nullptr) {
			return false;
		}
		dbinfo->SetDirty();

		if(is_delete)//move的情况能删除
			dbinfo->SetDeleted();

		item->OnCache();

		items_.erase(std::remove_if(std::begin(items_), std::end(items_),
			boost::bind(&MtItemBase::Guid, _1) == item->Guid()),items_.end());

		if (notify) {
			Packet::PlayerContainerDelItemNotify notify_msg;
			notify_msg.set_container_type(type_);
			notify_msg.add_guids(item->Guid());
			player->SendMessage(notify_msg);
		}
		return true;
	}

	void MtContainer::FillMessage(Packet::PlayerContainerReply& message)
	{
		message.set_max_block(db_info_->max_block());
		//message.set_crystal_lock_block(max_crystal_open_size_-db_info_->crystal_opened_block());
		//message.set_item_lock_block(max_item_open_size_-db_info_->item_opened_block());
		message.set_container_type(type_);
		for (auto& item : items_)
		{
			item->FillMessage(*(message.add_items()));
		}
	}

	boost::weak_ptr<MtPlayer> MtContainer::Player()
	{
		return player_;
	}

	const boost::shared_ptr<MtItemBase> MtContainer::FindItemByGuid(zxero::uint64 guid)
	{
		auto item_it = std::find_if(std::begin(items_), std::end(items_), boost::bind(&MtItemBase::Guid, _1) == guid);
		if (item_it == std::end(items_)) {
			return nullptr;
		}
		return *item_it;
	}

	const boost::shared_ptr<MtItemBase> MtContainer::FindItemByConfigId(int32 config_id)
	{
		auto it = std::find_if(std::begin(items_), std::end(items_), boost::bind(&MtItemBase::ConfigId, _1) == config_id);
		if (it == std::end(items_)) {
			return nullptr;
		}
		return *it;
	}

	void MtContainer::Reset()
	{
		auto player = player_.lock();
		if (player == nullptr) {
			LOG_ERROR << "[container] player is null";
			return;
		}
		Packet::PlayerContainerDelItemNotify notify;
		notify.set_container_type(Type());
		for (auto& item : items_) {
			notify.add_guids(item->Guid());
			item->DbInfo()->SetDirty();
			item->DbInfo()->SetDeleted();
			item->OnCache();
		}
		player->SendMessage(notify);
		items_.clear();
	}

	MtContainer::MtContainer(MtPlayer& player, 
		const boost::shared_ptr<Packet::Container>& db_info, 
		const Packet::ContainerType type, const int32 max_size,
		const int32 max_crystal_open_size, const int32 max_item_open_size) 
		:player_(player.weak_from_this())
		, type_(type)
		, db_info_(db_info)
		, max_size_(max_size)
		, max_crystal_open_size_(max_crystal_open_size)
		, max_item_open_size_(max_item_open_size)
	{

	}

}