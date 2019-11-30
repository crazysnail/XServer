--任务卷轴
function x001542_LuaCreateItem(player,item_data,config)
	return ResultCode_ResultOK
end

--添加到背包时候用，create之前，实际上这种道具没有创建实体，只是用来触发一段逻辑
function x001542_LuaAutoUse(player,config,count)
	return ResultCode_UnknownError
end

function x001542_LuaUseItem(player,item_data,config,target)	
	if config == nil then
		return ResultCode_InvalidConfigData
	end
	
	local id = config:id()
	local mission_id = config:param(0)
	
	local config = LuaMtMissionManager():FindMission(mission_id)
	if config == nil then
		return ResultCode_InvalidConfigData
	end
	
	--检查身上有没有同类型任务，有就不生成新的
	local mission_map=player:GetMissionProxy():GetMissionMap()	
	for k, mission in pairs(mission_map) do
		local config = mission:Config()
		if config:type() == MissionType_ScrollMission then
			return ResultCode_MissionCountLimit
		end
	end	
		
	if player:DelItemById(id, 1, ItemDelLogType_DelType_UseItem, config:id()) == false then
		return ResultCode_CostItemNotEnough
	end
	
	player:GetMissionProxy():OnAddMission(mission_id);
	
	return ResultCode_ResultOK
end
