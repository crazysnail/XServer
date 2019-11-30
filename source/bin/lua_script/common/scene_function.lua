
function x000004_OnPlayerEnter(scene,player)
	
end

function x000004_OnPlayerLeave(scene,player)

end

function x000004_OnOperateConfigInit(scene)
	----重置默认数据
	--for k,v in pairs(g_reward_activity_script) do
	--	scene:GetLuaCallProxy():OnLuaFunCall_n_Scene(scene,v,"OnUpdateAcrConfig",{})
	--end
	--更新数据
	local jsons =LuaOperativeConfigManager():GetConfigs()
	for k,config in pairs( jsons ) do
		if config ~= nil and  acr_static_config[config:script_id()] ~= nil then
			acr_static_config.decode(config:script_id(),config:content())
			scene:GetLuaCallProxy():OnLuaFunCall_n_Scene(scene,config:script_id(),"OnUpdateAcrConfig",{})
		end
	end
	return ResultCode_ResultOK
end



--首次进入需要做提示测场景
g_notify_scenes={
1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20
}
function x000004_OnClientEnterOk(scene,player)
	local curworldbossconfig = LuaMtWorldBossManager():GetCurWorldBossConfig();
	if curworldbossconfig ~= nil then
		if scene:SceneId() == curworldbossconfig:sceneid() then
			player:OnSendWorldBossBattleReply();
		end
	end
	
	--首次进入判定
	local index = 0
	for k,v in pairs(g_notify_scenes) do
		if scene:SceneId()== v then
			break
		end
		index = index+1
	end
	
	local cell_container = player:DataCellContainer()
	if cell_container ~= nil then
		local flag = cell_container:get_data_64(Cell64LogicType_NumberCell64, NumberCell64Index_SceneFirstFlag)
		if cell_container:check_bit_data_64(flag,index) == false then
			new_flag = cell_container:set_bit_data_64(flag,index)
			cell_container:set_data_64(Cell64LogicType_NumberCell64, NumberCell64Index_SceneFirstFlag,new_flag,true)			
			--通知
			player:SendLuaPacket("SceneFirstTimeNotify",{},{},{})
		end		
	end
	
	--同步下活动时间
	--x002623_TimeUpdate(player)
	
end

--这个先放这里吧
function x000004_OnSecondTick(scene)
	local scene_data = scene:GetSceneData();
	local last_date = scene_data:get_data(g_scene_data.last_date)
	local current_day = LuaMtTimerManager():GetDay();
	local current_hour = LuaMtTimerManager():GetHour();
	local current_minute = LuaMtTimerManager():GetMinute();
	local current_weekday = LuaMtTimerManager():GetWeekDay();
	
	local last_day = LuaMtTimerManager():GetDayFromUT(last_date);
	local last_hour = LuaMtTimerManager():GetHourFromUT(last_date);
	local last_minite = LuaMtTimerManager():GetMinuteFromUT(last_date);
	
	if current_day ~= last_day then
		x000004_OnGuildDay(scene,current_weekday);
	end

	--天灾军团
	--每15分钟一波，持续15分钟
	if scene_data:get_data(g_scene_data.skyfall_flag) == 0 then
		if current_minute == 0 or current_minute == 15 or current_minute == 30 or current_minute == 45 then
			x000004_OnSkyFallBegin(scene)
			scene_data:set_data(g_scene_data.skyfall_flag,1)
		end
	else
		--持续15分钟
		if current_minute == 14 or current_minute == 29 or current_minute == 44 or current_minute == 59 then
			--x000004_OnSkyFallEnd(scene)
			scene_data:set_data(g_scene_data.skyfall_flag,0)
		end
	end
	
	--燃烧军团
	--每天整点0分，持续1小时
	if scene_data:get_data(g_scene_data.burning_flag) == 0 then
		if current_minute == 0 then
			x000004_OnBurnningBegin(scene)
			scene_data:set_data(g_scene_data.burning_flag,1)
		end
	else
		--燃烧军团
		--持续30分钟
		if current_minute == 59 then
			--x000004_OnBurnningEnd(scene)
			scene_data:set_data(g_scene_data.burning_flag,0)
		end
	end

	
	local curTime = LuaMtTimerManager():Time2Num()
	
	--世界boss
	--每天8点
	local worldbossconfig = LuaMtWorldBossManager():GetCurWorldBossConfig();
	if worldbossconfig ~= nil and worldbossconfig:sceneid() == scene:SceneId() then
		local world_boss_type = scene:GetLuaCallProxy():OnLuaFunCall_n_Scene(scene,501, "GetWorldBossAc",{} )	
		if scene_data:get_data(g_scene_data.worldboss_flag) == 0 then
			local acTimes = LuaActivityManager():GetActivityTimes(world_boss_type)
			if curTime == 2000 then
				x000004_OnWorldBossBegin(scene,worldbossconfig)
				scene_data:set_data(g_scene_data.worldboss_flag,1)
			end
			
			--开启提示走统一的公告流程
			--if curTime == acTimes:begin_time() and scene_data:get_data(g_scene_data.worldboss_notify) == 0 then
			--	x000004_OnWorldBossNotify(scene,worldbossconfig)
			--	scene_data:set_data(g_scene_data.worldboss_notify,1)			
			--end
		else
			local acTimes = LuaActivityManager():GetActivityTimes(world_boss_type)		
			if curTime == acTimes:end_time() then
				x000004_OnWorldBossEnd(scene)
				scene_data:set_data(g_scene_data.worldboss_flag,0)
				scene_data:set_data(g_scene_data.worldboss_notify,0)				
			end
		end
	end
	-----------
	local current_date = LuaMtTimerManager():CurrentDate();
	scene_data:set_data(g_scene_data.last_date,current_date)

	--if scene:SceneId() == 1 then
	--	local insert_sql = "REPLACE INTO `player_register_log` VALUES ('36082009888697003', '9501', '1001', 'testuser1','54761d6b63e63329a9064cf4441adb71a33b5913','2018-02-24','2018-02-24 16:13:00');"
	--	print(insert_sql)
	--	LuaServer():SendLogMessage(insert_sql)
	--end
end
--天灾军团
function x000004_OnSkyFallBegin(scene)

	local config = nil
	for sceneid, group in pairs(g_skyfall_rule)	do
		if scene:SceneId() == group[1] then
			config = group
			break
		end
	end

	if config == nil then
		return;
	end

	----服务器等级判定
	--local server_level = LuaServer():ServerLevel()
	--if server_level<config[5] then
	--	return;
	--end

	local npc_node = LuaMtMonsterManager():GenRandomNpc(config[3],1);
	local pos_node_list = LuaMtMonsterManager():GenRandomPos(config[2], config[4]);

	--创建动态npc
	for k, node in pairs(pos_node_list)	do
		scene:AddNpc(node:pos_id(),npc_node[1]:index(),900);
	end
	
	local config = scene:Config()
	--系统公告				
	Lua_send_system_message_toworld("skyfall_comming|"..config:scene_name()); --天灾怪物突然涌现在各个领土内，勇士们快快前去击杀
	
end

function x000004_OnSkyFallEnd(scene)
	--print("x000004_OnSkyFallEnd")
	scene:DelNpcByType("skyfall_union");
end

--燃烧军团
function x000004_OnBurnningBegin(scene)

	----print("x000004_OnBurnningBegin")

	local config = nil
	for sceneid, group in pairs(g_burnning_union_rule)
	do
		if scene:SceneId() == group[1] then
			config = group
			break
		end
	end

	if config == nil then
		return
	end

	local npc_node = LuaMtMonsterManager():GenRandomNpc(config[3],1);
	local pos_node_list = LuaMtMonsterManager():GenRandomPos(config[2], config[4]);

	--创建动态npc
	for k, node in pairs(pos_node_list)	do
		scene:AddNpc(node:pos_id(),npc_node[1]:index(),3600);
	end
	
	local config = scene:Config()
	--系统公告				
	Lua_send_system_message_toworld("burning_comming|"..config:scene_name()); --燃烧军团怪物突然涌现在各个领土内，勇士们快快前去击杀	

end

function x000004_OnBurnningEnd(scene)

	----print("x000004_OnBurnningEnd")
	scene:DelNpcByType("burning_union")
end

function x000004_OnGuildDay(scene,weekday)
	local opendata = LuaMtConfig():open_server_date();
	local topendata = LuaMtTimerManager():Num2Date(opendata);
	--print("opendata "..topendata);
	--print("diffweek "..LuaMtTimerManager():DiffWeekToNow(topendata));
	local goweek = LuaMtTimerManager():DiffWeekToNow(topendata);
	local count,rand = math.modf(goweek/8);
	if rand == 0 then

	elseif rand >= 5 and weekday == 4 then

	end
	count,rand = math.modf(goweek/24);
	if rand == 0 then

	end
end

--世界boss
function x000004_OnWorldBossBegin(scene,config)
	local npc_node = LuaMtMonsterManager():FindDynamicNpc(config:npcid());
	local series_id =  scene:AddNpc(npc_node:pos_id(),npc_node:index(),-1);

	--世界boss活动正式开启
	LuaMtWorldBossManager():OnCreateBoss(series_id,scene:SceneId());

end

function x000004_OnWorldBossEnd(scene)
	scene:DelNpcByType("world_boss")
	LuaMtWorldBossManager():OnEndBoss(false);
	--世界boss活动正式结束
end

function x000004_OnWorldBossNotify(scene,config)
	local notify="wb_start|".."scene_name_"..tostring(math.floor(scene:SceneId())).."|monster_name_"..tostring(math.floor(LuaMtMonsterManager():GetFirstMosterByGroupId(config:gourpid()):id()));
	Lua_send_system_message_toworld(notify);
end

--世界boss
function x000004_OnGuildBossBegin(scene)

	if scene:SceneId() ~= 6 then
		return ResultCode_Wrong_Status;
	end
	--世界boss活动正式开启
	LuaMtGuildManager():OnCreateGuildBoss();
	return ResultCode_ResultOK;
end

function x000004_OnGuildBossEnd(scene)
	--scene:DelNpcByType("guild_boss")
	--世界boss活动正式结束
	LuaMtGuildManager():OnEndGuildBoss(false);
end

