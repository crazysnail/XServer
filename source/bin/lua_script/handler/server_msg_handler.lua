function x010005_xOnCG_UpdateStrongHold(player, packet)
	local guild = LuaMtGuildManager():GetGuildByid(player:GetGuildID());
	if guild == nil then
		player:SendClientNotify("Guild_NoExist", -1, -1)
		return
	end
	if player:GetPlayerGuildProxy():GetGuildPosition() ~=  GuildPosition_CHIEFTAIN
		and player:GetPlayerGuildProxy():GetGuildPosition() ~= GuildPosition_VICE_CHIEFTAIN then
		player:SendClientNotify("capture_member_limite_tip", -1, -1)
		return
	end
	if packet:strong_holds_size() ~= 5 then
		player:SendClientNotify("capture_point_strong_holds_data_wrong", -1, -1)
		return
	end

	if packet:strong_holds_size() ~= 5 then
		player:SendClientNotify("capture_point_strong_holds_hero_data_wrong", -1, -1)
		return
	end
	local hero_guid = {}
	local client_guid_count = 0
	for i=0,packet:strong_holds_size() - 1 do
		local hold = packet:strong_holds(i)
		if hold:heros_size() ~= 5 then
			player:SendClientNotify("capture_point_strong_holds_hero_data_wrong", -1, -1)
			return
		end
		for j=0,hold:heros_size()-  1 do
			local guid = hold:heros(j):player_guid()
			if guid ~= INVALID_GUID then
				hero_guid[guid] = 1
				client_guid_count = client_guid_count + 1
			end
		end
	end
	local unique_guid_count = 0
	for _,_ in pairs(hero_guid) do
		unique_guid_count = unique_guid_count + 1
	end
	if unique_guid_count ~= client_guid_count then
		player:SendClientNotify("capture_point_strong_hold_duplicate_hero", -1, -1)
		return
	end
	local relate_cp = LuaMtCapturePointManager():RelateCapturePoint(player:GetGuildID())
	if relate_cp ~= nil and relate_cp:status() ~= CaptureStatus_CS_INITED then
		player:SendClientNotify("capture_point_update_strong_hold_fail_wrong_status", -1, -1)
		return
	end
	LuaMtCapturePointManager():UpdateStrongHold(guild:GetGuildId(), packet) 
end

function x010005_xOnCG_ReleaseCapturePoint(player, packet)
	--发起方权限检查
	local self_guild = LuaMtGuildManager():GetGuildByid(player:GetGuildID());
	if self_guild == nil then
		player:SendClientNotify("Guild_NoExist", -1, -1)
		return
	end
	if player:GetPlayerGuildProxy():GetGuildPosition() ~=  GuildPosition_CHIEFTAIN
		and player:GetPlayerGuildProxy():GetGuildPosition() ~= GuildPosition_VICE_CHIEFTAIN then
		player:SendClientNotify("capture_member_limite_tip", -1, -1)
		return
	end
	local relate_cp = LuaMtCapturePointManager():RelateCapturePoint(player:GetGuildID())
	local self_capture_point = LuaMtCapturePointManager():FindCapturePoint(player:GetGuildID())
	if relate_cp == nil or relate_cp:capture_guild_guid() ~= player:GetGuildID() then
		player:SendClientNotify("capture_point_release_fail_status_wrong", -1, -1)
		return
	end
	if relate_cp:status() ~= CaptureStatus_CS_INITED and relate_cp:status() ~= CaptureStatus_CS_PROTECT then
		player:SendClientNotify("capture_point_release_fail_status_wrong", -1, -1)
		return
	end
	x001025_OnCapturePointInit(relate_cp:chapter_id(), relate_cp)
	x001025_SyncCapturePointToGuild(player:GetGuildID(), 0, relate_cp:chapter_id())
end

--发起战斗
--做一些基础检查之后, 设置自己的领土争夺状态为load_data,创建db_loader加载数据
--数据加载完成后会进行回调,开始战斗
function x010005_xOnCG_StartChallengeCapturePoint(player, packet)
	--发起方权限检查
	local self_guild = LuaMtGuildManager():GetGuildByid(player:GetGuildID());
	if self_guild == nil then
		player:SendClientNotify("Guild_NoExist", -1, -1)
		return
	end
	if player:GetPlayerGuildProxy():GetGuildPosition() ~=  GuildPosition_CHIEFTAIN
		and player:GetPlayerGuildProxy():GetGuildPosition() ~= GuildPosition_VICE_CHIEFTAIN then
		player:SendClientNotify("capture_member_limite_tip", -1, -1)
		return
	end
	local chapter_id = packet:chapter_id()
	local target_guild_guid  = packet:target_guild_guid()
	local self_cp = LuaMtCapturePointManager():FindGuildCapturePoint(player:GetGuildID())
	local relate_cp = LuaMtCapturePointManager():RelateCapturePoint(player:GetGuildID())
	if self_cp == nil then
		player:SendClientNotify("capture_point_need_init", -1, -1)
		return
	end
	
	if relate_cp ~= nil then
		if relate_cp:capture_guild_guid() == player:GetGuildID() then
			player:SendClientNotify("capture_point_need_release_point_first", -1, -1)--需要先释放当前占领的点
			return
		end
		if relate_cp:challenger_guild_guid() == player:GetGuildID() then
			player:SendClientNotify("capture_point_in_battle", -1, -1)  --当前正在进行领土争夺
			return
		end
	end

	--每个据点都必须配置英雄才能开战
	--[[for i = 0, self_capture_point:strong_holds_size() - 1 do
		local strong_hold = self_capture_point:strong_holds(i)
		local no_data = -1
		for j = 0, strong_hold:heros_size() - 1 do
			if strong_hold:heros(j):actor_config_id() > 0 then
				no_data = j
			end
		end
		if no_data == -1 then
			player:SendClientNotify("capture_point_strong_hold_need_set|"..tostring(i+1), -1, -1)
			return
		end
	end]]--
	local target_cp = LuaMtCapturePointManager():FindCapturePoint(chapter_id)
	if target_cp == nil then
		player:SendClientNotify("capture_point_invalid_config", -1, -1) --找不到目标
		return
	end
	if target_cp:capture_guild_guid() ~= target_guild_guid 
		or target_cp:status() ~= CaptureStatus_CS_INITED then
		player:SendClientNotify("capture_point_target_capture_point_wrong_status", -1, -1)
		return
	end
	local target_guild_cp = LuaMtCapturePointManager():FindGuildCapturePoint(target_cp:capture_guild_guid())
	local guids = {}
	for i = 0, self_cp:strong_holds_size() - 1 do
		local strong_hold = self_cp:strong_holds(i)
		for j = 0, strong_hold:heros_size() - 1 do
			guids[strong_hold:heros(j):player_guid()] = 1
		end
	end
	if target_guild_cp ~= nil then
		for i = 0, target_guild_cp:strong_holds_size() - 1 do
			local strong_hold = target_guild_cp:strong_holds(i)
			for j = 0, strong_hold:heros_size() - 1 do
				guids[strong_hold:heros(j):player_guid()] = 1
			end
		end
	end
	local player_guids = {}
	for k,_ in pairs(guids) do
		player_guids[k] = k
	end
	local attacker_guid = self_cp:guid()
	local defender_guid = 0
	if target_guild_cp ~= nil then defender_guid = target_guild_cp:guid() end
	LuaMtCapturePointManager():LoadPlayerAndActor(attacker_guid, defender_guid, chapter_id, player_guids)
	player:SendClientNotify("capture_point_load_battle_data_start", -1, -1)
end

--查看当前公会的战斗日志
function x010005_xOnCG_AllBattleLog(player, packet)
	local self_guild_guid = player:GetGuildID()
	if self_guild_guid == 0 then
		player:SendClientNotify("Guild_NoExist", -1, -1)
		return 
	end
	local ret_msg = GC_GuildCaptureAllBattleLog:new()
	local log_ids = LuaMtCapturePointManager():FindGuildBattleLog(self_guild_guid)
	for _, log_id in pairs(log_ids) do
		local log_ = LuaMtCapturePointManager():FindBattleLog(log_id)
		if log_ ~= nil and log_:finish() == true then
			local msg_log  = ret_msg:add_logs()
			msg_log:set_attacker_guid(log_:attacker_guid())
			msg_log:set_defender_guid(log_:defender_guid())
			msg_log:set_attacker_name("")
			msg_log:set_attacker_icon("")
			msg_log:set_defender_name("")
			msg_log:set_defender_icon("")
			msg_log:set_attacker_win(log_:attacker_win())
			msg_log:set_battle_time(log_:battle_time())
			msg_log:set_chapter_id(log_:chapter_id())
			local attacker_guild = LuaMtGuildManager():GetGuildByid(log_:attacker_guid())
			if attacker_guild ~= nil then
				msg_log:set_attacker_name(attacker_guild:GetDBInfo():name())
				msg_log:set_attacker_icon(attacker_guild:GetDBInfo():icon())
			end
			local defender_guild = LuaMtGuildManager():GetGuildByid(log_:defender_guid())
			if defender_guild ~= nil then
				msg_log:set_defender_name(defender_guild:GetDBInfo():name())
				msg_log:set_defender_icon(defender_guild:GetDBInfo():icon())
			end
			for i = 0, log_:details_size() - 1 do
				local detail = log_:details(i)
				local client_detail = msg_log:add_details()
				client_detail:CopyFrom(detail)
			end					
		end
	end
	player:SendMessage(ret_msg)
	ret_msg:delete()
end

function x010005_fill_self_cp_info(guild_guid,ret_msg)
	ret_msg:Clear()
	ret_msg:SetInitialized()
	local capture_point_data = LuaMtCapturePointManager():FindGuildCapturePoint(guild_guid)
	if capture_point_data == nil then
		for _ = 0,4 do
			local strong_hold = ret_msg:add_strong_holds()
			strong_hold:set_score(0)
			for _ = 0,4 do
				local hero = strong_hold:add_heros()
				hero:SetInitialized()
			end
		end
	else
		for i = 0, capture_point_data:strong_holds_size() - 1 do
			ret_msg:add_strong_holds():CopyFrom(capture_point_data:strong_holds(i))
		end
		local relate_cp = LuaMtCapturePointManager():RelateCapturePoint(guild_guid)
		if relate_cp ~= nil then
			local chapter_id = relate_cp:chapter_id()
			if relate_cp:capture_guild_guid() == guild_guid then
				ret_msg:set_chapter_id(chapter_id)
			end
			ret_msg:set_hook_users(LuaMtCapturePointManager():GetHookUserCount(chapter_id)) 
			local scene_stage = LuaMtSceneStageManager():GetGroupSceneStageByGroupId(chapter_id)
			if scene_stage ~= nil then
				ret_msg:set_tax(scene_stage:guild_tax())
				ret_msg:set_tax_rate(scene_stage:guild_tax_rate())
			end
		end
	end
end

--查看自己公会的领土争夺配置数据
function x010005_xOnCG_PullCapturePoint(player, packet)
	--功能解锁
	if player:OnLuaFunCall_n(100,"CheckFunction", { ["guanqiazhanling"]=999} ) ~= ResultCode_ResultOK then
		
		player:SendClientNotify("functionunlock", -1, -1)
		return
	end
	local guild = LuaMtGuildManager():GetGuildByid(player:GetGuildID());
	if guild == nil then
		player:SendClientNotify("Guild_NoExist", -1, -1)
		return
	end
	
	local ret_msg = GC_GuildCapturePoint:new()
	x010005_fill_self_cp_info(player:GetGuildID(), ret_msg)
	player:SendMessage(ret_msg)
	ret_msg:delete()
end

function x010005_fill_brief_info(brief, chapter_id)
	local cp = LuaMtCapturePointManager():FindCapturePoint(chapter_id)
	local scene_stage = LuaMtSceneStageManager():GetGroupSceneStageByGroupId(chapter_id)
	brief:SetInitialized()
	brief:set_chapter_id(chapter_id)
	brief:set_hook_users(LuaMtCapturePointManager():GetHookUserCount(chapter_id))
	
	if scene_stage ~= nil then
		brief:set_tax(scene_stage:guild_tax())
		brief:set_tax_rate(scene_stage:guild_tax_rate())	
	end
	brief:set_status(cp:status())
	local elapse = LuaMtTimerManager():CurrentTime() / 1000 - cp:status_change_time() - 1
	if cp:status() == CaptureStatus_CS_DECLARE_WAR  then
		brief:set_status_change_time(g_DECLARE_WAR_WAIT_TIME - elapse)
	end
	if cp:status() == CaptureStatus_CS_FIGHTING then
		brief:set_status_change_time(g_FIGHTING_TIME_OUT - elapse)
	end
	if cp:status() == CaptureStatus_CS_PROTECT then
		brief:set_status_change_time(g_PROTECT_TIME - elapse)
	end
	brief:set_capture_guild_guid(cp:capture_guild_guid())
	--brief:set_challenger_guild_guid(cp:challenger_guild_guid())
	local attacker_guild = LuaMtGuildManager():GetGuildByid(cp:challenger_guild_guid())
	local defender_guild = LuaMtGuildManager():GetGuildByid(cp:capture_guild_guid())
	if attacker_guild ~= nil then
		brief:set_challenger_name(attacker_guild:GetDBInfo():name())
		brief:set_challenger_icon(attacker_guild:GetDBInfo():icon())
	end
	if defender_guild ~=  nil then
		brief:set_capture_guild_name(defender_guild:GetDBInfo():name())
		brief:set_capture_guild_icon(defender_guild:GetDBInfo():icon())
	end
	for i=0,cp:capture_hero_datas_size() - 1 do
		brief:add_capture_hero_datas():CopyFrom(cp:capture_hero_datas(i))
		brief:add_challenger_hero_datas():CopyFrom(cp:challenger_hero_datas(i))
		brief:add_strong_hold_status(cp:strong_hold_status(i))
	end
end

--获取目标领土的公开信息
function x010005_xOnCG_CetCapturePointInfo(player, packet)
	local chapter_id = packet:chapter_id()
	local brief = CapturePotinBrief:new()
	x010005_fill_brief_info(brief, chapter_id)
	player:SendMessage(brief)
	brief:delete()
end

--设置税率
function x010005_xOnCG_UpdateTaxRate(player, packet)
	--功能解锁
	if player:OnLuaFunCall_n(100,"CheckFunction", { ["guanqiazhanling"]=999} ) ~= ResultCode_ResultOK then
		player:SendClientNotify("functionunlock", -1, -1)
		return
	end
	local guild = LuaMtGuildManager():GetGuildByid(player:GetGuildID());
	if guild == nil then
		player:SendClientNotify("Guild_NoExist", -1, -1)
		return
	end
	if player:GetPlayerGuildProxy():GetGuildPosition() ~=  GuildPosition_CHIEFTAIN
		and player:GetPlayerGuildProxy():GetGuildPosition() ~= GuildPosition_VICE_CHIEFTAIN then
		player:SendClientNotify("capture_member_limite_tip", -1, -1)
		return
	end
	
	if packet:rate() ~= 1 and packet:rate() ~= 0 then
		player:SendClientNotify("capture_point_wrong_rate_value|1", -1, -1)
		return
	end
	local relate_cp = LuaMtCapturePointManager():RelateCapturePoint(guild:GetGuildId())
	if relate_cp == nil or relate_cp:capture_guild_guid() ~= guild:GetGuildId() then
		player:SendClientNotify("Invalid_Request", -1, -1)
		return
	end
	local scene_stage = LuaMtSceneStageManager():GetGroupSceneStageByGroupId(relate_cp:chapter_id())
	if scene_stage ~= nil then
		scene_stage:set_guild_tax_rate(packet:rate())
		player:SendClientNotify("capture_point_set_tax_rate_ok", -1, -1)
		local rate = math.floor(packet:rate())
		if rate > 0 then
			Lua_send_run_massage("ac_notify_044|"..guild:GetDBInfo():name().."|capture_point_name_"..math.floor(relate_cp:chapter_id()).."|"..rate)
		end
	end
	x001025_SyncCapturePointToGuild(player:GetGuildID(),0,relate_cp:chapter_id())
end

--收取税金
function x010005_xOnCG_CollectGuildTax(player, packet)
	local guild = LuaMtGuildManager():GetGuildByid(player:GetGuildID());
	if guild == nil then
		player:SendClientNotify("Guild_NoExist", -1, -1)
		return
	end
	if player:GetPlayerGuildProxy():GetGuildPosition() ~=  GuildPosition_CHIEFTAIN
		and player:GetPlayerGuildProxy():GetGuildPosition() ~= GuildPosition_VICE_CHIEFTAIN then
		player:SendClientNotify("capture_member_limite_tip", -1, -1)
		return
	end
	--功能解锁
	if player:OnLuaFunCall_n(100,"CheckFunction", { ["guanqiazhanling"]=999} ) ~= ResultCode_ResultOK then
		player:SendClientNotify("functionunlock", -1, -1)
		return
	end
	
	local relate_cp = LuaMtCapturePointManager():RelateCapturePoint(guild:GetGuildId())
	if relate_cp == nil or relate_cp:capture_guild_guid() ~= guild:GetGuildId()  then
		player:SendClientNotify("Invalid_Request", -1, -1)
		return
	end
	local scene_stage = LuaMtSceneStageManager():GetGroupSceneStageByGroupId(relate_cp:chapter_id())
	local stage_config = LuaMtSceneStageManager():GetConfig(scene_stage:stage_id())
	if scene_stage ~= nil and stage_config ~= nil then
		if scene_stage:guild_tax() > 0 then
			local tax = math.floor(scene_stage:guild_tax())
			guild:AddGuildMoney(tax, player)
			player:SendClientNotify("capture_point_tax_collect_ok", -1, -1)
			scene_stage:set_guild_tax(0)
			scene_stage:set_guild_tax_broadcasted(false)
			if tax >= stage_config:guild_tax_broadcat_mark() then
				Lua_send_run_massage("ac_notify_045|"..guild:GetDBInfo():name()
					.."|capture_point_name_"..math.floor(relate_cp:chapter_id())
					.."|"..LuaItemManager():GetItemName(TokenType_Token_GuildMoney)
					.."|"..tax)
			end
		end
	end
	x001025_SyncCapturePointToGuild(player:GetGuildID(),0,relate_cp:chapter_id())
end