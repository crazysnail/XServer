--坐骑卷轴
function x001522_LuaCreateItem(player,item_data,config)
	return ResultCode_ResultOK
end

--添加到背包时候用，create之前，实际上这种道具没有创建实体，只是用来触发一段逻辑
function x001522_LuaAutoUse(player,config,count)
	return ResultCode_UnknownError
end

function x001522_LuaUseItem(player,item_data,config,target)	
	local id = config:id()
	
	--if id ~= 22000000 and id ~=  22000001 then
	--	return ResultCode_InvalidItemType
	--end

	local mount = LuaMtMonsterManager():FindMount(config:param(0))
	if mount == nil then
		return ResultCode_InvalidConfigData
	end

	local container = player:DataCellContainer()
	if container:check_data_32(CellLogicType_MountCell,mount:id()) == true then
		return ResultCode_MountAreadyGet
	end	
	
	if player:DelItemById(id, 1, ItemDelLogType_DelType_UseItem, config:id()) == false then
		return ResultCode_CostItemNotEnough
	end
	
	if container:push_data_32(CellLogicType_MountCell,mount:id(),true) == false then
		return ResultCode_MountAreadyGet
	end	
	
	local actors = player:Actors()
	for k,actor in pairs( actors ) do
		actor:OnBattleInfoChanged(false)
	end
	
	player:SendCommonReply("GetMountOk",{mount:id()},{},{})
	
	return ResultCode_ResultOK
end
