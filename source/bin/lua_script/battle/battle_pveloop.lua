function x001002_OnJoinCheck(player,params)
	return ResultCode_ResultOK
end

function x001002_OnBattleCheck(player,params)
	return ResultCode_ResultOK
end

function x001002_OnGoBattle(battle_ground,params)
	
	local player = battle_ground:GetCreator()
	if player == nil then
		return ResultCode_BattleCreateFailed
	end
		
	if player:PlayerCanHook(params["stage_id"]) == false then
		return ResultCode_LevelLimit
	end
	
	if player:GetPlayerStageProxy():CanRaidStage(params["stage_id"]) == false then
		return ResultCode_InvalidSceneStage
	end

	local battlegroup = LuaSceneManager():GetSceneBattleGroupById(player:GetSceneId(),params["group_id"]);
	if battlegroup == nil then
		return ResultCode_InvalidConfigData
	end
	if params["insight"] ~= nil and params["insight"] == 0 then
		battle_ground:SetClientInsight(false)
	end
	player:StopMove()	
	battle_ground:SetScriptId(1002)		
	
	local brief = battle_ground:Brief()
	brief:set_scene_id(player:GetSceneId())
	brief:set_battle_group_id(params["group_id"])		
	brief:set_stage_id(params["stage_id"])
	if params["battle_type"] ~= nil then
		brief:set_type(params["battle_type"])
	else
		brief:set_type(BattleGroundType_PVE_HOOK)
	end
		
	if battle_ground:Init() == false then
		return ResultCode_BattleCreateFailed
	end		
	if battle_ground:GetClientInsight() == true and params["send_start_notify"] == 1 then	
		--print("kkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkk")
		player:SetOnHook(true)
		player:SendCommonResult(ResultOption_OnHook_Opt,ResultCode_HookStartOK)
	end
	battle_ground:BattleEnterNotify()
	player:GetPlayerStageProxy():SyncEffe2Client()
	return ResultCode_ResultOK
end

function x001002_OnCreateReady(battle_ground, player)
	
	battle_ground:CreateAttackerPlayerBattleActors(false,false,ActorFigthFormation_AFF_NORMAL_HOOK)

	local stage_id = battle_ground:Brief():stage_id()	
	local config = LuaMtSceneStageManager():GetConfig(stage_id)
	if config:normal_monsters_size() <=0 then
		return ResultCode_InvalidConfigData
	end
	
	local old_tracker = player:GetDBPlayerInfo():hook_stage_tracker()
	local monster_group_id_index = math.fmod(old_tracker,config:normal_monsters_size())

	local monster_group_id = config:normal_monsters(monster_group_id_index)
	local monster_group_config = LuaMtMonsterManager():FindMonsterGroup(monster_group_id)
	if monster_group_config == nil then
		return ResultCode_InvalidConfigData
	end
			
	old_tracker=old_tracker+1
	player:GetDBPlayerInfo():set_hook_stage_tracker((math.fmod(old_tracker,100) ));
	
	local infos={}
	for i = 0,monster_group_config:monsters_size() - 1 do
		local monster_define = monster_group_config:monsters(i)
		if (monster_define:id() > 0) then
			infos[monster_define:loc()]=monster_define:id()
		end
	end
	
	battle_ground:CreateDefenderMonster(infos)
	
	--缓存当前战斗group，最后结算用
	battle_ground:SetParams32("monster_group_id",monster_group_id)	
	
	return ResultCode_ResultOK;	

end

function x001002_OnDamageRefix(battle_ground,damage_info,killer)

end

function x001002_OnDamage(battle, attacker, source_guid, target_guid, damage)
	return 0;
end

function x001002_OnCheckBattleEnd(battle_ground)

	local DeadCampID=battle_ground:GetAllDeadCampID();
	if DeadCampID == 1 then
		return 2;
	elseif DeadCampID == 2 then
		return 1;
	end
	return 0;
end

function x001002_OnBattleReward(battle_ground, winnercamp)
	
	local stage_id = battle_ground:Brief():stage_id()
	battle_ground:PushHookStatisticsToStageManager()
	if winnercamp == 1 then
		
		local player = battle_ground:GetCreator()
		--挂机掉落包
		local monster_group_id = battle_ground:GetParams32("monster_group_id",monster_group_id)	
		battle_ground:BattleGroupDrop(monster_group_id)
		
		battle_ground:AddBossChallengeProgress(stage_id, 1)
	end	
	return true
end

function x001002_OnBattleEnd(battle_ground,force, winnercamp)
    local player = battle_ground:GetCreator()
	if player ~= nil then
		local stage_id = battle_ground:Brief():stage_id()
		player:GetPlayerStageProxy():SyncSpecStageToClient(stage_id)
		player:GetPlayerStageProxy():SyncEffe2Client()
	end
	--通知客户端结束战斗
	--local brief = battle_ground:Brief()
	--if brief:type() ~= BattleGroundType_PVE_SIMULATE then
	battle_ground:BattleOverNotify(winnercamp)
	--end

	if force == false then
		battle_ground:RefreshBattle(battle_ground:GetRefreshWaitTime())
	else
		local player = battle_ground:GetCreator()
		if battle_ground:GetClientInsight() == true then	
			player:SendCommonResult(ResultOption_OnHook_Opt,ResultCode_HookEndOK)
		end
	end
	return true;
end
