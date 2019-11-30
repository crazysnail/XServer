
---------------------------------------------------------------------
local S2G_msg_common={}
S2G_msg_common["S2GDeletePlayer"]= function(sever, packet )	
	local guid = packet:int64_params(S2GDeletePlayer.data_64.guid)
	LuaServer():GmDeletePlayer(guid)
	return ResultCode_ResultOK
end
S2G_msg_common["S2GKickAll"]= function(sever, packet )	
	local guid = packet:int64_params(S2GKickAll.data_64.guid)
	LuaServer():KickAll(guid)
	return ResultCode_ResultOK
end
S2G_msg_common["S2GShutdown"]= function(sever, packet )	
	LuaServer():OnShutdown()
	return ResultCode_ResultOK
end
S2G_msg_common["S2GDayTriger"]= function(sever, packet )	
	local now_day = packet:int32_params(S2GDayTriger.data_32.day)
	LuaServer():OnDayTriger(now_day)
	return ResultCode_ResultOK
end

S2G_msg_common["S2GWeekTriger"]= function(sever, packet )	
	local week = packet:int32_params(S2GWeekTriger.data_32.week)
	LuaServer():OnWeekTriger(week)
	return ResultCode_ResultOK
end

S2G_msg_common["S2GMonthTriger"]= function(sever, packet )	
	local week = packet:int32_params(S2GMonthTriger.data_32.month)
	LuaServer():OnMonthTriger(month)
	return ResultCode_ResultOK
end


S2G_msg_common["S2GServerLevel"]= function(sever, packet )	
	LuaServer():EnableServerLevel()
	return ResultCode_ResultOK
end


S2G_msg_common["S2GLuaFilesUpdate"]= function(sever, packet )
	luafile_manager():on_update_luafile()
	LuaServer():BroadcastG2SCommonMessageToPlayer("G2SLuaFilesUpdate",{},{},{})
	return ResultCode_ResultOK
end

S2G_msg_common["S2GQuitBattleField"]= function(sever, packet )	
	local guid = packet:int64_params(S2GQuitBattleField.data_64.guid)
	local player = LuaServer():FindPlayer(guid)
	LuaBattleFieldManager():LeaveBattleField(player)
	return ResultCode_ResultOK
end

S2G_msg_common["S2GEnterBattleField"]= function(sever, packet )	
	local guid = packet:int64_params(S2GEnterBattleField.data_64.guid)
	local player = LuaServer():FindPlayer(guid)
	LuaBattleFieldManager():EnterBattleField(player)
	return ResultCode_ResultOK
end

--------------------------
S2G_msg_common["S2GInvest"]= function(sever, packet )	
	local count = packet:int32_params(S2GInvest.data_32.count)
	local old = LuaServer():GetInvestCount()
	LuaServer():SetInvestCount(old+count)
	LuaServer():BroadcastG2SCommonMessageToPlayer("G2SInvest",{},{},{})
	
	return ResultCode_ResultOK
end

S2G_msg_common["S2GAllPower"]= function(sever, packet )	

	local index = packet:int32_params(S2GAllPower.data_32.index)
	local count = packet:int32_params(S2GAllPower.data_32.count)
	if index < 1 or index >7 then 
		return ResultCode_Invalid_Request
	end
	
	local server_info = LuaServer():GetServerInfo()
	local offset = acr_dynamic_config[2635][index]
	local old_count = server_info:acr_data(offset)
	local new_count = old_count+count
	server_info:set_acr_data(offset,new_count)
	
	LuaServer():BroadcastG2SCommonMessageToPlayer("G2SPowerAcrRefresh",{new_count,index},{},{})
	
	return ResultCode_ResultOK
end

S2G_msg_common["S2GAllLevel"]= function(sever, packet )	

	local index = packet:int32_params(S2GAllLevel.data_32.index)
	local count = packet:int32_params(S2GAllLevel.data_32.count)
	if index < 1 or index >7 then 
		return ResultCode_Invalid_Request
	end

	local server_info = LuaServer():GetServerInfo()
	local offset = acr_dynamic_config[2636][index]
	local old_count = server_info:acr_data(offset)
	local new_count = old_count+count
	server_info:set_acr_data(offset,new_count)

	LuaServer():BroadcastG2SCommonMessageToPlayer("G2SLevelAcrRefresh",{new_count,index},{},{})
	
	return ResultCode_ResultOK
end

S2G_msg_common["S2GOpenDate"]= function(sever, packet )	
	local num = tonumber(packet:int32_params(S2GOpenDate.data_32.num))
	if num == -1 then
		LuaMtConfig():reset_open_server_date()
	else
		print("S2GOpenDate"..num)
		LuaMtConfig():set_open_server_date(num)
	end	
	return ResultCode_ResultOK
end


--全服战力比拼
S2G_msg_common["S2GPowerAcrCheck"]= function(sever, packet )	
	local index = packet:int32_params(S2GPowerAcrCheck.data_32.index)
	local limit_count = packet:int32_params(S2GPowerAcrCheck.data_32.limit_count)
	local guid = packet:int64_params(S2GPowerAcrCheck.data_64.guid)
	
	if index <1 or index > table.nums(acr_dynamic_config[2635]) then
		LuaServer():LuaLog("S2GPowerAcrCheck failed! guid="..guid,g_log_level.LL_ERROR)
		return  ResultCode_Invalid_Request
	end
	local player = LuaServer():FindPlayer(guid)
	if player == nil then
		LuaServer():LuaLog("S2GPowerAcrCheck failed! guid="..guid,g_log_level.LL_ERROR)
		return  ResultCode_Invalid_Request
	end
	local offset = acr_dynamic_config[2635][index]
	
	local server_info = LuaServer():GetServerInfo()
	local count = server_info:acr_data(offset)
	if count< limit_count then
	--还有名额
		server_info:set_acr_data(offset,count+1)
		LuaServer():SendG2SCommonMessage(player,"G2SPowerAcrCheck",{1,index},{},{})
		LuaServer():BroadcastG2SCommonMessageToPlayer("G2SPowerAcrRefresh",{count+1,index},{},{})
	else
	--失败
		LuaServer():SendG2SCommonMessage(player,"G2SPowerAcrCheck",{-1,index},{},{})
	end
	
	return ResultCode_ResultOK
end

--全服等级比拼
S2G_msg_common["S2GLevelAcrCheck"]= function(sever, packet )	
	local index = packet:int32_params(S2GLevelAcrCheck.data_32.index)
	local limit_count = packet:int32_params(S2GLevelAcrCheck.data_32.limit_count)
	local guid = packet:int64_params(S2GLevelAcrCheck.data_64.guid)
	local server_info = LuaServer():GetServerInfo()
	
	if index <1 or index > table.nums(acr_dynamic_config[2636]) then
		LuaServer():LuaLog("S2GLevelAcrCheck failed! guid="..guid,g_log_level.LL_ERROR)
		return  ResultCode_Invalid_Request
	end
	local player = LuaServer():FindPlayer(guid)
	if player == nil then
		LuaServer():LuaLog("S2GLevelAcrCheck failed! guid="..guid,g_log_level.LL_ERROR)
		return  ResultCode_Invalid_Request
	end
	local offset = acr_dynamic_config[2636][index]
	local count = server_info:acr_data(offset)
	if count< limit_count then
	--还有名额
		server_info:set_acr_data(offset,count+1)
		LuaServer():SendG2SCommonMessage(player,"G2SLevelAcrCheck",{1,index},{},{})
		LuaServer():BroadcastG2SCommonMessageToPlayer("G2SLevelAcrRefresh",{count+1,index},{},{})
	else
	--失败
		LuaServer():SendG2SCommonMessage(player,"G2SLevelAcrCheck",{-1,index},{},{})
	end
	
	return ResultCode_ResultOK
end

--团购
S2G_msg_common["S2GAllBuyAcrJoin"]= function(sever, packet )	
	local index = packet:int32_params(S2GAllBuyAcrJoin.data_32.index)
	local guid = packet:int64_params(S2GAllBuyAcrJoin.data_64.guid)
	local player = LuaServer():FindPlayer(guid)
	if player == nil then
		LuaServer():LuaLog("S2GAllBuyAcrJoin failed! guid="..guid,g_log_level.LL_ERROR)
		return  ResultCode_Invalid_Request
	end
	local server_info = LuaServer():GetServerInfo() 	
	local count = server_info:acr_data(g_acr_server_index.all_buy_count)
	server_info:set_acr_data(g_acr_server_index.all_buy_count,count+1)
	
	LuaServer():SendG2SCommonMessage(player,"G2SAllBuyAcrJoin",{1,index},{},{})
	
	return ResultCode_ResultOK
end


--秒杀
S2G_msg_common["S2GQuikBuyAcrJoin"]= function(sever, packet )	

	local index = packet:int32_params(S2GQuikBuyAcrJoin.data_32.index)
	local guid = packet:int64_params(S2GQuikBuyAcrJoin.data_64.guid)
	local player = LuaServer():FindPlayer(guid)
	if player == nil then
		LuaServer():LuaLog("S2GQuikBuyAcrJoin failed! guid="..guid,g_log_level.LL_ERROR)
		return  ResultCode_Invalid_Request
	end
	local server_info = LuaServer():GetServerInfo() 	
	if index ~= server_info:acr_data(g_acr_server_index.quik_buy_index) then
		player:SetParams32("quik_buy",-1)
		LuaServer():LuaLog("S2GQuikBuyAcrJoin failed! guid="..guid,g_log_level.LL_ERROR)
		return  ResultCode_Invalid_Request
	end
	local count = server_info:acr_data(g_acr_server_index.quik_buy_count) 
	if count >0 then
		local last_count = count-1
		server_info:set_acr_data(g_acr_server_index.quik_buy_count,last_count) --先占坑，最坏的情况就是占了坑，没买成
		LuaServer():SendG2SCommonMessage(player,"G2SQuikBuyAcrJoin",{1,index},{},{})
		if last_count <=0 then
			x002638_G_CheckEnd({["index"]=index})
		end
		LuaServer():BroadcastG2SCommonMessageToPlayer("G2SQuikBuyAcrRefresh",{last_count},{},{})
	else
		LuaServer():SendG2SCommonMessage(player,"G2SQuikBuyAcrJoin",{-1,index},{},{})
	end
	
	return ResultCode_ResultOK
end

	
S2G_msg_common["S2GTbReload"]= function(sever, packet )	
	
	local name = packet:string_params(S2GTbReload.data_string.name)
	if name == "item" then
		LuaMtItemPackageManager():LoadPackageTB(true)
		LuaItemManager():LoadItemTB(true)
		LuaItemManager():LoadGemTB(true)		
	elseif  name == "equip" then
		LuaMtItemPackageManager():LoadPackageTB(true)
		LuaItemManager():LoadEquipTB(true)
		LuaItemManager():LoadBookTB(true)
	elseif  name == "simpact" then
		LuaItemManager():LoadSimpleImpacktTB(true)
	elseif  name == "draw" then
		LuaMtDrawManager():LoadDrawTB(true)
	elseif  name == "answer" then
		LuaActivityManager():LoadAnswerTB(true)
	elseif  name == "activity" then
		LuaActivityManager():LoadActivityTB(true)
		LuaServer():BroadcastG2SCommonMessageToPlayer("G2SActivityRefresh",{},{},{})
	elseif  name == "market" then
		LuaMtMarketManager():LoadMarketTB(true)
		LuaMtMarketManager():LoadMarketItemTB(true)
	elseif  name == "const_config" then
		LuaMtConfigDataManager():LoadConstConfigTB(true)
	elseif name == "charge" then
		LuaMtMarketManager():LoadChargeStoreTB(true)
		LuaMtMarketManager():LoadChargeListTB(true)
	elseif  name == "scenelayout" then	--有问题，仅限测试用！！！！
		LuaMtMonsterManager():LoadSceneLayoutTB(true)
	elseif  name == "monster" then	
		LuaMtMonsterManager():LoadMonsterConfigTB(true)
		LuaMtMonsterManager():LoadMonsterGroupTB(true)
	elseif  name == "function" then	
		LuaMtConfigDataManager():LoadFunctionConfigTB(true)
	else
		return ResultCode_Invalid_Request
	end	
	return ResultCode_ResultOK
end


S2G_msg_common["S2GSwitchMarket"]= function(sever, packet )	
	local id = packet:int32_params(S2GSwitchMarket.data_32.id)
	local open = packet:int32_params(S2GSwitchMarket.data_32.open)
	LuaServer():BroadcastG2SCommonMessageToPlayer("G2SSwitchMarket",{id,open},{},{})
	return ResultCode_ResultOK
end


S2G_msg_common["S2GSwitchMarketItem"]= function(sever, packet )	
	local id = packet:int32_params(S2GSwitchMarketItem.data_32.id)
	local index = packet:int32_params(S2GSwitchMarketItem.data_32.index)
	local open = packet:int32_params(S2GSwitchMarketItem.data_32.open)
	LuaServer():BroadcastG2SCommonMessageToPlayer("G2SSwitchMarketItem",{id,index,open},{},{})
	return ResultCode_ResultOK
end

S2G_msg_common["S2GAcrRefresh"]= function(sever, packet )	
	LuaServer():BroadcastG2SCommonMessageToPlayer("G2SAcrRefresh",{},{},{})
	return ResultCode_ResultOK
end

S2G_msg_common["S2GUpdateOperate"]= function(sever, packet )	
	--sever:GetLuaCallProxy():OnLuaFunCall_n_Server(server, 5, "OnUpdateAcrConfig",{["script_id"]=script_id})
	LuaMtConfig():EnableActive(true)
	return ResultCode_ResultOK
end

S2G_msg_common["S2GFlushDelay"]= function(sever, packet )		
	LuaOperativeConfigManager():FlushCacheConfig()
	return ResultCode_ResultOK
end


--S2G_msg_common["S2GLogout"]= function(sever, packet )
--	local guid = packet:int64_params(S2GLogout.data_64.guid)
--	LuaServer():MarkPlayerLogout(guid)
--	return ResultCode_ResultOK
--end

S2G_msg_common["S2GCacheOk"]= function(sever, packet )	
	local guid = packet:int64_params(S2GCacheOk.data_64.guid)
	LuaServer():MarkPlayerCached(guid)
	return ResultCode_ResultOK
end

S2G_msg_common["S2GLeaveTeam"]= function(sever, packet )	
	local guid = packet:int64_params(S2GLeaveTeam.data_64.guid)
	local player = LuaServer():FindPlayer(guid)
	if player == nil then
		return ResultCode_ResultOK
	end
	LuaMtTeamManager():PlayerLeaveTeam(player);
	return ResultCode_ResultOK
end

S2G_msg_common["S2GLeaveTeamAndScene"]= function(sever, packet )	
	local guid = packet:int64_params(S2GLeaveTeamAndScene.data_64.guid)
	local player = LuaServer():FindPlayer(guid)
	if player == nil then
		return ResultCode_ResultOK
	end
	LuaMtTeamManager():PlayerLeaveTeam(player);
	LuaServer():SendG2SCommonMessage(player,"G2SGoingLeaveScene",{},{},{})
	return ResultCode_ResultOK
end

S2G_msg_common["S2GSyncTeamMemberLevel"]= function(sever, packet )
	local playerguid = packet:int64_params(S2GSyncTeamMemberLevel.data_64.playerguid)
	local teamid = packet:int64_params(S2GSyncTeamMemberLevel.data_64.teamid)
	local playerlevel = packet:int32_params(S2GSyncTeamMemberLevel.data_32.playerlevel)
	local player = LuaServer():FindPlayer(playerguid)
	if player == nil then
		return ResultCode_ResultOK
	end
	local teaminfo = LuaMtTeamManager():GetTeamInfo(teamid);
	if teaminfo == nil then
		LuaServer():LuaLog("S2GSyncTeamMemberLevel guild nil playerid:"..playerguid..",teamid:"..teamid,g_log_level.LL_ERROR)
		LuaMtTeamManager():ResetPlayerTeamDataToPlayer(player);
		return ResultCode_ResultOK
	end
	LuaMtTeamManager():S2GSyncTeamMemberLevel(teaminfo,playerguid,playerlevel);
	return ResultCode_ResultOK
end

S2G_msg_common["S2GCreateGuildReq"]= function(sever, packet )
	local playerguid = packet:int64_params(S2GCreateGuildReq.data_64.playerguid)
	local createname = packet:string_params(S2GCreateGuildReq.data_string.createname)
	local createnotice = packet:string_params(S2GCreateGuildReq.data_string.createnotice)
	local player = LuaServer():FindPlayer(playerguid)
	if player == nil then
		LuaServer():LuaLog("S2GCreateGuildReq player nil createname:"..createname..",createnotice:"..createnotice,g_log_level.LL_ERROR)
		return ResultCode_ResultOK
	end
	
	LuaMtGuildManager():CreateGuild(player,createname,createnotice);
	return ResultCode_ResultOK
end

S2G_msg_common["S2GLoginSyncGuildUserData"]= function(sever, packet )
	local playerguid = packet:int64_params(S2GLoginSyncGuildUserData.data_64.playerguid)
	local guildid = packet:int64_params(S2GLoginSyncGuildUserData.data_64.guildid)
	local playername = packet:string_params(S2GLoginSyncGuildUserData.data_string.playername)
	local playerlevel = packet:int32_params(S2GLoginSyncGuildUserData.data_32.playerlevel)
	local player = LuaServer():FindPlayer(playerguid)
	if player == nil then
		return ResultCode_ResultOK
	end
	local guild = LuaMtGuildManager():GetGuildByid(guildid);
	if guild == nil then
		LuaServer():LuaLog("S2GLoginSyncGuildUserData guild nil playerid:"..playerguid..",guildid:"..guildid,g_log_level.LL_ERROR)
		LuaServer():SendG2SCommonMessage(player,"G2SUpdateGuildInfo",{},{INVALID_GUID},{"","S2GLoginGuildNil"})
		return ResultCode_ResultOK
	end
	local guilduser = guild:GetGuildUser(playerguid);
	if guilduser == nullptr then
		LuaServer():LuaLog("SyncGuildUserData user nil playerguid:"..playerguid..",guildid:"..guildid,g_log_level.LL_ERROR)
		LuaServer():SendG2SCommonMessage(player,"G2SUpdateGuildInfo",{},{INVALID_GUID},{"","S2GLoginUserNil"})
		return ResultCode_ResultOK;
	end
	if guild:ChieftainGuid() == playerguid and guild:GetDBInfo():chieftainname() ~= playername then
		guild:GetDBInfo():set_chieftainname(playername);
	end
	guilduser:set_playerlevel(playerlevel);
	guilduser:set_playername(playername);
	guilduser:set_online(1);
	guilduser:set_lastlogouttime(LuaMtTimerManager():CurrentDate());
		
	guild:G2SSyncGuildUserData(player, guilduser);
	
	LuaMtGuildManager():OnGetGuildInfo(player);
	return ResultCode_ResultOK
end

S2G_msg_common["S2GLoginOutSyncGuildUserData"]= function(sever, packet )
	local playerguid = packet:int64_params(S2GLoginSyncGuildUserData.data_64.playerguid)
	local guildid = packet:int64_params(S2GLoginSyncGuildUserData.data_64.guildid)
	local player = LuaServer():FindPlayer(playerguid)
	if player == nil then
		return ResultCode_ResultOK
	end
	local guild = LuaMtGuildManager():GetGuildByid(guildid);
	if guild == nil then
		LuaServer():LuaLog("S2GLoginSyncGuildUserData guild nil playerid:"..playerguid..",guildid:"..guildid,g_log_level.LL_ERROR)
		LuaServer():SendG2SCommonMessage(player,"G2SUpdateGuildInfo",{},{INVALID_GUID},{"","S2GLoginOutGuildNil"})
		return ResultCode_ResultOK
	end
	local guilduser = guild:GetGuildUser(playerguid);
	if guilduser == nullptr then
		LuaServer():LuaLog("SyncGuildUserData user nil playerguid:"..playerguid..",guildid:"..guildid,g_log_level.LL_ERROR)
		LuaServer():SendG2SCommonMessage(player,"G2SUpdateGuildInfo",{},{INVALID_GUID},{"","S2GLoginOutUserNil"})
		return ResultCode_ResultOK;
	end
	guilduser:set_online(0);
	guilduser:set_lastlogouttime(LuaMtTimerManager():CurrentDate());
	return ResultCode_ResultOK
end

S2G_msg_common["S2GSyncGuildUserName"]= function(sever, packet )
	local playerguid = packet:int64_params(S2GSyncGuildUserName.data_64.playerguid)
	local guildid = packet:int64_params(S2GSyncGuildUserName.data_64.guildid)
	local playername = packet:string_params(S2GSyncGuildUserName.data_string.playername)
	local player = LuaServer():FindPlayer(playerguid)
	if player == nil then
		return ResultCode_ResultOK
	end
	local guild = LuaMtGuildManager():GetGuildByid(guildid);
	if guild == nil then
		LuaServer():LuaLog("S2GSyncGuildUserName guild nil playerid:"..playerguid..",guildid:"..guildid,g_log_level.LL_ERROR)
		LuaServer():SendG2SCommonMessage(player,"G2SUpdateGuildInfo",{},{INVALID_GUID},{"","S2GNameGuildNil"})
		return ResultCode_ResultOK
	end
	local guilduser = guild:GetGuildUser(playerguid);
	if guilduser == nullptr then
		LuaServer():LuaLog("S2GSyncGuildUserName user nil playerguid:"..playerguid..",guildid:"..guildid,g_log_level.LL_ERROR)
		LuaServer():SendG2SCommonMessage(player,"G2SUpdateGuildInfo",{},{INVALID_GUID},{"","S2GNameUserNil"})
		return ResultCode_ResultOK;
	end
	guilduser:set_playername(playername);
	return ResultCode_ResultOK
end

S2G_msg_common["S2GSyncGuildUserLevel"]= function(sever, packet )
	local playerguid = packet:int64_params(S2GSyncGuildUserLevel.data_64.playerguid)
	local guildid = packet:int64_params(S2GSyncGuildUserLevel.data_64.guildid)
	local playerlevel = packet:int32_params(S2GSyncGuildUserLevel.data_32.playerlevel)
	local player = LuaServer():FindPlayer(playerguid)
	if player == nil then
		return ResultCode_ResultOK
	end
	local guild = LuaMtGuildManager():GetGuildByid(guildid);
	if guild == nil then
		LuaServer():LuaLog("S2GSyncGuildUserLevel guild nil playerid:"..playerguid..",guildid:"..guildid,g_log_level.LL_ERROR)
		LuaServer():SendG2SCommonMessage(player,"G2SUpdateGuildInfo",{},{INVALID_GUID},{"","S2GLevelGuildNil"})
		return ResultCode_ResultOK
	end
	local guilduser = guild:GetGuildUser(playerguid);
	if guilduser == nullptr then
		LuaServer():LuaLog("S2GSyncGuildUserLevel user nil playerguid:"..playerguid..",guildid:"..guildid,g_log_level.LL_ERROR)
		LuaServer():SendG2SCommonMessage(player,"G2SUpdateGuildInfo",{},{INVALID_GUID},{"","S2GLevelUserNil"})
		return ResultCode_ResultOK;
	end
	guilduser:set_playerlevel(playerlevel);
	return ResultCode_ResultOK
end

S2G_msg_common["S2GSyncGuildUserCurcontribution"]= function(sever, packet )
	local playerguid = packet:int64_params(S2GSyncGuildUserCurcontribution.data_64.playerguid)
	local guildid = packet:int64_params(S2GSyncGuildUserCurcontribution.data_64.guildid)
	local curcontribution = packet:int32_params(S2GSyncGuildUserCurcontribution.data_32.curcontribution)
	local player = LuaServer():FindPlayer(playerguid)
	if player == nil then
		return ResultCode_ResultOK
	end
	local guild = LuaMtGuildManager():GetGuildByid(guildid);
	if guild == nil then
		LuaServer():LuaLog("S2GSyncGuildUserCurcontribution guild nil playerid:"..playerguid..",guildid:"..guildid,g_log_level.LL_ERROR)
		LuaServer():SendG2SCommonMessage(player,"G2SUpdateGuildInfo",{},{INVALID_GUID},{"","S2GCurcontriGuildNil"})
		return ResultCode_ResultOK
	end
	local guilduser = guild:GetGuildUser(playerguid);
	if guilduser == nullptr then
		LuaServer():LuaLog("S2GSyncGuildUserCurcontribution user nil playerguid:"..playerguid..",guildid:"..guildid,g_log_level.LL_ERROR)
		LuaServer():SendG2SCommonMessage(player,"G2SUpdateGuildInfo",{},{INVALID_GUID},{"","S2GCurcontriUserNil"})
		return ResultCode_ResultOK;
	end
	guilduser:set_curcontribution(curcontribution);
	return ResultCode_ResultOK
end

S2G_msg_common["S2GSyncGuildUserWeekcontribution"]= function(sever, packet )
	local playerguid = packet:int64_params(S2GSyncGuildUserWeekcontribution.data_64.playerguid)
	local guildid = packet:int64_params(S2GSyncGuildUserWeekcontribution.data_64.guildid)
	local weekcontribution = packet:int32_params(S2GSyncGuildUserWeekcontribution.data_32.weekcontribution)
	local player = LuaServer():FindPlayer(playerguid)
	if player == nil then
		return ResultCode_ResultOK
	end
	local guild = LuaMtGuildManager():GetGuildByid(guildid);
	if guild == nil then
		LuaServer():LuaLog("S2GSyncGuildUserWeekcontribution guild nil playerid:"..playerguid..",guildid:"..guildid,g_log_level.LL_ERROR)
		LuaServer():SendG2SCommonMessage(player,"G2SUpdateGuildInfo",{},{INVALID_GUID},{"","S2GWeekCurcontriGuildNil"})
		return ResultCode_ResultOK
	end
	local guilduser = guild:GetGuildUser(playerguid);
	if guilduser == nullptr then
		LuaServer():LuaLog("S2GSyncGuildUserWeekcontribution user nil playerguid:"..playerguid..",guildid:"..guildid,g_log_level.LL_ERROR)
		LuaServer():SendG2SCommonMessage(player,"G2SUpdateGuildInfo",{},{INVALID_GUID},{"","S2GWeekCurcontriUserNil"})
		return ResultCode_ResultOK;
	end
	guilduser:set_weekcontribution(weekcontribution);
	return ResultCode_ResultOK
end

S2G_msg_common["S2GSyncGuildUserAllcontribution"]= function(sever, packet )
	local playerguid = packet:int64_params(S2GSyncGuildUserAllcontribution.data_64.playerguid)
	local guildid = packet:int64_params(S2GSyncGuildUserAllcontribution.data_64.guildid)
	local allcontribution = packet:int32_params(S2GSyncGuildUserAllcontribution.data_32.allcontribution)
	local player = LuaServer():FindPlayer(playerguid)
	if player == nil then
		return ResultCode_ResultOK
	end
	local guild = LuaMtGuildManager():GetGuildByid(guildid);
	if guild == nil then
		LuaServer():LuaLog("S2GSyncGuildUserAllcontribution guild nil playerid:"..playerguid..",guildid:"..guildid,g_log_level.LL_ERROR)
		LuaServer():SendG2SCommonMessage(player,"G2SUpdateGuildInfo",{},{INVALID_GUID},{"","S2GAllCurcontriGuildNil"})
		return ResultCode_ResultOK
	end
	local guilduser = guild:GetGuildUser(playerguid);
	if guilduser == nullptr then
		LuaServer():LuaLog("S2GSyncGuildUserAllcontribution user nil playerguid:"..playerguid..",guildid:"..guildid,g_log_level.LL_ERROR)
		LuaServer():SendG2SCommonMessage(player,"G2SUpdateGuildInfo",{},{INVALID_GUID},{"","S2GAllCurcontriUserNil"})
		return ResultCode_ResultOK;
	end
	guilduser:set_allcontribution(allcontribution);
	return ResultCode_ResultOK
end

S2G_msg_common["S2GAddGuildBonus"]= function(sever, packet )
	local guildid = packet:int64_params(S2GAddGuildBonus.data_64.guildid)
	local add_count = packet:int32_params(S2GAddGuildBonus.data_32.add_count)
	local guild = LuaMtGuildManager():GetGuildByid(guildid);
	if guild == nil then
		LuaServer():LuaLog("S2GAddGuildBonus guild nil guildid:"..guildid,g_log_level.LL_ERROR)
		return ResultCode_ResultOK
	end
	guild:AddGuildWeekBonus(add_count);
	return ResultCode_ResultOK
end

S2G_msg_common["S2GAddGuildBattleTimes"]= function(sever, packet )
	local guildid = packet:int64_params(S2GAddGuildBattleTimes.data_64.guildid)
	local playerid = packet:int64_params(S2GAddGuildBattleTimes.data_64.playerid)
	local guilduser = LuaMtGuildManager():GetGuildUser(guildid,playerid);
	if guilduser == nil then
		LuaServer():LuaLog("S2GAddGuildBattleTimes guilduser nil guildid:"..guildid.." playerid:"..playerid,g_log_level.LL_ERROR)
		return ResultCode_ResultOK
	end
	local battletimes = guilduser:battletime();
	battletimes = battletimes +1 ;
	guilduser:set_battletime(battletimes);
	return ResultCode_ResultOK
end


S2G_msg_common["S2GUpdateTowerInfo"]= function(sever, packet )

	local guid 	= packet:int64_params(S2GUpdateTowerInfo.data_64.guid)
	local level = packet:int32_params(S2GUpdateTowerInfo.data_32.level)
	local time  = packet:int32_params(S2GUpdateTowerInfo.data_32.time)
	
	LuaMtTowerManager():UpdatePlayerTowerInfo(guid,level,time)
	
	return ResultCode_ResultOK
	
end

S2G_msg_common["S2GGuildBattleWin"]= function(sever, packet )
	local guid = packet:int64_params(S2GGuildBattleWin.data_64.guid)
	local win  = packet:int32_params(S2GGuildBattleWin.data_32.win)
	local score  = packet:int32_params(S2GGuildBattleWin.data_32.score)
	local playercount  = packet:int32_params(S2GGuildBattleWin.data_32.playercount)
	local allplayercount  = packet:int32_params(S2GGuildBattleWin.data_32.allplayercount)
	local guild = LuaMtGuildManager():GetGuildByid(guid);
	if guild == nil then
		LuaServer():LuaLog("S2GGuildBattleWin guild null,"..guid..","..win..","..score..","..playercount..","..allplayercount,g_log_level.LL_ERROR)
		return ResultCode_ResultOK
	end
	local buildlevel = guild:GetBuildLevel(GuildBuildType_GUILD_MAIN);
	local levelconfig = LuaMtGuildManager():GetGuildLevelFixConfig(buildlevel);
	if levelconfig == nil then
		LuaServer():LuaLog("S2GGuildBattleWin levelconfig null,"..guid..","..win..","..score..","..playercount..","..allplayercount..","..buildlevel,g_log_level.LL_ERROR)
		return ResultCode_ResultOK
	end
	local guildname = guild:GetDBInfo():name();
	local bt = guild:GetDBInfo():battletimes();
	bt = bt+1;
	guild:GetDBInfo():set_battletimes(bt);
	local bscore = guild:GetDBInfo():battlescore();
	bscore = bscore+score;
	local bwt = guild:GetDBInfo():battlewintimes();
	if win == 1 then
		bwt=bwt+1;
		guild:GetDBInfo():set_battlewintimes(bwt);
		local money = levelconfig:battle()*playercount + levelconfig:battle()*allplayercount;
		guild:AddGuildMoney_S(money);
		lua_send_guild_system_message("GuildBattle_Final_Win",guid);
		guild:AddRecord("GuildBattle_Final_Win");
	else
		local money = levelconfig:battle()*playercount;
		guild:AddGuildMoney_S(money);
		lua_send_guild_system_message("GuildBattle_Final_Lost",guid);
		guild:AddRecord("GuildBattle_Final_Lost");
	end
	guild:GetDBInfo():set_battlescore(bscore);
	LuaMtGuildManager():SetGuildBattleWeekRank(guid,guildname,buildlevel,bscore);
	LuaMtGuildManager():SetGuildBattleAllRank(guid,guildname,buildlevel,bwt,bt);
	LuaServer():LuaLog("S2GGuildBattleWin end,"..guid..","..win..","..score..","..playercount..","..allplayercount..","..buildlevel,g_log_level.LL_INFO)
	return ResultCode_ResultOK
end

S2G_msg_common["S2GGuildBattleRoundReward"]= function(sever, packet )	--每轮的第5/6/7/8周的周四公会战结束 发放奖励
	LuaMtGuildManager():GuildBattleRoundReward();
	return ResultCode_ResultOK
end

S2G_msg_common["S2GClearGuildBattleRoundScore"]= function(sever, packet )	--清空本轮帮会战绩
	LuaMtGuildManager():ClearGuildBattleRoundScore();
	return ResultCode_ResultOK
end

S2G_msg_common["S2GClearGuildBattleScore"]= function(sever, packet )	--清空帮会战绩
	LuaMtGuildManager():ClearGuildBattleScore();
	return ResultCode_ResultOK
end

S2G_msg_common["S2GFightGuildWetCopy"]= function(sever, packet )	
	local playerguid = packet:int64_params(S2GFightGuildWetCopy.data_64.playerguid)
	local player = LuaServer():FindPlayer(playerguid)
	if player == nil then
		return ResultCode_ResultOK
	end
	local guild = LuaMtGuildManager():GetGuildByid(player:GetGuildID());
	if guild == nil then
		return ResultCode_ResultOK
	end
	local copyid = packet:int32_params(S2GFightGuildWetCopy.data_32.copyid)
	local guildwetcopy = guild:GetGuildWetCopyInfo(copyid);
	if guildwetcopy == nil then
		return ResultCode_ResultOK
	end
	if guildwetcopy:status() == false then
		return player:SendClientNotify("wetcopyfinsh",-1,-1)
	end
	local fightname = guildwetcopy:fightname();
	if fightname ~= "" then
		player:SendClientNotify("gonghuifubenfighting|"..fightname,-1,-1)
		return ResultCode_ResultOK
	end
	local intparms = {copyid,guildwetcopy:stageid()}
	local bosshp = 0;
	for i=0,guildwetcopy:bosshp_size()-1,1 do
		bosshp = bosshp + guildwetcopy:bosshp(i);
		table.insert(intparms,guildwetcopy:bosshp(i));
	end
	if bosshp <= 0 then
		return player:SendClientNotify("yizhenwang",-1,-1)
	end
	local times = guild:GetWetCopyTimes(player:Guid(),copyid)
	if times >= 2 then
		return player:SendClientNotify("ChallengeTimeLimit",-1,-1)
	end
	guildwetcopy:set_fightname(player:Name());
	guildwetcopy:set_fighterid(player:Guid());
	guild:SetGuildWetCDTime(copyid,300);
	LuaServer():SendG2SCommonMessage(player,"G2SFightGuildWetCopy",intparms,{},{})
	LuaServer():LuaLog("G2SFightGuildWetCopy "..copyid..","..player:Name()..","..player:Guid(),g_log_level.LL_INFO)
	return ResultCode_ResultOK
end


S2G_msg_common["S2GBfCreateBattleResult"]= function(sever, packet )	
	local isok = packet:int32_params(S2GBfCreateBattleResult.data_32.isok)
	local player_guid = packet:int64_params(S2GBfCreateBattleResult.data_64.player_guid)
	local target_guid = packet:int64_params(S2GBfCreateBattleResult.data_64.target_guid)
	
	local player = LuaServer():FindPlayer(player_guid)
	if player ~= nil then
		LuaBattleFieldManager():OnCreatBattleResult(player,target_guid,isok==1)
	end
	
	return ResultCode_ResultOK
end

S2G_msg_common["S2GEndGuildWetCopy"]= function(sever, packet )	
	local guid = packet:int64_params(S2GEndGuildWetCopy.data_64.guid)
	local player = LuaServer():FindPlayer(guid)
	if player == nil then
		return ResultCode_ResultOK
	end
	local guild = LuaMtGuildManager():GetGuildByid(player:GetGuildID());
	if guild == nil then
		return ResultCode_ResultOK
	end
	local copyid = packet:int32_params(S2GEndGuildWetCopy.data_32.copyid)
	local damage = packet:int32_params(S2GEndGuildWetCopy.data_32.damage)
	local guildwetcopy = guild:GetGuildWetCopyInfo(copyid);
	if guildwetcopy == nil then
		return ResultCode_ResultOK
	end
	guildwetcopy:set_fightname("");
	guildwetcopy:set_fighterid(INVALID_GUID);
	guild:SetGuildWetCDTime(copyid,0);
	LuaServer():LuaLog("S2GEndGuildWetCopy "..copyid..","..player:Guid(),g_log_level.LL_INFO)
	local lefthp = 0
	for i=2,packet:int32_params_size()-1 do
		local hp = packet:int32_params(i)
		if hp < 0 then
			hp = 0
		end
		guildwetcopy:set_bosshp(i-2,hp);
		lefthp = lefthp + hp
	end
	guild:AddWetCopytimes(player, copyid);
	local stageid = guildwetcopy:stageid();
	local guildwetstage = LuaMtGuildManager():GetGuildWetCopyStage(stageid);
	if guildwetstage == nil then
		return ResultCode_ResultOK
	end
	

	if lefthp <= 0 then
		local dropid = guildwetstage:drop();
		local droppackage = LuaMtItemPackageManager():GetPackage(dropid);
		if droppackage ~= nil then
			droppackage:AddToGuildSpoil(guild, player, copyid);
		end
		player:SendClientNotify("wetcopybossdead|"..player:Name().."|gw_name_"..math.ceil(copyid).."|monster_name_"..math.ceil(guildwetstage:boss_id()),ChatChannel_GUILD,-1)
		local prostageid = guildwetstage:pro_stage();
		if prostageid == -1 then --打完了
			guildwetcopy:set_status(false);
			local opentime = guildwetcopy:opentime();
			local diffday = LuaMtTimerManager():DiffDayToNow(opentime);
			if diffday < 7 then
				guild:AddGuildMoney(4000,player)
			end
		else	--下一关
			local proguildwetstage = LuaMtGuildManager():GetGuildWetCopyStage(prostageid);
			if proguildwetstage == nil then
				return ResultCode_ResultOK;
			end
			LuaMtGuildManager():SetGuildWetCopyStageMonster(guildwetcopy,proguildwetstage);
		end
	end
	
	LuaMtGuildManager():SendGuildWetCopy(player,guildwetcopy);
	LuaMtGuildManager():EndGuildWetCopy(player,guild,copyid,damage);
	return ResultCode_ResultOK
end


S2G_msg_common["LoadZoneInfo"]= function(sever, packet )	
	local guid = packet:int64_params(LoadZoneInfo.data_64.guid)
	local playerzoneinfo = LuaMtFriendManager():GetPlayerZone(guid);
	if playerzoneinfo == nil then
		LuaServer():AddLoadZoneInfoWork(guid);
	end
	return ResultCode_ResultOK
end

S2G_msg_common["S2GPlayerReConnected"]= function(sever, packet )	
	local guid = packet:int64_params(S2GPlayerReConnected.data_64.guid)
	local player = LuaServer():FindPlayer(guid);
	if player ~= nil then
		lua_static_message_manager():player_catch_mails_g(player);
		lua_static_message_manager():player_catch_worldchat_g(player);
	end
	return ResultCode_ResultOK
end


S2G_msg_common["S2GGMMinTick"]= function(sever, packet )	
	for i=0,100,1 do
		LuaServer():OnMinuteTick();
	end
	return ResultCode_ResultOK
end

------------------------------------------------------------------------------------
function x010002_HandleS2GCommonMessage( sever,packet )
	if packet:request_name() ~="S2GCacheOk" then
		LuaServer():LuaLog("HandleS2GCommonMessage"..packet:request_name(),g_log_level.LL_INFO)
	end
	return S2G_msg_common[packet:request_name()]( sever, packet )	
end
-------------------------------------client packet req-----------------------------------------------

local server_common_request={}
server_common_request["LeaveGuildReq"]= function(sever, player, packet )
	if player == nil then
		return ResultCode_ResultOK
	end
	local playerguid = player:Guid();
	local guild = LuaMtGuildManager():GetGuildByid(player:GetGuildID());
	if guild == nil then
		return ResultCode_ResultOK
	end
	local guilduser=guild:GetGuildUser(playerguid)
	if guilduser == nil then
		return ResultCode_ResultOK
	end
	
	LuaServer():SendG2SCommonMessage(player,"G2SUpdateGuildInfo",{},{INVALID_GUID},{"","LeaveGuildReq"})
			
	local str = "likaixx|"..guilduser:playername();
	player:SendClientNotify(str,ChatChannel_GUILD,-1)
	guild:AddRecord(str);
	guild:DeleteUser(playerguid)

	
	local battlesceneid = LuaMtConfigDataManager():FindConfigValue("guild_battle_common"):value1();
	if player:GetSceneId() == battlesceneid then --在帮会战里
		LuaMtTeamManager():PlayerLeaveTeam(player);
		LuaServer():SendG2SCommonMessage(player,"G2SGoingLeaveScene",{},{},{})
	end
	return ResultCode_ResultOK
end
server_common_request["FireGuildUserReq"]= function(sever, player, packet )
	local playerid = packet:int64_params(FireGuildUserReq.data_64.guid)
	local guild = LuaMtGuildManager():GetGuildByid(player:GetGuildID());
	if guild == nil then
		return ResultCode_ResultOK
	end
	local targetuser = guild:GetGuildUser(playerid);
	if targetuser == nil then
		return ResultCode_ResultOK
	end
	local targetguildposition = targetuser:position();
	local selfguildposition = LuaMtGuildManager():GetPlayerGuildPosition(player:Guid(),guild)
	if selfguildposition <= targetguildposition then
		player:SendClientNotify("quanxianbuzu", -1, -1);
		return ResultCode_ResultOK
	end
	local str = "zhuchuxx|"..player:Name().."|"..targetuser:playername();
	player:SendClientNotify(str,ChatChannel_GUILD,-1)
	guild:AddRecord(str);
	guild:DeleteUser(playerid);
	guild:SendGuildUserList(player);
	
	local targetplayer = LuaServer():FindPlayer(playerid)
	if targetplayer ~= nil then
	--LuaMtGuildManager():OnGetGuildCopyDamage(player,copyid)
		LuaServer():SendG2SCommonMessage(targetplayer,"G2SUpdateGuildInfo",{},{INVALID_GUID},{"","FireGuildUserReq"})
		local battlesceneid = LuaMtConfigDataManager():FindConfigValue("guild_battle_common"):value1();
		if targetplayer:GetSceneId() == battlesceneid then --在帮会战里
			LuaMtTeamManager():PlayerLeaveTeam(targetplayer);
			LuaServer():SendG2SCommonMessage(targetplayer,"G2SGoingLeaveScene",{},{},{})
		end
	end
	return ResultCode_ResultOK
end

server_common_request["SetGuildUserPositionReq"]= function(sever, player, packet )
	local targetplayerid = packet:int64_params(SetGuildUserPositionReq.data_64.guid)
	local position = packet:int32_params(SetGuildUserPositionReq.data_32.position)
	local guild = LuaMtGuildManager():GetGuildByid(player:GetGuildID());
	if guild == nil then
		return ResultCode_ResultOK
	end
	local selfguildposition = LuaMtGuildManager():GetPlayerGuildPosition(player:Guid(),guild)
	if selfguildposition < position then
		player:SendClientNotify("quanxianbuzu", -1, -1);
		return ResultCode_ResultOK
	end
	local guilduser = guild:GetGuildUser(targetplayerid);
	if guilduser == nil then
		return ResultCode_ResultOK
	end	
	local oldposition = guilduser:position();
	if position >= GuildPosition_FOUR_LEADER then
		local positionuser = guild:GetGuildUserByPosition(position);
		if positionuser ~= nil then
			player:SendClientNotify("positionhavepeople", -1, -1);
			return ResultCode_ResultOK
		end
	end
	if player:Guid() ~= targetplayerid then
		if oldposition >= selfguildposition then
			player:SendClientNotify("quanxianbuzu", -1, -1);
			return ResultCode_ResultOK
		end
	end
	guilduser:set_position(position);
	LuaMtGuildManager():SendGuildUserPosition(player,targetplayerid,position);
	local targetplayer = LuaServer():FindPlayer(targetplayerid);
	if targetplayer ~= nil then
		LuaServer():SendG2SCommonMessage(targetplayer,"G2SGuildUserPosition",{position},{},{})
	end
	
	local str=""
	if position == GuildPosition_TRAINEE then
		if player:Guid() == targetplayerid then
			str = "cituixx|"..player:Name().."|GuildPosition_"..tostring(math.floor(oldposition));
			player:SendClientNotify(str,ChatChannel_GUILD,-1)
		else
			str = "mianzhixx|"..player:Name().."|"..guilduser:playername().."|GuildPosition_"..tostring(math.floor(oldposition));
			player:SendClientNotify(str,ChatChannel_GUILD,-1)
		end
	else
		str = "renmingxx|"..player:Name().."|"..guilduser:playername().."|GuildPosition_"..tostring(math.floor(position));
		player:SendClientNotify(str,ChatChannel_GUILD,-1)
	end
	guild:AddRecord(str);
	return ResultCode_ResultOK
end

server_common_request["GetGuildBossUserDamageReq"]= function(sever, player, packet )
	LuaMtGuildManager():OnGetGuildBossUserDamage(player)
	return ResultCode_ResultOK
end

server_common_request["GetGuildCopyDamageReq"]= function(sever, player, packet )
	local copyid = packet:int32_params(GetGuildCopyDamageReq.data_32.copyid)
	LuaMtGuildManager():OnGetGuildCopyDamage(player,copyid)
	return ResultCode_ResultOK
end

server_common_request["GetGuildCopyServerDamageReq"]= function(sever, player, packet )	
	LuaMtGuildManager():OnGetGuildCopyServerDamage(player)
	return ResultCode_ResultOK
end

server_common_request["GetGuildSpoilItemReq"]= function(sever, player, packet )	
	local guild = LuaMtGuildManager():GetGuildByid(player:GetGuildID());
	local copyid = packet:int32_params(GuildSpoilItemReq.data_32.copyid)
	if guild ~= nil then
		local user = guild:GetGuildUser(player:Guid());
		if user ~= nil then
			guild:SendSpoilItem(player,copyid);
			local times = user:spoiljumptimes();
			player:SendCommonReply("UpdateSpoilJumptimes", { times }, {}, {});
		end
	end
	return ResultCode_ResultOK
end

server_common_request["GuildSpoilApplyItemReq"]= function(sever, player, packet )	
	local guild = LuaMtGuildManager():GetGuildByid(player:GetGuildID());
	if guild == nil then
		return ResultCode_ResultOK;
	end
	local playerid = player:Guid();
	local index = packet:int32_params(GuildSpoilApplyItemReq.data_32.index)
	local spoils = guild:GetSpoilItem();
	if spoils == nil then
		return ResultCode_ResultOK;
	end
	local spoilitem = guild:GetSpoilItemByIndex(index);
	if spoilitem == nil then
		return ResultCode_ResultOK;
	end
	for i=0,spoilitem:playerid_size()-1,1 do
		if spoilitem:playerid(i) == playerid then
			player:SendClientNotify("yishenqing",-1,-1);
			return ResultCode_ResultOK;
		end			
	end
	local ok = false;
	for i=0,spoilitem:playerid_size()-1,1 do
		if spoilitem:playerid(i) == INVALID_GUID then
			ok = true
			break;
		end
	end
	if ok == false then
		player:SendClientNotify("shenqingmanle",-1,-1);
		return ResultCode_ResultOK;
	end
	
	for k, item in pairs(spoils) do
		for i=0,item:playerid_size()-1,1 do
			if item:playerid(i) == playerid then
				item:set_playerid(i,INVALID_GUID)
				guild:SendUpdateSpoilItem(player,item:index())
			end			
		end
	end
	for i=0,spoilitem:playerid_size()-1,1 do
		if spoilitem:playerid(i) == INVALID_GUID then
			spoilitem:set_playerid(i,playerid)
			break;
		end
	end
	guild:SendUpdateSpoilItem(player,index)
	player:SendClientNotify("Guild_ApplyOneKeyOK",-1,-1);
	
	return ResultCode_ResultOK
end

server_common_request["GuildSpoilApplyJumpReq"]= function(sever, player, packet )
	local guild = LuaMtGuildManager():GetGuildByid(player:GetGuildID());
	if guild == nil then
		return ResultCode_ResultOK;
	end
	local playerid = player:Guid();
	local user = guild:GetGuildUser(playerid);
	if user == nil then
		return ResultCode_ResultOK;
	end
	local times = user:spoiljumptimes();
	if times >= 2 then
		return player:SendClientNotify("chaduicishubuzu",-1,-1)
	end
	
	local index = packet:int32_params(GuildSpoilApplyItemReq.data_32.index)
	local spoilitem = guild:GetSpoilItemByIndex(index);
	if spoilitem == nil then
		return ResultCode_ResultOK;
	end
	local playercount = spoilitem:playerid_size();
	if playercount <= 1 then
		return ResultCode_ResultOK;
	end
	local in_ = false;
	for i=0,playercount-1,1 do
		if spoilitem:playerid(i) == playerid then
			in_ = true;
			break;
		end			
	end
	if in_ == false then
		return ResultCode_ResultOK;
	end
	--扣帮会贡献度
	local constdata = LuaMtConfigDataManager():FindConfigValue("union_spoil"):value1();
	local contribution = user:curcontribution();
	if contribution < constdata then
		return player:SendClientNotify("TokenGuildContributeNotEnough",-1,-1)
	end
	contribution = contribution - constdata;
	user:set_curcontribution(contribution);
	times = times + 1;
	user:set_spoiljumptimes(times);
	player:SendCommonReply("UpdateSpoilJumptimes", { times }, {}, {});
	
	local toke = 0;
	for i=playercount-1,1,-1 do
		local iterid = spoilitem:playerid(i);
		if iterid == playerid then
			if toke == 0 then --第一次排名
				toke = math.floor(constdata/i);
			end
			
			local preiterid = spoilitem:playerid(i-1);
			spoilitem:set_playerid(i,preiterid)
			spoilitem:set_playerid(i-1,iterid)
			guild:AddUserContribution(preiterid,toke);
		end
	end
	guild:SendUpdateSpoilItem(player,index)
	
	return ResultCode_ResultOK;
end

server_common_request["GetRecordReq"]= function(sever, player, packet )
	local guild = LuaMtGuildManager():GetGuildByid(player:GetGuildID());
	if guild == nil then
		return ResultCode_ResultOK;
	end
	guild:SendRecord(player)
	return ResultCode_ResultOK;
end
--获得当前弹劾信息
server_common_request["GuildImpeachReq"]= function(sever, player, packet )
	local guild = LuaMtGuildManager():GetGuildByid(player:GetGuildID());
	if guild == nil then
		return ResultCode_ResultOK;
	end
	guild:SendGuildImpeach(player)
	return ResultCode_ResultOK;
end
--发起弹劾
server_common_request["AskGuildImpeachReq"]= function(sever, player, packet )
	if player == nil then
		return ResultCode_ResultOK;
	end
	local guild = LuaMtGuildManager():GetGuildByid(player:GetGuildID());
	if guild == nil then
		return ResultCode_ResultOK;
	end
	local playerguid = player:Guid();
	local playername = player:Name();
	local guilduser = guild:GetGuildUser(playerguid);
	if guilduser == nil then
		return ResultCode_ResultOK;
	end
	if guilduser:position() == GuildPosition_CHIEFTAIN then
		player:SendClientNotify("huizhangtanhe",-1,-1);
		return ResultCode_ResultOK;
	end

	local guildimpeach = guild:GetGuildImpeach();
	if guildimpeach == nil then
		return ResultCode_ResultOK;
	end
	local impeachguild = guildimpeach:impeachguild();
	if impeachguild ~= INVALID_GUID then
		player:SendClientNotify("xxtanhe|"..guildimpeach:impeachname(),-1,-1);
		return ResultCode_ResultOK;
	end

	local usersize = guild:GetGuildUserSize();
	usersize = math.floor(usersize*0.3+0.5);
	local loginsize = guild:GetGuildUserSizeLessLoginDay(7);
	loginsize = math.floor(loginsize*0.5+0.5);
	local needcount = math.max(usersize,loginsize);
	
	guildimpeach:set_impeachguild(playerguid);
	guildimpeach:set_impeachname(playername);
	guildimpeach:set_endtime(3600);
	guildimpeach:set_needcount(needcount);
	guildimpeach:add_respondguild(playerguid);
	player:SendClientNotify("faqitanheok",-1,-1);
	lua_send_guild_system_message("xiangyingtanhetishi|"..playername,guild:GetGuildId());
	guild:SendGuildImpeach(player);
	return ResultCode_ResultOK;
end
--响应弹劾
server_common_request["ResponseGuildImpeachReq"]= function(sever, player, packet )
	if player == nil then
		return ResultCode_ResultOK;
	end
	local guild = LuaMtGuildManager():GetGuildByid(player:GetGuildID());
	if guild == nil then
		return ResultCode_ResultOK;
	end
	local playerguid = player:Guid();
	local guilduser = guild:GetGuildUser(playerguid);
	if guilduser == nil then
		return ResultCode_ResultOK;
	end
	if guilduser:position() == GuildPosition_CHIEFTAIN then
		return ResultCode_ResultOK;
	end
	--local allsecond = day * 24 * 3600;
	--if LuaMtTimerManager():DiffTimeToNow(guilduser:lastlogouttime()) > allsecond then
	--	return ResultCode_ResultOK;
	--end
	
	local guildimpeach = guild:GetGuildImpeach();
	if guildimpeach == nil then
		return ResultCode_ResultOK;
	end
	local impeachguild = guildimpeach:impeachguild();
	if impeachguild == INVALID_GUID then
		player:SendClientNotify("wurentanhe",-1,-1);
		return ResultCode_ResultOK;
	end
	for i=0,guildimpeach:respondguild_size()-1,1 do
		if guildimpeach:respondguild(i) == playerguid then
			player:SendClientNotify("havexiangyingtanhe",-1,-1);
			return ResultCode_ResultOK;
		end
	end
	guildimpeach:add_respondguild(playerguid);
	player:SendClientNotify("xiangyingtanheok",-1,-1);
	guild:SendGuildImpeach(player);
	return ResultCode_ResultOK;
end

server_common_request["SetSign"]= function(sever, player, packet )
	if player == nil then
		return ResultCode_ResultOK;
	end
	local size = packet:int32_params_size();
	if size > 4 then
		--player:SendClientNotify("",-1,-1);
		return ResultCode_ResultOK;
	end
	
	local playerzoneinfo = LuaMtFriendManager():GetPlayerZone(player:Guid());
	if playerzoneinfo == nil then
		return ResultCode_ResultOK;
	end
	local zone = playerzoneinfo:GetZoneInfo();
	if zone == nil then
		return ResultCode_ResultOK;
	end
	for i=0,size-1,1 do
		zone:set_signid(i,packet:int32_params(i))
	end
	for i=size,3,1 do
		zone:set_signid(i,0)
	end
	playerzoneinfo:UpdateSign(player)
	return ResultCode_ResultOK;
end
--写留言板
server_common_request["MessageBoardReq"]= function(sever, player, packet )
	if player == nil then
		return ResultCode_ResultOK;
	end
	local owerplayerid = packet:int64_params(MessageBoardReq.data_64.targetguid);
	
	local targetplayerzoneinfo = LuaMtFriendManager():GetPlayerZone(owerplayerid);
	if targetplayerzoneinfo == nil then
		return ResultCode_ResultOK;
	end
	local message = packet:string_params(MessageBoardReq.string.message)
	if LuaMtWordManager():NoSensitiveWord(message) == false then
		player:SendClientNotify("neirongfeifa",-1,-1)
		return ResultCode_ResultOK;
	end
	local replyplayer = packet:string_params(MessageBoardReq.string.replyplayer)
	if replyplayer ~= "" then
		if replyplayer == player:Name() then -- targetguid == player:Guid() and
			player:SendClientNotify("nosendself",-1,-1)
			return ResultCode_ResultOK
		end
	else
		if owerplayerid == player:Guid() then --  and
			player:SendClientNotify("nosendself",-1,-1)
			return ResultCode_ResultOK
		end
	end
	targetplayerzoneinfo:AddMessageBoard(owerplayerid,player,replyplayer,message);
	return ResultCode_ResultOK;
end
--Get留言板信息
server_common_request["GetMessageBoardReq"]= function(sever, player, packet )
	if player == nil then
		return ResultCode_ResultOK;
	end
	local targetguid = packet:int64_params(MessageBoardReq.data_64.targetguid)
	local targetplayer = LuaServer():FindPlayer(targetguid)
	if targetplayer == nil then
		return ResultCode_ResultOK;
	end
	return ResultCode_ResultOK;
end
--打开好友列表
server_common_request["OpenFriendsPanel"]= function(sever, player, packet )
	if player == nil then
		return ResultCode_ResultOK;
	end
	player:send_friendlistupdate();
	player:GetMessageContainer():send_notify_playerinfos(player);
	return ResultCode_ResultOK;
end

function x010002_HandleServerCommonMessage( sever, player, packet )
	LuaServer():LuaLog(packet:request_name(),g_log_level.LL_INFO)
	return server_common_request[packet:request_name()]( sever, player, packet )	
end