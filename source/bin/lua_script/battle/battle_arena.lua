
local activity_type=ActivityType.Arena


function x001012_OnJoinCheck(player,params)

	local ac_config = LuaActivityManager():FindActivity(activity_type)
	if ac_config == nil then
		return ResultCode_InternalResult
	end
	
	local ac_name = ac_config:name()
	
		
	--等级检查
	if player:PlayerLevel()<ac_config:open_level() then
		return player:SendClientNotify("ac_notify_002|"..math.floor(ac_config:open_level()).."|"..ac_name,-1,1)
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

	--if rtimes >= done_times then		--今日你已完成{0}，再次进入将不再获取任何奖励
	--	player:SendClientNotify("ac_notify_008|"..ac_name,-1,1)
	--end		

	return ResultCode_ResultOK
end


function x001012_OnBattleCheck(player,params)
	return ResultCode_ResultOK	
end


function x001012_OnGoBattle(battle_ground,params)
	
	local player = battle_ground:GetCreator()
	if player == nil then
		return ResultCode_BattleCreateFailed
	end

	local battlegroup = LuaSceneManager():GetSceneBattleGroupById(player:GetSceneId(),params["group_id"]);
	if battlegroup == nil then
		return ResultCode_InvalidConfigData
	end

	
	player:StopMove()
	
	battle_ground:SetScriptId(1012)		
	local brief = battle_ground:Brief()
	brief:set_scene_id(player:GetSceneId())
	brief:set_battle_group_id(params["group_id"])		
	brief:set_time_out(90*1000)
	brief:set_type(BattleGroundType_PVP_ARENA)
	
	if battle_ground:Init() == false then
		return ResultCode_BattleCreateFailed
	end		
	battle_ground:CreateBattleReply()
	battle_ground:BattleEnterNotify()
	
	return ResultCode_ResultOK
end

function x001012_OnCreateReady(battle_ground, player)
	
	battle_ground:CreateAttackerPlayerBattleActors(false,false,ActorFigthFormation_AFF_PVP_ARENA)
	battle_ground:CreateDefenderFromArena();
	return ResultCode_ResultOK;	
end

function x001012_OnDamageRefix(battle_ground,damage_info,killer)

end

function x001012_OnDamage(battle, attacker, source_guid, target_guid, damage)
	return 0;
end

function x001012_OnCheckBattleEnd(battle_ground)
	if battle_ground:TimeOut() then --竞技场超时后, 伤害高的一方获胜
		local stat = battle_ground:GetStat()
		if stat:attacker_damage() > stat:defender_damage() then
			return 1
		else
			return 2;
		end
	end
	local DeadCampID=battle_ground:GetAllDeadCampID();
	if DeadCampID == 1 then
		return 2;
	elseif DeadCampID == 2 then
		return 1;
	end
	return 0;
end

function x001012_OnBattleReward(battle_ground, winnercamp)
	return true
end

function x001012_OnBattleEnd(battle_ground,force, winnercamp)
	
	--通知客户端结束战斗
	battle_ground:BattleOverNotify(winnercamp)
	
	--竞技场战斗结束，等待副本传走
	local creator = battle_ground:GetCreator()	
	if creator ~= nil then
		creator:DataCellContainer():add_data_32(CellLogicType_NumberCell, NumberCellIndex_ArenaChallengeTimeToday,1, true)
		if battle_ground:AddStat2Reply() == false then
			creator:SendClientNotify("add_stat_to_reply_fails", -1, -1)
		end
		if winnercamp == 1 then
			creator:OnArenaBattleDone(true,battle_ground)
		else 
			creator:OnArenaBattleDone(false, battle_ground)
		end
		
		--活动次数更新活跃度
		creator:OnLuaFunCall_1(501,"AddActivityTimes",ActivityType.Arena) 
	end
	battle_ground:SaveBattleReply()

	return true
end
