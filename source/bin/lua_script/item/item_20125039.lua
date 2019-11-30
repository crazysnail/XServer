--稀有英雄随机碎片包

local item_group={
20234007,20234012,20234016,20234028,20234011
}

function x001547_LuaCreateItem(player,item_data,config)
	return ResultCode_UnknownError;
end

--添加到背包时候用，create之前，实际上这种道具没有创建实体，只是用来触发一段逻辑
function x001547_LuaAutoUse(player,config,count)
	if config:id() ~=20125039 then	
		return ResultCode_UnknownError;
	end
	local seed = LuaServer():RandomNum(1,table.nums(item_group))
	player:AddItemByIdWithNotify(item_group[seed],80*count,ItemAddLogType_AddType_UseItem,config:id(),1)
	
	return ResultCode_ResultOK;
end

function x001547_LuaUseItem(player,item_data,config,target)
	return ResultCode_UnknownError;
end