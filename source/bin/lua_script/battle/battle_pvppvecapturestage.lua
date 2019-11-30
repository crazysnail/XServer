function x001004_OnJoinCheck(player,params)
	return ResultCode_ResultOK
end

function x001004_OnBattleCheck(player,params)
	return ResultCode_ResultOK
end


function x001004_OnGoBattle(battle_ground,params)
		
	local player = battle_ground:GetCreator()
	if player == nil then
		return ResultCode_BattleCreateFailed
	end
	
	local battlegroup = LuaSceneManager():GetSceneBattleGroupById(player:GetSceneId(),params["group_id"]);
	if battlegroup == nil then
		return ResultCode_InvalidConfigData
	end
	
	player:StopMove()
		
	battle_ground:SetScriptId(1004)		
	local brief = battle_ground:Brief()
	brief:set_scene_id(player:GetSceneId())
	brief:set_battle_group_id(params["group_id"])		
	brief:set_stage_id(params["stage_id"])		
	brief:set_type(BattleGroundType_PVP_PVE_CAPTURE_STAGE)		
	
	if battle_ground:Init() == false then
		return ResultCode_BattleCreateFailed
	end			
	
	battle_ground:BattleEnterNotify()	
	
	return ResultCode_ResultOK
end

function x001004_OnCreateReady(battle_ground, player)

	battle_ground:CreateAttackerPlayerBattleActors(false,false,ActorFigthFormation_AFF_STAGE_CAPTURE)
	local stage_id = battle_ground:GetStageId()
	local config = LuaMtSceneStageManager():GetConfig(stage_id)
	--有防守阵容就提前ok
	if battle_ground:CreateDefenderFromStageCaptain() then
		return ResultCode_ResultOK;	
	end

	local guard_boss_id = config:guard_id()
	local monster_group_config = LuaMtMonsterManager():FindMonsterGroup(guard_boss_id);
	if monster_group_config == nil then
		return ResultCode_UnknownError;	
	end

	local infos ={}
	for i = 0,monster_group_config:monsters_size() - 1 do
		local monster_define = monster_group_config:monsters(i)
		if (monster_define:id() > 0) then
			infos[monster_define:loc()]=monster_define:id()
		end
	end
	battle_ground:CreateDefenderMonster(infos)
	
	return ResultCode_ResultOK;	

end

function x001004_OnDamageRefix(battle_ground,damage_info,killer)

end

function x001004_OnDamage(battle, attacker, source_guid, target_guid, damage)
	return 0;
end


function x001004_OnCheckBattleEnd(battle_ground)
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

function x001004_OnBattleReward(battle_ground, winnercamp)
	local stage_id = battle_ground:GetStageId()
	local creator = battle_ground:GetCreator()
	local brief = battle_ground:Brief()
	if creator == nil or brief == nil then
		return false
	end
	if winnercamp == 1 then
		creator:GetPlayerStageProxy():PlayerCaptrueStage(brief:stage_id(), true)
	else
		creator:GetPlayerStageProxy():PlayerCaptrueStage(brief:stage_id(), false)
	end
	return true
end

function x001004_OnBattleEnd(battle_ground,force, winnercamp)
	--通知客户端结束战斗
	battle_ground:BattleOverNotify(winnercamp)
	
	return true;
end
