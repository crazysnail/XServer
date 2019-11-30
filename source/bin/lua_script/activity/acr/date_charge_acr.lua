--单位时间内累计充值

local config = {}

-----------------------------------------------for server---------------------------------------------
function x002616_OnUpdateAcrConfig(params)
	config = acr_static_config[2616]
	return ResultCode_ResultOK
end

function x002616_IsAcrActive(player)

	if x000011_GOperateCheckOpenToLua(config) == false then
		return ResultCode_Activity_NotStart
	end	
	return ResultCode_ResultOK
	
end

function x002616_OnResolve(player)

	local dc_container = player:DataCellContainer()
	if dc_container == nil then		
		return
	end
	local config_check= 0
	if x002616_IsAcrActive(player) == ResultCode_ResultOK then
		config_check = 1
	end
	if config_check == 1 then	
		local count = table.nums(config.items)
		local check_count = dc_container:get_data_32(CellLogicType_ActionData, g_action_data_index.date_charge_count)
		for index=0,count-1,1 do 
			local flag_data=dc_container:get_data_32(CellLogicType_AcRewardCell, g_acr_datacell_index.date_charge_flag)
			if dc_container:check_bit_data_32(flag_data,index) == false and check_count >= config.items[index+1][1][1] then
				local new_data = dc_container:set_bit_data_32(flag_data,index)
				dc_container:set_data_32(CellLogicType_AcRewardCell, g_acr_datacell_index.date_charge_flag,new_data,true)
			end
		end
	end
	
end

function x002616_DoReward(player,params)
	
	if x002616_IsAcrActive(player)  ~= ResultCode_ResultOK then
		return ResultCode_Activity_NotStart
	end
	
	local index = params["index"]
	local dc_container = player:DataCellContainer()
	if dc_container == nil then
		return ResultCode_Invalid_Request
	end
			
	if index <1 or index > table.nums(config.items) then
		return
	end
		
	local group = config.items[index]	
	if table.nums( group ) == 0 then
		return ResultCode_Invalid_Request
	end
	
	--背包检查
	if x000010_CheckBagEx(player,group[2],{}) == false then
		return ResultCode_InternalResult
	end
	
	local offset = index -1
	local flag_data=dc_container:get_data_32(CellLogicType_AcRewardCell, g_acr_datacell_index.date_charge_flag)
	if dc_container:check_bit_data_32(flag_data,offset)== false then		
		return ResultCode_RewardConditionLimit
	end

	if dc_container:check_bit_data_32(flag_data,offset+16)== true then		
		return ResultCode_RewardAlready
	end

	--标记
	local new_flag_data = dc_container:set_bit_data_32(flag_data,offset+16)
	dc_container:set_data_32(CellLogicType_AcRewardCell, g_acr_datacell_index.date_charge_flag,new_flag_data,true)
	
	--给道具
	local items = table.nums(group[2])
	for i=1,items,1 do
		local id = group[2][i][1]
		local count = group[2][i][2]
		player:AddItemByIdWithNotify(id,count,ItemAddLogType_AddType_Acr,2616,1)
	end
		
	return ResultCode_ResultOK
end

---------------------------------------------------
function x002616_OnRegisterFunction(proxy)
	
	if proxy ~= nil then
		proxy:RegFunc("xOnCharge"			,2616)	
	end
end

function x002616_xOnCharge(player,params)
	if x002616_IsAcrActive(player) == ResultCode_ResultOK then
		local dc_container = player:DataCellContainer()
		if dc_container ~= nil then		
			local action_data= params["value"]
			dc_container:add_data_32(CellLogicType_ActionData, g_action_data_index.date_charge_count,action_data,true)
		end
	end	
	x002616_OnResolve(player)
	return ResultCode_ResultOK
end

function x002616_OnResetData(player,params)

	local dc_container = player:DataCellContainer()
	if dc_container == nil then
		return ResultCode_Invalid_Request
	end	
	dc_container:set_data_32(CellLogicType_ActionData, g_action_data_index.date_charge_count,0,true)
	dc_container:set_data_32(CellLogicType_AcRewardCell, g_acr_datacell_index.date_charge_flag,0,true)
	return ResultCode_ResultOK
end

--------------------------------------------------------------for client-------------------------------
return config
