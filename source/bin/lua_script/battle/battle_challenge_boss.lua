
function x001008_OnJoinCheck(player,params)
	return ResultCode_ResultOK
end

function x001008_OnBattleCheck(player,params)
	return ResultCode_ResultOK
end

function x001008_OnGoBattle(battle_ground,params)

	local player = battle_ground:GetCreator()
	if player == nil then
		return ResultCode_BattleCreateFailed
	end
	
	local battlegroup = LuaSceneManager():GetSceneBattleGroupById(player:GetSceneId(),params["group_id"]);
	if battlegroup == nil then
		return ResultCode_InvalidConfigData
	end
	
	local old_ground = player:BattleGround()
	if old_ground ~= nil then
		if old_ground:BattleType() ~= BattleGroundType_PVE_HOOK then
			return ResultCode_Wrong_Status
		end		
		player:QuitBattle()
	end
	
	player:StopMove()	
				
	battle_ground:SetScriptId(1008)				
	local brief = battle_ground:Brief()
	brief:set_scene_id(player:GetSceneId())
	brief:set_battle_group_id(params["group_id"])		
	brief:set_stage_id(params["stage_id"])
	brief:set_type(BattleGroundType_PVE_STAGE_BOSS)		
	
	if battle_ground:Init() == false then
		return ResultCode_BattleCreateFailed
	end		

	player:SendCommonResult(ResultOption_OnHook_Opt,ResultCode_HookStartOK)
	battle_ground:BattleEnterNotify()		
	

	return ResultCode_ResultOK
end

function x001008_OnCreateReady(battle_ground, player)
	
	local brief =battle_ground:Brief()
	local stage_config = LuaMtSceneStageManager():GetConfig(brief:stage_id())
	if brief == nil or stage_config == nil then
		return ResultCode_InvalidConfigData;	
	end
	if player:GetBossChallengeCount(brief:stage_id()) > 0 then
		battle_ground:CreateAttackerPlayerBattleActors(false,false,ActorFigthFormation_AFF_NORMAL_HOOK)
	
		if stage_config:bosses_size() <=0 then
			return ResultCode_InvalidConfigData;	
		end
		local monster_group_id_index = LuaServer():RandomNum(0, stage_config:bosses_size() - 1)
		local monster_group_id = stage_config:bosses(monster_group_id_index)
		battle_ground:SetParams32("monster_group_id",monster_group_id)		
		local monster_group_config = LuaMtMonsterManager():FindMonsterGroup(monster_group_id)
		local infos={}
		for i = 0,monster_group_config:monsters_size() - 1 do
			local monster_define = monster_group_config:monsters(i)
			if (monster_define:id() > 0) then
				infos[monster_define:loc()]=monster_define:id()
			end
		end
		battle_ground:CreateDefenderMonster(infos)
		return ResultCode_ResultOK;	
	end
	
	return ResultCode_InvalidConfigData;	

end

function x001008_OnDamageRefix(battle_ground,damage_info,killer)

end

function x001008_OnDamage(battle, attacker, source_guid, target_guid, damage)
	return 0;
end

function x001008_OnCheckBattleEnd(battle_ground)
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

function x001008_OnBattleReward(battle_ground, winnercamp)
	local stage_id = battle_ground:Brief():stage_id()
	local player = battle_ground:GetCreator()
	if player == nil then
		return false
	end

	if winnercamp == 1 then				
		--首杀掉落检查
		if player:FirstStageReward(stage_id) == false then
			--挂机掉落包	
			if  player:GetBossChallengeCount(stage_id) > 0 then
				local monster_group_id = battle_ground:GetParams32("monster_group_id")		
				battle_ground:BattleGroupDrop(monster_group_id)
			end
		end
		--解锁关卡
		battle_ground:PlayerRaidStage()		
		--扣次数
		player:DecBossChallengeCount(stage_id,1)
	end	
	return true
end

function x001008_OnBattleEnd(battle_ground,force, winnercamp)

	--通知客户端结束战斗
	if force == false then
		battle_ground:BattleOverNotify(winnercamp)
		local player = battle_ground:GetCreator()
		if player == nil then
			return false
		end
		
			
		local brief = battle_ground:Brief()
		local stage_id = brief:stage_id()
		
		local default_id = LuaMtSceneStageManager():DefaultStageId(player:GetCamp())
		--初始关卡特写，战斗结束不挂机！
		if stage_id == default_id then	
			player:SendCommonResult(ResultOption_OnHook_Opt,ResultCode_HookEndOK)
			
			--更新挂机关卡
			local next = LuaMtSceneStageManager():GetNextStageId(stage_id)			
			local proxy = player:GetPlayerStageProxy()
			local stagedb = player:GetPlayerStageDB()
			stagedb:set_current_stage(next)			
			proxy:SyncSpecStageToClient(next)
			
			return true
		end
		
		
		if player:GetBossChallengeCount(stage_id)>0 and winnercamp == 1then
			battle_ground:RefreshBattle(0)		
		else
			player:SendCommonResult(ResultOption_OnHook_Opt,ResultCode_HookEndOK)
			if player:PlayerCanHook(stage_id) then
				battle_ground:RefreshBattleGroup(0)
				player:GetPlayerStageProxy():ResetTo(stage_id)
				player:StartHookBattle(stage_id, brief:battle_group_id(), 1, 1)				
			end
		end
	else
		local player = battle_ground:GetCreator()
		player:SendCommonResult(ResultOption_OnHook_Opt,ResultCode_HookEndOK)
	end
	
	return true
	
end
