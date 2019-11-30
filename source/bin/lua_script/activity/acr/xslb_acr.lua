local config = {}

-----------------------------------------------for server---------------------------------------------
function x002628_OnUpdateAcrConfig(params)
	config = acr_static_config[2628]
	return ResultCode_ResultOK
end
function x002628_IsAcrActive(player)

	if x000011_GOperateCheckOpenToLua(config) == false then
		return ResultCode_Activity_NotStart
	end	
	return ResultCode_ResultOK
	
end

-----------------------------------------------
function x002628_OnRegisterFunction(proxy)
	
	if proxy ~= nil then
		proxy:RegFunc("xOnLevelUpTo"		,2628)	
		proxy:RegFunc("xAddHero"			,2628)	
	end
end

function x002628_OnResolve(player,params)
	
	--做个开启保护
	if x002628_IsAcrActive() ~= ResultCode_ResultOK then
		LuaServer():LuaLog("error , ChargeReward failed ! acr close script_id="..config.script_id.." player guid="..player:Guid(),g_log_level.LL_ERROR)	
		return
	end
	local index = params["index"]
	local effect_value = params["effect_value"]
	local dc_container = player:DataCellContainer()
	if dc_container == nil then
		LuaServer():LuaLog("error , ChargeReward failed ! script_id="..config.script_id.." player guid="..player:Guid(),g_log_level.LL_ERROR)	
		return
	end

	local group ={}
	for k,v in pairs(config.items) do
		if v[1][1]== index then
			group=v
		end
	end
	
	if table.nums( group ) == 0 then
		LuaServer():LuaLog("error , ChargeReward failed ! script_id="..config.script_id.." player guid="..player:Guid(),g_log_level.LL_ERROR)	
		return
	end
	
	--子索引
	if effect_value<1 or effect_value >table.nums(acr_dynamic_config[config.script_id]) then
		LuaServer():LuaLog("error , ChargeReward failed ! script_id="..config.script_id.." player guid="..player:Guid(),g_log_level.LL_ERROR)
		return
	end
	
	local times_index = acr_dynamic_config[config.script_id][effect_value]	
	local times=dc_container:get_data_32(CellLogicType_AcRewardCell, times_index)
	if times<=-60 then --倒计时结束,允许60s的超时
		LuaServer():LuaLog("error , ChargeReward failed ! time out script_id="..config.script_id.." player guid="..player:Guid(),g_log_level.LL_ERROR)
		return
	end
	
	--领奖标记
	local offset =effect_value -1
	local flag_data=dc_container:get_data_32(CellLogicType_AcRewardCell, g_acr_datacell_index.xslb_reward_flag)	
	local new_data = dc_container:set_bit_data_32(flag_data,offset)
	dc_container:set_data_32(CellLogicType_AcRewardCell, g_acr_datacell_index.xslb_reward_flag,new_data,true)
	
	return ResultCode_ResultOK
end


function x002628_DoReward(player,params)
	
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
	local reward_flag =dc_container:get_data_32(CellLogicType_AcRewardCell, g_acr_datacell_index.xslb_reward_flag)	
	if dc_container:check_bit_data_32(reward_flag,offset)== false then		
		return ResultCode_RewardConditionLimit
	end
	
	--
	if dc_container:check_bit_data_32(reward_flag,offset+16)== true then		
		return ResultCode_RewardAlready
	end
	
	--标记
	local new_flag_data = dc_container:set_bit_data_32(reward_flag,offset+16)
	dc_container:set_data_32(CellLogicType_AcRewardCell, g_acr_datacell_index.xslb_reward_flag,new_flag_data,true)	
		
	--关闭礼包,通过设置时间无穷大来表示今天已经刷过了	
	local times_index = acr_dynamic_config[config.script_id][index]	
	dc_container:set_data_32(CellLogicType_AcRewardCell, times_index,999999,true)	

	local flag_data=dc_container:get_data_32(CellLogicType_AcRewardCell, g_acr_datacell_index.xslb_flag)
	local new_flag = dc_container:reset_bit_data_32(flag_data,offset)	
	dc_container:set_data_32(CellLogicType_AcRewardCell, g_acr_datacell_index.xslb_flag,new_flag, true)
	
	--给道具
	local items = table.nums(group[2])
	for i=1,items,1 do
		local id = group[2][i][1]
		local count = group[2][i][2]
		player:AddItemByIdWithNotify(id,count,ItemAddLogType_AddType_Acr,2628,1)
	end
	
	return ResultCode_ResultOK
end


function x002628_xOnLevelUpTo(player,params)

	local dc_container = player:DataCellContainer()
	if dc_container == nil then
		return ResultCode_Invalid_Request
	end	

	if x002628_IsAcrActive(player) ~= ResultCode_ResultOK then
		return ResultCode_InternalResult
	end

	local level = params["value"]	
	local condition ={}
	
	local flag_data=dc_container:get_data_32(CellLogicType_AcRewardCell, g_acr_datacell_index.xslb_flag)
	local reward_flag =dc_container:get_data_32(CellLogicType_AcRewardCell, g_acr_datacell_index.xslb_reward_flag)
			
	for k,v in pairs(config.items) do
		condition = v[1]
		if condition[2] ==1 and level >=condition[3] and level <= condition[4] then			
		
			local time_index = acr_dynamic_config[config.script_id][k]	
			local time=dc_container:get_data_32(CellLogicType_AcRewardCell, time_index)
			local offset =k -1			
			if time <=0 and dc_container:check_bit_data_32(flag_data,offset) ==false and  dc_container:check_bit_data_32(reward_flag,offset) == false then
				dc_container:set_data_32(CellLogicType_AcRewardCell, time_index, condition[6],true)		
				--开启礼包
				local new_data = dc_container:set_bit_data_32(flag_data,offset)					
				dc_container:set_data_32(CellLogicType_AcRewardCell, g_acr_datacell_index.xslb_flag,new_data, true)
			end
		end
	end
	
	return ResultCode_ResultOK
end

function x002628_xAddHero(player,params)

	local dc_container = player:DataCellContainer()
	if dc_container == nil then
		return ResultCode_Invalid_Request
	end	

	if x002628_IsAcrActive(player) ~= ResultCode_ResultOK then
		return ResultCode_InternalResult
	end
	
	local condition ={}
	local flag_data=dc_container:get_data_32(CellLogicType_AcRewardCell, g_acr_datacell_index.xslb_flag)
	local reward_flag =dc_container:get_data_32(CellLogicType_AcRewardCell, g_acr_datacell_index.xslb_reward_flag)
	
	for k,v in pairs(config.items) do
		condition=v[1]
		if condition[2] ==2 then	
			local time_index = acr_dynamic_config[config.script_id][k]	
			local time=dc_container:get_data_32(CellLogicType_AcRewardCell, time_index)
			local offset =k -1
			
			if time <=0 and dc_container:check_bit_data_32(flag_data,offset) == false and  dc_container:check_bit_data_32(reward_flag,offset) == false then
				dc_container:set_data_32(CellLogicType_AcRewardCell, time_index, condition[6],true)		
				--开启礼包
				local new_data = dc_container:set_bit_data_32(flag_data,offset)					
				dc_container:set_data_32(CellLogicType_AcRewardCell, g_acr_datacell_index.xslb_flag,new_data, true)
			end
		end
	end
	
	return ResultCode_ResultOK
end



function x002628_xOnBigTick(player, params)
	if config.items == nil then
		return
	end
	if x002628_IsAcrActive(player) ~= ResultCode_ResultOK then
		return ResultCode_InternalResult
	end

	local dc_container =player:DataCellContainer()
	if dc_container == nil then
		return ResultCode_InternalResult
	end

	local flag_data=dc_container:get_data_32(CellLogicType_AcRewardCell, g_acr_datacell_index.xslb_flag)	
	local reward_flag=dc_container:get_data_32(CellLogicType_AcRewardCell, g_acr_datacell_index.xslb_reward_flag)		
	for k,v in pairs(config.items) do
		local offset = k-1
		if dc_container:check_bit_data_32(flag_data,offset)	then	
			local time_index = acr_dynamic_config[config.script_id][k]	
			local time=dc_container:get_data_32(CellLogicType_AcRewardCell, time_index)
						
			if time >=0 then
				dc_container:add_data_32(CellLogicType_AcRewardCell, time_index,-10,true)
				if time -10 < 0 then	
					--结束时判定自动领奖
					if dc_container:check_bit_data_32(reward_flag,offset) and  dc_container:check_bit_data_32(reward_flag,offset+16) ==  false then						
						--标记
						local new_flag_data = dc_container:set_bit_data_32(reward_flag,offset+16)
						dc_container:set_data_32(CellLogicType_AcRewardCell, g_acr_datacell_index.xslb_reward_flag,new_flag_data,true)		
						
						--给道具
						local items = table.nums(v[2])
						for i=1,items,1 do
							local id = v[2][i][1]
							local count = v[2][i][2]
							player:AddItemByIdWithNotify(id,count,ItemAddLogType_AddType_Acr,2628,1)
						end						
					end
					
					--关闭礼包,通过设置时间无穷大来表示今天已经刷过了	
					dc_container:set_data_32(CellLogicType_AcRewardCell, time_index,999999,true)						
					local new_data = dc_container:reset_bit_data_32(flag_data,offset)				
					dc_container:set_data_32(CellLogicType_AcRewardCell, g_acr_datacell_index.xslb_flag,new_data, true)						
				end	
			end
		end
	end	
end

function x002628_OnResetData(player,params)

	if acr_dynamic_config[config.script_id] == nil then
		return ResultCode_InternalResult
	end

	local dc_container =player:DataCellContainer()
	if dc_container == nil then
		return ResultCode_InternalResult
	end
	dc_container:set_data_32(CellLogicType_AcRewardCell, g_acr_datacell_index.xslb_flag,0, true)	
	dc_container:set_data_32(CellLogicType_AcRewardCell, g_acr_datacell_index.xslb_reward_flag,0,true)		

	
	for k,v in pairs(config.items) do
		local time_index = acr_dynamic_config[config.script_id][k]	
		dc_container:set_data_32(CellLogicType_AcRewardCell, time_index,0,true)
	end

	return ResultCode_ResultOK
end
--------------------------------------------------------------for client-------------------------------
return config
