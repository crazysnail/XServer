--修复石
function x001519_LuaCreateItem(player,item_data,config)
	return ResultCode_ResultOK
end

--添加到背包时候用，create之前，实际上这种道具没有创建实体，只是用来触发一段逻辑
function x001519_LuaAutoUse(player,config,count)
	return ResultCode_UnknownError
end

function x001519_LuaUseItem(player,item_data,config,target)	
	local id = config:id()
	if id ~= 20140004 then
		return ResultCode_InvalidItemType
	end
	
	if player:DelItemById(id, 1, ItemDelLogType_DelType_UseItem, config:id()) == false then
		return ResultCode_CostItemNotEnough;
	end
	
	local proxy = player:GetSimpleImpactSet()
	proxy:AddSimpleImpact(ProtectRobImpact, config:param(0),-1,-1,true)
		
	return ResultCode_ResultOK
end
