#include "mt_lua_initializer.h"
#include "module.h"
#include "../mt_config/mt_config.h"
#include "../mt_server/mt_server.h"
#include "../mt_scene/mt_scene_manager.h"
#include "../mt_arena/mt_arena.h"
#include "../mt_mission/mt_mission_proxy.h"
#include "../mt_data_cell/mt_data_cell.h"
#include "../mt_data_cell/mt_data_container.h"
#include "../mt_item/mt_container.h"
#include "../mt_item/mt_item.h"
#include "../mt_item/mt_item_package.h"
#include "../mt_player/mt_player.h"
#include "../mt_skill/mt_buff.h"
#include "../mt_mission/mt_mission.h"
#include "../mt_monster/mt_monster_manager.h"
#include "../mt_monster/mt_npc.h"
#include "../mt_scene/mt_scene.h"
#include "../mt_guild/mt_guild_manager.h"
#include "../mt_guild/mt_player_guild_proxy.h"
#include "../mt_team/mt_team_manager.h"
#include "../mt_activity/mt_activity.h"
#include "../mt_battle/mt_battle_ground.h"
#include "../mt_stage/mt_scene_stage.h"
#include "../mt_stage/mt_player_stage.h"
#include "../mt_scene/mt_raid.h"
#include "../mt_actor/mt_actor.h"
#include "../mt_battle_field/mt_battle_field_manager.h"
#include "../mt_mission/mt_arrange_proxy.h"
#include "../mt_battle/mt_worldboss_manager.h"
#include "../mt_skill/mt_skill.h"
#include "../mt_battle_field/mt_battle_field.h"
#include "../mt_capture_point/mt_capture_point.h"
#include "../mt_market/mt_market.h"
#include "../mt_activity/mt_draw.h"
#include "../mt_cache/mt_shm_manager.h"
#include "../friend/FriendContainer.h"
#include "../mt_trial_field/mt_tower.h"
#include "../friend/mt_friend_manager.h"
#include "../mt_chat/mt_chatmessage.h"
#include "../mt_chat/mt_word_manager.h"
#include "../mail/static_message_manager.h"
#include "../mail/MessageContainer.h"
#include "../mt_activity/mt_operative_config.h"
#include "../data/luafile_manager.h"

#include <ServerCommon.proto.fflua.h>
#include <MissionConfig.proto.fflua.h>
#include <MissionPacket.proto.fflua.h>
#include <GuildCapturePoint.proto.fflua.h>
#include <ItemAndEquip.proto.fflua.h>
#include <EquipmentConfig.proto.fflua.h>
#include <MonsterConfig.proto.fflua.h>
#include <ActorBasicInfo.proto.fflua.h>
#include <Upgrades.proto.fflua.h>
#include <BattleGroundInfo.proto.fflua.h>
#include <SceneStageConfig.proto.fflua.h>
#include <ItemConfig.proto.fflua.h>
#include <SceneObject.proto.fflua.h>
#include <SceneConfig.proto.fflua.h>
#include <GuildConfig.proto.fflua.h>
#include <BaseConfig.proto.fflua.h>
#include <BattleReply.proto.fflua.h>
#include <Team.proto.fflua.h>
#include <DataCellPacket.proto.fflua.h>
#include <ActivityPacket.proto.fflua.h>
#include <ActivityConfig.proto.fflua.h>
#include <DrawCard.proto.fflua.h>
#include <Guild.proto.fflua.h>
#include <TrialFieldDB.proto.fflua.h>
#include <BattleField.proto.fflua.h>
#include <BattleInfo.proto.fflua.h>
#include <SceneCommon.proto.fflua.h>
#include <SimpleImpact.proto.fflua.h>
#include <PackageConfig.proto.fflua.h>
#include <WorldBoss.proto.fflua.h>
#include <S2GMessage.proto.fflua.h>
#include <S2SMessage.proto.fflua.h>
#include <G2SMessage.proto.fflua.h>
#include <SceneConfig.proto.fflua.h>
#include <ActorConfig.proto.fflua.h>
#include <MarketConfig.proto.fflua.h>
#include <FriendMail.proto.fflua.h>
namespace Mt
{
	static MtLuaInitializer* __mt_lua_initializer = nullptr;
	REGISTER_MODULE(MtLuaInitializer)
	{
		require("data_manager");
	}

	MtLuaInitializer& MtLuaInitializer::Instance()
	{
		return *__mt_lua_initializer;
	}

	void MtLuaInitializer::GlobalDataInit(ff::fflua_t& ff_lua) const
	{
		if (!ff_lua.fflua_init(true, MtConfig::Instance().script_data_dir_,"script_config.lua")){
			//不让起服
			exit(1);
		};
		ff::fflua_register_t<google::protobuf::Message, int()>(ff_lua.get_lua_state(),
			"google::protobuf::Message")
			.def(&google::protobuf::Message::CopyFrom, "CopyFrom")
			.def(&google::protobuf::Message::Clear, "Clear")
			.def(&google::protobuf::Message::SetInitialized, "SetInitialized");
		//! 注册全局函数
		ff_lua.reg(chatmessage_lua_reg);
		//! 注册C++ 对象到lua中
		ff_lua.reg(BuffImpl::Buff::lua_reg);
		ff_lua.reg(Packet::ServerCommon_fflua_regist_all);
		ff_lua.reg(Config::ActorConfig_fflua_regist_all);
		ff_lua.reg(Config::MissionConfig_fflua_regist_all);
		ff_lua.reg(Packet::MissionPacket_fflua_regist_all);
		ff_lua.reg(Packet::ItemAndEquip_fflua_regist_all);
		ff_lua.reg(Packet::PlayerBasicInfo_fflua_regist_all);
		ff_lua.reg(Packet::BattleReply_fflua_regist_all);
		ff_lua.reg(Packet::BattleInfo_fflua_regist_all);
		ff_lua.reg(Config::MonsterConfig_fflua_regist_all);
		ff_lua.reg(Packet::ActorBasicInfo_fflua_regist_all);
		ff_lua.reg(Packet::BattleGroundInfo_fflua_regist_all);
		ff_lua.reg(Config::SceneStageConfig_fflua_regist_all);
		ff_lua.reg(Config::ItemConfig_fflua_regist_all);
		ff_lua.reg(Packet::SceneObject_fflua_regist_all);
		ff_lua.reg(Config::SceneConfig_fflua_regist_all);
		ff_lua.reg(Packet::Base_fflua_regist_all);
		ff_lua.reg(Packet::Team_fflua_regist_all);
		ff_lua.reg(Packet::DataCellPacket_fflua_regist_all);
		ff_lua.reg(Packet::ActivityPacket_fflua_regist_all);
		ff_lua.reg(Config::ActivityConfig_fflua_regist_all);
		ff_lua.reg(DB::TrialFieldDB_fflua_regist_all);
		ff_lua.reg(Packet::BattleField_fflua_regist_all);
		ff_lua.reg(Packet::Guild_fflua_regist_all);
		ff_lua.reg(Config::GuildConfig_fflua_regist_all);
		ff_lua.reg(Packet::SceneCommon_fflua_regist_all);
		ff_lua.reg(Packet::SimpleImpact_fflua_regist_all);
		ff_lua.reg(Config::PackageConfig_fflua_regist_all);
		ff_lua.reg(Packet::WorldBoss_fflua_regist_all);
		ff_lua.reg(Config::BaseConfig_fflua_regist_all);
		ff_lua.reg(Config::EquipmentConfig_fflua_regist_all);
		ff_lua.reg(Packet::BattleExpression_fflua_regist_all);
		ff_lua.reg(S2G::S2GMessage_fflua_regist_all);
		ff_lua.reg(S2S::S2SMessage_fflua_regist_all);
		ff_lua.reg(G2S::G2SMessage_fflua_regist_all);
		ff_lua.reg(Config::SceneConfig_fflua_regist_all);
		ff_lua.reg(Packet::GuildCapturePoint_fflua_regist_all);

		ff_lua.reg(MtLuaCallProxy::lua_reg);
		ff_lua.reg(MtConfig::lua_reg);

		ff_lua.reg(MtTowerManager::lua_reg);
		ff_lua.reg(MtBattleGroundManager::lua_reg);
		ff_lua.reg(MtBattleGround::lua_reg);
		ff_lua.reg(MtSceneManager::lua_reg);
		ff_lua.reg(MtActor::lua_reg);
		ff_lua.reg(MtMonsterManager::lua_reg);
		ff_lua.reg(Config::MarketConfig_fflua_regist_all);
		ff_lua.reg(Packet::FriendMail_fflua_regist_all);
		ff_lua.reg(Config::DrawCard_fflua_regist_all);
		ff_lua.reg(DateStruct::lua_reg);
		ff_lua.reg(OperativeInstance::lua_reg);
		ff_lua.reg(MtScene::lua_reg);
		ff_lua.reg(Server::lua_reg);
		ff_lua.reg(MtActivityManager::lua_reg);
		ff_lua.reg(luafile_manager::lua_reg);
		ff_lua.reg(MtTimerManager::lua_reg);
		ff_lua.reg(MtBattleFieldManager::lua_reg);
		ff_lua.reg(MtTeamManager::lua_reg);
		ff_lua.reg(MtPlayer::lua_reg);
		ff_lua.reg(MtBattleField::lua_reg);
		ff_lua.reg(MtGuildManager::lua_reg);
		ff_lua.reg(GuildInfo::lua_reg);
		ff_lua.reg(GuildBattle::lua_reg);
		ff_lua.reg(MtPlayerGuildProxy::lua_reg);
		ff_lua.reg(MtCapturePointManager::lua_reg);
		ff_lua.reg(MtItemPackage::lua_reg);
		ff_lua.reg(MtItemPackageManager::lua_reg);
		ff_lua.reg(MtConfigDataManager::lua_reg);
		ff_lua.reg(MtItemManager::lua_reg);
		ff_lua.reg(MtMarketManager::lua_reg);
		ff_lua.reg(MtMissionManager::lua_reg);
		ff_lua.reg(MtSceneStageManager::lua_reg);
		ff_lua.reg(MtDrawManager::lua_reg);
		ff_lua.reg(FriendContainer::lua_reg);
		ff_lua.reg(MtFriendManager::lua_reg);
		ff_lua.reg(PlayerZone::lua_reg);
		ff_lua.reg(MtWordManager::lua_reg);
		ff_lua.reg(static_message_manager::lua_reg);
		ff_lua.reg(MtOperativeConfigManager::lua_reg);
		ff_lua.reg(Packet::SceneStage_fflua_regist_all);
		ff_lua.reg(MessageContainer::lua_reg);
		//
		ff_lua.reg(MtDataCell::lua_reg);
		ff_lua.reg(MtData64Cell::lua_reg);
		ff_lua.reg(MtCellContainer::lua_reg);
		ff_lua.reg(luafile_manager::lua_reg);

		//测试用
		ff_lua.reg(MtShmManager::lua_reg);
	}

	void MtLuaInitializer::SceneDataInit(ff::fflua_t& ff_lua) const
	{
		GlobalDataInit(ff_lua);
		ff_lua.reg(MtArenaManager::lua_reg);
		ff_lua.reg(MtMissionProxy::lua_reg);
		ff_lua.reg(MtArrangeProxy::lua_reg);
		ff_lua.reg(MtContainer::lua_reg);		
		ff_lua.reg(MtMission::lua_reg);		
		ff_lua.reg(MtScene::lua_reg);		
		ff_lua.reg(MtActorConfig::lua_reg);
		ff_lua.reg(MtRaid::lua_reg);
		ff_lua.reg(MtSimpleImpactSet::lua_reg);
		ff_lua.reg(MtPlayerSimpleImpactSet::lua_reg);
		ff_lua.reg(MtActorSimpleImpactSet::lua_reg);
		ff_lua.reg(MtWorldBossManager::lua_reg);
		ff_lua.reg(MtPlayerStageProxy::lua_reg);
		ff_lua.reg(MtActivity::lua_reg);
		ff_lua.reg(MtNpc::lua_reg);		
		
		ff_lua.reg(MtItemBase::lua_reg);	
		ff_lua.reg(MtNormalItem::lua_reg);
		ff_lua.reg(MtEquipItem::lua_reg);
		ff_lua.reg(MtGemItem::lua_reg);
		ff_lua.reg(CMyTimer::lua_reg);
		ff_lua.reg(MtSkillManager::lua_reg);
		ff_lua.reg(MtMarket::lua_reg);
		ff_lua.reg(MtActorSkill::lua_reg);
		ff_lua.reg(G2S::G2SMessage_fflua_regist_all);
		ff_lua.reg(Packet::CG_GemFusion_fflua_reg);		
	}

	void MtLuaInitializer::ServerDataInit(ff::fflua_t& ff_lua) const
	{
		GlobalDataInit(ff_lua);
	}
	
}