
--高级试炼宝箱

function x001517_LuaCreateItem(player,item_data,config)
	return ResultCode_UnknownError;
end

--添加到背包时候用，create之前，实际上这种道具没有创建实体，只是用来触发一段逻辑
function x001517_LuaAutoUse(player,config,count)
	if config:id() ~=20125022 then	
		return ResultCode_UnknownError;
	end

	--背包检查
	if player:BagLeftCapacity(ContainerType_Layer_EquipAndItemBag,1) == false then
		return ResultCode_BagIsFull
	end
	
	local tb_item = LuaMtDrawManager():GetDrawList(DrawGroup_HellBigDraw,player:PlayerLevel())

	local allweight = 0;
	for key,iter in pairs(tb_item)	do
		allweight=allweight+iter:item_weight()
	end
	
	for i=0,count-1,1 do 
		local seed_weight = LuaServer():RandomNum(1,allweight)
		local base_weight = 0;
		for key,iter in pairs(tb_item)	do
			base_weight = base_weight+iter:item_weight()
			if  seed_weight<=base_weight then
				player:AddItemByIdWithNotify(iter:item_id(),iter:item_count(),ItemAddLogType_AddType_Activity,ActivityType.Hell,1)
				break
			end
		end
	end		
	return ResultCode_ResultOK;
end

function x001517_LuaUseItem(player,item_data,config,target)
	return ResultCode_UnknownError;
end
