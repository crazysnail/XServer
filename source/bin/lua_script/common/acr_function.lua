
g_last_serial={}		
g_last_serial[2601]=g_reset_time_index.acr_2601_last_id	
g_last_serial[2602]=g_reset_time_index.acr_2602_last_id	
g_last_serial[2603]=g_reset_time_index.acr_2603_last_id	
g_last_serial[2604]=g_reset_time_index.acr_2604_last_id	
g_last_serial[2605]=g_reset_time_index.acr_2605_last_id	
g_last_serial[2606]=g_reset_time_index.acr_2606_last_id	
g_last_serial[2607]=g_reset_time_index.acr_2607_last_id	
g_last_serial[2608]=g_reset_time_index.acr_2608_last_id	
g_last_serial[2609]=g_reset_time_index.acr_2609_last_id	
g_last_serial[2610]=g_reset_time_index.acr_2610_last_id	
g_last_serial[2611]=g_reset_time_index.acr_2611_last_id	
g_last_serial[2612]=g_reset_time_index.acr_2612_last_id	
g_last_serial[2613]=g_reset_time_index.acr_2613_last_id	
g_last_serial[2614]=g_reset_time_index.acr_2614_last_id	
g_last_serial[2615]=g_reset_time_index.acr_2615_last_id	
g_last_serial[2616]=g_reset_time_index.acr_2616_last_id	
g_last_serial[2617]=g_reset_time_index.acr_2617_last_id	
g_last_serial[2618]=g_reset_time_index.acr_2618_last_id	
g_last_serial[2619]=g_reset_time_index.acr_2619_last_id	
g_last_serial[2620]=g_reset_time_index.acr_2620_last_id	
g_last_serial[2621]=g_reset_time_index.acr_2621_last_id	
g_last_serial[2622]=g_reset_time_index.acr_2622_last_id	
g_last_serial[2623]=g_reset_time_index.acr_2623_last_id	
g_last_serial[2624]=g_reset_time_index.acr_2624_last_id	
g_last_serial[2625]=g_reset_time_index.acr_2625_last_id	
g_last_serial[2626]=g_reset_time_index.acr_2626_last_id	
g_last_serial[2627]=g_reset_time_index.acr_2627_last_id	
g_last_serial[2628]=g_reset_time_index.acr_2628_last_id	
g_last_serial[2629]=g_reset_time_index.acr_2629_last_id	
g_last_serial[2630]=g_reset_time_index.acr_2630_last_id	
g_last_serial[2631]=g_reset_time_index.acr_2631_last_id	
g_last_serial[2632]=g_reset_time_index.acr_2632_last_id	
g_last_serial[2633]=g_reset_time_index.acr_2633_last_id	
g_last_serial[2634]=g_reset_time_index.acr_2634_last_id	
g_last_serial[2635]=g_reset_time_index.acr_2635_last_id	
g_last_serial[2636]=g_reset_time_index.acr_2636_last_id	
g_last_serial[2637]=g_reset_time_index.acr_2637_last_id	
g_last_serial[2638]=g_reset_time_index.acr_2638_last_id	
g_last_serial[2639]=g_reset_time_index.acr_2639_last_id	
g_last_serial[2640]=g_reset_time_index.acr_2640_last_id	
g_last_serial[2641]=g_reset_time_index.acr_2641_last_id	
g_last_serial[2642]=g_reset_time_index.acr_2642_last_id	
g_last_serial[2643]=g_reset_time_index.acr_2643_last_id	


function x000011_GAcrstatusUpdateNew(player,params)
	local dc_container = player:DataCellContainer()
	if dc_container == nil then		
		return ResultCode_UnknownError
	end
	local flag = player:GetParams64("acr_activie")
	if flag <= 0 then
		flag = 0
	end	
	
	local script_id = params["value"]
	local user_data = params["user_data"]
	local check_show = params["check_show"]
	local check_open = params["check_open"]
	
	if check_show == 1 then
		player:SendLuaPacket("msg_acr_data", {script_id,user_data,check_open},{},{})
	else
		player:SendLuaPacket("msg_acr_close", {script_id},{},{})	
	end
	
	--LuaServer():LuaLog("serial="..params["serial_id"].." scriptid "..script_id.." check_show "..check_show.." check_open "..check_open.." guid:"..player:Guid(), g_log_level.LL_INFO)
	--print("serial="..params["serial_id"].." scriptid "..script_id.." check_show "..check_show.." check_open "..check_open.." guid:"..player:Guid())
	
	return ResultCode_ResultOK
end


function x000011_GAcrstatusCheckResetNewEx(player,script_id)
	if script_id == -1 then
		for k,v in pairs( acr_static_config ) do
			if type(v) == "table" then
				--print("ddddddddddddddddddd".. v.script_id)
				x000011_GAcrstatusCheckResetNew(player, v.script_id)
			end
		end	
	else
		x000011_GAcrstatusCheckResetNew(player,script_id)
	end
	return true
end

function x000011_GAcrstatusCheckResetNew(player,script_id)
	local object = acr_static_config[script_id]
	if object== nil then
		return false
	end
	
	local last_serial = g_last_serial[script_id]
	if last_serial == nil then
		return false
	end
	
	local dc_container = player:DataCellContainer()
	if dc_container == nil then		
		return false
	end
	
	last_serial = dc_container:get_data_32(CellLogicType_ResetTime,g_last_serial[script_id])

	--新一期的活动开启了	
	--print("object.script_id"..object.script_id)
	if x000011_GOperateCheckShowToLua(object) and last_serial ~= object.serial  then
		--重置数据	
		player:OnLuaFunCall_n(script_id, "OnResetData", {})		
		dc_container:set_data_32(CellLogicType_ResetTime,g_last_serial[script_id],object.serial,true)
		
		--LuaServer():LuaLog("acr reset data! script_id="..script_id.." object.serial"..object.serial.." guid:"..player:Guid(), g_log_level.LL_INFO)
		--print("acr reset data! script_id="..script_id.." object.serial"..object.serial.." guid:"..player:Guid())
		return true
	end

	return false
end

function x000011_GAcrstatusCheckUpdateNewEx(player,script_id)
	if script_id == -1 then
		for k,v in pairs( acr_static_config ) do
			if type(v) == "table" then
				--print("ddddddddddddddddddd".. v.script_id)
				x000011_GAcrstatusCheckUpdateNew(player, v.script_id)
			end
		end	
	else
		x000011_GAcrstatusCheckUpdateNew(player,script_id)
	end
	return true
end

function x000011_GAcrstatusCheckUpdateNew(player,script_id)
	local object = acr_static_config[script_id]
	if object== nil then
		return ResultCode_UnknownError
	end

	--特殊活动需要重载
	local user_data = -1
	if script_id == 2638 then
		local server_info = LuaServer():GetServerInfo() 	
		local index = server_info:acr_data(g_acr_server_index.quik_buy_index)
		if index <= 0 then
			index = 1
		end
		user_data = index
		
		--print("zzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzz index......."..index)
		
		--先同步一下库存
		local count = server_info:acr_data(g_acr_server_index.quik_buy_count)
		local dc_container = player:DataCellContainer()
		dc_container:set_data_32(CellLogicType_ActionData, g_action_data_index.quik_buy_count,count,true)
	end
	
	if object.items == nil then
		x000011_GAcrstatusUpdateNew(player,{["value"]=script_id,["check_open"]=0,["check_show"]=0,["user_data"]=user_data,["serial_id"]=object.serial})
	else
		local check_show = x000011_GOperateCheckShowToLua(object)
		local check_open = x000011_GOperateCheckOpenToLua(object)
		if check_open then
			x000011_GAcrstatusUpdateNew(player,{["value"]=script_id,["check_open"]=1,["check_show"]=1,["user_data"]=user_data,["serial_id"]=object.serial})	
		elseif check_show then
			x000011_GAcrstatusUpdateNew(player,{["value"]=script_id,["check_open"]=0,["check_show"]=1,["user_data"]=user_data,["serial_id"]=object.serial})	
		else
			x000011_GAcrstatusUpdateNew(player,{["value"]=script_id,["check_open"]=0,["check_show"]=0,["user_data"]=user_data,["serial_id"]=object.serial})
		end
	end
	
	return ResultCode_ResultOK
end



function x000011_GAcrstatusUpdate(player,params)
	local dc_container = player:DataCellContainer()
	if dc_container == nil then		
		return ResultCode_UnknownError
	end
	local flag = player:GetParams64("acr_activie")
	if flag <= 0 then
		flag = 0
	end	
	
	local script_id = params["value"]
	local config_check = params["config_check"]
	local user_data = params["user_data"]
	
	if config_check == 1 then
		player:SendLuaPacket("msg_acr_data", {script_id,user_data},{},{})
	else
		player:SendLuaPacket("msg_acr_close", {script_id},{},{})	
	end
	
	--LuaServer():LuaLog("scriptid="..script_id .." ..config_check "..config_check.." guid:"..player:Guid(),g_log_level.LL_INFO)
	--print("scriptid="..script_id .." ..config_check "..config_check.." guid:"..player:Guid())
		
	return ResultCode_ResultOK
end

