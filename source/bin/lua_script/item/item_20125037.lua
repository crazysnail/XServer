--一星英雄随机碎片包

local item_group={
20234003,20234006,20234009,20234010,20234014,20234022,20234024,20234029
}

function x001545_LuaCreateItem(player,item_data,config)
	return ResultCode_UnknownError;
end

--添加到背包时候用，create之前，实际上这种道具没有创建实体，只是用来触发一段逻辑
function x001545_LuaAutoUse(player,config,count)
	if config:id() ~=20125037 then	
		return ResultCode_UnknownError;
	end
	local seed = LuaServer():RandomNum(1,table.nums(item_group))
	player:AddItemByIdWithNotify(item_group[seed],30*count,ItemAddLogType_AddType_UseItem,config:id(),1)
	
	return ResultCode_ResultOK;
end

function x001545_LuaUseItem(player,item_data,config,target)
	return ResultCode_UnknownError;
end