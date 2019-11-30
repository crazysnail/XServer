
--id规则约束，等级高的任务id必须高于等级低的任务id
--等级触发
local mission_type = MissionType_LevelMission

-------------------------------------------------------
function x000517_xOnDrop(player, params)	
	return ResultCode_ResultOK
end
function x000517_xOnFinishBegin(player, params)	
	player:GetMissionProxy():OnMissionReward(params["id"])
	
	local mission = player:GetMissionById( params["id"] )
	if mission == nil then
		return ResultCode_InvalidMission
	end
	
	local config = mission:Config()	
	if config:type() == mission_type then	
		local cell_container= player:DataCellContainer()	
		--保证不能高于历史最高进度，防止刷任务
		local old = cell_container:get_data_32(CellLogicType_MissionCell,MissionCellIndex.LevelMissionId)
		if old<config:id() then
			cell_container:set_data_32(CellLogicType_MissionCell,MissionCellIndex.LevelMissionId,config:id(),true)
		end
	end	
	
	return ResultCode_ResultOK
end

function x000517_xOnReset(player, params)
	return ResultCode_ResultOK
end

function x000517_xOnAccept(player,params)
	if params["type"] == mission_type then		
		local cell_container = player:DataCellContainer()
		if cell_container ~= nil then
			local mission_proxy = player:GetMissionProxy()
			local mission_map = mission_proxy:GetMissionMap()
			for k, mission in pairs(mission_map) do
				local config = mission:Config()
				if config:type() == mission_type then
					return ResultCode_ResultOK
				end						
			end
			
			local last_id = cell_container:get_data_32(CellLogicType_MissionCell, MissionCellIndex.LevelMissionId)
			local missions = LuaMtMissionManager():GetLevelMissions(last_id, player:PlayerLevel(), player:GetCamp())	
			for k,v in pairs(missions) do	
				--每次只加一个,id规则约束，等级高的任务id必须高于等级低的任务id
				local old = cell_container:get_data_32(CellLogicType_MissionCell,MissionCellIndex.LevelMissionId)
				print("lv mission old id ".. old)
				if v>old then
					mission_proxy:OnAddMission(v)
					break
				end
			end
		end
		return ResultCode_ResultOK
	end
	return ResultCode_InvalidMissionType
end

function x000517_xOnFinishEnd(player,params)
	--继续同级别的其他等级任务
	player:GetMissionProxy():OnAcceptMission(MissionType_LevelMission)
	return ResultCode_ResultOK
end

------------------------------------------------------------------------------
function x000517_GetMissionParams(player)
	 return {}
end
function x000517_GetNextMission(player)
	return -1
end

function x000517_OnAddBegin(player,config)
	return ResultCode_ResultOK
end

function x000517_OnClone(player, target, config)
	return ResultCode_ResultOK
end

function x000517_OnAddEnd(player,config)
	--行为任务完成检查
	if config:finish_type()==FinishType_Action then
		player:SetParams32("CheckMission",1)
	end
	return ResultCode_ResultOK
end	


