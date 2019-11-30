
--暮光军团任务
----------------------------------------------
local activity_type=ActivityType.CorpsFight
local mission_type = MissionType_CorpsMission

--经验点每次消耗
local exp_point_cost = 4
--环递增经验系数
local circle_exp_rate=0.1
--人数经验系数
local count_exp_rate=0.1

local reward_point = {1,1,1,2,2,2,2,3,3,3}
-----------------------------------------------

function x000507_xOnFinishEnd(player, params)
	return ResultCode_ResultOK
end

function x000507_xOnReset(player, params)
	if params["type"] == mission_type then	
		--重置逻辑
		local cell_container= player:DataCellContainer()	
		cell_container:set_data_32(CellLogicType_MissionCell,MissionCellIndex.Corps_Circle,0,true)
		return ResultCode_ResultOK
	end

	return ResultCode_InvalidMissionType
end

function x000507_xOnAccept(player, params)
	if params["type"] == mission_type then	
		--检查身上有没有同类型任务，有就不生成新的
		local mission_map=player:GetMissionProxy():GetMissionMap()	
		for k, mission in pairs(mission_map) do
			local config = mission:Config()
			if config:type() == MissionType_CorpsMission then
				return ResultCode_MissionCountLimit
			end
		end
		
		local mission_id = LuaMtMissionManager():GenCorpsMission()	
		player:GetMissionProxy():OnAddMission(mission_id)
		
		return ResultCode_ResultOK
	end
	return ResultCode_InvalidMissionType
end

function x000507_xOnFinishBegin(player,params)
	
	local cell_container = player:DataCellContainer()	
	local circle = cell_container:get_data_32(CellLogicType_MissionCell,MissionCellIndex.Corps_Circle)		
		
	local ac_config = LuaActivityManager():FindActivity(activity_type)
	if ac_config == nil then
		return ResultCode_InvalidConfigData
	end
	local rtimes = ac_config:reward_count()
	
	local done_times = player:OnLuaFunCall_1(501,"GetActivityTimes",activity_type);
	if done_times >= rtimes then
		return ResultCode_ResultOK
	end
	
	local cur_circle = math.fmod( circle, 10)
	local cur_times = math.fmod( done_times, 10) +1
	local config = LuaMtMissionManager():FindMission(params["id"])
	if config:type() ==mission_type then
	
		--赏金积分奖励		
		player:AddItemByIdWithNotify(TokenType_Token_Reward,reward_point[cur_times],ItemAddLogType_AddType_Activity,activity_type,1)				
		
		--活跃度次数+活跃度奖励	
		player:OnLuaFunCall_1(501,"AddActivityTimes",activity_type);
	
		--奖励经验	
		local step_extra = 1
		if  cur_circle > 0 then
			step_extra = cur_circle*circle_exp_rate
		end		
		
		--奖励经验
		local level_config = LuaMtActorConfig():GetLevelData(player:PlayerLevel())
		if level_config == nil then
			return ResultCode_InvalidConfigData
		end
		
		local count_extra = #player:GetTeamMember(true)-1
		if count_extra<0 then
			count_extra=0
		end
		local final_exp =(1+step_extra)*level_config:light_exp()			
		final_exp = (1+count_extra*count_exp_rate) * final_exp
		
		--print("ddddddddddddddddddddddddddddddddddddddddddfinal_exp"..final_exp.." count_exp_rate "..count_exp_rate)
		--策划需要先注释经验点效果
		--if player:DelItemById(TokenType_Token_ExpPoint,exp_point_cost,ItemDelLogType_DelType_Mission,0) then
		----双倍奖励
		--	player:AddItemByIdWithNotify(TokenType_Token_Exp,2*final_exp,ItemAddLogType_AddType_Activity,activity_type,1)
		--else	
		--普通奖励	
			player:AddItemByIdWithNotify(TokenType_Token_Exp,final_exp,ItemAddLogType_AddType_Activity,activity_type,1)
		--end
		--队长而外经验
		if player:IsTeamLeader() then
			player:GetMissionProxy():TeamLeaderExtraReward(config,final_exp);
		end
		
		--金币没双倍
		local final_gold =(1+step_extra)*level_config:light_gold()	
		player:AddItemByIdWithNotify(TokenType_Token_Gold,final_gold,ItemAddLogType_AddType_Activity,activity_type,1)
		
		player:GetMissionProxy():OnMissionReward(params["id"])
		--掉落活动检查
		player:CheckAcrDrop(activity_type)
		
		if  cur_circle == 0 then
			--特殊奖励
			local id,count = x000010_GenSpecialReward()
			if id>= 0 and count> 0 then
				player:AddItemByIdWithNotify(id,count,ItemAddLogType_AddType_Activity,activity_type,1);
			end
			player:SendStatisticsResult(activity_type,true)	
		end	

		return ResultCode_ResultOK		
		
	end
	
	return ResultCode_InvalidMissionType	
end


function x000507_xOnDrop(player,params)
	local config = LuaMtMissionManager():FindMission(params["id"])	
	if config:type() == mission_type then	
		--处理环数计数
		local cell_container= player:DataCellContainer()
		cell_container:set_data_32(CellLogicType_MissionCell,MissionCellIndex.Corps_Circle,0,true)
		return ResultCode_ResultOK
	end
	return ResultCode_InvalidMissionType	
end

-----------------------------------------------------------------------------

function x000507_GetMissionParams(player)
	 return {}
end
function x000507_GetNextMission(player)
	return -1
end
			
function x000507_OnAddBegin(player,config)
	
	if config:type() ~= MissionType_CorpsMission then
		return ResultCode_InvalidMissionType
	end

	if 	config:finish_type() ~= FinishType_RandomNpc then
		return ResultCode_InvalidFinishType
	end

	local cell_container= player:DataCellContainer()	
	local circle = cell_container:get_data_32(CellLogicType_MissionCell,MissionCellIndex.Corps_Circle)
	if circle <0 then 
		circle = 0;
	end	
	
	circle = circle+1
	cell_container:set_data_32(CellLogicType_MissionCell,MissionCellIndex.Corps_Circle,circle,true)

	return ResultCode_ResultOK
end

function x000507_OnClone(player, target, config)
	
	local target_container = target:DataCellContainer()
	local player_container = player:DataCellContainer()

	local circle = target_container:get_data_32(CellLogicType_MissionCell, MissionCellIndex.Corps_Circle);	
	player_container:set_data_32(CellLogicType_MissionCell, MissionCellIndex.Corps_Circle, circle, true);
	--参与标记
	x000501_OnActivityJoin(player,activity_type)
	return ResultCode_ResultOK
end

function x000507_OnAddEnd(player,config)

	player:GetMissionProxy():Mission2Member(config:id())	
	--参与标记
	x000501_OnActivityJoin(player,activity_type)
	
	return ResultCode_ResultOK
end


function x000507_ResolveAllReward(player)
	
	local level_config = LuaMtActorConfig():GetLevelData(player:PlayerLevel())
	if level_config == nil then
		return 0,0
	end
	
	local ac_config = LuaActivityManager():FindActivity(activity_type)
	if ac_config == nil then
		return 0,0
	end
	
	local exp = level_config:light_exp()		
	local gold = level_config:light_gold()	
	
	local final_gold = 0
	local final_exp = 0
	
	local reward_count = ac_config:reward_count();
	for i=1,reward_count,1 do
		local step_extra = i*circle_exp_rate		
		final_gold = final_gold + (1+step_extra)*gold
		final_exp =	 final_exp + (1+step_extra)*exp
	end
	
	return final_gold,final_exp
end	


