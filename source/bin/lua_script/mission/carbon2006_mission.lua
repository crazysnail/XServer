
--团队副本任务
local time_limit = 2*3600
local mission_type = MissionType_Carbon2006Mission
local activity_type=ActivityType.TeamCarbon2006

local process = MissionCellIndex.Carbon2006_Process
local last_id = MissionCellIndex.Carbon2006_Last
local begin_mission = -1

---------------------------------------------------------
function x000526_xOnDrop(player, params)	
	--放弃归0
	local cell_container= player:DataCellContainer()	
	cell_container:set_data_32(CellLogicType_MissionCell,last_id,-1,true)
	
	return ResultCode_ResultOK
end
function x000526_xOnFinishEnd(player, params)	
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
		x000010_GRaidMissionOver(player,{["value"]=params["rtid"]})
		
		--延迟出结算
		local arrange_statistic = player:GetParams32("arrange_statistic")
		print("arrange_statistic----"..arrange_statistic)
		if arrange_statistic == -1 then
			player:SendStatisticsResult(activity_type,true)			
		end			
	end	
	return ResultCode_ResultOK
end

function x000526_xOnReset(player, params)	
	if params["type"] == mission_type then
		--重置逻辑
		local cell_container= player:DataCellContainer()	
		cell_container:set_data_32(CellLogicType_MissionCell,process,0,true)
		cell_container:set_data_32(CellLogicType_MissionCell,last_id,-1,true)
		return ResultCode_ResultOK
	end
	return ResultCode_InvalidMissionType
end

function x000526_xOnAccept(player,params)
	if params["type"] == mission_type then
		--检查身上有没有同类型任务，有就不生成新的
		local mission_map=player:GetMissionProxy():GetMissionMap()	
		for k, mission in pairs(mission_map) do
			local config = mission:Config()
			if config:type() == mission_type then
				return ResultCode_MissionCountLimit
			end
		end
		
		local next_mission = x000526_GetNextMission(player)
		if next_mission==-1 then
			return ResultCode_NoMoreMission
		end
		
		player:GetMissionProxy():OnAddMission(next_mission)
		return ResultCode_ResultOK
	end	
	return ResultCode_InvalidMissionType
end


function x000526_xOnFinishBegin(player,params)
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

	--print("xxxxxxxxxxxxxxxxxxxxxxxxxxxx2222dddddddd"..circle.."flag"..process_flags.."player"..player:Guid())
	--只能领一次
	if cell_container:check_bit_data_32(process_flags,circle) == true then
		return ResultCode_ResultOK
	end			
	local new_flag = cell_container:set_bit_data_32(process_flags,circle)
	cell_container:set_data_32(CellLogicType_MissionCell, process,new_flag,true)
	
	player:GetMissionProxy():OnMissionReward(params["id"])	
	--掉落活动检查
	player:CheckAcrDrop(activity_type)
	return ResultCode_ResultOK		
end

--------------------------------------------------
function x000526_GetMissionParams(player)
	 return {last_id,process}
end
function x000526_GetNextMission(player)
	
	local cell_container= player:DataCellContainer()	
	local lastmission = cell_container:get_data_32(CellLogicType_MissionCell,last_id)
	
	local next_mission = begin_mission
	if lastmission > 0 then
		next_mission=lastmission+1
	end
	local config = LuaMtMissionManager():FindMission(next_mission)
	if config == nil then
		return -1 --说明没任务了
	end
	
	--print(" lastmission"..lastmission.." begin_mission"..begin_mission.." next_mission"..next_mission)
	return next_mission

end

function x000526_OnAddBegin(player,config)
	
	if config:type() ~=mission_type then
		return ResultCode_InvalidMissionType
	end
	local cell_container= player:DataCellContainer()	
	cell_container:set_data_32(CellLogicType_MissionCell,last_id,config:id(),true)

	return ResultCode_ResultOK
end

function x000526_OnClone(player, target, config)

	local target_container = target:DataCellContainer()	
	local last = target_container:get_data_32(CellLogicType_MissionCell, last_id);
	
	local player_container = player:DataCellContainer()
	player_container:set_data_32(CellLogicType_MissionCell, last_id, last, true);	
		
	return ResultCode_ResultOK
end

function x000526_OnAddEnd(player,config)
	player:GetMissionProxy():Mission2Member(config:id())
	return ResultCode_ResultOK
end	


