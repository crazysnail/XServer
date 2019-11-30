local config = {}

-----------------------------------------------for server---------------------------------------------
function x002637_OnUpdateAcrConfig(params)
	config = acr_static_config[2637]
	return ResultCode_ResultOK
end
function x002637_IsAcrActive(player)

	if x000011_GOperateCheckOpenToLua(config) == false then
		return ResultCode_Activity_NotStart
	end	
	return ResultCode_ResultOK
	
end

function x002637_OnResolve(player,params)
	local result = params["result"]
	local index = params["index"]

	if result == 1 then
		if index <0 or index > table.nums(config.items) then
			return
		end
		local dc_container = player:DataCellContainer()
		local flag = dc_container:get_data_32(CellLogicType_AcRewardCell, g_acr_datacell_index.all_buy_acr_flag)
		local new_flag = dc_container:set_bit_data_32(flag,index)
		dc_container:set_data_32(CellLogicType_AcRewardCell, g_acr_datacell_index.all_buy_acr_flag,new_flag,true)
	else
		player:SendClientNotify("ResultCode_CountLimit",-1,-1)
	end
end

-----------------------------------------------
function x002637_OnRegisterFunction(proxy)
	
	if proxy ~= nil then
	end
end

function x002637_DoReward(player,params)
	
	local index = params["index"]
	local dc_container = player:DataCellContainer()
	if dc_container == nil then
		return ResultCode_Invalid_Request
	end

	local group ={}
	for k,v in pairs(config.items) do
		if v[1]== index then
			group=v
		end
	end
	
	if table.nums( group ) == 0 then
		return ResultCode_Invalid_Request
	end
	
	local server_info = LuaServer():GetServerInfo() 	
	if index ~= server_info:acr_data(g_acr_server_index.all_buy_index) then
		return player:SendClientNotify("acr_notify_000",-1,-1)
	end
	
	if server_info:acr_data(g_acr_server_index.all_buy_count) >= group[2] then
		return player:SendClientNotify("TimesLimit",-1,-1)
	end
	dc_container:add_data_32(CellLogicType_AcRewardCell, times_index,1,true)
	
	--给道具
	local items = table.nums(group[3])
	for i=1,items,1 do
		local id = group[3][i][1]
		local count = group[3][i][2]
		player:AddItemByIdWithNotify(id,count,ItemAddLogType_AddType_Acr,2637,1)
	end
	
	return ResultCode_ResultOK
end

function x002637_OnResetData(player,params)

	local dc_container = player:DataCellContainer()
	if dc_container == nil then
		return ResultCode_Invalid_Request
	end	
	
	dc_container:set_data_32( CellLogicType_AcRewardCell, g_acr_datacell_index.all_buy_acr_flag, 0, true)
	dc_container:set_data_32( CellLogicType_ActionData, g_action_data_index.all_buy_count, 0, true)

	return ResultCode_ResultOK
end


--------------------------------------------------------------for client-------------------------------
return config
