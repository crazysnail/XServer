local config = {}

-----------------------------------------------for server---------------------------------------------
function x002643_OnUpdateAcrConfig(params)
	config = acr_static_config[2643]
	return ResultCode_ResultOK
end

function x002643_IsAcrActive(player)

	if x000011_GOperateCheckOpenToLua(config) == false then
		return ResultCode_Activity_NotStart
	end	
	return ResultCode_ResultOK
	
end

function x002643_OnResolve(player)
end


-----------------------------------------------
function x002643_OnRegisterFunction(proxy)
	
	if proxy ~= nil then
		 
	end
end

function x002643_DoReward(player,params)
	--做个开启保护
	if x002643_IsAcrActive(player) ~= ResultCode_ResultOK then
		LuaServer():LuaLog("error , ChargeReward failed ! acr close script_id="..config.script_id.." player guid="..player:Guid(),g_log_level.LL_ERROR)
		return ResultCode_Invalid_Request
	end

	local dc_container = player:DataCellContainer()
	if dc_container == nil then
		return ResultCode_Invalid_Request
	end

	local index = params["index"]
	if index < 1 or index > table.nums(config.items) then
		return ResultCode_Invalid_Request
	end
	local group = config.items[index]	
	if table.nums( group ) == 0 then
		return ResultCode_Invalid_Request
	end
	
	--类型判定	
	if group[1][1] == 1 then
		if dc_container:check_bit_data(BitFlagCellIndex_MonthCardFlag) == false then
			return player:SendClientNotify("ac_notify_023",-1,-1)	
		end
	elseif group[1][1]==2 then
		if dc_container:check_bit_data(BitFlagCellIndex_LifeCardFlag) == false then
			return player:SendClientNotify("ac_notify_023",-1,-1)
		end
	else
		return ResultCode_InvalidConfigData
	end
	
	--	
	local times_index = acr_dynamic_config[config.script_id][index]	
	local times=dc_container:get_data_32(CellLogicType_AcRewardCell, times_index)
	if times >= group[1][3] then
		return player:SendClientNotify("TimesLimit",-1,-1)
	end
		
	--扣符石
	if player:DelItemById(TokenType_Token_Crystal,group[1][2],ItemDelLogType_DelType_Acr,2643) == false then
		return ResultCode_CrystalNotEnough
	end
	
	dc_container:add_data_32(CellLogicType_AcRewardCell, times_index,1,true)	


	--给道具
	local items = table.nums(group[2])
	for i=1,items,1 do	
		local id = group[2][i][1]
		local count = group[2][i][2]
		--print("dddddddddddddddddddd"..id)
		player:AddItemByIdWithNotify(id,count,ItemAddLogType_AddType_Acr,2643,1)
	end
	
	return ResultCode_ResultOK
end


function x002643_xOnNewDay(player,params)
	return x002643_OnResetData(player,params)
end

function x002643_OnResetData(player,params)

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
