--金币

function x001541_LuaCreateItem(player,item_data,config)
	return ResultCode_UnknownError;
end

--添加到背包时候用，create之前，实际上这种道具没有创建实体，只是用来触发一段逻辑
function x001541_LuaAutoUse(player,config,count)
	if config:id() ~=20233002 then	
		return ResultCode_UnknownError;
	end
	player:AddItemByIdWithNotify(TokenType_Token_Gold,count*10000,ItemAddLogType_AddType_UseItem,config:id(),1)
	return ResultCode_ResultOK;
end

function x001541_LuaUseItem(player,item_data,config,target)
	return ResultCode_UnknownError;
end