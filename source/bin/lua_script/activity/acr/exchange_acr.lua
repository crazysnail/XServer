local config = {}

-----------------------------------------------for server---------------------------------------------
function x002632_OnUpdateAcrConfig(params)
	config = acr_static_config[2632]
	return ResultCode_ResultOK
end

function x002632_IsAcrActive(player)

	if x000011_GOperateCheckOpenToLua(config) == false then
		return ResultCode_Activity_NotStart
	end	
	return ResultCode_ResultOK
	
end

function x002632_OnResolve(player)
end

-----------------------------------------------
function x002632_OnRegisterFunction(proxy)
	
	if proxy ~= nil then
	end
end

function x002632_DoReward(player,params)
	if x002632_IsAcrActive(player) ~= ResultCode_ResultOK then
		return ResultCode_ResultOK
	end

	local index = params["index"]
	local dc_container = player:DataCellContainer()
	if dc_container == nil then
		return ResultCode_Invalid_Request
	end

	--最多9种
	local numbers = table.nums(config.items)
	if numbers >9 then
		return ResultCode_InvalidConfigData
	end
	
	--数组是1起始
	if index <1 or index > numbers then
		return ResultCode_Invalid_Request
	end
	
	local group =config.items[index]	
	if table.nums( group ) == 0 then
		return ResultCode_Invalid_Request
	end
		
	--背包检查
	if x000010_CheckBagEx(player,group[4],{}) == false then
		return ResultCode_BagIsFull
	end
	
	local times_index = acr_dynamic_config[config.script_id][index]	
	local times=dc_container:get_data_32(CellLogicType_AcRewardCell, times_index)
	if times>=group[1][1] then				
		return ResultCode_TimesLimit
	end
	
	--print("ddddddddddddddddddddddddddddd1 times"..times)
	--扣道具
	for k,v in pairs(group[2]) do
		local have_count = player:GetBagItemCount(v[1])
		if have_count < v[2] then
			return ResultCode_CostItemNotEnough
		end
	end
	
	for k,v in pairs(group[2]) do
		if player:DelItemById(v[1],v[2],ItemDelLogType_DelType_Acr,2632) == false then
			return ResultCode_CrystalNotEnough
		end
	end
	dc_container:add_data_32(CellLogicType_AcRewardCell, times_index,1,true)
	
	--print("ddddddddddddddddddddddddddddd1 times"..times)
	--给道具
	local items = table.nums(group[4])
	for i=1,items,1 do
		local id = group[4][i][1]
		local count = group[4][i][2]
		player:AddItemByIdWithNotify(id,count,ItemAddLogType_AddType_Acr,2632,1)
	end
	
	return ResultCode_ResultOK
end

function x002632_OnResetData(player,params)

	if acr_dynamic_config[config.script_id] == nil then
		return ResultCode_InternalResult
	end
	
	local dc_container = player:DataCellContainer()
	if dc_container == nil then
		return ResultCode_Invalid_Request
	end	
	
	for k,v in pairs(acr_dynamic_config[config.script_id]) do
		dc_container:set_data_32( CellLogicType_AcRewardCell, v, 0, true)
	end

	return ResultCode_ResultOK
end
--------------------------------------------------------------for client-------------------------------
return config
