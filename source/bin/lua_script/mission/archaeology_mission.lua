
--考古任务
local activity_type=ActivityType.Ancholage
local mission_type = MissionType_ArchaeologyMission

function x000505_xOnDrop(player, params)	
	return ResultCode_ResultOK
end

function x000505_xOnReset(player, params)
	if params["type"] == mission_type then
		--重置逻辑
		local cell_container= player:DataCellContainer()	
		cell_container:set_data_32(CellLogicType_MissionCell,MissionCellIndex.Archaeology_LastDrop,-1,true)
		return ResultCode_ResultOK
	end

	return ResultCode_InvalidMissionType
end

function x000505_xOnAccept(player, params)
	if params["type"] == mission_type then
		--检查身上有没有同类型任务，有就不生成新的
		local mission_map=player:GetMissionProxy():GetMissionMap()
		for k, mission in pairs(mission_map) do
			config = mission:Config()
			if config:type() == MissionType_ArchaeologyMission then
				return ResultCode_MissionCountLimit
			end
		end
		
		local mission_id = LuaMtMissionManager():GenArchaeologyMission()	
		player:GetMissionProxy():OnAddMission(mission_id)
		return ResultCode_ResultOK
	end
	return ResultCode_InvalidMissionType
end

function x000505_xOnFinishEnd(player,params)
	--结束条判定
	local ac_config = LuaActivityManager():FindActivity(activity_type)
	if ac_config ~= nil then
		local ltimes = ac_config:limit_times()
		local done_times = player:OnLuaFunCall_1(501, "GetActivityTimes", activity_type)
		if done_times >= ltimes then
			player:SendStatisticsResult(activity_type,true)
		end
	end
	
	return ResultCode_ResultOK
end

function x000505_xOnFinishBegin(player,params)
	if params["type"] == mission_type then
	
		local cell_container= player:DataCellContainer()
	
		--藏宝图掉落计算
		local bingo= true
		--local lastdrop= cell_container:get_data_32(CellLogicType_MissionCell,MissionCellIndex.Archaeology_LastDrop)
		--if lastdrop <=0 then
		--	bingo=true
		--elseif LuaServer():RandomNum(1,100)<=50 then		
		--	bingo=true
		--end	
		
		if bingo then
			--小图
			player:AddItemByIdWithNotify(20125001,1,ItemAddLogType_AddType_Activity,activity_type,1)
			cell_container:set_data_32(CellLogicType_MissionCell,MissionCellIndex.Archaeology_LastDrop,1,true)
			
			--任务次数	
			player:OnLuaFunCall_1(501,"AddActivityTimes",activity_type);
			
		else
			cell_container:set_data_32(CellLogicType_MissionCell,MissionCellIndex.Archaeology_LastDrop,0,true)
		end	
		
		player:GetMissionProxy():OnMissionReward(params["id"])
		--掉落活动检查
		player:CheckAcrDrop(activity_type)
		
		return ResultCode_ResultOK		
	end
	
	return ResultCode_InvalidMissionType	
end

-------------------------------------------------------

function x000505_GetMissionParams(player)
	 return {}
end

function x000505_GetNextMission(player)
	return -1
end


function x000505_OnAddBegin(player,config)
	
	if config:type() ~= MissionType_ArchaeologyMission then
		return ResultCode_InvalidMissionType
	end
	
	if 	config:finish_type() ~= FinishType_RandomNpc then
		return ResultCode_InvalidFinishType
	end
	
	return ResultCode_ResultOK
end

function x000505_OnClone(player, target, config)
	--参与标记
	x000501_OnActivityJoin(player,activity_type)
	return ResultCode_ResultOK
end

function x000505_OnAddEnd(player,config)
	--参与标记
	x000501_OnActivityJoin(player,activity_type)
	return ResultCode_ResultOK
end	



