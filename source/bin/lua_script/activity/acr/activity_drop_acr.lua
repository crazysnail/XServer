local config = {}

-----------------------------------------------for server---------------------------------------------
function x002633_OnUpdateAcrConfig(params)
	config = acr_static_config[2633]
	return ResultCode_ResultOK
end
function x002633_IsAcrActive(player)

	if x000011_GOperateCheckOpenToLua(config) == false then
		return ResultCode_Activity_NotStart
	end	
	return ResultCode_ResultOK
end

function x002633_OnResolve(player)
end

-----------------------------------------------
function x002633_OnRegisterFunction(proxy)
	
	if proxy ~= nil then
		proxy:RegFunc("xAcrDropCheck"			,2633)	
	end
end

function x002633_xAcrDropCheck(player,params)
	if x002633_IsAcrActive(player) ~= ResultCode_ResultOK then
		return ResultCode_ResultOK
	end
	
	local actype = params["value"]
	local dc_container = player:DataCellContainer()
	if dc_container == nil then
		return ResultCode_Invalid_Request
	end
	
	--最多9种
	local numbers = table.nums(config.items)
	--print("numbers"..numbers)
	if numbers >9 then
		return ResultCode_InvalidConfigData
	end
	
	local group = {}
	for k,v in pairs(config.items) do
		if actype == v[1][1] then
			group = v
		end
	end
	
	--print(" table.nums( group )".. table.nums( group ))
	if table.nums( group ) == 0 then
		return ResultCode_ResultOK
	end
		
	--给道具
	local items = table.nums(group[3])
	for i=1,items,1 do
		local rate = group[3][i][3]
		local seed = LuaServer():RandomNum(1,100)
		if seed <= rate then
			local id = group[3][i][1]
			local count = group[3][i][2]
			player:AddItemByIdWithNotify(id,count,ItemAddLogType_AddType_Acr,2633,1)
		end
	end
	
	return ResultCode_ResultOK
end

function x002633_OnResetData(player,params)

	return ResultCode_ResultOK
end

--------------------------------------------------------------for client-------------------------------
return config
