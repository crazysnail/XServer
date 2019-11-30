local config = {}

-----------------------------------------------for server---------------------------------------------
function x002627_OnUpdateAcrConfig(params)
	config = acr_static_config[2627]
	return ResultCode_ResultOK
end
function x002627_IsAcrActive(player)

	if x000011_GOperateCheckOpenToLua(config) == false then
		return ResultCode_Activity_NotStart
	end	
	return ResultCode_ResultOK
	
end

-----------------------------------------------
function x002627_OnRegisterFunction(proxy)
	
	if proxy ~= nil then
		proxy:RegFunc("xOnNewMonth"			,2627)	
	end
end

function x002627_OnResolve(player,params)
	
	--做个开启保护
	if x002627_IsAcrActive() ~= ResultCode_ResultOK then
		LuaServer():LuaLog("error , 1 ChargeReward failed ! acr close script_id="..config.script_id.." player guid="..player:Guid(),g_log_level.LL_ERROR)	
		return 
	end
	local index = params["index"]
	local effect_value = params["effect_value"]
	local dc_container = player:DataCellContainer()
	if dc_container == nil then
		LuaServer():LuaLog("error , 2 ChargeReward failed ! script_id="..config.script_id.." player guid="..player:Guid().." index "..index,g_log_level.LL_ERROR)	
		return 
	end

	local group ={}
	for k,v in pairs(config.items) do
		if v[1][1]== index then
			group=v
		end
	end
	
	if table.nums( group ) == 0 then
		LuaServer():LuaLog("error , 3 ChargeReward failed ! script_id="..config.script_id.." player guid="..player:Guid().." index "..index,g_log_level.LL_ERROR)	
		return 
	end
	
	--子索引
	if effect_value<1 or effect_value >table.nums(acr_dynamic_config[config.script_id]) then
		LuaServer():LuaLog("error , 4 ChargeReward failed ! script_id="..config.script_id.." player guid="..player:Guid().." index "..index,g_log_level.LL_ERROR)	
		return 
	end
	
	local times_index = acr_dynamic_config[config.script_id][effect_value]	
	local times=dc_container:get_data_32(CellLogicType_AcRewardCell, times_index)
	if times>=group[1][2] then
		LuaServer():LuaLog("error , 5 ChargeReward failed times limit ! script_id="..config.script_id.." player guid="..player:Guid().." index "..index,g_log_level.LL_ERROR)	
		return
	end
	dc_container:add_data_32(CellLogicType_AcRewardCell, times_index,1,true)
	
	local offset = effect_value -1
	local reward_flag =dc_container:get_data_32(CellLogicType_AcRewardCell, g_acr_datacell_index.ylb_reward_flag)	
	local new_flag = dc_container:set_bit_data_32(reward_flag,offset)
	dc_container:set_data_32(CellLogicType_AcRewardCell, g_acr_datacell_index.ylb_reward_flag,new_flag,true)	
	
	return ResultCode_ResultOK
end


function x002627_DoReward(player,params)
	
	local index = params["index"]
	local dc_container = player:DataCellContainer()
	if dc_container == nil then
		return ResultCode_Invalid_Request
	end

	local numbers = table.nums(config.items)
	
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
	
	local offset = index -1
	local reward_flag =dc_container:get_data_32(CellLogicType_AcRewardCell, g_acr_datacell_index.ylb_reward_flag)	
	if dc_container:check_bit_data_32(reward_flag,offset)== false then		
		return ResultCode_RewardConditionLimit
	end
	
	--
	if dc_container:check_bit_data_32(reward_flag,offset+16)== true then		
		return ResultCode_RewardAlready
	end
	
	--标记
	--次数引发的标记重置检查
	local times_index = acr_dynamic_config[config.script_id][index]	
	local times=dc_container:get_data_32(CellLogicType_AcRewardCell, times_index)
	if times<group[1][2] then
		local new_flag_data = dc_container:reset_bit_data_32(reward_flag,offset)
		dc_container:set_data_32(CellLogicType_AcRewardCell, g_acr_datacell_index.ylb_reward_flag,new_flag_data,true)
	else
		local new_flag_data = dc_container:set_bit_data_32(reward_flag,offset+16)
		dc_container:set_data_32(CellLogicType_AcRewardCell, g_acr_datacell_index.ylb_reward_flag,new_flag_data,true)
	end
	
		
	--给道具
	local items = table.nums(group[2])
	for i=1,items,1 do
		local id = group[2][i][1]
		local count = group[2][i][2]
		player:AddItemByIdWithNotify(id,count,ItemAddLogType_AddType_Acr,2627,1)
	end
	
	return ResultCode_ResultOK
end

function x002627_xOnNewMonth(player,params)
	return x002627_OnResetData(player,params)
end

function x002627_OnResetData(player,params)

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

	dc_container:set_data_32(CellLogicType_AcRewardCell, g_acr_datacell_index.ylb_reward_flag,0,true)
		
	return ResultCode_ResultOK
end
--------------------------------------------------------------for client-------------------------------
return config
