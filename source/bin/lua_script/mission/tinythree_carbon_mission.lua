
--小三环副本任务
local time_limit = 2*3600
local activity_type=ActivityType.TinyThreeCarbon
local mission_type = MissionType_TinyThreeCarbonMission
local process = {MissionCellIndex.CarbonTinyThree_process1,MissionCellIndex.CarbonTinyThree_process2,MissionCellIndex.CarbonTinyThree_process3}
local last_id = MissionCellIndex.CarbonTinyThree_SelfLast
--一周每天任务组
local mission_group={
{1,2,3},
{2,3,4},
{3,4,5},
{4,5,6},
{5,6,7},
{6,7,1},
{7,1,2},
}

---------------------------------------------------------
function x000510_xOnDrop(player, params)	
	--放弃归0
	local cell_container= player:DataCellContainer()	
	cell_container:set_data_32(CellLogicType_MissionCell,last_id,-1,true)
	
	return ResultCode_ResultOK
end

function x000510_xOnReset(player, params)

	if params["type"] == mission_type then
		--重置逻辑
		local cell_container= player:DataCellContainer()	
		cell_container:set_data_32(CellLogicType_MissionCell,process[1],0,true)
		cell_container:set_data_32(CellLogicType_MissionCell,process[2],0,true)
		cell_container:set_data_32(CellLogicType_MissionCell,process[3],0,true)
		cell_container:set_data_32(CellLogicType_MissionCell,last_id,-1,true)
		return ResultCode_ResultOK
	end
	
	return ResultCode_InvalidMissionType
end

function x000510_xOnAccept(player,params)

	if params["type"] == mission_type then	
		local proxy = player:GetMissionProxy()
			
		--检查身上有没有同类型任务，有就不生成新的
		local mission_map=proxy:GetMissionMap()	
		for k, mission in pairs(mission_map) do
			local config = mission:Config()
			if config:type() == mission_type then
				return ResultCode_MissionCountLimit
			end
		end
			
		local next_mission = x000510_GetNextMission(player)
		if next_mission==-1 then
			return ResultCode_NoMoreMission
		end
		proxy:OnAddMission(next_mission)
		
		return ResultCode_ResultOK

	end
	
	return ResultCode_InvalidMissionType

end

function x000510_xOnFinishEnd(player,params)
	local config = LuaMtMissionManager():FindMission(params["id"])
	if config == nil then
		return ResultCode_InvalidConfigData
	end
	---最后一个任务给活跃度
	local last_mission_id = LuaMtMissionManager():GetLastMissionId(config:type(),config:group())
	if last_mission_id == config:id() then
	
		--超过活动等级不获得奖励
		local ac = LuaActivityManager():FindActivity(activity_type)
		if LuaActivityManager():CheckLevel(ac,player:PlayerLevel()) == true then	
			player:OnLuaFunCall_1(501,"AddActivityTimes",activity_type)
		end
		
		--重置一下lastid，保证能重入
		local index = x000510_GetIndex(player,config)	
		if math.fmod( index, 3) == 0 then
			local cell_container= player:DataCellContainer()	
			cell_container:set_data_32(CellLogicType_MissionCell,last_id,-1,true)	
		end
		player:SendStatisticsResult(activity_type,true)	
		x000010_GRaidMissionOver(player,{["value"]=params["rtid"]})
	end	
	
	return ResultCode_ResultOK
end

function x000510_xOnFinishBegin(player,params)
	local mission = player:GetMissionById( params["id"] )
	if mission == nil then
		return ResultCode_InvalidMission
	end
	
	local config = mission:Config()	
	if config == nil then
		return ResultCode_InvalidConfigData
	end
	local circle = config:circle();
	if circle<0 or circle >=32 then
		return ResultCode_InvalidConfigData
	end
	
	--超过活动等级不在获得奖励
	local ac = LuaActivityManager():FindActivity(activity_type)
	if LuaActivityManager():CheckLevel(ac,player:PlayerLevel()) == false then		
		player:SendClientNotify("NoMoreAcRweard|45|"..ac:name(),-1,-1)
		return ResultCode_ResultOK	
	end
	
	local index = x000510_GetIndex(player,config)	
	local cell_container = player:DataCellContainer()	
	local process_flags = cell_container:get_data_32(CellLogicType_MissionCell, process[index]);
	local last_mission_id = LuaMtMissionManager():GetLastMissionId(config:type(),config:group())
	
	--print("ddddddddddddddddddddddddddddddddd "..circle.." flag "..process_flags.." player "..player:Guid().." process[index]"..process[index].." index"..index)
	--只能领一次
	if cell_container:check_bit_data_32(process_flags,circle) == false then
		local new_flag = cell_container:set_bit_data_32(process_flags,circle)
		cell_container:set_data_32(CellLogicType_MissionCell, process[index],new_flag,true)
			
		--奖励经验
		local level_config = LuaMtActorConfig():GetLevelData(player:PlayerLevel())
		if level_config == nil then
			return ResultCode_InvalidConfigData
		end
				
		--取得波数
		local ids = LuaMtMissionManager():GetCarbonTinyThreeMission(config:group())
		local wave_count = #ids-1
		
		local exp = 0;
		local gold = 0;
		
		if wave_count == 5 then
			exp = level_config:three_5_exp()
			gold = level_config:three_5_gold()
		elseif wave_count == 6 then
			exp = level_config:three_6_exp()
			gold = level_config:three_6_gold()
		elseif wave_count == 7 then
			exp = level_config:three_7_exp()
			gold = level_config:three_7_gold()
		elseif wave_count == 8 then
			exp = level_config:three_8_exp()
			gold = level_config:three_8_gold()
		elseif wave_count == 9 then
			exp = level_config:three_9_exp()
			gold = level_config:three_9_gold()
		end
	
		local exp_rate = config:exp_rate()
		if exp_rate>=0 then
			exp = exp_rate*exp;
		end
		
		local gold_rate = config:gold_rate()
		if gold_rate>=0 then
			gold = gold_rate*gold;
		end
				
		
		player:AddItemByIdWithNotify(TokenType_Token_Exp,exp,ItemAddLogType_AddType_Activity,activity_type,1);
		player:AddItemByIdWithNotify(TokenType_Token_Gold,gold,ItemAddLogType_AddType_Activity,activity_type,1);
		
		--队长而外经验
		if player:IsTeamLeader() then
			player:GetMissionProxy():TeamLeaderExtraReward(config,exp);
		end
		
		---最后一个任务给活跃度		
		if last_mission_id == config:id() then			
			--特殊奖励
			local id,count = x000010_GenSpecialReward()
			if id>= 0 and count> 0 then
				player:AddItemByIdWithNotify(id,count,ItemAddLogType_AddType_Activity,activity_type,1) 
			end				
		end				
		player:GetMissionProxy():OnMissionReward(params["id"])
		--掉落活动检查
		player:CheckAcrDrop(activity_type)
	end	
		
	return ResultCode_ResultOK	
end

------------------------------------------------------------------
function x000510_GetMissionParams(player)

	local index = 1
	local cell_container = player:DataCellContainer()	
	local last = cell_container:get_data_32(CellLogicType_MissionCell,last_id)
	if last > 0 then		
		local config = LuaMtMissionManager():FindMission(last)
		if config ~= nil then
			index = x000510_GetIndex(player,config)
		end
	end
	return {last_id,process[index]}
end
function x000510_GetNextMission(player)
	local weekday = LuaMtTimerManager():GetWeekDay()
	local groupids = mission_group[weekday+1]
	
	local cell_container= player:DataCellContainer()		
	local lastmission = cell_container:get_data_32(CellLogicType_MissionCell,last_id)
	
	--clone是不会走nextmission接口的，所以不用担心curcircle对此处的影响
	if lastmission <=0 then
		lastmission = -1
	end	
		
	local mission_config = LuaMtMissionManager():FindMission(lastmission)
	local lastgroup =groupids[1]
	--print("lastgroup"..lastgroup)
	if mission_config ~= nil then
		lastgroup=mission_config:group()
	end	
	
	--检查组
	local check_group = false
	local group_offset = 0
	local group_count = 0
	for k,groupid in pairs(groupids)
	do
		if groupid == lastgroup then
			check_group = true
			group_offset =k
		end
		
		--print("groupid"..groupid)
		group_count = group_count+1
	end
	
	if check_group == false then
		return -1
	end	
	
	local next_mission = LuaMtMissionManager():GenCarbonTinyThreeMission(lastgroup,lastmission)
	--print("next_mission"..next_mission)
	
	if next_mission == -1 then
		return -1
	elseif next_mission == -2 then
		--检查下一组副本
		--print("group_offset"..group_offset)
		--print("group_count"..group_count)
		if group_count==group_offset then
			return -1
		end		
		
		lastgroup = groupids[group_offset+1]
		lastmission = -1
		next_mission = LuaMtMissionManager():GenCarbonTinyThreeMission(lastgroup,lastmission)
		--print("next_mission"..next_mission)
		--是真没任务了
		if next_mission == -1 then
			return -1
		elseif next_mission == -2 then
			return -1
		end
	end
	
	return next_mission
end

function x000510_OnAddBegin(player,config)
	
	if config:type() ~=mission_type then
		return ResultCode_InvalidMissionType
	end
	
	local cell_container= player:DataCellContainer()	
	cell_container:set_data_32(CellLogicType_MissionCell,last_id,config:id(),true)

	return ResultCode_ResultOK
end

function x000510_OnClone(player, target, config)
	
	local target_container = target:DataCellContainer()	
	local last = target_container:get_data_32(CellLogicType_MissionCell, last_id);
	
	local cell_container = player:DataCellContainer()	
	cell_container:set_data_32(CellLogicType_MissionCell, last_id, last, true)
	
	--参与标记
	x000501_OnActivityJoin(player,activity_type)
	
	return ResultCode_ResultOK
	
end

function x000510_OnAddEnd(player,config)
	player:GetMissionProxy():Mission2Member(config:id())
	--参与标记
	x000501_OnActivityJoin(player,activity_type)
	
	return ResultCode_ResultOK
end	

function x000510_GetIndex(player,config)
	local player_container = player:DataCellContainer()
	local weekday = LuaMtTimerManager():GetWeekDay()
	local groupids = mission_group[weekday+1]
	for k=1,3,1 do 
		if groupids[k]==config:group() then
			return k
		end		
	end
	return -1
end	



function x000510_ResolveAllReward(player)
	
	local level_config = LuaMtActorConfig():GetLevelData(player:PlayerLevel())
	if level_config == nil then
		return 0,0
	end
		
	local weekday = LuaMtTimerManager():GetWeekDay()
	local groupids = mission_group[weekday+1]
	
	local final_gold = 0
	local final_exp = 0
	for k,v in pairs( groupids ) do				
		--取得每组任务波数
		local ids = LuaMtMissionManager():GetCarbonBigThreeMission(v)
		local wave_count = #ids-1
		
		local exp = 0;
		local gold = 0;
		
		local group_exp =0;
		local group_gold =0;
		
		if wave_count == 5 then
			exp = level_config:three_5_exp()
			gold = level_config:three_5_gold()
		elseif wave_count == 6 then
			exp = level_config:three_6_exp()
			gold = level_config:three_6_gold()
		elseif wave_count == 7 then
			exp = level_config:three_7_exp()
			gold = level_config:three_7_gold()
		elseif wave_count == 8 then
			exp = level_config:three_8_exp()
			gold = level_config:three_8_gold()
		elseif wave_count == 9 then
			exp = level_config:three_9_exp()
			gold = level_config:three_9_gold()
		end
		
		--结算每个任务奖励
		for m,n in pairs(ids) do
			local config = LuaMtMissionManager():FindMission(n)
			if config ~= nil then
				local gold_rate = config:gold_rate()
				if gold_rate>=0 then
					group_gold = group_gold+gold_rate*gold;
				end
				local exp_rate = config:exp_rate()
				if exp_rate>=0 then
					group_exp = group_exp+exp_rate*exp;
				end
			end
		end
		
		final_gold = final_gold + group_gold
		final_exp = final_exp+ group_exp
	end
	
	return final_gold,final_exp
end	

