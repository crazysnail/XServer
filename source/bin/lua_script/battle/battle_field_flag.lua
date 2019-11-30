--夺旗战场
local  activity_type = ActivityType.BattleCarbon2


function x001022_OnJoinCheck(player,params)

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
	
	print("now_time"..now_time.." end_time"..end_time.. " begin_time"..newtime)
	
	if now_time> tonumber(end_time) then				--ac_notify_011,{0}活动已结束
		return player:SendClientNotify("ac_notify_011|"..ac_name,-1,1)
	end
	
	local ret= LuaActivityManager():IsActive(ac_config, player:PlayerLevel()) 
	if ret ~= ResultCode_ResultOK then					--ac_notify_020,{0}活动未开启
		return player:SendClientNotify("ac_notify_020|"..ac_name,-1,1)
	end
	
	--else												--ac_notify_013,{0}活动未开启，晚上{1}才可以进行
	--	return player:SendClientNotify("ac_notify_013|"..ac_name.."|"..newtime,-1,1)
	--end
	
	
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
	
	print("ltimes"..ltimes.." rtimes"..rtimes.. " done_times"..done_times)
	if ltimes >= 0 then
		if done_times >= ltimes then	--今日你已完成{0}，明日再来吧											
			return player:SendClientNotify("ac_notify_007|"..ac_name,-1,1)			
		end
	end	

	--if rtimes >= done_times then		--今日你已完成{0}，再次进入将不再获取任何奖励
	--	player:SendClientNotify("ac_notify_008|"..ac_name,-1,1)
	--end		

	return ResultCode_ResultOK
end

function x001022_OnBattleCheck(player,params)
	return ResultCode_ResultOK
end


function x001022_OnGoBattle(battle_ground,params)
	
	local player = battle_ground:GetCreator()
	local target = battle_ground:GetTarget()
	if player == nil or player:BattleGround() ~=nil  then
		return ResultCode_BattleCreateFailed
	end
	
	if target == nil or target:BattleGround() ~=nil  then
		return ResultCode_InvalidTargetPlayer
	end
	
	--local formation = target:GetActorFormation(ActorFigthFormation_AFF_PVP_BATTLE_FIELD)
	--if formation == nil then
	--	return ResultCode_InvalidTargetPlayer
	--end
	
	local battlegroup = LuaSceneManager():GetSceneBattleGroupById(player:GetSceneId(),params["group_id"]);
	if battlegroup == nil then
		return ResultCode_InvalidConfigData
	end

	player:StopMove()
	target:StopMove()
		
	battle_ground:SetScriptId(1022)				
	local brief = battle_ground:Brief()
	brief:set_scene_id(player:GetSceneId())
	brief:set_battle_group_id(params["group_id"])		
	brief:set_type(BattleGroundType_PVP_BATTEL_FIELD)
	
	if battle_ground:Init() == false then
		return ResultCode_BattleCreateFailed
	end		
	
	battle_ground:BattleEnterNotify()	
	player:SendClientNotify("bf_fight_to|" .. target:Name(), -1, -1);
	target:SendClientNotify("bf_fight_from|" .. player:Name(), -1, -1);
	
	return ResultCode_ResultOK
end

function x001022_OnCreateReady(battle_ground, player)
	--battle_ground:RefreshAttacker()
	battle_ground:CreateAttackerPlayerBattleActors(false,false,ActorFigthFormation_AFF_PVP_BATTLE_FIELD)	
	--血量继承
	local attackers = battle_ground:GetAttackers()
	for i,actor in pairs(attackers) do
		actor:SetHp(actor:GetLastHp(LastHpType_BFLastHp))		
	end
		
	battle_ground:CreateDefenderPlayerBattleActors(false,false,ActorFigthFormation_AFF_PVP_BATTLE_FIELD)	
	--血量继承
	local defenders = battle_ground:GetDefenders()
	for i,actor in pairs(defenders) do
		actor:SetHp(actor:GetLastHp(LastHpType_BFLastHp))	
	end
	
	--清理尸体
	battle_ground:CleanDeadBattleActors()
	
	return ResultCode_ResultOK;	
end

function x001022_OnCheckBattleEnd(battle_ground)
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


function x001022_SaveAttackerLastHp(battle_ground,player)
	local attackers = battle_ground:GetAttackers()
	for i,actor in pairs(attackers) do
		if actor:Hp()<=0 then		
			--受db初始化值0的影响，这里用-1表示死亡状态
			player:SavePlayerLastHp(actor:Guid(),LastHpType_BFLastHp,-1)
		else
			player:SavePlayerLastHp(actor:Guid(),LastHpType_BFLastHp,actor:Hp())
		end
	end
end

function x001022_SaveDefenderLastHp(battle_ground,player)
	local defenders = battle_ground:GetDefenders()
	for i,actor in pairs(defenders) do	
		if actor:Hp()<=0 then		
			--受db初始化值0的影响，这里用-1表示死亡状态
			player:SavePlayerLastHp(actor:Guid(),LastHpType_BFLastHp,-1)
		else
			player:SavePlayerLastHp(actor:Guid(),LastHpType_BFLastHp,actor:Hp())
		end
		
	end
end

function x001022_OnDamageRefix(battle_ground,damage_info,killer)

end

function x001022_OnDamage(battle, attacker, source_guid, target_guid, damage)
	return 0;
end


function x001022_OnBattleReward(battle_ground, winnercamp)		
		
	return true
end

function x001022_OnBattleEnd(battle_ground,force, winnercamp)		
	local creator = battle_ground:GetCreator()
	local target = battle_ground:GetTarget()
	if creator ~= nil and target~=nil then			
		local winer,loser
		if winnercamp == 1 then
			--攻击方获胜
			x001022_SaveAttackerLastHp(battle_ground,creator)
			target:ResetPlayerActorLastHp(LastHpType_BFLastHp)
			winer=creator:Guid()
			loser=target:Guid()
		else
			--防御方获胜
			x001022_SaveDefenderLastHp(battle_ground,target)
			creator:ResetPlayerActorLastHp(LastHpType_BFLastHp)
			winer=target:Guid()
			loser=creator:Guid()
		end
				
		local raid = creator:GetRaid()
		if raid ~= nil then
			raid:OnRaidBattleOver(winer,loser)
		end
	end
	--通知客户端结束战斗
	battle_ground:BattleOverNotify(winnercamp)	
	return true
end
