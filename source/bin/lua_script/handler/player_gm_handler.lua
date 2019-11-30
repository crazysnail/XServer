
local gm_handler={}
gm_handler["!test"]= function(player,params)
	--return player:OnLuaFunCall_1(504,"OnTargetGroupReward",9)
	--local pinfo = player:GetScenePlayerInfo()
	--print("now_power"..pinfo:battle_score())
	--Lua_send_run_massage("treasure_map_notify_06|")
	
	--local dbplayer = player:GetDBPlayerInfo()
	--if dbplayer == nil then
	--	return ResultCode_Activity_NotStart,-1
	--end
	--	
	--local cdate = dbplayer:create_time()
	--local days = LuaMtTimerManager():DiffDayToNow(cdate)
	--
	--local oldday = LuaMtTimerManager():GetDayFromUT(cdate)
	--print("days----"..days.."-----oldday------"..oldday)

	--local ac_config = LuaActivityManager():FindActivity( ActivityType.BattleCarbon2 )
	--if ac_config == nil then
	--	return ResultCode_InternalResult
	--end
	--ac_name = ac_config:name()
	--
	--dofile("../../lua_script/json_lua/json.lua")
	
	--Lua_send_run_massage("ac_notify_025|"..ac_name) 
	--local tab = { x=123, y={ a = "hello", b = 123456} }
	--local jsonstring = json.encode(tab)
	--
	--print("json"..jsonstring)
	----Lua_send_run_massage(jsonnode)
	----
	--local jsonstring = "{\"id\": 220000,\"name\": \"诺兹多姆-降低物理伤害\",\"passive\": true,\"before_be_damage_stub\": \"xxxxx\"}"
	--jsonstring = json.decode(jsonstring)
	--x000010_dump_json(jsonstring)
	--
	--local file = io.open("../../lua_script/json.lua/samples.json", "r")
	--io.input(file)
	--jsonstring = io.read("*a")
	--io.close(file)
	----print(jsonstring)
	------jsonstring = string.gsub(jsonstring,'\n','')
	------print(jsonstring)
	--jsonstring = json.decode(jsonstring)
    ----
	----x000010_dump_json(jsonstring)
	--
	--for k, v in pairs(player:Actors())	do	
	--	print("ddddddddddddd"..k)
	--end
	
	return ResultCode_ResultOK
end

gm_handler["!add_mail"] = function(player, params)
	lua_static_message_manager():create_message(player:Guid(),99,"test", "test_content", {[1] = 1,[2] = 2})
	return ResultCode_ResultOK
end

gm_handler["!ac_times"]=function(player,params)	
	player:OnLuaFunCall_1(501,"AddActivityTimes",tonumber(params[2]))
	return ResultCode_ResultOK
end

gm_handler["!main_process"]= function(player,params)
	local cell_container= player:DataCellContainer()	
	local main1=cell_container:get_data_32(CellLogicType_MissionCell,MissionCellIndex.MainLast)
	local main2=cell_container:get_data_32(CellLogicType_MissionCell,MissionCellIndex.Part1Last)
	local main3=cell_container:get_data_32(CellLogicType_MissionCell,MissionCellIndex.Part2Last)
	local main4=cell_container:get_data_32(CellLogicType_MissionCell,MissionCellIndex.Part3Last)
	
	print("main1"..main1.." main2"..main2.." main3"..main3.." main4"..main4)
	return ResultCode_ResultOK
end

gm_handler["!robot_team"] = function(player, params)
	if player:GetTeamID() == 0 then
		player:CreateRobotTeam(tonumber(params[2]))
		player:SendClientNotify("尝试创建", -1, -1);
	else
		player:SendClientNotify("已经有队伍了",-1,-1);
	end
	return ResultCode_ResultOK
end

gm_handler["!add_robot"] = function(player, params)
	if player:GetTeamID() == 0 then
		player:SendClientNotify("没有队伍", -1, -1);
	else
		player:AddRobotPlayerToTeam()
		player:SendClientNotify("尝试添加",-1,-1);
	end
	return ResultCode_ResultOK
end

gm_handler["!break_team"]  = function(player, params)
	if player:GetTeamID() == 0 then
		player:SendClientNotify("当前没有队伍", -1, -1);
	else
		player:DismissTeam()
		player:SendClientNotify("尝试解散",-1,-1);
	end
	return ResultCode_ResultOK
end

gm_handler["!reset_miss"] = function(player, params)
	x000100_ResetMissReward(player, params)
	return ResultCode_ResultOK
end
				
gm_handler["!test_drop"]= function(player,params)
	local item_drop = LuaMtItemPackageManager():GenDropFromGroup(307)
	if item_drop ~= nil then
		player:AddItemByIdWithNotify(item_drop:item_id(),1,ItemAddLogType_AddType_Package,307,1)
	end
	return ResultCode_ResultOK
end
				
gm_handler["!test_random"]= function(player,params)
	local file2=io.output("random.txt") 
	for k=0,10000,1 do
		for i=0,10,1 do
			io.write(LuaServer():RandomNum(0,10000 - 1))
			io.write("\n")
		end
		io.write("\n")
	end
	io.flush()
	io.close()
	return ResultCode_ResultOK
end

gm_handler["!sell_all"]= function(player,params)
	local bag = player:FindContainer(ContainerType_Layer_ItemBag)
	local items = bag:Items()
	
	for k,v in pairs(items) do
		player:SellItem(v:DbInfo():guid())
	end
	return ResultCode_ResultOK
end

gm_handler["!discon"]= function(player,params)

	player:DisConnected()
	return ResultCode_ResultOK
end


gm_handler["!reset_group"]= function(player,params)

	local index = tonumber(params[2])
	
	if index<0 or index >=16 then
		return ResultCode_Invalid_Request
	end
	
	local cell_container = player:DataCellContainer()
	local flag = cell_container:get_data_32(CellLogicType_NumberCell,NumberCellIndex_TargetRewardFlag)
	
	--已经领奖
	local new_flag = cell_container:reset_bit_data_32(flag,index+16)	
	local new_new_flag = cell_container:set_bit_data_32(new_flag,index)
	
	cell_container:set_data_32(CellLogicType_NumberCell,NumberCellIndex_TargetRewardFlag,new_new_flag,true)
	
	return ResultCode_ResultOK
	
end


gm_handler["!reset_cell"]= function(player,params)

	local type  = tonumber(params[2])
	local cell_container = player:DataCellContainer()
	local i = 0
	if type == CellLogicType_TargetData then		
		for i=TargetDataIndex_TargetDataStart,TargetDataIndex_TargetDataEnd-1,1 do
			cell_container:set_data_32(type, i, 0,true)
		end			
	elseif type == CellLogicType_MissionCell then
		for i=MissionCellIndex_MissionCellStart,MissionCellIndex_MissionCellEnd-1,1 do
			cell_container:set_data_32(type, i, 0,true)
		end
	elseif type == CellLogicType_AcRewardCell then
		for i=AcRewardIndex_RewardDataStart,AcRewardIndex_RewardDataEnd-1,1 do
			cell_container:set_data_32(type, i, 0,true)
		end
	elseif type == CellLogicType_ActivityCell then
		for i=ActivityDataIndex_ActivityDataStart,ActivityDataIndex_ActivityDataEnd-1,1 do
			cell_container:set_data_32(type, i, 0,true)
		end
	elseif type == CellLogicType_GuideCell then
		for i=GuideCellIndex_GuideGroupStart,GuideCellIndex_GuideGroupEnd-1,1 do
			cell_container:set_data_32(type, i, 0,true)
		end		
	elseif type == CellLogicType_ActionData then
		for i=ActionDataIndex_ActionDataStart,ActionDataIndex_ActionDataEnd-1,1 do
			cell_container:set_data_32(type, i, 0,true)
		end		
	elseif type == CellLogicType_ResetTime then
		for i=ResetTimeIndex_ResetTimeStart,ResetTimeIndex_ResetTimeEnd-1,1 do
			cell_container:set_data_32(type, i, 0,true)
		end		
	elseif type == CellLogicType_BitFlagCell then
		for i=BitFlagCellIndex_FirstCrystalDrawFlag,BitFlagCellIndex_BitFlagCellIndex_Max,1 do
			cell_container:reset_bit_data(i,true)
		end		
	end	
	return ResultCode_ResultOK
end

gm_handler["!utime"]= function(player,params)
	local utime = params[2]
	local y = LuaMtTimerManager():GetYearFromUT(utime)
	local m = LuaMtTimerManager():GetMonthFromUT(utime)
	local d = LuaMtTimerManager():GetDayFromUT(utime)
	print("utime:"..y.."-"..m.."-"..d)
	return ResultCode_ResultOK
end

gm_handler["!set_main_actor"] = function(player, params)
	local main_actor = player:MainActor()
	local actors  = player:Actors()
	print("set_main_actor")
	local target_actor = nil
	for i=1,#actors do
		if actors[i]:ActorDBInfo():actor_config_id() == tonumber(params[2]) then
			target_actor = actors[i]
			break
		end
	end
	if target_actor ~= nil then
		local hook_form = player:GetActorFormation(ActorFigthFormation_AFF_NORMAL_HOOK)
		for i=0,hook_form:actor_guids_size() - 1 do
				hook_form:set_actor_guids(i, 0)
		end
		hook_form:set_actor_guids(0, target_actor:Guid())
		main_actor:SetActorType(ActorType_Hero)
		target_actor:SetActorType(ActorType_Main)
		player:SendClientNotify("切换成功", -1, -1)
		return ResultCode_ResultOK
	else
		player:SendClientNotify("切换失败", -1, -1)
		return ResultCode_UnknownError
	end
end

gm_handler["!test_impact"]= function(player,params)
	local proxy = player:GetSimpleImpactSet()
	proxy:DecSimpleImpact(tonumber(params[2]))
	proxy:AddSimpleImpact(tonumber(params[2]),19999999,0,0,false)	
	return ResultCode_ResultOK
end

gm_handler["!one_p"]= function(player,params)
	local data = player:GetParams32("one_p")
	if data <= 0 then
		player:SetParams32("one_p",1)
	else
		player:SetParams32("one_p",0)
	end
	return ResultCode_ResultOK
end


gm_handler["!test_bigbag"]= function(player,params)
	local econtainer = player:FindContainer(ContainerType_Layer_EquipBag);
	econtainer:ExtendSize(0,100)
	for i=1,112,1 do
		player:AddItemByIdWithNotify(i,1,ItemAddLogType_AddType_Gm,0,1)
	end
	return ResultCode_ResultOK
end

gm_handler["!mount"]= function(player,params)

	local id = tonumber(params[2])
	local mount = LuaMtMonsterManager():FindMount(id)
	if mount == nil then
		return ResultCode_InternalResult
	end
	
	local data = player:GetScenePlayerInfo()
	data:set_mount_id(id)
	
	--速度buffer	
	local proxy = player:GetSimpleImpactSet()
	proxy:AddSimpleImpact(mount:impact_id(),99999999,mount:speed(),0,false)	
	--player:ResetSpeed()	
	player:BroadCastCommonReply("RideMount",{id},{player:Guid()},{})

	x000504_xRideMount(player,{["value"]=id})
		
	return ResultCode_ResultOK
end

gm_handler["!nogain"]= function(player,params)

	local data = player:GetParams32("nogain")
	if data <= 0 then
		player:SetParams32("nogain",1)
	else
		player:SetParams32("nogain",0)
	end
		
	return ResultCode_ResultOK
end

gm_handler["!resolve"]= function(player,params)
	x000502_OnActionResolve(player,{["action_type"]=params[2],["value"]=-1})
	return ResultCode_ResultOK
end

gm_handler["!refresh_market"]= function(player,params)
	
	local type = tonumber(params[2])	
	if type ==-1 then
		local markets = player:GetAllMarkets()
		for k,v in pairs( markets ) do
			v:Refresh(true)
		end
	else
		local market = player:FindMarket(type)
		if market ~= nil then
			market:Refresh(true)
		end
	end
		
	return ResultCode_ResultOK
end

gm_handler["!speedup"]= function(player,params)
		
	local enable = tonumber(params[2])	
	local speed = tonumber(params[3])	
	
	--速度buffer
	if enable == 1 then	
		local proxy = player:GetSimpleImpactSet()
		if proxy:OnCheckImpact(NewSpeedUpImpact) then
			return ResultCode_ResultOK
		end
		proxy:AddSimpleImpact(NewSpeedUpImpact,99999999,speed,0,false)
	else
		local proxy = player:GetSimpleImpactSet()	
		proxy:DecSimpleImpact(NewSpeedUpImpact)
	end
	return ResultCode_ResultOK
end

gm_handler["!5t"]= function(player,params)
	local count=tonumber(params[2])
	for k=1,#g_Treasure5,1 do
		player:AddItemByIdWithNotify(g_Treasure5[k],count,ItemAddLogType_AddType_Gm,0,1)
	end
	return player:OnLuaFunCall_n(100,"OnExchangeT5",{["value"]=-1})
end

gm_handler["!refixform"]= function(player,params)

	local form_type = tonumber(params[2])
	if form_type< ActorFigthFormation_INVALID_FORMATION or form_type>ActorFigthFormation_AFF_PVE_GUILD_COPY then
		return ResultCode_LuaScriptException
	end
	local formation = player:GetActorFormation(form_type)
	if formation == nil then
		return ResultCode_LuaScriptException
	end
	local main_actor = player:MainActor()
	if main_actor == nil then
		return ResultCode_LuaScriptException
	end
	
	if formation:actor_guids_size()> 0 then
		formation:set_actor_guids(0,main_actor:Guid())
	end
	
	player:SendMessage(formation)		
		
	return ResultCode_ResultOK
end

gm_handler["!resolve"]= function(player,params)
	x000502_OnActionResolve(player,{});
	return ResultCode_ResultOK
end

gm_handler["!skyfall"]= function(player,params)
	local sceneId = player:GetSceneId()
	local config = nil
	for sceneid, group in pairs(g_skyfall_rule)	do
		if sceneId == group[1] then
			config = group
			break
		end
	end

	if config == nil then
		return ResultCode_ResultOK
	end

	----服务器等级判定
	--local server_level = LuaServer():ServerLevel()
	--if server_level<config[5] then
	--	return ResultCode_ResultOK
	--end


	local npc_node = LuaMtMonsterManager():GenRandomNpc(config[3],1);
	local pos_node_list = LuaMtMonsterManager():GenRandomPos(config[2], config[4]);

	--创建动态npc
	for k, node in pairs(pos_node_list)	do
		player:AddSceneNpc(node:pos_id(),npc_node[1]:index(),20);
	end
	
	--系统公告	
	local scene = player:Scene():Config()
	Lua_send_system_message_toworld("skyfall_comming|"..scene:scene_name()); --天灾怪物突然涌现在各个领土内，勇士们快快前去击杀
	
	
	return ResultCode_ResultOK
end



gm_handler["!burning"]= function(player,params)
	local sceneId = player:GetSceneId()
	local config = nil
	for sceneid, group in pairs(g_burnning_union_rule) do
		if sceneId == group[1] then
			config = group
			break
		end
	end

	if config == nil then
		return ResultCode_ResultOK
	end

	local npc_node = LuaMtMonsterManager():GenRandomNpc(config[3],1);
	local pos_node_list = LuaMtMonsterManager():GenRandomPos(config[2], config[4]);

	--创建动态npc
	for k, node in pairs(pos_node_list)	do
		player:AddSceneNpc(node:pos_id(),npc_node[1]:index(),200);
	end
	
	--系统公告	
	local scene = player:Scene():Config()
	Lua_send_system_message_toworld("burning_comming|"..scene:scene_name()); --燃烧军团怪物突然涌现在各个领土内，勇士们快快前去击杀	
	return ResultCode_ResultOK
end

----------------------------------------------------------------------------------------------------------

all_hero={
{5  ,10001},
{15 ,10002},
{25 ,10003},
{35 ,10004},
{45 ,10005},
{55 ,10006},
{65 ,10007},
{75 ,10008},
{85 ,10009},
{95 ,10010},
{105,10012},
{115,10013},
{125,10014},
{135,10015},
{145,10016},
{155,10017},
{165,10018},
{175,10019},
{185,10020},
{195,10023},
{205,10026},
{215,10027},
{225,10028},
{235,10029},
{245,10030},
{255,10031},
{265,10032},
{275,10033},
{285,10034},
{295,10035},
}


main_hero={
{ Professions_Warrior	,  304  },
{ Professions_Shaman	,  314  },
{ Professions_Hunter	,  324  },
{ Professions_Mage		,  334  },
{ Professions_Druid		,  344  },
{ Professions_Warlock	,  354  },
{ Professions_Priest	,  364  },
{ Professions_Paladin	,  374  },
{ Professions_Rogue		,  384  },
}

function Sleep(n)
   local t0 = os.clock()
   while os.clock() - t0 <= n do end
end

gm_handler["!onekey_all"]= function(player,params)

	local main = player:MainActor()
	for k,v in pairs(main_hero) do
		if v[1]== main:Professions() then
			params[2]=v[2]
			gm_handler["!onekey"](player,params)
			break
		end
	end
	Sleep(1)	
	
	for k,v in pairs(all_hero) do
		print("onekey_all"..v[1])
		params[2]=v[1]
		gm_handler["!onekey"](player,params)
		Sleep(1)		
	end
	
	return ResultCode_ResultOK
end



gm_handler["!onekey"]= function(player,params)
	local config_id = tonumber(params[2])
	local config = LuaMtActorConfig():GetOneKeyConfig(config_id)
	if config == nil then
		return ResultCode_InvalidConfigData
	end
	
	local actor = player:FindActorByCfgId(config:hero_id())
	if actor == nil then
		player:AddHero(config:hero_id(),ActorType_Hero,"GM",false,ActorStar_ONE,ActorColor_WHILE,1)
		actor = player:FindActorByCfgId(config:hero_id())
		if actor == nil then
			return ResultCode_InvalidConfigData
		end
	end
	actor:ChangeStar(config:star())
	for i=ActorColor_WHILE,config:color(),1 do
		actor:ChangeColor(i)
	end
	actor:LevelUpTo(config:level(),true, true)
	actor:RefreshData2Client()
	
	--技能等级
	local skills = actor:DbSkills()
	for k,v in pairs( skills ) do 
		local dbinfo = v:DbInfo()
		if LuaMtActorConfig():IsStarSkill(config:hero_id(), dbinfo:skill_id()) == false then
			dbinfo:set_skill_level(config:skill_level())
		end
	end
	actor:RefreshSkillData2Client()
	
	--装备&宝石
	for i=0,config:equips_size()-1,1 do		
		local v = config:equips(i)
		local econfig = LuaItemManager():GetEquipmentConfig(v:id())
		local equip = nil
		if i==0 then
			equip = actor:AddEquipByConfigId(v:id(),EquipmentSlot_EQUIPMENT_SLOT_MAIN_HAND)
		elseif i== 1 then
			equip =actor:AddEquipByConfigId(v:id(),EquipmentSlot_EQUIPMENT_SLOT_ASSI_HAND)
		elseif i== 10 then
			equip = actor:AddEquipByConfigId(v:id(),EquipmentSlot_EQUIPMENT_SLOT_RING_ONE)
		elseif i== 11 then
			equip = actor:AddEquipByConfigId(v:id(),EquipmentSlot_EQUIPMENT_SLOT_RING_TWO)
		else
			if econfig ~= nil then
				equip =actor:AddEquipByConfigId(v:id(),LuaMtActorConfig():EquipmentType2Slot(econfig:type()))
			end
		end
		
		if equip ~= nil then
			--print("xxxxx"..tostring(v:id()).."dddddd="..i)
			equip:InsetGem(v:gem0(),0)
			equip:InsetGem(v:gem1(),1)
			equip:InsetGem(v:gem2(),2)
			equip:RefreshData2Client()		
		end
	end
	
	actor:OnBattleInfoChanged(false)
	
	return ResultCode_ResultOK
end

---------------------------------------------------------------------------------

gm_handler["!newday"]= function(player,params)
	local now_day = LuaMtTimerManager():GetDay()
	player:OnDayTriger(now_day,true)
	return ResultCode_ResultOK
end

gm_handler["!newweek"]= function(player,params)
	local now_week = LuaMtTimerManager():GetWeekNumber()
	player:OnWeekTriger(now_week)
	return ResultCode_ResultOK
end

gm_handler["!newmonth"]= function(player,params)
	local now_month = LuaMtTimerManager():GetMonth()
	player:OnMonthTriger(now_month)
	return ResultCode_ResultOK
end

gm_handler["!server_newday"]= function(player,params)
	local now_day = LuaMtTimerManager():GetDay()
	LuaServer():SendS2GCommonMessage("S2GDayTriger",{now_day},{},{})
	return ResultCode_ResultOK
end

gm_handler["!server_newweek"]= function(player,params)
	local now_week = LuaMtTimerManager():GetWeekNumber()
	LuaServer():SendS2GCommonMessage("S2GWeekTriger",{now_week},{},{})
	return ResultCode_ResultOK
end
gm_handler["!server_newmonth"]= function(player,params)
	local now_month = LuaMtTimerManager():GetMonth()
	LuaServer():SendS2GCommonMessage("S2GMonthTriger",{now_month},{},{})
	return ResultCode_ResultOK
end

gm_handler["!server_level"]= function(player,params)
	LuaServer():SendS2GCommonMessage("S2GServerLevel",{},{},{})
	return ResultCode_ResultOK
end

gm_handler["!reset_quikbuy"]= function(player,params)
	
	local group = acr_static_config[2638].items[1]
	local count = group[1][2]
	
	local server_info = LuaServer():GetServerInfo()	
	server_info:set_acr_data(g_acr_server_index.quik_buy_index,1)	
	server_info:set_acr_data(g_acr_server_index.quik_buy_count,count)	
	
	LuaServer():BroadcastG2SCommonMessageToPlayer("G2SQuikBuyAcrBegin",{2638,count,1},{},{})
	
	return ResultCode_ResultOK	
end

gm_handler["!server_operate"]= function(player,params)
	local id = tonumber(params[2])
	LuaServer():SendS2GCommonMessage("S2GUpdateOperate",{id},{},{})
	return ResultCode_ResultOK
end

gm_handler["!flush_delay"]= function(player,params)
	local id = tonumber(params[2])
	LuaServer():SendS2GCommonMessage("S2GFlushDelay",{},{},{})
	return ResultCode_ResultOK
end

	
gm_handler["!operate_one"]= function(player,params)
	local id = tonumber(params[2])
	local config = LuaOperativeConfigManager():FindConfig(id)
	if config == nil then
		return ResultCode_InternalResult
	end
	if acr_static_config[id] ~= nil then
		acr_static_config.decode(id,config:content())
		acr_static_config.dumpinfo(id)
		player:Scene():GetLuaCallProxy():OnLuaFunCall_n_Scene(player:Scene(),id,"OnUpdateAcrConfig",{})
		player:Refresh2ClientOperateConfig(id)		
	end
	return ResultCode_ResultOK
end

gm_handler["!operate_dump"]= function(player,params)
	local id = tonumber(params[2])
	acr_static_config.dumpinfo(id)
	return ResultCode_ResultOK
end

gm_handler["!acr_luaconfig"]= function(player,params)
	LuaServer():SendS2GCommonMessage("S2GLuaFilesUpdate",{},{},{})
	return ResultCode_ResultOK
end

gm_handler["!acr_refresh"]= function(player,params)
	LuaServer():SendS2GCommonMessage("S2GAcrRefresh",{},{},{})	
	return ResultCode_ResultOK
end

gm_handler["!arrange"]= function(player,params)
	local mproxy = player:GetMissionProxy()
	mproxy:OnTeamArrangeReward(67302)
	return ResultCode_ResultOK
end


gm_handler["!table"]= function(player,params)
	for k,v in pairs(gm_handler) do
		print(k)
		print(v)
	end
	return ResultCode_ResultOK
end

gm_handler["!suit"]= function(player,params)
	--装备
	local ids = LuaItemManager():GetSuitEquips(params[2])	
	for k,v in pairs(ids) do	
		player:AddItemByIdWithNotify(v,1,ItemAddLogType_AddType_Gm,0,1)
	end	
	return ResultCode_ResultOK
end

--gm_handler["!package"]= function(player,params)
--	--装备
--	local proxy = player:GetArrangeProxy()
--	proxy:OnClearContainer()
--	
--	for i=0,tonumber(params[3]),1 do
--		local pacage = LuaMtItemPackageManager():GetPackage(params[2])	
--		pacage:AddToPlayerArrange(player)
--	end
--	return ResultCode_ResultOK
--end

gm_handler["!package"]= function(player,params)
	--装备
	--local proxy = player:GetArrangeProxy()
	--proxy:OnClearContainer()
	
	for i=0,tonumber(params[3]),1 do
		local pacage = LuaMtItemPackageManager():GetPackage(params[2])	
		pacage:AddToPlayer(player)
	end
	return ResultCode_ResultOK
end

gm_handler["!reload_skill"] = function(player, params)
	--重新加载技能
	LuaMtSkillManager():InitLuaContext()
	return ResultCode_ResultOK
end

gm_handler["!drop_test"]= function(player,params)
	--print(tonumber(params[2]).." "..tonumber(params[3]))
	local all_items ={}
	for i=1,params[3],1 do
		local package = LuaMtItemPackageManager():GetPackage(params[2])
		local items = package:GenItems()
		for k,v in pairs(items) do 
			--print(k.." "..v)
			if k == 29910008 or k==29910009 or k== 29910001 then
			else
				if all_items[k] == nil then
					all_items[k] = v
				else
					all_items[k] = all_items[k]+v
				end
			end
		end
	end	
	
	player:SendClientNotify("drops id="..params[2],0,-1)
	for k,v in pairs(all_items) do 
		player:SendClientNotify(k.."===="..v.."rate="..(v/params[3]*100).."%",0,-1)
	end
	return ResultCode_ResultOK
end

gm_handler["!gbuild_level"] = function(player, params)
	local guild = LuaMtGuildManager():GetGuildByid(player:GetGuildID());
	if guild ~= nil then
		guild:SettBuildLevel(params[2],params[3],0)
	end
	return ResultCode_ResultOK
end

gm_handler["!guildbattle_easy"] = function(player, params)
	local ud_config = LuaMtConfigDataManager():FindConfigValue("guild_battle_enter");
	ud_config:set_value1(0);
	ud_config:set_value2(0);
	ud_config:set_value3(0);
	ud_config = LuaMtConfigDataManager():FindConfigValue("guild_battle_playerqualify");
	ud_config:set_value1(0);
	return ResultCode_ResultOK
end

gm_handler["!guildweek"] = function(player, params)
	LuaMtGuildManager():OnWeekTriger(0);
	return ResultCode_ResultOK
end

gm_handler["!guildfinish"] = function(player, params)
	x000007_GuildBattleFinish();
	return ResultCode_ResultOK
end

gm_handler["!invest"] = function(player, params)
	LuaServer():SendS2GCommonMessage("S2GInvest",{1},{},{})
	return ResultCode_ResultOK
end

gm_handler["!allpower"] = function(player, params)
	LuaServer():SendS2GCommonMessage("S2GAllPower",{tonumber(params[2]),tonumber(params[3])},{},{})
	return ResultCode_ResultOK
end

gm_handler["!alllevel"] = function(player, params)
	LuaServer():SendS2GCommonMessage("S2GAllLevel",{tonumber(params[2]),tonumber(params[3])},{},{})
	return ResultCode_ResultOK
end


gm_handler["!open_date"] = function(player, params)
	LuaServer():SendS2GCommonMessage("S2GOpenDate",{params[2]},{},{})
	return ResultCode_ResultOK
end

gm_handler["!past_day"] = function(player, params)
	local ds = LuaActivityManager():GetDateStruct()
	LuaServer():LuaLog("ds.past_day"..ds.past_day,g_log_level.LL_INFO)
	return ResultCode_ResultOK
end


gm_handler["!enable_msgc"] = function(player, params)
	player:EnableMsgCounter(tonumber(params[2])~=0)
	return ResultCode_ResultOK
end

gm_handler["!dump_msgc"] = function(player, params)
	player:DumpMsgCounterInfo(tonumber(params[2])==1)
	return ResultCode_ResultOK
end

--任务相关
gm_handler["!accept_mission"] = function(player, params)
	local proxy = player:GetMissionProxy()
	if proxy ~= nil then
		proxy:OnAcceptMission(params[2])
	end
	return ResultCode_ResultOK
end

gm_handler["!reset_mission"] = function(player, params)
	local proxy = player:GetMissionProxy()
	if proxy ~= nil then
		if tonumber(params[2])==-1 then
			--all
			for i=0,MissionType_Carbon2007Mission,1 do
				proxy:OnResetMission(i)
			end
		else
			proxy:OnResetMission(params[2])
		end
	end
	return ResultCode_ResultOK
end

gm_handler["!add_mission"] = function(player, params)
	local proxy = player:GetMissionProxy()
	if proxy ~= nil then
		proxy:OnAddMission(params[2])
	end
	return ResultCode_ResultOK
end

gm_handler["!drop_mission"] = function(player, params)
	local proxy = player:GetMissionProxy()
	if proxy ~= nil then
		proxy:OnDropMission(params[2])
	end
	return ResultCode_ResultOK
end

gm_handler["!fight_mission"] = function(player, params)
	local proxy = player:GetMissionProxy()
	if proxy ~= nil then
		proxy:OnMissionFight(params[2])
	end
	return ResultCode_ResultOK
end

gm_handler["!finish_mission"] = function(player, params)
	local proxy = player:GetMissionProxy()
	if proxy ~= nil then
		proxy:OnFinishMission(params[2])
	end
	return ResultCode_ResultOK
end
------------------------------------------------------------------------------

gm_handler["!clear_self_cp"] = function(player, params)
	local guild_guid = player:GetGuildID()
	local data = LuaMtCapturePointManager():FindGuildCapturePoint(guild_guid)
	if data ~= nil then
		data:Clear()
		data:set_guid(guild_guid)
		for i=0,4 do
			local sh = data:add_strong_holds()
			for j=0,4 do
				sh:add_heros()
			end
		end
		data:SetInitialized()
	end
	return ResultCode_ResultOK
end

gm_handler["!clear_all_cp"] = function(player, params)
	for _,chapter_id in pairs(LuaMtCapturePointManager():AllChapterId()) do
		local cp =  LuaMtCapturePointManager():FindCapturePoint(chapter_id)
		if cp ~= nil then
			x001025_OnCapturePointInit(chapter_id, cp)
		end
	end
	return ResultCode_ResultOK
end

gm_handler["!lua_gm_test"] = function(player, params)
	LuaBattleFieldManager():LeaveBattleField(player)
	local d = LevelConfig:new()
	d:set_level(1)
	player:SendMessage(d)
	d:delete()
	print("lua_gm_test")
	return ResultCode_ResultOK
end

gm_handler["!tb_reload"]= function(player,params)	
	LuaServer():SendS2GCommonMessage("S2GTbReload",{},{},{params[2]})	
	return ResultCode_ResultOK
end

gm_handler["!switch_market"]= function(player,params)
	LuaServer():SendS2GCommonMessage("S2GSwitchMarket",{params[2],params[3]},{},{})	
	return ResultCode_ResultOK
end

gm_handler["!switch_item"]= function(player,params)
	LuaServer():SendS2GCommonMessage("S2GSwitchMarketItem",{params[2],params[3],params[4]},{},{})	
	return ResultCode_ResultOK
end


gm_handler["!hero_draw"] = function(player,params)

	local ret = player:OnLuaFunCall_n(2622,"DoReward",{["index"]=params[2]})
	if ret ~= ResultCode_ResultOK then
		player:SendCommonResult(ResultOption_Default_Opt,ret)
	end
	return ResultCode_ResultOK
end

gm_handler["!time_package"] = function(player,params)

	local ret = player:OnLuaFunCall_n(2623,"DoReward",{["index"]=params[2]})
	if ret ~= ResultCode_ResultOK then
		player:SendCommonResult(ResultOption_Default_Opt,ret)
	end
	return ResultCode_ResultOK
end

gm_handler["!su_charge"]= function(player,params)
	local index = tonumber(params[2])

	local product_id = LuaMtMarketManager():FindChargeProductId("koogame_mt_1",index)
	if product_id == nil then
		return ResultCode_Invalid_Request
	end	
    
	x000100_OnChargeItem(player,"internal",product_id,"localhost","koogame_mt_1")	
    
	return ResultCode_ResultOK	
end

gm_handler["!form_test"]= function(player,params)
	local form_type = tonumber(params[2])
	local form = player:GetActorFormation(form_type)
	local scene = player:Scene()
	if form ~= nil and scene ~= nil then
		local actors = player:Actors()
		for k,v in pairs(actors) do
			if v ~= nil then
				form:set_actor_guids(k-1,v:Guid())
				if k>=5 then
					break
				end
			end
		end

		player:SendMessage(form)
		player:OnLuaFunCall_x("xOnSetActorFight",{["value"]=form:actor_guids_size()})
		player:UpdateBattleScore(false)
	end
		
	return ResultCode_ResultOK	
end

gm_handler["!set_md5"]= function(player,params)
	local md5str = "6660019501324591178177674443666001_9501_1520307642_15com.koogame.mt_6A34BF80C-F494-3D2C-9B13-6508A285052Debf7b5699c50fc00bd305d39ce1db958";
	local str = lua_md5(md5str);
	LuaServer():LuaLog("MD5, str="..str,g_log_level.LL_INFO)
	return ResultCode_ResultOK
end

gm_handler["!shm_cache"]= function(player,params)
	local player_base =36370000000000000;
	local player_count = 5;
	
	local item_base =36371000000000000;
	local item_count = 150;
	
	local hero_base =36374000000000000;
	local hero_count = 40;
	
	local gems_base =3637200000000000;
	local gems_count = hero_count*12*3
	
	local equip_base =36373000000000000;
	local equip_count = 200 + hero_count*12
	
	local skill_base =36375000000000000;
	local skill_count = 5;
	
	local market_base =36376000000000000;
	local market_count =10;
	
	for i=player_base,player_base+player_count,1 do		  
		local msg = PlayerDBInfo:new()
		msg:SetInitialized()
		msg:set_guid(i)
		for k=1,4,1 do 
			msg:add_lackeys(k)
		end		
		local basic = msg:mutable_basic_info()
		basic:SetInitialized()
		basic:set_guid(i)
		local token = msg:mutable_token_data()
		token:SetInitialized()
		local hook = msg:mutable_hook_option()
		hook:SetInitialized()
		LuaMtShmManager():Cache2Shm(msg)

		--for  k=hero_base,hero_base+hero_count,1 do
		--	local msg = ActorBasicInfo:new()
		--	msg:SetInitialized()
		--	msg:set_guid(k)
		--	LuaMtShmManager():Cache2Shm(msg)
		--	
		--	local binfo = msg:mutable_battle_info()
		--	binfo:SetInitialized()
		--	
		--	for m=skill_base,skill_base+skill_count,1 do
		--		local msg = ActorSkill:new()
		--		msg:SetInitialized()
		--		msg:set_guid(m)
		--		LuaMtShmManager():Cache2Shm(msg)
		--	end
		--
		--end
		
		--for  k=item_base,item_base+item_count,1 do
		--	local msg = NormalItem:new()
		--	msg:SetInitialized()
		--	msg:set_guid(k)
		--	LuaMtShmManager():Cache2Shm(msg)
		--end
        --
		--for  k=equip_base,equip_base+equip_count,1 do
		--	local msg = EquipItem:new()
		--	msg:SetInitialized()
		--	msg:set_guid(k)
		--	
		--	--for m=0,3,1 do
		--	--	local e = msg:add_enchants()
		--	--	e:SetInitialized()
		--	--end
		--	--
		--	--msg:add_gems(0)
		--	--msg:add_gems(0)
		--	--msg:add_gems(0)
		--	
		--	LuaMtShmManager():Cache2Shm(msg)
		--end
		--
		--for  k=gems_base,gems_base+gems_count,1 do
		--	local msg = EquipItem:new()
		--	msg:SetInitialized()
		--	msg:set_guid(k)
		--	LuaMtShmManager():Cache2Shm(msg)
		--end
		--
		--for  k=market_base,market_base+market_count,1 do
		--	local msg = Market:new()
		--	msg:SetInitialized()
		--	msg:set_guid(k)
		--	LuaMtShmManager():Cache2Shm(msg)
		--end
				
	end	
	return ResultCode_ResultOK
end

gm_handler["!shm_save"]= function(player,params)
	LuaMtShmManager():Shm2Save()
	return ResultCode_ResultOK
end


gm_handler["!shm_dump"]= function(player,params)
	LuaMtShmManager():DumpShm()
	return ResultCode_ResultOK
end

gm_handler["!shm_stat"]= function(player,params)
	LuaMtShmManager():DumpInfo()
	return ResultCode_ResultOK
end


---------------------------------------------------线程不安全的gm指令，测试用
gm_handler["!bf_drop"]= function(player,params)
	local proxy = player:GetBattleFeildProxy()
	local room = LuaBattleFieldManager():FindRoomById(proxy:raid_rtid())
	room:OnDropFlag(player,false)
	return ResultCode_ResultOK
end
gm_handler["!bf_newbuff"]= function(player,params)
	local proxy = player:GetBattleFeildProxy()
	local room = LuaBattleFieldManager():FindRoomById(proxy:raid_rtid())
	room:OnGenBuff()
	return ResultCode_ResultOK
end
gm_handler["!bf_begin"]= function(player,params)
	local proxy = player:GetBattleFeildProxy()
	local room = LuaBattleFieldManager():FindRoomById(proxy:raid_rtid())
	room:OnForceBegin()
	return ResultCode_ResultOK
end
gm_handler["!bf_end"]= function(player,params)
	local proxy = player:GetBattleFeildProxy()
	local room = LuaBattleFieldManager():FindRoomById(proxy:raid_rtid())
	room:ResultCalc()
	return ResultCode_ResultOK
end
gm_handler["!bf_onbuff"]= function(player,params)

	local raid = player:GetRaid();
	if raid ~= nil then
		raid:OnBuffPoint(player,1140200101)
	end
	
	return ResultCode_ResultOK
end

gm_handler["!server_mintick"]= function(player,params)
	LuaServer():SendS2GCommonMessage("S2GGMMinTick",{},{},{})
	return ResultCode_ResultOK
end

gm_handler["!guildbattlelist"]= function(player,params)
	LuaMtGuildManager():GenGuildBattleList();
	return ResultCode_ResultOK
end

gm_handler["!createguildbossnew"]= function(player,params)
	LuaMtGuildManager():OnCreateGuildBoss();
	return ResultCode_ResultOK
end

gm_handler["!testcall"]= function(player,params)
	player:OnLuaFunCall_c(19999,"testcallc",{1,2,3,4},{5,6,7,8},{"9","10","11"});
	return ResultCode_ResultOK
end

function x019999_testcallc(player,int32params,int64params,stringparams)
	print(int32params[1])
	print(int32params[2])
	print(int32params[3])
	print(int32params[4])
	print(int64params[1])
	print(int64params[2])
	print(int64params[3])
	print(int64params[4])
	print(stringparams[1])
	print(stringparams[2])
	print(stringparams[3])
	return ResultCode_ResultOK
end
---------------------------------------------------线程不安全的gm指令，测试用


--------------------------------------------------
function x019999_gm_handler(player,params)
	local name = params[1]
	LuaServer():LuaLog(name.."------"..player:Guid(),g_log_level.LL_INFO)
	local fun = gm_handler[name]
	if fun == nil then
		player:SendClientNotify("Commond "..name.." Error!",-1,-1)
		return ResultCode_UnknownError
	end
	local ret=gm_handler[name]( player, params )	
	if ret == ResultCode_ResultOK then
		player:SendClientNotify("Commond "..name.." OK!",-1,-1)
	else
		player:SendClientNotify("Commond "..name.." Error!",-1,-1)
	end
	return ret
end	


