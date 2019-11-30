--傻馒蓝色+1进阶包

function x001552_LuaCreateItem(player,item_data,config)
	return ResultCode_ResultOK
end

--添加到背包时候用，create之前，实际上这种道具没有创建实体，只是用来触发一段逻辑
function x001552_LuaAutoUse(player,config,count)
	return ResultCode_UnknownError
end

function x001552_LuaUseItem(player,item_data,config,target)

	if config:id() ~=20125048 then	
		return ResultCode_UnknownError
	end	
		
	--背包检查
	if player:BagLeftCapacity(ContainerType_Layer_ItemBag,1) == false then
		return ResultCode_BagIsFull
	end	
	
	if player:DelItemById(config:id(), 1, ItemDelLogType_DelType_UseItem, config:id()) == false then
		return ResultCode_CostItemNotEnough
	end
	
	player:AddItemByIdWithNotify(20230003,50,ItemAddLogType_AddType_UseItem,config:id(),1)
	player:AddItemByIdWithNotify(20231029,100,ItemAddLogType_AddType_UseItem,config:id(),1)
	
	return ResultCode_ResultOK
end