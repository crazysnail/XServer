
--通天塔任务
local begin_mission = 56001
local time_limit = 2*3600
local activity_type=ActivityType.Tower
local mission_type = MissionType_TowerMission

local process = MissionCellIndex.Tower_Process
local last_id = MissionCellIndex.Tower_Last
local tower_time = MissionCellIndex.Tower_Time

-------------------------------------------------------
function x000529_xOnDrop(player, params)	
	--放弃归0
	local cell_container= player:DataCellContainer()	
	cell_container:set_data_32(CellLogicType_MissionCell,last_id,-1,true)
	
	return ResultCode_ResultOK
end

function x000529_xOnReset(player, params)	
	if params["type"] == mission_type then
		--重置逻辑
		local cell_container= player:DataCellContainer()	
		cell_container:set_data_32(CellLogicType_MissionCell,process,0,true)
		cell_container:set_data_32(CellLogicType_MissionCell,last_id,-1,true)
		cell_container:set_data_32(CellLogicType_MissionCell,tower_time,0,true)

		return ResultCode_ResultOK
	end
	return ResultCode_InvalidMissionType
end

function x000529_xOnAccept(player,params)
	if params["type"] == mission_type then
		local proxy = player:GetMissionProxy()
		--检查身上有没有同类型任务，有就不生成新的
		local mission_map=proxy:GetMissionMap()	
		for k, mission in pairs(mission_map) do
			local config = mission:Config()
			if config:type() == mission_type then
				return ResultCode_ResultOK
			end
		end
		
		local next_mission = x000529_GetNextMission(player)
		if next_mission == -1 then
			return ResultCode_NoMoreMission
		end
	
		proxy:OnAddMission(next_mission)
	
		return ResultCode_ResultOK
	end	
	return ResultCode_InvalidMissionType
end

function x000529_xOnFinishEnd(player,params)
	local config = LuaMtMissionManager():FindMission(params["id"])
	if config == nil then
		return ResultCode_InvalidConfigData
	end
	---最后一个任务给活跃度
	local last_mission_id = LuaMtMissionManager():GetLastMissionId(config:type(),config:group())
	if last_mission_id == config:id() then
		player:OnLuaFunCall_1(501,"AddActivityTimes",activity_type)

		player:SendStatisticsResult(activity_type,true)	
		x000010_GRaidMissionOver(player,{["value"]=params["rtid"]})
		
	end	
	return ResultCode_ResultOK
end

function x000529_xOnFinishBegin(player,params)
	local mission = player:GetMissionById( params["id"] )
	if mission == nil then
		return ResultCode_InvalidMission
	end
	
	local config = mission:Config()
	if config == nil then
		return ResultCode_InvalidConfigData
	end
		
	local cell_container = player:DataCellContainer()	
	local old_circle = cell_container:get_data_32(CellLogicType_MissionCell, process)
	local circle = config:circle();
	if circle<old_circle then
		return ResultCode_InvalidConfigData
	end
	
	cell_container:set_data_32(CellLogicType_MissionCell, process,circle,true)
		
	player:GetMissionProxy():OnMissionReward(params["id"])	
	
	--掉落活动检查
	player:CheckAcrDrop(activity_type)
		
	--更新排行数据
	local tower_time = cell_container:get_data_32(CellLogicType_MissionCell,tower_time)
	LuaServer():SendS2GCommonMessage("S2GUpdateTowerInfo", { circle, tower_time }, { player:Guid()}, {});
		
	return ResultCode_ResultOK
end

---------------------------------------------------------------

function x000529_GetMissionParams(player)
	 return {last_id,process}
end
function x000529_GetNextMission(player)
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

function x000529_OnAddBegin(player,config)
	
	if config:type() ~=mission_type then
		return ResultCode_InvalidMissionType
	end
	local cell_container= player:DataCellContainer()	
	cell_container:set_data_32(CellLogicType_MissionCell,last_id,config:id(),true)

	return ResultCode_ResultOK
end

function x000529_OnClone(player, target, config)
	
	local target_container = target:DataCellContainer()	
	local last = target_container:get_data_32(CellLogicType_MissionCell, last_id);
	
	local player_container = player:DataCellContainer()
	player_container:set_data_32(CellLogicType_MissionCell, last_id, last, true);	
		
	return ResultCode_ResultOK
	
end

function x000529_OnAddEnd(player,config)
	player:GetMissionProxy():Mission2Member(config:id())
	return ResultCode_ResultOK
end	


function x000529_ResolveAllReward(player)
	return 0,0
end	

