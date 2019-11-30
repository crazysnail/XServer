

function x000100_OnRegisterFunction(proxy)
	
	if proxy ~= nil then
		proxy:RegFunc("xOnNewDay"			,100)	
		proxy:RegFunc("xOnNewWeek"			,100)	
		proxy:RegFunc("xOnLevelUpTo"		,100)			
		proxy:RegFunc("xUpdateBattleScore"	,100)
		proxy:RegFunc("xPlayerRaidStage"	,100)	
		proxy:RegFunc("xDrawCrystal"		,100)
		proxy:RegFunc("xDrawGold"			,100)	
		proxy:RegFunc("xOnMonsterDead"		,100)
		proxy:RegFunc("xOnEnchantEquip"		,100)	
		proxy:RegFunc("xCostItem"			,100)	
		proxy:RegFunc("xOnInsetEquip"		,100)
		proxy:RegFunc("xOnUpgradeSKill"		,100)
		proxy:RegFunc("xOnUpgradeColor"		,100)	
		proxy:RegFunc("xOnConsume"			,100)
		proxy:RegFunc("xOnAddItem"			,100)	
		proxy:RegFunc("xOnAddHero"			,100)	
		proxy:RegFunc("xOnAddItemNotify"	,100)	
		proxy:RegFunc("xUpdateAcrStatus"	,100)		
		proxy:RegFunc("xOnUpdateGuildInfo"  ,100)
		proxy:RegFunc("xOnArrangeOver"  	,100)	
		proxy:RegFunc("xOnInitCell"  		,100)		
		proxy:RegFunc("xDisConnected" 		,100)		
		proxy:RegFunc("xReConnected"		,100)	
		proxy:RegFunc("xRefreshData2Clinet"	,100)		
		proxy:RegFunc("xOnMissRewardResolve",100)		
	end
end

function x000100_xDisConnected(player,params)
	--更新每日在线时间
	local dbinfo = player:GetDBPlayerInfo()
	local login_time = dbinfo:last_logout_time()
	local online_time = LuaMtTimerManager():DiffTimeToNow(login_time)	
	local dc_container =  player:DataCellContainer()
	dc_container:add_data_32(CellLogicType_ActionData, g_action_data_index.day_online_time,online_time,false)
	
	--特殊的战斗直接退出
	local battle_ground = player:BattleGround()
	if battle_ground ~= nil then
		local mission_id =battle_ground:GetParams32("mission_id")
		if mission_id == 10013 or mission_id == 10062 or mission_id == 10001 or mission_id == 10051 then
			player:QuitBattle()
		end
	end
	return ResultCode_ResultOK
end

function x000100_xReConnected(player,params)
	player:SetParams32("HeartBeat",5)
	local level = player:PlayerLevel()
	if level >= LuaMtConfigDataManager():FindConfigValue("inner_hook_start_level"):value1() then
		--解锁内挂功能, 设置阵容
		local dc_container = player:DataCellContainer()
		if dc_container:check_bit_data(BitFlagCellIndex_InnerHookInitFlag) == false then
			local hook = player:GetActorFormation(ActorFigthFormation_AFF_NORMAL_HOOK)
			local pve_reward = player:GetActorFormation(ActorFigthFormation_AFF_PVE_REWARD_MISSION)
			local pve_archaeology = player:GetActorFormation(ActorFigthFormation_AFF_PVE_ARCHAEOLOGY_MISSION)
			local pve_goblin = player:GetActorFormation(ActorFigthFormation_AFF_PVE_GOBLIN_CARBON)
			local pve_blood_carbon = player:GetActorFormation(ActorFigthFormation_AFF_PVE_BLOOD_CARBON)
			if hook ~= nil and pve_reward ~= nil then
				pve_reward:CopyFrom(hook)
				pve_reward:set_type(ActorFigthFormation_AFF_PVE_REWARD_MISSION)
			end
			if hook ~= nil and pve_archaeology ~= nil then
				pve_archaeology:CopyFrom(hook)
				pve_archaeology:set_type(ActorFigthFormation_AFF_PVE_ARCHAEOLOGY_MISSION)
			end
			if hook ~= nil and pve_archaeology ~= nil then
				pve_goblin:CopyFrom(hook)
				pve_goblin:set_type(ActorFigthFormation_AFF_PVE_GOBLIN_CARBON)
			end
			if hook ~= nil and pve_archaeology ~= nil then
				pve_blood_carbon:CopyFrom(hook)
				pve_blood_carbon:set_type(ActorFigthFormation_AFF_PVE_BLOOD_CARBON)
			end
			dc_container:set_bit_data(BitFlagCellIndex_InnerHookInitFlag,true)
		end
	end
	return ResultCode_ResultOK
end


function x000100_xRefreshData2Clinet(player,params)
	local serverlv = LuaServer():ServerLevel()
	local nextlvday = LuaServer():NextServerLvDay()
	player:SendCommonReply("RefreshSeverLvRep",{serverlv,nextlvday},{},{})	
		
	local dc_container =  player:DataCellContainer()
	local ds = LuaActivityManager():GetDateStruct()
	if ds ~= nil then
		dc_container:set_data_32(CellLogicType_ActionData, g_action_data_index.past_days,ds.past_day,true)		
	end	
	
	--每次登陆都更新一下等级
	player:OnLuaFunCall_x("xActorLevelUpTo",{["value"]=player:PlayerLevel() })
	
	--登录初始化下心跳
	player:SetParams32("HeartBeat",5)
	
	x002601_OnUpdate(player)
	x002602_OnUpdate(player)
	x002603_OnUpdate(player)
	--x002604_OnUpdate(player)
	x002605_OnUpdate(player)
	x002606_OnUpdate(player)
	x002607_OnUpdate(player)
	x002608_OnUpdate(player)
	x002609_OnUpdate(player)
	--x002610_OnUpdate(player)
	x002611_OnUpdate(player)
	x002612_OnUpdate(player)
	x002613_OnUpdate(player)
	x002614_OnUpdate(player)
	x002615_OnUpdate(player)
	--x002616_OnUpdate(player)
	--x002617_OnUpdate(player)
	x002618_OnUpdate(player)
	x002619_OnUpdate(player)
	--x002620_OnUpdate(player)
	--x002621_OnUpdate(player)
	x002622_OnUpdate(player)
	x002623_OnUpdate(player)	
	--new
	
	--状态更新	
	x000011_GAcrstatusCheckUpdateNewEx(player, -1)
	--战力比拼数据
	x002635_OnRefreshDataToClient(player,params)
	--等级比拼数据
	x002636_OnRefreshDataToClient(player,params)
	
	return ResultCode_ResultOK
end

function x000100_xOnUpdateGuildInfo(player,pramas)
	--更新工会商店
	local market = player:FindMarket(MarketType_MarketType_Union)
	if market ~= nil then
		market:Refresh(true)
	end
	return ResultCode_ResultOK

end

function x000100_xOnArrangeOver(player,pramas)
	local config = LuaMtMissionManager():FindMission(pramas["id"])
	if config == nil then
		return ResultCode_ResultOK
	end
	local last_mission_id = LuaMtMissionManager():GetLastMissionId(config:type(),config:group())
	if last_mission_id == config:id() then	
		local activity_type = player:OnLuaFunCall_1(501, "MissTypeToActivityType", config:type());
		local members = player:GetTeamMember(true)
		for k,v in pairs(members) do	
			local tplayer = player:Scene():FindPlayer(v)
			if tplayer ~= nil then
				tplayer:SetParams32("arrange_statistic",-1)
				tplayer:SendStatisticsResult(activity_type,true)	
			end
		end
	end		
	return ResultCode_ResultOK
end

function x000100_xOnAddItem(player,pramas)

	local id = pramas["value"]
	local index = LuaItemManager():GetEquipBookIndex(id)
	if index <0 then
		return ResultCode_ResultOK
	end
	
	local dc_container = player:DataCellContainer()
	if dc_container ~= nil then
		local cell_index,rand = math.modf( index / 64);	--在第几个int64
		local offset = math.fmod( index, 64);			--在第几个int64
		local flag = dc_container:get_data_64(Cell64LogicType_EquipBook,cell_index)
		if dc_container:check_bit_data_64(flag,offset) == false then
			local new_flag = dc_container:set_bit_data_64(flag,offset)
			dc_container:set_data_64(Cell64LogicType_EquipBook,cell_index,new_flag,true)
		end		
	end	
	
	return ResultCode_ResultOK	

end

notify_items={}
notify_items["hell_box"]={}
notify_items["team_leader"]={}
notify_items["burning"]={}
notify_items["draw_good"]={
20236001, --助溶剂				
20236002, --幻化石				
28810013, --劣人				
28810015, --五火教主			
28810002, --美屡				
28810019, --神棍德				
28810031, --三颤				
28810020, --花花				
28810004, --兽战				
28810011, --炮姐				
28810018, --女德				
28810027, --老相好				
30101005, --5级红宝石			
30102005, --5级虎眼石			
30103005, --5级猫眼石			
30104005, --5级月亮石			
30105005, --5级翡翠石			
30106005, --5级绿玛瑙			
30107005, --5级暗影石			
30108005, --5级孔雀石			
30109005, --5级黄水晶			
20236021, --维库符文短棒		
20236022,  --德拉诺氏族演讲杖
}
notify_items["draw_normal"]={
28810003, --哀木涕				
28810006, --暗夜男				
28810009, --地中海				
28810010, --傻馒				
28810014, --方砖				
28810022, --沐丝				
28810024, --大小姐				
28810029, --呆贼				
28810013, --劣人				
28810015, --五火教主			
28810002, --美屡				
28810019, --神棍德				
28810031, --三颤				
28810020, --花花				
28810004, --兽战				
28810011, --炮姐				
28810018, --女德				
28810027, --老相好				
30101004, --4级红宝石			
30101005, --5级红宝石			
30102004, --4级虎眼石			
30102005, --5级虎眼石			
30103004, --4级猫眼石			
30103005, --5级猫眼石			
30104004, --4级月亮石			
30104005, --5级月亮石			
30105004, --4级翡翠石			
30105005, --5级翡翠石			
30106004, --4级绿玛瑙			
30106005, --5级绿玛瑙			
30107004, --4级暗影石			
30107005, --5级暗影石			
30108004, --4级孔雀石			
30108005, --5级孔雀石			
30109004, --4级黄水晶			
30109005, --5级黄水晶			
20236001, --助溶剂				
20236002, --幻化石				
20236021, --维库符文短棒		
20236022, --德拉诺氏族演讲杖	
}

function x000100_xOnAddItemNotify(player,pramas)

	local id = pramas["value"]
	local count = pramas["count"]
	local type = pramas["type"]
	local source_id = pramas["source_id"]	

	if type == ItemAddLogType_AddType_Activity and source_id == ActivityType.Hell then
		for k,v in pairs(notify_items["draw_normal"]) do	--试炼宝箱
			if v==id then
				local name = LuaItemManager():GetItemName(id)
				if name ~= nil then
					Lua_send_run_massage("ac_notify_037|"..player:Name().."|"..name.."|"..math.floor(count))
				end
				break
			end
		end
	elseif type == ItemAddLogType_AddType_UseItem and source_id == 20138011 then
		for k,v in pairs(notify_items["draw_normal"]) do	--队长礼盒
			if v==id then
				local name = LuaItemManager():GetItemName(id)
				if name ~= nil then
					Lua_send_run_massage("ac_notify_041|"..player:Name().."|"..name.."|"..math.floor(count))
				end
				break
			end
		end
	elseif type == ItemAddLogType_AddType_Activity and source_id == ActivityType.BurningFight then
		for k,v in pairs(notify_items["draw_normal"]) do	--燃烧军团
			if v==id then
				local name = LuaItemManager():GetItemName(id)
				if name ~= nil then
					Lua_send_run_massage("ac_notify_039|"..player:Name().."|"..name.."|"..math.floor(count))
				end
				break
			end
		end
	elseif type == ItemAddLogType_AddType_Mission then
		local config = LuaMtMissionManager():FindMission(source_id)
		if config ~= nil and config:type() == MissionType_TowerMission then
			print("dddddddddddddddddddddddddddddddddddddddddddddddddddddddd")
			for k,v in pairs(notify_items["draw_normal"]) do	--通天塔
				if v==id then
					local name = LuaItemManager():GetItemName(id)
					if name ~= nil then
						Lua_send_run_massage("ac_notify_047|"..player:Name().."|"..name.."|"..math.floor(count))
					end
					break
				end
			end
		end
	elseif type == ItemAddLogType_AddType_Draw  and ( source_id == 20236010 or source_id == 20236023) then
		for k,v in pairs(notify_items["draw_normal"]) do			--初级招募
			if v==id then
				local name = LuaItemManager():GetItemName(id)
				if name ~= nil then
					Lua_send_run_massage("ac_notify_028|"..player:Name().."|"..name.."|"..math.floor(count))
				end
				break
			end
		end
	elseif type == ItemAddLogType_AddType_Draw  and ( source_id == 20236011 or source_id == 20236024) then
		for k,v in pairs(notify_items["draw_good"]) do			--高级招募
			if v==id then
				local name = LuaItemManager():GetItemName(id)
				if name ~= nil then
					Lua_send_run_massage("ac_notify_028|"..player:Name().."|"..name.."|"..math.floor(count))
				end
				break
			end
		end
	elseif type == ItemAddLogType_AddType_Guild  and  source_id == ActivityType.UnionBattle then
		for k,v in pairs(notify_items["draw_normal"]) do			--工会战宝箱
			if v==id then
				local name = LuaItemManager():GetItemName(id)
				if name ~= nil then
					Lua_send_run_massage("ac_notify_036|"..player:Name().."|"..name.."|"..math.floor(count))
				end
				break
			end
		end
	--elseif type == ItemAddLogType_AddType_MonsterDrop or type == ItemAddLogType_AddType_Hook then
	elseif type == ItemAddLogType_AddType_Mail then
		local config = LuaItemManager():GetEquipmentConfig(id)
		if config ~= nil and config:color() >= EquipmentColor_EQUIP_PURPLE then
			local name = LuaItemManager():GetItemName(id)
			if name ~= nil then
				Lua_send_run_massage("ac_notify_035|"..player:Name().."|"..name)
			end
		end
	end		
	
	return ResultCode_ResultOK	

end


function x000100_xOnAddHero(player,pramas)

	local id = pramas["value"]
	local index = LuaItemManager():GetEquipBookIndex(id)
	if index <0 then
		return
	end
	
	local dc_container = player:DataCellContainer()
	if dc_container ~= nil then
		local cell_index,rand = math.modf( index / 64);	--在第几个int64
		local offset = math.fmod( index, 64);			--在第几个int64
		local flag = dc_container:get_data_64(Cell64LogicType_EquipBook,cell_index)
		if dc_container:check_bit_data_64(flag,offset) == false then
			local new_flag = dc_container:set_bit_data_64(flag,offset)
			dc_container:set_data_64(Cell64LogicType_EquipBook,cell_index,new_flag,true)
		end		
	end	
	
	return ResultCode_ResultOK	

end


function x000100_xOnNewWeek(player,params)
	--传出副本
	if  player:GetSceneId() == 1404 then
		local raid = player:GetRaid()
		if raid ~= nil then
			local msg = S2SCommonMessage:new()
			msg:set_request_name("S2SRaidDestroy")
			local rtid = raid:RuntimeId()
			msg:add_int64_params(rtid)
			player:Scene():ExecuteMessage(msg)
			--msg:delete()
		end
	end
	return ResultCode_ResultOK	
end

function x000100_xOnNewDay(player,params)
	local dc_container = player:DataCellContainer()
	if dc_container == nil then
		return ResultCode_UnknownError
	end
	
	dc_container:set_data_64(Cell64LogicType_LootCell64Type, LootCellIndex_LOOT_COUNT_TODAY, 0, true)
	dc_container:set_data_32(CellLogicType_NumberCell, NumberCellIndex_InspireTimes, 0, true)
	dc_container:set_data_32(CellLogicType_NumberCell, NumberCellIndex_ArenaChallengeTimeToday, 0, true)
	dc_container:set_data_32(CellLogicType_NumberCell, NumberCellIndex_ArenaBuyTimesToday, 0, true)
	
	--更新登陆天数
	dc_container:add_data_32(CellLogicType_ActionData, g_action_data_index.login_count,1,true)
	dc_container:set_data_32(CellLogicType_GuildUserCell, GuildUserCellIndex_DONATION, 0, true)
	
	--奖励活动标记
	player:SetParams32("acr_activity_bit_flag",0)
	
	local markets = player:GetAllMarkets()
	for k,v in pairs( markets ) do
		v:Refresh(true)
	end		
	
	--更新每日试炼场标记，保证lasthp能够每次进入试炼场都更新
	dc_container:reset_bit_data(BitFlagCellIndex_DayTriFieldFlag,true)
	--更新每日世界BOSS标记，保证lasthp能够每次进入试炼场都更新
	dc_container:reset_bit_data(BitFlagCellIndex_DayTriGuildBossFlag,true)
	
	--重置每日在线时间
	dc_container:set_data_32(CellLogicType_ActionData, g_action_data_index.day_online_time,0,false)		
	
	--重置每日重置计数
	dc_container:set_data_32(CellLogicType_ActionData, g_action_data_index.day_charge_count,0,true)
	
	local serverlv = LuaServer():ServerLevel()
	local nextlvday = LuaServer():NextServerLvDay()
	player:SendCommonReply("RefreshSeverLvRep",{serverlv,nextlvday},{},{})
	
	local ds = LuaActivityManager():GetDateStruct()
	if ds ~= nil then
		dc_container:set_data_32(CellLogicType_ActionData, g_action_data_index.past_days,ds.past_day,true)		
	end	
	
	player:SetParams32("guildbattle_box",INVALID_INT32)
	
	--x002620_xOnNewDay(player,params)
	--x002621_xOnNewDay(player,params)
	x002602_xOnNewDay(player,params)
	x002619_xOnNewDay(player,params)
	--x002616_xOnNewDay(player,params)
	--x002617_xOnNewDay(player,params)
	x002618_xOnNewDay(player,params)
	x002601_xOnNewDay(player,params)
	x002611_xOnNewDay(player,params)
	x002603_xOnNewDay(player,params)
	x002622_xOnNewDay(player,params)
	x002607_xOnNewDay(player,params)
	x002614_xOnNewDay(player,params)
	x002615_xOnNewDay(player,params)
	x002605_xOnNewDay(player,params)
	x002612_xOnNewDay(player,params)
	x002613_xOnNewDay(player,params)
	x002606_xOnNewDay(player,params)
	x002609_xOnNewDay(player,params)
	x002623_xOnNewDay(player,params)
	x002608_xOnNewDay(player,params)
	--x002604_xOnNewDay(player,params)
	
	--超值礼包
	x002629_xOnNewDay(player,params)
	x002639_xOnNewDay(player,params)
	x002640_xOnNewDay(player,params)
	x002641_xOnNewDay(player,params)
	x002642_xOnNewDay(player,params)	
	--惊喜礼包
	x002643_xOnNewDay(player,params)
	--特惠礼包
	x002624_xOnNewDay(player,params)
	--连续充值活动奖励没领取的需要自动领取	
	x002631_xOnNewDay(player,params)	
	
	--数据重置
	x000011_GAcrstatusCheckResetNewEx(player,-1)
	--状态更新	
	x000011_GAcrstatusCheckUpdateNewEx(player, -1)
		
	--其他更新	
	x000501_xOnNewDay(player,params)

	
	return ResultCode_ResultOK
end

function x000100_xOnLevelUpTo(player,params)
	
	local dc_container = player:DataCellContainer()
	if dc_container == nil then
		return ResultCode_UnknownError
	end
	
	local level = params["value"]
	if level == 10 then
		local proxy = player:GetSimpleImpactSet()
		proxy:DecSimpleImpact(NewSpeedUpImpact)
	end
		
	local action_data=dc_container:get_data_32(CellLogicType_ActionData, g_action_data_index.level_count)	
	if action_data < level then
		action_data = level
		dc_container:set_data_32(CellLogicType_ActionData, g_action_data_index.level_count,action_data,true)
	end	

	if x000100_CheckFunction(player,{ ["activity_name_5"]=999} ) == ResultCode_ResultOK then
		--解锁试炼场
		if dc_container:check_bit_data(BitFlagCellIndex_FirstTriFieldFlag) == false then			
			player:CopyActorFormation(ActorFigthFormation_AFF_NORMAL_HOOK, ActorFigthFormation_AFF_PVP_TRIAL_FIELD)
			player:ResetTrialFieldData()
			dc_container:set_bit_data(BitFlagCellIndex_FirstTriFieldFlag,true)
		end
	end
	
	if level >= LuaMtConfigDataManager():FindConfigValue("inner_hook_start_level"):value1() then
		--解锁内挂功能, 设置阵容
		if dc_container:check_bit_data(BitFlagCellIndex_InnerHookInitFlag) == false then			
			player:CopyActorFormation(ActorFigthFormation_AFF_NORMAL_HOOK, ActorFigthFormation_AFF_PVE_REWARD_MISSION)
			player:CopyActorFormation(ActorFigthFormation_AFF_NORMAL_HOOK, ActorFigthFormation_AFF_PVE_ARCHAEOLOGY_MISSION)
			player:CopyActorFormation(ActorFigthFormation_AFF_NORMAL_HOOK, ActorFigthFormation_AFF_PVE_GOBLIN_CARBON)
			player:CopyActorFormation(ActorFigthFormation_AFF_NORMAL_HOOK, ActorFigthFormation_AFF_PVE_BLOOD_CARBON)
			dc_container:set_bit_data(BitFlagCellIndex_InnerHookInitFlag,true)
		end
	end
	
	--解锁世界BOSS，设置世界BOSS
	if dc_container:check_bit_data(BitFlagCellIndex_FirstWorldBossFlag) == false then
		local activity_type = player:OnLuaFunCall_n(501, "GetWorldBossAc",{} )	
		local ac = LuaActivityManager():FindActivity(activity_type)
		if LuaActivityManager():CheckLevel(ac,level) then
			for i=ActorFigthFormation_AFF_PVP_WORLDBOSS_Sun,ActorFigthFormation_AFF_PVP_WORLDBOSS_Sat,1 do
				player:CopyActorFormation(ActorFigthFormation_AFF_NORMAL_HOOK, i);
			end			
			dc_container:set_bit_data(BitFlagCellIndex_FirstWorldBossFlag,true)
		end	
	end
	
	--战场
	if dc_container:check_bit_data(BitFlagCellIndex_FirstBattleFieldFlag) == false then		
		local ac = LuaActivityManager():FindActivity(ActivityType.BattleCarbon2)
		if LuaActivityManager():CheckLevel(ac,level) then		
			player:CopyActorFormation(ActorFigthFormation_AFF_NORMAL_HOOK, ActorFigthFormation_AFF_PVP_BATTLE_FIELD)
			dc_container:set_bit_data(BitFlagCellIndex_FirstBattleFieldFlag,true)
		end
	end	
	
	--公会BOSS血量重置
	if dc_container:check_bit_data(BitFlagCellIndex_DayTriGuildBossFlag) == false then	
		player:ResetPlayerActorLastHp(LastHpType_GBOSSLastHp)
	end
	return ResultCode_ResultOK
end

		
function x000100_xUpdateBattleScore(player,params)
	local dc_container = player:DataCellContainer()
	if dc_container == nil then
		return ResultCode_UnknownError
	end	
	local action_data=dc_container:get_data_32(CellLogicType_ActionData, g_action_data_index.power_count)	
	if action_data < params["value"] then
		action_data = params["value"]
		dc_container:set_data_32(CellLogicType_ActionData, g_action_data_index.power_count,action_data,true)
	end		
	return ResultCode_ResultOK
end
function x000100_xPlayerRaidStage(player,params)
	local dc_container = player:DataCellContainer()
	if dc_container == nil then
		return ResultCode_UnknownError
	end	
	local action_data=dc_container:get_data_32(CellLogicType_ActionData, g_action_data_index.stage_count)	
	if action_data < params["value"] then
		action_data = params["value"]
		dc_container:set_data_32(CellLogicType_ActionData, g_action_data_index.stage_count,action_data,true)
	end		
	return ResultCode_ResultOK
end

function x000100_xDrawCrystal(player,params)
	local dc_container = player:DataCellContainer()
	if dc_container == nil then
		return ResultCode_UnknownError
	end	
	local action_data= params["value"]
	dc_container:add_data_32(CellLogicType_ActionData, g_action_data_index.high_draw_count,action_data,true)
	return ResultCode_ResultOK
end
function x000100_xDrawGold(player,params)
	local dc_container = player:DataCellContainer()
	if dc_container == nil then
		return ResultCode_UnknownError
	end	
	local action_data= params["value"]
	dc_container:add_data_32(CellLogicType_ActionData, g_action_data_index.low_draw_count,action_data,true)
	return ResultCode_ResultOK
end
function x000100_xOnMonsterDead(player,params)
	local dc_container = player:DataCellContainer()
	if dc_container == nil then
		return ResultCode_UnknownError
	end	
	
	--首次紫装/提到沼泽/7	
	local flag = dc_container:get_data_64(Cell64LogicType_NumberCell64, NumberCell64Index_GuideFirstFlag)
	--print("params[scene_id]"..params["scene_id"])
	if dc_container:check_bit_data_64(flag,g_GuideFirstFlag.NiceEquip) == false then
		if params["scene_id"] == 10 and params["battle_type"] == BattleGroundType_PVE_HOOK then			
			local check_count = dc_container:get_data_32(CellLogicType_ActionData, g_action_data_index.nice_equip_value)
			print("check_count"..check_count)
			if check_count+1 >= 500 then
				local new_flag  = dc_container:set_bit_data_64(flag,g_GuideFirstFlag.NiceEquip) 
				dc_container:set_data_64(Cell64LogicType_NumberCell64, NumberCell64Index_GuideFirstFlag,new_flag,false)				
				local item_drop = LuaMtItemPackageManager():GenDropFromGroup(307)
				if item_drop ~= nil then
					player:AddItemByIdWithNotify(item_drop:item_id(),1,ItemAddLogType_AddType_Package,307,1)
				end
			end
			dc_container:add_data_32(CellLogicType_ActionData, g_action_data_index.nice_equip_value,1,false)
		end
	end
		
	if params["battle_type"] == BattleGroundType_PVE_STAGE_BOSS then
		local action_data = params["monster_count"]
		dc_container:add_data_32(CellLogicType_ActionData, g_action_data_index.stage_boss_count,action_data,true)
	end
	
	return ResultCode_ResultOK
end
function x000100_xOnEnchantEquip(player,params)
	local dc_container = player:DataCellContainer()
	if dc_container == nil then
		return ResultCode_UnknownError
	end	
	dc_container:add_data_32(CellLogicType_ActionData, g_action_data_index.magic_count,1,true)
	
	--首次附魔
	local flag = dc_container:get_data_64(Cell64LogicType_NumberCell64, NumberCell64Index_GuideFirstFlag)
	if  dc_container:check_bit_data_64(flag,g_GuideFirstFlag.EnchantEquip) == false then
		new_flag = dc_container:set_bit_data_64(flag,g_GuideFirstFlag.EnchantEquip)
		dc_container:set_data_64(Cell64LogicType_NumberCell64, NumberCell64Index_GuideFirstFlag,new_flag,true)
		
		LuaServer():LuaLog("NumberCell64Index_GuideFirstFlag done! guid ="..player:Guid(),g_log_level.LL_INFO)
	end
	
	return ResultCode_ResultOK
end
function x000100_xCostItem(player,params)
	local dc_container = player:DataCellContainer()
	if dc_container == nil then
		return ResultCode_UnknownError
	end		
	--print("costitem"..params["value"])
	if params["value"] == 20125001 or  params["value"] == 20125002 then
		local action_data= params["count"]
		dc_container:add_data_32(CellLogicType_ActionData, g_action_data_index.treasure_count,action_data,true)
	end
	return ResultCode_ResultOK
end

function x000100_xOnConsume(player,params)
	local dc_container = player:DataCellContainer()
	if dc_container == nil then
		return ResultCode_UnknownError
	end	
	
	local action_data= params["value"]
	dc_container:add_data_32(CellLogicType_ActionData, g_action_data_index.all_consume_count,action_data,true)
	
	if player:OnLuaFunCall_0(2617,"IsAcrActive") == ResultCode_ResultOK then
		dc_container:add_data_32(CellLogicType_ActionData, g_action_data_index.date_consume_count,action_data,true)
	end

	return ResultCode_ResultOK
end

function x000100_OnChargeDoubleCheck(player,params)
	local dc_container = player:DataCellContainer()
	if dc_container == nil then
		return ResultCode_UnknownError
	end		
		
	local index = tonumber(params["index"])
	local count = tonumber(params["count"])
	
	if index == 0 or index == 1 then
		count = 0
	else
		--双倍判定
		local flag_data = dc_container:get_data_32(CellLogicType_AcRewardCell,g_acr_datacell_index.charge_double_flag)
		if dc_container:check_bit_data_32(flag_data,index) == false then
			local new_flag = dc_container:set_bit_data_32(flag_data,index)
			dc_container:set_data_32(CellLogicType_AcRewardCell,g_acr_datacell_index.charge_double_flag,new_flag,true)
		else
			count = 0
		end
	end

	local charge_times = dc_container:get_data_32(CellLogicType_ActionData, g_action_data_index.charge_times)
	return {count,charge_times}
end

function x000100_OnBigTick(player)
	local dc_container = player:DataCellContainer()
	if dc_container == nil then
		return ResultCode_UnknownError
	end		
		
	if player:IsDisConnected() == false then
		local time = player:GetParams32("HeartBeat")
		if time < 0 then
			player:DisConnected()
		else
			player:SetParams32("HeartBeat",time-3)
		end
	end

	 if player:GetParams32("CheckMission")>0 then
		x000502_OnActionResolve(player,{});
		player:SetParams32("CheckMission",0)
	 end
	 
	x002623_xOnBigTick(player, {})
	x002628_xOnBigTick(player, {})

	return ResultCode_ResultOK
end


function x000100_xAddActivityTimes(player,params)
	local dc_container = player:DataCellContainer()
	if dc_container == nil then
		return ResultCode_UnknownError
	end	

	local ac_type= params["value"]
	
	if ac_type == ActivityType.GoldReward  then
		dc_container:add_data_32(CellLogicType_ActionData, g_action_data_index.reward_mission_count,1,true)
	elseif ac_type == ActivityType.CorpsFight  then
		dc_container:add_data_32(CellLogicType_ActionData, g_action_data_index.corps_mission_count,1,true)
	elseif ac_type == ActivityType.GoblinCarbon  then
		dc_container:add_data_32(CellLogicType_ActionData, g_action_data_index.goblin_count,1,true)
	elseif ac_type == ActivityType.Arena  then
		dc_container:add_data_32(CellLogicType_ActionData, g_action_data_index.arena_count,1,true)
	elseif ac_type == ActivityType.SkyFallFight  then
		dc_container:add_data_32(CellLogicType_ActionData, g_action_data_index.skyfall_count,1,true)
	elseif ac_type == ActivityType.BloodCarbon  then
		dc_container:add_data_32(CellLogicType_ActionData, g_action_data_index.blood_count,1,true)
	elseif ac_type == ActivityType.TinyThreeCarbon  then
		dc_container:add_data_32(CellLogicType_ActionData, g_action_data_index.tinythree_count,1,true)
	elseif ac_type==ActivityType.WolrdBossCarbon1 or
		ac_type==ActivityType.WolrdBossCarbon2 or
		ac_type==ActivityType.WolrdBossCarbon3 or
		ac_type==ActivityType.WolrdBossCarbon4 or
		ac_type==ActivityType.WolrdBossCarbon5 or
		ac_type==ActivityType.WolrdBossCarbon6 or
		ac_type==ActivityType.WolrdBossCarbon7 then	
		dc_container:add_data_32(CellLogicType_ActionData, g_action_data_index.world_boss_count,1,true)
	elseif ac_type == ActivityType.UnionBoss  then
		dc_container:add_data_32(CellLogicType_ActionData, g_action_data_index.union_boss_count,1,true)
	elseif ac_type == ActivityType.AcUnionMission  then		
		dc_container:add_data_32(CellLogicType_ActionData, g_action_data_index.union_mission_count,1,true)
	elseif ac_type == ActivityType.Hell  then
		local action_data=dc_container:get_data_32(CellLogicType_ActionData, g_action_data_index.hell_stage_count)	
		local now_times = player:OnLuaFunCall_1(501,"GetActivityTimes",ac_type)
		if action_data < now_times then
			dc_container:set_data_32(CellLogicType_ActionData, g_action_data_index.hell_stage_count,now_times,true)
		end	
	else
		--
	end	
	return ResultCode_ResultOK
end


function x000100_xOnInsetEquip(player,params)

	local dc_container = player:DataCellContainer()
	if dc_container == nil then
		return ResultCode_UnknownError
	end	
	
	local embed_count = {}
	embed_count[1] = {0, g_action_data_index.embed1_count }
	embed_count[2] = {0, g_action_data_index.embed2_count }
	embed_count[3] = {0, g_action_data_index.embed3_count }
	
	local gems = player:AllInsetGems()
	for k,item in pairs(gems) do
		local gem_config = LuaItemManager():GetGemConfig(item)
		for p,z in pairs( embed_count ) do 
			if p <= gem_config:level() then
				z[1] = z[1]+1
			end
		end							
	end	
		
	for p,z in pairs(embed_count) do
		local index = z[2]
		
		--print("embed lv"..p.." count "..z[1].." index "..index)
		local action_data=dc_container:get_data_32(CellLogicType_ActionData,index)	
		if action_data < z[1] then
			dc_container:set_data_32(CellLogicType_ActionData, index,z[1],true)
		end	
	end	
	
	return ResultCode_ResultOK
end



function x000100_xOnUpgradeSKill(player,params)

	local dc_container = player:DataCellContainer()
	if dc_container == nil then
		return ResultCode_UnknownError
	end	
	
	local level_count = {}
	level_count[5]  = {0, g_action_data_index.skill_level5_count  }
	level_count[10] = {0, g_action_data_index.skill_level10_count }
	level_count[15] = {0, g_action_data_index.skill_level15_count }
	level_count[20] = {0, g_action_data_index.skill_level20_count }
	level_count[25] = {0, g_action_data_index.skill_level25_count }
	level_count[30] = {0, g_action_data_index.skill_level30_count }
	level_count[35] = {0, g_action_data_index.skill_level35_count }
	level_count[40] = {0, g_action_data_index.skill_level40_count }

	local actors = player:Actors()
	
	for k,actor in pairs( actors ) do 
		local skills = actor:DbSkills()
		for m,n in pairs( skills ) do 
			local dbinfo = n:DbInfo()
			for p,z in pairs( level_count ) do 
				if p <= dbinfo:skill_level() then
					z[1] = z[1]+1
				end
			end			
		end
	end
	
	for p,z in pairs(level_count) do
		local index = z[2]
		
		--print("skill lv"..p.." count "..z[1].." index "..index)
		local action_data=dc_container:get_data_32(CellLogicType_ActionData,index)	
		if action_data < z[1] then
			dc_container:set_data_32(CellLogicType_ActionData, index,z[1],true)
		end	
	end	

	return ResultCode_ResultOK
end


function x000100_xOnUpgradeColor(player,params)

	local dc_container = player:DataCellContainer()
	if dc_container == nil then
		return ResultCode_UnknownError
	end	
	
	local color_count = {}
	color_count[2] = {0, g_action_data_index.color2_count }
	color_count[3] = {0, g_action_data_index.color3_count }
	color_count[4] = {0, g_action_data_index.color4_count }
	color_count[5] = {0, g_action_data_index.color5_count }
	
	local actors = player:Actors()
	
	for k,actor in pairs( actors ) do 
		local dbinfo = actor:ActorDBInfo()
		for p,z in pairs( color_count ) do 
			if p <= dbinfo:color() then
				z[1] = z[1]+1
			end
		end	
	end
	
	for p,z in pairs(color_count) do
		local index = z[2]
		
		--print("color lv"..p.." count "..z[1].." index "..index)
		
		local action_data=dc_container:get_data_32(CellLogicType_ActionData,index)	
		if action_data < z[1] then
			dc_container:set_data_32(CellLogicType_ActionData, index,z[1],true)
		end	
	end	
	
	return ResultCode_ResultOK
end


--更新奖励活动
function x000100_xUpdateAcrStatus(player,params)
	local dc_container = player:DataCellContainer()
	if dc_container == nil then	
		return
	end
	
	local index = params["index"]
	local flag_set = player:GetParams32("acr_activity_bit_flag")	
	local old_status = dc_container:check_bit_data_32(index)
	local new_status = true
	local new_data = 0
	
	if params[is_active]==1 then	
		new_data = dc_container:set_bit_data_32(index)
	else	
		new_status = false
		new_data = dc_container:reset_bit_data_32(index)
	end	
	player:SetParams32("acr_activity_bit_flag",new_data)
	
	if old_status ~= new_status then
		if old_status == false then  --说明是刚刚开启
			player:SendLuaPacket("msg_acr_data", {params["script_id"],params["week_day"]},{},{})
		else						 --说明刚刚结束
			player:SendLuaPacket("msg_acr_close", {params["script_id"]},{},{})	
		end	
	end
end

--5宝
function x000100_OnExchangeT5(player,params)

	if player:BagLeftCapacity(ContainerType_Layer_ItemBag,1) == false then
		return player:SendClientNotify( "BagIsFull",-1,-1)
	end
	
	for k=1,#g_Treasure5,1 do
		local count = player:GetBagItemCount( g_Treasure5[1] )
		if count <= 0 then
			return player:SendClientNotify( "CostItemNotEnough",-1,-1)
		end
	end
	
	for k=1,#g_Treasure5,1 do
		if player:DelItemById(g_Treasure5[k],1,ItemDelLogType_DelType_Exchange,0)== false then
			return player:SendClientNotify( "CostItemNotEnough",-1,-1)
		end
	end
	
	player:AddItemByIdWithNotify(20236024,1,ItemAddLogType_AddType_Exchange,0,1)
	
	return ResultCode_ResultOK
end

function x000100_ResolveAllReward(player,ac_type)
	
	if  ac_type == ActivityType.GoldReward then
		return x000508_ResolveAllReward(player)    
	elseif  ac_type == ActivityType.CorpsFight then
		return x000507_ResolveAllReward(player)
	elseif  ac_type == ActivityType.GoblinCarbon then
		return x000509_ResolveAllReward(player)
	elseif  ac_type == ActivityType.BigThreeCarbon then
		return x000514_ResolveAllReward(player)
	elseif  ac_type == ActivityType.TinyThreeCarbon then
		return x000510_ResolveAllReward(player)
	elseif  ac_type == ActivityType.SkyFallFight then
		return x001007_ResolveAllReward(player)
	elseif  ac_type == ActivityType.BloodCarbon then
		return x000512_ResolveAllReward(player)
	elseif  ac_type == ActivityType.DayKill then
		return x000516_ResolveAllReward(player)
	elseif  ac_type == ActivityType.BlackCarbon then
		return x000511_ResolveAllReward(player)
	end

	return 0,0
end

function x000100_xOnMissRewardResolve(player,params)

	--25级之前没有找回功能
	if player:PlayerLevel()< 25 then
		return ResultCode_ResultOK
	end
	
	local cell_container = player:DataCellContainer()
	if cell_container == nil then
		return ResultCode_ResultOK
	end	

	--当日参与过的都不算,没开启的也不算	
	for k,v in pairs(g_MissRewardFlag) do
		local times = cell_container:get_data_32(CellLogicType_ActionData,	v[2])
		local ac_type = v[1]	
		local ac_config = LuaActivityManager():FindActivity(ac_type)
		if ac_config ~= nil and LuaActivityManager():CheckLevel(ac_config,player:PlayerLevel()) then	
			local flag = cell_container:get_data_64(Cell64LogicType_NumberCell64, NumberCell64Index_MissRewardFlag)
			if ac_type >0 and ac_type <64 then
				if cell_container:check_bit_data_64(flag,ac_type) == false then
					local gold,exp = x000100_ResolveAllReward(player, ac_type)  
					if v[2] >= 0 then
						cell_container:add_data_32(CellLogicType_ActionData,	v[2],	1,	true)		
					end
					if exp > 0 and v[3] >= 0 then
						cell_container:set_data_32(CellLogicType_ActionData, 	v[3],	exp,	true)
					end
					if gold > 0 and v[4] >= 0 then
						cell_container:set_data_32(CellLogicType_ActionData, 	v[4],	gold,	true)
					end
				end
			end
		end
	end		
	return ResultCode_ResultOK
end

function x000100_ResetMissReward(player,params)
	local cell_container = player:DataCellContainer()
	if cell_container == nil then
		return ResultCode_ResultOK
	end	
	--当日参与过的都不算,没开启的也不算	
	for k,v in pairs(g_MissRewardFlag) do
		cell_container:set_data_32(CellLogicType_ActionData,	v[2], 0,true)
		local ac_type = v[1]	
		local flag = cell_container:get_data_64(Cell64LogicType_NumberCell64, NumberCell64Index_MissRewardFlag)
		if ac_type >0 and ac_type <64 then
			local newflag = cell_container:reset_bit_data_64(flag,ac_type)     
			cell_container:set_data_64(Cell64LogicType_NumberCell64, NumberCell64Index_MissRewardFlag, newflag ,true)	
			if v[2]>= 0 then
				cell_container:set_data_32(CellLogicType_ActionData,	v[2],	0,	true)
			end
			if v[3]>= 0 then
				cell_container:set_data_32(CellLogicType_ActionData, 	v[3],	0,	true)
			end
			if v[4]>= 0 then
				cell_container:set_data_32(CellLogicType_ActionData, 	v[4],	0,	true)
			end
		end		
	end		
	return ResultCode_ResultOK
end


function x000100_DoMissReward(player,params)
 

	local ac_type = params["ac_type"]
	local times = params["times"]
	local type = params["type"]
	
	local cell_container = player:DataCellContainer()
	if cell_container == nil then
		return ResultCode_ResultOK
	end
	
	if ac_type <=0 or ac_type >=64 then
		return ResultCode_ResultOK
	end	
 
	local config={}
	for k,v in pairs(g_MissRewardFlag) do
		if v[1] == ac_type then
			config =v
			break
		end
	end
 
	--次数判定
	local can_times = cell_container:get_data_32(CellLogicType_ActionData,	config[2])
	if times > can_times  then
		return ResultCode_TimesLimit
	end
		
	local exp = 0
	local gold = 0
	local rate = 0.5
	for i=1,times,1 do
		if  config[3]>=0 then
			exp =  exp + cell_container:get_data_32(CellLogicType_ActionData, config[3]	)
		end
		if  config[4]>=0 then
			gold = gold + cell_container:get_data_32(CellLogicType_ActionData, config[4])
		end
	end
 	
	local level_config = LuaMtActorConfig():GetLevelData(player:PlayerLevel())
	if level_config == nil then
		return 0,0
	end
		
	if type == 1 then--金币找回		
		if player:DelItemById(TokenType_Token_Gold,level_config:miss_gold()*times,ItemDelLogType_DelType_MissReward,ac_type) == false then
			return ResultCode_GoldNotEnough
		end			
		rate = 0.7
	else--钻石找回	
		if player:DelItemById(TokenType_Token_Crystal,level_config:miss_crystal()*times,ItemDelLogType_DelType_MissReward,ac_type) == false then
			return ResultCode_CrystalNotEnough
		end
		rate = 0.9
	end
	cell_container:add_data_32(CellLogicType_ActionData, config[2], -times, true)	
			
	--经验
	exp = math.floor(rate*exp)+1
	if exp > 0 then		
		player:AddItemByIdWithNotify(TokenType_Token_Exp, exp, ItemAddLogType_AddType_MissReward,ac_type,1)
	end	
	--金币
	gold = math.floor(rate*gold)+1
	if gold > 0 then
		player:AddItemByIdWithNotify(TokenType_Token_Gold, gold, ItemAddLogType_AddType_MissReward,ac_type,1);
	end	
	
	--道具
	local item_count = 0
	if ac_type == ActivityType.BigThreeCarbon then
		item_count = times*(math.floor(rate*1)+1)
		player:AddItemByIdWithNotify(config[5][1], item_count, ItemAddLogType_AddType_MissReward,ac_type,1);
	elseif ac_type == ActivityType.BlackCarbon then
		item_count = times*(math.floor(rate*18)+1)
		player:AddItemByIdWithNotify(config[5][1], item_count, ItemAddLogType_AddType_MissReward,ac_type,1);
	elseif ac_type == ActivityType.Ancholage then
		item_count = times*(math.floor(rate*20)+1)
		player:AddItemByIdWithNotify(config[5][1], item_count, ItemAddLogType_AddType_MissReward,ac_type,1);
	elseif ac_type == ActivityType.BloodCarbon then
		for i=0,times-1,1 do
			x000512_ResolvePotionReward(player,rate)
		end
	end	
		
	return ResultCode_ResultOK
end

function x000100_xOnInitCell(player,params)

	local dc_container = player:DataCellContainer()
	local i=0
	for i = MissionCellIndex_MissionCellStart,MissionCellIndex_MissionCellEnd-1,1 do
		dc_container:try_init_int32_data(CellLogicType_MissionCell, i, 0);
	end
	for i = AcRewardIndex_RewardDataStart,AcRewardIndex_RewardDataEnd-1,1 do
		dc_container:try_init_int32_data(CellLogicType_AcRewardCell, i, 0)
	end
	for i = ActivityDataIndex_ActivityDataStart,ActivityDataIndex_ActivityDataEnd-1,1 do
		dc_container:try_init_int32_data(CellLogicType_ActivityCell, i, 0)
	end
	for i = TargetDataIndex_TargetDataStart,TargetDataIndex_TargetDataEnd-1,1 do
		dc_container:try_init_int32_data(CellLogicType_TargetData, i, 0)
	end		
	for i = ActionDataIndex_ActionDataStart,ActionDataIndex_ActionDataEnd-1,1 do
		dc_container:try_init_int32_data(CellLogicType_ActionData, i, 0)
	end
	for i = ResetTimeIndex_ResetTimeStart,ResetTimeIndex_ResetTimeEnd-1,1 do
		dc_container:try_init_int32_data(CellLogicType_ResetTime, i, 0)
	end
	--初始化新手引导数据
	for i = GuideCellIndex_GuideGroupStart,GuideCellIndex_GuideGroupEnd-1,1 do
		dc_container:try_init_int32_data(CellLogicType_GuideCell, i, 0)
	end
	for i = GuildUserCellIndex_DONATION,GuildUserCellIndex_GUILDUSER_MAX-1,1 do
		dc_container:try_init_int32_data(CellLogicType_GuildUserCell, i, 0)
	end

	--bit
	dc_container:try_init_int32_data(CellLogicType_BitFlagCell, 1, 0)
	--number32
	for i = 0,63,1 do
		dc_container:try_init_int32_data(CellLogicType_NumberCell, i, 0)
	end
	--number64
	for i = 0,63,1 do
		dc_container:try_init_int64_data(Cell64LogicType_NumberCell64, i, 0)
	end
	return ResultCode_ResultOK
end


function x000100_CheckFunction(player,params)

	local name = ""
	for k,v in pairs(params) do
		if v==999 then
			name = k
			break
		end
	end
	
	local config = LuaMtConfigDataManager():FindFunction(name)
	if config == nil then
		--player:SendClientNotify("InvalidConfigData",-1,-1)
		return ResultCode_InternalResult
	end
	
	local dc_container =  player:DataCellContainer()
	if config:unlock() == 1 then
		local action_data=  player:PlayerLevel()
		--print("name "..name.." indexdddddddddddddddd "..action_data.." config "..config:param())
		if action_data<config:param() then
			--player:SendClientNotify("duoshaojijiesuo|"..config:param(),-1,1)
			return ResultCode_InternalResult
		end
	elseif config:unlock() ==2 then
		local action_data=dc_container:get_data_32(CellLogicType_ActionData, g_action_data_index.stage_count)	
		--print("indexdddddddddddddddd"..action_data)
		if action_data<config:param() then
			--player:SendClientNotify("functionunlock",-1,1)
			return ResultCode_InternalResult
		end
	else		
		local flag = dc_container:get_data_64(Cell64LogicType_NumberCell64, NumberCell64Index_GuideFirstFlag)
		local flagindex = config:param() 
		--print("indexdddddddddddddddd"..flagindex..name)
		if  dc_container:check_bit_data_64(flag,flagindex) == false then
			player:SendClientNotify("functionunlock",-1,1)
			return ResultCode_InternalResult
		end
	end
	
	return ResultCode_ResultOK
end

function x000100_GetPowerCount(player)
	local dc_container = player:DataCellContainer()
	if dc_container == nil then
		return -1
	end	
	local action_data=dc_container:get_data_32(CellLogicType_ActionData, g_action_data_index.power_count)	
	return action_data	
end

function x000100_OnChargeItem(player,orderid,productid,ip, channel_id )


	print("channel_id="..channel_id.." productid="..productid.." orderid="..orderid)
	
	local index = LuaMtMarketManager():FindChargeIndex(channel_id,productid)
	if index == -1 then
		LuaServer():LuaLog("xOnChangeOk failed! invalid charge_list! channel_id="..channel_id.." productid="..productid.." orderid="..orderid.." Guid="..player:Guid(),g_log_level.LL_INFO)
		return player:SendClientNotify("Invalid_Request", -1, -1)
	end
	
	local config = LuaMtMarketManager():FindChargeItem(index)
	if config == nil then
		LuaServer():LuaLog("xOnChangeOk failed! invalid charge_list! index="..index.." channel_id="..channel_id.." productid="..productid.." orderid="..orderid.." Guid="..player:Guid(),g_log_level.LL_INFO)
		return player:SendClientNotify("Invalid_Request", -1, -1)
	end
		
	local id = config:effect_value()
	local count = config:effect_count()
	local extra_count = config:extra_value()
	local price = config:rmb_price()
		
	local dc_container = player:DataCellContainer()		
	--累充
	dc_container:add_data_32(CellLogicType_ActionData, g_action_data_index.charge_times,1,true)	
	local charge_times = dc_container:get_data_32(CellLogicType_ActionData, g_action_data_index.charge_times)
	dc_container:add_data_32(CellLogicType_ActionData, g_action_data_index.all_charge_count,price,true)	
	
	player:AddAllCharge(price)
	if config:group_type()=="charge" then
		--双倍判定
		local flag_data = dc_container:get_data_32(CellLogicType_AcRewardCell,g_acr_datacell_index.charge_double_flag)
		if dc_container:check_bit_data_32(flag_data,index) == false then
			local new_flag = dc_container:set_bit_data_32(flag_data,index)
			dc_container:set_data_32(CellLogicType_AcRewardCell,g_acr_datacell_index.charge_double_flag,new_flag,true)
			count = count*2
		end	
		count = count + extra_count
		if id>=0 and count >= 0 then
			player:AddItemByIdWithNotify(id, count, ItemAddLogType_AddType_Charge, index, 0)
		end
		--日充(礼包不算)
		dc_container:add_data_32(CellLogicType_ActionData, g_action_data_index.day_charge_count,price,true)
		player:OnLuaFunCall_x("xOnCharge",{ ["value"]=price , ["index"]=index })	
		
	elseif config:group_type()=="card" then
		if index == 0 then 
			--终生卡
			if dc_container:check_bit_data( BitFlagCellIndex_LifeCardFlag) then
				LuaServer():LuaLog("xOnChangeOk failed! index="..index.." channel_id="..channel_id.." productid="..productid.." orderid="..orderid.." Guid="..player:Guid(),g_log_level.LL_ERROR)
				return player:SendClientNotify("ac_notify_016", -1, -1)
			end
			dc_container:set_bit_data(BitFlagCellIndex_LifeCardFlag, true)
			player:AddItemByIdWithNotify(20140002, 4, ItemAddLogType_AddType_Charge, index,0)
			player:AddItemByIdWithNotify(20140001, 2, ItemAddLogType_AddType_Charge, index,0)
			player:AddItemByIdWithNotify(20140005, 2, ItemAddLogType_AddType_Charge, index,0)
			--日充(礼包不算)
			dc_container:add_data_32(CellLogicType_ActionData, g_action_data_index.day_charge_count,price,true)
			player:OnLuaFunCall_x("xOnCharge",{ ["value"]=price , ["index"]=index })	
			
		elseif index == 1 then
			--月卡
			if dc_container:check_bit_data( BitFlagCellIndex_MonthCardFlag) then
				LuaServer():LuaLog("xOnChangeOk failed! index="..index.." channel_id="..channel_id.." productid="..productid.." orderid="..orderid.." Guid="..player:Guid(),g_log_level.LL_ERROR)
				return player:SendClientNotify("ac_notify_017", -1, -1)
			end
			dc_container:set_bit_data(BitFlagCellIndex_MonthCardFlag, true)
			dc_container:set_data_32( CellLogicType_AcRewardCell, g_acr_datacell_index.month_card_flag, 0, true);
			--日充(礼包不算)
			dc_container:add_data_32(CellLogicType_ActionData, g_action_data_index.day_charge_count,price,true)
			player:OnLuaFunCall_x("xOnCharge",{ ["value"]=price , ["index"]=index })	
		elseif index == 6 then
			--基金
			if dc_container:check_bit_data( BitFlagCellIndex_InvestFlag) then
				LuaServer():LuaLog("xOnChangeOk failed! index="..index.." channel_id="..channel_id.." productid="..productid.." orderid="..orderid.." Guid="..player:Guid(),g_log_level.LL_ERROR)
				return player:SendClientNotify("ac_notify_019", -1, -1)
			end
			dc_container:set_bit_data(BitFlagCellIndex_InvestFlag, true)
			dc_container:set_data_32(CellLogicType_AcRewardCell, g_acr_datacell_index.invest_flag, 0,true)
			dc_container:set_data_32(CellLogicType_AcRewardCell, g_acr_datacell_index.invest_all_flag, 0,true)
			
			--同步一下数据
			local now_count = LuaServer():GetInvestCount()
			dc_container:set_data_32(CellLogicType_ActionData, g_action_data_index.invest_count, now_count+1,true)
			
			LuaServer():SendS2GCommonMessage("S2GInvest",{1},{},{})
			--日充(礼包不算)
			dc_container:add_data_32(CellLogicType_ActionData, g_action_data_index.day_charge_count,price,true)
			player:OnLuaFunCall_x("xOnCharge",{ ["value"]=price , ["index"]=index })	
		end
	--lb不计入累计充值活动
	elseif config:group_type()=="acr_lb" then
		x002624_OnResolve(player,{ ["effect_value"]=id , ["index"]=index })
	elseif config:group_type()=="acr_tclb" then
		x002625_OnResolve(player,{ ["effect_value"]=id , ["index"]=index })
	elseif config:group_type()=="acr_zlb" then
		x002626_OnResolve(player,{ ["effect_value"]=id , ["index"]=index })
	elseif config:group_type()=="acr_ylb" then
		x002627_OnResolve(player,{ ["effect_value"]=id , ["index"]=index })
	elseif config:group_type()=="acr_xslb" then
		x002628_OnResolve(player,{ ["effect_value"]=id , ["index"]=index })
	elseif config:group_type()=="acr_zmlb" then
		x002628_OnResolve(player,{ ["effect_value"]=id , ["index"]=index })
	else
		--nothing
	end
	
	player:OnChargeOk(productid,ip,orderid,price,count,charge_times,extra_count)
	
	LuaServer():LuaLog("xOnChangeOk ! index="..index.." channel_id="..channel_id.." productid="..productid.." orderid="..orderid.." Guid="..player:Guid(),g_log_level.LL_INFO)
	
	return ResultCode_ResultOK
end
