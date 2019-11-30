

---------------------------------------------------------------------
G2S_msg_common={}
S2S_msg_common={}

S2S_msg_common["S2SRaidEndAllBattle"]= function(scene, player, packet )		
	local raid_id = packet:int64_params(S2SRaidEndAllBattle.data_64.raid_rtid)
	local raid = scene:GetRaid(raid_id)
	if raid ~= nil then
		local attackers =raid:GetAttackers()
		for i,player in pairs(attackers) do
			player:QuitBattle()
		end
		local defenders = raid:GetDefenders()
		for i,player in pairs(defenders) do
			player:QuitBattle()
		end
	end
	return ResultCode_ResultOK
end


S2S_msg_common["S2SCloneMission"]	=function(scene, player, packet )
	
	local target_guid = packet:int64_params(S2SCloneMission.data_64.target_guid)
	local mission_id = packet:int32_params(S2SCloneMission.data_32.mission_id)
	local target_player = LuaServer():FindPlayer(target_guid)
	local proxy = player:GetMissionProxy()
	proxy:OnCloneMission(target_player,mission_id)
	return ResultCode_ResultOK
end

S2S_msg_common["S2SRaidDestroy"]	=function(scene, player, packet )
	local raid_rtid = packet:int64_params(S2SRaidDestroy.data_64.raid_rtid)
	local raid = scene:GetRaid(raid_rtid)
	if raid ~= nil then
		raid:OnDestroy()
	end
	return ResultCode_ResultOK
end

S2S_msg_common["S2SGenRaidBuff"]	=function(scene, player, packet )
	local raid_rtid = packet:int32_params(S2SGenRaidBuff.data_32.raid_rtid)
	local raid = scene:GetRaid(raid_rtid)
	if raid ~= nil then
		raid:OnGenBuff()
	end
	return ResultCode_ResultOK
end


----------------G2S-----------------------------------
G2S_msg_common["G2SCreateGuildReq"]	=function(scene, player, packet )
	local createname = packet:string_params(G2SCreateGuildReq.data_string.createname)
	local createnotice = packet:string_params(G2SCreateGuildReq.data_string.createnotice)
	player:OnCreateGuild(createname,createnotice)
	return ResultCode_ResultOK
end

G2S_msg_common["G2SGuildBuildLevelUp"]	=function(scene, player, packet )
	local buildtype = packet:int32_params(G2SGuildBuildLevelUp.data_32.type)
	local buildlevel = packet:int32_params(G2SGuildBuildLevelUp.data_32.level)
	if buildtype == GuildBuildType_GUILD_SHOP then
		local market = player:FindMarket(MarketType_MarketType_Union)
		if market ~= nil then
			market:Refresh(true)
		end
	end
	player:GetPlayerGuildProxy():SetBuildLevel(buildtype,buildlevel);
	return ResultCode_ResultOK
end

G2S_msg_common["GuildBonus"]	=function(scene, player, packet )
	local bonus = packet:int32_params(G2SGuildBonus.data_32.bonus)
	player:AddItemByIdWithNotify(TokenType_Token_Gold, bonus,ItemAddLogType_AddType_Guild,0,1);
	return ResultCode_ResultOK
end

G2S_msg_common["G2SGuildUserPosition"]	=function(scene, player, packet )
	local position = packet:int32_params(G2SGuildUserPosition.data_32.position)
	player:GetPlayerGuildProxy():SetGuildPosition(position);
	player:SendCommonReply("UpdateSelfPosition", { position }, {}, {});
	return ResultCode_ResultOK
end

G2S_msg_common["G2SUpdateGuildInfo"]	=function(scene, player, packet )
	local guildid = packet:int64_params(G2SUpdateGuildInfo.data_64.guild_guid)
	local guildname = packet:string_params(G2SUpdateGuildInfo.data_string.guild_name)
	local log_from = packet:string_params(G2SUpdateGuildInfo.data_string.log_from)
	player:UpdateGuildInfo(guildname,guildid,log_from)
	return ResultCode_ResultOK
end

G2S_msg_common["G2SUpdateGuildName"]	=function(scene, player, packet )
	local guildname = packet:string_params(G2SUpdateGuildName.data_string.guild_name)
	player:SetGuildName(guildname)
	return ResultCode_ResultOK
end

G2S_msg_common["G2SAddPracticeExp"]	=function(scene, player, packet )
	local id = packet:int32_params(G2SAddPracticeExp.data_32.id)
	local type = packet:int32_params(G2SAddPracticeExp.data_32.type)
	local level = packet:int32_params(G2SAddPracticeExp.data_32.level)
	local contribution = packet:int32_params(G2SAddPracticeExp.data_32.contribution)
	player:AddPracticeExp(id,type,level,contribution)
	return ResultCode_ResultOK
end


G2S_msg_common["G2SJoinGuildOk"]	=function(scene, player, packet )

	x000504_xOnJoinGuildOk(player,{["value"]=-1})
	return ResultCode_ResultOK
end


G2S_msg_common["G2SCreateBfBattle"]	=function(scene, player, packet )

	local battle_group = packet:int32_params(G2SCreateBfBattle.data_32.battle_group_id)
	local target_guid = packet:int64_params(G2SCreateBfBattle.data_64.target_guid)
	
	local target = scene:FindPlayer(target_guid)
	local battle = nil
	if target ~= nil then	
		--battle = scene:GetBattleManager():CreateFrontBattle(player,target,
		battle = player:GetBattleManager():CreateFrontBattle(player,target,
		{
			["script_id"]=1022,
			["group_id"] =battle_group
		})
	end
	
	local isok = 1
	if battle == nil then
		isok = 0
	end
	
	--通知主线程结果
	LuaServer():SendS2GCommonMessage("S2GBfCreateBattleResult",{isok},{player:Guid(),target_guid},{})
	
	
	return ResultCode_ResultOK
end


G2S_msg_common["G2SRelive"]	=function(scene, player, packet )

	local hp_type = packet:int32_params(G2SRelive.data_32.hp_type)
	--重置一下阵容血量
	player:ResetPlayerActorLastHp(hp_type)
	
	return ResultCode_ResultOK
end

G2S_msg_common["G2SGoRaid"]	=function(scene, player, packet )

	local sceneid = packet:int32_params(G2SGoRaid.data_32.sceneid)
	local scriptid = packet:int32_params(G2SGoRaid.data_32.scriptid)
	local raidid = packet:int32_params(G2SGoRaid.data_32.raidid)	
	local camp = packet:int32_params(G2SGoRaid.data_32.camp)
	local x = packet:int32_params(G2SGoRaid.data_32.x)
	local y = packet:int32_params(G2SGoRaid.data_32.y)
	local z = packet:int32_params(G2SGoRaid.data_32.z)	
	player:GoRaid(sceneid, scriptid,raidid,camp, {x,y,z})
	
	return ResultCode_ResultOK
end

G2S_msg_common["G2SGoTo"]	=function(scene, player, packet )

	local sceneid = packet:int32_params(G2SGoTo.data_32.sceneid)
	local x = packet:int32_params(G2SGoTo.data_32.x)
	local y = packet:int32_params(G2SGoTo.data_32.y)
	local z = packet:int32_params(G2SGoTo.data_32.z)	
	player:GoTo(sceneid, {x,y,z})
	
	return ResultCode_ResultOK
end


G2S_msg_common["G2SInvest"]	=function(scene, player, packet )

	local dc_container = player:DataCellContainer()
	local old = dc_container:get_data_32(CellLogicType_ActionData, g_action_data_index.invest_count)
	local action_data= LuaServer():GetInvestCount()

	if old ~= action_data then
		dc_container:set_data_32(CellLogicType_ActionData, g_action_data_index.invest_count,action_data,true)
	end

	x002614_OnResolve(player)
	
	return ResultCode_ResultOK
end

G2S_msg_common["G2SPowerAcrCheck"]	=function(scene, player, packet )

	local result = packet:int32_params(G2SPowerAcrCheck.data_32.result)
	local index = packet:int32_params(G2SPowerAcrCheck.data_32.index)
	x002635_OnResolve(player,{["index"]=index,["result"]=result})			
	return ResultCode_ResultOK
end

G2S_msg_common["G2SPowerAcrRefresh"]	=function(scene, player, packet )

	local count = packet:int32_params(G2SPowerAcrRefresh.data_32.count)
	local index = packet:int32_params(G2SPowerAcrRefresh.data_32.index)
	
	player:SendLuaPacket("PowerAcrRefresh",{index,count},{},{})
	
	return ResultCode_ResultOK
end

G2S_msg_common["G2SLevelAcrCheck"]	=function(scene, player, packet )

	local result = packet:int32_params(G2SLevelAcrCheck.data_32.result)
	local index = packet:int32_params(G2SLevelAcrCheck.data_32.index)
	x002636_OnResolve(player,{["index"]=index,["result"]=result})	
	return ResultCode_ResultOK
end

G2S_msg_common["G2SLevelAcrRefresh"]	=function(scene, player, packet )

	local count = packet:int32_params(G2SLevelAcrRefresh.data_32.count)
	local index = packet:int32_params(G2SLevelAcrRefresh.data_32.index)
	
	player:SendLuaPacket("LevelAcrRefresh",{index,count},{},{})
	
	return ResultCode_ResultOK
end


G2S_msg_common["G2SQuikBuyAcrRefresh"]	=function(scene, player, packet )

	local dc_container = player:DataCellContainer()
	local old = dc_container:get_data_32(CellLogicType_ActionData, g_action_data_index.quik_buy_count)
	
	local count = packet:int32_params(G2SQuikBuyAcrRefresh.data_32.count)
    
	if old ~= count then
		dc_container:set_data_32(CellLogicType_ActionData, g_action_data_index.quik_buy_count,count,true)
	end

	return ResultCode_ResultOK
end

G2S_msg_common["G2SQuikBuyAcrOver"]	=function(scene, player, packet )

	local script_id = packet:int32_params(G2SQuikBuyAcrOver.data_32.script_id)
	x000011_GAcrstatusUpdateNew(player,{["value"]=script_id,["check_open"]=0,["check_show"]=0,["user_data"]=-1,["serial_id"]=-1})			
	
	return ResultCode_ResultOK
end

G2S_msg_common["G2SQuikBuyAcrBegin"]	=function(scene, player, packet )

	--重置一下锁标记
	player:SetParams32("quik_buy",-1)
	
	if packet:int32_params_size() ~= table.nums(G2SQuikBuyAcrBegin.data_32) then
		return player:SendClientNotify("Invalid_Params",-1,-1)
	end
	
	local index = packet:int32_params(G2SQuikBuyAcrBegin.data_32.index)
	local script_id = packet:int32_params(G2SQuikBuyAcrBegin.data_32.script_id)
	local count = packet:int32_params(G2SQuikBuyAcrBegin.data_32.count)
	
	local dc_container = player:DataCellContainer()
	dc_container:set_data_32(CellLogicType_ActionData, g_action_data_index.quik_buy_count,count,true)
		
	--每次新开启第一档就重置所有标记	
	if index == 1 then	
		dc_container:set_data_32(CellLogicType_AcRewardCell, g_acr_datacell_index.quik_buy_acr_flag, 0, true)
	end
	
	x000011_GAcrstatusUpdateNew(player,{["value"]=script_id,["check_open"]=1,["check_show"]=1,["user_data"]=index,["serial_id"]=-1})					
	
	return ResultCode_ResultOK
end


G2S_msg_common["G2SAllBuyAcrJoin"]	=function(scene, player, packet )

	local result = packet:int32_params(G2SAllBuyAcrJoin.data_32.result)
	local index = packet:int32_params(G2SAllBuyAcrJoin.data_32.index)
	x002637_OnResolve(player,{["index"]=index,["result"]=result})	
	return ResultCode_ResultOK
end


G2S_msg_common["G2SQuikBuyAcrJoin"]	=function(scene, player, packet )

	local result = packet:int32_params(G2SQuikBuyAcrJoin.data_32.result)
	local index = packet:int32_params(G2SQuikBuyAcrJoin.data_32.index)
	x002638_OnResolve(player,{["index"]=index,["result"]=result})	
	return ResultCode_ResultOK
end



------------
G2S_msg_common["G2SMonthTriger"]	=function(scene, player, packet )
	local week = packet:int32_params(G2SMonthTriger.data_32.value)
	scene:OnMonthTriger(week)	
	return ResultCode_ResultOK
end
G2S_msg_common["G2SWeekTriger"]	=function(scene, player, packet )
	local week = packet:int32_params(G2SWeekTriger.data_32.value)
	scene:OnWeekTriger(week)	
	return ResultCode_ResultOK
end
G2S_msg_common["G2SDayTriger"]	=function(scene, player, packet )
	local day = packet:int32_params(G2SDayTriger.data_32.value)
	scene:OnDayTriger(day)	
	return ResultCode_ResultOK
end
G2S_msg_common["G2SHourriger"]	=function(scene, player, packet )
	local hour = packet:int32_params(G2SHourriger.data_32.value)
	scene:OnHourTriger(hour)	
	return ResultCode_ResultOK
end
G2S_msg_common["G2SMinTriger"]	=function(scene, player, packet )
	local mini = packet:int32_params(G2SMinTriger.data_32.value)
	scene:OnMinTriger(mini)	
	return ResultCode_ResultOK
end
----------------------
G2S_msg_common["G2SUpdateAcrConfig"]	=function(scene, player, packet )
	local id = packet:int32_params(G2SUpdateAcrConfig.data_32.activity_id)
	if id == -1 then
		local jsons =LuaOperativeConfigManager():GetConfigs()
		for k,config in pairs( jsons ) do
			if config ~= nil and  acr_static_config[config:script_id()] ~= nil then
				acr_static_config.decode(config:script_id(),config:content())
				scene:GetLuaCallProxy():OnLuaFunCall_n_Scene(scene,config:script_id(),"OnUpdateAcrConfig",{})
			end
		end
	else
		local config = LuaOperativeConfigManager():FindConfig(id)
		if config ~= nil and acr_static_config[config:script_id()] ~= nil then
			acr_static_config.decode(config:script_id(),config:content())
			scene:GetLuaCallProxy():OnLuaFunCall_n_Scene(scene,config:script_id(),"OnUpdateAcrConfig",{})
		end
	end
	return ResultCode_ResultOK
end

G2S_msg_common["G2SRefreshAcrConfig"]	=function(scene, player, packet )
	local id = packet:int32_params(G2SRefreshAcrConfig.data_32.activity_id)
	if player ~= nil then
		--状态更新	
		x000011_GAcrstatusCheckUpdateNewEx(player, id)
		--数据同步
		player:Refresh2ClientOperateConfig(id)
	end
	return ResultCode_ResultOK
end
--------------------------------

G2S_msg_common["G2SFightGuildWetCopy"]	=function(scene, player, packet )
	local copyid = packet:int32_params(G2SFightGuildWetCopy.data_32.copyid)
	local copyconfig = LuaMtGuildManager():GetGuildWetCopyConfig(copyid);
	if copyconfig == nil then
		return ResultCode_ResultOK;
	end
	local stageid = packet:int32_params(G2SFightGuildWetCopy.data_32.stageid)
	player:GetPlayerGuildProxy():SetWetcopyCopyId(copyid)
	player:GetPlayerGuildProxy():SetWetcopyStageId(stageid)
	for i=2,packet:int32_params_size()-1 do
		local hp = packet:int32_params(i)
		if hp < 0 then
			hp = 0
		end
		player:GetPlayerGuildProxy():SetWetcopyMonsterHp(i-2,hp)
	end

	player:GoRaid(copyconfig:sceneid() ,2005,0,-1,{-1,-1,-1}); 
	return ResultCode_ResultOK
end


G2S_msg_common["G2SGoingLeaveScene"] = function(scene,player, packet)
	player:GoingLeaveScene();
	return ResultCode_ResultOK
end

G2S_msg_common["G2SLuaFilesUpdate"] = function(scene,player, packet)
	player:Refresh2ClientLuaFiles(true)
	return ResultCode_ResultOK
end


G2S_msg_common["G2SSwitchMarket"] = function(scene,player, packet)
	local id = packet:int32_params(G2SSwitchMarket.data_32.id)
	local open = packet:int32_params(G2SSwitchMarket.data_32.open)
	local market = player:FindMarket(id)
	if market ~= nil then
		if open == 1 then
			market:Enable(true,false)
		else
			market:Enable(false,false)
		end
		
	end
	return ResultCode_ResultOK
end

G2S_msg_common["G2SSwitchMarketItem"] = function(scene,player, packet)
	local id = packet:int32_params(G2SSwitchMarketItem.data_32.id)
	local index = packet:int32_params(G2SSwitchMarketItem.data_32.index)
	local open = packet:int32_params(G2SSwitchMarketItem.data_32.open)
	local market = player:FindMarket(id)
	if market ~= nil then
		if open == 1 then
			market:EnableItem(index,true)
		else
			market:EnableItem(index,false)
		end
		
	end
	return ResultCode_ResultOK
end



G2S_msg_common["G2SUpdateServerLevel"] = function(scene,player, packet)
	player:ServerLevelCheck(player:PlayerLevel())
	return ResultCode_ResultOK
end

G2S_msg_common["G2SAcrRefresh"] = function(scene,player, packet)
	--player:SetParams64("acr_activie", 0)
	player:OnLuaFunCall_x("xRefreshData2Clinet",{["value"]=-1})
	return ResultCode_ResultOK
end

G2S_msg_common["G2SLogout"] = function(scene,player, packet)
	if player ~= nil then
		player:OnLogout()
	end
	return ResultCode_ResultOK
end



G2S_msg_common["G2SActivityRefresh"] = function(scene,player, packet)
	player:ActivityListModify()
	return ResultCode_ResultOK
end

G2S_msg_common["G2SChargeItem"]	=function(scene, player, packet )
	local orderid = packet:string_params(G2SChargeItem.data_string.orderid)
	local productid = packet:string_params(G2SChargeItem.data_string.productid)
	local ip = packet:string_params(G2SChargeItem.data_string.ip)
	local channel_id = packet:string_params(G2SChargeItem.data_string.extra)	
		
	x000100_OnChargeItem(player,orderid,productid,ip,channel_id)	
	
	return ResultCode_ResultOK
end
------------------------------------------------------------------------------------

function x010003_HandleS2SCommonMessage( scene,player, packet )
	--LuaServer():LuaLog(packet:request_name(),g_log_level.LL_INFO)
	
	return S2S_msg_common[packet:request_name()]( scene, player, packet )	
end

function x010003_HandleG2SCommonMessage( scene,player, packet )
	--LuaServer():LuaLog(packet:request_name(),g_log_level.LL_INFO)
	
	return G2S_msg_common[packet:request_name()]( scene, player, packet )	
end


