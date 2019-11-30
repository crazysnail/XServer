local config = {}

-----------------------------------------------for server---------------------------------------------
function x002638_OnUpdateAcrConfig(params)
	config = acr_static_config[2638]
	return ResultCode_ResultOK
end

function x002638_IsAcrActive(player)

	if x000011_GOperateCheckOpenToLua(config) == false then
		return ResultCode_Activity_NotStart
	end	
	return ResultCode_ResultOK
	
end

function x002638_OnResolve(player,params)
	
	player:SetParams32("quik_buy",-1)
	
	local result = params["result"]
	if result == 1 then
		local index = params["index"]
		local dc_container = player:DataCellContainer()
		if dc_container == nil then
			return
		end

		if index <1 or index > table.nums(config.items) then
			return
		end
		
		local group = config.items[index]
		if table.nums( group ) == 0 then
			return
		end
		
		local offset = index -1
		local flag_data = dc_container:get_data_32(CellLogicType_AcRewardCell, g_acr_datacell_index.quik_buy_acr_flag)
		if dc_container:check_bit_data_32(flag_data,offset) then		
			return
		end
		
		--扣钱
		local cost = group[1][1]
		if player:DelItemById(TokenType_Token_Crystal, cost, ItemDelLogType_DelType_Acr,2638) == false then
			player:SendClientNotify("ResultCode_CrystalNotEnough",-1,-1)
			return
		end
		
		local new_flag = dc_container:set_bit_data_32(flag_data,offset)
		dc_container:set_data_32(CellLogicType_AcRewardCell, g_acr_datacell_index.quik_buy_acr_flag,new_flag,true)
		
		--给道具
		local items = table.nums(group[2])
		for i=1,items,1 do
			local id = group[2][i][1]
			local count = group[2][i][2]
			player:AddItemByIdWithNotify(id,count,ItemAddLogType_AddType_Acr,2638,1)
		end
	
	else
		player:SendClientNotify("ResultCode_CountLimit",-1,-1)
	end	
end

-----------------------------------------------
function x002638_OnRegisterFunction(proxy)
	
	if proxy ~= nil then
	end
end

function x002638_DoReward(player,params)
	
	if x002638_IsAcrActive(player) ~= ResultCode_ResultOK then
		return ResultCode_InternalResult
	end
	
	if player:GetParams32("quik_buy") == 1 then
		return ResultCode_InternalResult
	end
	
	local index = params["index"]
	local dc_container = player:DataCellContainer()
	if dc_container == nil then
		return ResultCode_Invalid_Request
	end

	if index <1 or index > table.nums(config.items) then
		return ResultCode_Invalid_Request
	end
	
	local group = config.items[index]
	if table.nums( group ) == 0 then
		return ResultCode_InvalidConfigData
	end
	
	local offset = index -1
	local flag_data = dc_container:get_data_32(CellLogicType_AcRewardCell, g_acr_datacell_index.quik_buy_acr_flag)
	if dc_container:check_bit_data_32(flag_data,offset) then		
		return player:SendClientNotify("acr_notify_000",-1,-1)
	end
	
	--符石检查
	if player:GetPlayerToken(TokenType_Token_Crystal) < group[1][1] then
        return ResultCode_CrystalNotEnough
    end
	
	local server_info = LuaServer():GetServerInfo() 	
	if index ~= server_info:acr_data(g_acr_server_index.quik_buy_index) then
		return player:SendClientNotify("acr_notify_000",-1,-1)
	end
	
	local count = server_info:acr_data(g_acr_server_index.quik_buy_count) 
	if count <=0 then
		return player:SendClientNotify("acr_notify_001",-1,-1)
	end

	--加个逻辑锁
	player:SetParams32("quik_buy",1)
	--消息
	LuaServer():SendS2GCommonMessage("S2GQuikBuyAcrJoin",{index},{player:Guid()},{})
	
	return ResultCode_ResultOK
end


function x002638_OnResetData(player,params)

	local dc_container = player:DataCellContainer()
	if dc_container == nil then
		return ResultCode_Invalid_Request
	end	
	
	player:SetParams32("quik_buy",-1)
	dc_container:set_data_32( CellLogicType_AcRewardCell, g_acr_datacell_index.quik_buy_acr_flag, 0, true)
	dc_container:set_data_32( CellLogicType_ActionData, g_action_data_index.quik_buy_count, 0, true)

	return ResultCode_ResultOK
end


--注意这个接口是提供给主线程的lua state调用的

function x002638_G_OnResetData(params)
	--print("dddddddddddddddddddddddddddddddd")
	if x000011_GOperateCheckShowToLua(config) == false then
		return
	end
	local server_info = LuaServer():GetServerInfo() 	
	local last_serial = server_info:acr_data(g_acr_server_index.quik_buy_last_serial	)
	if last_serial ~= config.serial then
		server_info:set_acr_data(g_acr_server_index.quik_buy_last_serial,config.serial)
		server_info:set_acr_data(g_acr_server_index.quik_buy_index,0)
		server_info:set_acr_data(g_acr_server_index.quik_buy_count,0)		
	end
	print("x002638_G_OnResetData done old serial="..last_serial.." new serial="..config.serial)
end

--注意这个接口是提供给主线程的lua state调用的
function x002638_G_CheckBegin(params)

	if x000011_GOperateCheckOpenToLua(config) == false then
		return
	end	
	local server_info = LuaServer():GetServerInfo() 	
	local index = server_info:acr_data(g_acr_server_index.quik_buy_index)
	if index <=0 then
		index = 1
	else
		index = index +1
	end	
	
	if index > table.nums(config.items) then
		--通知结束
		LuaServer():BroadcastG2SCommonMessageToPlayer("G2SQuikBuyAcrOver",{config.script_id},{},{})
		return
	end
	
	local group = config.items[index]
	server_info:set_acr_data(g_acr_server_index.quik_buy_index,index)	
	server_info:set_acr_data(g_acr_server_index.quik_buy_count,group[1][2])
	
	--广播开始
	LuaServer():BroadcastG2SCommonMessageToPlayer("G2SQuikBuyAcrBegin",{config.script_id,group[1][2],index},{},{})
	
end

--注意这个接口是提供给主线程的lua state调用的
function x002638_G_CheckEnd(params)
	local index = params["index"]
	if index >= table.nums(config.items) then
		--通知结束
		LuaServer():BroadcastG2SCommonMessageToPlayer("G2SQuikBuyAcrOver",{config.script_id},{},{})		
	end	
end

--------------------------------------------------------------for client-------------------------------
return config
