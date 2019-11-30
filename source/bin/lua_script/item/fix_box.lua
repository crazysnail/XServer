--随机宝箱
function x001535_LuaCreateItem(player,item_data,config)
	return ResultCode_ResultOK
end

--添加到背包时候用，create之前，实际上这种道具没有创建实体，只是用来触发一段逻辑
function x001535_LuaAutoUse(player,config,count)
	return ResultCode_UnknownError
end

function x001535_LuaUseItem(player,item_data,config,target)	
	local id = config:id()
	
	local package = LuaMtItemPackageManager():GetPackage(config:param(0))
	if package == nil then
		return ResultCode_InvalidConfigData
	end
			
	--背包检查
	if player:BagLeftCapacity(ContainerType_Layer_ItemBag,package:ItemCount()) == false then
		return ResultCode_BagIsFull
	end
	
	if player:DelItemById(id, 1, ItemDelLogType_DelType_UseItem, config:id()) == false then
		return ResultCode_CostItemNotEnough
	end
	
	package:AddToPlayer(player)
	
	return ResultCode_ResultOK
end
