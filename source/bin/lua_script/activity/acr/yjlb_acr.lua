local config = {}

-----------------------------------------------for server---------------------------------------------
function x002630_OnUpdateAcrConfig(params)
	config = acr_static_config[2630]
	return ResultCode_ResultOK
end

function x002630_IsAcrActive(player)

	if x000011_GOperateCheckOpenToLua(config) == false then
		return ResultCode_Activity_NotStart
	end	
	return ResultCode_ResultOK
	
end

function x002630_OnResolve(player)
end

-----------------------------------------------
function x002630_OnRegisterFunction(proxy)
	
	if proxy ~= nil then
	end
end

function x002630_DoReward(player,params)

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
	
	local group = config.items[index]	
	if table.nums( group ) == 0 then
		return ResultCode_Invalid_Request
	end

	--背包检查
	if x000010_CheckBagEx(player,group[2],{}) == false then
		return ResultCode_InternalResult
	end
	
	local times_index = acr_dynamic_config[config.script_id][index]	
	local times=dc_container:get_data_32(CellLogicType_AcRewardCell, times_index)
	if times>=group[1][2] then				
		return ResultCode_TimesLimit
	end
	
	--扣符石
	if player:DelItemById(TokenType_Token_Crystal,group[1][1],ItemDelLogType_DelType_Acr,2630) == false then
		return ResultCode_CrystalNotEnough
	end
	
	dc_container:add_data_32(CellLogicType_AcRewardCell, times_index,1, true)
	
	--给道具
	local items = table.nums(group[2])
	for i=1,items,1 do
		local id = group[2][i][1]
		local count = group[2][i][2]
		player:AddItemByIdWithNotify(id,count,ItemAddLogType_AddType_Acr,2630,1)
	end
	
	return ResultCode_ResultOK
end

function x002630_OnResetData(player,params)

	local dc_container = player:DataCellContainer()
	if dc_container == nil then
		return ResultCode_Invalid_Request
	end	
	
	if acr_dynamic_config[config.script_id] ~= nil then
		for k,v in pairs(acr_dynamic_config[config.script_id]) do
			dc_container:set_data_32( CellLogicType_AcRewardCell, v, 0, true)
		end
	end
	return ResultCode_ResultOK
end


--------------------------------------------------------------for client-------------------------------
return config
