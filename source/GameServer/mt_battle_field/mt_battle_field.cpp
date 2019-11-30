#include "mt_battle_field.h"
#include "mt_battle_field_manager.h"
#include "module.h"
#include "../mt_player/mt_player.h"
#include "../mt_actor/mt_actor.h"
#include "../mt_scene/mt_raid.h"
#include "../mt_scene/mt_scene.h"
#include "../mt_scene/mt_copy_scene.h"
#include "../mt_scene/mt_scene_manager.h"
#include "../mt_monster/mt_monster_manager.h"
#include "../mt_data_cell/mt_data_container.h"
#include "../mt_config/mt_config.h"
#include "../mt_skill/mt_simple_impact.h"
#include "../mt_chat/mt_chatmessage.h"
#include <map>
#include <AllPacketEnum.pb.h>
#include <EnterScene.pb.h>
#include <ActivityPacket.pb.h>
#include <BaseConfig.pb.h>
#include <S2GMessage.pb.h>
#include <S2SMessage.pb.h>
#include <BattleField.proto.fflua.h>

namespace Mt
{

	void MtBattleField::lua_reg(lua_State* ls)
	{
		ff::fflua_register_t<MtBattleField, ctor()>(ls, "MtBattleField")
			//.def(&MtBattleField::OnBuffPoint, "OnBuffPoint")
			.def(&MtBattleField::OnForceBegin, "OnForceBegin")
			.def(&MtBattleField::ResultCalc, "ResultCalc")
			.def(&MtBattleField::OnDropFlag, "OnDropFlag")		
			//.def(&MtBattleField::OnGenBuff, "OnGenBuff")
			;
	}

	MtBattleField::MtBattleField() {
		reset();
	};
	MtBattleField::~MtBattleField() {
	};

	void MtBattleField::OnSecondTick(int32 elapse_second)
	{
		if (room_info_.bf_state() == Packet::BattleFieldState::bf_state_inroom){

			wait_timer_ += elapse_second;
			if (IsFull() || wait_timer_ >= 300 ) {//Ҫô����Ҫô5���Ӿ��Զ�����
				//׼������ս��
				OnBattlePrepare();
				wait_timer_ = 0;
			}
		}

		if (room_info_.bf_state() == Packet::BattleFieldState::bf_state_room_waiting) {
			OnBattleEnter();
			wait_timer_ = 0;			
		}

		if (room_info_.bf_state() == Packet::BattleFieldState::bf_state_battle_waiting) {
			wait_timer_ += elapse_second;
			//��ʽ��ʼ
			if (wait_timer_ > 30) {
				OnBeginBattleField();
				wait_timer_ = 0;
			}
		}

		if (room_info_.bf_state() == Packet::BattleFieldState::bf_state_inbattle) {

			//BuffPointCheck(elapse_second);

			PickFlagCheck(attacker_flag_queue_,elapse_second);
			
			PickFlagCheck(defender_flag_queue_,elapse_second);

			ReliveCheck(elapse_second);

			EndCheck(elapse_second);

		}

	}

	void MtBattleField::OnBigTick(uint64 elapseTime)
	{
		elapseTime;
		
	}

	//���·�����Ϣ
	void MtBattleField::BroadcastMsg(const google::protobuf::Message& msg, const int32 camp, const std::vector<uint64>& filter_list )
	{
		std::unique_lock<std::mutex> lock(mutex_);

		if (camp != -1) {
			for (int32 i = 0; i < room_info_.max_count(); i++) {
				auto pi = room_info_.mutable_players(i);
				if (pi != nullptr) {
					if (pi->bf_state() == Packet::BattleFieldState::bf_state_disagree) {
						continue;
					}
					if (std::find_if(filter_list.begin(), filter_list.end(), [=](auto& iter)
					{return (iter == pi->guid()); }) != filter_list.end()) {
						continue;
					}
					auto p = Server::Instance().FindPlayer(pi->guid());
					if (p != nullptr && CheckCamp(pi->guid()) == camp ) {
						p->SendMessage(msg);
					}
				}
			}
		}
		else {
			for (int32 i = 0; i < room_info_.max_count(); i++) {
				auto pi = room_info_.mutable_players(i);
				if (pi != nullptr) {
					
					if (std::find_if(filter_list.begin(), filter_list.end(), [=](auto& iter)
					{return (iter == pi->guid());}) != filter_list.end()) {
						continue;
					}
					auto p = Server::Instance().FindPlayer(pi->guid());
					if (p != nullptr) {
						p->SendMessage(msg);
					}
				}
			}
		}
	}

	bool MtBattleField::CheckLevel(const int32 level)
	{
		if (level < room_info_.min_level() || level >room_info_.max_level())
			return false;
		return true;
	}

	bool MtBattleField::IsFull() {
		for (int32 i = 0; i < room_info_.max_count(); i++) {
			auto pi = room_info_.mutable_players(i);
			if (pi != nullptr) {
				if (pi->guid() == INVALID_GUID) {
					return false;
				}
			}
		}
		return true;
	}

	bool MtBattleField::IsEmpty() {
		for (int32 i = 0; i < room_info_.max_count(); i++) {
			auto pi = room_info_.mutable_players(i);
			if (pi != nullptr) {
				if (pi->guid() != INVALID_GUID) {
					return false;
				}
			}

		}
		return true;
	}

	bool MtBattleField::IsHasPlayer(const uint64 guid) {
		for (int32 i = 0; i < room_info_.max_count(); i++) {
			auto pi = room_info_.mutable_players(i);
			if (pi != nullptr) {
				if (pi->guid() == guid) {
					return true;
				}
			}
		}
		return false;
	}

	int MtBattleField::CheckCamp(const uint64 guid)
	{
		for (int32 i = 0; i < bf_info_.attackers().size(); i++){
			if (bf_info_.attackers(i) == guid) {
				return 1;
			}
		}
		for (int32 i = 0; i < bf_info_.defenders().size(); i++){
			if (bf_info_.defenders(i) == guid) {
				return 0;
			}
		}

		return -1;
	}

	int32 MtBattleField::GetPosIndex(const int32 camp, int32 flag )
	{

		if( flag == -1 ){//����
			if (camp == -1)
				return 0;
			else if (camp == 1) {
				return 1;
			}
			else {
				return 2;
			}
		}
		if (flag == 1) {//����
			if (camp == -1)
				return -1;
			else if (camp == 1) {
				return 4;
			}
			else {
				return 3;
			}
		}
		if (flag == 2) {//����
			if (camp == -1)
				return -1;
			else if (camp == 1) {
				return 3;
			}
			else {
				return 4;
			}
		}

		return -1;
	}

	void MtBattleField::OnBeginPickFlag(const uint64 guid)
	{
		if (CheckCamp(guid) == 1) {
			attacker_flag_queue_.insert({ guid,0 });
		}
		else if (CheckCamp(guid) == 0) {
			defender_flag_queue_.insert({ guid,0 });
		}
		else {
			return;
		}
	}
	
	void MtBattleField::OnCancelPickFlag(MtPlayer * player)
	{
		if (player == nullptr) {
			return;
		}
		auto aiter = attacker_flag_queue_.find(player->Guid());
		if (aiter != attacker_flag_queue_.end()) {
			attacker_flag_queue_.erase(aiter);

			Packet::FlagOprate msg;
			msg.set_option(Packet::FlagOption::CancelPick);
			msg.set_guid(player->Guid());
			BroadcastMsg(msg);
			return;
		}

		auto diter = defender_flag_queue_.find(player->Guid());
		if (diter != defender_flag_queue_.end()) {
			defender_flag_queue_.erase(diter);

			Packet::FlagOprate msg;
			msg.set_option(Packet::FlagOption::CancelPick);
			msg.set_guid(player->Guid());
			BroadcastMsg(msg);
			return;
		}
	}

	void MtBattleField::OnRelive( MtPlayer * player)
	{
		if (player == nullptr) {
			return;
		}
		auto info = GetBattlePlayerInfo(player->Guid());
		if (info == nullptr) {
			return;
		}

		//ͬ�����proxy
		Packet::BattleFeildProxy bproxy;
		bproxy.set_raid_rtid(raid_rtid_);
		bproxy.set_bf_state(Config::BFState::Bf_Normal);
		player->SetBattleFeildProxy(bproxy);

		//ok�������н�ɫ
		Server::Instance().SendG2SCommonMessage(player, "G2SRelive", { Packet::LastHpType::BFLastHp }, {}, {});

		//�����ٶ�����30%			
		auto proxy = player->GetSimpleImpactSet();
		//BfSpeedUpImpact
		proxy->AddSimpleImpact(1001,5,1200,0,false);

		info->set_dead(false);
		BroadcastMsg(*info);
		
	}

	void MtBattleField::OnDropFlag(MtPlayer * player, bool iswin)
	{
		if (player == nullptr) {
			return;
		}

		auto info = GetBattlePlayerInfo(player->Guid());
		if (info == nullptr) {
			return;
		}

		//ʤ��Ҫ������
		if (iswin && info->has_flag()) {
			auto count = info->get_flag_num();
			info->set_get_flag_num(count + 1);
			info->set_honor(info->honor() + 100);
		}
		info->set_has_flag(false);

		BroadcastMsg(*info);

		//ͬ�����proxy
		Packet::BattleFeildProxy bproxy;
		bproxy.set_raid_rtid(raid_rtid_);
		bproxy.set_bf_state(Config::BFState::Bf_Normal);
		player->SetBattleFeildProxy(bproxy);
	
		auto proxy = player->GetSimpleImpactSet();
		if (proxy) {
			proxy->DecSimpleImpact(1002);
		}

	}
	void MtBattleField::OnPickFlag(MtPlayer * player)
	{
		if (player == nullptr) {
			return;
		}

		auto info = GetBattlePlayerInfo(player->Guid());
		if (info == nullptr) {
			return player->SendCommonResult(Packet::ResultOption::BattleField_Opt, Packet::ResultCode::InvalidPlayerData);
		}
		info->set_has_flag(true);
		BroadcastMsg(*info);

		//ͬ�����proxy
		Packet::BattleFeildProxy bproxy;
		bproxy.set_raid_rtid(raid_rtid_);
		bproxy.set_bf_state(Config::BFState::Bf_Flag);
		player->SetBattleFeildProxy(bproxy);

		//�����ٶȵݼ�30%				
		auto proxy = player->GetSimpleImpactSet();
		//BfSpeedDownImpact
		proxy->AddSimpleImpact(1002,999999999, -1200, 0, false);
	}

	Packet::BattleFieldPlayerInfo*  MtBattleField::OnBattleLoss(MtPlayer * player)
	{
		if (player == nullptr) {
			return nullptr;
		}

		auto info = GetBattlePlayerInfo(player->Guid());
		if (info == nullptr) {
			return nullptr;
		}
		
		info->set_locked(false);
		//
		OnDropFlag(player, false);

		//ʧ��Ҳ������
		info->set_dead(true);
		auto dead_num = info->dead_num();
		info->set_dead_num(dead_num + 1);
		info->set_honor(info->honor() + 5);		

		//ͬ�����proxy
		Packet::BattleFeildProxy bproxy;
		bproxy.set_raid_rtid(raid_rtid_);
		bproxy.set_bf_state(Config::BFState::Bf_Dead);
		player->SetBattleFeildProxy(bproxy);

		BroadcastMsg(*info);

		if (30 - relive_timer_>0)
		{
			player->SendLuaPacket("relive_time", { 30 - relive_timer_ }, {}, {});
		}		

		return info;
	}

	Packet::BattleFieldPlayerInfo*  MtBattleField::OnBattleWin(MtPlayer * player)
	{
		if (player == nullptr) {
			return nullptr;
		}

		auto info = GetBattlePlayerInfo(player->Guid());
		if (info == nullptr) {
			return nullptr;
		}

		info->set_locked(false);
		
		info->set_dead(false);
		auto kill_num = info->kill_num();
		info->set_kill_num(kill_num + 1);
		info->set_honor(info->honor() + 10);

		//ͬ�����proxy
		Packet::BattleFeildProxy bproxy;
		bproxy.set_raid_rtid(raid_rtid_);
		bproxy.set_bf_state(Config::BFState::Bf_Normal);
		player->SetBattleFeildProxy(bproxy);

		//ս��ʤ���ٶ�����30%			
		auto proxy = player->GetSimpleImpactSet();
		//BfSpeedUpImpact
		proxy->AddSimpleImpact(1001, 5, 1200, 0, false);

		BroadcastMsg(*info);

		return info;
	}

	void MtBattleField::OnSuccessPickFlag(MtPlayer * player)
	{
		if (player == nullptr) {
			return;
		}

		OnPickFlag(player);
		
		int camp = CheckCamp(player->Guid());
		if (camp == 1) {
			Packet::Notify notify;
			notify.set_notify("attacker_flag_notify|"+ player->Name());
			BroadcastMsg(notify,1);
			notify.set_notify("defender_flag_notify|" + player->Name());
			BroadcastMsg(notify, 0);

			bf_info_.set_defender_flag(false);
			
		}
		else if (camp == 0) {
			Packet::Notify notify;
			notify.set_notify("attacker_flag_notify|" + player->Name());
			BroadcastMsg(notify, 0);
			notify.set_notify("defender_flag_notify|" + player->Name());
			BroadcastMsg(notify, 1);

			bf_info_.set_attacker_flag(false);
		}
		
		//����ս������
		BroadcastMsg(bf_info_);
	}

	void MtBattleField::OnSuccessWinFlag(MtPlayer * player)
	{
		if (player == nullptr) {
			return;
		}

		OnDropFlag(player, true);

		//�㲥֪ͨ
		int camp = CheckCamp(player->Guid());
		if (camp == 1) {
			Packet::Notify notify;
			notify.set_notify("attacker_getflag_notify|" + player->Name());
			BroadcastMsg(notify, 1);
			notify.set_notify("defender_getflag_notify|" + player->Name());
			BroadcastMsg(notify, 0);

			bf_info_.set_defender_flag(true);

			//
			bf_info_.set_attacker_flag_count(bf_info_.attacker_flag_count() + 1);
			bf_info_.set_attacker_point(bf_info_.attacker_point() + 100);
		}
		else if (camp == 0) {
			Packet::Notify notify;
			notify.set_notify("attacker_getflag_notify|" + player->Name());
			BroadcastMsg(notify, 0);
			notify.set_notify("defender_getflag_notify|" + player->Name());
			BroadcastMsg(notify, 1);

			bf_info_.set_attacker_flag(true);

			bf_info_.set_defender_flag_count(bf_info_.defender_flag_count() + 1);
			bf_info_.set_defender_point(bf_info_.defender_point() + 100);
		}

		//
		auto scene = player->Scene();
		auto bf_config = MtBattleFieldManager::Instance().FindBfConfig(player->PlayerLevel());
		if (scene != nullptr && bf_config != nullptr ) {
			
			scene->QueueInLoop([=]() {
				player->AddItemByIdWithNotify(Packet::TokenType::Token_Exp,
					bf_config->get_flag_exp(), 
					Config::ItemAddLogType::AddType_BattleFeild,
					player->Guid()); });
			scene->QueueInLoop([=]() {
				player->AddItemByIdWithNotify(Packet::TokenType::Token_Gold,
					bf_config->get_flag_gold(),
					Config::ItemAddLogType::AddType_BattleFeild, player->Guid());
			});
		}

		//����ս������
		BroadcastMsg(bf_info_);
		
		//ս��ʤ���ж�
		if( bf_info_.attacker_flag_count() >= 3 || bf_info_.defender_flag_count() >= 3){
			ResultCalc();
		}
	}

	void MtBattleField::OnBattleOver(MtPlayer * winner, MtPlayer * losser)
	{
		if (winner == nullptr || losser == nullptr)
			return;

		auto brlosser  = GetBattlePlayerInfo(losser->Guid()); 	
		auto brwinner = GetBattlePlayerInfo(winner->Guid());

		if (brwinner == nullptr) {
			return winner->SendCommonResult(Packet::ResultOption::BattleField_Opt, Packet::ResultCode::NotJoinRoom);
		}
		if (brlosser == nullptr) {
			return losser->SendCommonResult(Packet::ResultOption::BattleField_Opt, Packet::ResultCode::NotJoinRoom);
		}
		
		//�ɹ���������
		if (brlosser->has_flag()) {
			auto return_num = brwinner->return_flag_num();
			brwinner->set_return_flag_num(return_num + 1);

			auto camp = CheckCamp(brwinner->guid());
			if (camp == 1) {
				bf_info_.set_attacker_flag(true);
				bf_info_.set_attacker_point(bf_info_.attacker_point() + 10);
				bf_info_.set_defender_point(bf_info_.defender_point() + 5);
			}
			else if (camp == 0) {
				bf_info_.set_defender_flag(true);
				bf_info_.set_defender_point(bf_info_.defender_point() + 10);
				bf_info_.set_attacker_point(bf_info_.attacker_point() + 5);
			}

			//����ս������
			BroadcastMsg(bf_info_);
		}	
		OnBattleLoss(losser);
		OnBattleWin(winner);

		auto bf_config = MtBattleFieldManager::Instance().FindBfConfig(winner->PlayerLevel());
		auto scene = winner->Scene();
		if (scene != nullptr && bf_config != nullptr) {
			scene->QueueInLoop([=]() {
				winner->AddItemByIdWithNotify(Packet::TokenType::Token_Exp,
					bf_config->battle_win_exp(),
					Config::ItemAddLogType::AddType_BattleFeild, winner->Guid());
			});
			scene->QueueInLoop([=]() {
				winner->AddItemByIdWithNotify(Packet::TokenType::Token_Gold,
					bf_config->battle_win_gold(),
					Config::ItemAddLogType::AddType_BattleFeild, winner->Guid());
			});
		}
	}

	void MtBattleField::EndAllBattle()
	{
		auto scene = MtSceneManager::Instance().GetSceneById(room_info_.scene_id());
		if (scene != nullptr) {
			auto message = boost::make_shared<S2S::S2SCommonMessage>();
			message->set_request_name("S2SRaidEndAllBattle");
			message->add_int64_params(raid_rtid_);
			scene->ExecuteMessage(message, nullptr);
		}
	}
	void MtBattleField::ResultCalc()
	{
		//ǿ�ƽ�������ս��
		EndAllBattle();

		//����
		OnBFOver();

		Packet::BfResult result;
		int32 winner = -1;
		if (bf_info_.attacker_flag_count() > bf_info_.defender_flag_count()) {
			winner = 1;
		}
		else if (bf_info_.attacker_flag_count() < bf_info_.defender_flag_count()) {
			winner = 0;
		}
		else {
			if (bf_info_.attacker_point() > bf_info_.defender_point() ) {
				winner = 1;
			}
			else if (bf_info_.attacker_point() < bf_info_.defender_point()) {
				winner = 0;
			}
			else
			{
				if (bf_info_.attackers_size() > bf_info_.defenders_size()) {
					winner = 1;
				}
				else if (bf_info_.attackers_size() < bf_info_.defenders_size()) {
					winner = 0;
				}
			}
		}

		result.set_winner(winner);
		BroadcastMsg(result);
		room_info_.set_bf_state(Packet::BattleFieldState::bf_state_wait_leave);

		DoReward(winner);

		//////////////
		auto scene = MtSceneManager::Instance().GetSceneById(room_info_.scene_id());
		if (scene != nullptr) {
			auto raid = scene->GetRaid(raid_rtid_);
			if (raid != nullptr) {
				raid->SetRaidTimer(999, 60, -1);
			}
		}

		//����������Ϣ
		Packet::LuaReplyPacket reply;
		reply.set_reply_name("raid_mission_over");
		reply.add_int32_params(-1);
		BroadcastMsg(reply);
	}

	void MtBattleField::DoReward(int32 winner)
	{		
		int32 attacker_reward = 50;
		int32 defender_reward = 50;

		//������������
		if (winner == 1) {
			attacker_reward = 100;
		}
		else if (winner == 0) {
			defender_reward = 100;
		}

		//mvp
		int32 max_honor[2] = { 0 };
		boost::shared_ptr<MtPlayer> mvp[2] = { 0 };

		for (auto child : bf_info_.attackers()) {
			auto info = GetBattlePlayerInfo(child);
			if (info != nullptr) {
				info->set_honor(info->honor() + attacker_reward);
				BroadcastMsg(*info);
				auto player = Server::Instance().FindPlayer(child);
				if (player != nullptr) {
					//�¿�
					auto final_reward = info->honor();
					auto dc_container = player->DataCellContainer();
					if (dc_container != nullptr && dc_container->check_bit_data(Packet::BitFlagCellIndex::MonthCardFlag)) {
						final_reward = (int32)((float)final_reward*(1.0 + 0.05));
					}
					auto scene = player->Scene();
					if (scene != nullptr) {
						scene->QueueInLoop([=]() {
							player->AddItemByIdWithNotify(
								Packet::TokenType::Token_Honor,
								final_reward,
								Config::ItemAddLogType::AddType_BattleFeild,
								winner);
						});
					}
					if (info->honor() > max_honor[1]) {
						max_honor[1] = info->honor();
						mvp[1] = player;
					}
				}

			}
		}

		for (auto child : bf_info_.defenders()) {
			auto info = GetBattlePlayerInfo(child);
			if (info != nullptr) {
				info->set_honor(info->honor() + defender_reward);
				BroadcastMsg(*info);
				auto player = Server::Instance().FindPlayer(child);
				if (player != nullptr) {
					//�¿�
					auto final_reward = info->honor();
					auto dc_container = player->DataCellContainer();
					if (dc_container != nullptr && dc_container->check_bit_data(Packet::BitFlagCellIndex::MonthCardFlag)) {
						final_reward = (int32)((float)final_reward*(1.0 + 0.05));
					}
					auto scene = player->Scene();
					if (scene != nullptr) {
						scene->QueueInLoop([=]() {
							player->AddItemByIdWithNotify(
								Packet::TokenType::Token_Honor,
								final_reward,
								Config::ItemAddLogType::AddType_BattleFeild,
								winner);
						});
					}

					if (info->honor() > max_honor[0]) {
						max_honor[0] = info->honor();
						mvp[0] = player;
					}
				}
			}
		}

		//�����ҽ�������
		if (winner == 1) {
			for (auto child : bf_info_.attackers()) {
				auto player = Server::Instance().FindPlayer(child);
				if (player != nullptr) {
					auto bf_config = MtBattleFieldManager::Instance().FindBfConfig(player->PlayerLevel());
					auto scene = player->Scene();
					if (scene != nullptr && bf_config != nullptr) {
						scene->QueueInLoop([=]() {
							player->AddItemByIdWithNotify(
								Packet::TokenType::Token_Exp,
								bf_config->win_exp(),
								Config::ItemAddLogType::AddType_BattleFeild,
								winner);
						});
						scene->QueueInLoop([=]() {
							player->AddItemByIdWithNotify(
								Packet::TokenType::Token_Gold,
								bf_config->win_gold(),
								Config::ItemAddLogType::AddType_BattleFeild,
								winner);
						});
					}
				}
			}
			for (auto child : bf_info_.defenders()) {
				auto player = Server::Instance().FindPlayer(child);
				if (player != nullptr) {
					auto bf_config = MtBattleFieldManager::Instance().FindBfConfig(player->PlayerLevel());
					auto scene = player->Scene();
					if (scene != nullptr && bf_config != nullptr) {
						scene->QueueInLoop([=]() {
							player->AddItemByIdWithNotify(
								Packet::TokenType::Token_Exp,
								bf_config->lose_exp(),
								Config::ItemAddLogType::AddType_BattleFeild,
								winner);
						});
						scene->QueueInLoop([=]() {
							player->AddItemByIdWithNotify(
								Packet::TokenType::Token_Gold,
								bf_config->lose_gold(),
								Config::ItemAddLogType::AddType_BattleFeild,
								winner);
						});
					}
				}
			}
		
			//�㲥
			if (mvp[1] != nullptr) {
				std::stringstream msg;
				msg << "ac_notify_026|" << mvp[1]->Name();
				send_run_massage(msg.str());
			}
		}
		else if (winner == 0) {
			for (auto child : bf_info_.attackers()) {
				auto player = Server::Instance().FindPlayer(child);
				if (player != nullptr) {
					auto bf_config = MtBattleFieldManager::Instance().FindBfConfig(player->PlayerLevel());
					auto scene = player->Scene();
					if (scene != nullptr && bf_config != nullptr) {
						scene->QueueInLoop([=]() {
							player->AddItemByIdWithNotify(
								Packet::TokenType::Token_Exp,
								bf_config->lose_exp(),
								Config::ItemAddLogType::AddType_BattleFeild,
								winner); 
						});
						scene->QueueInLoop([=]() {
							player->AddItemByIdWithNotify(
								Packet::TokenType::Token_Gold,
								bf_config->lose_gold(),
								Config::ItemAddLogType::AddType_BattleFeild,
								winner);
						});
					}
				}
			}
			for (auto child : bf_info_.defenders()) {
				auto player = Server::Instance().FindPlayer(child);
				if (player != nullptr) {
					auto bf_config = MtBattleFieldManager::Instance().FindBfConfig(player->PlayerLevel());
					auto scene = player->Scene();
					if (scene != nullptr && bf_config != nullptr) {
						scene->QueueInLoop([=]() {
							player->AddItemByIdWithNotify(
								Packet::TokenType::Token_Exp,
								bf_config->win_exp(),
								Config::ItemAddLogType::AddType_BattleFeild,
								winner); 
						});
						scene->QueueInLoop([=]() {
							player->AddItemByIdWithNotify(
								Packet::TokenType::Token_Gold,
								bf_config->win_gold(),
								Config::ItemAddLogType::AddType_BattleFeild,
								winner);
						});
					}
				}
			}
	
			//�㲥
			if (mvp[0] != nullptr) {
				std::stringstream msg;
				msg << "ac_notify_026|" << mvp[0]->Name();
				send_run_massage(msg.str());
			}
		}		
	}
	bool MtBattleField::OnJoin(MtPlayer * player)
	{
		if (player == nullptr) {
			return false;
		}
		bool isjoined = false;
		for (int32 i = 0; i < room_info_.max_count(); i++) {
			auto pi = room_info_.mutable_players(i);
			if (pi != nullptr) {
				if (pi->guid() == player->Guid()) {
					isjoined = true;
					break;
				}
			}
		}
		if (!isjoined) {

			for (int32 i = 0; i < room_info_.max_count(); i++) {
				auto pi = room_info_.mutable_players(i);
				if (pi != nullptr) {

					if (pi->guid() == INVALID_GUID) {
						pi->set_guid(player->Guid());
						pi->set_honor(0);
						pi->set_kill_num(0);
						pi->set_dead_num(0);
						pi->set_get_flag_num(0);
						pi->set_return_flag_num(0);
						pi->set_has_flag(false);
						pi->set_bf_state(Packet::BattleFieldState::bf_state_inroom);
						pi->set_locked(false);
						pi->set_dead(false);
						pi->set_old_camp(-1);
						pi->set_name(player->Name());
						pi->set_prof(player->MainActor()->Professions());

						//֪ͨ
						BroadcastMsg(*pi);

						return true;
					}
				}
			}
		}

		return false;
	}

	bool MtBattleField::OnLeave(MtPlayer * player)
	{
		if (player == nullptr)
			return false;

		auto pinfo = GetBattlePlayerInfo(player->Guid());
		if (pinfo == nullptr) {
			return false;
		}

		//���뿪ս��
		if (room_info_.bf_state() == Packet::BattleFieldState::bf_state_inbattle) {

			for (int32 i = 0; i < bf_info_.attackers_size(); i++) {
				if (bf_info_.attackers(i) == player->Guid()) {
					//bf_info_.set_attackers(i, INVALID_GUID);	
					if (pinfo->has_flag()) {//Ҫ����
						bf_info_.set_defender_flag(true);
					}
					break;
				}
			}
			for (int32 i = 0; i < bf_info_.defenders_size(); i++) {
				if (bf_info_.defenders(i) == player->Guid()) {
					//bf_info_.set_defenders(i, INVALID_GUID);
					if (pinfo->has_flag()) {//Ҫ����
						bf_info_.set_attacker_flag(true);
					}
					break;
				}
			}

			BroadcastMsg(bf_info_);
		}


		//��������
		OnDropFlag(player, false);
		
		//֪ͨ
		BroadcastMsg(*pinfo);

		//ͬ�����proxy
		Packet::BattleFeildProxy bproxy;
		bproxy.set_raid_rtid(raid_rtid_);
		bproxy.set_bf_state(Config::BFState::Bf_Leave);
		player->SetBattleFeildProxy(bproxy);

		return false;
	}

	void MtBattleField::OnBFOver()
	{

		for (int32 i = 0; i < bf_info_.attackers_size(); i++) {
			auto pinfo = GetBattlePlayerInfo(bf_info_.attackers(i));
			if (pinfo == nullptr) {
				continue;
			}

			if (pinfo->has_flag()) {//Ҫ����
				bf_info_.set_defender_flag(true);

				//��������
				auto player = Server::Instance().FindPlayer(pinfo->guid()); 
				if(player != nullptr){
					OnDropFlag(player.get(), false);
				}		
				else {
					//֪ͨ
					BroadcastMsg(*pinfo);
				}
			}
		}

		for (int32 i = 0; i < bf_info_.defenders_size(); i++) {
			auto pinfo = GetBattlePlayerInfo(bf_info_.defenders(i));
			if (pinfo == nullptr) {
				continue;
			}
			if (pinfo->has_flag()) {//Ҫ����
				bf_info_.set_attacker_flag(true);

				//��������
				auto player = Server::Instance().FindPlayer(pinfo->guid());
				if (player != nullptr){
					OnDropFlag(player.get(), false);
				}
				else {
					//֪ͨ
					BroadcastMsg(*pinfo);
				}

			}
		}

		BroadcastMsg(bf_info_);
	}

	bool MtBattleField::OnJoinBattle(const uint64 guid)
	{
		auto pi = GetBattlePlayerInfo(guid);
		if (pi != nullptr) {
			int32 camp = pi->old_camp();
			if (camp == -1) {
				//���ȼ������ٵ�һ������Ⱦ��������һ��
				if (bf_info_.defenders_size() == bf_info_.attackers_size()) {
					camp= rand_gen->intgen(0, 1);
				}
				else if (bf_info_.defenders_size() < bf_info_.attackers_size()) {
					camp = 0;
				}
				else {
					camp = 1;
				}
				pi->set_old_camp(camp);
			}		

			bool already_join = false;
			bool join_ok = false;
			if (camp == 0) {	
				for (int32 i = 0; i < bf_info_.defenders_size(); i++) {
					if (bf_info_.defenders(i) == guid) {
						already_join = true;
						break;
					}
				}
				if (!already_join) {
					for (int32 i = 0; i < bf_info_.defenders_size(); i++) {
						if (bf_info_.defenders(i) == INVALID_GUID) {
							bf_info_.set_defenders(i, guid);
							join_ok = true;
							break;
						}
					}
					if (!join_ok) {
						bf_info_.add_defenders(guid);
					}
				}
			}
			else {
				for (int32 i = 0; i < bf_info_.attackers_size(); i++) {
					if (bf_info_.attackers(i) == guid) {
						already_join = true;
						break;
					}
				}
				if(!already_join){
					for (int32 i = 0; i < bf_info_.attackers_size(); i++) {
						if (bf_info_.attackers(i) == INVALID_GUID) {
							bf_info_.set_attackers(i, guid);
							join_ok = true;
							break;
						}
					}
					if (!join_ok) {
						bf_info_.add_attackers(guid);
					}
				}
			}
		}

		BroadcastMsg(bf_info_);
		return true;
	}

	bool MtBattleField::OnLeaveBattleField(MtPlayer * player)
	{
		if (player == nullptr)
			return false;


		auto pinfo = GetBattlePlayerInfo(player->Guid());
		if (pinfo == nullptr) {
			return false;
		}

		for (int32 i = 0; i < bf_info_.attackers_size(); i++) {
			if (bf_info_.attackers(i) == player->Guid()) {
				//bf_info_.set_attackers(i, INVALID_GUID);	
				if (pinfo->has_flag()) {//Ҫ����
					bf_info_.set_defender_flag(true);
				}				
				break;
			}
		}
		for (int32 i = 0; i < bf_info_.defenders_size(); i++) {
			if (bf_info_.defenders(i) == player->Guid()) {
				//bf_info_.set_defenders(i, INVALID_GUID);
				if (pinfo->has_flag()) {//Ҫ����
					bf_info_.set_attacker_flag(true);
				}				
				break;
			}
		}

		BroadcastMsg(bf_info_);

		//��������
		OnDropFlag(player, false);	
		
		return true;
	}

	
	std::vector<MtPlayer*> MtBattleField::GetPlayers() {
		std::vector<MtPlayer*> players;
		for (int32 i = 0; i < room_info_.max_count(); i++) {
			auto pi = room_info_.mutable_players(i);
			if (pi != nullptr) {
				auto p = Server::Instance().FindPlayer(pi->guid());
				if (p != nullptr) {
					players.push_back(p.get());
				}
			}
		}
		return players;
	}

	void MtBattleField::AlignPlayers()
	{
		std::vector<std::pair<uint64, int32>> ptmap;
		for (int32 i = 0; i < room_info_.max_count(); i++) {
			auto pi = room_info_.mutable_players(i);
			if (pi != nullptr) {
				auto p = Server::Instance().FindPlayer(pi->guid());
				if (p != nullptr) {
					int32 power = p->GetActorFormationPower(Packet::ActorFigthFormation::AFF_PVP_BATTLE_FIELD);
					ptmap.push_back({ pi->guid(),power });
				}
			}
		}
		
		std::sort(std::begin(ptmap), std::end(ptmap), [](auto& lhs, auto& rhs)
		{
			return lhs.second < rhs.second;
		});

		//��������
		int32 i = 0;
		for (auto child: ptmap) {
			if (i % 2 == 0) {
				bf_info_.add_attackers(child.first);
				auto pi = GetBattlePlayerInfo(child.first);
				if (pi != nullptr) {
					pi->set_old_camp(1);
				}
			}
			else {
				bf_info_.add_defenders(child.first);
				auto pi = GetBattlePlayerInfo(child.first);
				if (pi != nullptr) {
					pi->set_old_camp(0);
				}
			}
			i++;
		}
	}

	void MtBattleField::PickFlagCheck(std::map<uint64, int32>& queue, int32 elapse_second)
	{		
		std::vector<MtPlayer*> cancels;
		bool success = false;
		for (auto iter = queue.begin(); iter != queue.end(); ) {
			auto player = Server::Instance().FindPlayer(iter->first);
			if (player == nullptr) {
				continue;
			}
			auto data = player->GetScenePlayerInfo();
			if (data->player_status() != Packet::PlayerSceneStatus::IDEL || success) {
				//���˾�ȡ��,���˳ɹ��������˶�ȡ��
				cancels.push_back(player.get());
			}			
			else {
				iter->second += elapse_second;
				if (iter->second > 10) {
					//��������
					success = true;
					OnSuccessPickFlag(player.get());

					iter=queue.erase(iter);
					
					continue;
				}
			}
			++iter;
		}

		for (auto &child : cancels) {
			OnCancelPickFlag(child);
		}
	}

	void MtBattleField::EndCheck(int32 elapse_second)
	{
		
		attacker_timer_ += elapse_second;
		defender_timer_ += elapse_second;
		wait_timer_ += (int32)elapse_second;

		for (int32 i = 0; i < bf_info_.attackers_size(); i++) {
			if (bf_info_.attackers(i) != INVALID_GUID) {
				attacker_timer_ = 0;
				break;				
			}
		}

		for (int32 i = 0; i < bf_info_.defenders_size(); i++) {
			if (bf_info_.defenders(i) != INVALID_GUID) {
				defender_timer_ = 0;
				break;
			}
		}

		//ս����ǰ��������������
		//3����һ��û�˾ͽ���
		if (attacker_timer_ >= 180 || defender_timer_ >= 180 ) {
			ResultCalc();
			wait_timer_ = 0;
		}


		//������ս��ʱ���������������
		if (wait_timer_ >= 1800) {
			defender_timer_ = 0;
			attacker_timer_ = 0;
			wait_timer_ = 0;
			ResultCalc();
		}

	}
	
	void MtBattleField::ReliveCheck(int32 elapse_second)
	{
		auto scene = MtSceneManager::Instance().GetSceneById(room_info_.scene_id());
		if (scene == nullptr) {
			return;
		}
		relive_timer_ += elapse_second;
		if (relive_timer_ >=30 ) {
			relive_timer_ = 0;
			for (int32 i = 0; i < room_info_.max_count(); i++) {
				auto pi = room_info_.mutable_players(i);
				if (pi != nullptr && pi->dead()){
					auto player = scene->FindPlayer(pi->guid());
					if (player != nullptr) {
						OnRelive(player.get());
						//���͵������
						OnGoGrave(player.get());
					}					
				}
			}
		}
	}

	void MtBattleField::OnBattlePrepare()
	{
		room_info_.set_bf_state(Packet::BattleFieldState::bf_state_room_waiting);

		AlignPlayers();
		
		auto scene = MtSceneManager::Instance().GetSceneById(room_info_.scene_id());
		if (scene == nullptr) {
			return;
		}
		std::vector<uint64> inner_player;
		for (int32 i = 0; i < room_info_.max_count(); i++) {
			auto pi = room_info_.mutable_players(i);
			if (pi != nullptr) {
				if (scene->FindPlayer(pi->guid()) != nullptr) {
					pi->set_bf_state(Packet::BattleFieldState::bf_state_room_waiting);//ս����Ĭ��ͬ��
					inner_player.push_back(pi->guid());
				}
				else {
					pi->set_bf_state(Packet::BattleFieldState::bf_state_disagree);//˵��Ĭ���ǲ�ͬ�����ս���ģ���Ȼ�����ǰ�л���ȥս�������ߺ�������			
				}
			}
		}
		//ս����������
		bf_info_.set_attacker_flag(true);
		bf_info_.set_defender_flag(true);
		bf_info_.set_attacker_flag_count(0);
		bf_info_.set_defender_flag_count(0);
		bf_info_.set_attacker_point(0);
		bf_info_.set_defender_point(0);
		BroadcastMsg(bf_info_);

		//����ս������ʱ120s,����cdУ��
		Packet::EnterBattleFieldNotify msg;
		msg.set_time(120);
		BroadcastMsg(msg,-1, inner_player);
	}

	void MtBattleField::OnBattleEnter()
	{
		//����ҡ��
		Packet::LuaReplyPacket reply;
		reply.set_reply_name("ForbidenJoy");
		BroadcastMsg(reply);

		//��ʼ����ʣ����ҽ�ս���ĸ���λ��
		for (int32 i = 0; i < room_info_.max_count(); i++) {
			auto pi = room_info_.mutable_players(i);
			if (pi != nullptr) {
				auto player = Server::Instance().FindPlayer(pi->guid());
				if (player == nullptr) {
					continue;
				}
				if (pi->bf_state() == Packet::BattleFieldState::bf_state_room_waiting) {				
					MtBattleFieldManager::Instance().TryEnterBattleField(player.get());
				}
				else if( pi->bf_state() == Packet::BattleFieldState::bf_state_disagree ){
					//ûͬ��Ĵ�����ս��,�뿪ս�����ؽ������Ի�ս��
					//auto msg = boost::make_shared<Packet::LeaveBattleField>();
					//Server::Instance().FakeClientMessage(player->Session(), msg);
					Server::Instance().SendS2GCommonMessage("S2GQuitBattleField", {}, { (int64)player->Guid() }, {});
				}
			}
		}

		//��enter��state
		room_info_.set_bf_state(Packet::BattleFieldState::bf_state_battle_waiting);
		BroadcastMsg(room_info_);

		//��ʼս������ʱ30s
		Packet::BeginBattleFieldNotify msg;
		msg.set_time(30);
		BroadcastMsg(msg);

		//����ս��������ʼʱ��
		auto scene = MtSceneManager::Instance().GetSceneById(GetSceneId());
		if (scene != nullptr) {
			auto raid = scene->GetRaid(GetId());
			if (raid != nullptr) {
				raid->RegenBeginDate();
			}
		}
	}

	void MtBattleField::OnBeginBattleField()
	{
		room_info_.set_bf_state(Packet::BattleFieldState::bf_state_inbattle);

		//��ʽ��ʼս��
		Packet::BeginBattleField msg;
		BroadcastMsg(msg);
		BroadcastMsg(room_info_);

		auto scene = MtSceneManager::Instance().GetSceneById(room_info_.scene_id());
		if (scene == nullptr) {
			return;
		}
		//ս����ʼ30%����
		for (int32 i = 0; i < bf_info_.attackers_size(); i++) {
			auto player = scene->FindPlayer(bf_info_.attackers(i));
			if (player != nullptr) {
				auto proxy = player->GetSimpleImpactSet();
				//BfSpeedUpImpact
				proxy->AddSimpleImpact(1001, 5, 1200, 0, false);
			}			
		}

		for (int32 i = 0; i < bf_info_.defenders_size(); i++) {
			auto player = scene->FindPlayer(bf_info_.defenders(i));
			if (player != nullptr) {
				auto proxy = player->GetSimpleImpactSet();
				//BfSpeedUpImpact
				proxy->AddSimpleImpact(1001, 5, 1200, 0, false);
			}
		}

		scene->SendS2SCommonMessage("S2SGenRaidBuff", { raid_rtid_ }, {}, {});
	}
	
	void MtBattleField::OnForceBegin()
	{
		if (room_info_.bf_state() == Packet::BattleFieldState::bf_state_inroom){
			wait_timer_ = 999999;
		}
	}

	Packet::BattleFieldPlayerInfo* MtBattleField::GetBattlePlayerInfo(const uint64 guid)
	{
		for (int32 i = 0; i < room_info_.max_count(); i++) {
			auto pi = room_info_.mutable_players(i);
			if (pi != nullptr) {
				if (pi->guid() == guid) {
					return pi;
				}
			}
		}
		return nullptr;
	}

	void MtBattleField::reset() {
		raid_rtid_ = INVALID_GUID;
		room_info_.set_min_level(-1);
		room_info_.set_max_level(-1);
		auto count = room_info_.GetDescriptor()->FindFieldByName("players")->options().GetExtension(Packet::column_count);
		room_info_.set_max_count(count);
		room_info_.set_scene_id(-1);

		room_info_.clear_players();
		for (int32 i = 0; i < count; i++) {
			auto pi = room_info_.add_players();
			if (pi != nullptr) {
				pi->set_guid(INVALID_GUID);
				pi->set_honor(0);
				pi->set_kill_num(0);
				pi->set_dead_num(0);
				pi->set_get_flag_num(0);
				pi->set_return_flag_num(0);
				pi->set_has_flag(false);
				pi->set_bf_state(Packet::BattleFieldState::bf_state_invalid);
				pi->set_locked(false);
				pi->set_dead(false);
				pi->set_old_camp(-1);
				pi->set_name("");
				pi->set_prof(Packet::Professions::Non_Prof);
			}
		}

		bf_info_.set_attacker_flag(false);
		bf_info_.set_defender_flag(false);
		bf_info_.set_attacker_flag_count(0);
		bf_info_.set_defender_flag_count(0);
		bf_info_.set_attacker_point(0);
		bf_info_.set_defender_point(0);

		wait_timer_ = 0;
	}	


	void MtBattleField::OnGoGrave(MtPlayer * player)
	{
		if (player == nullptr) {
			return;
		}

		auto camp = CheckCamp(player->Guid());
		if (camp < 0) {
			return;
		}

		auto player_scene = player->Scene();
		if (player_scene != nullptr)
		{
			int pos_index = MtBattleField::GetPosIndex(camp);
			auto init_pos = player_scene->Config()->spawn_pos(pos_index);

			Server::Instance().SendG2SCommonMessage(player, "G2SGoTo", { player_scene->SceneId(), init_pos.x(),init_pos.y(),init_pos.z() }, {}, {});
		}
	}
}