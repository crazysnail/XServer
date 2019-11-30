--随机宝箱
function x001526_LuaCreateItem(player,item_data,config)
	return ResultCode_ResultOK
end

--添加到背包时候用，create之前，实际上这种道具没有创建实体，只是用来触发一段逻辑
function x001526_LuaAutoUse(player,config,count)
	local id = config:id()
	
	local item_drop = LuaMtItemPackageManager():GenDropFromGroup(config:param(0))
	if item_drop == nil then
		return ResultCode_InvalidConfigData
	end
				
	if player:DelItemById(id, 1, ItemDelLogType_DelType_UseItem, config:id()) == false then
		return ResultCode_CostItemNotEnough;
	end
	
	player:AddItemByIdWithNotify(item_drop:item_id(),item_drop:item_amount(),ItemAddLogType_AddType_UseItem,config:id(),1)
	
	return ResultCode_ResultOK;
end

function x001526_LuaUseItem(player,item_data,config,target)	
	return ResultCode_UnknownError
end
