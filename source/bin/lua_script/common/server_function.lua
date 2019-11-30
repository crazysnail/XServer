
function x000005_OnDayTriger( day )
	--刷新延迟更新的活动数据
	LuaOperativeConfigManager():FlushCacheConfig()
end

function x000005_OnHourTriger( hour )
	--两小时1轮
	local num1,num2=math.modf(hour/2)--返回整数和小数部分
	if num2 == 0 then
		x002638_G_CheckBegin({})
	end
end

function x000005_OnMinTriger( mini )
end

function x000005_OnUpdateAcrConfig(server,params)
	local id = tonumber(params["script_id"])
	--print("shit!!!!!!!!!!!!"..id)
	if id == -1 then
		local jsons =LuaOperativeConfigManager():GetConfigs()
		for k,config in pairs( jsons ) do
			--print("ddddddddddddddddddd"..k)
			if config ~= nil and  acr_static_config[config:script_id()] ~= nil then
				acr_static_config.decode(config:script_id(),config:content())
				server:GetLuaCallProxy():OnLuaFunCall_n_Server(server,config:script_id(),"OnUpdateAcrConfig",{})
			end
		end
	else
		local config = LuaOperativeConfigManager():FindConfig(id)
		if config ~= nil and acr_static_config[config:script_id()] ~= nil then
			acr_static_config.decode(config:script_id(),config:content())
			server:GetLuaCallProxy():OnLuaFunCall_n_Server(server,config:script_id(),"OnUpdateAcrConfig",{})
		end
	end
	
	--检查全局活动
	x002635_G_OnResetData({})
	x002636_G_OnResetData({})
	x002638_G_OnResetData({})
	
	--全服广播更新
	server:BroadcastG2SCommonMessageToScene("G2SUpdateAcrConfig", { id }, {}, {})
	server:BroadcastG2SCommonMessageToPlayer("G2SRefreshAcrConfig", { id }, {}, {})

	return ResultCode_ResultOK
end

function x000005_TowerRankReward( rank )
	if rank == 1 then
		return {20236024,20139006,20236001,20139005}
	elseif rank >=2 and rank <=3 then
		return {20139006,20236001,20139005}
	elseif rank >=4 and rank <=10 then
		return {20236001,20139005}
	elseif rank <=50 then
		return {20139005}
	else
		return {}
	end	
end