local config = {}

-----------------------------------------------for server---------------------------------------------
function x002639_OnUpdateAcrConfig(params)
	config = acr_static_config[2639]
	return ResultCode_ResultOK
end

function x002639_IsAcrActive(player)

	if x000011_GOperateCheckOpenToLua(config) == false then
		return ResultCode_Activity_NotStart
	end	
	return ResultCode_ResultOK
	
end

function x002639_OnResolve(player)
end

-----------------------------------------------
function x002639_OnRegisterFunction(proxy)
	
	if proxy ~= nil then
	end
end

function x002639_DoReward(player,params)
	
	local index = params["index"]
	
	local dc_container = player:DataCellContainer()
	if dc_container == nil then
		return ResultCode_Invalid_Request
	end
	
	--最多15种
	local numbers = table.nums(config.items)
	if numbers >15 then
		return ResultCode_InvalidConfigData
	end
	
	if index <1 or index > numbers then
		return ResultCode_Invalid_Request
	end
	
	local group = config.items[index]
	if table.nums( group ) == 0 then
		return ResultCode_Invalid_Request
	end
		
	--背包检查
	if x000010_CheckBag(player,1,group[2]) == false then
		return ResultCode_InternalResult
	end
	
	local offset = index -1
	local flag_index = g_acr_datacell_index.czlb_2_flag
	local flag_data=dc_container:get_data_32(CellLogicType_AcRewardCell, flag_index)
	if dc_container:check_bit_data_32(flag_data,offset) then				
		return ResultCode_TimesLimit
	end
	
	--扣符石
	if player:DelItemById(TokenType_Token_Crystal,group[1][1],ItemDelLogType_DelType_Acr,2639) == false then
		return ResultCode_CrystalNotEnough
	end
	
	local newflag = dc_container:set_bit_data_32(flag_data,offset)
	dc_container:set_data_32(CellLogicType_AcRewardCell, flag_index,newflag,true)
	
	--给道具
	local items = table.nums(group[2])
	for i=1,items,1 do
		local id = group[2][i][1]
		local count = group[2][i][2]
		player:AddItemByIdWithNotify(id,count,ItemAddLogType_AddType_Acr,2639,1)
	end
	
	return ResultCode_ResultOK
end

function x002639_xOnNewDay(player,params)
	return x002639_OnResetData(player,params)
end

function x002639_OnResetData(player,params)

	local dc_container = player:DataCellContainer()
	if dc_container == nil then
		return ResultCode_Invalid_Request
	end	

	dc_container:set_data_32( CellLogicType_AcRewardCell, g_acr_datacell_index.czlb_2_flag, 0, true)

	return ResultCode_ResultOK
end

--------------------------------------------------------------for client-------------------------------
return config
