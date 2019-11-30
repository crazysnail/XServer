
local activity_type = ActivityType.UnionBoss

function x001020_OnJoinCheck(player,params)
	--是否加入公会
	if player:GetGuildID() == 0 then
		return player:SendClientNotify("Guild_NoExist",-1,1)
	end
	
	local ac_config = LuaActivityManager():FindActivity(activity_type)
	if ac_config == nil then
		return ResultCode_InternalResult
	end
	
	local ac_name = ac_config:name()
		
	--等级检查
	if player:PlayerLevel()<ac_config:open_level() then
		return player:SendClientNotify("ac_notify_002|"..math.floor(ac_config:open_level()).."|"..ac_name,-1,1)
	end
	--活动开启性判定
	local now_time = LuaMtTimerManager():Time2Num()
	local end_time = ac_config:day_end_time()
	local begin_time = ac_config:day_begin_time()
	local newtime = x000010_TimeToString(begin_time)
	if now_time> tonumber(end_time) then				--ac_notify_011,{0}活动已结束
		return player:SendClientNotify("ac_notify_011|"..ac_name,-1,1)
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
	--local ltimes = ac_config:limit_times()
	--local rtimes = ac_config:reward_count();
	--local done_times = player:OnLuaFunCall_1(501, "GetActivityTimes", activity_type)
	--if ltimes >= 0 then
	--	if done_times >= ltimes then	--今日你已完成{0}，明日再来吧											
	--		return player:SendClientNotify("ac_notify_007|"..ac_name,-1,1)			
	--	end
	--end	

	--if rtimes > 0 and  done_times >= rtimes then		--今日你已完成{0}，再次进入将不再获取任何奖励
	--		player:SendClientNotify("ac_notify_008|"..ac_name,-1,1)
	--end		
	return ResultCode_ResultOK
end

function x001020_OnBattleCheck(player,params)
	return ResultCode_ResultOK
end


function x001020_OnGoBattle(battle_ground,params)

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
			
	battle_ground:SetScriptId(1020)		
		
	local brief = battle_ground:Brief()
	brief:set_scene_id(player:GetSceneId())
	brief:set_battle_group_id(params["group_id"])	
	brief:set_stage_id(1)	
	brief:set_type(BattleGroundType_PVE_GUILD_BOSS)
	
	if battle_ground:Init() == false then
		return ResultCode_BattleCreateFailed
	end		
	
	battle_ground:BattleEnterNotify()	
	
	--记录标记
	local dc_container = player:DataCellContainer()
	dc_container:set_bit_data(BitFlagCellIndex_DayTriGuildBossFlag,true)

	return ResultCode_ResultOK
end

function x001020_OnCreateReady(battle_ground, player)

	battle_ground:CreateAttackerPlayerBattleActors(false,false,ActorFigthFormation_AFF_PVE_GUILD_BOSS)
	--血量继承
	local attackers = battle_ground:GetAttackers()
	for i,actor in pairs(attackers) do		
		local lasthp = actor:GetLastHp(LastHpType_GBOSSLastHp)
		actor:SetHp(lasthp)		
	end

	local infos={}
	infos[0]=LuaMtConfigDataManager():FindConfigValue("guild_boss_config"):value1();
	battle_ground:CreateDefenderMonster(infos)
	local defenders = battle_ground:GetDefenders()
	for i,actor in pairs(defenders) do
		actor:SetHp(2000000000)
	end
	--清理尸体
	battle_ground:CleanDeadBattleActors()
	
	--参与战斗就生效	
	--每天最多算一次
	local done_times = x000501_GetActivityTimes(player,activity_type)
	if done_times <=0 then
		x000501_AddActivityTimes(player,activity_type)
	end

	player:UpdateGuildBossDamage(0,0);
	return ResultCode_ResultOK;	
end

function x001020_OnDamageRefix(battle_ground,damage_info,killer)

end

function x001020_OnDamage(battle, attacker, source_guid, target_guid, damage)
	if attacker == true then
		local creator = battle:GetCreator();
		if creator ~= nil then
			--月卡
			local cell_container = creator:DataCellContainer()
			if cell_container:check_bit_data(BitFlagCellIndex_MonthCardFlag) then
				damage=(1.0+0.05)*damage
			end	
			creator:EndGuildBoss(damage);
		end
	end
	return 0;
end

function x001020_OnCheckBattleEnd(battle_ground)
	if battle_ground:TimeOut() then
		--策划要求超时需要血量置0
		local attackers = battle_ground:GetAttackers()
		for i,actor in pairs(attackers) do
			--受db初始化值0的影响，这里用-1表示死亡状态
			actor:SetHp(0)
		end
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

function x001020_OnBattleReward(battle_ground, winnercamp)
	
	return true
end

function x001020_SavePlayerLastHp(battle_ground,player)
	local attackers = battle_ground:GetAttackers()
	for i,actor in pairs(attackers) do
		if actor:Hp()<=0 then		
			--受db初始化值0的影响，这里用-1表示死亡状态
			player:SavePlayerLastHp(actor:Guid(),LastHpType_GBOSSLastHp,-1)
		else
			player:SavePlayerLastHp(actor:Guid(),LastHpType_GBOSSLastHp,actor:Hp())
		end
	end
end

function x001020_OnBattleEnd(battle_ground,force, winnercamp)
	local creator = battle_ground:GetCreator();
	if creator ~= nil then
		x001020_SavePlayerLastHp(battle_ground,creator)
		creator:UpdateFormationLastHpSet(ActorFigthFormation_AFF_PVE_GUILD_BOSS,LastHpType_GBOSSLastHp)	
		local raid = creator:GetRaid()
		if raid ~= nil then
			raid:SetRaidTimer(g_raid_timer.common_destroy,5,-1)
		end
	end
	--通知客户端结束战斗
	battle_ground:BattleOverNotify(winnercamp)
	return true;
end
