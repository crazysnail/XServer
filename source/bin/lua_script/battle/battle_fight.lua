--切磋


function x001024_OnGoBattle(battle_ground,params)
	
	local player = battle_ground:GetCreator()
	local target = battle_ground:GetTarget()
	if player == nil or player:BattleGround() ~=nil  then
		return ResultCode_BattleCreateFailed
	end
	
	if target == nil or target:BattleGround() ~=nil  then
		return ResultCode_InvalidTargetPlayer
	end
	
	--local formation = target:GetActorFormation(ActorFigthFormation_AFF_NORMAL_HOOK)
	--if formation == nil then
	--	return ResultCode_InvalidTargetPlayer
	--end

	player:StopMove()
	target:StopMove()

	battle_ground:SetScriptId(1024)		
		
	local brief = battle_ground:Brief()
	brief:set_scene_id(player:GetSceneId())
	brief:set_battle_group_id(1)		
	brief:set_type(BattleGroundType_PVP_ONLINE)
	
	if battle_ground:Init() == false then
		return ResultCode_BattleCreateFailed
	end		
	
	battle_ground:BattleEnterNotify()
	
	
	return ResultCode_ResultOK
end

function x001024_OnCreateReady(battle_ground, player)
	battle_ground:CreateAttackerPlayerBattleActors(false,false,ActorFigthFormation_AFF_NORMAL_HOOK)			
	battle_ground:CreateDefenderPlayerBattleActors(false,false,ActorFigthFormation_AFF_NORMAL_HOOK)	
	return ResultCode_ResultOK;	
end

function x001024_OnCheckBattleEnd(battle_ground)
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

function x001024_OnDamageRefix(battle_ground,damage_info,killer)

end

function x001024_OnDamage(battle, attacker, source_guid, target_guid, damage)
	return 0;
end


function x001024_OnBattleReward(battle_ground, winnercamp)		
	local battle_owner = battle_ground:GetCreator();
	local target = battle_ground:GetTarget();
	if battle_owner == nil or target == nil then
		return false
	end
	if winnercamp == 1 then
		battle_owner:SendClientNotify("pvp_fight_win_with|"..target:Name(), -1, -1)
		target:SendClientNotify("pvp_fight_lose_with|"..battle_owner:Name(), -1, -1)
	else
		battle_owner:SendClientNotify("pvp_fight_lose_with|"..target:Name(), -1, -1)
		target:SendClientNotify("pvp_fight_win_with|"..battle_owner:Name(), -1, -1)
	end
	return true
end

function x001024_OnBattleEnd(battle_ground,force, winnercamp)	
	--通知客户端结束战斗
	battle_ground:BattleOverNotify(winnercamp)	
	return true
end
