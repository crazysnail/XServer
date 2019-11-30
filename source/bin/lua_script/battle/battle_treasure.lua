--藏宝图遗迹守卫战斗脚本
-----------------------------------------------------------------------

function x001010_OnJoinCheck(player,params)
	return ResultCode_ResultOK
end

function x001010_OnBattleCheck(player,params)
	return ResultCode_ResultOK
end

--这个先放这里吧
function x001010_OnBigTick(scene)
end



function x001010_OnGoBattle(battle_ground,params)

	local player = battle_ground:GetCreator()
	if player == nil then
		return ResultCode_BattleCreateFailed
	end
	
	local treasure_pos = player:GetParams32("treasure_pos")
	local layout = LuaMtMonsterManager():FindScenePos(treasure_pos)
	if layout == nil then
		return ResultCode_InvalidConfigData
	end
	
	if layout:scene_id() ~= player:GetSceneId() then
		return ResultCode_InvalidConfigData
	end
	params["group_id"]=layout:group_index()

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
		
	battle_ground:SetScriptId(1010)		
	battle_ground:SetParams32("monster_group_id",params["monster_group_id"])		
	
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


function x001010_OnCreateReady(battle_ground,player)

	battle_ground:CreateAttackerPlayerBattleActors(false,false,ActorFigthFormation_AFF_NORMAL_HOOK)
	monster_group_id = battle_ground:GetParams32("monster_group_id")
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
	
	return ResultCode_ResultOK;	

end

function x001010_OnDamageRefix(battle_ground,damage_info,killer)

end

function x001010_OnDamage(battle, attacker, source_guid, target_guid, damage)
	return 0;
end

function x001010_OnCheckBattleEnd(battle_ground)
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


function x001010_OnBattleReward(battle_ground,winnercamp)
	local winners = nil
	if winnercamp ==1 then
		winners=battle_ground:GetAttackerPlayers()
	else
		return false;
	end

	for k, player in pairs(winners) do
		--奖励经验
		local level_config = LuaMtActorConfig():GetLevelData(player:PlayerLevel())
		if level_config == nil then
			return false
		end
		
		player:AddItemByIdWithNotify(TokenType_Token_Exp,level_config:treasure_exp(),2,1010,1)
		player:AddItemByIdWithNotify(TokenType_Token_Gold,level_config:treasure_gold(),2,1010,1)		
	end
	return true
end

function x001010_OnBattleEnd(battle_ground,force,winnercamp)
	--通知客户端结束战斗
	battle_ground:BattleOverNotify(winnercamp)
	return true;
end
