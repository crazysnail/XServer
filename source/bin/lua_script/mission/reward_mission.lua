
--赏金任务

local activity_type=ActivityType.GoldReward

--环递增经验系数
local circle_exp_rate=0.1
--人数递增经验系数
local count_exp_rate=0.1

local reward_point = {1,1,1,2,2,2,2,3,3,3}
-------------------------------------------------------

function x000508_xOnFinishEnd(player, params)	
	return ResultCode_ResultOK
end

function x000508_xOnReset(player, params)
	local mission_type = params["type"]
	if mission_type == MissionType_RewardMission or mission_type == MissionType_ExtraRewardMission then	
		--重置逻辑
		local cell_container= player:DataCellContainer()	
		cell_container:set_data_32(CellLogicType_MissionCell,MissionCellIndex.Reward_Circle,0,true)	--重置环数
		cell_container:set_data_32(CellLogicType_MissionCell,MissionCellIndex.ExtraMission_Id,-1,true)	--重置任务号
		return ResultCode_ResultOK
	end	
	return ResultCode_InvalidMissionType
end

function x000508_xOnAccept(player, params)
	local mission_type = params["type"]
	if mission_type == MissionType_RewardMission or mission_type == MissionType_ExtraRewardMission then		
		--检查身上有没有同类型任务，有就不生成新的
		local mission_map=player:GetMissionProxy():GetMissionMap()	
		for k, mission in pairs(mission_map) do
			--print(mission)
			config = mission:Config()
			if config:type() == MissionType_RewardMission or config:type() == MissionType_ExtraRewardMission then
				return ResultCode_MissionCountLimit
			end
		end
			
		local cell_container= player:DataCellContainer()		
		local circle_count = cell_container:get_data_32(CellLogicType_MissionCell,MissionCellIndex.Reward_Circle) 	--第5个值为赏金任务进度
		local next_circel = math.fmod( circle_count, 10) +1
		if next_circel <= 0 then
			next_circel = 1
		end	
	
		local mission_id = LuaMtMissionManager():GenCommonRewardMission(next_circel)	
		player:GetMissionProxy():OnAddMission(mission_id)

		return ResultCode_ResultOK
	end
	
	return ResultCode_InvalidMissionType
end

function x000508_NormalReard(player,config)
	
	local level_config = LuaMtActorConfig():GetLevelData(player:PlayerLevel())
	if level_config == nil then
		return ResultCode_InvalidConfigData
	end
	
	--奖励经验	
	local exp_rate = config:exp_rate()
	local exp = level_config:reward_exp()
	if exp_rate >= 0 then
		exp = exp_rate*exp;
	end
	player:AddItemByIdWithNotify(TokenType_Token_Exp,exp,ItemAddLogType_AddType_Activity,activity_type,1)
	--队长而外经验
	if player:IsTeamLeader() then
		player:GetMissionProxy():TeamLeaderExtraReward(config,exp);
	end
	
	--奖励金币
	local gold_rate = config:gold_rate()
	local gold = level_config:reward_gold()
	if gold_rate >= 0 then
		gold = gold_rate*gold;
	end
	
	player:AddItemByIdWithNotify(TokenType_Token_Gold,gold,ItemAddLogType_AddType_Activity,activity_type,1)
	
	player:GetMissionProxy():OnMissionReward(config:id())
	--掉落活动检查
	player:CheckAcrDrop(activity_type)
end

function x000508_xOnFinishBegin(player,params)	
	
	--额外任务不受次数影响
	local config = LuaMtMissionManager():FindMission(params["id"])
	if config == nil then
		return ResultCode_InvalidConfigData
	end
	
	local cell_container = player:DataCellContainer()	
	if config:type() == MissionType_ExtraRewardMission then
		x000508_NormalReard(player,config)	
		local mission_id = cell_container:get_data_32(CellLogicType_MissionCell,MissionCellIndex.ExtraMission_Id)
		local next_mission_id = LuaMtMissionManager():GenNextExtraRewardMission(mission_id)
		if next_mission_id ~= -1 then
			player:GetMissionProxy():OnAddMission(next_mission_id)		
		end
		return ResultCode_ResultOK
	end
	
	
	local circle = cell_container:get_data_32(CellLogicType_MissionCell,MissionCellIndex.Reward_Circle)+1	
	cell_container:set_data_32(CellLogicType_MissionCell,MissionCellIndex.Reward_Circle,circle,true)	
	
	--正常任务次数影响
	local ac_config = LuaActivityManager():FindActivity(activity_type)
	if ac_config == nil then
		return ResultCode_InvalidConfigData
	end
	local rtimes = ac_config:reward_count()
	
	local done_times = player:OnLuaFunCall_1(501,"GetActivityTimes",activity_type);
	if done_times >= rtimes then
		return ResultCode_ResultOK
	end
	
	local cur_times =math.fmod( done_times, 10)+1	
	if config:type() ==MissionType_RewardMission then	
	
		--赏金积分奖励		
		player:AddItemByIdWithNotify(TokenType_Token_Reward,reward_point[cur_times],ItemAddLogType_AddType_Activity,activity_type,1)				
		
		--活跃度次数+活跃度奖励	
		player:OnLuaFunCall_1(501,"AddActivityTimes",activity_type);
		
		x000508_NormalReard(player,config)
				
		--周三，周天完成10环有而外任务一次
		local weekday = LuaMtTimerManager():GetWeekDay()
		if weekday==0 or weekday==3 then
			if circle==10 then
				mission_id = cell_container:get_data_32(CellLogicType_MissionCell,MissionCellIndex.ExtraMission_Id)
				next_mission_id = LuaMtMissionManager():GenNextExtraRewardMission(mission_id)
				player:GetMissionProxy():OnAddMission(next_mission_id)
			end
		end
		
		if circle == 10 then
			player:SendStatisticsResult(activity_type,true)
		end
				
		return ResultCode_ResultOK
	end
	
	return ResultCode_InvalidMissionType
end

function x000508_xOnDrop(player,params)
	return ResultCode_ResultOK	
end

-------------------------------------------------------------------
			
function x000508_GetMissionParams(player)
	 return {}
end
function x000508_GetNextMission(player)
	return -1
end

function x000508_OnAddBegin(player,config)
	
	if config:type() ~= MissionType_RewardMission and config:type() ~= MissionType_ExtraRewardMission then
		return ResultCode_InvalidMissionType
	end

	--赏金任务存储的是环数
	local cell_container= player:DataCellContainer()
	if config:type() == MissionType_ExtraRewardMission then		
		cell_container:set_data_32(CellLogicType_MissionCell,MissionCellIndex.ExtraMission_Id,config:id(),true)
	end	

	return ResultCode_ResultOK
end

function x000508_OnClone(player, target, config)
	--参与标记
	x000501_OnActivityJoin(player,activity_type)
	return ResultCode_ResultOK
end

function x000508_OnAddEnd(player,config)
	--参与标记
	x000501_OnActivityJoin(player,activity_type)	
	return ResultCode_ResultOK
end	



function x000508_ResolveAllReward(player)
	
	local level_config = LuaMtActorConfig():GetLevelData(player:PlayerLevel())
	if level_config == nil then
		return 0,0
	end
	local exp = level_config:reward_exp()
	local gold = level_config:reward_gold()
	
	local final_gold = 0
	local final_exp = 0
		
	--结算每个任务奖励
	for i=1,10,1 do
		local mission_id = LuaMtMissionManager():GenCommonRewardMission(i)	
		local config = LuaMtMissionManager():FindMission(mission_id)
		if config ~= nil then
			local gold_rate = config:gold_rate()			
			if gold_rate >= 0 then
				final_gold = final_gold+ gold_rate*gold
			end
			
			local exp_rate = config:exp_rate()		
			if exp_rate >= 0 then
				final_exp = final_exp+ exp_rate*exp
			end
		end
	end
	
	return final_gold,final_exp
end	


