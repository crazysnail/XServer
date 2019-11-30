local config = {}

-----------------------------------------------for server---------------------------------------------
function x002636_OnUpdateAcrConfig(params)
	config = acr_static_config[2636]
	return ResultCode_ResultOK
end
function x002636_IsAcrActive(player)

	if x000011_GOperateCheckOpenToLua(config) == false then
		return ResultCode_Activity_NotStart
	end	
	return ResultCode_ResultOK
	
end

function x002636_OnResolve(player,params)
	local result = params["result"]
	local index = params["index"]
	if index <1 or index > table.nums(config.items) then
		return
	end
	
	local offset = index -1
	local dc_container = player:DataCellContainer()
	local flag = dc_container:get_data_32(CellLogicType_AcRewardCell, g_acr_datacell_index.all_level_acr_flag)
	if result == 1 then
		local new_flag = dc_container:set_bit_data_32(flag,offset)
		dc_container:set_data_32(CellLogicType_AcRewardCell, g_acr_datacell_index.all_level_acr_flag,new_flag,true)
	else
		local new_flag = dc_container:set_bit_data_32(flag,offset+16)
		dc_container:set_data_32(CellLogicType_AcRewardCell, g_acr_datacell_index.all_level_acr_flag,new_flag,true)
	end
end

-----------------------------------------------
function x002636_OnRegisterFunction(proxy)
	if proxy ~= nil then
		proxy:RegFunc("xOnLevelUpTo"		,2636)			
	end
end

function x002636_xOnLevelUpTo(player,params)
	if x002636_IsAcrActive(player) ~= ResultCode_ResultOK then
		return ResultCode_InternalResult
	end
	
	local dc_container = player:DataCellContainer()
	if dc_container == nil then
		return ResultCode_UnknownError
	end
	
	local flag_data=dc_container:get_data_32(CellLogicType_AcRewardCell, g_acr_datacell_index.all_level_acr_flag)
	local level = params["value"]
	local offset = 0
	for k,v in pairs( config.items ) do
		offset = k-1
		--print("v[1][1] "..v[1][1])
		if level >= v[1][1] then
			if dc_container:check_bit_data_32(flag_data,offset) == false and dc_container:check_bit_data_32(flag_data,offset+16) == false then		
				LuaServer():SendS2GCommonMessage("S2GLevelAcrCheck",{k,v[1][2]},{player:Guid()},{})
			end
		end		
	end
	return ResultCode_ResultOK
end


function x002636_DoReward(player,params)
	
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
	local flag_data=dc_container:get_data_32(CellLogicType_AcRewardCell, g_acr_datacell_index.all_level_acr_flag)
	if dc_container:check_bit_data_32(flag_data,offset)== false then		
		return ResultCode_RewardConditionLimit
	end

	if dc_container:check_bit_data_32(flag_data,offset+16)== true then		
		return ResultCode_RewardAlready
	end

	--标记
	local new_flag_data = dc_container:set_bit_data_32(flag_data,offset+16)
	dc_container:set_data_32(CellLogicType_AcRewardCell, g_acr_datacell_index.all_level_acr_flag,new_flag_data,true)
	
	--给道具
	local items = table.nums(group[2])
	for i=1,items,1 do
		local id = group[2][i][1]
		local count = group[2][i][2]
		player:AddItemByIdWithNotify(id,count,ItemAddLogType_AddType_Acr,2636,1)
	end
	
	return ResultCode_ResultOK
end

function x002636_OnResetData(player,params)

	local dc_container = player:DataCellContainer()
	if dc_container == nil then
		return ResultCode_Invalid_Request
	end	
	
	dc_container:set_data_32( CellLogicType_AcRewardCell, g_acr_datacell_index.all_level_acr_flag, 0, true)

	return ResultCode_ResultOK
end


function x002636_OnRefreshDataToClient(player,params)
	
	if x002636_IsAcrActive(player) ~= ResultCode_ResultOK then
		return ResultCode_InternalResult
	end
	
	local server_info = LuaServer():GetServerInfo() 	
	
	local i=0
	local count_set ={}
	for k,v in pairs(acr_dynamic_config[config.script_id]) do
		count_set[k] = server_info:acr_data(v)	
	end
	
	player:SendLuaPacket("LevelAcrReply",count_set,{},{})

	return ResultCode_ResultOK
end

----------------------------------------------------------------------------------

--注意这个接口是提供给主线程的lua state调用的
function x002636_G_OnResetData(params)
	if x000011_GOperateCheckShowToLua(config) == false then
		return
	end
	local server_info = LuaServer():GetServerInfo() 	
	local last_serial = server_info:acr_data(g_acr_server_index.all_level_last_serial)
	if last_serial ~= config.serial then
		for k,v in pairs( acr_dynamic_config[config.script_id] ) do
			server_info:set_acr_data(v,0)
		end
		
		server_info:set_acr_data(g_acr_server_index.all_level_last_serial,config.serial)
	end
			
	print("x002636_G_OnResetData done old serial="..last_serial.." new serial="..config.serial)
	
end

--注意这个接口是提供给主线程的lua state调用的
function x002636_G_CheckBegin(params)	
end

--注意这个接口是提供给主线程的lua state调用的
function x002636_G_CheckEnd(params)
end


--------------------------------------------------------------for client-------------------------------
return config
