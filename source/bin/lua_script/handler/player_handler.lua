
---------------------------------------------------
function x010000_OnRegisterFunction(proxy)
	
	if proxy ~= nil then
		proxy:RegFunc("xOnLuaRequestPacket"		,10000)
		proxy:RegFunc("xOnCommonRequestPacket"	,10000)
		proxy:RegFunc("xOnCG_GemFusion"			,10000)
	end
end
---------------------------------------------------------------------
local msg_common_request={}
local msg_lua_request={}
--local msg_S2G_request={}

msg_lua_request["refresh_market_req"]= function(player,packet)	
	local type = packet:int32_params(refresh_market_req.data_32.mtype)
	--print("dddddddddddddddddddddddddddddddddd")
	local market = player:FindMarket(type)
	if market ~= nil then
		local config = market:GetConfig()
		if config ~= nil then
			if  config:refresh_token()<=0 or config:token_count() <=0 or config:refresh_count()<=0 then
				return ResultCode_InternalResult
			end
			if player:DelItemById(config:refresh_token(),config:token_count(),ItemDelLogType_DelType_Market,-100) == false then
				if type == MarketType_MarketType_Honor then
					return player:SendClientNotify("TokenHonorNotEnough", -1, -1)
				elseif type == MarketType_MarketType_Scroll then
					return player:SendClientNotify("CrystalNotEnough", -1, -1)
				elseif type == MarketType_MarketType_LawFal then
					return player:SendClientNotify("TokenLawFulNotEnough", -1, -1)
				end
			end
		end
		market:Refresh(true)
	end		
	return ResultCode_ResultOK
end

msg_common_request["buy_item_request"]= function(player, packet )
	if packet:int32_params_size() ~= table.nums(buy_item_request.data_32) then
		return player:SendClientNotify("Invalid_Params",-1,-1)
	end
	local mtype = packet:int32_params(buy_item_request.data_32.mtype)
	local index = packet:int32_params(buy_item_request.data_32.index)
	local count = packet:int32_params(buy_item_request.data_32.count)
	if count == 0 then
		return player:SendClientNotify("BuyCountZero",-1,-1)
	end
	if mtype == MarketType_MarketType_MonthCard then 
		local cell_container = player:DataCellContainer()
		if cell_container:check_bit_data(BitFlagCellIndex_MonthCardFlag) == false and  cell_container:check_bit_data(BitFlagCellIndex_LifeCardFlag) == false then
			return player:SendClientNotify("ac_notify_023",-1,-1)			
		end
	end
	return player:BuyMarketItem(mtype,index,count,false)
end
msg_common_request["market_request"]	=function(player, packet )
	local mtype = packet:int32_params(market_request.data_32.mtype)
	return player:MarketReq(mtype)
end
msg_common_request["npcs_request"]		=function(player, packet )
	player:Refresh2ClientNpcList()
	return ResultCode_ResultOK
end

msg_common_request["guide_step_requet"]	=function(player, packet )
	local group = packet:int32_params(guide_step_requet.data_32.group)
	local step = packet:int32_params(guide_step_requet.data_32.step)
	
	--给客户端特写点东西
	--首次扫荡
	if group==23 then
		--print("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx")
		--加波数
		local stage_proxy = player:GetPlayerStageProxy()
		local dbinfo = player:GetPlayerStageDB()
		local camp = player:GetCamp()
		local stage_id = 60111
		if camp == Camp_Alliance then
			stage_id = 60101
		end
		local stage = stage_proxy:GetPlayerStage(stage_id)
		if stage ~= nil and stage:boss()<=0 then
			stage:set_boss(1)		
		end
		dbinfo:set_current_stage(stage_id)
		if stage:raid()==false then
			stage_proxy:OnPlayerRaidStage(stage_id)
		else
			stage_proxy:SyncSpecStageToClient(stage_id)				
		end

		LuaServer():LuaLog("first rush done! guid ="..player:Guid(),g_log_level.LL_INFO)
	--print("ddddddddddddddddddddddddddddddddddddddddddd")
	end

	return player:SetGuideStep(group,step)	
end

msg_lua_request["guide_flag_request"]	=function(player, packet )
	
	local flag = packet:int64_params(guide_flag_request.data_64.flag)	
	local cell_container = player:DataCellContainer()
	cell_container:set_data_64(Cell64LogicType_NumberCell64,NumberCell64Index_GuideFlagData,flag,true)
	
	--print("ddddddddddddddddddddddddddddddddddddddddddd"..flag)
	
	return ResultCode_ResultOK
end

msg_lua_request["inner_hook_flag_request"] = function(player, packet)
	local flag = packet:int64_params(inner_hook_flag_request.data_64.flag)	
	local cell_container = player:DataCellContainer()
	cell_container:set_data_64(Cell64LogicType_NumberCell64,NumberCell64Index_InnerHookFlag,flag,true)	
	return ResultCode_ResultOK	
end

msg_lua_request["set_mission_auto_buy_item_flag"] = function(player, packet)
	local dc_container = player:DataCellContainer()
	if dc_container == nil then
		return ResultCode_UnknownError
	end
	local flag = packer:int32_params(set_mission_auto_buy_item_flag_request.data_32.flag)
	dc_container:set_bit_data(BitFlagCellIndex_MissionAutoBuyItemFlag, flag)
	return ResultCode_ResultOK	
end

msg_lua_request["get_mission_auto_buy_item_flag"] = function(player, packer)
	local dc_container = player:DataCellContainer()
	if dc_container == nil then
		return ResultCode_UnknownError
	end
	local flag = dc_container:check_bit_data(BitFlagCellIndex_MissionAutoBuyItemFlag)
	player:SendLuaPacket("mission_auto_buy_item_flag", {flag},{},{})
	return ResultCode_ResultOK	
end

msg_lua_request["robot_team_create"]	=function(player, packet )
	
	if player:GetTeamID() == 0 then
		player:CreateRobotTeam(1)		
		--player:AddRobotPlayerToTeam()
	else
		LuaServer():SendS2GCommonMessage("S2GLeaveTeam", {}, {player:Guid()}, {});
	end
	
	return ResultCode_ResultOK
end

msg_lua_request["robot_team_destroy"]	=function(player, packet )	
	player:DismissTeam()	
	return ResultCode_ResultOK
end

msg_lua_request["TreasurePosReq"]	=function(player, packet )	
	local guid = packet:int64_params(TreasurePosReq.data_64.guid)	
	local item =player:FindItemByGuid(ContainerType_Layer_ItemBag,guid)
	if item == nil then
		return ResultCode_InvalidItem
	end
	local pos_list = LuaMtMonsterManager():GenRandomPos(2,1)
	if #pos_list < 1 then
		return ResultCode_InvalidConfigData
	end
	local pos_id = pos_list[1]:pos_id()
	player:SetParams32("treasure_pos",pos_id)
	player:SendLuaPacket("TreasurePosRep",{pos_id},{guid},{})
	return ResultCode_ResultOK
end

msg_lua_request["SpeedImpactRequset"]	=function(player, packet )
	local enable = packet:int32_params(SpeedImpactRequset.data_32.enable)		
	if player:PlayerLevel() >= 14 then
		return ResultCode_ResultOK
	end

	local proxy = player:GetSimpleImpactSet()
	proxy:DecSimpleImpact(NewSpeedUpImpact)
	
	--速度buffer
	if enable == 1 then		
		proxy:AddSimpleImpact(NewSpeedUpImpact,99999999,2400,0,false)
	end
	
	return ResultCode_ResultOK
end

msg_common_request["lock_request"]		=function(player, packet )
	local guid = packet:int64_params(lock_request.data_64.guid)
	return player:ItemLockReq(guid,true)
end
msg_common_request["unlock_request"]	=function(player, packet )
	local guid = packet:int64_params(unlock_request.data_64.guid)
	return player:ItemLockReq(guid,false)
end
msg_common_request["exppoint_request"]	=function(player, packet )
	local switch = packet:int32_params(exppoint_request.data_32.switch)
	return player:ExpPointOption(switch)
end
msg_common_request["StoreGold"]			=function(player, packet )
	local value = packet:int64_params(StoreGold.data_64.value)
	return player:GoldOperation(false,value)
end
msg_common_request["TakeGold"]			=function(player, packet )
	local value = packet:int64_params(TakeGold.data_64.value)
	return player:GoldOperation(true,value)
end
msg_common_request["UseItem"]			=function(player, packet )
	local guid = packet:int64_params(UseItem.data_64.guid)
	return player:UseItem(guid,0)
end

msg_common_request["UseMultItem"]			=function(player, packet )
	local config_id = packet:int32_params(UseMultItem.data_32.config_id)
	local count = packet:int32_params(UseMultItem.data_32.count)
	return player:UseItemByCfgId(config_id,count)
end

msg_common_request["DestroyItem"]		=function(player, packet )
	local guid = packet:int64_params(DestroyItem.data_64.guid)
	return player:DestroyItem(guid)
end
msg_common_request["SellItem"]			=function(player, packet )
	local guid = packet:int64_params(SellItem.data_64.guid)
	return player:SellItem(mtype,index,count,true)
end
msg_common_request["CollectOver"]		=function(player, packet )
	local posid = packet:int32_params(CollectOver.data_32.posid)
	return player:OnLuaFunCall_1(502,"OnCollectOver",posid)
end
msg_common_request["ActivityRewardBox"]	=function(player, packet )
	local index = packet:int32_params(ActivityRewardBox.data_32.index)
	return player:ActivityBoxReward(index)
end
msg_common_request["DrawRequest"]		=function(player, packet )
	if player:OnLuaFunCall_n(100,"CheckFunction", { ["zhaomu"]=999} ) ~= ResultCode_ResultOK then
		return ResultCode_InternalResult
	end
	local dtype = packet:int32_params(DrawRequest.data_32.dtype)
	return player:OnDrawCard(dtype)
end
msg_common_request["Inspire"]			=function(player, packet )
	local itype = packet:int32_params(Inspire.data_32.itype)
	return player:OnInspire(itype)
end
msg_common_request["WorldbossRelive"]	=function(player, packet )
	return player:OnWorldBossRelive()
end
msg_common_request["HellRelive"]		=function(player, packet )
	return player:OnHellRelive()
end
msg_common_request["UpgradeMission"]	=function(player, packet )
	local hero_id = packet:int32_params(UpgradeMission.data_32.hero_id)
	local proxy = player:GetMissionProxy()
	proxy:OnAcceptGradeUpMission(hero_id)
	
	local dc_container = player:DataCellContainer()
	if dc_container == nil then
		return ResultCode_UnknownError
	end	
	
	--首次进阶任务	
	local flag = dc_container:get_data_64(Cell64LogicType_NumberCell64, NumberCell64Index_GuideFirstFlag)
	--print("params[scene_id]"..params["scene_id"])
	if dc_container:check_bit_data_64(flag,g_GuideFirstFlag.GradeUpMission) == false then
		local new_flag  = dc_container:set_bit_data_64(flag,g_GuideFirstFlag.GradeUpMission) 
		dc_container:set_data_64(Cell64LogicType_NumberCell64, NumberCell64Index_GuideFirstFlag,new_flag,false)		
		
		local actors = player:Actors()
		for k,actor in pairs(actors) do
			if actor:ActorType() == ActorType_Hero then
				hero_id = actor:ConfigId()
				proxy:OnAcceptGradeUpMission(hero_id)
			end
		end
	end
	return ResultCode_ResultOK
				
end
msg_common_request["DayKillMission"]	=function(player, packet )
	local proxy = player:GetMissionProxy()
	proxy:OnAcceptMission(MissionType_DayKillMission)
	return ResultCode_ResultOK
end
msg_common_request["RideMount"]	=function(player, packet )
	
	--战场里,死亡，扛旗都不允许骑马
	local bf_proxy = player:GetBattleFeildProxy()
	if bf_proxy:raid_rtid() ~= 0 then
		if	bf_proxy:bf_state() == BFState_Bf_Dead
		or bf_proxy:bf_state() == BFState_Bf_Flag
		or bf_proxy:bf_state() == BFState_Bf_Fighting then
			return player:SendClientNotify("MountLimited",-1,-1)
		end
	end

	if player:GetInBattle() == true then
		return player:SendClientNotify("MountLimited",-1,-1)
	end

	local hook = player:HookBattle()
	if hook ~= nil and hook:GetClientInsight() then	
		return player:SendClientNotify("MountLimited",-1,-1)
	end
	
	local id = packet:int32_params(RideMount.data_32.index)
	local mount = LuaMtMonsterManager():FindMount(id)
	if mount == nil then
		return ResultCode_InternalResult
	end

	local container = player:DataCellContainer()
	if container:check_data_32(CellLogicType_MountCell,id) == false then
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
msg_common_request["UnRideMount"]	=function(player, packet )
	local data = player:GetScenePlayerInfo()
	local mount = LuaMtMonsterManager():FindMount(data:mount_id())
	if mount == nil then
		return player:SendClientNotify("not_no_mount",-1,-1)
	end
	data:set_mount_id(-1)
	--清掉速度buffer
	local proxy = player:GetSimpleImpactSet()
	proxy:DecSimpleImpact(mount:impact_id())
	--player:ResetSpeed()
	player:BroadCastCommonReply("UnRideMount",{},{player:Guid()},{})
	return ResultCode_ResultOK
end
msg_common_request["UnlockMount"]	=function(player, packet )
	local id = packet:int32_params(UnlockMount.data_32.index)
	local config = LuaMtMonsterManager():FindMount(id)
	if config == nil then
		return player:SendClientNotify("InvalidConfigData",-1,-1)
	end
	local container = player:DataCellContainer()
	if container:push_data_32(CellLogicType_MountCell,id,true) == false then
		return ResultCode_MountAreadyGet
	end
	return ResultCode_ResultOK
end

--------------------------------------------------------------------------------
msg_lua_request["SetLackeysReq"]	=function(player, packet )

	if player:OnLuaFunCall_n(100,"CheckFunction", { ["pucongshouhu"]=999} ) ~= ResultCode_ResultOK then
		return ResultCode_InternalResult
	end

	local db_info = player:GetDBPlayerInfo()
	local index = packet:int32_params(SetLackeysReq.data_32.index)
	local guid = packet:int64_params(SetLackeysReq.data_64.actor_guid)
	if index <0 or index >3 then
		return ResultCode_Invalid_Request
	end

	if guid ~= 0 then
		local actor = player:FindActorByGuid(guid)
		if actor==nil then
			return ResultCode_Invalid_Request
		end
	end

	db_info:set_lackeys(index,guid)

	player:SendCommonReply("UpdateLackeysRep",{index},{guid},{})
	
	--目标更新
	local count = 0
	for i=0,db_info:lackeys_size()-1,1 do
		if db_info:lackeys(i) ~= 0 then
			count= count +1
		end
	end
	
	x000504_xOnSetLackeys(player,{["value"]=count})

	return ResultCode_ResultOK
end

------------------------------------------------
msg_lua_request["ConfirmRollItem"]	=function(player, packet )
	local scene = player:Scene()
	local leader = scene:FindPlayer(player:GetTeamLeader())
	if leader == nil then
		return player:SendClientNotify("team_not_leader",-1,-1)
	end
	if leader:GetTeamArrangeType() ~= ArrangeType_AT_Roll then
		return player:SendClientNotify("InvalidArrangeType",-1,-1)
	end
	local proxy = leader:GetArrangeProxy()
	local item = packet:int64_params(ConfirmRollItem.data_64.item_guid)
	proxy:ConfirmRollItem(item,player:Guid())

	return ResultCode_ResultOK
end

msg_lua_request["CancelRollItem"]	=function(player, packet )
	local scene = player:Scene()
	local leader = scene:FindPlayer(player:GetTeamLeader())
	if leader == nil then
		return player:SendClientNotify("team_not_leader",-1,-1)
	end
	if leader:GetTeamArrangeType() ~= ArrangeType_AT_Roll then
		return player:SendClientNotify("InvalidArrangeType",-1,-1)
	end
	local proxy = leader:GetArrangeProxy()
	proxy:CancelRollItem(player:Guid())
	return ResultCode_ResultOK
end


msg_lua_request["ArrangeItem"]	=function(player, packet )
	if player:IsTeamLeader()==false then
		return player:SendClientNotify("team_not_leader",-1,-1)
	end

	if player:GetTeamArrangeType() ~= ArrangeType_AT_Leader then
		return player:SendClientNotify("InvalidArrangeType",-1,-1)
	end

	local proxy = player:GetArrangeProxy()
	local item = packet:int64_params(ArrangeItem.data_64.item_guid)
	local guid = packet:int64_params(ArrangeItem.data_64.player_guid)

	if proxy:SetTo(item,guid) == false then
		return player:SendClientNotify("ArrangeFaild",-1,-1)
	end

	return ResultCode_ResultOK
end

msg_lua_request["CloseArrangeUIReq"]	=function(player, packet )
	if player:IsTeamLeader() == false then
		return player:SendClientNotify("team_not_leader",-1,-1)
	end
	local scene = player:Scene()
	if scene == nil then
		return player:SendClientNotify("ResultCode_Invalid_Request",-1,-1)
	end


	local teammemberlist = player:GetTeamMember(true)
	for i,teammemberid in pairs(teammemberlist) do
		local teamplayer = scene:FindPlayer(teammemberid)
		--跳过队长，避免死循环
		if teamplayer:IsTeamLeader() == false then
			teamplayer:SendLuaPacket("CloseArrangeUIRep",{},{},{})
		end
	end

	--提前结束分配
	local proxy = player:GetArrangeProxy()
	proxy:OnArrangeEnd()

	return ResultCode_ResultOK
end

msg_lua_request["PullPlayerSetting"] = function(player, packet)
	local s = player:GetPlayerSetting()
	player:SendLuaPacket("CurrentPlayerSetting", {s:ignore_add_friend(), s:reject_fight(),}, {}, {})
	return ResultCode_ResultOK
end

msg_lua_request["UpdatePlayerSetting"] = function(player, packet)
	local s = player:GetPlayerSetting()
	s:set_ignore_add_friend(packet:int32_params(UpdatePlayerSetting.data_32.ignore_add_friend))
	s:set_reject_fight(packet:int32_params(UpdatePlayerSetting.data_32.reject_fight))
	player:SendLuaPacket("CurrentPlayerSetting", {s:ignore_add_friend(), s:reject_fight(),}, {}, {})
	return ResultCode_ResultOK
end

msg_lua_request["FightRequest"] = function(player, packet)
	local target_guid = packet:int64_params(FightRequest.data_64.target_guid)
	local target = player:Scene():FindPlayer(target_guid)
	if target == nil then
		player:SendClientNotify("pvp_fight_target_cannot_found", -1, -1)
		return ResultCode_ResultOK
	end
	if player:GetTeamID() ~= 0 then
		player:SendClientNotify("pvp_fight_self_in_team", -1, -1)
		return ResultCode_ResultOK
	end
	if player:BattleGround() ~= nil then
		player:SendClientNotify("pvp_fight_self_in_battle", -1, -1)
		return ResultCode_ResultOK
	end

	if target:GetTeamID() ~= 0 then
		player:SendClientNotify("pvp_fight_target_in_team", -1, -1)
		return ResultCode_ResultOK
	end
	if target:BattleGround() ~= nil then
		player:SendClientNotify("pvp_fight_target_in_battle", -1, -1)
		return ResultCode_ResultOK
	end

	if target:GetPlayerSetting():reject_fight() == 1 then
		player:SendClientNotify("pvp_fight_player_reject_fight|"..target:Name(), -1, -1)
		return ResultCode_ResultOK
	end
	target:AddChallengReq(player:Guid())
	target:SendLuaPacket("FightNotify", {}, {player:Guid()}, {player:Name()})
	return ResultCode_ResultOK
end

msg_lua_request["FightAck"] = function(player, packet)
	local challenger_guid = packet:int64_params(FightAck.data_64.challenger_guid)
	local challenger = player:Scene():FindPlayer(challenger_guid)
	if challenger == nil then
		player:SendClientNotify("pvp_fight_challenger_leave_scene", -1, -1)
		return ResultCode_ResultOK
	end
	if player:CheckChallengReq(challenger_guid) == false then
		player:SendClientNotify("pvp_fight_pvp_fight_out_of_time", -1, -1) --超时了
		return ResultCode_ResultOK
	end
	if packet:int32_params(FightAck.data_32.confirm) == 0 then--拒绝
		challenger:SendClientNotify("pvp_fight_player_reject_fight|"..player:Name(), -1, -1)
		return ResultCode_ResultOK
	else
		if player:GetTeamID() ~= 0 then
			player:SendClientNotify("pvp_fight_self_in_team", -1, -1)
			return ResultCode_ResultOK
		end
		if player:BattleGround() ~= nil  then
			player:SendClientNotify("pvp_fight_self_in_battle", -1, -1)
			return ResultCode_ResultOK
		end
		if challenger:GetTeamID() ~= 0 then
			player:SendClientNotify("pvp_fight_target_in_team", -1, -1)
			return ResultCode_ResultOK
		end
		if challenger:BattleGround() ~= nil then
			player:SendClientNotify("pvp_fight_target_in_battle", -1, -1)
			return ResultCode_ResultOK
		end
		
		local battle = challenger:GetBattleManager():CreateFrontBattle(challenger,player,
		{
			["script_id"]=1024,
			["group_id"] =1
		})
			
		--if challenger:OnGoFight(player:Guid(), 1, 1024) == true then
		if battle ~= nil then
			player:SendClientNotify("pvp_fight_pvp_fight_start", -1, -1)
			challenger:SendClientNotify("pvp_fight_pvp_fight_start", -1, -1)
		end
	end
	return ResultCode_ResultOK
end


msg_lua_request["SetOfflineHookData"] = function (player, packet)
	if player:CanAutoHook() == false then
		return player:SendClientNotify("offline_hook_should_raid_first_stage", -1, -1)
	end

	local stage_db = player:GetPlayerStageDB()
	local last_stage_id = packet:int32_params(SetOfflineHookData.data_32.stage_2)
	local stage_config = LuaMtSceneStageManager():GetConfig(last_stage_id)
	if last_stage_id == 0 or stage_config == nil then
		return player:SendClientNotify("offline_hook_last_stage_should_have_data",-1,-1)
	end
	local first_stage = packet:int32_params(SetOfflineHookData.data_32.stage_0)
	local second_stage = packet:int32_params(SetOfflineHookData.data_32.stage_1)
	if first_stage ~= 0 and LuaMtSceneStageManager():GetConfig(first_stage) == nil then
		return player:SendClientNotify("offline_hook_invalid_off_hook_data",-1,-1)
	end
	if second_stage ~= 0 and LuaMtSceneStageManager():GetConfig(second_stage) == nil then
		return player:SendClientNotify("offline_hook_invalid_off_hook_data",-1,-1)
	end
	if first_stage > 0 and (first_stage == second_stage  or first_stage == last_stage_id) then
		return player:SendClientNotify("offline_hook_no_duplicate_scene", -1, -1)
	end
	if second_stage > 0 and second_stage == last_stage_id then
		return player:SendClientNotify("offline_hook_no_duplicate_scene", -1, -1)
	end
	if first_stage > 0 and packet:int32_params(SetOfflineHookData.data_32.time_0) <= 0 then
		return player:SendClientNotify("offline_hook_invalid_off_hook_data",-1,-1)
	end
	if second_stage > 0 and packet:int32_params(SetOfflineHookData.data_32.time_1) <= 0 then
		return player:SendClientNotify("offline_hook_invalid_off_hook_data",-1,-1)
	end
	if first_stage > 0 and player:PlayerCanHook(first_stage) == false then
		return player:SendClientNotify("LevelLimit", -1, -1)
	end
	if second_stage > 0 and player:PlayerCanHook(second_stage) == false then
		return player:SendClientNotify("LevelLimit", -1, -1)
	end
	if last_stage_id > 0 and player:PlayerCanHook(last_stage_id) == false then
		return player:SendClientNotify("LevelLimit", -1, -1)
	end
	--0
	for i=0,stage_db:configs_size() - 1 do
		local data = stage_db:mutable_configs(i)
		data:Clear()
		data:SetInitialized()
	end
	if first_stage > 0 then
		print("set_first_stage")
		local hook_data = stage_db:mutable_configs(0)
		hook_data:set_stage_id(packet:int32_params(SetOfflineHookData.data_32.stage_0))
		hook_data:set_total_time(packet:int32_params(SetOfflineHookData.data_32.time_0))
		hook_data:set_hook_time(0)
		hook_data:set_calc_done(false)
	end
	--1
	if second_stage > 0 then
		print("set_second_stage")
		local hook_data =  stage_db:mutable_configs(1)
		hook_data:set_stage_id(packet:int32_params(SetOfflineHookData.data_32.stage_1))
		hook_data:set_total_time(packet:int32_params(SetOfflineHookData.data_32.time_1))
		hook_data:set_hook_time(0)
		hook_data:set_calc_done(false)
	end
	--2
	local max_hook_time = 60 * 60 * 24 * 1000 - packet:int32_params(SetOfflineHookData.data_32.time_0) - packet:int32_params(SetOfflineHookData.data_32.time_1)
	local hook_data = stage_db:mutable_configs(2)
	hook_data:set_stage_id(packet:int32_params(SetOfflineHookData.data_32.stage_2))
	hook_data:set_total_time(max_hook_time)
	hook_data:set_hook_time(0)
	hook_data:set_calc_done(false)
	stage_db:set_current_stage(player:GetPlayerStageProxy():FirstNeedCalcId())
	player:GetPlayerStageProxy():ClearEfficentLog()
	local hook_battle = player:HookBattle()
	if hook_battle ~= nil and hook_battle:Brief():stage_id() ~= stage_db:current_stage() then
		local client_insight = hook_battle:GetClientInsight()
		player:QuitHookBattle()
		if client_insight then
			player:SendCommonResult(ResultOption_OnHook_Opt,ResultCode_HookEndOK)	
		end
	end
	player:SendClientNotify("offline_hook_set_success", -1, -1)
	local msg = PlayerStages:new()
	msg:set_player_guid(stage_db:player_guid())
	msg:set_current_stage(stage_db:current_stage())
	for i=0, stage_db:configs_size() - 1 do
		msg:add_configs():CopyFrom(stage_db:configs(i))
	end
	player:SendMessage(msg)
	msg:delete()
	player:DBLog("SetOffline","","0");
	return ResultCode_ResultOK
end

msg_lua_request["GetOfflineHookData"] = function(player, packet)
	local player_hook_data = player:GetOfflineHookData()
	local exp_efficent = 0;
	local hook_data_0= player_hook_data:hook_datas(0)
	if hook_data_0:left_time() > 0 then
		exp_efficent = hook_data_0:main_actor_exp_freq() * 60
	end
	local hook_data_1= player_hook_data:hook_datas(1)
	if hook_data_1:left_time() > 0 then
		exp_efficent = hook_data_1:main_actor_exp_freq() * 60
	end
	local hook_data_2= player_hook_data:hook_datas(2)
	--如果前面的都完成了, 使用最后的默认关卡效率
	if hook_data_0:left_time() == 0 and hook_data_1:left_time() == 0 then
		exp_efficent = hook_data_2:main_actor_exp_freq() * 60
	end
	local result = {
		hook_data_0:stage_id(),hook_data_0:total_time(),hook_data_0:left_time(),
		hook_data_1:stage_id(),hook_data_1:total_time(),hook_data_1:left_time(),
		hook_data_2:stage_id(),hook_data_2:total_time(),hook_data_2:left_time(),
		exp_efficent,
	}
	player:SendLuaPacket("CurrentOfflineHookData", result, {}, {})
	return ResultCode_ResultOK
end

msg_lua_request["PauseBattle"] = function(player, packet)
	local bg = player:BattleGround()
	if bg == nil then
		return ResultCode_ResultOK
	end
	if bg:GetStatus() == BattleGroundStatus_FIGHTING or bg:GetStatus() == BattleGroundStatus_WAIT then
		bg:SetStatus(BattleGroundStatus_PAUSE)
	end
	return ResultCode_ResultOK
end

msg_lua_request["MissRewardReq"] = function(player, packet)
	if packet:int32_params_size() ~= table.nums(MissRewardReq.data_32) then
		return player:SendClientNotify("Invalid_Params",-1,-1)
	end
	local ac_type = packet:int32_params(MissRewardReq.data_32.index)	
	local times = packet:int32_params(MissRewardReq.data_32.times)	
	local type = packet:int32_params(MissRewardReq.data_32.type)	
	return x000100_DoMissReward(player,{["ac_type"]=ac_type,["times"]=times,["type"]=type})
end


msg_lua_request["ResumeBattle"] = function(player, packet)
	local bg = player:BattleGround()
	if bg == nil then
		return ResultCode_ResultOK
	end
	if bg:GetStatus() ~= BattleGroundStatus_PAUSE then
		return ResultCode_ResultOK
	end
	bg:SetStatus(BattleGroundStatus_FIGHTING)
	return ResultCode_ResultOK
end

msg_lua_request["Logout"]	=function(player, packet )
	
	if player ~= nil then
		player:OnLogout()
	end
	return ResultCode_ResultOK
end

--服务器维护
msg_lua_request["KickOut"]	=function(player, packet )
	
	if player ~= nil then
		player:SendCommonReply("Shutdown",{},{},{})
		player:OnLogout()
	end
	return ResultCode_ResultOK
end


msg_lua_request["HellStatisticsReq"] = function(player, packet)
	player:SendStatisticsResult(ActivityType.Hell,false)
	return ResultCode_ResultOK
end


msg_lua_request["ChargeItemReq"] = function(player, packet)

	local dc_container = player:DataCellContainer()
	if dc_container == nil then
		return ResultCode_Invalid_Request
	end
	
	local index = packet:int32_params(ChargeItemReq.data_32.index)
	local productid = packet:string_params(ChargeItemReq.data_string.productid)
	local channelid = packet:string_params(ChargeItemReq.data_string.channelid)

	--校验数据
	if LuaMtMarketManager():FindChargeIndex(channelid,productid) ~= index then
		return player:SendClientNotify("Invalid_Request",-1,-1)
	end
	
	local config = LuaMtMarketManager():FindChargeItem(index)
	if config == nil then
		return ResultCode_Invalid_Request
	end
    
	local price = config:rmb_price()
	if index == 0 then
		--终生卡
		if dc_container:check_bit_data(BitFlagCellIndex_LifeCardFlag) then
			return player:SendClientNotify("ac_notify_016",-1,-1)
		end        
	elseif index == 1 then
		--月卡
		if dc_container:check_bit_data(BitFlagCellIndex_MonthCardFlag) then
			return player:SendClientNotify("ac_notify_017",-1,-1)
		end    
	elseif index == 6 then
		--基金
		if dc_container:check_bit_data(BitFlagCellIndex_InvestFlag) then
			return player:SendClientNotify("ac_notify_019",-1,-1)
		end 			
		
		--提前设置基金类型，就不透传到充值成功后来处理了
		local type = packet:int32_params(ChargeItemReq.data_32.ud)
		dc_container:set_data_32(CellLogicType_AcRewardCell, g_acr_datacell_index.invest_type, type,true)
	else
		--nothing
	end
	LuaServer():LuaLog("MqGenerateOrderID begin,"..productid..","..channelid,g_log_level.LL_INFO)	
	player:MqGenerateOrderID(productid,channelid)
    
	return ResultCode_ResultOK

end

msg_lua_request["StoryStage"] = function(player, packet)
	local battle = player:BattleGround()
	if battle == nil then
		return ResultCode_Invalid_Request
	end
	local story_stage_index = packet:int32_params(StoryStage.data_32.pause_idx)
	local story_config = LuaMtMissionManager():FindStoryStage(story_stage_index)
	local mission_id = battle:GetParams32("mission_id")
	if story_config == nil or story_config:mission_id() ~= mission_id then
		print("e1:"..mission_id)
		return ResultCode_InvalidMission
	end
	local mission = player:GetMissionById(mission_id)
	if mission == nil then
		print("e2")
		return ResultCode_InvalidMission
	end
	local stage_index = battle:GetParams32("mission_stage")
	if stage_index == -1 then 
		stage_index = 0
		battle:SetParams32("mission_stage", 0)
	end
	if story_config:stage_index() ~= stage_index + 1 then
		print("e3")
		return ResultCode_InvalidMission
	end

	if story_config:event_type_1() == 1 then --进场
		if story_config:npc_id_1() > 0 then
			battle:AddMonster(story_config:npc_side_1(), story_config:npc_id_1(), story_config:npc_pos_1() - 1)
		end
		if story_config:npc_id_2() > 0 then
			battle:AddMonster(story_config:npc_side_2(), story_config:npc_id_2(), story_config:npc_pos_2() - 1)
		end
	end
	if story_config:event_type_1() == 2 then --离场
		if story_config:npc_id_1() > 0 then
			for _, actor in pairs(battle:GetAttackers()) do
				if actor:ConfigId() == story_config:npc_id_1()  then
					battle:DeleteActorNow(actor:Guid())
				end
			end
			for _, actor in pairs(battle:GetDefenders()) do
				if actor:ConfigId() == story_config:npc_id_1()  then
					battle:DeleteActorNow(actor:Guid())
				end
			end
		end
		if story_config:npc_id_2() > 0 then
			for _, actor in pairs(battle:GetAttackers()) do
				if actor:ConfigId() == story_config:npc_id_1()  then
					battle:DeleteActorNow(actor:Guid())
				end
			end
			for _, actor in pairs(battle:GetDefenders()) do
				if actor:ConfigId() == story_config:npc_id_1()  then
					battle:DeleteActorNow(actor:Guid())
				end
			end
		end
	end
	battle:SetParams32("mission_stage",story_config:stage_index())
	return ResultCode_ResultOK
end

msg_lua_request["ClearEfficentLog"] = function(player, packet)
	local player_stage_db = player:GetPlayerStageDB()
	for i = 0, player_stage_db:configs_size() - 1 do
		local config = player_stage_db:mutable_configs(i)
		if config:stage_id() > 0 and config:calc_done() == false then
			local stage_id = config:stage_id()
			local total_time = config:total_time()
			config:Clear()
			config:SetInitialized()
			config:set_stage_id(stage_id)
			config:set_total_time(total_time)
		end
	end
	player:GetPlayerStageProxy():ClearEfficentLog()
	return ResultCode_ResultOK
end

msg_lua_request["HeartBeat"] = function(player, packet)
	player:SetParams32("HeartBeat",5)
	if player:GetParams32("CheckMission")>0 then
		x000502_OnActionResolve(player,{});
		player:SetParams32("CheckMission",0)
	end
	return ResultCode_ResultOK
end

msg_lua_request["GuildDonationReq"] = function(player, packet)
	local type = packet:int32_params(0)	--GuildDonationReq.data_32.type
	local ud_config = LuaMtGuildManager():GetGuildDonateConfig(type);
	if ud_config == nil then
		return ResultCode_Invalid_Request 
	end
	
	local dc_container = player:DataCellContainer()
	if dc_container == nil then
		return ResultCode_Invalid_Request
	end
	local guildid = player:GetGuildID()
	if guildid == INVALID_GUID then
		return ResultCode_Invalid_Request;
	end

	local flag_data = dc_container:get_data_32(CellLogicType_GuildUserCell,GuildUserCellIndex_DONATION)
	if dc_container:check_bit_data_32(flag_data,type) == true then
		return player:SendClientNotify("Guild_HaveDonation",-1,-1);
	end
	local new_flag = dc_container:set_bit_data_32(flag_data,type)
	if type == 0 then
		if player:DelItemById(TokenType_Token_Gold, ud_config:cost(), ItemDelLogType_DelType_Guild,0) == false then
			return player:SendClientNotify("GoldNotEnough",-1,-1);
		end
		dc_container:set_data_32(CellLogicType_GuildUserCell,GuildUserCellIndex_DONATION,new_flag,true)
		player:AddItemByIdWithNotify(TokenType_Token_GuildMoney,ud_config:addmoney(),ItemAddLogType_AddType_Guild,0,1)
		player:AddItemByIdWithNotify(TokenType_Token_GuildContribute,ud_config:addcontribute(),ItemAddLogType_AddType_Guild,0,1)
	else
		if player:DelItemById(TokenType_Token_Crystal, ud_config:cost(), ItemDelLogType_DelType_Guild,0) == false then
			return player:SendClientNotify("CrystalNotEnough",-1,-1);
		end
		dc_container:set_data_32(CellLogicType_GuildUserCell,GuildUserCellIndex_DONATION,new_flag,true)
		player:AddItemByIdWithNotify(TokenType_Token_GuildMoney,ud_config:addmoney(),ItemAddLogType_AddType_Guild,0,1)
		player:AddItemByIdWithNotify(TokenType_Token_GuildContribute,ud_config:addcontribute(),ItemAddLogType_AddType_Guild,0,1)
	end
	
	player:OnLuaFunCall_1(501,"AddActivityTimes",ActivityType.UnionDonate)
	player:SendClientNotify("Guild_DonationOK",-1,-1);
	return ResultCode_ResultOK
end

msg_lua_request["OnBuffPointReq"] = function(player, packet)
	local posindex = packet:int32_params(OnBuffPointReq.data_32.index)	--
	if not player:ValidPosition(posindex) then
		return ResultCode_ResultOK
	end
	local raid = player:GetRaid();
	if raid ~= nil then
		raid:OnBuffPoint(player,posindex)
	end
	return ResultCode_ResultOK
end


msg_lua_request["t5_request"]		=function(player, packet )
	return player:OnLuaFunCall_n(100,"OnExchangeT5",{["value"]=-1})
end

msg_lua_request["cdkcode_request"]		=function(player, packet )
	local code = packet:string_params(cdkcode_request.data_string.code)
	player:CheckCDKCode(code);
	return ResultCode_ResultOK
end
---------------------------------------------msg handle------------------------------------
function x010000_xOnLuaRequestPacket( player, packet )
	if packet == nil then
		return ResultCode_Invalid_Request
	end
	
	--print("x010000_xOnLuaRequestPacket"..packet:request_name())
	if x000010_IsTableHasKey(msg_lua_request,packet:request_name()) then
		return  msg_lua_request[packet:request_name()]( player, packet )
	end
	return ResultCode_ResultOK
end

function x010000_xOnCommonRequestPacket( player, packet )
	if packet == nil then
		return ResultCode_Invalid_Request
	end
	
	--print("x010000_xOnCommonRequestPacket"..packet:request_name())
	if x000010_IsTableHasKey(msg_common_request,packet:request_name()) then
		return msg_common_request[packet:request_name()]( player, packet )
	end
	return ResultCode_ResultOK
end




---------------------------------player logic------------------------------


--宝石合成
function x010000_xOnCG_GemFusion(player, packet)
	print("x000010_xOnCG_GemFusion")
    --消息包内容基本检测
    local target_gem_config = LuaItemManager():GetGemConfig(packet:target_gem_id())
    if target_gem_config == nil then
        player:SendClientNotify("gem_fusion_invalid_target_gem_config", -1, -1)
    end
    if packet:bag_gem_guids_size() ~= packet:bag_gem_counts_size() 
        or packet:buy_gem_ids_size() ~= packet:buy_gem_counts_size() then
        player:SendClientNotify("gem_fusion_invalid_request", -1, -1)
        return ResultCode_ResultOK
    end
	local bag_guids = {}
    for i=0, packet:bag_gem_guids_size() - 1 do
		bag_guids[packet:bag_gem_guids(i)] = 1
        if packet:bag_gem_counts(i) < 1 or packet:bag_gem_counts(i) > 3 ^ (target_gem_config:level() - 1) then
            player:SendClientNotify("gem_fusion_invalid_request_gem_count", -1, -1)
            return ResultCode_ResultOK
        end
    end
	local buy_ids ={}
    for i=0, packet:buy_gem_counts_size() - 1 do
		buy_ids[packet:buy_gem_ids(i)] = 1
        if packet:buy_gem_counts(i) < 1 or packet:buy_gem_counts(i) > 3 ^ (target_gem_config:level() -1) then
            player:SendClientNotify("gem_fusion_invalid_request", -1, -1)
            return ResultCode_ResultOK
        end
    end
	if table.nums(buy_ids) ~= packet:buy_gem_ids_size()  or table.nums(bag_guids) ~= packet:bag_gem_guids_size() then
		player:SendClientNotify("gem_fusion_invalid_request", -1, -1)
		return
	end

    local from_equip = packet:has_equip_guid()
    local self_gem_config = nil
    local gem_config_ids = {}
    --检查装备宝石
    if packet:has_equip_guid() then
        local equip = player:FindItemByGuid(ContainerType_Layer_Equip, packet:equip_guid())
        if equip == nil then
            equip = player:FindItemByGuid(ContainerType_Layer_EquipBag, packet:equip_guid())
        end
        if equip == nil then
            player:SendClientNotify("gem_fusion_cannot_find_target_equip", -1, -1)
            return ResultCode_ResultOK
        end
        local db_info = equip:DbInfo()
        if packet:has_gem_index() == false 
            or packet:gem_index() >= db_info:gems_size() 
            or packet:gem_index() < 0 then
            player:SendClientNotify("gem_fusion_invalid_gem_index", -1, -1)
            return ResultCode_ResultOK
        end
        local gem_id = db_info:gems(packet:gem_index())
        local gem_config = LuaItemManager():GetGemConfig(gem_id)
        if gem_config == nil then
            player:SendClientNotify("gem_fusion_invalid_equip_gem_config", -1, -1)
            return ResultCode_ResultOK
        end
        gem_config_ids[gem_id] = 1;
    end
    --检查消耗的背包宝石数据
    for i=0,packet:bag_gem_guids_size()-1 do
        local gem_item = player:FindItemByGuid(ContainerType_Layer_ItemBag, packet:bag_gem_guids(i))
        if gem_item == nil or gem_item:DbInfo():count() < packet:bag_gem_counts(i) then
            player:SendClientNotify("gem_fusion_cannot_find_bag_gem", -1, -1)
            return ResultCode_ResultOK
        end
		if gem_config_ids[gem_item:Config():id()] == nil then
			gem_config_ids[gem_item:Config():id()] = 0
		end
        gem_config_ids[gem_item:Config():id()] = gem_config_ids[gem_item:Config():id()] + packet:bag_gem_counts(i)
    end
    --检查需要购买的宝石数据
    for i=0, packet:buy_gem_ids_size() - 1 do
        if LuaItemManager():GetGemConfig(packet:buy_gem_ids(i)) == nil then
            player:SendClientNotify("gem_fusion_invalid_buy_gem_config", -1, -1)
            return ResultCode_ResultOK
        end
        if LuaMtMarketManager():BuyCheck(MarketType_MarketType_Gem, packet:buy_gem_ids(i), player, packet:buy_gem_counts(i)) == false then
            player:SendClientNotify("gem_fusion_buy_gem_limit", -1, -1)
            return ResultCode_ResultOK
        end
		if gem_config_ids[packet:buy_gem_ids(i)] == nil then
			gem_config_ids[packet:buy_gem_ids(i)] = 0
		end
        gem_config_ids[packet:buy_gem_ids(i)] = gem_config_ids[packet:buy_gem_ids(i)] + packet:buy_gem_counts(i)
    end

    --检查类型
    local to_level_one_count = 0
    for gem_config_id, gem_count in pairs(gem_config_ids) do
		--print("---table----:"..tostring(gem_config_id)..","..tostring(gem_count))
        local config = LuaItemManager():GetGemConfig(gem_config_id)
        if config == nil or config:type() ~= target_gem_config:type() then
            player:SendClientNotify("gem_fusion_type_dismatch", -1, -1)
            return ResultCode_ResultOK
        end
        to_level_one_count = to_level_one_count + (3 ^ (config:level() - 1)) * gem_count
    end
    --检查数量
    local need_level_one_count = 3 ^(target_gem_config:level() - 1)
	--print("level_one:"..tostring(to_level_one_count)..",need:"..tostring(need_level_one_count))
    if to_level_one_count < need_level_one_count then
        player:SendClientNotify("gem_fusion_gem_not_enough", -1, -1)
        return ResultCode_ResultOK
    end
    if to_level_one_count > need_level_one_count then
        player:SendClientNotify("gem_fusion_gem_too_many", -1, -1)
        return ResultCode_ResultOK
    end
    
    --计算融合剂数量, N个一级宝石合成目标宝石所需融合剂数量 - 其他宝石从1级合成到当前等级用掉的融合剂数量 = 最终需要的融合剂数量
    local default_fusion_item_count = x010000_gem_fusion_item_count(target_gem_config:id())
    local sub_fusion_item_count = 0;
    local fusion_item_config_id = LuaItemManager():GetGemCostConfig(target_gem_config:level()):cost_item()
    for gem_config_id, gem_count in pairs(gem_config_ids) do
        sub_fusion_item_count = sub_fusion_item_count + x010000_gem_fusion_item_count(gem_config_id) * gem_count
    end
    local need_fusion_item_count = default_fusion_item_count - sub_fusion_item_count
    if need_fusion_item_count > 0 then
        local item = player:FindItemByConfigId(ContainerType_Layer_ItemBag, fusion_item_config_id)
        if item == nil or item:DbInfo():count() < need_fusion_item_count then
            player:SendClientNotify("gem_fusion_not_enough_fusion_item", -1, -1)
          return ResultCode_ResultOK
        end
    end
    
    --购买宝石,代币数量计算
    local need_golds = 0
    for i=0, packet:buy_gem_ids_size() - 1 do 
        local gem_id = packet:buy_gem_ids(i)
        local market_item_config = LuaMtMarketManager():FindItemByItemIdEx(MarketType_MarketType_Gem, gem_id)
        if market_item_config == nil then
            player:SendClientNotify("gem_fusion_buy_gem_market_error", -1, -1)
            return ResultCode_ResultOK
        end
		--print(market_item_config:price_type()..","..tostring(TokenType_Token_Gold))
        if market_item_config:price_type() ~= TokenType_Token_Gold then
            player:SendClientNotify("gem_fusion_bug_gem_token_type_error", -1, -1)
            return ResultCode_ResultOK
        end
        need_golds = need_golds + market_item_config:price_value() * packet:buy_gem_counts(i)
    end
	--print("need_gold:"..tostring(need_golds)..",need_fusion_item:"..tostring(need_fusion_item_count))
    --购买宝石,扣钱
	if player:DelItemById(TokenType_Token_Gold, need_golds, ItemDelLogType_DelType_Gem,0) == false then
        player:SendClientNotify("gem_fusion_not_enough_gold", -1, -1)
        return ResultCode_ResultOK
    end
    --购买宝石,增加限购次数
    for i=0, packet:buy_gem_ids_size() - 1 do
        local gem_id = packet:buy_gem_ids(i)
        local market_item_config = LuaMtMarketManager():FindItemByItemIdEx(MarketType_MarketType_Gem, gem_id)
        local market = player:FindMarket(MarketType_MarketType_Gem)
        market:UpdateItem(market_item_config:id(), packet:buy_gem_counts(i))
    end

    --删除宝石和融合剂
	--print("size:"..packet:bag_gem_guids_size())
    for i=0,packet:bag_gem_guids_size()-1 do
        local gem_item = player:FindItemByGuid(ContainerType_Layer_ItemBag, packet:bag_gem_guids(i))
		--print("gem:"..packet:bag_gem_guids(i))
		--print(gem_item)
        gem_item:DecCount(packet:bag_gem_counts(i))
    end
    player:DelItemById(fusion_item_config_id, need_fusion_item_count, ItemDelLogType_DelType_Gem, 0)

    --合成宝石
    if from_equip then
        local equip = player:FindItemByGuid(ContainerType_Layer_Equip, packet:equip_guid())
        equip:DbInfo():set_gems(packet:gem_index(), packet:target_gem_id())
        equip:RefreshData2Client()
		local actor = player:FindActor(equip:DbInfo():actor_guid())
		if actor ~= nil then
			actor:OnBattleInfoChanged(false)
		end
		player:SendCommonResult(ResultOption_Gem_Compose, ResultCode_ResultOK)
		player:SendClientNotify("gem_fusion_success", -1, -1)
    else
        player:AddItemByIdWithNotify(packet:target_gem_id(), 1, ItemAddLogType_AddType_Gem, 0, 0)
		player:SendCommonResult(ResultOption_Gem_Compose, ResultCode_ResultOK)
		player:SendClientNotify("gem_fusion_success", -1, -1)
    end
	
end


--计算宝石融合剂数量
function x010000_gem_fusion_item_count(gem_id)
    local total_need = 0;
    local config = LuaItemManager():GetGemConfig(gem_id)  
	local self_level = config:level()
    for level=config:level(),1,-1 do
         total_need = total_need + LuaItemManager():GetGemCostConfig(level):cost_count() * (3 ^ (self_level - level))
    end
    return total_need
end
