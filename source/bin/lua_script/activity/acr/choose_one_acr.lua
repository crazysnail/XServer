local config = acr_static_config[2634]

-----------------------------------------------for server---------------------------------------------
function x002634_OnUpdateAcrConfig(params)
	return ResultCode_ResultOK
end

function x002634_IsAcrActive(player)

	--开关
	if config.switch_open == 0 then 
		return ResultCode_Activity_NotStart
	end	
		--时间
	if x000011_GOperateCheckOpenToLua(config) == false then
		return ResultCode_Activity_NotStart
	end	
	return ResultCode_ResultOK
	
end



function x002634_OnResolve(player)
	local dc_container = player:DataCellContainer()
	if dc_container == nil then		
		return
	end
	local config_check= 0
	if x002631_IsAcrActive(player) == ResultCode_ResultOK then
		config_check = 1
	end
	if config_check == 1 then	
		local day_charge = dc_container:get_data_32(CellLogicType_ActionData, g_action_data_index.day_charge_count) 		
		local flag_data=dc_container:get_data_32(CellLogicType_AcRewardCell, g_acr_datacell_index.choose_one_flag)		
		for k,v in pairs(config.items) do 
			if v[1]<=day_charge and dc_container:check_bit_data_32(flag_data,charge_days) == false then
				local new_data = dc_container:set_bit_data_32(flag_data,k-1)
				dc_container:set_data_32(CellLogicType_AcRewardCell, g_acr_datacell_index.choose_one_flag,new_data,true)
			end
		end
	end		
end

-----------------------------------------------
function x002634_OnRegisterFunction(proxy)
	
	if proxy ~= nil then
		proxy:RegFunc("xOnCharge"			,2634)	
	end
end

function x002634_DoReward(player,params)
	
	local index = params["index"]
	local value = params["value"]
	local dc_container = player:DataCellContainer()
	if dc_container == nil then
		return ResultCode_Invalid_Request
	end

	if index <0 or index >= table.nums(config.items) then
		return ResultCode_Invalid_Request
	end
			
	local group = config.items[index+1]
	if table.nums( group ) == 0 then
		return ResultCode_Invalid_Request
	end
	
	if value<0 or value >= table.nums(group[2]) then
		return ResultCode_Invalid_Request
	end
	
	local flag_data=dc_container:get_data_32(CellLogicType_AcRewardCell, g_acr_datacell_index.choose_one_flag)
	if dc_container:check_bit_data_32(flag_data,index)== false then		
		return ResultCode_RewardConditionLimit
	end

	if dc_container:check_bit_data_32(flag_data,index+16)== true then		
		return ResultCode_RewardAlready
	end

	--标记
	local new_flag_data = dc_container:set_bit_data_32(flag_data,index+16)
	dc_container:set_data_32(CellLogicType_AcRewardCell, g_acr_datacell_index.choose_one_flag,new_flag_data,true)
	
	--给道具
	local id = group[2][value+1][1]
	local count = group[2][value+1][2]
	player:AddItemByIdWithNotify(id,count,ItemAddLogType_AddType_Acr,2634,1)	
	
	return ResultCode_ResultOK
end

function x002634_xOnCharge(player,params)
	x002634_OnResolve(player)
	return ResultCode_ResultOK
end


function x002634_OnResetData(player,params)

	local dc_container = player:DataCellContainer()
	if dc_container == nil then
		return ResultCode_Invalid_Request
	end	
	
	dc_container:set_data_32( CellLogicType_AcRewardCell, g_acr_datacell_index.choose_one_flag, 0, true)

	return ResultCode_ResultOK
end

--------------------------------------------------------------for client-------------------------------
return config
