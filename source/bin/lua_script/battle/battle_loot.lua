

function x001009_OnJoinCheck(player,params)
	return ResultCode_ResultOK
end

function x001009_OnBattleCheck(player,params)
	return ResultCode_ResultOK
end


function x001009_OnGoBattle(battle_ground,params)
	
	local player = battle_ground:GetCreator()
	local target = battle_ground:GetTarget()
	if player == nil or player:BattleGround() ~=nil  then
		return ResultCode_BattleCreateFailed
	end
	
	if target == nil or target:BattleGround() ~=nil  then
		return ResultCode_InvalidTargetPlayer
	end
	
	--local formation = target:GetActorFormation(ActorFigthFormation_AFF_PVP_LOOT_FORMATION)
	--if formation == nil then
	--	return ResultCode_InvalidTargetPlayer
	--end
	
	local battlegroup = LuaSceneManager():GetSceneBattleGroupById(player:GetSceneId(),params["group_id"]);
	if battlegroup == nil then
		return ResultCode_InvalidConfigData
	end

	player:StopMove()
	target:StopMove()
		
			
	battle_ground:SetScriptId(1009)		
		
	local brief = battle_ground:Brief()
	brief:set_scene_id(player:GetSceneId())
	brief:set_battle_group_id(params["group_id"])		
	brief:set_type(BattleGroundType_PVP_LOOT)
	
	if battle_ground:Init() == false then
		return ResultCode_BattleCreateFailed
	end		
	
	battle_ground:BattleEnterNotify()	
	
	return ResultCode_ResultOK
end


function x001009_OnCreateReady(battle_ground, player)	
	battle_ground:CreateAttackerPlayerBattleActors(false,false,ActorFigthFormation_AFF_PVP_LOOT_FORMATION)
	battle_ground:CreateDefenderPlayerBattleActors(false,false,ActorFigthFormation_AFF_PVP_LOOT_FORMATION)
	return ResultCode_ResultOK;	
end

function x001009_OnDamageRefix(battle_ground,damage_info,killer)

end

function x001009_OnDamage(battle, attacker, source_guid, target_guid, damage)
	return 0;
end

function x001009_OnCheckBattleEnd(battle_ground)
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

function x001009_OnBattleReward(battle_ground, winnercamp)	
	local stage_id = battle_ground:GetStageId()
	if winnercamp == 1 then
		local creator = battle_ground:GetCreator()
		local loot_target = battle_ground:GetTarget()
		loot_target:BeLoot(creator:Guid())
	end
	return true
end

function x001009_OnBattleEnd(battle_ground,force, winnercamp)
	--通知客户端结束战斗
	battle_ground:BattleOverNotify(winnercamp)
	return true
end
