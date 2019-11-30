
local  activity_type = ActivityType.GuildWetCopy

function x001021_OnJoinCheck(player,params)

	return ResultCode_ResultOK
end
function x001021_OnBattleCheck(player,params)
	return ResultCode_ResultOK
end


function x001021_OnGoBattle(battle_ground,params)
	
	local player = battle_ground:GetCreator()
	if player == nil then
		return ResultCode_BattleCreateFailed
	end

	local battlegroup = LuaSceneManager():GetSceneBattleGroupById(player:GetSceneId(),params["group_id"]);
	if battlegroup == nil then
		return ResultCode_InvalidConfigData
	end
	
	--local old_battle = player:BattleGround()
	--if old_battle ~= nil then
	--	if old_battle:BattleType() ~= BattleGroundType_PVE_HOOK then
	--		return ResultCode_Wrong_Status
	--	end
	--	player:QuitBattle()
	--end
	
	player:StopMove()	

	battle_ground:SetScriptId(1021)			
	
	local brief = battle_ground:Brief()
	brief:set_scene_id(player:GetSceneId())
	brief:set_battle_group_id(params["group_id"])		
	brief:set_type(BattleGroundType_PVE_SINGLE)
	
	if battle_ground:Init() == false then
		return ResultCode_BattleCreateFailed
	end		
		
	battle_ground:BattleEnterNotify()
	
	return ResultCode_ResultOK
end

function x001021_OnCreateReady(battle_ground, player)
	battle_ground:CreateAttackerPlayerBattleActors(false,false,ActorFigthFormation_AFF_PVE_GUILD_COPY)
	local copyid = player:GetPlayerGuildProxy():GetWetcopyCopyId();
	local wetcopystageid = player:GetPlayerGuildProxy():GetWetcopyStageId();
	local wetcopystageconfig = LuaMtGuildManager():GetGuildWetCopyStage(wetcopystageid);
	if wetcopystageconfig == nil then
		return ResultCode_UnknownError;
	end
	local monster_group_id = wetcopystageconfig:monster_id()
	local monster_group_config = LuaMtMonsterManager():FindMonsterGroup(monster_group_id);
	if monster_group_config == nil then
		return ResultCode_UnknownError;
	end

	local infos={}
	for i = 0,monster_group_config:monsters_size() - 1 do
		local monster_define = monster_group_config:monsters(i)
		if (monster_define:id() > 0) then
			infos[monster_define:loc()]=monster_define:id()
		end
	end
	battle_ground:CreateDefenderMonster(infos)
	local defenders = battle_ground:GetDefenders()
	for i,actor in pairs(defenders) do
		actor:SetHp(player:GetPlayerGuildProxy():GetWetcopyMonsterHp(i-1))
	end
	
	return ResultCode_ResultOK;
end

function x001021_OnDamageRefix(battle_ground,damage_info,killer)

end

function x001021_OnDamage(battle, attacker, source_guid, target_guid, damage)
	return 0;
end

function x001021_OnCheckBattleEnd(battle_ground)
	if battle_ground:TimeOut() then
		return 2;
	end
	local DeadCampID=battle_ground:GetAllDeadCampID();
	if DeadCampID == 1 then
		return 2;
	elseif DeadCampID == 2 then
		return 1;
	end
	return 0;
end

function x001021_OnBattleReward(battle_ground, winnercamp)
	return true
end

function x001021_OnBattleEnd(battle_ground,force, winnercamp)
	local creater = battle_ground:GetCreator();
	if creater == nil then
		return true
	end
	local raid = creater:GetRaid()
	if raid ~= nil then
		raid:SetRaidTimer(g_raid_timer.common_destroy,5,-1)
	end
	local copyid = creater:GetPlayerGuildProxy():GetWetcopyCopyId();
	local wetcopystageid = creater:GetPlayerGuildProxy():GetWetcopyStageId();
	local wetcopystage = LuaMtGuildManager():GetGuildWetCopyStage(wetcopystageid);
	local monster_group_id = wetcopystage:monster_id()
	local monster_group_config = LuaMtMonsterManager():FindMonsterGroup(monster_group_id);
	if monster_group_config == nil then
		return true;
	end
	
	local damage = battle_ground:GetStatisticsDamage();
	local defenders = battle_ground:GetDefenders()
	local intparms = {copyid,damage}
	for i,actor in pairs(defenders) do
		local monsterid=actor:ConfigId();
		for i = 0,monster_group_config:monsters_size() - 1 do
			local monster_define = monster_group_config:monsters(i)
			if monster_define:id() == monsterid then
				table.insert(intparms,actor:Hp());
			end
		end
	end
	
	creater:GetPlayerGuildProxy():ResetWetcopy();
	LuaServer():SendS2GCommonMessage("S2GEndGuildWetCopy",intparms,{creater:Guid()},{})	--同步到主线程
	
	--通知客户端结束战斗
	battle_ground:BattleOverNotify(winnercamp)
	return true
end
