local config = {}

-----------------------------------------------for server---------------------------------------------
function x002631_OnUpdateAcrConfig(params)
	config = acr_static_config[2631]
	return ResultCode_ResultOK
end

function x002631_IsAcrActive(player)

	if x000011_GOperateCheckOpenToLua(config) == false then
		return ResultCode_Activity_NotStart
	end	
	return ResultCode_ResultOK
	
end


function x002631_OnResolve(player)
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
		local count = table.nums(config.items)
		local flag_data=dc_container:get_data_32(CellLogicType_AcRewardCell, g_acr_datacell_index.continue_charge_flag)
		local charge_days = dc_container:get_data_32(CellLogicType_AcRewardCell, g_acr_datacell_index.continue_charge_days)
		if charge_days <0 then
			charge_days = 0
		end
		if charge_days < count then		
			local condition = config.items[charge_days+1][1][1]		
			local offset = charge_days
			if day_charge>=condition and dc_container:check_bit_data_32(flag_data,offset) == false then	
				print("day_charge "..day_charge.." charge_days "..charge_days.." condition "..condition)
				local new_data = dc_container:set_bit_data_32(flag_data,offset)				
				dc_container:set_data_32(CellLogicType_AcRewardCell, g_acr_datacell_index.continue_charge_flag,new_data,true)
			end
		end
	end		
end


function x002631_DoReward(player,params)
	
	local index = params["index"]
	local dc_container = player:DataCellContainer()
	if dc_container == nil then
		return ResultCode_Invalid_Request
	end

	local charge_days = dc_container:get_data_32(CellLogicType_AcRewardCell, g_acr_datacell_index.continue_charge_days)
	if index>charge_days+1 then
		return ResultCode_Invalid_Request
	end

	local group = config.items[index]
	if table.nums(group) == 0 then
		return ResultCode_InternalResult
	end
	----背包检查
	--if x000010_CheckBagEx(player,group[2],{}) == false then
	--	return ResultCode_InternalResult
	--end
	
	local offset =index -1
	local flag_data=dc_container:get_data_32(CellLogicType_AcRewardCell, g_acr_datacell_index.continue_charge_flag)
	if dc_container:check_bit_data_32(flag_data,offset)== false then		
		return ResultCode_RewardConditionLimit
	end
	
	if dc_container:check_bit_data_32(flag_data,offset+16)== true then		
		return ResultCode_RewardAlready
	end
	
	--标记
	local new_flag_data = dc_container:set_bit_data_32(flag_data,offset+16)
	dc_container:set_data_32(CellLogicType_AcRewardCell, g_acr_datacell_index.continue_charge_flag,new_flag_data,true)
			
	--给道具
	
	for i=1,table.nums(group[2]),1 do
		local id = group[2][i][1]
		local count = group[2][i][2]
		player:AddItemByIdWithNotify(id,count,ItemAddLogType_AddType_Acr,2631,1)
	end
	
	--特殊奖励
	for i=1,table.nums(group[4]),1 do
		local id = group[4][i][1]
		local count = group[4][i][2]
		player:AddItemByIdWithNotify(id,count,ItemAddLogType_AddType_Acr,2631,1)
	end
	
	return ResultCode_ResultOK
end

-----------------------------------------------
function x002631_OnRegisterFunction(proxy)
	
	if proxy ~= nil then
		proxy:RegFunc("xOnCharge"	,2631)		
	end
end

function x002631_xOnCharge(player,params)
	x002631_OnResolve(player)	
	return ResultCode_ResultOK
end

function x002631_xOnNewDay(player,params)
	local dc_container = player:DataCellContainer()
	if dc_container == nil then
		return ResultCode_Invalid_Request
	end	
	if config.items == nil then
		return ResultCode_InternalResult
	end
	--重置前做个领奖检查，有的话就发邮件		
	local charge_days = dc_container:get_data_32(CellLogicType_AcRewardCell, g_acr_datacell_index.continue_charge_days)
	local index = charge_days +1
	local group = config.items[index]
	if group ~= nil and table.nums(group) ~= 0 then
		local flag_data=dc_container:get_data_32(CellLogicType_AcRewardCell, g_acr_datacell_index.continue_charge_flag)
		if dc_container:check_bit_data_32(flag_data,charge_days) then
			if dc_container:check_bit_data_32(flag_data,charge_days+16)== false then	
				x002631_DoReward(player,{["index"]=index})
			end
			--更新天计数
			dc_container:add_data_32(CellLogicType_AcRewardCell, g_acr_datacell_index.continue_charge_days,1,true)
		end
	end
	
	charge_days = dc_container:get_data_32(CellLogicType_AcRewardCell, g_acr_datacell_index.continue_charge_days)

end

function x002631_OnResetData(player,params)

	local dc_container = player:DataCellContainer()
	if dc_container == nil then
		return ResultCode_Invalid_Request
	end	
	
	dc_container:set_data_32(CellLogicType_AcRewardCell, g_acr_datacell_index.continue_charge_flag,0,true)
	dc_container:set_data_32(CellLogicType_AcRewardCell, g_acr_datacell_index.continue_charge_days, 0,true)	
			
	--print("dddddddddddddddddddddddddddddddddd")

	return ResultCode_ResultOK
end


--------------------------------------------------------------for client-------------------------------
return config
