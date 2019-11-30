#include "ClientMove.pb.h"
#include "PlayerBasicInfo.pb.h"
#include "G2SMessage.pb.h"
#include "ActorBasicInfo.pb.h"
#include "MonsterConfig.pb.h"
#include "SceneCommon.pb.h"
#include "SceneObject.pb.h"
#include "BaseConfig.pb.h"
#include <EnterScene.pb.h>
#include <ActorConfig.pb.h>
#include <Team.pb.h>
#include <DataCellPacket.pb.h>
#include <DrawCard.pb.h>
#include <LootMessage.pb.h>
#include <HeroFragment.pb.h>
#include <HeroFragmentConfig.pb.h>
#include <SceneStageConfig.pb.h>
#include <SceneStage.pb.h>
#include "Guild.pb.h"
#include "WorldBoss.pb.h"
#include "GuildConfig.pb.h"
#include <S2GMessage.pb.h>
#include "../mt_guid/mt_guid.h"
#include "active_model.h"
#include "../mt_config/mt_config.h"
#include "../mt_server/mt_server.h"
#include "../mt_item/mt_item.h"
#include "../mt_item/mt_container.h"
#include "../mt_market/mt_market.h"
#include "../mt_item/mt_item_package.h"
#include "../mt_upgrade/mt_upgrade.h"
#include "../base/mt_timer.h"
#include "../mt_stage/mt_scene_stage.h"
#include "../mt_stage/mt_player_stage.h"
#include "../mt_activity/mt_draw.h"
#include "../data/luafile_manager.h"
#include "mt_player.h"
#include "../mt_monster/mt_npc.h"
#include "../mt_actor/mt_actor.h"
#include "../mt_scene/mt_scene.h"
#include "../mt_scene/mt_logic_area.h"
#include "../mt_scene/mt_zone.h"
#include "../mt_scene/mt_raid.h"
#include "../mt_trial_field/mt_trial_field.h"
#include "../mt_server/mt_server.h"
#include "../mt_battle/mt_battle_ground.h"
#include "../base/mt_db_load_work.h"
#include "../mt_actor/mt_actor_config.h"
#include "../mt_skill/mt_skill.h"
#include "../mt_arena/mt_arena.h"
#include "../base/client_session.h"
#include "../base/mt_db_save_work.h"
#include "../friend/FriendContainer.h"
#include "../friend/mt_friend_manager.h"
#include "../mail/MessageContainer.h"
#include "../mail/static_message_manager.h"
#include "../mt_monster/mt_monster_manager.h"
#include "../mt_mission/mt_mission.h"
#include "../mt_mission/mt_mission_proxy.h"
#include "../mt_mission/mt_arrange_proxy.h"
#include "../mt_scene/mt_scene_manager.h"
#include "../mt_team/mt_team_manager.h"
#include "../mt_data_cell/mt_data_cell.h"
#include "../mt_data_cell/mt_data_container.h"
#include "../mt_guild/mt_guild_manager.h"
#include "../mt_guild/mt_player_guild_proxy.h"
#include "../mt_activity/mt_activity.h"
#include "../mt_battle/mt_worldboss_manager.h"
#include "../mt_chat/mt_chatmessage.h"
#include "../mt_chat/mt_word_manager.h"
#include "../mt_activity/mt_activity.h"
#include "../mt_activity/mt_operative_config.h"
#include "../mt_battle_field/mt_battle_field.h"
#include "../mt_battle_field/mt_battle_field_manager.h"
#include "../mt_cache/mt_shm_manager.h"
#include "mt_player_offline_ai.h"
#include "../ranklist/top_ranklist.h"
#include "../base/mt_game_urlwork.h"

#include "utils.h"
#include <ActivityPacket.pb.h>
#include <ActivityConfig.pb.h>
#include <ActivityPacket.proto.fflua.h>
#include <ItemConfig.pb.h>
#include <ItemConfig.proto.fflua.h>
#include <MissionConfig.proto.fflua.h>
#include <SceneCommon.proto.fflua.h>
#include <SceneObject.proto.fflua.h>
#include <BattleField.proto.fflua.h>
#include <boost/make_shared.hpp>
#include <algorithm> // std::move_backward  
#include <random> // std::default_random_engine  
#include <chrono> // std::chrono::system_clock 

namespace Mt
{

	void MtPlayer::lua_reg(lua_State* ls)
	{
		ff::fflua_register_t<MtPlayer, ctor()>(ls, "MtPlayer")
			.def(&MtPlayer::LuaDispatchMessage, "DispatchMessage")
			.def(&MtPlayer::LuaExecuteMessage, "ExecuteMessage")
			.def(&MtPlayer::LuaSendMessage, "SendMessage")
			.def(&MtPlayer::SendLogMessage, "SendLogMessage")
			.def(&MtPlayer::GetTeamStatus, "GetTeamStatus")
			.def(&MtPlayer::OnChargeOk, "OnChargeOk")
			.def(&MtPlayer::GetAllPracticeInfo,"GetAllPracticeInfo")
			.def(&MtPlayer::CreateRobotTeam, "CreateRobotTeam")
			.def(&MtPlayer::CurrentRobotCount, "CurrentRobotCount")
			.def(&MtPlayer::DismissTeam, "DismissTeam")
			.def(&MtPlayer::GetPlayerMaxStageId, "GetPlayerMaxStageId")		
			.def(&MtPlayer::GetAccount,"GetAccount")
			.def(&MtPlayer::GetAllCharge, "GetAllCharge")
			.def(&MtPlayer::AddAllCharge, "AddAllCharge")
			.def(&MtPlayer::DisConnected, "DisConnected")
			.def(&MtPlayer::CheckAcrDrop,"CheckAcrDrop")
			.def(&MtPlayer::ActivityListRefresh,"ActivityListRefresh")
			.def(&MtPlayer::ActivityListModify, "ActivityListModify")
			.def(&MtPlayer::Guid, "Guid")
			.def(&MtPlayer::Name, "Name")
			.def(&MtPlayer::ServerLevelCheck,"ServerLevelCheck")
			.def(&MtPlayer::GetAllMarkets,"GetAllMarkets")
			.def(&MtPlayer::SendStatisticsResult,"SendStatisticsResult")
			.def(&MtPlayer::StatisticsResolve,"StatisticsResolve")
			.def(&MtPlayer::ResetStatistics,"ResetStatistics")
			.def(&MtPlayer::OnSendWorldBossHpUpdate,"OnSendWorldBossHpUpdate")
			.def(&MtPlayer::FindMarket,"FindMarket")
			.def(&MtPlayer::EquipAndItemBagLeftCheck, "EquipAndItemBagLeftCheck")
			.def(&MtPlayer::OnMinTriger, "OnMinTriger")
			.def(&MtPlayer::OnHourTriger, "OnHourTriger")
			.def(&MtPlayer::OnDayTriger, "OnDayTriger")
			.def(&MtPlayer::OnMonthTriger,"OnMonthTriger")
			.def(&MtPlayer::OnWeekTriger, "OnWeekTriger")
			.def(&MtPlayer::AddPracticeExp,"AddPracticeExp")
			.def(&MtPlayer::GetBattleManager,"GetBattleManager")
			.def(&MtPlayer::ResetActivityData, "ResetActivityData")
			.def(&MtPlayer::CanAutoHook, "CanAutoHook")
			.def(&MtPlayer::GetActorFormation,"GetActorFormation")
			.def(&MtPlayer::GetActorFormationPower, "GetActorFormationPower")
			.def(&MtPlayer::GetSceneId, "GetSceneId")
			.def(&MtPlayer::GetMoveSpeed, "GetMoveSpeed")
			.def(&MtPlayer::SetMoveSpeed, "SetMoveSpeed")
			.def(&MtPlayer::ResetSpeed, "ResetSpeed")
			.def(&MtPlayer::GetMessageContainer,"GetMessageContainer")
			.def(&MtPlayer::GetMissionProxy, "GetMissionProxy")
			.def(&MtPlayer::GetArrangeProxy, "GetArrangeProxy")
			.def(&MtPlayer::GetPlayerStageProxy, "GetPlayerStageProxy")
			.def(&MtPlayer::HookBattle, "HookBattle")
			.def(&MtPlayer::QuitHookBattle, "QuitHookBattle")
			.def(&MtPlayer::StartHookBattle, "StartHookBattle")
			.def(&MtPlayer::Friend_Container,"Friend_Container")
			.def(&MtPlayer::SetTeamArrangeType, "SetTeamArrangeType")
			.def(&MtPlayer::GetTeamArrangeType, "GetTeamArrangeType")
			.def(&MtPlayer::PlayerLevel, "PlayerLevel")
			.def(&MtPlayer::AddItemById, "AddItemById")
			.def(&MtPlayer::AddItemByIdWithNotify, "AddItemByIdWithNotify")
			.def(&MtPlayer::AddItemByIdWithoutNotify, "AddItemByIdWithoutNotify")
			.def(&MtPlayer::DelItemById, "DelItemById")
			.def(&MtPlayer::GetCamp, "GetCamp")
			.def(&MtPlayer::CopyActorFormation, "CopyActorFormation")
			.def(&MtPlayer::SendClientNotify, "SendClientNotify")
			.def(&MtPlayer::ValidPositionById, "ValidPosition")
			.def(&MtPlayer::IsTeamLeader, "IsTeamLeader")
			.def(&MtPlayer::IsTeamGroup, "IsTeamGroup")
			.def(&MtPlayer::GetScenePlayerInfo, "GetScenePlayerInfo")
			.def(&MtPlayer::FindActor, "FindActor")
			.def(&MtPlayer::GetDBPlayerInfo, "GetDBPlayerInfo")			
			.def(&MtPlayer::BuyMarketItem, "BuyMarketItem")
			.def(&MtPlayer::AddNpc, "AddNpc")
			.def(&MtPlayer::IsLastAnswer,"IsLastAnswer")
			.def(&MtPlayer::ExpPointOption, "ExpPointOption")
			.def(&MtPlayer::GoldOperation, "GoldOperation")
			.def(&MtPlayer::OnDrawCard, "OnDrawCard")
			.def(&MtPlayer::OnInspire, "OnInspire")
			.def(&MtPlayer::OnWorldBossRelive, "OnWorldBossRelive")
			.def(&MtPlayer::OnHellRelive, "OnHellRelive")
			.def(&MtPlayer::OnRepairAllEquip, "OnRepairAllEquip")
			.def(&MtPlayer::GetMissionById, "GetMissionById")
			.def(&MtPlayer::UseItem, "UseItem")
			.def(&MtPlayer::UseItemByCfgId, "UseItemByCfgId")
			.def(&MtPlayer::ActivityBoxReward, "ActivityBoxReward")
			.def(&MtPlayer::DestroyItem, "DestroyItem")
			.def(&MtPlayer::SellItem, "SellItem")
			.def(&MtPlayer::AddOtherNpc, "AddOtherNpc")
			.def(&MtPlayer::AddSceneNpc, "AddSceneNpc")
			.def(&MtPlayer::Actors, "Actors")
			.def(&MtPlayer::AllInsetGems, "AllInsetGems")
			.def(&MtPlayer::ResetPlayerActorLastHp, "ResetPlayerActorLastHp")
			.def(&MtPlayer::GetCurBattleType,"GetCurBattleType")
			.def(&MtPlayer::AddChallengReq, "AddChallengReq")
			.def(&MtPlayer::CheckChallengReq, "CheckChallengReq")
			.def(&MtPlayer::SendCommonResult, "SendCommonResult")
			.def(&MtPlayer::DelNpc, "DelNpc")
			.def(&MtPlayer::DelNpcByGroup, "DelNpcByGroup")
			.def(&MtPlayer::DelNpcByType, "DelNpcByType")
			.def(&MtPlayer::GetRaid, "GetRaid")
			.def(&MtPlayer::GetInBattle,"GetInBattle")
			.def(&MtPlayer::SendCommonReply, "SendCommonReply")
			.def(&MtPlayer::BroadCastCommonReply, "BroadCastCommonReply")			
			.def(&MtPlayer::SendLuaPacket, "SendLuaPacket")
			.def(&MtPlayer::BroadCastLuaPacket,"BroadCastLuaPacket")
			.def(&MtPlayer::DataCellContainer, "DataCellContainer")
			.def(&MtPlayer::BeLoot, "BeLoot")
			.def(&MtPlayer::ResetTrialFieldData,"ResetTrialFieldData")
			.def(&MtPlayer::ResetTrialGBOSSData, "ResetTrialGBOSSData")
			.def(&MtPlayer::UpdateFormationLastHpSet, "UpdateFormationLastHpSet")
			.def(&MtPlayer::OnCreateGuild, "OnCreateGuild")
			.def(&MtPlayer::EndGuildBoss, "EndGuildBoss")
			.def(&MtPlayer::UpdateGuildBossDamage, "UpdateGuildBossDamage")
			.def(&MtPlayer::EndWorldBoss, "EndWorldBoss")
			.def(&MtPlayer::OnWorldBossDamage, "OnWorldBossDamage")
			.def(&MtPlayer::SendGuildBattleStage, "SendGuildBattleStage")
			.def(&MtPlayer::GetActivity, "GetActivity")
			.def(&MtPlayer::MainActor, "MainActor")
			.def(&MtPlayer::OnMissionRaid, "OnMissionRaid")
			.def(&MtPlayer::GetPlayerStages, "GetPlayerStages")
			.def(&MtPlayer::GetPlayerStageDB, "GetPlayerStageDB")
			.def(&MtPlayer::GetPlayerStage,"GetPlayerStage")
			.def(&MtPlayer::OnNpcOptionReply, "OnNpcOptionReply")
			.def(&MtPlayer::FirstStageReward, "FirstStageReward")
			.def(&MtPlayer::DecBossChallengeCount, "DecBossChallengeCount")
			.def(&MtPlayer::GetBossChallengeCount, "GetBossChallengeCount")
			.def(&MtPlayer::GetPlayerNpc, "GetPlayerNpc")
			.def(&MtPlayer::GetPlayerNpcByIndex, "GetPlayerNpcByIndex")			
			.def(&MtPlayer::SetGuideStep, "SetGuideStep")
			.def(&MtPlayer::MarketReq, "MarketReq")
			.def(&MtPlayer::ItemLockReq, "ItemLockReq")
			.def(&MtPlayer::GetBattleFeildProxy, "GetBattleFeildProxy")
			.def(&MtPlayer::RefreshPlayerNpc, "RefreshPlayerNpc")
			.def(&MtPlayer::DelPlayerNpc, "DelPlayerNpc")
			.def(&MtPlayer::DelPlayerNpcByIndex,"DelPlayerNpcByIndex")
			.def(&MtPlayer::CheckRefreshNextWave, "CheckRefreshNextWave")
			.def(&MtPlayer::GetSceneNpcs, "GetSceneNpcs")
			.def(&MtPlayer::GetScenePlayerInfo,"GetScenePlayerInfo")
			.def(&MtPlayer::GetTagNpcs, "GetTagNpcs")
			.def(&MtPlayer::SavePlayerLastHp, "SavePlayerLastHp")
			.def(&MtPlayer::SavePlayerNpcLastHp, "SavePlayerNpcLastHp")
			.def(&MtPlayer::OnArenaBattleDone, "OnArenaBattleDone")
			.def(&MtPlayer::GetTeamLeader, "GetTeamLeader")
			.def(&MtPlayer::GetTeamID, "GetTeamID")
			.def(&MtPlayer::GetTeamMember,"GetTeamMember")
			.def(&MtPlayer::GetBagItemCount, "GetBagItemCount")		
			.def(&MtPlayer::GetGuildID,"GetGuildID")
			.def(&MtPlayer::SetGuildName, "SetGuildName")
			.def(&MtPlayer::GetPlayerGuildProxy,"GetPlayerGuildProxy")
			.def(&MtPlayer::UpdateGuildInfo, "UpdateGuildInfo")
			.def(&MtPlayer::CheckGuildBattleQualify,"CheckGuildBattleQualify")
			.def(&MtPlayer::OnJoinBattleRoom,"OnJoinBattleRoom")	
			.def(&MtPlayer::GetWorldBossReliveCd, "GetWorldBossReliveCd")
			.def(&MtPlayer::OnSendWorldBossBattleReply, "OnSendWorldBossBattleReply")
			.def(&MtPlayer::Refresh2ClientNpcList, "Refresh2ClientNpcList")
			.def(&MtPlayer::OnResetAnswer,"OnResetAnswer")
			.def(&MtPlayer::OnBeginAnswer,"OnBeginAnswer")
			.def(&MtPlayer::OnNextAnswer,"OnNextAnswer")
			.def(&MtPlayer::GetPicResult,"GetPicResult")
			.def(&MtPlayer::RecordPicResult, "RecordPicResult")
			.def(&MtPlayer::ValidAnswerIndex, "ValidAnswerIndex")
			.def(&MtPlayer::CheckPicResult, "CheckPicResult")
			.def(&MtPlayer::OnConfirmAnswer,"OnConfirmAnswer")
			.def(&MtPlayer::GoTo, "GoTo")
			.def(&MtPlayer::GoingLeaveScene,"GoingLeaveScene")
			.def(&MtPlayer::QuitBattle,"QuitBattle")
			.def(&MtPlayer::GoRaid, "GoRaid")
			.def(&MtPlayer::GoRaidByPosIndex,"GoRaidByPosIndex")
			.def(&MtPlayer::ActivityCheck,"ActivityCheck")			
			.def(&MtPlayer::BagLeftCapacity,"BagLeftCapacity")
			.def(&MtPlayer::OnLuaFunCall_0, "OnLuaFunCall_0")
			.def(&MtPlayer::OnLuaFunCall_1, "OnLuaFunCall_1")
			.def(&MtPlayer::OnLuaFunCall_n, "OnLuaFunCall_n")
			.def(&MtPlayer::OnLuaFunCall_n_64, "OnLuaFunCall_n_64")
			.def(&MtPlayer::OnLuaFunCall_x, "OnLuaFunCall_x")
			.def(&MtPlayer::OnLuaFunCall_c, "OnLuaFunCall_c")
			.def(&MtPlayer::FindContainer, "FindContainer")
			.def(&MtPlayer::GetSimpleImpactSet, "GetSimpleImpactSet")
			.def(&MtPlayer::PlayerAutoMatch,"PlayerAutoMatch")
			.def(&MtPlayer::DBLog,"DBLog")
			.def(&MtPlayer::CostToken,"CostToken")
			.def(&MtPlayer::GetPlayerToken,"GetPlayerToken")
			.def(&MtPlayer::PlayerAutoMatch,"PlayerAutoMatch")
			.def(&MtPlayer::AddHero,"AddHero")
			.def(&MtPlayer::FindActorByCfgId,"FindActorByCfgId")
			.def(&MtPlayer::FindActorByGuid, "FindActorByGuid")
			.def(&MtPlayer::Scene,"Scene")
			.def(&MtPlayer::BattleGround, "BattleGround")
			.def(&MtPlayer::OnReSetHookData,"OnReSetHookData")
			.def(&MtPlayer::GetPlayerSetting, "GetPlayerSetting")
			.def(&MtPlayer::PlayerCanHook,"PlayerCanHook")
			.def(&MtPlayer::StopMove,"StopMove")
			.def(&MtPlayer::SetOnHook, "SetOnHook")
			.def(&MtPlayer::GetParams32, "GetParams32")
			.def(&MtPlayer::GetParams64, "GetParams64")
			.def(&MtPlayer::SetParams32, "SetParams32")
			.def(&MtPlayer::SetParams64, "SetParams64")
			.def(&MtPlayer::FindItemByGuid, "FindItemByGuid")
			.def(&MtPlayer::FindItemByConfigId,"FindItemByConfigId")
			.def(&MtPlayer::OnLogout,"OnLogout")
			.def(&MtPlayer::EnchantSlotEquips,"EnchantSlotEquips")
			.def(&MtPlayer::DumpMsgCounterInfo,"DumpMsgCounterInfo")
			.def(&MtPlayer::EnableMsgCounter, "EnableMsgCounter")
			.def(&MtPlayer::IsDisConnected,"IsDisConnected")
			.def(&MtPlayer::Refresh2ClientLuaFiles,"Refresh2ClientLuaFiles")
			.def(&MtPlayer::Refresh2ClientOperateConfig,"Refresh2ClientOperateConfig")
			.def(&MtPlayer::MqGenerateOrderID,"MqGenerateOrderID")
			.def(&MtPlayer::UpdateBattleScore,"UpdateBattleScore")
			.def(&MtPlayer::CheckCDKCode, "CheckCDKCode")
			.def(&MtPlayer::send_friendlistupdate,"send_friendlistupdate")
			;

			ff::fflua_register_t<boost::shared_ptr<MtPlayer>, ctor()>(ls, "MtSharePlayer");
	}

	MtPlayer::MtPlayer(const boost::shared_ptr<Packet::PlayerDBInfo>& player_db, const boost::shared_ptr<Packet::AccountModel>& account)
		:player_db_(player_db)
		,account_(account)
	{
		LOG_INFO << "[mt player] create player :" << this;
		player_data_ = player_db->mutable_basic_info();
		std::fill(std::begin(chatcd_), std::end(chatcd_), 0);
		runtime_status_ = boost::make_shared<Packet::PlayerRunTimeStatus>();
		runtime_status_->set_in_battle(false);
		runtime_status_->set_on_hook(false);
		runtime_status_->set_guildposition(Packet::GuildPosition::TRAINEE);
		CleanTeamData();
		bf_proxy_.set_raid_rtid(INVALID_GUID);
		worldboss_relivecd_ = 0;
		worldboss_hp_ = 0;
		simple_impacts_.Init(this);
		mission_proxy_ = nullptr;
		arrange_proxy_ = nullptr;
		player_guild_proxy_ = nullptr;
	}

	MtPlayer::MtPlayer()
	{
		LOG_ERROR << "[mt player] create player with out context:" << this;
	}

	zxero::uint64 MtPlayer::Guid() const
	{
		return player_db_->guid();
	}

	Packet::ResultCode MtPlayer::SetGuideStep(const int32 group, const int32 step)
	{
		int32 fstep = step < 0 ? 0 : step;
		if (group < Packet::GuideCellIndex::GuideGroupStart
			|| group >= Packet::GuideCellIndex::GuideGroupEnd) {
			return Packet::ResultCode::Invalid_Request;
		}
		datacell_container_->set_data_32(Packet::CellLogicType::GuideCell, group, fstep, true);

		return Packet::ResultCode::ResultOK;
	}
	Packet::PlayerBasicInfo * MtPlayer::GetScenePlayerInfo() const
	{
		return player_data_;
	}
	const boost::shared_ptr<Packet::PlayerDBInfo>& MtPlayer::GetDBPlayerInfo() const
	{
		return player_db_;
	}

	const std::vector<boost::shared_ptr<MtActor>>& MtPlayer::Actors() const {
		return actors_;
	}

	std::vector<int32> MtPlayer::AllInsetGems() {
		auto& equips_in_equip_bag = containers_[Packet::ContainerType::Layer_EquipBag]->Items();
		auto& equips_in_equip = containers_[Packet::ContainerType::Layer_Equip]->Items();
		std::vector<int32> result;
		for (auto& equip : equips_in_equip_bag) {
			equip->GatherGemId(result);
		}
		for (auto& equip : equips_in_equip) {
			equip->GatherGemId(result);
		}
		return result;
	}

	std::map<int32, int32> MtPlayer::EnchantSlotEquips(bool effect)
	{
		std::map<int32, int32> temp;
		auto& equips_in_equip_bag = containers_[Packet::ContainerType::Layer_EquipBag]->Items();
		auto& equips_in_equip = containers_[Packet::ContainerType::Layer_Equip]->Items();
		std::vector<int32> result;
		for (auto& equip : equips_in_equip_bag) {
			auto e = dynamic_cast<MtEquipItem*>(equip.get());
			if( e== nullptr ) continue;
			auto all_slots = e->OpenedEnchants({ 0,1,2,3 },effect);
			if (temp.find(all_slots.size()) != temp.end()) {
				temp[all_slots.size()] += 1;
			}
			else {
				temp[all_slots.size()] = 1;
			}
		}
		for (auto& equip : equips_in_equip) {
			auto e = dynamic_cast<MtEquipItem*>(equip.get());
			if (e == nullptr) continue;
			auto all_slots = e->OpenedEnchants({ 0,1,2,3 }, effect);
			if (temp.find(all_slots.size()) != temp.end()) {
				temp[all_slots.size()] += 1;
			}
			else {
				temp[all_slots.size()] = 1;
			}
		}
		return temp;
	}

	const std::vector<boost::shared_ptr<MtActor>> MtPlayer::BattleActor(Packet::ActorFigthFormation form_type) 
	{
		if (form_type == Packet::ActorFigthFormation::INVALID_FORMATION) {
			form_type = Packet::ActorFigthFormation::AFF_NORMAL_HOOK;
		}
		auto formation_it = std::find_if(std::begin(ActorFormation()), std::end(ActorFormation()), [=](auto& form) {return form->type() == form_type; });
		if (formation_it == std::end(ActorFormation())) {
			ZXERO_ASSERT(false) << "can not find formation type = " << form_type;
			auto max = actors_.size() > 5 ? 5 : actors_.size();
			std::vector<boost::shared_ptr<MtActor>> result(std::begin(actors_), std::begin(actors_) + max);
			return result;
		}
		else {
			std::vector<boost::shared_ptr<MtActor>> online_fighters;
			for (auto guid : (*formation_it)->actor_guids()) {
				for (auto child : actors_) {
					if (guid == child->Guid()) {
						online_fighters.push_back(child);
					}
				}
			}

			if (online_fighters.empty()) {
				ZXERO_ASSERT(false) << "empty actor list !!! guid" << Guid() << " form type " << form_type;
			}
			return online_fighters;
		}
	}

	Packet::BattleGroundType MtPlayer::GetCurBattleType()
	{
		if (BattleGround() != nullptr)
				return BattleGround()->BattleType();
		return Packet::BattleGroundType::NO_BATTLE;
	}
	Packet::BattleGroundStatus MtPlayer::GetCurBattleStatus()
	{
		if (BattleGround() != nullptr)
			return BattleGround()->GetStatus();
		return Packet::BattleGroundStatus::WAIT;
	}
	const std::vector<boost::shared_ptr<MtActor>> MtPlayer::TeamFormBattleActor(Packet::ActorFigthFormation form) 
	{
		std::vector<boost::shared_ptr<MtActor>> temp;
		if (scene_ == nullptr) {
			return temp;
		}

		if (TeamMemberSize() == 0 || GetTeamStatus() == Packet::PlayerTeamStatus::Team_AFK) {
			return BattleActor(form);
		}
		if (MainActor() != nullptr)
			temp.push_back(MainActor());
		for (auto child : runtime_status_->team().member()) {
			if (child.memberid() == Guid()) {//跳过队长
				continue;
			}
			if (child.teamstatus() != Packet::PlayerTeamStatus::Team_Follow){
				continue;
			}
			auto tplayer = scene_->FindPlayer(child.memberid());
			if (tplayer != nullptr ) {
				auto actor = tplayer->MainActor();
				if (actor != nullptr) {
					temp.push_back(actor);
				}
			}			
		}		
		//不足5人需要用队长挂机阵容补满5人
		auto form_actors = BattleActor(form);
		for (uint32 i = 0; i < form_actors.size(); i++) {
			if (temp.size() >= 5){
				break;
			}
			if (form_actors[i]->ActorType() != Packet::ActorType::Main)
				temp.push_back(form_actors[i]);
		}
		return temp;
	}

	const std::vector<boost::shared_ptr<MtActor>> MtPlayer::TeamMainBattleActor()
	{
		std::vector<boost::shared_ptr<MtActor>> temp;
		if (scene_ == nullptr) {
			return temp;
		}
		if (runtime_status_->team().teamid() != INVALID_GUID && GetTeamStatus() != Packet::PlayerTeamStatus::Team_AFK)
		{
			for (auto child : runtime_status_->team().member()) {
				//if (child.memberid() == Guid()) {//不能跳过
				//	continue;
				//}
				if (child.teamstatus() != Packet::PlayerTeamStatus::Team_Follow) {
					continue;
				}
				auto tplayer = scene_->FindPlayer(child.memberid());
				if (tplayer != nullptr) {
					temp.push_back(tplayer->MainActor());
				} else {//尝试通过自己身上的机器人创建
					auto robot_it = std::find_if(robots_.begin(), robots_.end(),
						boost::bind(&MtRobot::Guid, _1) == child.memberid());
					if (robot_it == robots_.end())
						continue;
					auto& robot = *robot_it;
					auto actor = boost::make_shared<ActorFullInfoDummy>(robot.actor_full_);
					if (actor) temp.push_back(actor);
				}
			}
		}
		else
		{
			temp.push_back(MainActor());
		}
		

		return temp;
	}

	const boost::shared_ptr<MtActor> MtPlayer::FindActor(uint64 guid) const {
		auto actor_it = std::find_if(std::begin(actors_), std::end(actors_), boost::bind(&MtActor::Guid, _1) == guid);
		if (actor_it == std::end(actors_)) {
			return nullptr;
		}
		else {
			return *actor_it;
		}
	}

	void MtPlayer::DumpMsgCounterInfo(bool clean)
	{
		std::lock_guard<std::mutex> guard(session_mutex_);
		LOG_INFO << "player message counter info guid=" << Guid() << " infos: /n" << message_counter_;
		if (clean) {
			message_counter_.clear();
		}
	}

	void MtPlayer::UpdateMsgCounter(const google::protobuf::Message& message) 
	{
		auto name = message.GetTypeName();
		if (message_counter_.find(name) != message_counter_.end()) {
			message_counter_[name]++;
		}
		else {
			message_counter_[name] = 1;
		}
	}

	bool MtPlayer::SendMessage(const google::protobuf::Message& message) 
	{
		std::lock_guard<std::mutex> guard(session_mutex_);
		if (session_ != nullptr) {

			if (enable_msg_counter_) {
				UpdateMsgCounter(message);
			}

			return session_->send(message);
		}
		return false;
	}

	bool MtPlayer::LuaSendMessage(const google::protobuf::Message* message) 
	{
		if (message != nullptr)
			return SendMessage(*message);
		return false;
	}

	Packet::ResultCode MtPlayer::ActivityBoxReward(const int32 index)
	{
		if (index < 0 || index >4) {
			return Packet::ResultCode::Invalid_Request;
		}

		char buffer[64] = { 0 };
		zprintf(buffer, sizeof(buffer), "activity_reward_%d", index);
		auto config = MtConfigDataManager::Instance().FindConfigValue(buffer);
		if (nullptr == config) {
			return Packet::ResultCode::InvalidConfigData;
		}
		int32 token = GetPlayerToken(Packet::TokenType::Token_Day);
		if (token < config->value2()) {
			return Packet::ResultCode::TokenDayNotEnough;
		}

		int32 flag = datacell_container_->get_data_32(Packet::CellLogicType::NumberCell, Packet::NumberCellIndex::ActivityRewardFlag);
		if (utils::bit_check(flag, index)) {
			return Packet::ResultCode::ActivityRewardBoxDone;
		}

		auto package = MtItemPackageManager::Instance().GetPackage(config->value1());
		if (package == nullptr) {
			return Packet::ResultCode::InvalidConfigData;
		}

		utils::bit_set(flag, index);
		datacell_container_->set_data_32(Packet::CellLogicType::NumberCell, Packet::NumberCellIndex::ActivityRewardFlag,flag,true);

		Packet::NotifyItemList notify;
		package->AddToPlayer(this, nullptr, &notify, std::function<int(int)>());

		if (notify.item_size() > 0) {
			SendMessage(notify);
		}
		OnLuaFunCall_x("xActivityBoxReward",
		{
			{ "value",index }
		});

		return Packet::ResultCode::ResultOK;
	}

	bool MtPlayer::FirstStageReward(const int32 stage_id)
	{
		GetPlayerStageProxy()->IsStageRaid(stage_id);
		if (GetPlayerStageProxy()->IsStageRaid(stage_id))
			return false;

		auto config = MtSceneStageManager::Instance().GetConfig(stage_id);
		if (config == nullptr)
			return false;

		int32 first_drop = config->first_drop();
		auto package = MtItemPackageManager::Instance().GetPackage(first_drop);
		if (package) {
			Packet::NotifyItemList notify;
			package->AddToPlayer(this, nullptr, &notify);

			if (notify.item_size() > 0) {
				SendMessage(notify);
			}
			
			//OnLuaFunCall_x("xFirstStageReward",
			//{
			//	{ "value",stage_id }
			//});
			
			return true;
		}

		return false;
	}


	bool MtPlayer::ClickMoveProcessDone(uint64 elapseTime)
	{
		auto current_zone = scene_->GetZoneByPos(Position());
		if (current_zone == nullptr) {
			return false;
		}
		if (move_path_->sub_position_index() < move_path_->sub_positions_size()) {
			auto next_target_pos = move_path_->sub_positions(move_path_->sub_position_index());
			auto curr_pos = this->player_data_->scene_pos();
			auto curr_distance = zxero::distance2d(curr_pos, next_target_pos);
			auto move_distance = int(this->player_data_->move_speed() * elapseTime / 1000.f);
			auto move_radian = zxero::get_direction(curr_pos, next_target_pos);
			auto next_pos = zxero::move_point(curr_pos, move_distance, move_radian);
			if (move_distance < curr_distance) {
				if (this->Scene()->ValidPos(next_pos)) {
					Position(next_pos);
					return false;
				} else {
					Packet::ClientPlayerStop notify;
					notify.set_guid(this->player_data_->guid());
					notify.set_direction(this->player_data_->direction());
					auto stop_pos = notify.mutable_stop_pos();
					stop_pos->CopyFrom(player_data_->scene_pos());
					current_zone->BroadcastMessage(notify);
					move_path_.reset();
					return true;
				}
			}
			else {
				auto sub_distance = move_distance - curr_distance; //剩余移动距离
				move_path_->set_sub_position_index(move_path_->sub_position_index() + 1);
				if (move_path_->sub_position_index() == move_path_->sub_positions_size()) {
					Position(next_target_pos);
					move_path_.reset();
					return true; //移动完毕
				}
				else {
					curr_pos = next_target_pos;
					next_target_pos = move_path_->sub_positions(move_path_->sub_position_index());
					curr_distance = zxero::distance2d(curr_pos, next_target_pos);
					move_radian = zxero::get_direction(curr_pos, next_target_pos);
					next_pos = zxero::move_point(curr_pos, sub_distance, move_radian);
					Position(next_pos);
					return false;
				}
			}
		}
		return true;
	}

	void MtPlayer::OnTick(uint64 elapseTime)
	{

		boost::for_each(player_ai_, boost::bind(&PlayerAi::OnTick, _1, elapseTime));
	
		simple_impacts_.SimpleImpactUpdate(elapseTime);

		//
		int64 timer = (int64)text_answers_timer_;
		if (timer > 0 && !text_answers_.empty()) {
			timer -= elapseTime;
			text_answers_timer_=timer;
			if (timer <= 0) {
				auto message = boost::make_shared<Packet::LuaRequestPacket >();
				message->set_request_name("ConfirmTextAnswer");
				message->add_int32_params(text_answers_[0]);
				message->add_string_params("");
				DispatchMessage(message);
			}
		}

		if (change_scenecd_ > 0) {
			change_scenecd_ -= elapseTime;
		}

		if (arrange_proxy_ != nullptr) {
			arrange_proxy_->OnTick(elapseTime);
		}

		//任务同步超时判定
		if (mission_proxy_ != nullptr) {
			mission_proxy_->MissionSyncTimer((int32)elapseTime);
		}
	}

	void MtPlayer::OnNpcRefreshTime( const int32 delta)
	{
		//npc刷新
		std::vector<boost::shared_ptr<MtNpc>> to_delete;
		for (auto iter = npc_map_.begin(); iter != npc_map_.end(); ) {
			iter->second->OnBigTick(delta);
			if (iter->second->IsDead()) {
				to_delete.push_back(iter->second);
				iter = npc_map_.erase(iter);
			}
			else {
				++iter;
			}
		}

		//通知客户端
		if (!to_delete.empty()) {
			Packet::NpcList reply;
			reply.set_option(Packet::NpcOption::NpcDel);
			for (auto child : to_delete) {
				auto npc_info = reply.add_npc_info();
				npc_info->CopyFrom(*(child->NpcInfo()));
			}
			SendMessage(reply);
		}
	}
	void MtPlayer::OnBigTick_1(uint64 elapseTime)
	{
		///
		{
			int32 delta = (int32)(elapseTime / 1000);
			//profiler p("OnNpcRefreshTime");
			OnNpcRefreshTime(delta);
		}
	}
	void MtPlayer::OnBigTick_2(uint64 elapseTime)
	{
		//只有item_bag_和bank需要有cd
		{
			int32 delta = (int32)(elapseTime / 1000);
			//profiler p("OnItemRefreshTime");
			containers_[Packet::ContainerType::Layer_ItemBag]->OnRefreshTime(delta);
			containers_[Packet::ContainerType::Layer_Bank]->OnRefreshTime(delta);
		}
	}
	void MtPlayer::OnBigTick_3(uint64 elapseTime)
	{
		//actortick
		{
			//profiler p("actors OnBigTick");
			for (auto child : actors_) {
				child->OnBigTick(elapseTime);
			}
		}
	}
	void MtPlayer::OnBigTick_4(uint64 elapseTime)
	{
		elapseTime;
		////任务同步超时判定
		//{
		//	int32 delta = (int32)(elapseTime / 1000);
		//	//profiler p("MissionSyncTimer");
		//	if (mission_proxy_ != nullptr) {
		//		mission_proxy_->MissionSyncTimer(delta);
		//	}
		//}
	}
	void MtPlayer::OnBigTick_5(uint64 elapseTime)
	{
		{
			elapseTime;
			//profiler p("EquipDurableCheck");
			EquipDurableCheck();
		}
	}
	void MtPlayer::OnBigTick_6(uint64 elapseTime)
	{
		//luaxOnBigTick
		{
			//profiler p("xOnBigTick");
			//OnLuaFunCall_x("xOnBigTick",
			//{
			//	{ "value",-1 }
			//});
			elapseTime;
			OnLuaFunCall_0(100, "OnBigTick");
		}
	}

	void MtPlayer::OnBigTick(uint64 elapseTime)
	{
		{
			std::lock_guard<std::mutex> guard(session_mutex_);
			if (session_) {
				int32 frame_packet = session_->get_frame_packet();
				if (frame_packet > 40 * 10) {//每秒最多40个包
					session_->close(true);
					LOG_WARN << "player:" << Guid() << " got "
						<< frame_packet << " in 10 seconds. kicked";
				} else {
					session_->reset_frame_packet();
				}
			}
		}
		int32 delta = (int32)(elapseTime / 1000);

		{
			//profiler p("SyncHookData2Server");
			if (rand_gen->intgen(0, 10) == 0) {
				player_stage_proxy_->SyncHookData2Server();
			}
		}

		for (int32 i = 0; i < Packet::ChatChannel::MAX; i++){
			if (chatcd_[i] > 0)
			{
				if (chatcd_[i] > delta)	{
					chatcd_[i] -= delta;
				}
				else{
					chatcd_[i] = 0;
				}
			}
		}

		{
			//profiler p("ActivityListRefresh");
			//ActivityListRefresh();
		}		

		if (worldboss_relivecd_ > 0){
			if (worldboss_relivecd_ > delta){
				worldboss_relivecd_ -= delta;
			}
			else{
				worldboss_relivecd_ = 0;
			}
		}

		if (scene_ == nullptr) {
			//空场景还有大量消息没处理就踢下线
			if (client_msgs_.size() > 20) {
				//踢下线
				LOG_INFO << "[player logout]," << Guid() << "set logout in tick! logout_status"<< logout_status_;
				
				OnLogout();
			}
		}
	}
		

	void MtPlayer::OnCache2Shm( )
	{
		OnCache();

		//通知主线程
		Server::Instance().SendS2GCommonMessage("S2GCacheOk", {}, { (int64)Guid() }, {});
	}

	
	void MtPlayer::OnMonthTriger(int32 month)
	{

		OnLuaFunCall_x("xOnNewMonth",
		{
			{ "value", month }
		});
	}

	void MtPlayer::OnWeekTriger(int32 week)
	{

		OnLuaFunCall_x("xOnNewWeek",
		{
			{ "value", week }
		});

		ActivityListRefresh();
	}
	
	void MtPlayer::OnLeaveDaysTriger(int32 day)
	{
		OnLuaFunCall_x("xOnMissRewardResolve",
		{
			{ "value", day }
		});
	}

	void MtPlayer::OnDayTriger(int32 day, bool Interday)
	{
		auto refresh_time = MtTimerManager::Instance().CurrentDate();
		player_db_->set_refresh_time(refresh_time);

		UpdateWebPlayerInfo("NewDay", Interday);
		//同步服务器时间
		Packet::ServerTime stime;
		stime.set_time(now_second());
		SendMessage(stime);

		//活动脚本
		Refresh2ClientLuaFiles(true);
		Refresh2ClientOperateConfig(-1);
		ExpPointReward();
		ResetTrialFieldData();
		ResetTrialGBOSSData();
		ResetPlayerActorLastHp(Packet::LastHpType::BFLastHp);

		//重置答题时间
		text_answers_timer_=0;
		player_db_->set_text_answers("");
		player_db_->set_pic_answers("");

		friend_->OnNewDay();

		//线上跨天[创角和登录都不算]，未参于的活动优先结算
		if (Interday) {
			OnLeaveDaysTriger(day);
		}
		
		OnLuaFunCall_x("xOnNewDay",
		{
			{ "value", day }
		});

		OnLuaFunCall_x("xOnResetCheck",
		{
			{ "value", -1 }
		});

		ServerLevelCheck(PlayerLevel());
		
		ActivityListRefresh();
	}

	void MtPlayer::OnHourTriger(int32 hour)
	{
		switch (hour)
		{
		case 5:
		{
			int32 guilddata = DataCellContainer()->get_data_32(Packet::CellLogicType::NumberCell, Packet::NumberCellIndex::GuildCommonData);
			utils::bit_reset(guilddata, 0);
			DataCellContainer()->set_data_32(Packet::CellLogicType::NumberCell, Packet::NumberCellIndex::GuildCommonData, guilddata, true);
		}
			break;
		default:
			break;
		}

		ActivityListRefresh();
	}

	void MtPlayer::OnMinTriger(int32 min)
	{
		min;
		ActivityListRefresh();
	}

	void MtPlayer::ExpPointReward() 
	{
		auto token = player_db_->token_data();
		int32 cold_count = token.cold_exp_point();
		auto data = MtConfigDataManager::Instance().FindConfigValue("cold_exp_point_max");
		int32 max_count = 0;
		if (data != nullptr) {
			max_count = data->value1();
		}

		data = MtConfigDataManager::Instance().FindConfigValue("exp_point_day_reward");
		int32 day_count = 0;
		if (data != nullptr) {
			day_count = data->value1();
		}

		int32 delta_count = max_count - cold_count;
		if (delta_count < 0)
			delta_count = 0;
		int32 final_count = delta_count > day_count ? day_count : delta_count;
		AddItemById(Packet::TokenType::Token_ColdExpPoint, final_count, Config::ItemAddLogType::AddType_ExpPoint, nullptr, Guid());
	}

	void MtPlayer::SetScene(const boost::shared_ptr<MtScene>& scene)
	{
		scene_ = scene;
		if (scene_ != nullptr) {
			player_data_->set_scene_id(scene->SceneId());
		}	
	}

	void MtPlayer::Session(const boost::shared_ptr<client_session>& session)
	{
		std::lock_guard<std::mutex> guard(session_mutex_);
		session_ = session;
	}
	
	const boost::shared_ptr<client_session> MtPlayer::Session() {
		return session_;
	}

	std::string MtPlayer::GetIP()
	{
		std::lock_guard<std::mutex> guard(session_mutex_);
		if (session_)
		{
			return session_->get_ip();
		}
		return "0.0.0.0";
	}
	void MtPlayer::OnLogin()
	{
		//Onlogin晚于playerentersceneReply是个蛋疼的问题，某些player_data的数据可能还没初始化完成，存在隐患！

		//任务数据初始化操作
		if (mission_proxy_ != nullptr) {
			mission_proxy_->InitMission();
		}
		
		InitDataCell();
		InitActivity();
		InitMount();	
		//正式重连mt
		ReConnected();
		
	}

	void MtPlayer::OnLogout()
	{
		DisConnected();

		QuitBattle();
		
		npc_map_.clear();
		if (scene_ != nullptr)
		{
			//有队伍先离队
			if (GetTeamID() != INVALID_GUID) {
				Server::Instance().SendS2GCommonMessage("S2GLeaveTeam", {}, {(int64)Guid()}, {});
			}
			scene_->OnPlayerLeave(shared_from_this());
			//MtTeamManager::Instance().OnClearPlayerAutoMatch(Guid());
		}

		ZXERO_ASSERT(scene_ == nullptr);

		OnCache();//离线存储

		logout_status_ = eLogoutStatus::eOffLine;
		LOG_INFO << "[player logout]," << Guid()<<" logout_status_="<< logout_status_;
	}

	void MtPlayer::SendLogoutLog()
	{
		if (login_time_ != 0)
		{
			uint32 onlinetime = MtTimerManager::Instance().DiffTimeToNow(login_time_);
			std::stringstream DBlog;
			DBlog << "LOGOUT," << PlayerBibase() << "," << onlinetime << "," << disconnect_;
			Server::Instance().AddUrlTask(new db_log_require_work(DBlog.str(), __FILE__, __LINE__));
		}
	}
	void MtPlayer::RefreshData2Clinet()
	{
		Refresh2ClientLuaFiles(false);
		Refresh2ClientOperateConfig(-1);
		Refresh2ClientDataCellList();
		Refresh2ClientNpcList();
		Refresh2ClientMissionList();
		Refresh2ClientActivityList();			
		Refresh2ClientBattleFeild();
		Refresh2ClientImpactList();
		for (auto child : markets_)
		{
			child->RefreshData2Client();
		}
		for (auto child : Actors()) {
			child->OnBattleInfoChanged(true);
		}
		Server::Instance().SendS2GCommonMessage("S2GPlayerReConnected", {}, {(int64)Guid()}, {});
		OnLuaFunCall_x("xRefreshData2Clinet",
		{
			{ "value", -1 }
		});
	}

	void MtPlayer::LoginTimeCheck()
	{
		//同步服务器时间
		Packet::ServerTime stime;
		stime.set_time(now_second());
		SendMessage(stime);		

		uint32 last_logout_time = player_db_->last_logout_time();		
		int32 offline_seconds = MtTimerManager::Instance().DiffTimeToNow(player_db_->last_logout_time());
		//解决未初始化宕机重登后多次newday的问题
		login_time_ = MtTimerManager::Instance().CurrentDate();
		player_db_->set_last_logout_time(login_time_);

		//登出时间用来计算登出时长和模拟
		int32 logout_hour = 0;
		if (last_logout_time != 0) {
			logout_hour = MtTimerManager::Instance().GetHourFromUT(last_logout_time);
		}

		//刷新日期用来计算刷新逻辑
		int32 refresh_day = 0;
		int32 refresh_week = 0;
		int32 refresh_month = 1;
		uint32 refresh_time = player_db_->refresh_time();
		if (refresh_time != 0) {
			refresh_day = MtTimerManager::Instance().GetDayFromUT(refresh_time);
			refresh_week = MtTimerManager::Instance().GetWeekFromUT(refresh_time);
			refresh_month = MtTimerManager::Instance().GetMonthFromUT(refresh_time);
		}

		int32 curday = MtTimerManager::Instance().GetDay();
		int32 curhour = MtTimerManager::Instance().GetHour();
		int32 curweek = MtTimerManager::Instance().GetWeekNumber();	
		int32 curnmonth = MtTimerManager::Instance().GetMonth();
		//这里需要考虑一个创角的逻辑，不需要模拟过去的时间
		if (refresh_time == 0) {
			//注意顺序,月,周,天
			OnMonthTriger(curnmonth);
			OnWeekTriger(curweek);
			OnDayTriger(curday, false);
		}
		else {
			//注意顺序,月,周,天
			if (refresh_month != curnmonth) {
				OnMonthTriger(curnmonth);
			}
			if (refresh_week != curweek) {
				OnWeekTriger(curweek);
			}
			//
			if (refresh_day != curday) {
				int32 leave_days = curday - refresh_day;
				//离开的这些天需要模拟处理一些特殊逻辑
				for (int32 i = 0; i < leave_days; ++i) {
					OnLeaveDaysTriger(refresh_day + i);
				}
				OnDayTriger(curday, false);
				//
				for (int32 i = 0; i < leave_days&&i < 7; ++i) {
					for (int32 h = (logout_hour + 1); h <= 23; ++h) {
						OnHourTriger(h);
					}
					for (int32 h = 0; h <= curhour; ++h) {
						OnHourTriger(h);
					}
				}
			}
			else {
				if (curhour > logout_hour) {
					for (int32 h = logout_hour + 1; h <= curhour; ++h) {
						OnHourTriger(h);
					}
				}
			}
		}
		
		std::stringstream DBlog;
		DBlog << "LOGIN," << PlayerBibase() << "," << login_time_;
		Server::Instance().AddUrlTask(new db_log_require_work(DBlog.str(), __FILE__, __LINE__));

		{
			std::lock_guard<std::mutex> guard(session_mutex_);
			if (session_) {
				//mqdblopg 玩家登录信息
				auto logininfo = session_->get_logininfo();
				if (account_ != nullptr && logininfo != nullptr) {

					auto create_time = GetDBPlayerInfo()->create_time();
					std::string ctstr = MtTimerManager::Instance().UTimeString(create_time);
					std::string cdstr = MtTimerManager::Instance().TimeString2DateString(ctstr);
					std::string tstr = MtTimerManager::Instance().NowTimeString();
					std::string dstr = MtTimerManager::Instance().TimeString2DateString(tstr);

					boost::format fmt("REPLACE INTO `player_login_log` (`ServerID`,`PartnerID`,`UserID`,`PlayerID`,`VipLevel`,`FAP`,`DeviceIdentifier`,`DeviceModel`,`OS`,`LoginIP`,`GameVersionID`,`RegisterDate`,`RegisterTime`,`LoginDate`,`LoginTime`) VALUES('%d','%d','%s','%llu','%d','%d','%s','%s','%s','%s','%s','%s','%s','%s','%s');");
					fmt% MtConfig::Instance().server_group_id_%account_->partnerid() % account_->userid() % Guid() % GetAllCharge() % player_data_->battle_score() % account_->udid()
						% logininfo->device_type() % logininfo->osversion() % logininfo->login_ip() % logininfo->version()
						% cdstr %ctstr % dstr %tstr;
					Server::Instance().SendLogMessage(fmt.str());

					//mqdblopg 设备登录
					boost::format fmt2("REPLACE INTO `device_login_log` (`ServerID`,`PartnerID`,`UserID`,`DeviceIdentifier`,`CrDate`,`CrTime`) VALUES('%d', '%d', '%s', '%s', '%s', '%s'); ");
					fmt2%MtConfig::Instance().server_group_id_%account_->partnerid() % account_->userid() % account_->udid() % dstr %tstr;
					Server::Instance().SendLogMessage(fmt2.str());
				}
			}
		}

		/////////////////////////
		if (player_ai_.empty()) {
			player_ai_.push_back(CreateOffLineAi(PlayerAiType::AutoHook, shared_from_this()));
			player_ai_.push_back(CreateOffLineAi(PlayerAiType::QuitTeam, shared_from_this()));
		}
		boost::for_each(player_ai_, boost::bind(&PlayerAi::OnPlayerOnLine, _1));
		if (GetPlayerStageProxy() != nullptr) {
			Refresh2ClientStageInfo();
			GetPlayerStageProxy()->OnPlayerOnline(offline_seconds);
		}
	}
	
	void MtPlayer::ReConnected() {
		
		//更新活动列表
		SetParams64("acr_activie", 0);
		disconnect_ = false;
		cache_stamp_ = 0;
		//
		LoginTimeCheck();
		//
		if (GetGuildID() != INVALID_GUID){
			Server::Instance().SendS2GCommonMessage("S2GLoginSyncGuildUserData", {PlayerLevel()}, { (int64)Guid(),(int64)GetGuildID() }, { Name() });
		}

		RefreshData2Clinet();

		if (raid_ != nullptr){
			try {
				thread_lua->call<int32>(raid_->GetScriptId(), "ReConnected", raid_, this);
			}
			catch (ff::lua_exception& e) {
				e;
			}
		}

		OnLuaFunCall_x("xReConnected",
		{
			{ "value", -1 }
		});

		logout_status_ = eLogoutStatus::eOnLine;

		message_->delete_offlinemessage();

		UpdateWebPlayerInfo();

		SendCommonReply("LoginDataOK", {}, {}, {});
	}
	void MtPlayer::OfflineHookReward()
	{
	}
	void MtPlayer::DisConnected() {

		if (player_db_ != nullptr) {
			OnLuaFunCall_x("xDisConnected",
			{
				{ "value", -1 }
			});

			uint32 now_time = MtTimerManager::Instance().CurrentDate();
			player_db_->set_last_logout_time(now_time);
		}
		if (GetGuildID() != INVALID_GUID)
		{
			Server::Instance().SendS2GCommonMessage("S2GLoginOutSyncGuildUserData", {}, { (int64)Guid(),(int64)GetGuildID() }, {});
		}

		boost::for_each(player_ai_, boost::bind(&PlayerAi::OnPlayerOffLine, _1));
		disconnect_ = true;

		boost::shared_ptr<client_session> session = Session();
		Session(nullptr);
		if (session != nullptr) {
			session->close(true);
		}
	}


	void MtPlayer::QuitBattle()
	{
		if (battle_ground_ != nullptr) {
			battle_ground_->OnBattleEnd(true, Guid());
			if (hook_battle_ != nullptr) {
				//hook_battle_->SetHookPause(false);
			}
		}
	}

	void MtPlayer::OnActorDead(MtBattleGround* battle_ground,
		MtActor* actor)
	{
		if (battle_ground == hook_battle_.get()) {
			if (actor->Player().lock() != nullptr && actor->Player().lock()->Guid() == Guid()) {
				player_stage_proxy_->OnActorDead(actor);
			}
		}
	}
	void MtPlayer::OnBattleEnd(
		const boost::shared_ptr<MtBattleGround>& battle_ground, 
		bool force_end)
	{
		if (hook_battle_ == battle_ground) {
			if (force_end == false) {
				player_stage_proxy_->OnBattleEnd(battle_ground.get());
			} else {
				hook_battle_ = nullptr;
			}
		}
		else {
			if (GetTeamID() != INVALID_GUID && GetTeamStatus() == Packet::PlayerTeamStatus::Team_AFKING) {
				MtTeamManager::Instance().OnPlayerBattleEnd(shared_from_this());
			}
			battle_ground_ = nullptr;
			if (hook_battle_ != nullptr) {
				//hook_battle_->SetHookPause(false);
			}
		}		
	}

	float MtPlayer::Direction()
	{
		return player_data_->direction();
	}

	void MtPlayer::Direction(float dir)
	{
		player_data_->set_direction(dir);
	}

	zxero::uint32 MtPlayer::GetMoveSpeed()
	{
		auto old = player_data_->move_speed();
		simple_impacts_.foreach([&](auto ptr) {
			if (ptr != nullptr) {
				auto config = MtItemManager::Instance().ImpactType2Config(ptr->impact_type());
				if (config && config->effect() == Packet::SimpleImpactEffectType::EF_PlayerSpeed) {
					old += ptr->param1();
				}
			}
		});
		return old;
	}

	void MtPlayer::ResetSpeed()
	{
		SetMoveSpeed(4000);
	}


	void MtPlayer::UpdateSimpleImpact()
	{
	}
	
	void MtPlayer::SetMoveSpeed(uint32 move_speed)
	{

		player_data_->set_move_speed(move_speed);
		if (scene_) {
			Packet::PlayerSpeedChanged msg;
			msg.set_guid(Guid()), msg.set_move_speed(GetMoveSpeed());
			BroadCastSceneMessage(msg);
		}
		if (IsTeamLeader()) {
			Packet::TeamLeaderUpdate update;
			update.set_playerguid(Guid());
			update.set_playername(Name());
			update.set_charge(false);
			update.set_move_speed(GetMoveSpeed());
			BroadcastTeamEx(true, update);
		}
	}	

	zxero::int32 MtPlayer::GetSceneId()
	{
		return player_data_->scene_id();
	}

	void MtPlayer::SetSceneId(int32 scene_id)
	{
		player_data_->set_scene_id(scene_id);
	}

	const Packet::Position& MtPlayer::Position()
{
		return player_data_->scene_pos();
	}

	void MtPlayer::Position(const Packet::Position& pos)
	{
		if (scene_) {
			auto current_area = scene_->GetCurrentLogicArea(shared_from_this());
			Packet::Position old_pos = player_data_->scene_pos();
			player_data_->mutable_scene_pos()->CopyFrom(pos);
			auto new_area = scene_->GetPlayerLogicArea(shared_from_this());
			if (!current_area.Empty() && !new_area.Empty() && !current_area.EqualTo(new_area))
			{
				/*LOG_INFO << "change zone (" << old_pos.x() << "," << old_pos.y() << ")->("
					<< player_data_->scene_pos().x() << ","
					<< player_data_->scene_pos().y() << ")";*/
				new_area.OnPlayerEnter(shared_from_this(), &current_area,nullptr);
			}

		}
		else {
			player_data_->mutable_scene_pos()->CopyFrom(pos);
		}
	}
	

	bool MtPlayer::EnterBattleGround(MtBattleGround* battle)
	{
		auto new_battle = battle->shared_from_this();
		battle->AddPlayer(shared_from_this());
		if (battle->Brief().type() == Packet::BattleGroundType::PVE_HOOK) {
			hook_battle_ = new_battle;
			return true;
		} else {
			battle_ground_ = new_battle;
			return true;
		}
	}

	void MtPlayer::EnterRaid(const boost::shared_ptr<MtRaid>& raid) 
	{ 
		raid_ = raid;

		if (raid_->RuntimeId() == bf_proxy_.raid_rtid()) {
			auto message = boost::make_shared<S2G::S2GCommonMessage>();
			message->set_request_name("S2GEnterBattleField");
			message->add_int64_params(Guid());
			Server::Instance().SendMessage(message, nullptr);
		}
	}

	void MtPlayer::LeaveRaid() 
	{ 
		raid_ = nullptr;
	}
	void MtPlayer::GoingLeaveScene()
	{
		boost::shared_ptr<Packet::PlayerLeaveScene> message = boost::make_shared<Packet::PlayerLeaveScene>();
		DispatchMessage(message);
	}

	const boost::shared_ptr<MtRaid> MtPlayer::GetRaid() const 
	{
		return raid_;
	}

	bool MtPlayer::AddHero(zxero::int32 config_id, Packet::ActorType type,
		const std::string& source, bool init_equip /*= false*/,
		Packet::ActorStar star /*= Packet::ActorStar::ONE*/,
		Packet::ActorColor color /*= Packet::ActorColor::WHILE*/,
		const int32 level /*= 1*/)
	{

		//不允许重复的
		if (FindActorByCfgId(config_id) != nullptr) {
			return false;
		}
		auto config = MtActorConfig::Instance().GetBaseConfig(config_id);
		if (config == nullptr){
			return false;
		}
		auto db_info = boost::make_shared<Packet::ActorBasicInfo>();
		if (type == Packet::ActorType::Main) {
			db_info->set_guid(Guid());
		}
		else {
			db_info->set_guid(MtGuid::Instance()(*db_info));
		}
		db_info->set_player_guid(Guid());
		db_info->set_race(config->race);
		db_info->set_profession(config->profession);
		db_info->set_gender(config->gender);
		db_info->set_level(level);
		db_info->set_name(player_data_->name());
		db_info->set_talent(config->talent);
		db_info->set_type(type);
		db_info->set_exp(0);
		db_info->set_star(star);
		db_info->set_color(color);
		db_info->set_level_2(0);
		db_info->set_exp_2(0);
		db_info->set_score(0);
		db_info->set_client_res_id(config->client_res_id);
		db_info->set_actor_config_id(config_id);
		db_info->set_battle_type(Packet::ActorBattleType::USE_PHYSICAL);
		db_info->set_hair(0);
		db_info->set_gem_master_lv(0);
		auto binfo = db_info->mutable_battle_info();
		binfo->CopyFrom(config->property);
		MtActorConfig::Instance().LevelOne2LevelTwo(config->talent, *binfo);
		//
		db_info->SetInitialized();

		auto actor = boost::make_shared<MtActor>(db_info, *this);
		if (Packet::ActorType::Main == type) {
			main_actor_ = actor;
		}
		for (auto skill_id : config->skills) {
			actor->AddSkill(skill_id);
		}
		if (star >= Packet::ActorStar::THREE && config->star_skill > 0) {
			actor->AddSkill(config->star_skill, int32(star - Packet::ActorStar::TWO));
		}

		if (init_equip) {
			actor->InitEquip();
		}

		//初始化last_hp数据，如果为0表示是初始状态，需要用hp重置，否则就读取db数据
		for (int32 i = 0; i < Packet::LastHpType::LastHpMax; i++) {
			if (db_info->last_hp_size() > i) {
				if (db_info->last_hp(i) == 0) {
					db_info->set_last_hp(i, binfo->hp());
				}
			}
			else {
				db_info->add_last_hp(binfo->hp());
			}
		}		

		if (!AddActor(actor, source == "INIT")) {//addactor会有OnBattleInfoChanged的lua回调问题，此时还没有scene
			return false;
		}

		Packet::NewActorNotify new_actor_notify;
		new_actor_notify.set_source(source);
		auto actor_info = new_actor_notify.add_actor_lsit();
		actor->SerializationToMessage(*actor_info);
		SendMessage(new_actor_notify);

		//mqdblopg 不包含模型ID的资源类型流水日志表
		std::string tstr = MtTimerManager::Instance().NowTimeString();
		std::string dstr = MtTimerManager::Instance().TimeString2DateString(tstr);
		boost::format fmt("INSERT INTO `resource_nohaveid_log` (`ServerID`,`PartnerID`,`UserID`,`PlayerID`,`VipLevel`,`ModuleID`,`BeforeValue`,`AfterValue`,`EffectValue`,`CrDate`,`CrTime`) VALUES('%d','%d','%s','%llu','%d','%d','%d','%d','%d','%s','%s');");
		fmt% MtConfig::Instance().server_group_id_%account_->partnerid() % account_->userid() % Guid() % GetAllCharge() % 0 % 0 % 1 % config_id % dstr %tstr;
		SendLogMessage(fmt.str());

		LOG_INFO << "player:" << Guid() << ",AddHero:" << config_id << "," << type << "," << source;

		return true;
	}

	void MtPlayer::SendLogMessage(const std::string& msg)
	{

		Json::FastWriter writer;
		Json::Value root;
		try {
			root["ServerGroupId"] = MtConfig::Instance().server_group_id_;
			root["Sql"] = msg;
			uLongf blen = 0;
			LogMsg log_msg;
			if (msg != "") {

				std::string content = writer.write(root);
				uLong tlen = content.length();// +1;  //需要把字符串的结束符'\0'也一并处理

				if (tlen >= sizeof(log_msg.content)) {
					ZXERO_ASSERT(false) << "SendLogMessage failed! no enough memory! content=" << msg;
					return;
				}

				blen = compressBound(tlen); //压缩后的长度是不会超过blen的


				if (compress(log_msg.content, &blen, (const Bytef *)(content.c_str()), tlen) != Z_OK) {
					ZXERO_ASSERT(false) << "SendLogMessage failed! compress error! content=" << msg;
					return;
				}
			}
			log_msg.head_len = blen;
			Server::Instance().SendLogMessageEx(log_msg);
		}
		catch (std::exception& e)
		{
			ZXERO_ASSERT(false) << "SendLogMessage exception! " << e.what();
		}
	}

	void MtPlayer::OnCache()
	{		
		if (logout_status_==eLogoutStatus::eOffLine) {//登出就不再cache数据了
			return;
		}

		cache_stamp_ = MtTimerManager::Instance().CurrentDate();

		Cache2Save(player_db_.get());

		for (auto& child : actors_) {
			child->OnCache();
		}

		for (auto child : containers_) {
			child.second->OnCache();
		}

		if (mission_proxy_ != nullptr) {
			for (auto& child : mission_proxy_->GetMissionMap()) {
				Cache2Save(child.second->MissionDB());
			}
		}
		for (auto& child : markets_) {
			child->OnCache();
		}
		
		for (auto& form : actor_formations_) {
			Cache2Save(form.get());
		}

		for (auto& prac : guild_practice_)
		{
			Cache2Save(prac.get());
		}
		
		message_->OnCache();
		datacell_container_->OnCache();
		Cache2Save(player_stage_db_.get());
		Cache2Save(settings_.get());
		for (auto& stage : stages_) {
			Cache2Save(stage.get());
		}
		simple_impacts_.OnCache();

	}

	void MtPlayer::OnServerCache()
	{
		if (logout_status_ == eLogoutStatus::eOffLine) {//登出就不再cache数据了
			return;
		}
		friend_->OnCache();
	}
	void MtPlayer::AddItemNotify(const int32 config_id, const int32 ammout, Packet::NotifyItemList *nlist, Config::ItemAddLogType type, int32 ud)
	{
		if (ammout <= 0) {
			return;
		}

		if (nlist == nullptr) {
			return;
		}

		Packet::NotifyItemItem* item;
		auto item_it = std::find_if(
			nlist->mutable_item()->begin(), nlist->mutable_item()->end(),
			boost::bind(&Packet::NotifyItemItem::config_id, _1) == config_id);
		if (item_it == nlist->mutable_item()->end()
			|| type == Config::ItemAddLogType::AddType_Draw) {
			item = nlist->add_item();
		} else {
			item = &*item_it;
		}
		item->set_amount(item->amount() + ammout);
		item->set_config_id(config_id);
		item->set_ud(ud);
	}
	
	bool MtPlayer::AutoDismantleEquip(const int32 config_id, const int32 count, bool force, Packet::NotifyItemList *nlist)
	{
		auto equip_config = MtItemManager::Instance().GetEquipmentConfig(config_id);
		if (equip_config == nullptr) {
			return false;
		}

		bool dis = force;
		if ((equip_config->color() == Config::EquipmentColor::EQUIP_GREEN && player_db_->hook_option().auto_split_green())
			|| (equip_config->color() == Config::EquipmentColor::EQUIP_BLUE && player_db_->hook_option().auto_split_blue())) {
			dis = true;
		}
		if (!dis) {
			return false;
		}
		
		auto dismantle_config = MtUpgradeConfigManager::Instance().DismantleData(equip_config.get());
		if (dismantle_config == nullptr) {
			return false;
		}
		if (!BagLeftCapacity(Packet::ContainerType::Layer_ItemBag, dismantle_config->dismantle_items_size()*count)) {
			SendClientNotify("ItemBagIsFull", -1, -1);
			return false;
		}
		for (auto& item_info : dismantle_config->dismantle_items()) {
			AddItemById(item_info.item_id(), item_info.item_amount()*count, Config::ItemAddLogType::AddType_DismantleItem, nlist);
		}	
		return true;		
	}
	
	//任务相关道具分3类，手动获得奖励道具的途径由外部逻辑判满，自动获得奖励逻辑不做判定，满了自动抛弃，任务道具进任务仓库，保证不会丢失
	//所有的代币获得，都不做背包满判定
	//所有的手动获得奖励途径由外部做判满处理
	bool MtPlayer::AddItemByIdWithNotify(const int32 config_id,
		const int32 count, Config::ItemAddLogType type,
		const int64 source_id /*= 0*/,
		Packet::ActorFigthFormation formation/* = Packet::ActorFigthFormation::AFF_NORMAL_HOOK*/)
	{
		if (count <= 0)
			return false;

		bool result = false;
		Packet::NotifyItemList notify_list;
		result= AddItemById(config_id, count, type, &notify_list,source_id, formation);

		////掉落包还需要把自己的信息也通知到客户端
		//if (type == Config::ItemAddLogType::AddType_Package) {
		//	AddItemNotify((int32)source_id, 1, &notify_list, type);
		//}

		if (notify_list.item_size() > 0) {
			SendMessage(notify_list);
		}
		return result;
	}

	bool MtPlayer::AddItemByIdWithoutNotify(const int32 config_id,
		const int32 count, Config::ItemAddLogType type,
		const int64 source_id /*= 0*/,
		Packet::ActorFigthFormation formation/* = Packet::ActorFigthFormation::AFF_NORMAL_HOOK*/)
	{
		if (count <= 0)
			return false;

		return AddItemById(config_id, count, type, nullptr, source_id, formation);
	}
	
	bool MtPlayer::AddCard(const int32 config_id, const int32 count, Config::ItemAddLogType type, Packet::NotifyItemList *nlist, const int64 source_id)
	{
		if (count <= 0)
			return false;

		auto item_config = MtItemManager::Instance().GetItemConfig(config_id);
		if (item_config == nullptr) {
			return false;
		}
		auto config = MtUpgradeConfigManager::Instance().FragmentConfig(item_config->value());
		if (config == nullptr) {
			return false;
		}

		//检查英雄是否已存在
		auto actors = Actors();
		auto actor_it = std::find_if(std::begin(actors), std::end(actors), boost::bind(&MtActor::ConfigId, _1) == config->hero_id());
		if (actor_it != std::end(actors)) {
			//转化为碎片
			if (AddItemEx(item_config->value(), config->smelt_amount()*count, type, source_id)){
				if (nlist != nullptr) {
					nlist->set_add_type(type);
					nlist->set_source_id(source_id);

					//加一个碎片转化的标记
					AddItemNotify(config_id, count, nlist,type,1);
				}
				return true;
			}
		}
		else{//直接给英雄
			AddHero(config->hero_id(), Packet::ActorType::Hero, "draw", true,
				config->star(), config->color());
			if (nlist != nullptr) {
				nlist->set_add_type(type);
				nlist->set_source_id(source_id);

				//首个礼包英雄不展示
				auto ud = 2;
				if (source_id == 20125011 || source_id == 20125012 ) {
					ud = 0;
				}
				AddItemNotify(config_id, count, nlist, type, ud);
			}
			return true;
		}

		return false;
	}

	bool MtPlayer::AddItemById(const int32 config_id, const int32 count, 
		Config::ItemAddLogType type,Packet::NotifyItemList *nlist /*=nullptr*/,
		const int64 source_id/* = 0*/,
		Packet::ActorFigthFormation formation /*= Packet::ActorFigthFormation::AFF_NORMAL_HOOK*/)
	{
		if (GetParams32("nogain") > 0) {
			return false;
		}

		if (count <= 0) {
			return false;
		}

		//递归调用检查,防止开箱子道具递归调用相同的开箱子道具！
		if (config_id == (int32)source_id) {
			ZXERO_ASSERT(false) << "AddItemById recursion call! config_id=" << config_id << "ItemAddLogType=" << type << " player guid=" << Guid();
			return false;
		}

		if (config_id >= Packet::TokenType::Token_Gold && config_id <= (int32)Packet::TokenType_MAX){
			if (AddToken((Packet::TokenType)config_id, count,type, source_id, formation)) {
				if (nlist != nullptr) {
					nlist->set_add_type(type);
					nlist->set_source_id(source_id);

					AddItemNotify(config_id, count, nlist,type);
				}
				return true;
			}
		}
		else {


			OnLuaFunCall_x("xOnAddItemNotify",
			{
				{ "value",config_id },
				{ "count",count },
				{ "type",type },
				{ "source_id",(int32)source_id }
			});

			auto item_type = MtItemManager::Instance().GetItemType(config_id);
			if (item_type == Config::ItemType::CARD)	{//整卡
				return AddCard(config_id, count, type, nlist,source_id);
			}
			else{
				//自动使用逻辑
				int32 lua_ret = -1;
				auto item_config = MtItemManager::Instance().GetItemConfig(config_id);
				if(item_config!=nullptr && item_config->script_id() >= 0) {
					try {
						//背包满了发邮件
						std::map<int32, int32> temp;
						temp[config_id] = 1;
						if (!EquipAndItemBagLeftCheck(temp)) {
							google::protobuf::RepeatedPtrField<Packet::ItemCount> items;
							auto item_ = items.Add();
							item_->set_itemid(config_id);
							item_->set_itemcount(count);
							auto mail = static_message_manager::Instance().create_message(Guid(), shared_from_this(),  MAIL_OPT_TYPE::FULLBAG_ADD, "BagIsFull", "BagIsFull", &items);
							return true;
						}

						lua_ret = thread_lua->call<int32>(item_config->script_id(), "LuaAutoUse", this, item_config, count);
						if (lua_ret == 0) {
							return true;
						}
					}
					catch (ff::lua_exception& e) {
						e;
						SendCommonResult(Packet::ResultOption::Default_Opt, Packet::ResultCode::LuaScriptException);
						return false;
					}
				}
				if (lua_ret != 0) {//说明没有OnAutoUse

					if (AddItemEx(config_id, count, type, source_id)) {
						if (nlist != nullptr) {
							nlist->set_add_type(type);
							nlist->set_source_id(source_id);

							//抽卡奖励不走自动叠加通知
							AddItemNotify(config_id, count, nlist, type);
						}
						return true;
					}
				}				
			}
		}	
		LOG_INFO << "add item failed! player:" << Guid() << ",AddItem:" << config_id << "," << count << "," << type << "," << source_id;
		return false;
	}

	bool MtPlayer::AddItemEx(const int32 config_id, const int32 count, Config::ItemAddLogType type, const int64 source_id)
	{
		auto container = FindContainer(MtItemManager::Instance().CheckOwnerContainer(config_id));
		if (container) {
			auto old_count = container->GetItemCount(config_id);
			if (container->AddItemEx(config_id, count, type)){
				OnLuaFunCall_x("xOnAddItem",
				{
					{ "value",config_id }
				});

				//活动奖励统计
				StatisticsResolve(config_id, count, type, source_id);

				//mqdblopg 包含模型ID的资源类型流水日志表
				std::string tstr = MtTimerManager::Instance().NowTimeString();
				std::string dstr = MtTimerManager::Instance().TimeString2DateString(tstr);
				auto new_count = container->GetItemCount(config_id);
				boost::format fmt("INSERT INTO `resource_haveid_log` (`ServerID`,`PartnerID`,`UserID`,`PlayerID`,`VipLevel`,`ModuleID`,`SourceModelID`,`BeforeValue`,`AfterValue`,`EffectValue`,`CrDate`,`CrTime`) VALUES('%d','%d','%s','%llu','%d','%d','%d','%d','%d','%d','%s','%s');");
				fmt% MtConfig::Instance().server_group_id_%account_->partnerid() % account_->userid() % Guid() % GetAllCharge() % type % config_id % old_count % new_count % (new_count-old_count) % dstr %tstr;
				SendLogMessage(fmt.str());

				LOG_INFO << "player:" << Guid() << ",AddItem:" << config_id << "," << count << "," << type << "," << source_id;

				return true;

			}
		}
		return false;
	}

	bool MtPlayer::AddItem(const boost::shared_ptr<MtItemBase>& item, Config::ItemAddLogType type, const int64 source_id/* = 0*/)
	{
		auto container = FindContainer(MtItemManager::Instance().CheckOwnerContainer(item->ConfigId()));
		if (container) {
			auto old_count = container->GetItemCount(item->ConfigId());
			if (container->AddItem(item, type))	{
				OnLuaFunCall_x("xOnAddItem",
				{
					{ "value",item->ConfigId() }
				});
				
				//mqdblopg 包含模型ID的资源类型流水日志表
				std::string tstr = MtTimerManager::Instance().NowTimeString();
				std::string dstr = MtTimerManager::Instance().TimeString2DateString(tstr);
				auto new_count = container->GetItemCount(item->ConfigId());
				boost::format fmt("INSERT INTO `resource_haveid_log` (`ServerID`,`PartnerID`,`UserID`,`PlayerID`,`VipLevel`,`ModuleID`,`SourceModelID`,`BeforeValue`,`AfterValue`,`EffectValue`,`CrDate`,`CrTime`) VALUES('%d','%d','%s','%llu','%d','%d','%d','%d','%d','%d','%s','%s');");
				fmt% MtConfig::Instance().server_group_id_%account_->partnerid() % account_->userid() % Guid() % GetAllCharge() % type % item->ConfigId() % old_count % new_count % (new_count - old_count) % dstr %tstr;
				SendLogMessage(fmt.str());

				//活动奖励统计
				StatisticsResolve(item->ConfigId(), item->Count(), type, source_id);

				LOG_INFO << "player:" << Guid() << ",AddItem:" << item->ConfigId() << "," << item->Count() << "," << type << "," << source_id;

				return true;	
			}
		}
		return false;
	}

	void MtPlayer::CheckAcrDrop(int32 actype) 
	{
		//限时掉落活动事件	
		if (actype > 0) {
			OnLuaFunCall_x("xAcrDropCheck",
			{
				{ "value", actype }
			});
		}
	}
	void MtPlayer::StatisticsResolve(const int32 item_id, const int32 count, Config::ItemAddLogType type, const int64 source_id)
	{
		if (type != Config::ItemAddLogType::AddType_Mission
			&& type != Config::ItemAddLogType::AddType_Activity
			&& type != Config::ItemAddLogType::AddType_Arrange
			&& type != Config::ItemAddLogType::AddType_Guild) {
			return;
		}
		int32 statistics = (int32)source_id;
		if (type == Config::ItemAddLogType::AddType_Mission ||
			type == Config::ItemAddLogType::AddType_Arrange ) {
			auto config = MtMissionManager::Instance().FindMission((int32)(source_id));
			if (config) {
				statistics = OnLuaFunCall_1(501, "MissTypeToActivityType", config->type());
			}
		}
		//活动统计,对应global_funtion.lua,g_statistics_type类型
		int32 type_count = thread_lua->call<int32>(10, "GetLuaTBSize", "g_statistics_type");
		if (statistics > 0 && statistics < type_count) {
			auto iter = statistics_data_.find(statistics);
			if (iter == statistics_data_.end()) {
				std::map<int32, int32> temp;
				temp .insert({ item_id ,count });
				statistics_data_.insert({ statistics,temp });
			}
			else {
				auto temp = iter->second.find(item_id);
				if (temp == iter->second.end()) {
					iter->second.insert({ item_id ,count });
				}
				else {
					iter->second[item_id] += count;
				}
			}
		}
	}

	void MtPlayer::SendStatisticsResult(const int32 staticstics,bool reset)
	{
		auto iter = statistics_data_.find(staticstics);
		if (iter == statistics_data_.end()) {
			return;
		}
		
		Packet::StatisticsResult result;
		result.set_statistics(staticstics);
		for (auto child : iter->second){
			auto item = result.add_item();
			if (item != nullptr) {
				item->set_config_id(child.first);
				item->set_amount(child.second);
				item->set_ud(-1);
			}
		}
		SendMessage(result);


		if (reset) {
			ResetStatistics(staticstics);
		}
	}

	void MtPlayer::ResetStatistics(const int32 statistics)
	{
		if (statistics_data_.find(statistics) != statistics_data_.end()) {
			statistics_data_[statistics].clear();
		}
	}

	bool MtPlayer::DelItems(const std::vector<boost::shared_ptr<MtItemBase>>& items, Config::ItemDelLogType type, const int64 source_id/* = 0*/)
	{
		std::map<Packet::ContainerType, std::vector<uint64>> deleted_items;
		for (auto& item : items) {
			auto container = FindContainer(MtItemManager::Instance().CheckOwnerContainer(item->ConfigId()));
			if (container) {				
				int32 config_id = item->ConfigId();
				int32 count = item->Count();
				auto old_count = container->GetItemCount(config_id);
				if (container->RemoveItem(item, true, false))
				{
					deleted_items[container->Type()].push_back(item->Guid());

					LOG_INFO << "player:" << Guid() << ",DelItem:" << item->ConfigId() << "," << "," << count << "," << type << "," << source_id;

					//mqdblopg 包含模型ID的资源类型流水日志表
					std::string tstr = MtTimerManager::Instance().NowTimeString();
					std::string dstr = MtTimerManager::Instance().TimeString2DateString(tstr);
					auto new_count = container->GetItemCount(config_id);
					boost::format fmt("INSERT INTO `resource_haveid_log` (`ServerID`,`PartnerID`,`UserID`,`PlayerID`,`VipLevel`,`ModuleID`,`SourceModelID`,`BeforeValue`,`AfterValue`,`EffectValue`,`CrDate`,`CrTime`) VALUES('%d','%d','%s','%llu','%d','%d','%d','%d','%d','%d','%s','%s');");
					fmt% MtConfig::Instance().server_group_id_%account_->partnerid() % account_->userid() % Guid() % GetAllCharge() % type % config_id % old_count % new_count % (new_count - old_count) % dstr %tstr;
					SendLogMessage(fmt.str());

				} else {
					return false;
				}
			}
		}
		for (auto& pair : deleted_items) {
			Packet::PlayerContainerDelItemNotify notify;
			notify.set_container_type(pair.first);
			for (auto& guid : pair.second) {
				notify.add_guids(guid);
			}
			SendMessage(notify);
		}
		return true;
	}

	bool MtPlayer::DelItemById(const int32 config_id, const int32 count, Config::ItemDelLogType type, const int64 source_id/* = 0*/)
	{
		if (config_id >= Packet::TokenType::Token_Gold && config_id <= Packet::TokenType_MAX){
			return CostToken((Packet::TokenType)config_id, count,type, source_id);
		}
		else{
			auto container = FindContainer(MtItemManager::Instance().CheckOwnerContainer(config_id));
			if (container) {
				auto old_count = container->GetItemCount(config_id);
				if (container->DelItem(config_id, count))
				{
					OnLuaFunCall_x("xCostItem",
					{
						{ "value",config_id },
						{ "count",count }
					});

					LOG_INFO << "player:" << Guid() << ",DelItemById:" << config_id << "," << count << "," << type << "," << source_id;

					//mqdblopg 包含模型ID的资源类型流水日志表
					std::string tstr = MtTimerManager::Instance().NowTimeString();
					std::string dstr = MtTimerManager::Instance().TimeString2DateString(tstr);
					auto new_count = container->GetItemCount(config_id);
					boost::format fmt("INSERT INTO `resource_haveid_log` (`ServerID`,`PartnerID`,`UserID`,`PlayerID`,`VipLevel`,`ModuleID`,`SourceModelID`,`BeforeValue`,`AfterValue`,`EffectValue`,`CrDate`,`CrTime`) VALUES('%d','%d','%s','%llu','%d','%d','%d','%d','%d','%d','%s','%s');");
					fmt% MtConfig::Instance().server_group_id_%account_->partnerid() % account_->userid() % Guid() % GetAllCharge() % type % config_id % old_count % new_count % (new_count - old_count) % dstr %tstr;
					SendLogMessage(fmt.str());

					return true;
				}
			}
			return false;
		}
	}

	int32 MtPlayer::GetBagItemCount(const int32 config_id)
	{
		auto container = FindContainer(MtItemManager::Instance().CheckOwnerContainer(config_id));
		if (container != nullptr) {
			return container->GetItemCount(config_id);
		}	
		return 0;
	}

	bool MtPlayer::MoveItem(const boost::shared_ptr<MtItemBase>& item, const Packet::ContainerType desType)
	{
		if (item->Container()->Type() == desType)
			return false;

		if (desType == Packet::ContainerType::Layer_Bank
			||desType == Packet::ContainerType::Layer_ItemBag
			||desType == Packet::ContainerType::Layer_EquipBag) 
		{
			auto container = FindContainer(desType);
			if (container == nullptr) {
				return false;
			}
			return container->AddItem(item);
		}
		else 
		{
			ZXERO_ASSERT(false) << "MoveItem unsupport dest container type ! guid=" << Guid();
			return false;
		}
	}

	const boost::shared_ptr<MtContainer> MtPlayer::FindContainer(const Packet::ContainerType layer)
	{
		auto container = containers_.find(layer);
		if (container != containers_.end()) {
			return container->second;
		}
		else {
			ZXERO_ASSERT(false) << "Invalid ContainerType! type=" << layer;
			return nullptr;
		}	
	}

	bool MtPlayer::BagLeftCapacity(const Packet::ContainerType layer, const int32 check_count)
	{
		auto container = containers_.find(layer);
		if (container != containers_.end()) {
			if ( container->second->LeftSize() < check_count) {
				return false;
			}
			return true;
		}
		else {
			if (layer == Packet::ContainerType::Layer_EquipAndItemBag) {
				if (containers_[Packet::ContainerType::Layer_EquipBag]->LeftSize() < check_count
					|| containers_[Packet::ContainerType::Layer_ItemBag]->LeftSize() < check_count) {
					return false;
				}
				return true;
			}
			else if (layer == Packet::ContainerType::Layer_MissionAndItemBag) {
				if (containers_[Packet::ContainerType::Layer_MissionBag]->LeftSize() < check_count
					|| containers_[Packet::ContainerType::Layer_ItemBag]->LeftSize() < check_count) {
					return false;
				}
				return true;
			}
			else if (layer == Packet::ContainerType::Layer_AllBag) {
				if (containers_[Packet::ContainerType::Layer_MissionBag]->LeftSize() < check_count
					|| containers_[Packet::ContainerType::Layer_ItemBag]->LeftSize() < check_count
					|| containers_[Packet::ContainerType::Layer_MissionBag]->LeftSize() < check_count) {
					return false;
				}
				return true;
			}
			else if (layer == Packet::ContainerType::Layer_TokenBag) {
				return true;
			}
			else {
				ZXERO_ASSERT(false) << "Invalid ContainerType! type="<<layer;
				return false;
			}
		}
	}

	bool MtPlayer::EquipAndItemBagLeftCheck(const std::map<int32, int32>& items)
	{
		int32 item_block = 0;
		int32 equip_block = 0;
		for (auto child : items) {
			auto container_type = MtItemManager::Instance().CheckOwnerContainer(child.first);
			if (container_type == Packet::ContainerType::Layer_ItemBag){
				item_block += containers_[container_type]->CheckBlockCost(child.first, child.second);
			}
			else if(container_type == Packet::ContainerType::Layer_EquipBag){				
				equip_block += containers_[container_type]->CheckBlockCost(child.first, child.second);			
			}
		}
		if (containers_[Packet::ContainerType::Layer_ItemBag]->LeftSize() < item_block) {
			SendClientNotify("ItemBagIsFull", -1, -1);
			return false;
		}
		else if (containers_[Packet::ContainerType::Layer_EquipBag]->LeftSize() < equip_block) {
			SendClientNotify("EquipBagIsFull", -1, -1);
			return false;
		}

		return true;
	}
	const boost::shared_ptr<MtItemBase> MtPlayer::FindItemByGuid(const Packet::ContainerType layer, zxero::uint64 guid)
	{
		if (layer == Packet::ContainerType::Layer_AllBag){
			auto item = containers_[Packet::ContainerType::Layer_MissionBag]->FindItemByGuid(guid);
			if (item == nullptr) {
				item = containers_[Packet::ContainerType::Layer_ItemBag]->FindItemByGuid(guid);
				if (item == nullptr) {
					item = containers_[Packet::ContainerType::Layer_EquipBag]->FindItemByGuid(guid);
				}
			}
			return item;
		}
		else if (layer == Packet::ContainerType::Layer_EquipAndItemBag) {
			auto item = containers_[Packet::ContainerType::Layer_ItemBag]->FindItemByGuid(guid);
			if (item == nullptr) {
				item = containers_[Packet::ContainerType::Layer_EquipBag]->FindItemByGuid(guid);
			}
			return item;
		}
		else if (layer == Packet::ContainerType::Layer_MissionAndItemBag) {
			auto item = containers_[Packet::ContainerType::Layer_ItemBag]->FindItemByGuid(guid);
			if (item == nullptr) {
				item = containers_[Packet::ContainerType::Layer_MissionBag]->FindItemByGuid(guid);
			}
			return item;
		}
		else {
			auto container = FindContainer(layer);
			if (container == nullptr) {
				return nullptr;
			}
			return container->FindItemByGuid(guid);
		}
	}

	const boost::shared_ptr<MtItemBase> MtPlayer::FindItemByConfigId(const Packet::ContainerType layer, int32 config_id)
	{
		if (layer == Packet::ContainerType::Layer_AllBag) {
			auto item = containers_[Packet::ContainerType::Layer_MissionBag]->FindItemByConfigId(config_id);
			if (item == nullptr) {
				item = containers_[Packet::ContainerType::Layer_ItemBag]->FindItemByConfigId(config_id);
				if (item == nullptr) {
					item = containers_[Packet::ContainerType::Layer_EquipBag]->FindItemByConfigId(config_id);
				}
			}
			return item;
		}
		else if (layer == Packet::ContainerType::Layer_EquipAndItemBag) {
			auto item = containers_[Packet::ContainerType::Layer_ItemBag]->FindItemByConfigId(config_id);
			if (item == nullptr) {
				item = containers_[Packet::ContainerType::Layer_EquipBag]->FindItemByConfigId(config_id);
			}
			return item;
		}
		else if (layer == Packet::ContainerType::Layer_MissionAndItemBag) {
			auto item = containers_[Packet::ContainerType::Layer_ItemBag]->FindItemByConfigId(config_id);
			if (item == nullptr) {
				item = containers_[Packet::ContainerType::Layer_MissionBag]->FindItemByConfigId(config_id);
			}
			return item;
		}
		else {
			auto container = FindContainer(layer);
			if (container == nullptr) {
				return nullptr;
			}
			return container->FindItemByConfigId(config_id);
		}
	}

	void MtPlayer::fill_hyper_iteminfo(std::string message, Packet::MessageHyperItemInfo &item)
	{
		std::vector<std::string> ahref_str;
		utils::regexstring(message, "<a item=\\d+>", ahref_str);
		for (auto ahref_iter : ahref_str)
		{
			std::vector<std::string> itemguid_str;
			utils::regexstring(ahref_iter, "\\d+", itemguid_str);
			for (auto itemguid_iter : itemguid_str)
			{
				uint64 itemguid = utils::guid_string_to_uint64(itemguid_iter);
				auto item_ = FindItemByGuid(Packet::ContainerType::Layer_AllBag,itemguid);
				if (item_ == nullptr) {
					item_ = FindItemByGuid(Packet::ContainerType::Layer_Equip,itemguid); //穿在身上的装备
				}
				if (item_ != nullptr)
				{
					auto item_info = item.add_iteminfo();
					item_->FillMessage(*item_info);
				}
			}
		}
	}
	
	boost::shared_ptr<MtActor> MtPlayer::MainActor() const
	{
		return main_actor_;
	}

	int32 MtPlayer::PlayerLevel() const
	{
		auto mc = MainActor();
		if (mc == nullptr)
			return 1;
				
		return mc->Level();
	}

	void MtPlayer::OnLevelUpTo(const int32 level)
	{
		if (player_data_ != nullptr) {
			auto old_level = player_data_->level();
			player_data_->set_level(level);

			//刷新一下活动列表状态
			ActivityListRefresh();

			//单独更新一下player的level变量
			Packet::CommonReply reply;
			reply.set_reply_name("player_level");
			reply.add_int32_params(level);
			SendMessage(reply);

			if (GetGuildID() != INVALID_GUID)
			{
				Server::Instance().SendS2GCommonMessage("S2GSyncGuildUserLevel", { level }, { (int64)Guid(),(int64)GetGuildID() }, { });
			}
			if (GetTeamID() != INVALID_GUID)
			{
				Server::Instance().SendS2GCommonMessage("S2GSyncTeamMemberLevel", { level }, { (int64)Guid(),(int64)GetTeamID() }, {});
			}
			MtTopRankManager::Instance().OnPlayerLevelChanged(shared_from_this());
			OnLuaFunCall_x("xOnLevelUpTo",
			{
				{ "value", level }
			});

			UpdateWebPlayerInfo();

			//更新一下静态商店
			for (auto child : markets_) {
				if (child->IsStatic()) {
					child->Refresh(old_level>level);
				}
			}

			LOG_INFO << "player:" << Guid() << ",OnLevelUpTo:" << level;

			//mqdblopg 包含模型ID的资源类型流水日志表
			std::string tstr = MtTimerManager::Instance().NowTimeString();
			std::string dstr = MtTimerManager::Instance().TimeString2DateString(tstr);
			boost::format fmt("INSERT INTO `player_lv_change_log` (`PartnerID`,`ServerID`,`UserID`,`PlayerID`,`VipLevel`,`ModuleID`,`BeforeValue`,`EffectValue`,`AfterValue`,`Crtime`,`Crdate`) VALUES('%d','%d','%s','%llu','%d','%d','%d','%d','%d','%s','%s');");
			fmt% account_->partnerid() % MtConfig::Instance().server_group_id_% account_->userid() % Guid() % GetAllCharge() % 0% old_level % (level-old_level) % level % tstr %dstr;
			SendLogMessage(fmt.str());
		}		
	}

	const boost::shared_ptr<MtMarket> MtPlayer::FindMarket(Config::MarketType type) const	{
		auto iter = std::find_if(std::begin(markets_), std::end(markets_), boost::bind(&MtMarket::GetType, _1) == type);
		if (iter == std::end(markets_)) {
			return nullptr;
		}
		else {
			return *iter;
		}
	}

	void MtPlayer::InitMount()
	{
		auto id = GetScenePlayerInfo()->mount_id();
		if (id >= 0) {
			auto mount = MtMonsterManager::Instance().FindMount(id);
			if (mount == nullptr) {
				return;
			}
			//MountSpeedImpact
			simple_impacts_.AddSimpleImpact(mount->impact_id(), 99999999, mount->speed(), 0, false);
			ResetSpeed();
		}
	}
	void MtPlayer::InitMarket()
	{
		for (auto child : markets_) {
			child->Refresh();
		}
	}

	void MtPlayer::InitActivity()
	{
		for (auto child : activities_) {
			int32 times = OnLuaFunCall_1(501, "GetActivityTimes", child.first);			
			auto activity_db = child.second->ActivityDB();
			if (activity_db != nullptr) {
				activity_db->set_times(times < 0 ? 0 : times);
				auto ret= (Packet::ResultCode)MtActivityManager::Instance().IsActive(&(activity_db->config()), PlayerLevel()) == Packet::ResultCode::ResultOK ? 1 : 0;
				activity_db->set_is_active(ret);
			}
		}

		//答题存档数据
		LoadAnswerCache();

		//上线的活动通知lua接口
		for (auto child : activities_) {
			if (child.second->ActivityDB()->is_active()) {
				OnLuaFunCall_n(501, "OnActivityNotify", 
				{
					{ "type",child.first }
				});
			}
		}
	}

	bool MtPlayer::AddMarket(const boost::shared_ptr<MtMarket> &market)
	{
		if (market == nullptr)
			return false;

		auto iter = std::find_if(std::begin(markets_), std::end(markets_), boost::bind(&MtMarket::GetType, _1) == market->GetType());
		if (iter != std::end(markets_)) {
			return false;
		}
		else {			
			markets_.push_back(market);
			return true;
		}
	}


	bool MtPlayer::AddActivity(const boost::shared_ptr<MtActivity> &activity)
	{
		if (activity == nullptr)
			return false;

		int32 id = activity->ActivityDB()->config().id();
		auto iter = activities_.find(id);
		if (iter != std::end(activities_)) {
			return false;
		}
		else {
			activities_.insert({ id,activity });
			return true;
		}
	}

	boost::shared_ptr<MtActivity> MtPlayer::GetActivity(const int32 type)
	{
		auto iter = activities_.find(type);
		if (iter != std::end(activities_)) {
			return iter->second;
		}		
		return nullptr;
	}

	Packet::ResultCode MtPlayer::ActivityCheck(const int32 type) 
	{
		auto script_id = OnLuaFunCall_1(501, "AcTypeToAcScriptId", type);
		if (script_id > 0) {
			return (Packet::ResultCode)OnLuaFunCall_n(script_id, "OnJoinCheck",
			{
				{"activity_type",type }
			});
		}
		else {//默认检查
			auto activity = GetActivity(type);
			if (activity == nullptr) {
				return SendClientNotify("Activity_NotStart", -1, 1);
			}
			auto config = MtActivityManager::Instance().FindActivity(type);
			if (config == nullptr) {
				return SendClientNotify("Activity_NotStart", -1, 1);
			}
			auto ltimes = config->limit_times();
			//auto rtimes = config->reward_count();
			auto done_times = OnLuaFunCall_1(501, "GetActivityTimes", type);
			if (ltimes >= 0 && done_times >= ltimes) {
				return SendClientNotify("ac_notify_007|" + config->name(), -1, 1);
			}
		}
		return Packet::ResultCode::ResultOK;
	}
	
	bool MtPlayer::CostToken(const Packet::TokenType token, const int32 cost_count, Config::ItemDelLogType type, const int64 source_id /*= 0*/)
	{
		if (cost_count <= 0) {
			return true;
		}
		int32 oldtoken = 0;
		int32 newtoken = 0;
		auto token_data = player_db_->mutable_token_data();
		if (token_data == nullptr)
			return false;
		switch (token)
		{
		case Packet::TokenType::Token_Gold:
		{
			oldtoken = token_data->gold();
			if (oldtoken < cost_count)
				return false;
			newtoken = oldtoken - cost_count;
			token_data->set_gold(newtoken);

			std::stringstream DBlog;
			DBlog << "CostGold," << PlayerBibase() << "," << type <<","<< source_id<<","<< oldtoken << "," << cost_count << "," << newtoken;
			Server::Instance().AddUrlTask(new db_log_require_work(DBlog.str(), __FILE__, __LINE__));

			//mqdblopg 包含模型ID的资源类型流水日志表
			std::string tstr = MtTimerManager::Instance().NowTimeString();
			std::string dstr = MtTimerManager::Instance().TimeString2DateString(tstr);
			boost::format fmt("INSERT INTO `resource_haveid_log` (`ServerID`,`PartnerID`,`UserID`,`PlayerID`,`VipLevel`,`ModuleID`,`SourceModelID`,`BeforeValue`,`AfterValue`,`EffectValue`,`CrDate`,`CrTime`) VALUES('%d','%d','%s','%llu','%d','%d','%d','%d','%d','%d','%s','%s');");
			fmt% MtConfig::Instance().server_group_id_%account_->partnerid() % account_->userid() % Guid() % GetAllCharge() % type % token % oldtoken % newtoken % cost_count % dstr %tstr;
			SendLogMessage(fmt.str());
		}
			break;
		case Packet::TokenType::Token_Crystal:
		{
			oldtoken = token_data->crystal();
			if (oldtoken < cost_count)
				return false;
			newtoken = oldtoken - cost_count;
			token_data->set_crystal(newtoken);

			OnLuaFunCall_x("xOnConsume",
			{
				{ "value", cost_count }
			});

			std::stringstream DBlog;
			DBlog << "CostCrystal," << PlayerBibase() << "," << type << "," << source_id << "," << oldtoken << "," << cost_count << "," << newtoken;
			Server::Instance().AddUrlTask(new db_log_require_work(DBlog.str(), __FILE__, __LINE__));

			//mqdblopg 包含模型ID的资源类型流水日志表
			std::string tstr = MtTimerManager::Instance().NowTimeString();
			std::string dstr = MtTimerManager::Instance().TimeString2DateString(tstr);
			boost::format fmt("INSERT INTO `resource_haveid_log` (`ServerID`,`PartnerID`,`UserID`,`PlayerID`,`VipLevel`,`ModuleID`,`SourceModelID`,`BeforeValue`,`AfterValue`,`EffectValue`,`CrDate`,`CrTime`) VALUES('%d','%d','%s','%llu','%d','%d','%d','%d','%d','%d','%s','%s');");
			fmt% MtConfig::Instance().server_group_id_%account_->partnerid() % account_->userid() % Guid() % GetAllCharge() % type % token % oldtoken % newtoken % cost_count % dstr %tstr;
			SendLogMessage(fmt.str());
		}
			break;
		case Packet::TokenType::Token_Honor:
		{
			oldtoken = token_data->honor();
			if (oldtoken < cost_count)
				return false;
			newtoken = oldtoken - cost_count;
			token_data->set_honor(newtoken);

			std::stringstream DBlog;
			DBlog << "CostHonor," << PlayerBibase() << "," << type << "," << source_id << "," << oldtoken << "," << cost_count << "," << newtoken;
			Server::Instance().AddUrlTask(new db_log_require_work(DBlog.str(), __FILE__, __LINE__));

			//mqdblopg 包含模型ID的资源类型流水日志表
			std::string tstr = MtTimerManager::Instance().NowTimeString();
			std::string dstr = MtTimerManager::Instance().TimeString2DateString(tstr);
			boost::format fmt("INSERT INTO `resource_haveid_log` (`ServerID`,`PartnerID`,`UserID`,`PlayerID`,`VipLevel`,`ModuleID`,`SourceModelID`,`BeforeValue`,`AfterValue`,`EffectValue`,`CrDate`,`CrTime`) VALUES('%d','%d','%s','%llu','%d','%d','%d','%d','%d','%d','%s','%s');");
			fmt% MtConfig::Instance().server_group_id_%account_->partnerid() % account_->userid() % Guid() % GetAllCharge() % type % token % oldtoken % newtoken % cost_count % dstr %tstr;
			SendLogMessage(fmt.str());
		}
			break;
		case Packet::TokenType::Token_Reward:
		{
			oldtoken = token_data->reward();
			if (oldtoken < cost_count)
				return false;
			newtoken = oldtoken - cost_count;
			token_data->set_reward(newtoken);

			std::stringstream DBlog;
			DBlog << "CostTokenReward," << PlayerBibase() << "," << type << "," << source_id << "," << oldtoken << "," << cost_count << "," << newtoken;
			Server::Instance().AddUrlTask(new db_log_require_work(DBlog.str(), __FILE__, __LINE__));

			//mqdblopg 包含模型ID的资源类型流水日志表
			std::string tstr = MtTimerManager::Instance().NowTimeString();
			std::string dstr = MtTimerManager::Instance().TimeString2DateString(tstr);
			boost::format fmt("INSERT INTO `resource_haveid_log` (`ServerID`,`PartnerID`,`UserID`,`PlayerID`,`VipLevel`,`ModuleID`,`SourceModelID`,`BeforeValue`,`AfterValue`,`EffectValue`,`CrDate`,`CrTime`) VALUES('%d','%d','%s','%llu','%d','%d','%d','%d','%d','%d','%s','%s');");
			fmt% MtConfig::Instance().server_group_id_%account_->partnerid() % account_->userid() % Guid() % GetAllCharge() % type % token % oldtoken % newtoken % cost_count % dstr %tstr;
			SendLogMessage(fmt.str());

			break;
		}
		case Packet::TokenType::Token_Day: {
			oldtoken = token_data->dayvalue();
			if (oldtoken < cost_count)
				return false;
			newtoken = oldtoken - cost_count;
			token_data->set_dayvalue(newtoken);

			std::stringstream DBlog;
			DBlog << "CostTokenDay," << PlayerBibase() << "," << type << "," << source_id << "," << oldtoken << "," << cost_count << "," << newtoken;
			Server::Instance().AddUrlTask(new db_log_require_work(DBlog.str(), __FILE__, __LINE__));

			//mqdblopg 包含模型ID的资源类型流水日志表
			std::string tstr = MtTimerManager::Instance().NowTimeString();
			std::string dstr = MtTimerManager::Instance().TimeString2DateString(tstr);
			boost::format fmt("INSERT INTO `resource_haveid_log` (`ServerID`,`PartnerID`,`UserID`,`PlayerID`,`VipLevel`,`ModuleID`,`SourceModelID`,`BeforeValue`,`AfterValue`,`EffectValue`,`CrDate`,`CrTime`) VALUES('%d','%d','%s','%llu','%d','%d','%d','%d','%d','%d','%s','%s');");
			fmt% MtConfig::Instance().server_group_id_%account_->partnerid() % account_->userid() % Guid() % GetAllCharge() % type % token % oldtoken % newtoken % cost_count % dstr %tstr;
			SendLogMessage(fmt.str());

			break;
		}
		case Packet::TokenType::Token_BankGold:
			oldtoken = token_data->bank_gold();
			if (oldtoken < cost_count)
				return false;
			newtoken = oldtoken - cost_count;
			token_data->set_bank_gold(newtoken);
			break;
		case Packet::TokenType::Token_ExpPoint:
			oldtoken = token_data->exp_point();
			if (oldtoken < cost_count)
				return false;
			newtoken = oldtoken - cost_count;
			token_data->set_exp_point(newtoken);
			break;
		case Packet::TokenType::Token_ColdExpPoint:
			oldtoken = token_data->cold_exp_point();
			if (oldtoken < cost_count)
				return false;
			newtoken = oldtoken - cost_count;
			token_data->set_cold_exp_point(newtoken);
			break;
		case Packet::TokenType::Token_LawFul:
		{
			oldtoken = token_data->lawful();
			if (oldtoken < cost_count)
				return false;
			newtoken = oldtoken - cost_count;
			token_data->set_lawful(newtoken);

			std::stringstream DBlog;
			DBlog << "CostLawFul," << PlayerBibase() << "," << type << "," << source_id << "," << oldtoken << "," << cost_count << "," << newtoken;
			Server::Instance().AddUrlTask(new db_log_require_work(DBlog.str(), __FILE__, __LINE__));

			//mqdblopg 包含模型ID的资源类型流水日志表
			std::string tstr = MtTimerManager::Instance().NowTimeString();
			std::string dstr = MtTimerManager::Instance().TimeString2DateString(tstr);
			boost::format fmt("INSERT INTO `resource_haveid_log` (`ServerID`,`PartnerID`,`UserID`,`PlayerID`,`VipLevel`,`ModuleID`,`SourceModelID`,`BeforeValue`,`AfterValue`,`EffectValue`,`CrDate`,`CrTime`) VALUES('%d','%d','%s','%llu','%d','%d','%d','%d','%d','%d','%s','%s');");
			fmt% MtConfig::Instance().server_group_id_%account_->partnerid() % account_->userid() % Guid() % GetAllCharge() % type % token % oldtoken % newtoken % cost_count % dstr %tstr;
			SendLogMessage(fmt.str());
		}
			break;
		case Packet::TokenType::Token_GuildContribute:
		{
			int32 curcontribution = GetPlayerGuildProxy()->Getcurcontribution();
			if (curcontribution < cost_count)
				return false;
			curcontribution -= cost_count;
			GetPlayerGuildProxy()->Setcurcontribution(curcontribution);
			newtoken = curcontribution;

			std::stringstream DBlog;
			DBlog << "CostToken_GuildContribute," << PlayerBibase() << "," << type << "," << source_id << "," << oldtoken << "," << cost_count << "," << newtoken;
			Server::Instance().AddUrlTask(new db_log_require_work(DBlog.str(), __FILE__, __LINE__));

			//mqdblopg 包含模型ID的资源类型流水日志表
			std::string tstr = MtTimerManager::Instance().NowTimeString();
			std::string dstr = MtTimerManager::Instance().TimeString2DateString(tstr);
			boost::format fmt("INSERT INTO `resource_haveid_log` (`ServerID`,`PartnerID`,`UserID`,`PlayerID`,`VipLevel`,`ModuleID`,`SourceModelID`,`BeforeValue`,`AfterValue`,`EffectValue`,`CrDate`,`CrTime`) VALUES('%d','%d','%s','%llu','%d','%d','%d','%d','%d','%d','%s','%s');");
			fmt% MtConfig::Instance().server_group_id_%account_->partnerid() % account_->userid() % Guid() % GetAllCharge() % type % token % oldtoken % newtoken % cost_count % dstr %tstr;
			SendLogMessage(fmt.str());
		}
		break;
		case Packet::TokenType::Token_Exp:
			return false;
			break;

		default:
			return false;
		};
		
		Packet::CommonReply reply;
		reply.set_reply_name(Packet::TokenType_Name(token));
		reply.add_int64_params(newtoken);
		SendMessage(reply);
		
		return true;
	}
	void MtPlayer::OnChargeOk( const std::string& productid, const std::string& ip, const std::string & order_id,int32 price,int32 count, int32 charge_times, int32 extra_count )
	{
		//auto index = MtMarketManager::Instance().FindChargeIndex(channel_id, productid);
		//if (index == -1) {
		//	SendClientNotify("Invalid_Request", -1, -1);
		//	LOG_ERROR << "xOnChangeOk failed! invalid charge_list! channel_id=" << channel_id << "productid="<< productid<<" order_id="<< order_id<<" Guid=" << Guid();
		//	return;
		//}
		//auto config = MtMarketManager::Instance().FindChargeItem(index);
		//if (config == nullptr) {
		//	SendClientNotify("Invalid_Request", -1, -1);
		//	LOG_ERROR << "xOnChangeOk failed! index" << index << " channel_id=" << channel_id << "productid=" << productid << " order_id=" << order_id << " Guid=" << Guid();
		//	return;
		//}

		//auto price = config->rmb_price();
		//auto id = config->effect_value();
		//auto count = config->effect_count();
		//auto extra_count = config->extra_value();
		//auto charge_times = 0;

		////双倍判定
		//try {
		//	std::map<std::string, int32> temp;
		//	temp["count"] = count;
		//	temp["index"] = index;
		//	auto ret_value = thread_lua->call<std::vector<int32>>(100, "OnChargeDoubleCheck", this, temp);
		//	if (ret_value.size() != 2) {
		//		LOG_ERROR << "xOnChangeOk failed! index" << index << " channel_id=" << channel_id << "productid=" << productid << " order_id=" << order_id << " Guid=" << Guid();
		//		return;
		//	}
		//
		//	charge_times = ret_value[1];
		//	extra_count += ret_value[0];
		//	count += extra_count;

		//	if ( index == 0) {
		//		//终生卡
		//		if (datacell_container_->check_bit_data(Packet::BitFlagCellIndex::LifeCardFlag)) {
		//			SendClientNotify("ac_notify_016", -1, -1);
		//			LOG_ERROR << "xOnChangeOk failed! index" << index << " Guid=" << Guid();
		//			return;
		//		}
		//		datacell_container_->set_bit_data(Packet::BitFlagCellIndex::LifeCardFlag, true);
		//		AddItemByIdWithNotify(20140002, 4, Config::ItemAddLogType::AddType_Charge, 0);
		//		AddItemByIdWithNotify(20140001, 2, Config::ItemAddLogType::AddType_Charge, 0);
		//		AddItemByIdWithNotify(20140005, 2, Config::ItemAddLogType::AddType_Charge, 0);
		//	}
		//	else if ( index == 1) {
		//		//月卡
		//		if (datacell_container_->check_bit_data(Packet::BitFlagCellIndex::MonthCardFlag)) {
		//			SendClientNotify("ac_notify_017", -1, -1);
		//			LOG_ERROR << "xOnChangeOk failed! index" << index << " channel_id=" << channel_id << "productid=" << productid << " order_id=" << order_id << " Guid=" << Guid();
		//			return;
		//		}
		//		datacell_container_->set_bit_data(Packet::BitFlagCellIndex::MonthCardFlag, true);			
		//	}
		//	else if (index == 6) {
		//		//基金
		//		if (datacell_container_->check_bit_data(Packet::BitFlagCellIndex::InvestFlag)) {
		//			SendClientNotify("ac_notify_019", -1, -1);
		//			LOG_ERROR << "xOnChangeOk failed! index" << index << " channel_id=" << channel_id << "productid=" << productid << " order_id=" << order_id << " Guid=" << Guid();
		//			return;
		//		}
		//		datacell_container_->set_bit_data(Packet::BitFlagCellIndex::InvestFlag, true);
		//	}
		//	else{
		//		if (id <= 0 || count <= 0) {
		//			SendClientNotify("Invalid_Request", -1, -1);
		//			LOG_ERROR << "xOnChangeOk failed! index" << index << " channel_id=" << channel_id << "productid=" << productid << " order_id=" << order_id << " Guid=" << Guid();
		//			return;
		//		}
		//	}
		//
		//	AddAllCharge((int32)price);
		//	OnLuaFunCall_x("xOnCharge",
		//	{
		//		{ "value", (int32)price },
		//		{ "index", index },
		//	});

		//	if (id > 0 && count > 0) {
		//		AddItemByIdWithNotify(id, count, Config::ItemAddLogType::AddType_Charge, 0);
		//	}
		//	//log
			//LOG_INFO << "OnChangeOk! index=" << index << " channel_id=" << channel_id << "productid=" << productid << " order_id=" << order_id << " Guid=" << Guid();

			//mqdblopg 包含模型ID的资源类型流水日志表
			std::string tstr = MtTimerManager::Instance().NowTimeString();
			std::string dstr = MtTimerManager::Instance().TimeString2DateString(tstr);
			boost::format fmt("INSERT INTO `charge_log` (`OrderID`,`ServerID`,`PartnerID`,`UserID`,`PlayerID`,`VipLevel`,`PlayerLevel`,`DeviceIdentifier`,`ProductID`,`ChargeMoney`,`ActivityChargeMoney`,`GamePoint`,`ActivityGamePoint`,`CurChargeTimes`,`IP`,`CrDate`,`CrTime`) VALUES('%s','%d','%d','%s','%llu','%d','%d','%s','%s','%d','%d','%d','%d','%d','%s','%s','%s');");
			fmt % order_id
				% MtConfig::Instance().server_group_id_%account_->partnerid() % account_->userid() % Guid()
				% GetAllCharge() % PlayerLevel()
				% account_->udid() % productid
				% price % 0 % count % extra_count % (charge_times+1)
				% ip % dstr %tstr;
			SendLogMessage(fmt.str());

		//}
		//catch (ff::lua_exception& e) {
		//	LOG_ERROR << " player guid=" << Guid() << e.what();
		//	SendCommonResult(Packet::ResultOption::Default_Opt, Packet::ResultCode::LuaScriptException);
		//}
	}
	void MtPlayer::MqGenerateOrderID(std::string ProductId, std::string Extra)
	{
		Server::Instance().AddUrlTask(new GenerateOrderIDWork(shared_from_this(), ProductId, Extra));
		LOG_INFO << "MqGenerateOrderID Player="<<Guid()<<",ProductId="<< ProductId<<",Extra="<< Extra;
	}
	bool MtPlayer::AddToken(const Packet::TokenType token,
		const int32 add_count, Config::ItemAddLogType type, 
		const int64 source_id/* = 0*/,
		Packet::ActorFigthFormation formation /*= Packet::ActorFigthFormation::AFF_NORMAL_HOOK*/)
	{
		if (add_count <= 0) {
			return true;
		}
		int32 oldtoken = 0;
		int32 newtoken = 0;
		auto token_data = player_db_->mutable_token_data();
		if (token_data == nullptr)
			return false;

		switch (token)
		{
		case Packet::TokenType::Token_Gold:
		{
			oldtoken = token_data->gold();
			newtoken = oldtoken + add_count;
			token_data->set_gold(newtoken);
			if (Config::ItemAddLogType::AddType_MonsterDrop != type && Config::ItemAddLogType::AddType_Hook != type)
			{
				std::stringstream DBlog;
				DBlog << "AddGold," << PlayerBibase() << "," << type << "," << source_id << "," << oldtoken << "," << add_count << "," << newtoken;
				Server::Instance().AddUrlTask(new db_log_require_work(DBlog.str(), __FILE__, __LINE__));
				
				//mqdblopg 包含模型ID的资源类型流水日志表
				std::string tstr = MtTimerManager::Instance().NowTimeString();
				std::string dstr = MtTimerManager::Instance().TimeString2DateString(tstr);
				boost::format fmt("INSERT INTO `resource_haveid_log` (`ServerID`,`PartnerID`,`UserID`,`PlayerID`,`VipLevel`,`ModuleID`,`SourceModelID`,`BeforeValue`,`AfterValue`,`EffectValue`,`CrDate`,`CrTime`) VALUES('%d','%d','%s','%llu','%d','%d','%d','%d','%d','%d','%s','%s');");
				fmt% MtConfig::Instance().server_group_id_%account_->partnerid() % account_->userid() % Guid() % GetAllCharge() % type % token % oldtoken % newtoken % add_count % dstr %tstr;
				SendLogMessage(fmt.str());
			}
		}
			break;
		case Packet::TokenType::Token_Crystal:
		{
			oldtoken = token_data->crystal();
			newtoken = oldtoken + add_count;
			token_data->set_crystal(newtoken);
			std::stringstream DBlog;
			DBlog << "AddCrystal," << PlayerBibase() << "," << type << "," << source_id << "," << oldtoken << "," << add_count << "," << newtoken;
			Server::Instance().AddUrlTask(new db_log_require_work(DBlog.str(), __FILE__, __LINE__));

			//mqdblopg 包含模型ID的资源类型流水日志表
			std::string tstr = MtTimerManager::Instance().NowTimeString();
			std::string dstr = MtTimerManager::Instance().TimeString2DateString(tstr);
			boost::format fmt("INSERT INTO `resource_haveid_log` (`ServerID`,`PartnerID`,`UserID`,`PlayerID`,`VipLevel`,`ModuleID`,`SourceModelID`,`BeforeValue`,`AfterValue`,`EffectValue`,`CrDate`,`CrTime`) VALUES('%d','%d','%s','%llu','%d','%d','%d','%d','%d','%d','%s','%s');");
			fmt% MtConfig::Instance().server_group_id_%account_->partnerid() % account_->userid() % Guid() % GetAllCharge() % type % token % oldtoken % newtoken % add_count % dstr %tstr;
			SendLogMessage(fmt.str());
		}
			break;
		case Packet::TokenType::Token_Honor:
		{
			oldtoken = token_data->honor();
			newtoken = oldtoken + add_count;
			token_data->set_honor(newtoken);
			std::stringstream DBlog;
			DBlog << "AddHonor," << PlayerBibase() << "," << type << "," << source_id << "," << oldtoken << "," << add_count << "," << newtoken;
			Server::Instance().AddUrlTask(new db_log_require_work(DBlog.str(), __FILE__, __LINE__));

			//mqdblopg 包含模型ID的资源类型流水日志表
			std::string tstr = MtTimerManager::Instance().NowTimeString();
			std::string dstr = MtTimerManager::Instance().TimeString2DateString(tstr);
			boost::format fmt("INSERT INTO `resource_haveid_log` (`ServerID`,`PartnerID`,`UserID`,`PlayerID`,`VipLevel`,`ModuleID`,`SourceModelID`,`BeforeValue`,`AfterValue`,`EffectValue`,`CrDate`,`CrTime`) VALUES('%d','%d','%s','%llu','%d','%d','%d','%d','%d','%d','%s','%s');");
			fmt% MtConfig::Instance().server_group_id_%account_->partnerid() % account_->userid() % Guid() % GetAllCharge() % type % token % oldtoken % newtoken % add_count % dstr %tstr;
			SendLogMessage(fmt.str());
		}
			break;
		case Packet::TokenType::Token_Reward:
		{
			oldtoken = token_data->reward();
			newtoken = oldtoken + add_count;
			token_data->set_reward(newtoken);

			std::stringstream DBlog;
			DBlog << "Token_Reward," << PlayerBibase() << "," << type << "," << source_id << "," << oldtoken << "," << add_count << "," << newtoken;
			Server::Instance().AddUrlTask(new db_log_require_work(DBlog.str(), __FILE__, __LINE__));

			//mqdblopg 包含模型ID的资源类型流水日志表
			std::string tstr = MtTimerManager::Instance().NowTimeString();
			std::string dstr = MtTimerManager::Instance().TimeString2DateString(tstr);
			boost::format fmt("INSERT INTO `resource_haveid_log` (`ServerID`,`PartnerID`,`UserID`,`PlayerID`,`VipLevel`,`ModuleID`,`SourceModelID`,`BeforeValue`,`AfterValue`,`EffectValue`,`CrDate`,`CrTime`) VALUES('%d','%d','%s','%llu','%d','%d','%d','%d','%d','%d','%s','%s');");
			fmt% MtConfig::Instance().server_group_id_%account_->partnerid() % account_->userid() % Guid() % GetAllCharge() % type % token % oldtoken % newtoken % add_count % dstr %tstr;
			SendLogMessage(fmt.str());

			break;
		}
		case Packet::TokenType::Token_Day:
		{
			oldtoken = token_data->dayvalue();
			newtoken = oldtoken + add_count;
			token_data->set_dayvalue(newtoken);
			std::stringstream DBlog;
			DBlog << "AddTokenDay," << PlayerBibase() << "," << type << "," << source_id << "," << oldtoken << "," << add_count << "," << newtoken;
			Server::Instance().AddUrlTask(new db_log_require_work(DBlog.str(), __FILE__, __LINE__));

			//mqdblopg 包含模型ID的资源类型流水日志表
			std::string tstr = MtTimerManager::Instance().NowTimeString();
			std::string dstr = MtTimerManager::Instance().TimeString2DateString(tstr);
			boost::format fmt("INSERT INTO `resource_haveid_log` (`ServerID`,`PartnerID`,`UserID`,`PlayerID`,`VipLevel`,`ModuleID`,`SourceModelID`,`BeforeValue`,`AfterValue`,`EffectValue`,`CrDate`,`CrTime`) VALUES('%d','%d','%s','%llu','%d','%d','%d','%d','%d','%d','%s','%s');");
			fmt% MtConfig::Instance().server_group_id_%account_->partnerid() % account_->userid() % Guid() % GetAllCharge() % type % token % oldtoken % newtoken % add_count % dstr %tstr;
			SendLogMessage(fmt.str());
		}
			break;		
		case Packet::TokenType::Token_BankGold:
			oldtoken = token_data->bank_gold();
			newtoken = oldtoken + add_count;
			token_data->set_bank_gold(newtoken);
			break;
		case Packet::TokenType::Token_ExpPoint:
			oldtoken = token_data->exp_point();
			newtoken = oldtoken + add_count;
			token_data->set_exp_point(newtoken);
			
			break;
		case Packet::TokenType::Token_ColdExpPoint:
			oldtoken = token_data->cold_exp_point();
			newtoken = oldtoken + add_count;
			token_data->set_cold_exp_point(newtoken);
			break;
		case Packet::TokenType::Token_LawFul:
		{
			oldtoken = token_data->lawful();
			newtoken = oldtoken + add_count;
			token_data->set_lawful(newtoken);
			std::stringstream DBlog;
			DBlog << "AddLawFul," << PlayerBibase() << "," << type << "," << source_id << "," << oldtoken << "," << add_count << "," << newtoken;
			Server::Instance().AddUrlTask(new db_log_require_work(DBlog.str(), __FILE__, __LINE__));

			//mqdblopg 包含模型ID的资源类型流水日志表
			std::string tstr = MtTimerManager::Instance().NowTimeString();
			std::string dstr = MtTimerManager::Instance().TimeString2DateString(tstr);
			boost::format fmt("INSERT INTO `resource_haveid_log` (`ServerID`,`PartnerID`,`UserID`,`PlayerID`,`VipLevel`,`ModuleID`,`SourceModelID`,`BeforeValue`,`AfterValue`,`EffectValue`,`CrDate`,`CrTime`) VALUES('%d','%d','%s','%llu','%d','%d','%d','%d','%d','%d','%s','%s');");
			fmt% MtConfig::Instance().server_group_id_%account_->partnerid() % account_->userid() % Guid() % GetAllCharge() % type % token % oldtoken % newtoken % add_count % dstr %tstr;
			SendLogMessage(fmt.str());
		}
			break;
		case Packet::TokenType::Token_Exp:
		{
			auto actor = MainActor();
			if (actor == nullptr)
				return false;
			if (Config::ItemAddLogType::AddType_MonsterDrop != type)
			{
				std::stringstream DBlog;
				DBlog << "AddExp," << PlayerBibase() << "," << type << "," << source_id << "," << oldtoken << "," << add_count << "," << newtoken;
				Server::Instance().AddUrlTask(new db_log_require_work(DBlog.str(), __FILE__, __LINE__));

				//mqdblopg 包含模型ID的资源类型流水日志表
				std::string tstr = MtTimerManager::Instance().NowTimeString();
				std::string dstr = MtTimerManager::Instance().TimeString2DateString(tstr);
				boost::format fmt("INSERT INTO `resource_haveid_log` (`ServerID`,`PartnerID`,`UserID`,`PlayerID`,`VipLevel`,`ModuleID`,`SourceModelID`,`BeforeValue`,`AfterValue`,`EffectValue`,`CrDate`,`CrTime`) VALUES('%d','%d','%s','%llu','%d','%d','%d','%d','%d','%d','%s','%s');");
				fmt% MtConfig::Instance().server_group_id_%account_->partnerid() % account_->userid() % Guid() % GetAllCharge() % type % token % oldtoken % newtoken % add_count % dstr %tstr;
				SendLogMessage(fmt.str());
			}
			actor->AddExp(add_count, true);
		}	
		break;
		case Packet::TokenType::Token_Hero_Exp:
		{
			auto actors = BattleActor(formation);
			for (auto& a : actors) {
				if (a->ActorType() != Packet::ActorType::Main)
					a->AddExp(add_count, true);
			}
		}
		break;
		case Packet::TokenType::Token_FormationExp:
		{
			auto actors = BattleActor(formation);
			std::for_each(actors.begin(), actors.end(), boost::bind(&MtActor::AddExp, _1, add_count, true));
		}
		break;
		case Packet::TokenType::Token_GuildMoney:
		{
			auto msg = boost::make_shared<S2G::AddGuildMoney>();
			msg->set_addcount(add_count);
			msg->set_guildid(GetGuildID());
			Server::Instance().SendMessage(msg, shared_from_this());
		}
		break;
		case Packet::TokenType::Token_GuildContribute:
		{

			int32 weekcontribution = GetPlayerGuildProxy()->Getweekcontribution();
			int32 allcontribution = GetPlayerGuildProxy()->Getallcontribution();
			int32 curcontribution = GetPlayerGuildProxy()->Getcurcontribution();
			oldtoken = curcontribution;
			
			weekcontribution += add_count;
			allcontribution += add_count;
			curcontribution += add_count;
			GetPlayerGuildProxy()->Setweekcontribution(weekcontribution);
			GetPlayerGuildProxy()->Setallcontribution(allcontribution);
			GetPlayerGuildProxy()->Setcurcontribution(curcontribution);
			newtoken = curcontribution;

			std::stringstream DBlog;
			DBlog << "AddExp," << PlayerBibase() << "," << type << "," << source_id << "," << oldtoken << "," << add_count << "," << newtoken;
			Server::Instance().AddUrlTask(new db_log_require_work(DBlog.str(), __FILE__, __LINE__));

			//mqdblopg 包含模型ID的资源类型流水日志表
			std::string tstr = MtTimerManager::Instance().NowTimeString();
			std::string dstr = MtTimerManager::Instance().TimeString2DateString(tstr);
			boost::format fmt("INSERT INTO `resource_haveid_log` (`ServerID`,`PartnerID`,`UserID`,`PlayerID`,`VipLevel`,`ModuleID`,`SourceModelID`,`BeforeValue`,`AfterValue`,`EffectValue`,`CrDate`,`CrTime`) VALUES('%d','%d','%s','%llu','%d','%d','%d','%d','%d','%d','%s','%s');");			fmt% MtConfig::Instance().server_group_id_%account_->partnerid() % account_->userid() % Guid() % GetAllCharge() % type % token % oldtoken % newtoken % add_count % dstr %tstr;
			SendLogMessage(fmt.str());
		}
		break;
		case Packet::TokenType::Token_GuildBonus:
		{
			Server::Instance().SendS2GCommonMessage("S2GAddGuildBonus", { add_count }, { (int64)GetGuildID() }, {});
		}
		break;
		default:
			return false;
		};

		Packet::CommonReply reply;
		reply.set_reply_name(Packet::TokenType_Name(token));
		reply.add_int64_params(newtoken);
		SendMessage(reply);

		//活动奖励统计
		StatisticsResolve(token, add_count, type, source_id);

		return true;
	}

	int32 MtPlayer::GetPlayerToken(const Packet::TokenType token) const
	{
		auto token_data = player_db_->mutable_token_data();
		if (token_data == nullptr)
			return -1;
		switch (token)
		{
		case Packet::TokenType::Token_Gold:
			return token_data->gold();
			break;
		case Packet::TokenType::Token_Crystal:
			return token_data->crystal();
			break;
		case Packet::TokenType::Token_Honor:
			return token_data->honor();
			break;
		case Packet::TokenType::Token_Reward:
			return token_data->reward();
			break;
		case Packet::TokenType::Token_Day:
			return token_data->dayvalue();
			break;
		case Packet::TokenType::Token_BankGold:
			return token_data->bank_gold();
			break;
		case Packet::TokenType::Token_ExpPoint:
			return token_data->exp_point();
			break;
		case Packet::TokenType::Token_ColdExpPoint:
			return token_data->cold_exp_point();
			break;
		case Packet::TokenType::Token_Exp:
			{
			auto actor = MainActor();
			if (actor == nullptr)
				return -1;
			return actor->DbInfo()->exp();
			break;
			}
		default:
			return -1;
		};
	}

	std::vector<boost::shared_ptr<MtActor>> MtPlayer::OtherActors() const
	{
		std::vector<boost::shared_ptr<MtActor>> result;
		std::copy_if(std::begin(actors_), std::end(actors_), std::back_inserter(result), boost::bind(&MtActor::ActorType, _1) == Packet::ActorType::Hero);
		return result;
	}

	bool MtPlayer::ActorFull() const {
		return actors_.size() >= MtConfig::Instance().max_actor_count;
	}

	bool MtPlayer::AddActor(const boost::shared_ptr<MtActor>& actor,bool is_init)
	{
		if (ActorFull()) {
			return false;
		}
		actors_.push_back(actor);

		//
		actor->OnCache();

		if (!is_init) {
			actor->OnBattleInfoChanged();

			///
			OnLuaFunCall_x("xAddHero",
			{
				{ "value",actor->ConfigId() }
			});
		}
		//
		return true;
	}

	bool MtPlayer::LoadActor(const boost::shared_ptr<MtActor>& actor) 
	{
		if (ActorFull()) {
			return false;
		}
		actors_.push_back(actor);
		auto main_actor_it = std::find_if(actors_.begin(), actors_.end(),
			boost::bind(&MtActor::ActorType, _1) == Packet::ActorType::Main);
		if (main_actor_it != actors_.end()) {
			main_actor_ = *main_actor_it;
		}
		return true;
	}

	void MtPlayer::FillScenePlayerInfo(Packet::ScenePlayerInfo& msg) 
	{		
		msg.mutable_runtime_status()->CopyFrom(*runtime_status_);
		auto basic_info = msg.mutable_basic_info();
		basic_info->CopyFrom(*player_data_);
		basic_info->set_move_speed(GetMoveSpeed());

		for (auto& item : containers_[Packet::ContainerType::Layer_Equip]->Items()) {
			auto equip = boost::dynamic_pointer_cast<MtEquipItem>(item);
			if (equip != nullptr) {
				if (equip->ActorGuid() == Guid())
					equip->FillEquipInfo(*msg.add_main_equips());
			}
		}
	}

	void MtPlayer::SendScenePlayerInfoToServer()
	{
		boost::shared_ptr<Packet::ScenePlayerInfo> sceneinfo = boost::make_shared<Packet::ScenePlayerInfo>();
		if (sceneinfo != nullptr)
		{
			FillScenePlayerInfo(*sceneinfo.get());
			auto message = boost::make_shared<S2G::InsertScenePlayer>();
			message->mutable_msg()->CopyFrom(*sceneinfo);
			Server::Instance().SendMessage(message, shared_from_this());
		}
	}

	const boost::shared_ptr<MtZone>& MtPlayer::Zone()
	{
		return zone_;
	}

	void MtPlayer::Zone(const boost::shared_ptr<MtZone>& zone)
	{
		zone_ = zone;
	}

	bool MtPlayer::BroadCastMessage(const google::protobuf::Message& msg)
	{
		if (zone_) {
			return zone_->BroadcastMessageRelateZone(msg);
		}
		else if (raid_) {
			return raid_->BroadCastMessage(msg);
		}
		else {
			SendMessage(msg);
		}
		return false;
	}

	bool MtPlayer::BroadCastSceneMessage(const google::protobuf::Message& msg)
	{
		if (scene_) {
			scene_->BroadcastMessage(msg);
		}
		if (raid_) {
			raid_->BroadCastMessage(msg);
		}
		return true;
	}

	void MtPlayer::FillPlayerOperateInfo(Packet::PlayerOperateInfo &info)
	{
		auto spi = GetScenePlayerInfo();
		if (spi == nullptr)
			return;
		
		info.set_guid(Guid());
		info.set_name(spi->name());
		info.set_level(spi->level());
		info.set_hair(spi->hair());
		info.set_init_actor_id(spi->init_actor_id());
		info.set_teamid(GetTeamID());
		uint64 guildid = spi->guildid();
		info.set_guildid(guildid);
		info.set_guildname(spi->guildname());
		info.set_guildposition((Packet::GuildPosition)GetPlayerGuildProxy()->GetGuildPosition());
	}
	void MtPlayer::FillPlayerOperateInfoBySceneInfo(Packet::PlayerOperateInfo &info, uint64 playerid, const Packet::ScenePlayerInfo* spi, Packet::GuildPosition guildposition)
	{
		if (spi == nullptr)
			return;
		info.set_guid(playerid);
		info.set_name(spi->basic_info().name());
		info.set_level(spi->basic_info().level());
		info.set_hair(spi->basic_info().hair());
		info.set_init_actor_id(spi->basic_info().init_actor_id());
		info.set_teamid(INVALID_GUID);
		uint64 guildid = spi->basic_info().guildid();
		info.set_guildid(guildid);
		info.set_guildname(spi->basic_info().guildname());
		info.set_guildposition(guildposition);
	}
	//friend
	void MtPlayer::fill_friend_protocol(Packet::FriendInfotReply &reply)
	{
		friend_->fill_protocol(reply);
		auto mailmessage = reply.mutable_mailmessages();
		message_->fill_mail_protocol(*mailmessage);
		auto friendnotify = reply.mutable_notifyinfos();
		for (auto &notifyplayerinfo : message_->notify_playerinfo())
		{
			auto notify_ = friendnotify->add_notify();
			MtFriendManager::Fill_MessagePlayerInfo_MessageList(*notify_, notifyplayerinfo);
		}
		for (auto &offlinemsg  : message_->offline_messageinfo())
		{
			auto msg = reply.add_offlinemsgs();
			msg->CopyFrom(*offlinemsg.get());
		}
		
	}

	void MtPlayer::fill_recommend_protocol(Packet::RecommendsUpdate &reply)
	{
		friend_->ask_recommend_req(reply);
	}

	void MtPlayer::fill_friendplayerinfo_protocol(Packet::FriendInfo &info)
	{
		auto playerzoneinfo = MtFriendManager::Instance().GetPlayerZone(Guid());
		info.set_guid(Guid());
		info.set_hair(player_data_->hair());
		info.set_name(player_data_->name());
		info.set_level(player_data_->level());
		if (playerzoneinfo)
		{
			info.set_signature(playerzoneinfo->get_zone_signature());
		}
		else
		{
			info.set_signature("");
		}
		info.set_init_actor_id(player_data_->init_actor_id());
		info.set_guildname("mt");
		info.set_friendvalue(0);
		info.set_teamid(GetTeamID());
		if (Online())
		{
			info.set_online(true);
		}
		else
		{
			info.set_online(false);
		}
	}
	void MtPlayer::send_friendlistupdate()
	{
		Packet::FriendListUpdate update;
		friend_->fill_friend_protocol(update);
		SendMessage(update);
	}

	void MtPlayer::NewMessageNotify()
	{
		if (message_->get_message_size() > 0)
		{
			Packet::NewMessageNotify newmsg;
			SendMessage(newmsg);
		}
	}
	Packet::ResultCode MtPlayer::add_friend(uint64 playerguid, const Packet::PlayerBasicInfo* targetinfo)
	{
		if (targetinfo == nullptr)
			return Packet::ResultCode::UnknownError;
		
		Packet::ResultCode ret = friend_->apply_add_friend(50, playerguid, targetinfo);
		if (ret == Packet::ResultCode::FRIEND_SUCCESS)
		{
			send_friendlistupdate();
			

			//OnFriendMessageReq(Packet::FriendMessageType::FriendTips,targetinfo->guid(), notify.str(), -1);
			//这段代码的逻辑和OnFriendMessageReq 很像。但是因为发送内容不一样导致，接口没有弄好，下次重构 没时间啦
			{
				//发个消息给对方
				std::stringstream notify;
				notify << "friendaddnotify" << "|" << targetinfo->name();

				Packet::AddFriendMessagesReply selfadd;
				Packet::FriendMessageInfo* selfmsg = selfadd.mutable_messages();
				selfmsg->set_guid(INVALID_GUID);
				MtFriendManager::Fill_FriendMessage(*selfmsg, Packet::FriendMessageType::FriendTips, targetinfo->guid(), Guid(), notify.str(), -1);
				SendMessage(selfadd);


				std::stringstream targetmessage;
				targetmessage << "friendaddmessage_1|" << Guid() << "|" << Name() << "|" << Guid();

				Packet::AddFriendMessagesReply targetadd;
				Packet::FriendMessageInfo* targetmsg = targetadd.mutable_messages();
				targetmsg->set_guid(INVALID_GUID);
				MtFriendManager::Fill_FriendMessage(*targetmsg, Packet::FriendMessageType::SystemM, targetinfo->guid(), Guid(), targetmessage.str(), -1);

				//check内容
				auto p_target_player = Server::Instance().FindPlayer(targetinfo->guid());
				if (p_target_player != nullptr)
				{
					update_MessagePlayerInfo(p_target_player->GetScenePlayerInfo());
					p_target_player->update_MessagePlayerInfo(GetScenePlayerInfo());
					p_target_player->SendMessage(targetadd);
				}
				else
				{
					auto FriendMessageFunc = [=](boost::shared_ptr<Packet::ScenePlayerInfo>& spi)
					{
						if (spi)
						{
							update_MessagePlayerInfo(&spi->basic_info());
							boost::shared_ptr<Packet::FriendMessageInfo> dbmsg = boost::make_shared<Packet::FriendMessageInfo>();
							if (dbmsg)
							{
								dbmsg->CopyFrom(targetadd.messages());
								dbmsg->set_guid(MtGuid::Instance()(*dbmsg));
								MtFriendManager::Instance().AddFriendMessageInfo(dbmsg);

								auto dbinfo = MtFriendManager::Instance().CreateMessageListPlayerInfo(spi->basic_info().guid(), GetScenePlayerInfo());
								if (dbinfo)
								{
									MtFriendManager::Instance().AddMessageListPlayerInfo(dbinfo);
								}
							}
						}
					};
					auto targetsceneinfo = Server::Instance().FindPlayerSceneInfo(targetinfo->guid());
					if (targetsceneinfo)
					{
						FriendMessageFunc(targetsceneinfo);
					}
					else
					{
						Server::Instance().AddLoadScenePlayerInfo(targetinfo->guid(), FriendMessageFunc);
					}
				}
			}
		}
		return ret;
	}
	Packet::ResultCode MtPlayer::del_friend(uint64 playerguid)
	{
		Packet::ResultCode ret = friend_->del_friend_request(playerguid);
		if (ret == Packet::ResultCode::ResultOK)
		{
			send_friendlistupdate();
		}
		return ret;
	}

	void MtPlayer::set_zone_signature(std::string signature)
	{
		auto zone = MtFriendManager::Instance().GetPlayerZone(Guid());
		if (zone)
		{
			if (!MtWordManager::Instance()->NoSensitiveWord(signature))
			{
				SendClientNotify("neirongfeifa", -1, -1);
				return;
			}
			zone->set_zone_signature(signature);
			Packet::SignatureUpdate update;
			update.set_signature(zone->get_zone_signature());
			SendMessage(update);
		}
	}
	
	void MtPlayer::fill_ViewZoneInfoEquip(Packet::ViewZoneReply& msg) 
	{
		for (auto& item : containers_[Packet::ContainerType::Layer_Equip]->Items()) {
			auto equip = boost::dynamic_pointer_cast<MtEquipItem>(item);
			if (equip->ActorGuid() == Guid())
				equip->FillEquipInfo(*msg.add_main_equips());
		}
	}
	bool MtPlayer::check_MessagePlayerInfo(boost::shared_ptr<Packet::MessagePlayerInfo> info)
	{
		auto zone = MtFriendManager::Instance().GetPlayerZone(Guid());
		if (zone == nullptr)
		{
			return false;
		}
		bool online = (info->online() > 0) ? true : false;
		if (online != Online() || info->source_name() != player_data_->name() ||
			info->source_hair() != player_data_->hair() || info->signature() != zone->get_zone_signature() )
		{
			return true;
		}
		return false;
	}

	void MtPlayer::update_MessagePlayerInfo(const Packet::PlayerBasicInfo* targetinfo)
	{
		if (targetinfo)
		{
			boost::shared_ptr<Packet::MessageListPlayerInfo> info = message_->get_notify_playerinfo(targetinfo->guid());
			if (info == nullptr)
			{
				info = MtFriendManager::Instance().CreateMessageListPlayerInfo(Guid(), targetinfo);
				if (info)
				{
					message_->add_notify_playerinfo(info);

					Packet::AddClientFriendMessageNotify add;
					auto notify_ = add.mutable_notify();
					MtFriendManager::Fill_MessagePlayerInfo_MessageList(*notify_, info);
					SendMessage(add);
				}
			}
		}
	}

	void MtPlayer::OnSendGiftGiveReq(const boost::shared_ptr<MtPlayer>& sourceplayer, const boost::shared_ptr<Packet::SendGiftReq>& message)
	{
		if (sourceplayer == nullptr)
		{
			ZXERO_ASSERT(false);
			return;
		}
		if ( !BagLeftCapacity(Packet::ContainerType::Layer_ItemBag, message->items_size())) {
			SendClientNotify("ItemBagIsFull", -1, -1);
			return;
		}
		std::stringstream notify;
		notify << "zengsongliwuxiaoxi|" << sourceplayer->Name() << "|";
		for (auto item_ : message->items())
		{
			int32 item_id = item_.itemid();
			int32 item_count = item_.itemcount();
			notify << "[ITEM_" << item_id << "]";
			AddItemByIdWithNotify(item_id, item_count, Config::ItemAddLogType::AddType_Friend, sourceplayer->Guid());
		}
		//系统消息
		send_friendsystemmessage(notify.str());
	}

	void MtPlayer::OnFriendMessageReq(Packet::FriendMessageType type, uint64 targetguid, std::string chatmessage, int32 chattime)
	{
		Packet::AddFriendMessagesReply add;
		Packet::FriendMessageInfo* msg = add.mutable_messages();
		msg->set_guid(INVALID_GUID);
		MtFriendManager::Fill_FriendMessage(*msg, type, targetguid, Guid(), chatmessage, chattime);
		SendMessage(add);
		//check内容
		auto p_target_player = Server::Instance().FindPlayer(targetguid);
		if (p_target_player != nullptr)
		{
			update_MessagePlayerInfo(p_target_player->GetScenePlayerInfo());
			p_target_player->update_MessagePlayerInfo(GetScenePlayerInfo());
			if (p_target_player->Online() == true)
			{
				p_target_player->SendMessage(add);
			}
			else
			{
				boost::shared_ptr<Packet::FriendMessageInfo> dbmsg = boost::make_shared<Packet::FriendMessageInfo>();
				if (dbmsg)
				{
					dbmsg->CopyFrom(add.messages());
					dbmsg->set_guid(MtGuid::Instance()(*dbmsg));
					p_target_player->GetMessageContainer()->add_offlinemessage(dbmsg);
				}
			}
			
		}
		else
		{
			auto FriendMessageFunc = [=](boost::shared_ptr<Packet::ScenePlayerInfo>& spi)
			{
				if (spi)
				{
					update_MessagePlayerInfo(&spi->basic_info());
					boost::shared_ptr<Packet::FriendMessageInfo> dbmsg = boost::make_shared<Packet::FriendMessageInfo>();
					if (dbmsg)
					{
						dbmsg->CopyFrom(add.messages());
						dbmsg->set_guid(MtGuid::Instance()(*dbmsg));
						MtFriendManager::Instance().AddFriendMessageInfo(dbmsg);

						auto dbinfo = MtFriendManager::Instance().CreateMessageListPlayerInfo(spi->basic_info().guid(), GetScenePlayerInfo());
						if (dbinfo)
						{
							MtFriendManager::Instance().AddMessageListPlayerInfo(dbinfo);
						}
					}
				}
			};
			auto targetsceneinfo = Server::Instance().FindPlayerSceneInfo(targetguid);
			if (targetsceneinfo)
			{
				FriendMessageFunc(targetsceneinfo);
			}
			else
			{
				Server::Instance().AddLoadScenePlayerInfo(targetguid, FriendMessageFunc);
			}
		}
		
	}

	void MtPlayer::send_friendsystemmessage(std::string chatmessage)
	{
		OnFriendMessageReq(Packet::FriendMessageType::SystemM, INVALID_GUID, chatmessage, -1);
	}
	void MtPlayer::add_message(boost::shared_ptr<Packet::MessageInfo> mail_, MAIL_OPT_TYPE opt_type)
	{
		if (mail_ == nullptr )
		{
			ZXERO_ASSERT(false) << "add_message mail null ";
			return;
		}
		//std::lock_guard<std::recursive_mutex> gurad(mail_mutex_);
		message_->add_mail(mail_, opt_type);
		Packet::MessagesUpdate update;
		update.add_messages()->CopyFrom(*mail_);
		SendMessage(update);
	}
	Packet::ResultCode MtPlayer::del_message(uint64 messageid)
	{
		auto message = message_->get_mail(Guid(),messageid);
		if (message == nullptr)
		{
			return Packet::ResultCode::MESSAGE_NOT_EXIT;
		}

		std::map<int32, int32> temp;
		for (auto item : message->items()){
			if (item.itemid() > 0 && item.itemcount() > 0){
				temp.insert({ item.itemid(), item.itemcount() });
			}
		}
		//背包空间是否充足
		if (!EquipAndItemBagLeftCheck(temp)) {
			return  Packet::ResultCode::InternalResult;
		}

		//我先删掉。。哼
		message_->delete_mail(messageid, MAIL_OPT_TYPE::GET_DEL);
		for (auto item : message->items())
		{
			int32 configindex = item.itemid();
			int32 itemcount = item.itemcount();
			if (configindex > 0)
			{
				AddItemByIdWithNotify(configindex, itemcount, Config::ItemAddLogType::AddType_Mail, messageid);
			}
		}
		return Packet::ResultCode::ResultOK;
	}

	void  MtPlayer::del_notify_playerinfo(uint64 guid)
	{
		message_->del_notify_playerinfo(guid);
		Packet::DelClientFriendMessageNotifyRR rr;
		rr.set_source_guid(guid);
		SendMessage(rr);
	}
	void MtPlayer::AddActorFormation(boost::shared_ptr<Packet::ActorFormation>& formation)
	{
		for (auto child : actor_formations_){
			if (child->type() == formation->type()) {
				child->CopyFrom(*formation);
				return;
			}
		}
		actor_formations_.push_back(formation);
	}

	bool MtPlayer::FixActorFormations()
	{
		for (auto type = Packet::ActorFigthFormation_MIN + 1; type <= Packet::ActorFigthFormation_MAX; ++type) {
			if (std::find_if(std::begin(actor_formations_), std::end(actor_formations_), [&](auto& form) {return form->type() == type; }) == std::end(actor_formations_)) {
				auto tmp_form = boost::make_shared<Packet::ActorFormation>();
				actor_formations_.push_back(tmp_form);
				tmp_form->set_player_guid(Guid());
				tmp_form->set_type(Packet::ActorFigthFormation(type));

				if (MainActor() == nullptr){
					return false;
				}
				tmp_form->add_actor_guids(MainActor()->Guid());
				tmp_form->add_actor_guids(INVALID_GUID);
				tmp_form->add_actor_guids(INVALID_GUID);
				tmp_form->add_actor_guids(INVALID_GUID);
				tmp_form->add_actor_guids(INVALID_GUID);
			}
		}
		return true;
	}
	void MtPlayer::CopyActorFormation(const Packet::ActorFigthFormation source_type, const Packet::ActorFigthFormation target_type)
	{
		if (Scene() == nullptr)
			return;
		auto sformation = GetActorFormation(source_type);
		if (sformation == nullptr)
			return;
		boost::shared_ptr<Packet::SetActorFightReq> req = boost::make_shared<Packet::SetActorFightReq>();
		req->set_type(target_type);
		for (auto actorid : sformation->actor_guids())
		{
			if (actorid != INVALID_GUID)
			{
				req->add_guids(actorid);
			}
		}
		DispatchMessage(req);
	}
	boost::shared_ptr<Packet::ActorFormation> MtPlayer::GetActorFormation(const Packet::ActorFigthFormation type)
	{
		for (auto child : actor_formations_) {
			if (child->type() == type) {
				return child;
			}
		}
		return nullptr;
	}

	int32 MtPlayer::GetActorFormationPower(const Packet::ActorFigthFormation type)
	{
		int32 power = 0;
		for (auto child : actor_formations_) {
			if (child->type() == type) {
				for(int32 i=0; i<child->actor_guids_size(); i++)
				{
					auto guid = child->actor_guids(i);
					auto actor = FindActor(guid);
					if (actor != nullptr) {
						power += actor->Score();
					}
				}
			}
		}

		return power;
	}

	const std::vector<boost::shared_ptr<Packet::ActorFormation>>& MtPlayer::ActorFormation() const
	{
		return actor_formations_;
	}

	void MtPlayer::UpdateFormationLastHpSet(int32 formation, int32 lasthp) {
		auto form = GetActorFormation((Packet::ActorFigthFormation)formation);
		if (form == nullptr) {
			return;
		}

		for (int32 i = 0; i < form->actor_guids_size(); i++) {
			auto actor = FindActor(form->actor_guids(i));
			if( actor == nullptr )
				continue;

			if (actor->GetLastHp((Packet::LastHpType)lasthp) < 0) {
				form->set_actor_guids(i, INVALID_GUID);
			}
		}
		SendMessage(*form);
	}

	bool MtPlayer::IsEmptyFormation(Packet::ActorFigthFormation type) {
		auto form = GetActorFormation(type);
		if (form == nullptr) {
			return true;
		}

		for (int32 i = 0; i < form->actor_guids_size(); i++) {
			if (form->actor_guids(i) != INVALID_GUID) {
				return false;
			}
		}

		return true;

	}

	void MtPlayer::DelNpc( const int32 series_id )
	{
		auto iter = npc_map_.find(series_id);
		if (iter != npc_map_.end())
		{
			Packet::NpcList reply;
			reply.set_option(Packet::NpcOption::NpcDel);
			auto npc_info = reply.add_npc_info();
			npc_info->CopyFrom(*(iter->second->NpcInfo()));
			npc_map_.erase(iter);

			SendMessage(reply);

			LOG_INFO << "del player npc ok ! player guid" << Guid()  << " scene_id" << GetSceneId() << " series_id=" << series_id;
		}
		else{
			if (scene_ != nullptr) {
				scene_->DelNpc(series_id);
			}
		}
	}

	void MtPlayer::LockNpc(const int32 series_id, const bool lock)
	{
		auto iter = npc_map_.find(series_id);
		if (iter != npc_map_.end())
		{
			iter->second->Lock(lock);
		}
		else {
			if (scene_ != nullptr) {
				scene_->LockNpc(series_id, lock);
			}
		}
	}

	void MtPlayer::DelNpcByType(const std::string src_type)
	{
		if (npc_map_.empty()) {
			return;
		}
		Packet::NpcList reply;
		reply.set_option(Packet::NpcOption::NpcDel);

		auto iter = npc_map_.begin();
		for (; iter != npc_map_.end(); ) {
			if (iter->second->NpcDynamicConfig()->npc_tag() == src_type) {

				auto npc_info = reply.add_npc_info();
				npc_info->CopyFrom(*(iter->second->NpcInfo()));
				iter = npc_map_.erase(iter);
			}
			else{
				++iter;
			}
		}

		SendMessage(reply);

		LOG_INFO << "del player npc ok ! player guid" << Guid() << " src_type=" << src_type;
	}

	void MtPlayer::DelNpcByGroup(const int32 group)
	{
		if (npc_map_.empty()) {
			return;
		}

		if (group < 0)
			return;

		Packet::NpcList reply;
		reply.set_option(Packet::NpcOption::NpcDel);

		auto iter = npc_map_.begin();
		for (; iter != npc_map_.end(); ) {
			if (iter->second->NpcDynamicConfig()->group_id() == group) {
				auto npc_info = reply.add_npc_info();
				npc_info->CopyFrom(*(iter->second->NpcInfo()));
				iter = npc_map_.erase(iter);
			}
			else {
				++iter;
			}
		}

		SendMessage(reply);

		LOG_INFO << "del player npc ok ! player guid" << Guid() << " group=" << group;
	}


	void MtPlayer::SetPlayerNpc(const boost::shared_ptr<DB::PlayerTrialInfo> trialinfo)
	{
		if (trialinfo == nullptr) {
			ZXERO_ASSERT(false) << "invalid trialinfo! guid =" << Guid();
			return;
		}
		if (trialinfo->target().targets_size() <= 2) {
			ZXERO_ASSERT(false) << "invalid trialinfo! guid =" << Guid();
			return;
		}
		cur_trialinfo_ = trialinfo;
		datacell_container_->add_data_32(Packet::CellLogicType::NumberCell, Packet::NumberCellIndex::HellDayWaveCount, 1);

		if (raid_ != nullptr) {			
			ResetPlayerNpcLastHp();//满血
			RefreshPlayerNpc( false );
		}

		//日志，查问题
		for (int i = 0; i < trialinfo->target().targets_size(); i++) {
			auto form = trialinfo->mutable_target();
			auto target = form->mutable_targets(i);
			LOG_INFO << "SetPlayerNpc ! target guid" << target->mutable_player()->guid() << " passed=" << target->passed() << " reward=" << target->rewarded() << " player guid=" << Guid();
		}
		
	}

	void MtPlayer::ResetPlayerNpcLastHp()
	{
		if (cur_trialinfo_ == nullptr)
			return;

		auto target_form = cur_trialinfo_->mutable_target();
		for (int i = 0; i < target_form->targets_size(); i++) {
			auto tplayer = target_form->mutable_targets(i);
			for (int k = 0; k < tplayer->actors_size(); k++) {
				auto tactor = tplayer->mutable_actors(k);
				auto dbinfo = tactor->mutable_actor();
				auto battle_info = dbinfo->mutable_battle_info();

				dbinfo->clear_last_hp();
				for (int32 j = 0; j < Packet::LastHpType::LastHpMax; j++) {
					dbinfo->add_last_hp(battle_info->hp());
				}
			}
		}
	}

	void MtPlayer::ResetPlayerActorLastHp(Packet::LastHpType type)
	{
		for (auto child : actors_) {
			child->SetLastHp(type,child->MaxHp());
		}
		//
		Refresh2ClientActorList();
	}

	void MtPlayer::RefreshPlayerNpc(bool dirty)
	{
		if (cur_trialinfo_ == nullptr) {
			return;
		}

		Packet::PlayerNpcList reply;
		auto form = cur_trialinfo_->mutable_target();
		int32 count = datacell_container_->get_data_32(Packet::CellLogicType::NumberCell, Packet::NumberCellIndex::HellDayWaveCount);
		for (int i = 0; i < form->targets_size(); i++) {
			auto target = form->mutable_targets(i);
			if (target->passed() && target->rewarded()) {
				continue;
			}
			auto npc = reply.add_player_npc();
			auto target_player = npc->mutable_target();
			target_player->CopyFrom(*target);
			SetParams64("target_guid", int64(target_player->mutable_player()->guid()));
			if (dirty) {
				reply.set_option(Packet::NpcOption::NpcUpdate);
			}
			else {
				reply.set_option(Packet::NpcOption::NpcAdd);
				if (i == 2) {//第3个是大宝箱		
					if (count*(i+1) == 15) {//终极宝箱
						AddNpc(1140100003, 19996, -1);
					}
					else {//大宝箱
						AddNpc(1140100003, 19999, -1);
					}
				}
				else {
					AddNpc(1140100003, 19998,-1);
				}
			}
			SendMessage(reply);

			LOG_INFO << "[RefreshPlayerNpc] target guid:" << target_player->mutable_player()->guid() << " dirty="<<dirty<<" guid="<<Guid();

			break;
			
		}

	}

	void MtPlayer::DelPlayerNpc(const uint64 target_guid)
	{
		if (cur_trialinfo_ == nullptr)
			return;

		Packet::PlayerNpcList reply;
		reply.set_option(Packet::NpcOption::NpcDel);
		auto target_form = cur_trialinfo_->mutable_target();
		for (int i = 0; i < target_form->targets_size(); i++) {
			auto tplayer = target_form->mutable_targets(i);
			if (tplayer->player().guid() == target_guid) {
				auto npc = reply.add_player_npc();
				auto target = npc->mutable_target();
				target->CopyFrom(*tplayer);

				LOG_INFO << "[DelPlayerNpc] target guid:" << target_guid << " guid=" << Guid();

				break;
			}
		}

		SendMessage(reply);
	}

	void MtPlayer::DelPlayerNpcByIndex(const int32 index)
	{
		if (cur_trialinfo_ == nullptr)
			return;

		Packet::PlayerNpcList reply;
		reply.set_option(Packet::NpcOption::NpcDel);
		auto target_form = cur_trialinfo_->mutable_target();
		auto tplayer = target_form->mutable_targets(index);
		if (tplayer != nullptr) {
			auto npc = reply.add_player_npc();
			auto target = npc->mutable_target();
			target->CopyFrom(*tplayer);
			target->mutable_player()->set_guid(index);
		}

		LOG_INFO << "[DelPlayerNpcByIndex] target index:" << index << " guid=" << Guid();

		SendMessage(reply);
	}

	DB::TrialTarget* MtPlayer::GetPlayerNpc(const uint64 target_guid)
	{
		if (cur_trialinfo_ == nullptr)
			return nullptr;

		auto target_form = cur_trialinfo_->mutable_target();
		for (int i = 0; i < target_form->targets_size(); i++) {
			auto tplayer = target_form->mutable_targets(i);
			if (tplayer->player().guid() == target_guid) {
				return tplayer;
			}
		}
		return nullptr;
	}

	DB::TrialTarget* MtPlayer::GetPlayerNpcByIndex(const int32 index)
	{
		if (cur_trialinfo_ == nullptr)
			return nullptr;

		auto target_form = cur_trialinfo_->mutable_target();
		for (int i = 0; i < target_form->targets_size(); i++) {
			if (i == index) {
				return target_form->mutable_targets(i);
			}
		}
		return nullptr;
	}

	bool MtPlayer::CheckRefreshNextWave()
	{
		if (cur_trialinfo_ == nullptr)//初始必需true
			return true;

		int32 count = datacell_container_->get_data_32(Packet::CellLogicType::NumberCell, Packet::NumberCellIndex::HellDayWaveCount);
		int32 done_times = OnLuaFunCall_1(501, "GetActivityTimes", 5);
		if (count >= 5 && done_times>=15) {
			return false;
		}

		for (int i = 0; i < cur_trialinfo_->target().targets_size(); i++) {
			auto target = cur_trialinfo_->target().targets(i);
			if ( !target.passed()) {
				return false;
			}

			if ( !target.rewarded()) {
				return false;
			}
		}
		
		return true;
	}

	bool MtPlayer::CanGo()
	{
		if (GetCurBattleType() != Packet::BattleGroundType::NO_BATTLE
			&& GetCurBattleType() != Packet::BattleGroundType::PVE_HOOK 
			&& GetCurBattleStatus() != Packet::BattleGroundStatus::END)
		{
			SendClientNotify("GoTo_InBattleOpt", -1, -1);
			return false;
		}
		if (player_data_->player_status() == Packet::PlayerSceneStatus::READY_TO_ENTER) {
			//做个保护，防止多个请求导致切换场景逻辑错误！
			SendClientNotify("GoTo_InBattleOpt", -1, -1);
			return false;
		}
		return true;
	}

	bool MtPlayer::GoTo(int32 sceneid, std::vector<int32> pos)
	{
		if (!CanGo()) {
			return false;
		}

		StopMove();

		if (scene_ != nullptr && scene_->SceneId() == sceneid) {
			Packet::Position init_pos;
			if (pos.size() == 3 ) {
				init_pos.set_x(pos[0]);
				init_pos.set_y(pos[1]);
				init_pos.set_z(pos[2]);
			}
			scene_->Transport(shared_from_this(), init_pos);
		} else {

			if (GetChangeSceneCD() > 0) {
				SendClientNotify("ChangeSceneCD", -1, -1);
				return false;
			}

			auto enter_msg = boost::make_shared<Packet::PlayerEnterScene>();
			enter_msg->set_scene_id(sceneid);
			enter_msg->set_pre_scene_id(GetSceneId());

			if (pos.size() == 3 && pos[0] >= 0 && pos[1] >= 0 && pos[1] >= 0) {
				auto init_pos = enter_msg->mutable_init_pos();
				init_pos->set_x(pos[0]);
				init_pos->set_y(pos[1]);
				init_pos->set_z(pos[2]);
			}
			player_data_->set_player_status(Packet::PlayerSceneStatus::CHANGE_SCENE);
			DispatchMessage(enter_msg);


		}
		return true;
	}

	void MtPlayer::GoRaidByPosIndex(int32 sceneid, int32 script_id, int32 raidid, int32 camp, int32 posindex)
	{
		auto sceneconfig = MtSceneManager::Instance().GetSceneConfig(sceneid);
		if (sceneconfig == nullptr)
			return;
		auto init_pos = sceneconfig->spawn_pos(posindex);
		std::vector<int32> pos = { init_pos.x(),init_pos.y(),init_pos.z() };
		GoRaid(sceneid,script_id,raidid,camp, pos);
	}

	bool MtPlayer::GoRaid(int32 sceneid, int32 script_id, int32 raidid, int32 camp, std::vector<int32> pos)
	{
		if (GetRaid() != nullptr)
		{
			if (GetRaid()->RuntimeId() != raidid)
			{
				SendClientNotify("nofubentofuben", -1, -1);
				return false;
			}
		}
		if (OnRaidNpc2Scene(sceneid) == nullptr) {
			SendClientNotify("InvalidDestScene", -1, -1);
			return false;
		}

		StopMove();
		if (player_data_->player_status() == Packet::PlayerSceneStatus::READY_TO_ENTER) {
			//做个保护，防止多个请求导致切换场景逻辑错误！
			return false;
		}		
		
		if (scene_ != nullptr && scene_->SceneId() == sceneid) {
			Packet::Position init_pos;
			if (pos.size() == 3) {
				init_pos.set_x(pos[0]);
				init_pos.set_y(pos[1]);
				init_pos.set_z(pos[2]);
			}
			scene_->Transport(shared_from_this(), init_pos);
		} else {
			auto enter_msg = boost::make_shared<Packet::PlayerEnterScene>();
			enter_msg->set_scene_id(sceneid);
			enter_msg->set_script_id(script_id);
			enter_msg->set_rtid(raidid);
			enter_msg->set_pre_scene_id(GetSceneId());
			enter_msg->set_camp(camp);
			auto portal_pos = enter_msg->mutable_portal_pos();
			portal_pos->CopyFrom(Position());
			if (pos.size() == 3 && pos[0] >= 0 && pos[1] >= 0 && pos[1] >= 0) {
				auto init_pos = enter_msg->mutable_init_pos();
				init_pos->set_x(pos[0]);
				init_pos->set_y(pos[1]);
				init_pos->set_z(pos[2]);
			}
			player_data_->set_player_status(Packet::PlayerSceneStatus::CHANGE_SCENE);
			DispatchMessage(enter_msg);
		}
		return true;
	}

	/*void MtPlayer::ChangeScene(const boost::shared_ptr<Packet::PlayerEnterScene>& enter_msg)
	{
		if (scene_ == nullptr) {//第一次进场景是空的
			auto new_scene = MtSceneManager::Instance().GetSceneById(enter_msg->scene_id());
			if (new_scene == nullptr) {
				ZXERO_ASSERT(false) << "raid scene is null! scene id=" << enter_msg->scene_id();
				return;
			}
			player_data_->set_player_status(Packet::PlayerSceneStatus::ENTERING);
			new_scene->OnClientMessage(shared_from_this(), enter_msg, now());
			return;
		}

		//不是同一个场景需要先切换场景
		auto new_scene = MtSceneManager::Instance().GetSceneById(enter_msg->scene_id());
		if (new_scene == nullptr) {
			ZXERO_ASSERT(false) << "raid scene is null! scene id=" << enter_msg->scene_id();
			return;
		}
		player_data_->set_player_status(Packet::PlayerSceneStatus::ENTERING);

		scene_->OnPlayerLeave(shared_from_this());
		new_scene->OnClientMessage(shared_from_this(), enter_msg, now());
	}*/

	int32 MtPlayer::AddNpc(const int32 pos_id, const int32 index, const int32 lifetime)
	{
		auto npc = MtMonsterManager::Instance().CreateNpc(pos_id, index, -1, lifetime);
		if (!npc) {
			ZXERO_ASSERT(false) << "can not find monster data ! index=" << index << " player guid" << Guid() << " pos_id" << pos_id;
			return -1;
		}

		auto info = npc->NpcInfo();
		int32 series_id = info->series_id();
		auto iter = npc_map_.find(series_id);
		if (iter != npc_map_.end())
			return -1;

		auto npc_data = MtMonsterManager::Instance().FindDynamicNpc(index);
		if (!npc_data) {
			ZXERO_ASSERT(false) << "FindDynamicNpc failed ! index=" << index << " player guid" << Guid() << " pos_id" << pos_id << "series_id " << series_id;
			return -1;
		}

		int32 npc_id = npc_data->npc_id();
		auto monster_config = MtMonsterManager::Instance().FindMonster(npc_id);
		if (!monster_config) {
			ZXERO_ASSERT(false) << "FindMonster failed ! npc_id=" << npc_id << " player guid" << Guid() << " pos_id" << pos_id << "series_id " << series_id;
			return -1;
		}
		auto pos_config = MtMonsterManager::Instance().FindScenePos(pos_id);
		if (!pos_config) {
			ZXERO_ASSERT(false) << "FindScenePos failed ! npc_id=" << npc_id << " player guid" << Guid() << " pos_id" << pos_id << "series_id " << series_id;
			return -1;
		}

		npc_map_.insert({ series_id, npc });

		Packet::NpcList reply;
		reply.set_option(Packet::NpcOption::NpcAdd);
		auto npc_info = reply.add_npc_info();
		npc_info->CopyFrom(*info);
		SendMessage(reply);


		LOG_INFO << "add player npc ok ! index=" << index << " player guid" << Guid() << " pos_id" << pos_id << " scene_id" << GetSceneId() << " series_id=" << series_id;

		return series_id;
	}

	void MtPlayer::AddOtherNpc(const int32 group, const int32 lifetime)
	{
		auto npcs = MtMonsterManager::Instance().FindNpcGroup(group);
		if (npcs != nullptr) {
			for (auto child : *npcs) {
				AddNpc(child->pos_id(), child->index(), lifetime);
			}
		}
	}

	//跨线程添加场景npc接口
	bool MtPlayer::AddSceneNpc(const int32 pos_id, const int32 index, const int32 lifetime)
	{
		auto layout = MtMonsterManager::Instance().FindScenePos(pos_id);
		if (layout == nullptr)
			return false;

		auto target_scene = MtSceneManager::Instance().GetSceneById(layout->scene_id());
		if (target_scene == nullptr) {
			return false;
		}

		target_scene->QueueInLoop(boost::bind(&MtScene::AddNpc, target_scene, pos_id, index, lifetime));

		return true;
	}

	Packet::ResultCode MtPlayer::SendClientNotify(const std::string & content, const int32 channel, const int32 show_type)
	{
		if (content != "") {
			switch (channel)
			{
			case Packet::ChatChannel::TEAM:
				send_team_system_message(content, GetTeamID());
				break;
			case Packet::ChatChannel::GUILD:
				send_guild_system_message(content, GetGuildID());
				break;
			case Packet::ChatChannel::SYSTEM:
				send_system_message_toplayer(content, shared_from_this());
				break;
			default:
				if (channel == -999) {
					LOG_ERROR << content << " player guid=" << Guid();
				}
				Packet::Notify notify;
				notify.set_notify(content);
				notify.set_show_type(show_type);
				SendMessage(notify);
				break;
			}
		}

		return Packet::ResultCode::InternalResult;
	}
	Packet::ResultCode MtPlayer::CanChat(Packet::ChatChannel channel)
	{
		if (channel < Packet::ChatChannel::SYSTEM || channel >= Packet::ChatChannel::MAX)
			return Packet::ResultCode::UnknownError;
		if (chatcd_[channel] > 0)
		{
			return Packet::ResultCode::Chat_CD;
		}
		if (channel == Packet::ChatChannel::WORLD)
		{
			if (PlayerLevel() < MtConfig::Instance().world_chat_level_)
			{
				return Packet::ResultCode::Chat_WordLevel ;
			}
		}
		else if(channel == Packet::ChatChannel::FARMSG)
		{
			if (GetPlayerToken(Packet::TokenType::Token_Crystal)<50)
			{
				return Packet::ResultCode::CrystalNotEnough;
			}
		}
		return Packet::ResultCode::ResultOK;
	}
	void MtPlayer::SetChatChannelCD(Packet::ChatChannel channel, int32 cdtime)
	{
		if (channel < Packet::ChatChannel::SYSTEM || channel >= Packet::ChatChannel::MAX)
			return ;
		chatcd_[channel] = cdtime;
	}
	void MtPlayer::SendCommonResult(Packet::ResultOption
		option, Packet::ResultCode code)
	{
		//内部就不需要了
		if (code == Packet::ResultCode::InternalResult) {
			return;
		}
		Packet::CommonResult result;
		result.set_option(option);
		result.set_code(code);
		 
		SendMessage(result);
	}
	//校验位置id
	bool MtPlayer::ValidPositionById(const int32 posid)
	{
		if (scene_ == nullptr) {
			return false;
		}
		auto config = MtMonsterManager::Instance().FindScenePos(posid);
		if (config == nullptr) {
			return false;
		}
		if (scene_->SceneId() != config->scene_id()) {
			return false;
		}

		auto cur_pos = player_data_->scene_pos();			
		auto new_distance = zxero::distance2d(cur_pos, config->position());
		if (new_distance > 2500){
			return false;
		}

		return true;
	}

	bool MtPlayer::ValidPosition(const Packet::Position& pos,const int32 dist )
	{
		auto cur_pos = player_data_->scene_pos();
		auto new_distance = zxero::distance2d(cur_pos, pos);
		if (new_distance > dist) {
			return false;
		}

		return true;
	}

	std::vector<boost::shared_ptr<MtNpc>> MtPlayer::GetSceneNpcs( const int32 sceneid, const std::string &tag)
	{
		std::vector<boost::shared_ptr<MtNpc>> npcs;
		for (auto child : npc_map_)
		{
			if (child.second->PositionConfig()->scene_id() == sceneid){
				if (tag == "") {
					npcs.push_back(child.second);
				}
				else if( child.second->NpcDynamicConfig()->npc_tag() == tag) {
					npcs.push_back(child.second);
				}				
			}
		}
		return npcs;
	}

	std::vector<boost::shared_ptr<MtNpc>> MtPlayer::GetTagNpcs(const std::string &tag)
	{
		std::vector<boost::shared_ptr<MtNpc>> npcs;
		for (auto child : npc_map_)
		{
			if (child.second->NpcDynamicConfig()->npc_tag() == tag) {
				npcs.push_back(child.second);
			}
		}
		return npcs;
	}

	void MtPlayer::Refresh2ClientNpcList()
	{
		Packet::NpcList msg;
		msg.set_option(Packet::NpcOption::NpcUpdate);

		if (scene_ != nullptr) {
			auto scene_npcs = scene_->GetNpcList();
			for (auto child : scene_npcs)
			{
				auto obj = msg.add_npc_info();
				obj->CopyFrom(*(child.second->NpcInfo()));
			}
		}
		
		//auto npcs = GetSceneNpcs(SceneId(), "");
		for (auto child : npc_map_)
		{
			auto obj = msg.add_npc_info();
			obj->CopyFrom(*(child.second->NpcInfo()));
		}

		SendMessage(msg);
	}

	void MtPlayer::Refresh2ClientMissionList()
	{
		if (mission_proxy_ != nullptr) {

			Packet::MissionListReply msg;
			for (auto child : mission_proxy_->GetMissionMap()){
				auto mission = msg.add_mission_list();
				mission->CopyFrom(*(child.second->MissionDB()));
			}

			SendMessage(msg);
		}
	}

	void MtPlayer::Refresh2ClientLuaFiles(bool dirty)
	{
		//活动脚本更新
		Packet::LuaFilesReply reply;
		auto luas = luafile_manager::instance().get_luafiles(dirty);
		for (auto child : luas) {
			auto file = reply.add_files();
			file->CopyFrom(*child);
		}
		SendMessage(reply);
	}

	void MtPlayer::Refresh2ClientOperateConfig(int32 activity_id)
	{
		//活动脚本更新
		if (activity_id == -1) {
			Packet::OperativeConfigReply reply;
			auto configs = MtOperativeConfigManager::Instance().GetConfigs();
			for (auto child : configs) {
				auto file = reply.add_configs();
				file->CopyFrom(*(child.second));
			}
			SendMessage(reply);
		}
		else {
			auto config = MtOperativeConfigManager::Instance().FindConfig(activity_id);
			Packet::OperativeConfigReply reply;
			auto file = reply.add_configs();
			file->CopyFrom(*config);
			SendMessage(reply);
		}
	}
	void MtPlayer::Refresh2ClientDataCellList()
	{
		datacell_container_->DataCellListUpdate();
		datacell_container_->DataCell64ListUpdate();
	}

	void MtPlayer::Refresh2ClientActorList()
	{
		Packet::ActorListReply reply;
		for (auto& actor : actors_) {
			actor->SerializationToMessage(*reply.add_actors());
		}
		SendMessage(reply);
	}

	//////////////////////////////////////////////////
	void MtPlayer::Refresh2ClientImpactList()
	{
		Packet::SimpleImpactUpdate msg;
		simple_impacts_.foreach([&](auto ptr) {
			auto impact = msg.add_simple_impact();
			impact->CopyFrom(*ptr);
		});

		for (auto actor : actors_){
			actor->GetSimpleImpactSet()->foreach([&](auto ptr) {
				auto impact = msg.add_simple_impact();
				impact->CopyFrom(*ptr);
			});
		}
		SendMessage(msg);
	}
		

	void MtPlayer::Refresh2ClientBattleFeild()
	{
		//如果玩家当前在战场中，需要同步一下战场数据
		if (raid_ != nullptr && raid_->RuntimeId() == bf_proxy_.raid_rtid()) {
			auto room = MtBattleFieldManager::Instance().FindRoomById(bf_proxy_.raid_rtid());
			if (room != nullptr) {
				auto roomInfo = room->GetBattleRoomInfo();
				SendMessage(roomInfo);

				auto battleInfo = room->GetBattleFieldInfo();
				SendMessage(battleInfo);
			}
		}
	}

	void MtPlayer::Refresh2ClientActivityList()
	{
		Packet::ActivityListReply msg;

		for (auto child : activities_)
		{
			auto activity = msg.add_activity_list();
			activity->CopyFrom(*(child.second->ActivityDB()));
		}

		SendMessage(msg);
	}

	void MtPlayer::ActivityListRefresh()
	{
		for (auto child : activities_) {
			child.second->OnRefresh();
		}
	}

	void MtPlayer::ActivityListModify()
	{
		for (auto child : activities_) {
			auto config = MtActivityManager::Instance().FindActivity(child.second->ActivityDB()->config().id());
			if (config != nullptr) {
				auto ac_db = child.second->ActivityDB();
				if (ac_db != nullptr) {
					auto old_config = ac_db->mutable_config();
					old_config->CopyFrom(*config);
				}
			}

			child.second->OnRefresh();
		}
	}

	Packet::ResultCode MtPlayer::DrawCrystalOne(Packet::NotifyItemList &list) 
	{		
		int32 all_weight = 0;
		auto const_config = MtConfigDataManager::Instance().FindConfigValue("crystal_draw");
		if (const_config == nullptr) {
			return Packet::ResultCode::InvalidDrawConfigData;
		}

		std::vector<boost::shared_ptr<Config::DrawCardConfig>> temp_pool;
		MtDrawManager::Instance().GetDrawList(0, PlayerLevel(), temp_pool, all_weight);
		if (temp_pool.empty()) {
			return Packet::ResultCode::InvalidDrawConfigData;
		}
		if (all_weight == 0) {
			return Packet::ResultCode::InvalidDrawConfigData;
		}

		//优先扣代币
		if (!DelItemById(const_config->value1(), 1, Config::ItemDelLogType::DelType_Draw)) {
			return Packet::ResultCode::CostItemNotEnough;
		}

		if (!DrawCardInternal(temp_pool, all_weight, &list, const_config->value1())) {
			ZXERO_ASSERT(false) << "DrawCardInternal failed! guid" << Guid() << " draw type =" << Config::DrawType::CrystalOne;// << "cd time=" << cd;
		}

		//保底药水
		AddItemById(20121003, 1, Config::ItemAddLogType::AddType_Draw, nullptr, const_config->value1());

		return Packet::ResultCode::ResultOK;
	}
	Packet::ResultCode MtPlayer::DrawGoldOne(Packet::NotifyItemList &list)
	{
		int32 all_weight = 0;
		auto const_config = MtConfigDataManager::Instance().FindConfigValue("coin_draw");
		if (const_config == nullptr) {
			return Packet::ResultCode::InvalidDrawConfigData;
		}

		std::vector<boost::shared_ptr<Config::DrawCardConfig>> temp_pool;
		MtDrawManager::Instance().GetDrawList(3, PlayerLevel(), temp_pool,all_weight);
		if (temp_pool.empty()) {
			return Packet::ResultCode::InvalidDrawConfigData;
		}
		if (all_weight == 0) {
			return Packet::ResultCode::InvalidDrawConfigData;
		}

		//优先扣代币
		if (!DelItemById(const_config->value1(), 1, Config::ItemDelLogType::DelType_Draw)) {
			return Packet::ResultCode::CostItemNotEnough;
		}
		if (!DrawCardInternal(temp_pool, all_weight, &list, const_config->value1())) {
			ZXERO_ASSERT(false) << "DrawCardInternal failed! guid" << Guid() << " draw type =" << Config::DrawType::GoldOne;// << "cd time=" << cd << " free times=" << free_times;
		}
		
		//保底药水
		AddItemById(20121001, 1, Config::ItemAddLogType::AddType_Draw, nullptr, const_config->value1());

		return Packet::ResultCode::ResultOK;
	}
	Packet::ResultCode MtPlayer::DrawCrystalTen(Packet::NotifyItemList &list)
	{
		int32 all_weight = 0;
		auto const_config = MtConfigDataManager::Instance().FindConfigValue("crystal_draw");
		if (const_config == nullptr) {
			return Packet::ResultCode::InvalidDrawConfigData;
		}

		if (!DelItemById(const_config->value2(), 1, Config::ItemDelLogType::DelType_Draw, Guid())) {
			return Packet::ResultCode::CostItemNotEnough;
		}

		std::vector<boost::shared_ptr<Config::DrawCardConfig>> temp_pool;
		MtDrawManager::Instance().GetDrawList(0, PlayerLevel(), temp_pool, all_weight);
		if (temp_pool.empty()) {
			return Packet::ResultCode::InvalidDrawConfigData;
		}
		if (all_weight == 0) {
			return Packet::ResultCode::InvalidDrawConfigData;
		}

		//先记录luckyvalue
		datacell_container_->add_data_32(Packet::CellLogicType::NumberCell, Packet::NumberCellIndex::DrawLuckyValue,1);
		//
		for (int32 i = 0; i < 9; i++) {
			if (!DrawCardInternal(temp_pool, all_weight, &list, const_config->value2())) {
				ZXERO_ASSERT(false) << "DrawCardInternal failed! guid" << Guid() << " draw type =" << Config::DrawType::CrystalTen;
			}
		}
		temp_pool.clear();

		//首抽
		if (!datacell_container_->check_bit_data(Packet::BitFlagCellIndex::FirstCrystalDrawFlag)) {
			datacell_container_->set_bit_data(Packet::BitFlagCellIndex::FirstCrystalDrawFlag);
			MtDrawManager::Instance().GetDrawList(6, PlayerLevel(), temp_pool, all_weight);
		}
		else {
			int32 value = datacell_container_->get_data_32(Packet::CellLogicType::NumberCell, Packet::NumberCellIndex::DrawLuckyValue);
			if (value >= 3) {//大保底
				MtDrawManager::Instance().GetDrawList(6, PlayerLevel(), temp_pool, all_weight);
			}
			else {//小保底
				MtDrawManager::Instance().GetDrawList(1, PlayerLevel(), temp_pool, all_weight);
			}
		}
		
		if (temp_pool.empty()) {
			return Packet::ResultCode::InvalidDrawConfigData;
		}
		if (all_weight == 0) {
			return Packet::ResultCode::InvalidDrawConfigData;
		}
		if (!DrawCardInternal(temp_pool, all_weight, &list, const_config->value2())) {
			ZXERO_ASSERT(false) << "DrawCardInternal failed! guid" << Guid() << " draw type =" << Config::DrawType::CrystalTen;
		}

		//保底药水
		AddItemById(20121003, 10, Config::ItemAddLogType::AddType_Draw, nullptr, const_config->value2());

		return Packet::ResultCode::ResultOK;
	}
	Packet::ResultCode MtPlayer::DrawGoldTen(Packet::NotifyItemList &list)
	{
		int32 all_weight = 0;
		auto const_config = MtConfigDataManager::Instance().FindConfigValue("coin_draw");
		if (const_config == nullptr) {
			return Packet::ResultCode::InvalidDrawConfigData;
		}
		if (!DelItemById(const_config->value2(), 1, Config::ItemDelLogType::DelType_Draw)) {
			return Packet::ResultCode::CostItemNotEnough;
		}

		std::vector<boost::shared_ptr<Config::DrawCardConfig>> temp_pool;
		MtDrawManager::Instance().GetDrawList(3, PlayerLevel(), temp_pool,all_weight);
		if (temp_pool.empty()) {
			return Packet::ResultCode::InvalidDrawConfigData;
		}
		if (all_weight == 0) {
			return Packet::ResultCode::InvalidDrawConfigData;
		}
		for (int32 i = 0; i < 9; i++) {
			if (!DrawCardInternal(temp_pool, all_weight, &list, const_config->value2())) {
				ZXERO_ASSERT(false) << "DrawCardInternal failed! guid" << Guid() << " draw type =" << Config::DrawType::GoldTen;
			}
		}
		//保底
		temp_pool.clear();
		MtDrawManager::Instance().GetDrawList(4, PlayerLevel(), temp_pool,all_weight);
		if (temp_pool.empty()) {
			return Packet::ResultCode::InvalidDrawConfigData;
		}
		if (all_weight == 0) {
			return Packet::ResultCode::InvalidDrawConfigData;
		}
		if (!DrawCardInternal(temp_pool, all_weight, &list, const_config->value2())) {
			ZXERO_ASSERT(false) << "DrawCardInternal failed! guid" << Guid() << " draw type =" << Config::DrawType::GoldTen;
		}

		//保底药水
		AddItemById(20121001, 10, Config::ItemAddLogType::AddType_Draw, nullptr, const_config->value2());

		return Packet::ResultCode::ResultOK;
	}

	Packet::ResultCode MtPlayer::OnDrawCard(const Config::DrawType draw_type)
	{
		if (draw_type < Config::DrawType::CrystalOne || draw_type >= Config::DrawType_ARRAYSIZE){
			return Packet::ResultCode::InvalidDrawType;
		}		

		Packet::ResultCode ret = Packet::ResultCode::ResultOK;
		Packet::NotifyItemList list;

		switch (draw_type){
			case Config::DrawType::CrystalOne:{
				if (!BagLeftCapacity(Packet::ContainerType::Layer_ItemBag, 2)) {
					return Packet::ResultCode::BagIsFull;
				}
				ret = DrawCrystalOne(list);
				
				if (ret == Packet::ResultCode::ResultOK) {
					OnLuaFunCall_x("xDrawCrystal",
					{
						{ "value",1 }
					});
				}
				break;
			}
			case Config::DrawType::CrystalTen:{	
				if (!BagLeftCapacity(Packet::ContainerType::Layer_ItemBag, 11)) {
					return Packet::ResultCode::BagIsFull;
				}
				ret= DrawCrystalTen(list);
				if (ret == Packet::ResultCode::ResultOK) {
					OnLuaFunCall_x("xDrawCrystal",
					{
						{ "value",10 }
					});
				}
				break;
			}
			case Config::DrawType::GoldOne:{
				if (!BagLeftCapacity(Packet::ContainerType::Layer_ItemBag,2 )) {
					return Packet::ResultCode::BagIsFull;
				}
				ret =DrawGoldOne(list);
				if (ret == Packet::ResultCode::ResultOK) {
					OnLuaFunCall_x("xDrawGold",
					{
						{ "value",1 }
					});
				}
				break;
			}
			case Config::DrawType::GoldTen:{
				if (!BagLeftCapacity(Packet::ContainerType::Layer_ItemBag, 11)) {
					return Packet::ResultCode::BagIsFull;
				}
				ret =DrawGoldTen(list);

				if (ret == Packet::ResultCode::ResultOK) {
					OnLuaFunCall_x("xDrawGold",
					{
						{ "value",10 }
					});						
				}
				break;
			}
		}

		if (ret != Packet::ResultCode::ResultOK) {
			return ret;
		}

		//日志统计
		if (list.item_size() > 0){
			SendMessage(list);
			std::string Statisticslog = "";
			for (auto di : list.item())	{
				char itemconfigid[64] = { 0 };
				zprintf(itemconfigid, sizeof(itemconfigid), "[%d-%d],", di.config_id(), di.amount());
				Statisticslog += itemconfigid;
			}
			LOG_TRACE << "#Statistics##DrawItem#" << Guid() << "," << draw_type << "," << Statisticslog;
		}

		return Packet::ResultCode::ResultOK;
	}

	void MtPlayer::EquipDurableCheck()
	{
		auto items = containers_[Packet::ContainerType::Layer_Equip]->Items();
		bool isdamage = false;
		bool iswarning = false;

		for (auto& item : items) {
			auto equip = boost::dynamic_pointer_cast<MtEquipItem>(item);
			if (equip == nullptr) {
				continue;
			}

			equip->AutoRefixEquipDurable(false);

			auto durable = equip->DbInfo()->durable();
			if (durable <= 0.00001 ) {
				isdamage = true;
				break;
			}
			else if (durable <= equip->Config()->init_durable()*0.2) {
				iswarning = true;
				//break;//此处不能break，有可能会有损坏的
			}
		}
		if (iswarning) {
			if (simple_impacts_.OnCheckImpact(1006)) {
				simple_impacts_.DecSimpleImpact(1006);
			}
			if (!simple_impacts_.OnCheckImpact(1005)) {
				simple_impacts_.AddSimpleImpact(1005, 999999999, 0, 0, false);
			}
		}
		else if (isdamage) {
			if (simple_impacts_.OnCheckImpact(1005)) {
				simple_impacts_.DecSimpleImpact(1005);
			}
			if (!simple_impacts_.OnCheckImpact(1006)) {
				simple_impacts_.AddSimpleImpact(1006, 999999999, 0, 0, false);
			}
		}
		else {
			if (simple_impacts_.OnCheckImpact(1006)) {
				simple_impacts_.DecSimpleImpact(1006);
			}
			if (simple_impacts_.OnCheckImpact(1005)) {
				simple_impacts_.DecSimpleImpact(1005);
			}
		}
	}


	bool MtPlayer::DrawCardInternal( std::vector<boost::shared_ptr<Config::DrawCardConfig>> &draw_pool, const int32 all_weight, Packet::NotifyItemList *list, const int32 draw_cost)
	{
		if (draw_pool.empty() || all_weight<=0)
			return false;

		//struct stemp {
		//	int32 id;
		//	int32 weight;
		//};
		//std::vector<boost::shared_ptr<stemp>> indexs;
		//for (auto child : draw_pool) {
		//	auto ptr = boost::make_shared<stemp>();// (child->id(), child->item_weight());
		//	indexs.push_back(ptr);
		//}
		//每次洗牌
/*
		auto tseed = (uint32)std::chrono::system_clock::now().time_since_epoch().count();
		std::shuffle(draw_pool.begin(), draw_pool.end(), std::default_random_engine(tseed));

		int32 seed = rand_gen->intgen(1, all_weight);
		int32 weigt_temp = 0;
		for (auto child : draw_pool) {
			weigt_temp += child->item_weight();
			if ( seed <= weigt_temp ){
				int32 item_index = child->item_id();
				int32 item_count = child->item_count();

				if (AddItemById(item_index, item_count, Config::ItemAddLogType::AddType_Draw, list, Guid())){
					return true;
				}
				break;
			}
		}
		return false;*/

		std::vector<int32> weights;
		boost::transform(draw_pool, std::back_inserter(weights),
			boost::bind(&Config::DrawCardConfig::item_weight, _1));
		boost::random::discrete_distribution<int, double> dist(std::begin(weights), std::end(weights));
		auto index = dist(rand_gen->generator());
		auto& item = draw_pool[index];

		//三星就重置luackyvalue
		auto checkList = MtDrawManager::Instance().LuaGetDrawList(6, PlayerLevel());
		for (auto child : checkList) {
			if (child->item_id() == item->item_id()) {
				datacell_container_->set_data_32(Packet::CellLogicType::NumberCell, Packet::NumberCellIndex::DrawLuckyValue,0);
				break;
			}
		}
		////

		return AddItemById(item->item_id(), item->item_count(), Config::ItemAddLogType::AddType_Draw, list, draw_cost);
	}

	Packet::ResultCode MtPlayer::OnInspire(int32 type)
	{
		//上限检查
		int32 times = datacell_container_->get_data_32(Packet::CellLogicType::NumberCell, Packet::NumberCellIndex::InspireTimes);
		auto cost = MtConfigDataManager::Instance().FindConfigValue("inspire_cost");
		auto config = MtConfigDataManager::Instance().FindConfigValue("inspire_config");
		if (cost == nullptr || config == nullptr ) {
			return Packet::ResultCode::InvalidDrawConfigData;
		}
		if (times >= config->value2()){
			return Packet::ResultCode::CountLimit;
		}
		if (type == 0){
			//概率
			int32 seed = (int32)rand_gen->intgen(1, 100);
			if (seed > config->value3()) {
				return Packet::ResultCode::InspireFailed;
			}
			//消耗检查
			/////////////////////////////////////优先扣代币
			if (!DelItemById(cost->value3(), 1, Config::ItemDelLogType::DelType_WorldBoss)) {
				//再扣钻石
				if (!DelItemById(Packet::TokenType::Token_Crystal, cost->value1(), Config::ItemDelLogType::DelType_WorldBoss, Guid())) {
					return Packet::ResultCode::CrystalNotEnough;
				}
			}
		}
		else{
			//概率
			int32 seed = (int32)rand_gen->intgen(1, 100);
			if (seed > config->value1()) {
				return Packet::ResultCode::InspireFailed;
			}
			//消耗检查
			if (!DelItemById(Packet::TokenType::Token_Gold, cost->value2(), Config::ItemDelLogType::DelType_WorldBoss, Guid())) {
				return Packet::ResultCode::GoldNotEnough;
			}
		}		

		//ok，加效果
		datacell_container_->add_data_32(Packet::CellLogicType::NumberCell, Packet::NumberCellIndex::InspireTimes,1,true);
		std::string notify = (boost::format("encourage_ok|%1%") % ((times+1) * 20)).str();
		SendClientNotify(notify,-1,-1);
		return Packet::ResultCode::ResultOK;

	}
	Packet::ResultCode MtPlayer::OnWorldBossRelive()
	{
		auto const_config = MtConfigDataManager::Instance().FindConfigValue("worldboss_config");
		if (const_config == nullptr) {
			return Packet::ResultCode::InvalidDrawConfigData;
		}
		//消耗检查
		/////////////////////////////////////优先扣代币
		if (!DelItemById(const_config->value3(), 1, Config::ItemDelLogType::DelType_WorldBoss)) {
			//再扣钻石
			if (!DelItemById(Packet::TokenType::Token_Crystal, const_config->value1(), Config::ItemDelLogType::DelType_WorldBoss, Guid())) {
				return Packet::ResultCode::CrystalNotEnough;
			}
		}

		//ok复活所有角色
		//for (auto child : actors_) {
		//	child->OnRelive();
		//}

		worldboss_relivecd_ = 0;
		Packet::WorldBossReliveCDUpdate update;
		update.set_relivecd(worldboss_relivecd_);
		SendMessage(update);
		//OnGoWorldBossBattle();
		return Packet::ResultCode::ResultOK;
	}

	Packet::ResultCode MtPlayer::OnRepairAllEquip()
	{
		for (auto child : containers_[Packet::ContainerType::Layer_Equip]->Items()) {
			auto equip = boost::dynamic_pointer_cast<MtEquipItem>(child);
			if ( equip == nullptr) {
				continue;
			}					
			equip->DbInfo()->set_durable(float(equip->Config()->init_durable()));
			equip->RefreshData2Client();
		}
		EquipDurableCheck();
		return Packet::ResultCode::ResultOK;
	}

	Packet::ResultCode MtPlayer::OnConfirmAnswer(const std::string& type,const int32 index)
	{
		if (type == "text") {		

			for (auto iter = text_answers_.begin(); iter != text_answers_.end(); ) {
				if (*iter == index) {
					text_answers_.erase(iter);
					//更新存储缓存
					SycnAnswerCache();
					break;
				}
			}
			//return OnNextAnswer(type);
		}
		else if (type == "pic") {		
			if(!CheckPicResult()){//最后一小题															 
				pic_answers_.erase(std::remove(pic_answers_.begin(), pic_answers_.end(), index), pic_answers_.end()); //移除答题
				//更新存储缓存
				SycnAnswerCache();
			}
			//return OnNextAnswer(type);
		}
		else {
			return Packet::ResultCode::UnknownError;
		}

		return Packet::ResultCode::ResultOK;
	}

	bool MtPlayer::CheckPicResult()
	{
		if (pic_group_result_.empty()) {
			return false;
		}
		return true;	//有题
	}

	void MtPlayer::RecordPicResult(const int32 index, const int32 result)
	{
		if (result == 1) {
			pic_group_result_.clear();
		}
		else {
			if (pic_group_result_.find(index) != pic_group_result_.end()) {
				pic_group_result_[index] = result;
			}
			else {
				pic_group_result_.insert({ index, result });
			}

			bool all_ok = true;
			for (auto child : pic_group_result_) {
				if (child.second < 0) {
					all_ok = false;
				}
			}
			if (all_ok) {
				pic_group_result_.clear();
			}
		}
	}

	bool MtPlayer::ValidAnswerIndex(const std::string& type, const int32 index)
	{
		if (type == "text") {
			if (text_answers_.empty()) {
				return false;
			}
			return text_answers_[0] == index;
		}
		else if (type == "pic"){
			if (pic_answers_.empty()) {
				return false;
			}
			return pic_answers_[0] == index;
		}
		else {
			return false;
		}
	}

	bool MtPlayer::IsLastAnswer(const std::string& type)
	{
		if (type == "text") {
			if (text_answers_.empty()) {
				LOG_BI << "TextAnswer," << PlayerBibase() << ",";
				return true;
			}
			return false;
		}
		else if (type == "pic") {
			if (pic_answers_.empty()) {
				LOG_BI << "PicAnswer," << PlayerBibase() << ",";
				return true;
			}
			return false;
		}
		return true;
	}


	bool MtPlayer::ServerLevelCheck(const int32 level)
	{
		//服务器等级判定,等级到了经验先展起来
		auto server_lv = Server::Instance().ServerLevel();
		if (server_lv <= level) {
			//if (!simple_impacts_.OnCheckImpact(1019)) {
			simple_impacts_.AddSimpleImpact(1019, 999999999, server_lv, Server::Instance().NextServerLvDay(), false);
			//}
			if (server_lv == level) {//升到同级别是允许的
				return true;
			}
			return false;
		}
		else {
			if (simple_impacts_.OnCheckImpact(1019)) {
				simple_impacts_.DecSimpleImpact(1019);
			}
		}
		return true;
	}

	Packet::ResultCode MtPlayer::OnNextAnswer(const std::string& type)
	{
		if (type == "text") {
			if (text_answers_.empty()) {
				return Packet::ResultCode::InternalResult;
			}
			//生成客户端题目
			auto config = MtActivityManager::Instance().FindAnswerConfig(text_answers_[0]);
			if (config == nullptr) {
				return SendClientNotify("InvalidConfigData", -1, -1);
			}
			std::vector<std::string> option;
			for (auto child : config->option()) {
				option.push_back(child);
			}
			std::random_shuffle(option.begin(), option.end());

			//设置倒计时
			auto timer_config = MtConfigDataManager::Instance().FindConfigValue("text_answer_timer");
			if (timer_config == nullptr) {
				return SendClientNotify("InvalidConfigData", -1, -1);
			}
			text_answers_timer_=timer_config->value1()*1000;
			///

			Packet::LuaReplyPacket reply;
			reply.set_reply_name("NewTextAnswer");
			reply.add_int32_params(text_answers_[0]);
			for (auto child : option) {
				reply.add_string_params(child);
			}
			SendMessage(reply);
		}
		else if (type == "pic") {
			if (pic_answers_.empty()) {
				return Packet::ResultCode::InternalResult;
			}
			//
			if (pic_group_result_.empty()) {
				//生成客户端题目
				auto configs = MtActivityManager::Instance().FindPicAnswerConfig(pic_answers_[0]);
				if (configs.size() > 0) {
					for (uint32 i = 0; i < configs.size(); i++) {
						RecordPicResult(configs[i]->index(), -1);
					}
				}
			}

			int32 index = -1;
			for (auto child : pic_group_result_) {
				if (child.second < 0) {
					index = child.first;
					break;
				}
			}

			auto config = MtActivityManager::Instance().FindPicAnswerConfigByIndex(index);
			if (config == nullptr) {
				return SendClientNotify("ac_notify_022", -1, 1);
			}
			else{
				std::vector<int32> options;
				for (int32 i = 0; i < config->option_size(); i++) {
					options.push_back(config->option(i));
				}
				std::random_shuffle(options.begin(), options.end());

				Packet::LuaReplyPacket reply;
				reply.set_reply_name("NewPicAnswer");
				reply.add_int32_params(config->index());
				for (auto child : options) {
					reply.add_int32_params(child);
				}
				SendMessage(reply);
			}			
		}

		return Packet::ResultCode::ResultOK;

	}

	Packet::ResultCode MtPlayer::OnResetAnswer(const std::string& type)
	{
		if (type == "text") {
			text_answers_.clear();
		}
		else if (type == "pic") {
			pic_answers_.clear();
			pic_group_result_.clear();
		}
		else {
			return Packet::ResultCode::UnknownError;
		}
		return Packet::ResultCode::ResultOK;

	}
	Packet::ResultCode MtPlayer::OnBeginAnswer(const std::string& type,const int32 count)
	{		
		if (type == "text") {
			if (text_answers_.empty()) {
				text_answers_ = MtActivityManager::Instance().GenTextAnswers(count);
			}	
			OnNextAnswer(type);
		}
		else if (type == "pic") {
			if (pic_answers_.empty()) {
				pic_answers_ = MtActivityManager::Instance().GenPicAnswers(count);
			}
			OnNextAnswer(type);
		}
		else{
			return Packet::ResultCode::UnknownError;
		}
		//更新存储缓存
		SycnAnswerCache();

		return Packet::ResultCode::ResultOK;
	
	}

	void MtPlayer::SycnAnswerCache()
	{
		//更新存储缓存
		std::stringstream result;
		std::copy(text_answers_.begin(), text_answers_.end(), std::ostream_iterator<int>(result, "|"));
		player_db_->set_text_answers(result.str());
			
		//
		std::copy(pic_answers_.begin(), pic_answers_.end(), std::ostream_iterator<int>(result, "|"));
		player_db_->set_pic_answers(result.str());

	}

	void MtPlayer::LoadAnswerCache()
	{
		//uint32 old_size = text_answers_.size();
		//uint32 old_size2 = pic_answers_.size();

		text_answers_.clear();
		pic_answers_.clear();
		utils::split_string(player_db_->text_answers().c_str(), "|", text_answers_);		
		utils::split_string(player_db_->pic_answers().c_str(), "|", pic_answers_);

		//ZXERO_ASSERT(old_size == text_answers_.size()) << "old text answer count size error! guid=" << Guid();
		//ZXERO_ASSERT(old_size2 == pic_answers_.size()) << "old pic answer count size error! guid=" << Guid();
	}

	Packet::ResultCode MtPlayer::OnHellRelive()
	{
		//次数检查
		int32 times = datacell_container_->get_data_32(Packet::CellLogicType::NumberCell, Packet::NumberCellIndex::HellReliveTimes);
		auto const_config = MtConfigDataManager::Instance().FindConfigValue("hell_relive");
		if (const_config == nullptr) {
			return  SendClientNotify("InvalidDrawConfigData", -1, -1);
		}
		if (times >= 4 || times<0) {
			return  SendClientNotify("CountLimit", -1, -1);
		}

		bool is_all_ok = true;
		for (auto child : actors_) {
			if (child->GetLastHp(Packet::LastHpType::HellLastHp) <= 0) {
				is_all_ok = false;
				break;
			}
		}
		if (is_all_ok) {
			return SendClientNotify("nobody_dead", -1, -1);
		}

		std::vector<int32> cost;
		cost.push_back(const_config->value1());
		cost.push_back(const_config->value2());
		cost.push_back(const_config->value3());
		cost.push_back(const_config->value4());

		//消耗检查
		if (!DelItemById(Packet::TokenType::Token_Crystal, cost[times], Config::ItemDelLogType::DelType_Hell, Guid())) {
			return  SendClientNotify("CrystalNotEnough", -1, -1);
		}
		//ok复活所有角色
		ResetPlayerActorLastHp(Packet::LastHpType::HellLastHp);

		return SendClientNotify("hero_relive_ok", -1, -1);

		return Packet::ResultCode::ResultOK;
	}

	void MtPlayer::SendCommonReply(const std::string &name, const std::vector<int32>& int32values, const std::vector<uint64>& int64values, const std::vector<std::string>& stringvalues)
	{
		Packet::CommonReply reply;
		reply.set_reply_name(name);
		for (auto child : int32values) {
			reply.add_int32_params(child);
		}
		for (auto child : int64values) {
			reply.add_int64_params(child);
		}
		for (auto child : stringvalues) {
			reply.add_string_params(child);
		}
		
		SendMessage(reply);
	}

	void MtPlayer::BroadCastCommonReply(const std::string &name, const std::vector<int32>& int32values, const std::vector<uint64>& int64values, const std::vector<std::string>& stringvalues)
	{
		Packet::CommonReply reply;
		reply.set_reply_name(name);
		for (auto child : int32values) {
			reply.add_int32_params(child);
		}
		for (auto child : int64values) {
			reply.add_int64_params(child);
		}
		for (auto child : stringvalues) {
			reply.add_string_params(child);
		}

		BroadCastMessage(reply);
	}

	void MtPlayer::SendLuaPacket(const std::string &name, const std::vector<int32>& int32values, const std::vector<uint64>& int64values, const std::vector<std::string>& stringvalues)
	{
		Packet::LuaReplyPacket reply;
		reply.set_reply_name(name);
		for (auto child : int32values) {
			reply.add_int32_params(child);
		}
		for (auto child : int64values) {
			reply.add_int64_params(child);
		}
		for (auto child : stringvalues) {
			reply.add_string_params(child);
		}

		SendMessage(reply);
	}

	void MtPlayer::BroadCastLuaPacket(const std::string &name, const std::vector<int32>& int32values, const std::vector<uint64>& int64values, const std::vector<std::string>& stringvalues)
	{
		Packet::LuaReplyPacket reply;
		reply.set_reply_name(name);
		for (auto child : int32values) {
			reply.add_int32_params(child);
		}
		for (auto child : int64values) {
			reply.add_int64_params(child);
		}
		for (auto child : stringvalues) {
			reply.add_string_params(child);
		}

		BroadCastMessage(reply);
	}
	
	int MtPlayer::OnLuaFunCall_0(const int32 script_id, const std::string &func_name)
	{
		try {
			return thread_lua->call<int32>(script_id, func_name.c_str(), this);
		}
		catch (ff::lua_exception& e) {
			LOG_ERROR << " player guid=" << Guid() << e.what();
			SendCommonResult(Packet::ResultOption::Default_Opt, Packet::ResultCode::LuaScriptException);
			return -1;
		}
	}

	int MtPlayer::OnLuaFunCall_1( const int32 script_id, const std::string &func_name, const int32 param1)
	{
		try {
			return thread_lua->call<int32>(script_id, func_name.c_str(), this, param1);
		}
		catch (ff::lua_exception& e) {
			LOG_ERROR << " player guid=" << Guid() << e.what();
			SendCommonResult(Packet::ResultOption::Default_Opt, Packet::ResultCode::LuaScriptException);
			return -1;
		}
	}
	
	int MtPlayer::OnLuaFunCall_n(const int32 script_id, const std::string &func_name, const std::map<std::string, int32> params)
	{
		try {
			return thread_lua->call<int32>(script_id, func_name.c_str(), this, params);
		}
		catch (ff::lua_exception& e) {
			LOG_ERROR << " player guid=" << Guid() << e.what();
			SendCommonResult(Packet::ResultOption::Default_Opt, Packet::ResultCode::LuaScriptException);
			return -1;
		}
	}
	int MtPlayer::OnLuaFunCall_n_64(const int32 script_id, const std::string &func_name, const std::map<std::string, uint64> params)
	{
		try {
			return thread_lua->call<int32>(script_id, func_name.c_str(), this, params);
		}
		catch (ff::lua_exception& e) {
			LOG_ERROR << " player guid=" << Guid() << e.what();
			SendCommonResult(Packet::ResultOption::Default_Opt, Packet::ResultCode::LuaScriptException);
			return -1;
		}
	}
	
	int MtPlayer::OnLuaFunCall_x(const std::string &func_name, const std::map<std::string, int32> params)
	{
		if (scene_ != nullptr) {
			scene_->GetLuaCallProxy()->CallFunc_x(this,func_name, params);
		}
		else {
			ZXERO_ASSERT(false) << "OnLuaFunCall_x failed! scene is null! guid=" << Guid()<<func_name;
		}
		return 0;
	}
	int MtPlayer::OnLuaFunCall_c(const int32 script_id, const std::string &func_name, const std::vector<int32> int32param, const std::vector<uint64> int64param, const std::vector<std::string> stringparam)
	{
		try {
			return thread_lua->call<int32>(script_id, func_name.c_str(), this, int32param, int64param, stringparam);
		}
		catch (ff::lua_exception& e) {
			LOG_ERROR << " player guid=" << Guid() << e.what();
			SendCommonResult(Packet::ResultOption::Default_Opt, Packet::ResultCode::LuaScriptException);
			return -1;
		}
	}
	int MtPlayer::OnLuaFunCall_OnMessageHandler(const std::string& funcname, google::protobuf::Message *message)
	{
		if (scene_ != nullptr) {
			scene_->GetLuaCallProxy()->OnLuaFunCall_OnMessageHandler(this, funcname, message);
		}
		else {
			ZXERO_ASSERT(false) << "OnLuaFunCall_x failed! scene is null! guid=" << Guid() << funcname;
		}
		return 0;
	}

	boost::shared_ptr<MtScene> MtPlayer::OnRaidNpc2Scene(const int32 sceneid)
	{
		if (!MtScene::IsRaidScene(sceneid)) {
			ZXERO_ASSERT(false) << "it is not raid scene ,scene id=" << sceneid;
			SendCommonResult(Packet::ResultOption::Raid_Opt, Packet::ResultCode::CreateRaidFailed);
			return nullptr;
		}
		auto raid_scene = MtSceneManager::Instance().GetSceneById(sceneid);
		if (raid_scene == nullptr) {
			SendCommonResult(Packet::ResultOption::Raid_Opt, Packet::ResultCode::CreateRaidFailed);
			return nullptr;
		}

		return raid_scene;
	}

	void MtPlayer::OnNpcOptionReply(const int32 npc_index, const std::string msg, const std::string str_param, const int32 int_param)
	{
		Packet::NpcSessionReply reply;
		reply.set_index(npc_index);
		reply.set_msg_name(msg);
		reply.set_string_param(str_param);
		reply.set_int_param(int_param);

		SendMessage(reply);
	}

	MtMission* MtPlayer::GetMissionById(int32 missionid)
	{
		auto missionproxy = GetMissionProxy();
		if (missionproxy == nullptr)
		{
			return nullptr;
		}
		return missionproxy->GetMissionById(missionid);
	}
	void MtPlayer::OnMissionRaid(const int32 mission_id)
	{
		auto mission_config = MtMissionManager::Instance().FindMission(mission_id);
		if (mission_config == nullptr) {
			return SendCommonResult(Packet::ResultOption::Raid_Opt, Packet::ResultCode::InvalidConfigData);
		}

		if (mission_proxy_ == nullptr) {
			return SendCommonResult(Packet::ResultOption::Raid_Opt, Packet::ResultCode::InvalidMissionData);
		}

		auto ret = mission_proxy_->OnAcceptCheck(mission_config->type());
		if (ret != Packet::ResultCode::ResultOK) {
			return SendCommonResult(Packet::ResultOption::Raid_Opt, ret);
		}

		int32 sceneid = mission_config->param(1);

		//检查身上是否了同类型任务
		MtMission* mission = nullptr;
		for (auto child : mission_proxy_->GetMissionMap()) {
			if (child.second->Config()->type() == mission_config->type()) {
				mission = child.second;
				break;
			}
		}
		if (mission !=nullptr){
			//如果是副本任务posid
			int32 posid = mission->MissionDB()->param(Packet::MissionDB_Param::Pos_Id);
			auto layout = MtMonsterManager::Instance().FindScenePos(posid);
			if (layout != nullptr) {
				//以任务实例为准
				sceneid = layout->scene_id();
			}

			auto raid_scene = OnRaidNpc2Scene(sceneid);
			if (raid_scene != nullptr) {
				int32 rtid = mission->MissionDB()->param(Packet::MissionDB_Param::CarbonRuntimeId);
				auto raid = raid_scene->GetRaid(rtid);
				if (raid == nullptr) {
					rtid = INVALID_GUID; //需要创建副本实例
				}
								
				GoRaid(raid_scene->SceneId(), 2001, rtid, -1, INVALID_POS);
			}
		}
		else{
			auto raid_scene = OnRaidNpc2Scene(sceneid);
			if (raid_scene != nullptr && mission_proxy_ != nullptr ) {
				//添加入口任务
				mission_proxy_->OnAddMission(mission_id);
				GoRaid(raid_scene->SceneId(), 2001, INVALID_GUID, -1, INVALID_POS);
			}
		}		
	}

	void MtPlayer::OnJoinBattleRoom( const int32 scene_id, const int32 script_id)
	{
		if (session_ != nullptr) {
			auto msg = boost::make_shared<Packet::JoinBattleRoom>();
			msg->set_npc_index(-1);
			msg->set_scene_id(scene_id);
			msg->set_script_id(script_id);
			Server::Instance().FakeClientMessage(session_, msg);
		}
	}

	boost::shared_ptr<MtNpc> MtPlayer::FindNpc(const int32 series_id)
	{
		if (npc_map_.find(series_id) != npc_map_.end()) {
			return npc_map_[series_id];
		}

		if (scene_ != nullptr) {
			auto npc = scene_->FindNpc(series_id);
			if (npc != nullptr)
				return npc;
		}

		if (raid_ != nullptr)
		{
			return raid_->FindNpc(series_id);
		}
		return nullptr;
	}

	void MtPlayer::DecBossChallengeCount(int32 stage_id, int32 count) 
	{
		GetPlayerStageProxy()->DecBossChallengeCount(stage_id, count);
	}
	int32 MtPlayer::GetBossChallengeCount(int32 stage_id)
	{
		auto player_stage = GetPlayerStageProxy()->GetPlayerStage(stage_id);
		if (player_stage == nullptr)
			return 0;
		return player_stage->boss();
	}

	MtBattleGroundManager * MtPlayer::GetBattleManager()
	{
		if (scene_ == nullptr) {
			return nullptr;
		}

		return &scene_->GetBattleManager();
	}

	void MtPlayer::OnSendWorldBossBattleReply()
	{
		auto boss = MtWorldBossManager::Instance().GetCurWorldBossInfo();
		if (boss == nullptr)
			return ;

		Packet::WorldBossInfoReply bossreply;
		bossreply.mutable_boss()->CopyFrom(*boss.get());
		bossreply.set_boss_seriesid(MtWorldBossManager::Instance().BossSeriesid());
		bossreply.set_allhp(MtWorldBossManager::Instance().BossAllHp());
		SendMessage(bossreply);

		Packet::WorldBossReliveCDUpdate update;
		update.set_relivecd(worldboss_relivecd_);
		SendMessage(update);
	}
	void MtPlayer::OnSendWorldBossHpUpdate()
	{
		auto boss = MtWorldBossManager::Instance().GetCurWorldBossInfo();
		if (boss == nullptr)
			return;
		Packet::WorldBossHpUpdate update;
		update.set_hp(boss->hp());
		SendMessage(update);
	}
	bool MtPlayer::CheckGuildBattleQualify()
	{
		uint64 guildid = GetGuildID();
		if (guildid == INVALID_GUID)
			return false;
		auto data = MtConfigDataManager::Instance().FindConfigValue("guild_battle_playerqualify");
		if (data == nullptr)
			return false;
		auto active = MtActivityManager::Instance().FindActivity(25);
		if (active == nullptr)
			return false;
		if (PlayerLevel() < active->open_level())
			return false;
		auto inbattle = MtGuildManager::Instance().GetGuildBattle(guildid);
		if (inbattle == nullptr)
			return false;
		uint32 jointimetonow = MtTimerManager::Instance().DiffTimeToNow(GetPlayerGuildProxy()->Getjointime());
		if (jointimetonow < (uint32)(data->value1() * 24 * 3600) )
			return false;
		return true;
	}
	
	void MtPlayer::SendGuildBattleStage(int32 stage, bool dialog)
	{
		Packet::UpdateGuildBattleStage update;
		update.set_stage((Packet::GuildBattle_Stage)(stage));
		update.set_dialog(dialog);
		SendMessage(update);
	}
	bool MtPlayer::OnUpdateFollowPath()
	{		
		auto cur_pos = player_data_->scene_pos();
		if (follow_path_.size() > 0) {
			auto head = follow_path_.front();			
			auto new_distance = zxero::distance2d(cur_pos, head);
			if (new_distance > 2100)
			{//记录锚点
				follow_path_.push_front(cur_pos);
				if (follow_path_.size() > 4) { //只保留4个点
					follow_path_.pop_back();
				}
				//return true;
			}
		}
		else {
			follow_path_.push_front(cur_pos);
			return false;
		}
		//return false;
		return true;
	}	

	void MtPlayer::OnResetFollowPath()
	{
		follow_path_.clear();
	}

	void MtPlayer::UpdateContainer(const Packet::ContainerType type)
	{
		Packet::PlayerContainerReply container_info;
		auto con = FindContainer(type);
		if (con) {
			con->FillMessage(container_info);
			SendMessage(container_info);
		}
		else {		
			SendCommonResult(Packet::ResultOption::Item_Common, Packet::ResultCode::InvalidContainerType);
		}
	}

	void MtPlayer::EndGuildBoss(int32 damage)
	{
		auto message = boost::make_shared<S2G::EndGuildBoss>();
		message->set_guid(Guid());
		message->set_guild_id(GetScenePlayerInfo()->guildid());
		message->set_damage(damage);
		Server::Instance().SendMessage(message, shared_from_this()); 
		UpdateGuildBossDamage(1, damage);
	}
	void MtPlayer::UpdateGuildBossDamage(int32 opt, int32 damage)
	{
		Packet::UpdateGuildBossUserDamage up;
		up.set_opt(opt);
		up.set_damagevalue(damage);
		SendMessage(up);
	}
	void MtPlayer::EndWorldBoss()
	{
		worldboss_relivecd_ = 45;
		Packet::WorldBossReliveCDUpdate update;
		update.set_relivecd(worldboss_relivecd_);
		SendMessage(update);
	}
	void MtPlayer::OnWorldBossDamage(int32 damage)
	{
		MtWorldBossManager::Instance().OnBattleEndDamageBoss(shared_from_this(), damage);
	}
	void MtPlayer::AddWorldBossActorDamage(uint64 guid, int32 damage)
	{
		auto wad = worldboss_actordamage_.find(guid);
		if (wad == worldboss_actordamage_.end())
		{
			worldboss_actordamage_.insert({ guid ,damage });
		}
		else
		{
			wad->second += damage;
		}
	}
	const std::map<uint64, int32>& MtPlayer::GetWorldBossActorDamage()
	{
		return worldboss_actordamage_;
	}

	void MtPlayer::SetContainer(const Packet::ContainerType layer, const boost::shared_ptr<MtContainer>& container)
	{
		if (containers_.find(layer) != containers_.end()) {
			ZXERO_ASSERT(false) << "repeated container! type =" << layer;
		}
		containers_[layer] = container;
	}

	Packet::ResultCode MtPlayer::GoldOperation( const bool is_take, const int32 count)
	{
		if (is_take) {
			auto token_data = player_db_->token_data();
			int32 have = token_data.bank_gold();
			if (have < count) {
				return Packet::ResultCode::GoldNotEnough;
			}
			else {
				if (!DelItemById(Packet::TokenType::Token_BankGold, count, Config::ItemDelLogType::DelType_BankStore)) {
					return Packet::ResultCode::UnknownError;
				}
				AddItemById(Packet::TokenType::Token_Gold, count, Config::ItemAddLogType::AddType_BankStore);
			}
		}
		else {
			if (!DelItemById(Packet::TokenType::Token_Gold, count, Config::ItemDelLogType::DelType_BankTake)) {
				return Packet::ResultCode::UnknownError;
			}
			AddItemById(Packet::TokenType::Token_BankGold, count, Config::ItemAddLogType::AddType_BankTake);
		}

		return Packet::ResultCode::ResultOK;
	}

	Packet::ResultCode MtPlayer::UseItem(const int64 guid, const int64 actor_guid)
	{
		auto item_base = FindItemByGuid(Packet::ContainerType::Layer_MissionAndItemBag,guid);
		if (item_base == nullptr) {
			return Packet::ResultCode::InvalidItem;
		}

		MtNormalItem* item = static_cast<MtNormalItem*>(item_base.get());
		if (item == nullptr) {
			return Packet::ResultCode::InvalidItem;
		}

		auto actor_target = FindActorByGuid(actor_guid);

		return item->OnUse(actor_target);

	}

	Packet::ResultCode MtPlayer::UseItemByCfgId(const int32 config_id, const int32 count)
	{
		auto left = containers_[Packet::ContainerType::Layer_ItemBag]->GetItemCount(config_id);
		if (left < count) {
			return Packet::ResultCode::CostItemNotEnough;
		}

		//批量使用保护
		int32 can_use = count;
		if (can_use > 100) {
			can_use = 100;
		}		
		for (int32 i = 0; i < can_use; i++) {
			auto item_base = FindItemByConfigId(Packet::ContainerType::Layer_ItemBag, config_id);
			if (item_base == nullptr) {
				return Packet::ResultCode::InvalidItem;
			}

			MtNormalItem* item = static_cast<MtNormalItem*>(item_base.get());
			if (item == nullptr) {
				return Packet::ResultCode::InvalidItem;
			}
			
			auto ret = item->OnUse();
			if (ret != Packet::ResultCode::ResultOK) {
				return ret;
			}
		}

		return Packet::ResultCode::ResultOK;
	}


	Packet::ResultCode MtPlayer::ExpPointOption(const int32 lock_or_get)
	{
		auto token = player_db_->token_data();
		int32 have_count = token.exp_point();
		int32 cold_count = token.cold_exp_point();
		if (lock_or_get == 0) {//冻结
			if(!DelItemById(Packet::TokenType::Token_ExpPoint, have_count,Config::ItemDelLogType::DelType_ExpPoint, Guid()))
				return Packet::ResultCode::UnknownError;

			AddItemById(Packet::TokenType::Token_ColdExpPoint, have_count, Config::ItemAddLogType::AddType_ExpPoint, nullptr, Guid());
		}
		else {
			auto data = MtConfigDataManager::Instance().FindConfigValue("exp_point_max");			
			int32 max_count = 0;
			if (data != nullptr) {
				max_count = data->value1();
			}

			if (have_count >= max_count) {
				return Packet::ResultCode::ExpPointLimit;
			}

			if (cold_count <= 0) {
				return Packet::ResultCode::ColdExpPointNotEnough;
			}

			data = MtConfigDataManager::Instance().FindConfigValue("exp_point_day_reward");
			int32 get_count = 0;
			if (data != nullptr) {
				get_count = data->value1();
			}
			
			if (get_count > cold_count) {
				get_count = cold_count;
			}
			
			//两头都要判定，蛋疼的规则！
			int32 need_count = max_count - have_count;
			int32 final_count = need_count > get_count ? get_count : need_count;

			if (!DelItemById(Packet::TokenType::Token_ColdExpPoint, final_count, Config::ItemDelLogType::DelType_ExpPoint, Guid()))
				return Packet::ResultCode::UnknownError;

			AddItemById(Packet::TokenType::Token_ExpPoint, final_count, Config::ItemAddLogType::AddType_ExpPoint, nullptr, Guid());
		}

		return Packet::ResultCode::ResultOK;
	}

	Packet::ResultCode MtPlayer::DestroyItem(const int64 guid)
	{
		auto item_base = FindItemByGuid(Packet::ContainerType::Layer_AllBag,guid);
		if (item_base == nullptr) {
			return Packet::ResultCode::InvalidItem;
		}

		if (item_base->IsLock()) {
			return Packet::ResultCode::ItemIsLocked;
		}

		auto config = MtItemManager::Instance().GetItemConfig(item_base->ConfigId());
		if (config == nullptr) {
			return Packet::ResultCode::InvalidConfigData;
		}

		if (!config->can_destroy()) {
			return Packet::ResultCode::CantDestroyItem;
		}
		
		if (!item_base->Container()->RemoveItem(item_base)) {
			return Packet::ResultCode::CantDestroyItem;
		}

		return Packet::ResultCode::ResultOK;
	}

	Packet::ResultCode MtPlayer::SellItem(const int64 guid)
	{
		//10级以前不让卖东西
		if (PlayerLevel() < 10) {
			return Packet::ResultCode::CantSellItem;
		}

		auto item_base = FindItemByGuid(Packet::ContainerType::Layer_ItemBag,guid);
		if (item_base == nullptr) {
			return Packet::ResultCode::InvalidItem;
		}

		if (item_base->IsLock()) {
			return Packet::ResultCode::ItemIsLocked;
		}

		auto config = MtItemManager::Instance().GetItemConfig(item_base->ConfigId());
		auto price = 0;
		int32 type = Packet::TokenType::Token_Gold;
		if (config == nullptr) {
			auto gem_config = MtItemManager::Instance().GetGemConfig(item_base->ConfigId());
			if (gem_config == nullptr) {
				return Packet::ResultCode::InvalidConfigData;
			} else {
				price = gem_config->coin();
			}

		} else {
			if (config->can_sale() <= 0) {
				return Packet::ResultCode::CantSellItem;
			}
			else{
				//非0都是代币
				type = (int32)config->can_sale();
			}			
			price = config->coin();
		}

		if (price <=0) {
			return Packet::ResultCode::CantSellItem;
		}

		if (!DelItems({ item_base }, Config::ItemDelLogType::DelType_SellItem)) {
			return Packet::ResultCode::CantSellItem;
		}

		AddItemByIdWithNotify(type, price, Config::ItemAddLogType::AddType_SellItem, guid);

		return Packet::ResultCode::ResultOK;
	}

	Packet::ResultCode MtPlayer::BuyMarketItem(const Config::MarketType type, const int32 index, const int32 count, const bool isauto)
	{
		auto config = MtMarketManager::Instance().FindMarket(type);
		if (config == nullptr){
			return SendClientNotify("InvalidMarketData", -1, -1);
		}

		if (config->open() != 1) {
			return SendClientNotify("InvalidMarketData", -1, -1);
		}

		auto market = FindMarket(type);
		if (market == nullptr){
			return SendClientNotify("InvalidMarketData", -1, -1);
		}
		
		auto market_item = MtMarketManager::Instance().FindItem(index);
		if (market_item == nullptr)	{
			return SendClientNotify("InvalidConfigData", -1, -1);
		}

		if (!MtMarketManager::Instance().BuyCheck(market, market_item, this, count)){
			return Packet::ResultCode::InternalResult;
		}

		if (!BagLeftCapacity(MtItemManager::Instance().CheckOwnerContainer(market_item->item_id()), 1)){
			return SendClientNotify("BagIsFull", -1, -1);
		}

		//扣钱
		int32 final_cost = count*market_item->price_value();
		if (!DelItemById(market_item->price_type(), final_cost, Config::ItemDelLogType::DelType_Market, index))
		{
			if (market_item->price_type() == Packet::TokenType::Token_Gold)
				return Packet::ResultCode::GoldNotEnough;
			else if (market_item->price_type() == Packet::TokenType::Token_Crystal)
				return Packet::ResultCode::CrystalNotEnough;
			else if (market_item->price_type() == Packet::TokenType::Token_Honor)
				return SendClientNotify("TokenHonorNotEnough", -1, -1);
			else if (market_item->price_type() == Packet::TokenType::Token_Reward) 
				return SendClientNotify("TokenRewardNotEnough", -1, -1);
			else if (market_item->price_type() == Packet::TokenType::Token_Day) 
				return SendClientNotify("TokenDayNotEnough", -1, -1);
			else if (market_item->price_type() == Packet::TokenType::Token_GuildContribute)
				return SendClientNotify("TokenGuildContributeNotEnough", -1, -1);
			else if (market_item->price_type() == Packet::TokenType::Token_GuildMoney)
				return SendClientNotify("TokenGuildMoneyNotEnough", -1, -1);
			else if (market_item->price_type() == Packet::TokenType::Token_GuildBonus)
				return SendClientNotify("TokenGuildBonusNotEnough", -1, -1);
			else if (market_item->price_type() == Packet::TokenType::Token_LawFul)
				return SendClientNotify("TokenLawFulNotEnough", -1, -1);
			else
				return SendClientNotify("CostItemNotEnough", -1, -1);
		}

		//
		int32 final_count = count*market_item->item_count();
		if (isauto) {
			if (!AddItemById(market_item->item_id(), final_count, Config::ItemAddLogType::AddType_Market,nullptr,type)) {
				return SendClientNotify("BagIsFull", -1, -1);
			}
		}
		else {
			if (!AddItemByIdWithNotify(market_item->item_id(), final_count, Config::ItemAddLogType::AddType_Market, type)) {
				return SendClientNotify("BagIsFull", -1, -1);
			}
		}
		market->UpdateItem(index, count);

		return Packet::ResultCode::ResultOK;
	}
	

	void MtPlayer::SetInBattle(bool in_battle)
	{

		Packet::PlayerRunTimeStatusNotify notify;
		runtime_status_->set_in_battle(in_battle);
		notify.set_guid(Guid());
		notify.mutable_status()->CopyFrom(*runtime_status_);
		BroadCastMessage(notify);
	}

	bool MtPlayer::GetInBattle()
	{
		if (runtime_status_ == nullptr)
		{
			return false;
		}
		return runtime_status_->in_battle();
	}
	void MtPlayer::SetOnHook(bool on_hook)
	{
		Packet::PlayerRunTimeStatusNotify notify;
		runtime_status_->set_on_hook(on_hook);
		notify.set_guid(Guid());
		notify.mutable_status()->CopyFrom(*runtime_status_);
		BroadCastMessage(notify);
	}

	void MtPlayer::SetOnHookOption(const Packet::HookOption &option)
	{	
		auto hook_option = player_db_->mutable_hook_option();
		hook_option->CopyFrom(option);
		//TODO  需要做相应的校验
		Packet::ChangeHookInfo reply;
		hook_option = reply.mutable_hook_option();
		hook_option->CopyFrom(option);
		SendMessage(reply);
	}

	Packet::PlayerRunTimeStatus& MtPlayer::RunTimeStatus()
	{
		return *runtime_status_;
	}
	void MtPlayer::BroadCastRunTimeStatus()
	{
		Packet::PlayerRunTimeStatusNotify notify;
		notify.set_guid(Guid());
		notify.mutable_status()->CopyFrom(*runtime_status_);
		BroadCastMessage(notify);
	}
	//
	//
	const Packet::BattleFeildProxy* MtPlayer::GetBattleFeildProxy() {
		std::lock_guard<std::mutex> guard(bfproxy_mutex_);
		return &bf_proxy_;
	}

	void MtPlayer::SetBattleFeildProxy(const Packet::BattleFeildProxy& proxy) {
		std::lock_guard<std::mutex> guard(bfproxy_mutex_);
		bf_proxy_ = proxy;
	}

	//////////////
	uint64 MtPlayer::GetTeamID()
	{
		std::lock_guard<std::mutex> guard(team_mutex_);
		return runtime_status_->team().teamid();
	}

	uint64 MtPlayer::GetTeamLeader()
	{
		std::lock_guard<std::mutex> guard(team_mutex_);
		return runtime_status_->team().leaderid();
	}
	Packet::PlayerTeamStatus MtPlayer::GetTeamStatus()
	{
		std::lock_guard<std::mutex> guard(team_mutex_);
		for (auto member : runtime_status_->team().member())
		{
			if (member.memberid() == Guid()){
				return member.teamstatus();
			}
		}
		return Packet::PlayerTeamStatus::Team_AFKING;
	}

	bool MtPlayer::IsTeamLeader()
	{
		std::lock_guard<std::mutex> guard(team_mutex_);
		return (runtime_status_->team().leaderid() == Guid());
	}
	int32 MtPlayer::TeamMemberSize() const
	{
		std::lock_guard<std::mutex> guard(team_mutex_);
		return runtime_status_->team().member_size();
	}
	bool MtPlayer::IsTeamGroup()
	{
		std::lock_guard<std::mutex> guard(team_mutex_);
		return runtime_status_->team().group();
	}
	Packet::ArrangeType MtPlayer::GetTeamArrangeType()
	{
		std::lock_guard<std::mutex> guard(team_mutex_);
		return runtime_status_->team().arrangetype();
	}
	std::vector<uint64> MtPlayer::GetTeamMember(bool follow_state)
	{
		std::lock_guard<std::mutex> guard(team_mutex_);
		std::vector<uint64> members;
		for (auto member : runtime_status_->team().member())
		{
			if (follow_state == true)
			{
				if (member.teamstatus() == Packet::PlayerTeamStatus::Team_Follow)
				{
					members.push_back(member.memberid());
				}
			}
			else
			{
				members.push_back(member.memberid());
			}
		}
		return members;
	}
	void MtPlayer::OnSyncPlayerTeamData(const boost::shared_ptr<G2S::SyncPlayerTeamData>& msg)
	{
		runtime_status_->mutable_team()->CopyFrom(msg->team());
		BroadCastRunTimeStatus();

		OnLuaFunCall_x("xOnTeam",
		{
			{ "value",-1 }
		});
	}
	void MtPlayer::CleanTeamData()
	{
		runtime_status_->mutable_team()->set_teamid(INVALID_GUID);
		runtime_status_->mutable_team()->set_leaderid(INVALID_GUID);
		runtime_status_->mutable_team()->set_arrangetype(Packet::ArrangeType::AT_Roll);
		runtime_status_->mutable_team()->set_group(false);
		runtime_status_->mutable_team()->clear_member();
		Packet::PlayerLeaveZone leave_notify;
		for (auto& robot : robots_) {
			leave_notify.set_guid(robot.basic_info_.guid());
			SendMessage(leave_notify);
		}
		robots_.clear();
	}

	int32 MtPlayer::TeamLevel()
	{
		if (GetTeamID() == INVALID_GUID)
		{
			return 0;
		} else {
			std::vector<int32> levels;
			for (auto& member : runtime_status_->team().member()) {
				auto player = Server::Instance().FindPlayer(member.memberid());
				if (player != nullptr)
				{
					levels.push_back(player->PlayerLevel());
				}
			}
			int32 s1 = std::accumulate(levels.begin(), levels.end(), 0,
				[](int32 sum, int32 level) {
				return sum + int32(std::pow(level, 3)); });
			int32 s2 = std::accumulate(levels.begin(), levels.end(), 0,
				[](int32 sum, int32 level) {
				return sum + int32(std::pow(level, 2)); });
			if (s1 > 0 && s2 > 0) {
				return int32(std::ceil(s1 / s2));
			} else {
				return 0;
			}
		}
	}

	void MtPlayer::ResetTeamData()
	{
		CleanTeamData();
		//runtime_status_->mutable_team()->set_teamstatus(Packet::PlayerTeamStatus::Team_AFK);
		BroadCastRunTimeStatus();
	}
	void MtPlayer::PlayerAutoMatch( int32 purpose, bool setauto)
	{
		auto message = boost::make_shared<S2G::PlayerAutoMatch>(); 
		message->set_purpose(purpose);
		message->set_purpose(setauto);
		Server::Instance().SendMessage(message,shared_from_this());

		//Server::Instance().RunInLoop(boost::bind(&MtTeamManager::OnPlayerAutoMatch, boost::ref(MtTeamManager::Instance()),shared_from_this(), purpose, setauto));
	}

	void MtPlayer::BroadcastTeamEx( bool follow_state, const google::protobuf::Message& msg)
	{
		if (follow_state) {
			for (auto member : runtime_status_->team().member()) {
				if (member.teamstatus() == Packet::PlayerTeamStatus::Team_Follow) {
					auto player_ = Server::Instance().FindPlayer(member.memberid());
					if (player_) {
						player_->SendMessage(msg);
					}
				}
			}
		}
		else {
			for (auto member : runtime_status_->team().member()) {
				auto player_ = Server::Instance().FindPlayer(member.memberid());
				if (player_) {
					player_->SendMessage(msg);
				}
			}
		}
	}
	void MtPlayer::BroadcastTeam( const std::vector<uint64>& filter_list, const google::protobuf::Message& msg)
	{
		for (auto member : runtime_status_->team().member())
		{
			uint64 memberid = member.memberid();
			if (std::find(filter_list.begin(), filter_list.end(), memberid) != filter_list.end())
				continue;
			auto player_ = Server::Instance().FindPlayer(memberid);
			if (player_)
			{
				player_->SendMessage(msg);
			}
		}
	}
	const std::string& MtPlayer::Name() const
	{
		return player_data_->name();
	}

	void MtPlayer::SetName(const std::string &name)
	{
		player_data_->set_name(name);
		auto mactor_ = MainActor();
		if (mactor_)
		{
			mactor_->SetName(name);
		}
	}

	void  MtPlayer::OnGetPractice()
	{
		Packet::GetPracticeReply reply;
		for (auto practice : guild_practice_)
		{
			reply.add_list()->CopyFrom(*practice.get());
		}
		SendMessage(reply);
	}

	boost::shared_ptr<Packet::PracticeInfo> MtPlayer::GetPracticeInfo(int32 id)
	{
		for (auto practice : guild_practice_)
		{
			if (practice->id() == id)
			{
				return practice;
			}
		}
		return nullptr;
	}
	void MtPlayer::AddPracticeInfo(boost::shared_ptr<Packet::PracticeInfo> p)
	{
		guild_practice_.push_back(p);
	}

	std::vector<boost::shared_ptr<Packet::PracticeInfo >>& MtPlayer::GetAllPracticeInfo()
	{
		return guild_practice_;
	}
	void MtPlayer::AddPracticeExp(int32 id, int32 type, int32 guildlevel, int32 allcontribution)
	{
		auto costconfig = MtConfigDataManager::Instance().FindConfigValue("guild_practice_cost");
		if (costconfig == nullptr ) {
			SendClientNotify("InvalidDrawConfigData", -1, -1);
			return ;
		}
		int32 cost = costconfig->value1();
		int32 practicelevel = 0;
		int32 practiceexp = 0;
		int32 addexp = 0;
		boost::shared_ptr<Packet::PracticeInfo> practice = GetPracticeInfo(id);
		if (practice != nullptr)
		{
			practicelevel = practice->level();
			practiceexp = practice->exp();
		}
		boost::shared_ptr<Config::GuildPracticeLevelConfig> config = MtGuildManager::Instance().GetPracticeLevelConfig(practicelevel);
		if (config == nullptr)
		{
			SendClientNotify("xiuliandengjiyiman", -1, -1);
			return;
		}
		if (MtGuildManager::Instance().GetPracticeLevelConfig(practicelevel + 1) == nullptr)
		{
			SendClientNotify("xiuliandengjiyiman", -1, -1);
			return;
		}
		
		//if (PlayerLevel() < 40)
		//	return Packet::ResultCode::Guild_PracticePlayerlevel;
		if (config->playerlimit() > PlayerLevel())
		{
			SendClientNotify("Guild_PracticePlayerlevel", -1, -1);
			return;
		}
		if (config->guildlimit() > guildlevel)
		{
			SendClientNotify("Guild_PracticeGuildlevel", -1, -1);
			return;
		}
		if (config->contributionlimit() > allcontribution)
		{
			SendClientNotify("Guild_PracticeContribution", -1, -1);
			return;
		}
		if (type == 0)
		{
			if (!DelItemById(Packet::TokenType::Token_Gold, cost, Config::ItemDelLogType::DelType_Guild)) {
				SendClientNotify("GoldNotEnough", -1, -1);
				return ;
			}
			addexp = 10;
		}
		else if (type == 1)
		{

			if (!DelItemById(Packet::TokenType::Token_Gold, cost*10, Config::ItemDelLogType::DelType_Guild)) {
				SendClientNotify("GoldNotEnough", -1, -1);
				return ;
			}
			addexp = 100;
		}
		else {
			//得到物品数量
			int32 itemcount = GetBagItemCount(20236004);
			if (itemcount <= 0)
			{
				SendClientNotify("CostItemNotEnough", -1, -1);
				return;
			}
			if (itemcount > 10)
			{
				itemcount = 10;
			}
			//扣除itemcount
			DelItemById(20236004, itemcount,Config::ItemDelLogType::DelType_Guild);
			addexp = itemcount*10;
		}
		if (practice == nullptr)
		{
			practice = boost::make_shared<Packet::PracticeInfo>();
			if (practice == nullptr)
			{
				SendClientNotify("UnknownError", -1, -1);
				return;
			}
			practice->set_playerguid(Guid());
			practice->set_id(id);
			practice->set_exp(addexp);
			practice->set_level(0);
			guild_practice_.push_back(practice);
		}
		else
		{
			practiceexp += addexp;
			practice->set_exp(practiceexp);
		}

		while (MtGuildManager::Instance().NeedPracticeLevelUp(practice->level(), practice->exp())) {
			
			bool ret = MtGuildManager::Instance().AddPracticeLevel(practice);
			if (ret == false)
			{
				break;
			}
			OnLuaFunCall_x("xOnAddPracticeLevel",
			{
				{ "value",practice->level() }
			});
		}
		MainActor()->OnBattleInfoChanged();
		Packet::GetPracticeReply reply;
		auto pr_ = reply.add_list();
		pr_->set_playerguid(Guid());
		pr_->set_id(id);
		pr_->set_level(practice->level());
		pr_->set_exp(practice->exp());
		SendMessage(reply);
		LOG_INFO << "AddPracticeExp playerid,"<<GetGuildID()<<",id,"<< id <<",exp,"<< practice->exp()<<",level,"<< practice->level();
		return ;
	}
	int32 MtPlayer::GetPracticeProperty(Packet::Property pro)
	{
		int32 attributevalue = 0;
		for (auto practice : guild_practice_)
		{
			boost::shared_ptr<Config::GuildPracticeConfig> config = MtGuildManager::Instance().GetPracticeConfig(practice->id());
			if (config == nullptr)
				continue;
			for (int32 i=0;i<config->attribute_size();i++)
			{
				if (config->attribute(i) == pro)
				{
					attributevalue += practice->level();
					break;								//单一性
				}
			}
		}
		return attributevalue;
	}

	void MtPlayer::ResetTrialFieldData()
	{
		if (datacell_container_ == nullptr)
			return;

		datacell_container_->set_data_32(Packet::CellLogicType::NumberCell, Packet::NumberCellIndex::HellDayWaveCount, 0, false);
		ResetPlayerActorLastHp(Packet::LastHpType::HellLastHp);
		DelPlayerNpc(GetParams64("target_guid"));
		DelNpcByType("hell_box");
		if (cur_trialinfo_ != nullptr) {
			cur_trialinfo_->mutable_target()->clear_targets();
		}
		cur_trialinfo_ = nullptr;
	}
	void MtPlayer::ResetTrialGBOSSData()
	{
		ResetPlayerActorLastHp(Packet::LastHpType::GBOSSLastHp);
	}
	void MtPlayer::ResetActivityData()
	{
		if (datacell_container_ == nullptr)
			return;

		//重置活跃度
		auto token_data = player_db_->mutable_token_data();
		if (token_data != nullptr)
		{
			token_data->set_dayvalue(0);

			Packet::CommonReply reply;
			reply.set_reply_name(Packet::TokenType_Name(Packet::TokenType::Token_Day));
			reply.add_int64_params(0);
			SendMessage(reply);
		}

		//刷新活动状态
		for (auto child : activities_){
			child.second->OnRefresh2Client();
		}
		//世界BOSS伤害清空
		worldboss_hp_ = 0;
	}
	void MtPlayer::OnCreateGuild(std::string createname, std::string createnotice)
	{
		auto union_fee_config = MtConfigDataManager::Instance().FindConfigValue("union_fee");
		if (union_fee_config == nullptr) {
			return ;
		}
		
		if (!DelItemById(Packet::TokenType::Token_Gold, union_fee_config->value1() * 10000, Config::ItemDelLogType::DelType_Guild)) {
			SendCommonResult(Packet::ResultOption::Guild_Opt, Packet::ResultCode::GoldNotEnough);
			return ;
		}
		Server::Instance().SendS2GCommonMessage("S2GCreateGuildReq", {}, {(int64)Guid()}, { createname ,createnotice });
		LOG_INFO << "createguild,0," << Guid() << "," << createname << "," << createnotice;
	}

	void MtPlayer::UpdateGuildInfo(std::string guildname, uint64 guildid,std::string log_from)
	{
		player_data_->set_guildid(guildid);
		player_data_->set_guildname(guildname);
		Packet::UpdatePlayerGuildInfo info;
		info.set_guildname(guildname);
		info.set_guildid(guildid);
		SendMessage(info);
		if (guildid == INVALID_GUID)
		{
			auto missionproxy = GetMissionProxy();
			if (missionproxy)
			{
				missionproxy->OnResetMission(Config::MissionType::UnionMission);
			}
		}
		
		OnLuaFunCall_x("xOnUpdateGuildInfo",
		{
			{ "value", -1 }
		});

		LOG_INFO << "UpdateGuildInfo,"<<Guid()<<","<< guildname<<","<< guildid<<","<< log_from;
	}
	uint64 MtPlayer::GetGuildID()
	{
		return GetScenePlayerInfo()->guildid();
	}
	std::string MtPlayer::GetGuildName()
	{
		return GetScenePlayerInfo()->guildname();
	}
	void MtPlayer::SetGuildName(std::string guildname)
	{
		GetScenePlayerInfo()->set_guildname(guildname);
	}
	MtPlayerGuildProxy* MtPlayer::GetPlayerGuildProxy()
	{
		if (player_guild_proxy_ == nullptr)
		{
			player_guild_proxy_ = boost::make_shared<MtPlayerGuildProxy>(weak_from_this());
		}
		return player_guild_proxy_.get();
	}
	
	void MtPlayer::StopMove()
	{
		auto stop_msg = boost::make_shared<Packet::ClientPlayerStop>();
		stop_msg->set_guid(Guid());
		stop_msg->set_direction(Direction());
		stop_msg->mutable_stop_pos()->CopyFrom(Position());
		if (scene_ != nullptr)
			scene_->OnClientStop(shared_from_this(), stop_msg, MessageSource::CLIENT);
	}

	bool MtPlayer::Moving() const
	{
		return player_data_->player_status() == Packet::PlayerSceneStatus::MOVING;
	}

	bool MtPlayer::BeLoot(uint64 looter_guid)
	{
		auto looter = scene_->FindPlayer(looter_guid);
		if (looter == nullptr) {
			return false;
		}
		Packet::LootNotify notify;
		notify.set_be_loot_player_guid(Guid());
		notify.set_be_loot_player_name(Name());
		notify.set_loot_player_guid(looter->Guid());
		notify.set_loot_player_name(looter->Name());
		auto current_gold = GetPlayerToken(Packet::TokenType::Token_Gold);
		auto loot_gold = 0;
		if (current_gold > 20) {
			loot_gold = int32(current_gold * 0.05);
		}
		notify.set_loot_coin_a(loot_gold);
		notify.set_loot_coin_b(loot_gold);

		//装备，道具二选一
		int32 seed = rand_gen->intgen(Packet::ContainerType::Layer_EquipBag, Packet::ContainerType::Layer_ItemBag);
		auto container = FindContainer((Packet::ContainerType)seed);
		if (container == nullptr) {
			return false;
		}
		auto& items = container->Items();
		std::random_shuffle(std::begin(items), std::end(items));
		for (auto i = 0; i < (int32)items.size() && i < 2; ++i) {
			auto item = items[i];
			notify.add_item_id(item->ConfigId());
			notify.add_item_count(1);
			if (item->Count() < 1) {
				return false;
			}
		}
		CostToken(Packet::TokenType::Token_Gold, loot_gold * 2, Config::ItemDelLogType::DelType_Loot,(int64)looter->Guid());
		DataCellContainer()->set_data_64(Packet::Cell64LogicType::LootCell64Type, Packet::LootCellIndex::UNBIND_COIN, loot_gold);
		DataCellContainer()->set_data_64(Packet::Cell64LogicType::LootCell64Type, Packet::LootCellIndex::LAST_BE_LOOT_TIME, now_millisecond());
		looter->DataCellContainer()->set_data_64(Packet::Cell64LogicType::LootCell64Type, Packet::LootCellIndex::LAST_LOOT_PLAYER_GUID, Guid());
		looter->DataCellContainer()->set_data_64(Packet::Cell64LogicType::LootCell64Type, Packet::LootCellIndex::LAST_LOOT_TIME, now_millisecond());
		looter->DataCellContainer()->add_data_64(Packet::Cell64LogicType::LootCell64Type, Packet::LootCellIndex::LOOT_COUNT_TODAY, 1);
		AddItemById(Packet::TokenType::Token_Gold, loot_gold, Config::ItemAddLogType::AddType_Loot, nullptr, (int64)Guid());

		for (auto i = 0; i < notify.item_id_size(); ++i) {
			DelItemById(notify.item_id(i), notify.item_count(i), Config::ItemDelLogType::DelType_Loot, looter->Guid());
			AddItemById(notify.item_id(i), notify.item_count(i), Config::ItemAddLogType::AddType_Loot, nullptr, (int64)Guid());
		}
		SendMessage(notify);
		looter->SendMessage(notify);
		return true;
	}

	bool MtPlayer::InLootProtect()
	{
		auto last_be_loot_time = DataCellContainer()->get_data_64(Packet::Cell64LogicType::LootCell64Type, Packet::LootCellIndex::LAST_BE_LOOT_TIME);
		if ((MtTimerManager::Instance().CurrentTime() - last_be_loot_time) < (uint64)hours(4).total_milliseconds()){
			return true;
		}

		//ProtectRobImpact
		if((&simple_impacts_)->OnCheckImpact(1003)) {
			return true;
		}
		else{
			return false;
		}
	}



	void MtPlayer::SetTeamArrangeType(Packet::ArrangeType type)
	{
		arrange_type_ = type;
	}

	void MtPlayer::CheckInit()
	{
		////计算一次属性
		//for (auto child : actors_) {
		//	child->OnBattleInfoChanged();
		//}


		if (player_db_->inited())
			return;

		player_db_->set_inited(true);
		InitActorEquip();
		InitMarket();
	}

	void MtPlayer::InitActorEquip()
	{
		std::for_each(std::begin(actors_), std::end(actors_), boost::bind(&MtActor::InitEquip, _1));
	}

	void MtPlayer::InitDataCell()
	{		
		OnLuaFunCall_x("xOnInitCell",
		{
			{ "value", -1 }
		});
		OnLuaFunCall_x("xOnResetCheck",
		{
			{ "value", -1 }
		});

		OnLuaFunCall_x("xOnTargetCheck",
		{
			{ "value", -1 }
		});
	}

	boost::shared_ptr<MtActor> MtPlayer::FindActorByCfgId(int32 config_id)
	{
		auto it = std::find_if(std::begin(actors_), std::end(actors_), boost::bind(&MtActor::ConfigId, _1) == config_id);
		if (it != std::end(actors_)) {
			return *it;
		}
		return nullptr;
	}

	boost::shared_ptr<MtActor> MtPlayer::FindActorByGuid(uint64 guid)
	{
		auto it = std::find_if(std::begin(actors_), std::end(actors_), boost::bind(&MtActor::Guid, _1) == guid);
		if (it != std::end(actors_)) {
			return *it;
		}
		return nullptr;
	}


	void MtPlayer::OnArenaBattleDone(bool win, MtBattleGround* battle_ground)
	{
		if (battle_ground != nullptr && battle_ground->GetBattleReply() != nullptr
			&& curr_arena_target_ != nullptr) {
			uint64 reply_guid = INVALID_GUID;
			if (battle_ground->GetBattleReply() != nullptr) {
				reply_guid = battle_ground->GetBattleReply()->guid();
			}
			MtArenaManager::Instance().OnChallengeDone(shared_from_this(),
				curr_arena_target_, reply_guid, win);
			curr_arena_target_ = nullptr;
		}
	}

	void MtPlayer::SavePlayerLastHp(const uint64 actor_guid, Packet::LastHpType type,const int32 hp)
	{
		for (auto child : actors_) {
			if (child->Guid() == actor_guid) {
				child->SetLastHp(type,hp);
			}
		}

		Refresh2ClientActorList();
	}
	void MtPlayer::SavePlayerNpcLastHp(const uint64 target_guid, const uint64 actor_guid, const int32 hp)
	{
		if (cur_trialinfo_ == nullptr)
			return;

		auto target_form = cur_trialinfo_->mutable_target();
		for (int i = 0; i < target_form->targets_size(); i++) {
			auto tplayer = target_form->mutable_targets(i);
			if (tplayer->player().guid() == target_guid) {
				for (int k = 0; k < tplayer->actors_size(); k++) {
					auto tactor = tplayer->mutable_actors(k);
					auto dbinfo = tactor->mutable_actor();
					if (dbinfo->guid() == actor_guid) {
						dbinfo->clear_last_hp();
						for (int32 j = 0; j < Packet::LastHpType::LastHpMax; j++) {
							dbinfo->add_last_hp(hp);
						}
					}
				}
			}
		}

		RefreshPlayerNpc(true);
	}

	bool MtPlayer::PlayerCanHook( int32 scene_stage_id )
	{
		//初始关卡特写!
		auto stage_config = MtSceneStageManager::Instance().GetConfig(scene_stage_id);
		if (stage_config == nullptr) {
			return false;
		}
		return PlayerLevel() >= stage_config->min_level();
	}
	
	bool MtPlayer::CanAutoHook()
	{
		return GetPlayerStageProxy()->CanAutoHook();
	}

	void MtPlayer::Cache2Save(google::protobuf::Message * msg)
	{
		if (msg == nullptr)
			return;

		MtShmManager::Instance().Cache2Shm(msg,Guid(), cache_stamp_);

	}

	Packet::ResultCode MtPlayer::MarketReq( const Config::MarketType type)
	{
		Packet::PlayerMarketReply message;
		auto market = FindMarket(type);
		if (market) {
			market->FillMessage(message);
			SendMessage(message);
			return Packet::ResultCode::ResultOK;
		}
		else {
			return Packet::ResultCode::InvalidMarketData;
		}
	}
	void MtPlayer::CheckCDKCode(std::string cdkcode)
	{
		Server::Instance().AddUrlTask(new CheckCDKCODEWork(Guid(), account_->partnerid(), cdkcode));
	}
	Packet::ResultCode MtPlayer::ItemLockReq( const int64 guid, bool lock)
	{
		auto item_base = FindItemByGuid(Packet::ContainerType::Layer_EquipAndItemBag,guid);
		if (item_base == nullptr) {
			return Packet::ResultCode::InvalidItem;
		}

		item_base->EnableLock(lock);
		item_base->RefreshData2Client();

		return Packet::ResultCode::ResultOK;
	}

	void MtPlayer::SetPlayerStageDbInfo(const boost::shared_ptr<Packet::PlayerStageDB>& data)
	{
		player_stage_db_ = data;
	}
	std::string MtPlayer::PlayerBibase()
	{
		char buffer[1024] = { 0 };
		zprintf(buffer, sizeof(buffer), "%s,%s,%d,%" PRIu64 ",%" PRIu64 ",%d,%d,%d", account_->userid().c_str(), account_->udid().c_str(), account_->partnerid(), player_db_->account_guid(), player_data_->guid(), Server::Instance().ServerId(), player_data_->level(), player_data_->init_actor_id());
		return buffer;
	}
	void MtPlayer::DBLog(std::string type,std::string log, int32 ud)
	{
		std::stringstream DBlog;
		DBlog << type<<"," << PlayerBibase() << "," << log;
		Server::Instance().AddUrlTask(new db_log_require_work(DBlog.str(), __FILE__, __LINE__));

		if (type == "ActivityTimes") {
			//mqdblopg 模块使用日志表
			std::string tstr = MtTimerManager::Instance().NowTimeString();
			std::string dstr = MtTimerManager::Instance().TimeString2DateString(tstr);
			boost::format fmt("INSERT INTO `module_use_log` (`ServerID`,`PartnerID`,`UserID`,`PlayerID`,`VipLevel`,`PlayerLevel`,`ModuleID`,`CrDate`,`CrTime`) VALUES('%d', '%d', '%s', '%llu', '%d', '%d', '%d', '%s', '%s'); ");
			fmt% MtConfig::Instance().server_group_id_%account_->partnerid() % account_->userid() % Guid() % GetAllCharge() % PlayerLevel() % ud % dstr %tstr;
			SendLogMessage(fmt.str());
		}
	}


	void MtPlayer::UpdateWebPlayerInfo(std::string type, bool Interday)
	{
		std::stringstream DBlog;
		DBlog << type<< "," << PlayerBibase() << "," <<Name()<<","<<player_db_->token_data().dayvalue() << "," << GetPlayerToken(Packet::Token_Crystal) << "," << GetPlayerToken(Packet::Token_Gold) << ",";
		for (int32 i = Packet::GuideCellIndex::GuideGroupStart; i < Packet::GuideCellIndex::GuideGroupEnd; i++) {
			DBlog << i << "_" << datacell_container_->get_data_32(Packet::CellLogicType::GuideCell, i) << "#";
		}
		DBlog << "," << Interday;
		Server::Instance().AddUrlTask(new db_log_require_work(DBlog.str(), __FILE__, __LINE__));
	}
	int32 MtPlayer::BattleScore() const
	{
		return player_data_->battle_score();
	}

	int32 MtPlayer::PowerCount()
	{
		int32 ret = OnLuaFunCall_0(100, "GetPowerCount");
		int32 battle_score = this->BattleScore();
		if (ret < battle_score) {
			return battle_score;
		} else {
			return ret;
		}
	}
	
	void MtPlayer::UpdateBattleScore(bool init)
	{
		//std::vector<int32> scores;
		//scores.reserve(actors_.size());
		//for (auto& actor : actors_) {
		//	if (actor->ActorType() != Packet::ActorType::Main) scores.push_back(actor->Score());
		//}
		//std::sort(scores.begin(), scores.end(), std::greater<uint64>());
		//int32 count = std::minmax(int32(scores.size()), 4).first;
		//int32 sum = std::accumulate(scores.begin(), scores.begin() + count, 0);
		//sum += MainActor()->Score();

		auto hook_sum_score = GetActorFormationPower(Packet::ActorFigthFormation::AFF_NORMAL_HOOK);
		auto pvp_guard_score = GetActorFormationPower(Packet::ActorFigthFormation::AFF_PVP_GUARD);
		player_data_->set_battle_score(hook_sum_score);
		boost::for_each(player_ai_, boost::bind(&PlayerAi::OnPlayerScoreChanged, _1));
		MtTopRankManager::Instance().OnPlayerScoreChanged(shared_from_this());
		MtArenaManager::Instance().OnPlayerScoreChanged(shared_from_this(), pvp_guard_score);
		MtTopRankManager::Instance().OnMainActorScoreChanged(shared_from_this());

		//更新客户端战力
		SendLuaPacket("BattleScore", { hook_sum_score }, {}, {});

		if (scene_ && !init) {
			
			OnLuaFunCall_x("xUpdateBattleScore",
			{
				{ "value", hook_sum_score }
			});
		}
	}

	bool MtPlayer::DispatchMessage(const boost::shared_ptr<google::protobuf::Message>& msg)
	{
		std::lock_guard<std::mutex> g(client_msgs_mutext_);
		client_msgs_.push({MessageSource::CLIENT, msg});
		return true;
	}

	bool MtPlayer::LuaDispatchMessage(const google::protobuf::Message* msg)
	{
		if (msg != nullptr) {
			boost::shared_ptr<google::protobuf::Message> m = nullptr;
			m.reset(const_cast<google::protobuf::Message*>(msg));
			return DispatchMessage(m);
		}
		return false;
	}


	bool MtPlayer::ExecuteMessage(const boost::shared_ptr<google::protobuf::Message>& msg)
	{
		std::lock_guard<std::mutex> g(client_msgs_mutext_);
		client_msgs_.push({ MessageSource::GAME_EXECUTE, msg });
		return true;
	}


	bool MtPlayer::LuaExecuteMessage(const google::protobuf::Message* msg)
	{
		if (msg != nullptr) {
			boost::shared_ptr<google::protobuf::Message> m = nullptr;
			m.reset(const_cast<google::protobuf::Message*>(msg));
			return ExecuteMessage(m);
		}
		return false;
	}

	bool MtPlayer::HandleMessage()
	{
		for (auto i = 0; i < 10 && !client_msgs_.empty(); ++i) {//每次tick最多处理10个消息
			std::pair<int32, boost::shared_ptr<google::protobuf::Message>> pair;
			{
				std::lock_guard<std::mutex> g(client_msgs_mutext_);
				pair = client_msgs_.front();
				client_msgs_.pop();
			}
			if (scene_ == nullptr) {
				LOG_WARN << "handle message can not found scene:" << Guid()
					<< ". data_scene_id:" << player_data_->scene_id();
				return false;
			}
			if (pair.first == MessageSource::CLIENT) {
				bool ret = scene_->HandlePlayerMessage(shared_from_this(), pair.second, pair.first);
				if (ret == false) //消息需要等待下个tick执行
					break;
			}
			else if (pair.first == MessageSource::GAME_EXECUTE) {
				bool ret = scene_->HandleInnerMessage(shared_from_this(), pair.second, pair.first);
				if (ret == false) //消息需要等待下个tick执行
					break;
			}

		}
		return true;
	}


	boost::shared_ptr<MtArrangeProxy>& MtPlayer::GetArrangeProxy() {
		if (arrange_proxy_ == nullptr) {
			arrange_proxy_ = boost::make_shared<MtArrangeProxy>(*this);
		}
		return arrange_proxy_;
	};
	//mission begin
	boost::shared_ptr<MtMissionProxy>& MtPlayer::GetMissionProxy() {
		if (mission_proxy_ == nullptr) {
			mission_proxy_ = boost::make_shared<MtMissionProxy>(*this);
		}
		return mission_proxy_;
	}

	MtPlayerStageProxy* MtPlayer::GetPlayerStageProxy()
	{
		if (player_stage_proxy_ == nullptr) {
			player_stage_proxy_ = boost::make_shared<MtPlayerStageProxy>(weak_from_this());
		}
		return player_stage_proxy_.get();
	}

	bool MtPlayer::StartHookBattle(int32 stage_id, int32 group_id, 
		int32 insight, int32 send_start_notify)
	{
		auto stage_config = MtSceneStageManager::Instance().GetConfig(stage_id);
		if (stage_config == nullptr) {
			return false;
		}
		if (stage_config->normal_monsters_size() <= 0) {
			return false;
		}

		if (hook_battle_ != nullptr) {
			hook_battle_->OnBattleEnd(true,Guid());
		}
		auto battle = scene_->GetBattleManager().CreateHookBattle(this, {
			{ "script_id",1002 },
			{ "stage_id",stage_id },
			{ "group_id",group_id },
			{ "insight", insight },
			{ "send_start_notify", send_start_notify},
			{ "battle_type" , Packet::BattleGroundType::PVE_HOOK },
		});

		if (battle == nullptr) {
			LOG_ERROR << "start simulate fail with scene_id: " << scene_->SceneId() << ", stage_id:" << stage_id;
			return false;
		}
		hook_battle_ = battle;
		return true;
	}

	void MtPlayer::LogoutStatus(eLogoutStatus status) {
		if (logout_status_ != eCanLogout && status == eCanLogout) {
			//logout消息,不能在tick里logout，需要通过消息!
			auto message = boost::make_shared<Packet::LuaRequestPacket>();
			message->set_request_name("Logout");
			DispatchMessage(message);

			//通知主线程
			//Server::Instance().SendS2GCommonMessage("S2GLogout", {}, { (int32)Guid() }, {});
		}
		logout_status_ = status;
	}

	void MtPlayer::OnReSetHookData() const
	{
		boost::for_each(player_ai_, boost::bind(&PlayerAi::OnReSetHookData, _1));
	}

	void MtPlayer::AddChallengReq(uint64 guid)
	{
		if (pvp_fight_challengers_.size() >= 3) {
			decltype(pvp_fight_challengers_) pop_one(pvp_fight_challengers_.begin(), pvp_fight_challengers_.begin() + 2);
			pop_one.push_back(guid);
			pvp_fight_challengers_.swap(pop_one);
		} else {
			pvp_fight_challengers_.push_back(guid);
		}
	}

	bool MtPlayer::CheckChallengReq(uint64 guid) const
	{
		if (std::find(pvp_fight_challengers_.begin(),
			pvp_fight_challengers_.end(), guid) != pvp_fight_challengers_.end())
			return true;
		return false;
	}

	void MtPlayer::SetPlayerSetting(const boost::shared_ptr<Packet::PlayerSetting>& setting)
	{
		settings_ = setting;
	}

	Packet::PlayerSetting* MtPlayer::GetPlayerSetting() const
	{
		return settings_.get();
	}

	void MtPlayer::OnLeaveScene()
	{
		boost::for_each(player_ai_, boost::bind(&PlayerAi::OnLeaveScene, _1));
	}

	google::protobuf::RepeatedPtrField<Packet::RTHC>* MtPlayer::GetRealTimeHookData()
	{
		if (player_stage_db_ == nullptr)
			return nullptr;
		return player_stage_db_->mutable_configs();
	}

	void MtPlayer::Refresh2ClientStageInfo()
	{
		GetPlayerStageProxy()->SyncSpecStageToClient(-1);
	}

	void MtPlayer::QuitHookBattle()
	{
		if (hook_battle_ != nullptr) {
			if (hook_battle_ != nullptr) {
				hook_battle_->OnBattleEnd(true, Guid());
			}
		}
	}

	Packet::PlayerStage* MtPlayer::GetPlayerStage(int32 stage_id) const
	{
		auto it = std::find_if(stages_.begin(), stages_.end(),
			boost::bind(&Packet::PlayerStage::id, _1) == stage_id);
		if (it == stages_.end()) {
			return nullptr;
		} else {
			return (*it).get();
		}
	}

	void MtPlayer::AddPlayerStage(int32 stage_id)
	{
		auto it = std::find_if(stages_.begin(), stages_.end(),
			boost::bind(&Packet::PlayerStage::id, _1) == stage_id);
		if (it == stages_.end()) {
			auto stage = boost::make_shared<Packet::PlayerStage>();
			stage->SetInitialized();
			stage->set_player_guid(Guid());
			int32 den = MtSceneStageManager::Instance().GetStageDefaultDen(stage_id);
			stage->set_den(den);
			stage->set_id(stage_id);
			stage->set_raid(false);
			stage->set_boss(0);
			stages_.push_back(stage);
			LOG_INFO << "player:" << Guid() << " add stage ok! " << stage_id;
		} else {
			LOG_ERROR << "player:" << Guid() << " add duplicate stage:" << stage_id;
		}
	}

	const std::vector<boost::shared_ptr<Packet::PlayerStage>>& MtPlayer::GetPlayerStages() const
	{
		return stages_;
	}

	int32 MtPlayer::GetPlayerMaxStageId() const
	{
		return stages_[stages_.size()-1]->id();
	}

	Packet::PlayerStageDB* MtPlayer::GetPlayerStageDB() const
	{
		return player_stage_db_.get();
	}

	void MtPlayer::LoadPlayerStage(const std::vector<boost::shared_ptr<Packet::PlayerStage>>& stages)
	{
		stages_ = stages;
	}

	MtPlayer::~MtPlayer()
	{
		LOG_INFO << "[mt player] destroy player :" << this;
	}
	bool MtPlayer::CreateRobotTeam(int32 group)
	{
		if (GetTeamID() == INVALID_GUID) {
			auto add_req = boost::make_shared<Packet::CreateRobotTeam>();
			auto one_key_ids = MtActorConfig::Instance().TeamRobotIds(group);
			for (std::size_t i = 0; i < one_key_ids.size(); ++i) {
				auto one_key_id = one_key_ids[i];
				auto one_key_config = MtActorConfig::Instance().GetOneKeyConfig(one_key_id);
				if (one_key_config == nullptr) continue;;

				auto& basic_info = *add_req->add_basic_info();
				add_req->add_one_key_id(one_key_id);
				basic_info.CopyFrom(*player_data_);
				basic_info.set_guid(Guid() + i + 1);
				basic_info.set_init_actor_id(one_key_config->hero_id());
				basic_info.set_name(one_key_config->display_name());
				basic_info.set_icon(one_key_config->icon());
				basic_info.set_hair(one_key_config->hair());
				basic_info.set_level(one_key_config->level());
			}

			if (add_req->basic_info_size() > 0) {
				Server::Instance().SendMessage(add_req, shared_from_this());
				return true;
			}
		}
		return false;
	}

	int32 MtPlayer::CurrentRobotCount() const
	{
		return robots_.size();
	}

	bool MtPlayer::DismissTeam()
	{
		if (GetTeamID() == INVALID_GUID) {
			return false;
		}
		if (session_ != nullptr) {
			auto req = boost::make_shared<Packet::LevelTeamReq>();
			Server::Instance().FakeClientMessage(session_, req);
		}

		OnLuaFunCall_x("xRobotTeamDismiss",
		{
			{ "value",-1 }
		});
		return true;
	}

	void MtPlayer::OnRobotPlayerInOk(const boost::shared_ptr<Packet::RobotPlayerInOk>& msg)
	{
		if (msg->basic_info_size() != msg->one_key_id_size()) {
			LOG_ERROR << "[player] robot team create error, dismatch player size and one key id";
		}
		for (int32 i = 0; i < msg->basic_info_size() && i < msg->one_key_id_size(); ++i){
			auto& robot_basic_info = msg->basic_info(i);
			auto one_key_id = msg->one_key_id(i);
			if (robots_.size() < 9) {
				MtRobot robot(robot_basic_info);
				MtActorConfig::Instance().OnekeyActor2FullInfo(robot.actor_full_, one_key_id);
				robots_.push_back(robot);

				//通知目标客户端,有一个玩家上线了
				Packet::ScenePlayerInfo notify;
				notify.mutable_basic_info()->CopyFrom(robot_basic_info);
				notify.mutable_main_equips()->CopyFrom(robot.actor_full_.equips());
				SendMessage(notify);
			}
		}

		OnLuaFunCall_x("xRobotTeamOk",
		{
			{ "value",-1 }
		});
	}

	void MtPlayer::FillRobotPlayer(Packet::ScenePlayerList& list)
	{
		for (auto& robot : robots_) {
			auto spi = list.add_player_list();
			spi->mutable_basic_info()->CopyFrom(robot.basic_info_);
			spi->mutable_main_equips()->CopyFrom(robot.actor_full_.equips());
		}
	}

	bool MtPlayer::OnRobotMove(const boost::shared_ptr<Packet::ClientPlayerMove>& message)
	{
		auto it = std::find_if(robots_.begin(), robots_.end(),
			boost::bind(&MtRobot::Guid, _1) == message->guid());
		if (it == robots_.end())
			return false;

		auto& robot = *it;
		robot.basic_info_.mutable_scene_pos()->CopyFrom(message->new_pos());
		robot.basic_info_.set_direction(message->direction());
		return true;
	}

	bool MtPlayer::ResetRobotPos()
	{
		for (auto& robot : robots_) {
			robot.basic_info_.set_scene_id(player_data_->scene_id());
			robot.basic_info_.mutable_scene_pos()->CopyFrom(player_data_->scene_pos());
			robot.basic_info_.set_direction(player_data_->direction());
		}
		return true;
	}

	int32 MtPlayer::GuildPracticeLevelSum() const
	{
		return std::accumulate(guild_practice_.begin(),
			guild_practice_.end(), 0, [](auto acce, auto& prac) {
			return acce + prac->level();
		});
	}

	void MtPlayer::EnterSceneSync()
	{
		Refresh2ClientNpcList();
	}

	MtRobot::MtRobot(const Packet::PlayerBasicInfo& basic_info)
	{
		basic_info_.CopyFrom(basic_info);
		actor_full_.mutable_actor()->set_guid(basic_info_.guid());
		actor_full_.mutable_actor()->set_player_guid(basic_info_.guid());
	}

}
