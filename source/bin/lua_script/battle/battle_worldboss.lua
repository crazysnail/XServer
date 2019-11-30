

function x001011_OnJoinCheck(player,params)

	local activity_type = player:OnLuaFunCall_n(501, "GetWorldBossAc",{} )	
	local ac_config = LuaActivityManager():FindActivity(activity_type)
	if ac_config == nil then
		return ResultCode_InternalResult
	end
	
	local ac_name = ac_config:name()
	
	--活动开启性判定
	local now_time = LuaMtTimerManager():Time2Num()
	local end_time = ac_config:day_end_time()
	local begin_time = ac_config:day_begin_time()
	local newtime = x000010_TimeToString(begin_time)
	
	--print("xxxxxxxxxxxxxxxxx begin_time"..begin_time.." newtime "..newtime)
	if now_time> tonumber(end_time) then				--ac_notify_011,{0}活动已结束
		return player:SendClientNotify("ac_notify_011|"..ac_name,-1,1)
	end
			
	--等级检查
	if player:PlayerLevel()<ac_config:open_level() then
		return player:SendClientNotify("ac_notify_002|"..math.floor(ac_config:open_level()).."|"..ac_name,-1,1)
	end
	
	local ret= LuaActivityManager():IsActive(ac_config, player:PlayerLevel()) 
	if ret ~= ResultCode_ResultOK then		--ac_notify_013,{0}活动未开启，晚上{1}才可以进行
		return player:SendClientNotify("ac_notify_013|"..ac_name.."|"..newtime,-1,1)
	end
	
	--队伍检查
	if player:GetTeamID() ~= 0 then
		return player:SendClientNotify("ac_notify_004|"..ac_name,-1,1)
	end
	--团队检查
	if player:IsTeamGroup() then
		return player:SendClientNotify("ac_notify_003|"..ac_name,-1,1)
	end
	
	--次数判定,只有自己
	local ltimes = ac_config:limit_times()
	local rtimes = ac_config:reward_count();
	local done_times = player:OnLuaFunCall_1(501, "GetActivityTimes", activity_type)
	if ltimes >= 0 then
		if done_times >= ltimes then	--今日你已完成{0}，明日再来吧											
			return player:SendClientNotify("ac_notify_007|"..ac_name,-1,1)			
		end
	end	

	--if rtimes > 0 and  done_times >= rtimes then		--今日你已完成{0}，再次进入将不再获取任何奖励
	--	player:SendClientNotify("ac_notify_008|"..ac_name,-1,1)
	--end	
	
	return ResultCode_ResultOK
end

function x001011_OnBattleCheck(player,params)
	return ResultCode_ResultOK
end



function x001011_OnGoBattle(battle_ground,params)
	
	local player = battle_ground:GetCreator()
	if player == nil then
		return ResultCode_BattleCreateFailed
	end
	
	local config = LuaMtWorldBossManager():GetCurWorldBossConfig()
	if config == nil then
		return ResultCode_InvalidConfigData
	end
	if config:sceneid() ~= player:GetSceneId() then
		return ResultCode_InvalidConfigData
	end

	local boss = LuaMtWorldBossManager():GetCurWorldBossInfo()
	if boss == nil then
		return ResultCode_InvalidConfigData
	end
	
	if boss:hp() <=0 then
		return ResultCode_Guild_BossDead
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
	
			
	battle_ground:SetScriptId(1011)		
		
	local brief = battle_ground:Brief()
	brief:set_scene_id(player:GetSceneId())
	brief:set_battle_group_id(params["group_id"])	
	brief:set_stage_id(boss:monstergourpid())	
	brief:set_type(BattleGroundType_PVP_WORLDBOSS_BATTLE)
	
	if battle_ground:Init() == false then
		return ResultCode_BattleCreateFailed
	end		
	
	player:OnSendWorldBossHpUpdate()
	
	battle_ground:BattleEnterNotify()	
	
	return ResultCode_ResultOK
end

function x001011_OnCreateReady(battle_ground, player)

	local weekday = LuaMtTimerManager():GetWeekDay();
	battle_ground:CreateAttackerPlayerBattleActors(false,false,ActorFigthFormation_AFF_PVP_WORLDBOSS_Sun + weekday)
	local monster_group_id = battle_ground:GetStageId();
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


function x001011_OnCheckBattleEnd(battle_ground)
	if battle_ground:TimeOut() then
		return 2;
	end
	local DeadCampID=battle_ground:GetAllDeadCampID();
	if DeadCampID == 1 then
		return 2;
	elseif DeadCampID == 2 then
		return 1;
	end
	if LuaMtWorldBossManager():WorldBossDead() == true then
		return 1;
	end
	return 0;
end

function x001011_OnDamageRefix(battle_ground,damage_info,killer)
	local creator = battle_ground:GetCreator();
	local container = creator:DataCellContainer()	
	local times = container:get_data_32(CellLogicType_NumberCell,NumberCellIndex_InspireTimes)
	if times > 0 then
		local source_guid = damage_info:source_guid();
		local attacker = battle_ground:IsAttacker(source_guid);--
		if attacker == true then
			local damage = damage_info:damage();
			damage = damage*(1 + times*0.2);
			damage_info:set_damage(damage);
		end
	end
end

function x001011_OnDamage(battle, attacker, source_guid, target_guid, damage)
	if attacker == true then
		local creator = battle:GetCreator();
		if creator ~= nil then
			creator:OnWorldBossDamage(damage);
		end
	end
	return 0;
end

function x001011_OnBattleReward(battle_ground, winnercamp)
	local creator = battle_ground:GetCreator();
	if creator ~= nil then
		--活动次数更新活跃度
		local activity_type = creator:OnLuaFunCall_n(501, "GetWorldBossAc",{} )		
		
		--每天最多算一次
		local done_times = x000501_GetActivityTimes(creator,activity_type)
		if done_times <=0 then
			x000501_AddActivityTimes(creator,activity_type)
		end
		--print("activity_type done_times"..done_times)

		--目标
		x000504_xOnWorldBoss(creator,{["value"]=1})
	end
	return true
end

function x001011_OnBattleEnd(battle_ground,force, winnercamp)
	local creator = battle_ground:GetCreator();
	if creator ~= nil then
		if winnercamp ~= 1 then
			creator:EndWorldBoss();
		end
	end
	
	--通知客户端结束战斗
	battle_ground:BattleOverNotify(winnercamp);
	if creator ~= nil then
		creator:GoTo(LuaMtWorldBossManager():GetCurWorldBossConfig():sceneid(),{-1,-1,-1});
	end
	return true
end
