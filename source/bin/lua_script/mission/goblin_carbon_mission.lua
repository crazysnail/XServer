
--哥布林洞穴副本任务
local begin_mission = 62001
local time_limit = 2*3600
local activity_type=ActivityType.GoblinCarbon
local mission_type = MissionType_GoblinCarbonMission

local process = MissionCellIndex.CarbonGoblin_MaxMissionId
local last_id = MissionCellIndex.CarbonGoblin_Last

-------------------------------------------------------
function x000509_xOnDrop(player, params)	
	--放弃归0
	local cell_container= player:DataCellContainer()	
	cell_container:set_data_32(CellLogicType_MissionCell,last_id,-1,true)
	
	return ResultCode_ResultOK
end

function x000509_xOnReset(player, params)	
	if params["type"] == mission_type then
		--重置逻辑
		local cell_container= player:DataCellContainer()	
		cell_container:set_data_32(CellLogicType_MissionCell,process,0,true)
		cell_container:set_data_32(CellLogicType_MissionCell,last_id,-1,true)
		return ResultCode_ResultOK
	end
	return ResultCode_InvalidMissionType
end

function x000509_xOnAccept(player,params)
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
		
		local next_mission = x000509_GetNextMission(player)
		if next_mission == -1 then
			return ResultCode_NoMoreMission
		end
	
		proxy:OnAddMission(next_mission)
	
		return ResultCode_ResultOK
	end	
	return ResultCode_InvalidMissionType
end

function x000509_xOnFinishEnd(player,params)
	local config = LuaMtMissionManager():FindMission(params["id"])
	if config == nil then
		return ResultCode_InvalidConfigData
	end
	---最后一个任务给活跃度
	local last_mission_id = LuaMtMissionManager():GetLastMissionId(config:type(),config:group())
	if last_mission_id == config:id() then
		player:OnLuaFunCall_1(501,"AddActivityTimes",activity_type)
		
		--重置一下lastid，保证能重入
		local cell_container= player:DataCellContainer()	
		cell_container:set_data_32(CellLogicType_MissionCell,last_id,-1,true)	
		player:SendStatisticsResult(activity_type,true)	
		x000010_GRaidMissionOver(player,{["value"]=params["rtid"]})
		
		--首次
		local flag = cell_container:get_data_64(Cell64LogicType_NumberCell64, NumberCell64Index_GuideFirstFlag)
		if  cell_container:check_bit_data_64(flag,g_GuideFirstFlag.GoblinCarbon) == false then
			new_flag = cell_container:set_bit_data_64(flag,g_GuideFirstFlag.GoblinCarbon)
			cell_container:set_data_64(Cell64LogicType_NumberCell64, NumberCell64Index_GuideFirstFlag,new_flag,true)
		end
	end	
	return ResultCode_ResultOK
end

function x000509_xOnFinishBegin(player,params)
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
	
	local cell_container = player:DataCellContainer()	
	local process_flags = cell_container:get_data_32(CellLogicType_MissionCell, process);
	if cell_container == nil then
		return ResultCode_InvalidPlayerData
	end
	
	--只能领一次
	if cell_container:check_bit_data_32(process_flags,circle) == false then
	
		local new_flag = cell_container:set_bit_data_32(process_flags,circle)
		cell_container:set_data_32(CellLogicType_MissionCell, process,new_flag,true)
		--奖励经验
		local level_config = LuaMtActorConfig():GetLevelData(player:PlayerLevel())
		if level_config == nil then
			return ResultCode_InvalidConfigData
		end
		--奖励经验
		local exp_rate = config:exp_rate()
		local exp = level_config:goblin_exp()
		if exp_rate >= 0 then
			exp = exp_rate*exp;
		end
		player:AddItemByIdWithNotify(TokenType_Token_Exp,exp,ItemAddLogType_AddType_Activity,activity_type,1);
		
		--队长而外经验
		if player:IsTeamLeader() then
			player:GetMissionProxy():TeamLeaderExtraReward(config,exp);
		end
		
		--奖励金币
		local gold_rate = config:gold_rate()
		local gold = level_config:goblin_gold()
		if gold_rate >= 0 then
			gold = gold_rate*gold;
		end
		
		--print("ggggggg"..gold)
		--月卡
		if cell_container:check_bit_data(BitFlagCellIndex_MonthCardFlag) then
			gold=(1.0+0.05)*gold
		end	
		
		--print("ggggggg"..gold)
		
		player:AddItemByIdWithNotify(TokenType_Token_Gold,gold,ItemAddLogType_AddType_Activity,activity_type,1);	
		
		player:GetMissionProxy():OnMissionReward(params["id"])		
		--掉落活动检查
		player:CheckAcrDrop(activity_type)
	end
		
	return ResultCode_ResultOK
end

---------------------------------------------------------------

function x000509_GetMissionParams(player)
	 return {last_id,process}
end
function x000509_GetNextMission(player)
	local cell_container= player:DataCellContainer()	
	local lastmission = cell_container:get_data_32(CellLogicType_MissionCell,last_id)
		
	local next_mission = begin_mission
	if lastmission > 0 then
		next_mission=lastmission+1
	end
	--print(" lastmission"..lastmission.." begin_mission"..begin_mission.." next_mission"..next_mission)
	
	local config = LuaMtMissionManager():FindMission(next_mission)
	if config == nil then
		return -1 --说明没任务了
	end		
	return next_mission
end

function x000509_OnAddBegin(player,config)
	
	if config:type() ~=mission_type then
		return ResultCode_InvalidMissionType
	end
	local cell_container= player:DataCellContainer()	
	cell_container:set_data_32(CellLogicType_MissionCell,last_id,config:id(),true)

	return ResultCode_ResultOK
end

function x000509_OnClone(player, target, config)
	
	local target_container = target:DataCellContainer()	
	local last = target_container:get_data_32(CellLogicType_MissionCell, last_id);
	
	local player_container = player:DataCellContainer()
	player_container:set_data_32(CellLogicType_MissionCell, last_id, last, true);	
	--参与标记
	x000501_OnActivityJoin(player,activity_type)
	
	return ResultCode_ResultOK
	
end

function x000509_OnAddEnd(player,config)
	player:GetMissionProxy():Mission2Member(config:id())
	--参与标记
	x000501_OnActivityJoin(player,activity_type)
	
	return ResultCode_ResultOK
end	


function x000509_ResolveAllReward(player)
	
	local level_config = LuaMtActorConfig():GetLevelData(player:PlayerLevel())
	if level_config == nil then
		return 0,0
	end
	local exp = level_config:goblin_exp()		
	local gold = level_config:goblin_gold()	
	
	local final_gold = 0
	local final_exp = 0
		
	--结算每个任务奖励
	for i=1,5,1 do
		local config = LuaMtMissionManager():FindMission(begin_mission+i) 
		if config ~= nil then
			local gold_rate = config:gold_rate()
			if gold_rate >= 0 then
				final_gold = final_gold + gold_rate*gold
			end
			local exp_rate = config:exp_rate()
			if exp_rate >= 0 then
				final_exp= final_exp + exp_rate*exp
			end
		end
	end
	
	return final_gold,final_exp
end	

