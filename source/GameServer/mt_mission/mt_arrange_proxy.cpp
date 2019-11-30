#include "mt_arrange_proxy.h"
#include "module.h"
#include "../data/data_manager.h"
#include "../mt_player/mt_player.h"
#include "../mt_server/mt_server.h"
#include "../mt_actor/mt_actor.h"
#include "../mt_scene/mt_scene.h"
#include "../mt_item/mt_container.h"
#include "../mt_item/mt_item_package.h"
#include "../mt_item/mt_item.h"
#include "../mt_team/mt_team_manager.h"

#include <BaseConfig.pb.h>
#include <ItemConfig.pb.h>
#include <SceneCommon.pb.h>
#include <AllPacketEnum.proto.fflua.h>
#include "../zxero/mem_debugger.h"

namespace Mt
{
	/*
	1.关闭按钮		放弃roll点	系统时间到也不roll点
	2.需求按钮		roll点【指定roll点】	系统时间到也不roll点
					roll点【非指定roll点】	系统时间到也不roll点
	3.什么都不做	客户端cd完，不处理		系统时间到roll点
					掉线，不处理			系统时间到roll点

	权限规则		有权限的人分配未分配完的装备	已完成过任务的无分配权限
					单次放弃roll点无权限
					单次roll到装备的，获得剩余分配权限
					单次未roll到装备，先随机一件，并获得剩余分配权限

	*/

	void MtArrangeProxy::lua_reg(lua_State* ls)
	{
		ff::fflua_register_t<MtArrangeProxy, ctor()>(ls, "MtArrangeProxy")
			.def(&MtArrangeProxy::OnArrangeTo, "OnArrangeTo")
			.def(&MtArrangeProxy::OnRollArrange, "OnRollArrange")
			.def(&MtArrangeProxy::ConfirmRollItem, "ConfirmRollItem")
			.def(&MtArrangeProxy::CancelArrangeItem, "CancelRollItem")
			.def(&MtArrangeProxy::SetTo, "SetTo")
			.def(&MtArrangeProxy::SetMissionId,"SetMissionId")
			.def(&MtArrangeProxy::GetContainer,"GetContainer")
			.def(&MtArrangeProxy::OnClearContainer,"OnClearContainer")
			.def(&MtArrangeProxy::OnArrangeEnd, "OnArrangeEnd")
			.def(&MtArrangeProxy::UpdateArrangeInfo,"UpdateArrangeInfo")
			.def(&MtArrangeProxy::OnArrangeBegin, "OnArrangeBegin")
			;
	}

	MtArrangeProxy::MtArrangeProxy(MtPlayer& player)
		:leader_(player.weak_from_this()) {
		auto container_db = boost::make_shared<Packet::Container>();
		container_db->set_guid(INVALID_GUID);
		container_db->set_player_guid(player.Guid());
		container_db->set_max_block(3);
		container_db->set_type(Packet::ContainerType::Layer_ArrangeBag);
		container_db->set_crystal_opened_block(0);
		container_db->set_item_opened_block(0);
		container_ = boost::make_shared<MtContainer>(player, container_db, Packet::ContainerType::Layer_ArrangeBag, 3, 0, 0);
	}

	void MtArrangeProxy::OnClearContainer()
	{
		container_->Items().clear();
	}

	void MtArrangeProxy::OnTick(uint64 elapseTime)
	{
		if (wait_timer_ <= 0) {
			return;
		}

		wait_timer_ -= elapseTime;
		if (wait_timer_ <= 0) {
			OnArrangeEnd();
		}
	}

	void MtArrangeProxy::OnArrangeEnd() 
	{
		auto leader = leader_.lock();
		if (leader == nullptr) {
			ZXERO_ASSERT(false);
			return;
		}
		wait_timer_ = 0;

		auto type = leader->GetTeamArrangeType();
		auto members = leader->GetTeamMember(true);
		CheckLostArrange(members);
		if (type == Packet::ArrangeType::AT_Leader) {
			OnArrangeTo();
		}
		else if (type == Packet::ArrangeType::AT_Roll) {
			if (leader == nullptr) {
				return;
			}
			//补roll
			for (auto child : lost_players_) {				
				RollItem(INVALID_GUID, child);
			}
			OnRollArrange();
		}

		//arrange_end特写!!!
		leader->OnLuaFunCall_x("xOnArrangeOver",
		{
			{ "id",leader->GetParams32("arrange_statistic") }
		});		
		//
	}
	void MtArrangeProxy::OnArrangeBegin(const std::vector<uint64> & filter_list)
	{
		arrange_list_.clear();
		roll_points_.clear();

		auto leader = leader_.lock();
		if (leader == nullptr) {
			ZXERO_ASSERT(false);
			return;
		}
		auto type = leader->GetTeamArrangeType();
		if (type == Packet::ArrangeType::AT_Leader) {
			wait_timer_ = 60000;
		}
		else if (type == Packet::ArrangeType::AT_Roll) {
			wait_timer_ = 30000;
		}
		else {
			//wait_timer_ = 9999999999;
			return;
		}
		

		//更新过滤列表
		for (auto child : filter_list) {
			CancelArrangeItem(child);
		}

		for (auto child : container_->Items()) {
			if (child != nullptr) {
				arrange_list_.insert({ child->Guid(),INVALID_GUID });
			}
		}

		//过滤列表
		if (type == Packet::ArrangeType::AT_Leader) {
			leader->SendLuaPacket("ATFilterList", {}, filter_list, {});
		}

		UpdateArrangeInfo();			
	}

	void MtArrangeProxy::RefreshData2Client()
	{
		auto leader = leader_.lock();
		if (leader == nullptr) {
			ZXERO_ASSERT(false);
			return;
		}
		Packet::PlayerContainerReply reply;
		container_->FillMessage(reply);
		leader->BroadcastTeam(filter_list_, reply);
	}

	void MtArrangeProxy::UpdateArrangeInfo()
	{
		auto leader = leader_.lock();
		if (leader == nullptr) {
			ZXERO_ASSERT(false);
			return;
		}
		Packet::UpdateArrangeInfoReply reply;
		for (auto child : arrange_list_) {
			reply.add_items(child.first);
			reply.add_players(child.second);
			reply.set_second((int32)(wait_timer_/1000));
			reply.set_at_type(leader->GetTeamArrangeType());
		}
		leader->BroadcastTeam( filter_list_, reply);

	}

	bool MtArrangeProxy::OnArrangeTo()
	{		
		auto leader = leader_.lock();
		if (leader == nullptr) {
			ZXERO_ASSERT(false);
			return false;
		}
		
		//分配
		std::vector<uint64> un_arrange_item;
		DoArrange(un_arrange_item);

		//没roll到的分配
		DoLostArrange(un_arrange_item);

		//最后剩余分配，如果还有的话
		//此处会被玩家刷，一人获得多件,先关掉
		//DoLastArrange(un_arrange_item);

		//其他就不管了
		arrange_list_.clear();
		roll_points_.clear();
		//
		lost_players_.clear();
		last_chance_.clear();
		join_players_.clear();
		filter_list_.clear();	

		return true;
	}

	void MtArrangeProxy::DoArrange(std::vector<uint64>& un_arrange_item)
	{
		for (auto child : arrange_list_) {
			if (!ArrangeTo(child.first, child.second)) {
				un_arrange_item.push_back(child.first);
				continue;
			}
			//进入最后分配池
			last_chance_.push_back(child.second);
		}
	}

	void MtArrangeProxy::DoLostArrange(std::vector<uint64>& un_arrange_item)
	{
		auto leader = leader_.lock();
		if (leader == nullptr) {
			ZXERO_ASSERT(false);
			return;
		}
		auto members = leader->GetTeamMember(true);
		CheckLostArrange(members);
		for (auto iter = un_arrange_item.begin(); iter != un_arrange_item.end(); ) {
			if (lost_players_.empty()) {
				break;
			}
			int32 seed = (int32)rand_gen->intgen(0, lost_players_.size() - 1);
			if (ArrangeTo(*iter, lost_players_[seed])) {
				//进入最后分配池
				last_chance_.push_back(lost_players_[seed]);
				lost_players_.erase(lost_players_.begin() + seed);
				iter = un_arrange_item.erase(iter);
			}
			else {
				++iter;
			}
		}
	}

	void MtArrangeProxy::DoLastArrange(std::vector<uint64>& un_arrange_item)
	{
		//剩下的人分剩下的装备，看人品分，允许重复获得
		if (last_chance_.size() < 1) return;

		for (auto child : un_arrange_item) {
			int32 seed = (int32)rand_gen->intgen(0, last_chance_.size() - 1);
			ArrangeTo(child, last_chance_[seed]);
		}
	}

	void MtArrangeProxy::CheckLostArrange(const std::vector<uint64> & members)
	{
		lost_players_.clear();		
		for (auto child : members) {

			//检查过滤列表
			if (std::find(filter_list_.begin(), filter_list_.end(), child) != filter_list_.end()) {
				continue;
			}

			//检查过滤列表
			if (std::find(lost_players_.begin(), lost_players_.end(), child) != lost_players_.end()) {
				continue;
			}

			//检查已经参与分配的
			bool had = false;
			for (auto iter : arrange_list_) {
				if (child == iter.second) {
					had = true;
					break;
				}
			}
			//
			if (!had) {
				lost_players_.push_back(child);
			}
		}
	}
	bool MtArrangeProxy::ArrangeTo(const uint64 item_guid, const uint64 player_guid)
	{
		auto leader = leader_.lock();
		if (leader == nullptr) {
			ZXERO_ASSERT(false);
			return false;
		}
		auto scene = leader->Scene();
		if (scene == nullptr) {
			return false;
		}

		auto player = scene->FindPlayer(player_guid);
		if (player == nullptr) {
			return false;
		}

		auto item = container_->FindItemByGuid(item_guid);
		if (item == nullptr) {
			return false;
		}


		if (player->AddItem(item, Config::ItemAddLogType::AddType_Arrange, mission_id_)) {

			std::string id = item->GenHyperStr();
			std::string notify = "cast_get_arrange_item|" + player->Name()+"|"+id;
			leader->SendClientNotify(notify, Packet::ChatChannel::TEAM, -1);

			notify = "get_arrange_item|" + id;
			player->SendClientNotify(notify, Packet::ChatChannel::SYSTEM, -1);

			return true;
		}

		return false;
	}

	bool MtArrangeProxy::OnRollArrange()
	{
		//current_arrange_.clear();
		for (auto child : roll_points_) {
			int32 max = 0;
			uint64 guid = INVALID_GUID;
			for (auto point : child.second) {
				if (max < point.second) {
					max = point.second;
					guid = point.first;
				}
			}

			if (arrange_list_.find(child.first)!= arrange_list_.end() ) {
				arrange_list_[child.first] = guid;
			}
			else {
				arrange_list_.insert({ child.first,guid });
			}
		}

		OnArrangeTo();

		return true;
	}

	bool MtArrangeProxy::CancelArrangeItem(const uint64 player_guid)
	{
		//检查过滤列表
		if (std::find(filter_list_.begin(), filter_list_.end(), player_guid) == filter_list_.end()) {
			filter_list_.push_back(player_guid);
		}		

		if (IsAllRollOver()) {
			//提前结束
			OnArrangeEnd();
		}
		return true;
	}

	bool MtArrangeProxy::IsAllRollOver() {
		auto leader = leader_.lock();
		if (leader == nullptr) {
			ZXERO_ASSERT(false);
			return false;
		}
		auto memebers = leader->GetTeamMember(true);
		for (auto child : memebers) {
			if (RollCheck(child)) {
				return false;
			}
		}
		return true;
	}
	bool MtArrangeProxy::RollCheck(const uint64 player_guid)
	{
		for (auto child : filter_list_) {
			if (child == player_guid) {
				return false;
			}
		}
		for (auto child : join_players_) {
			if (child == player_guid) {
				return false;
			}
		}
		return true;
	}

	bool MtArrangeProxy::RollPoint(const uint64 item_guid, const uint64 player_guid)
	{
		auto leader = leader_.lock();
		if (leader == nullptr) {
			ZXERO_ASSERT(false);
			return false;
		}
		auto scene = leader->Scene();
		if (scene == nullptr) {
			return false;
		}

		auto player = scene->FindPlayer(player_guid);
		if (player == nullptr) {
			return false;
		}

		auto item = container_->FindItemByGuid(item_guid);
		if (item == nullptr) {
			return false;
		}

		int32 point = 0;

		auto iter = roll_points_.find(item_guid);
		if (iter == roll_points_.end()) {
			point = rand_gen->intgen(1, 100);
			std::map<uint64, int32> temp;
			temp.insert({ player_guid,point });
			roll_points_.insert({ item_guid, temp });
		}
		else {
			auto piter = iter->second.find(player_guid);
			if (piter != iter->second.end()) {
				player->SendClientNotify("re_roll_point", -1, -1);
				return false;
			}
			point = rand_gen->intgen(1, 100);
			iter->second.insert({ player_guid,point });
		}


		//
		std::string id = std::to_string(item->ConfigId());
		if (item->Type() == MtItemType::EQUIP_ITEM) {
			id = "|equip_name_" + id;
		}
		else if (item->Type() == MtItemType::NORMAL_ITEM) {
			id = "|item_name_" + id;
		}
		else if (item->Type() == MtItemType::GEM_ITEM) {
			id = "|gem_name_" + id;
		}
		std::string notify = "roll_out_point|" + player->Name() + id + "|" + std::to_string(point);
		player->SendClientNotify(notify, Packet::ChatChannel::TEAM, -1);


		return true;
	}

	bool MtArrangeProxy::RollItem(uint64 item_guid, const uint64 player_guid)
	{
		auto leader = leader_.lock();
		if (leader == nullptr) {
			ZXERO_ASSERT(false);
			return false;
		}

		if (arrange_list_.empty()) {
			return false;
		}

		//参与过就不能再roll了
		if (!RollCheck(player_guid)) {
			return false;
		}


		//没有指定装备就随一件roll
		if (item_guid == INVALID_GUID) {
			int32 seed = rand_gen->intgen(0, arrange_list_.size() - 1);
			int32 i = 0;
			for (auto child : arrange_list_) {
				if (i == seed) {
					item_guid = child.first;
					break;
				}
				i++;
			}
		}

		if (RollPoint(item_guid, player_guid)) {
			join_players_.push_back(player_guid);
		}
		return true;
	}

	bool MtArrangeProxy::ConfirmRollItem(uint64 item_guid, const uint64 player_guid)
	{
		if (!RollItem(item_guid, player_guid)) {
			return false;
		}

		if (IsAllRollOver()) {
			//提前结束
			OnArrangeEnd();
		}
		return true;
	}

	bool MtArrangeProxy::SetTo( uint64 item_guid, const uint64 player_guid)
	{
		auto leader = leader_.lock();
		if (leader == nullptr) {
			ZXERO_ASSERT(false);
			return false;
		}

		auto scene = leader->Scene();
		if (scene == nullptr) {
			return false;
		}

		auto player = scene->FindPlayer(player_guid);
		if (player == nullptr) {
			return false;
		}

		auto item = container_->FindItemByGuid(item_guid);
		if (item == nullptr) {
			return false;
		}

		for (auto child : join_players_) {
			if (child == player_guid) {
				return false;
			}
		}
		//可顶替
		if (arrange_list_[item_guid] != player_guid) {			
			for (auto iter = join_players_.begin(); iter != join_players_.end();) {
				if (*iter == arrange_list_[item_guid]) {
					iter = join_players_.erase(iter);
				}
				else {
					++iter;
				}
			}
		}

		arrange_list_[item_guid] = player_guid;
		join_players_.push_back(player_guid);

		UpdateArrangeInfo();

		return true;
	}

	bool MtArrangeProxy::IsArrangeBagFull()
	{
		return container_->LeftSize() < 0;
	}

	bool MtArrangeProxy::OnAddItem(const int32 configid, const int32 count)
	{
		if (container_ == nullptr || count <= 0) {
			return false;
		}
		auto leader = leader_.lock();
		if (leader == nullptr) {
			ZXERO_ASSERT(false);
			return false;
		}
		auto item = MtItemManager::Instance().CreateItem(leader.get(), configid, count);
		if (item == nullptr) {
			return false;
		}

		if (!container_->AddItem(item)){
			return false;
		}
		
		LOG_INFO<<"MtArrangeProxy::OnAddItem ok! itemmid=" << item->Guid() << " config id =" << item->ConfigId() << " player guid=" << leader->Guid();

		return true;
	}
}
