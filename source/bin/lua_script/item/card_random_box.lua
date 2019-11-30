--随机宝箱
function x001544_LuaCreateItem(player,item_data,config)
	return ResultCode_ResultOK
end

--添加到背包时候用，create之前，实际上这种道具没有创建实体，只是用来触发一段逻辑
function x001544_LuaAutoUse(player,config,count)	
	return ResultCode_UnknownError
end

function x001544_LuaUseItem(player,item_data,config,target)	

	local id = config:id()
	local drop_index = config:param(0)
		
	if player:DelItemById(id, 1, ItemDelLogType_DelType_UseItem, config:id()) == false then
		return ResultCode_CostItemNotEnough;
	end
	if drop_index == 1 then		--金币
		local count = LuaServer():RandomNum(500000,1000000)
		player:AddItemByIdWithNotify(TokenType_Token_Gold,count,ItemAddLogType_AddType_UseItem,config:id(),1)
	elseif drop_index == 2 then	--附魔材料
		local count = LuaServer():RandomNum(20,40)
		player:AddItemByIdWithNotify(20235002,count,ItemAddLogType_AddType_UseItem,config:id(),1)
	elseif drop_index == 3 then	--宝石
		local item = {30101003,30102003,30103003,30104003,30105003,30106003,30107003,30108003,30108003}
		local seed =  LuaServer():RandomNum(1,table.nums(item))		
		local item_id = item[seed]
		seed = LuaServer():RandomNum(1,10000)
		local count = 1
		if seed <=6000 then
			count =1
		elseif seed <=9000 then
			count =2
		else
			count =3
		end	
		player:AddItemByIdWithNotify(item_id,count,ItemAddLogType_AddType_UseItem,config:id(),1)
	elseif drop_index == 4 then	--助溶剂
		local seed =  LuaServer():RandomNum(1,10000)		
		local count = 1
		if seed <=6000 then
			count =1
		elseif seed <=9000 then
			count =2
		else
			count =3
		end	
		player:AddItemByIdWithNotify(20236001,count,ItemAddLogType_AddType_UseItem,config:id(),1)
	end
	
	return ResultCode_ResultOK
end
