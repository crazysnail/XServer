--公会占点

function x001025_OnCreateReady(battle_ground, player)
	local chapter_id = battle_ground:GetParams64("chapter_id")
	local strong_hold_id = battle_ground:GetParams32("strong_hold_id")
	local cp = LuaMtCapturePointManager():FindCapturePoint(chapter_id)
	if cp == nil then return ResultCode_InvalidConfigData end
	local attacker_cp = LuaMtCapturePointManager():FindGuildCapturePoint(cp:challenger_guild_guid())
	local defender_cp = LuaMtCapturePointManager():FindGuildCapturePoint(cp:capture_guild_guid())
	
	local attacker_strong_hold = attacker_cp:strong_holds(strong_hold_id)
	for i=0, attacker_strong_hold:heros_size() - 1 do
		local player_guid = attacker_strong_hold:heros(i):player_guid()
		if player_guid ~= INVALID_GUID then
			local player = LuaServer():FindPlayer(player_guid)
			if player ~= nil then
				battle_ground:InitAttackerFromActorWithoutPlayer(player:MainActor())
			end
		end
	end
	local battle_log = LuaMtCapturePointManager():FindBattleLog(cp:log_guid())
	local detail = battle_log:mutable_details(strong_hold_id)
	
	if defender_cp == nil then--攻打boss
		local boss_group_id = LuaMtCapturePointManager():CapturePointBossGroupId(chapter_id,strong_hold_id)
		local monster_group_config = LuaMtMonsterManager():FindMonsterGroup(boss_group_id)
		local infos={}
		local monster_count = 0
		for i = 0,monster_group_config:monsters_size() - 1 do
			local monster_define = monster_group_config:monsters(i)
			if (monster_define:id() > 0) then
				infos[monster_define:loc()]=monster_define:id()
				local monster_config = LuaMtMonsterManager():FindMonster(monster_define:id())
				if monster_config ~= nil and monster_count < 5 then
					local monster_info = detail:mutable_defender_heros(monster_count)
					monster_info:set_player_guid(0)
					monster_info:set_actor_config_id(monster_define:id())
					monster_info:set_level(monster_config:level())
					monster_info:set_color(1)
					monster_info:set_star(1)
					monster_info:set_hair(0)
					monster_count = monster_count + 1
				end
			end
		end
		for i = 0, attacker_strong_hold:heros_size() - 1 do
			detail:mutable_attacker_heros(i):CopyFrom(attacker_strong_hold:heros(i))
		end
		battle_ground:CreateDefenderMonster(infos)	
		
	else --打其他公会
		local defender_strong_hold = defender_cp:mutable_strong_holds(strong_hold_id)
		for i = 0, attacker_strong_hold:heros_size() - 1 do
			detail:mutable_attacker_heros(i):CopyFrom(attacker_strong_hold:heros(i))
			detail:mutable_defender_heros(i):CopyFrom(defender_strong_hold:heros(i))
		end
		local defender_strong_hold = defender_cp:strong_holds(strong_hold_id)
		for i=0, defender_strong_hold:heros_size() - 1 do
			local player_guid = defender_strong_hold:heros(i):player_guid()
			local player = LuaServer():FindPlayer(player_guid)
			if player ~= nil then
				battle_ground:InitDefenderFromActorWithoutPlayer(player:MainActor())
			end
		end
	end
	return ResultCode_ResultOK;	
end

function x001025_OnCheckBattleEnd(battle_ground)
	if battle_ground:TimeOut() then
		return 2;
	end
	local DeadCampID=battle_ground:GetAllDeadCampID();
	if DeadCampID == 1 then
		return 2;
	elseif DeadCampID == 2 then
		return 1;
	end
	return 0;
end

function x001025_OnDamageRefix(battle_ground,damage_info,killer)

end

function x001025_OnDamage(battle, attacker, source_guid, target_guid, damage)
	return 0;
end


function x001025_OnBattleReward(battle_ground, winnercamp)		
	return true
end

function x001025_OnBattleEnd(battle_ground,force, winnercamp)	
	--通知客户端结束战斗
	battle_ground:BattleOverNotify(winnercamp)
	local attacker_win = false
	if winnercamp == 1 then
		attacker_win = true
	end
	battle_ground:AddStat2Reply() 
	battle_ground:SaveBattleReply()
	local chapter_id = battle_ground:GetParams64("chapter_id")
	local strong_hold_id = battle_ground:GetParams32("strong_hold_id")
	local relate_cp = LuaMtCapturePointManager():FindCapturePoint(chapter_id)
	if relate_cp == nil then
		return true
	end
	local log_guid = relate_cp:log_guid()
	local battle_log = LuaMtCapturePointManager():FindBattleLog(relate_cp:log_guid())
	if battle_log == nil then
		return true
	end
	local attacker_cp = LuaMtCapturePointManager():FindGuildCapturePoint(relate_cp:challenger_guild_guid())
	local defender_cp = LuaMtCapturePointManager():FindGuildCapturePoint(relate_cp:capture_guild_guid())

	local reply = battle_ground:GetBattleReply()
	local detail = battle_log:mutable_details(strong_hold_id)
	
	local attackers = battle_ground:GetAttackers()
	local defenders = battle_ground:GetDefenders()
	if #attackers == 0 then
		relate_cp:set_strong_hold_status(strong_hold_id,StrongHoldStatus_SHS_DEFENDER_WIN_BYE)
	elseif #defenders == 0 then
		relate_cp:set_strong_hold_status(strong_hold_id,StrongHoldStatus_SHS_ATTACKER_WIN_BYE)
	elseif attacker_win then
		relate_cp:set_strong_hold_status(strong_hold_id,StrongHoldStatus_SHS_ATTACKER_WIN)
	else
		relate_cp:set_strong_hold_status(strong_hold_id,StrongHoldStatus_SHS_DEFENDER_WIN)
	end
	detail:set_attacker_win(attacker_win)
	detail:set_reply_guid(reply:guid())
	x001025_OnCaptureBattleCheckAllEnd(relate_cp, strong_hold_id, attacker_win)
	return true
end

function x001025_SendGuildChieftainMsg(chieftain, msg)
	if chieftain ~= nil then
		chieftain:SendClientNotify(msg, -1, -1)
	end
end
--数据加载完毕的回调
function x001025_OnLoadPlayerAndActorDone(attacker_guid, defender_guid, chapter_id, load_success)
	print("x001025_OnLoadPlayerAndActorDone load data:"..tostring(load_success))
	local attacker_guild = LuaMtGuildManager():GetGuildByid(attacker_guid)
	local defender_guild = LuaMtGuildManager():GetGuildByid(defender_guid)
	local attacker_cp = LuaMtCapturePointManager():FindGuildCapturePoint(attacker_guid)
	local defender_cp = LuaMtCapturePointManager():FindGuildCapturePoint(defender_guid)
	local relate_cp = LuaMtCapturePointManager():FindCapturePoint(chapter_id)
	local chieftain = LuaServer():FindPlayer(attacker_guild:ChieftainGuid())
	if attacker_cp == nil or relate_cp == nil then
		x001025_SendGuildChieftainMsg(chieftain, "capture_point_load_battle_data_fail")
		return
	end
	if relate_cp:status() ~= CaptureStatus_CS_INITED then
		x001025_SendGuildChieftainMsg(chieftain, "capture_potin_attacker_status_wrong")
		LuaServer():LuaLog("[capture_point] pvp start fail, challenger:"..attacker_cp:guid()..",status:"..relate_cp:status(),g_log_level.LL_INFO)
		return
	end
	if load_success == false then
		x001025_SendGuildChieftainMsg(chieftain, "capture_point_load_battle_data_fail")
		LuaServer():LuaLog("[capture_point] pvp start fail, challenger"..attacker_cp:guid()..",load data fail",g_log_level.LL_INFO)
		return
	end
	relate_cp:set_status(CaptureStatus_CS_DECLARE_WAR)
	relate_cp:set_status_change_time(LuaMtTimerManager():CurrentTime() / 1000)
	relate_cp:set_challenger_guild_guid(attacker_guid)
	attacker_guild:AddGuildMoney(-20000, chieftain)
	for i=0,attacker_cp:strong_holds_size() - 1 do
		relate_cp:mutable_challenger_hero_datas(i):CopyFrom(attacker_cp:strong_holds(i):heros(0))
	end
	--[[local battle_log = LuaMtCapturePointManager():CreateBattleLog()
	battle_log:set_attacker_guid(attacker_guid)
	battle_log:set_defender_guid(defender_guid)
	battle_log:set_chapter_id(chapter_id)
	battle_log:set_battle_time(LuaMtTimerManager():CurrentTime() / 1000)
	local log_guid = battle_log:guid()]]--
	LuaServer():LuaLog("[capture_point] pvp battle declare "..tostring(attacker_guid).." vs " .. tostring(defender_guid),g_log_level.LL_INFO)
	if defender_cp ~= nil then		-- 打别人的公会
		local content = "capture_point_be_attack_content|"..attacker_guild:GetDBInfo():name().."|".."capture_point_name_"..math.floor(chapter_id)
		lua_static_message_manager():create_message(defender_guild:ChieftainGuid(),99, "capture_point_mail_title",content,{})
	end
	x001025_SyncCapturePointToGuild(attacker_guid, defender_guid, chapter_id)
end

function x001025_OnCaptureBattleStart(cp, battle_index)

	local log_guid = cp:log_guid()
	local battle_log = LuaMtCapturePointManager():FindBattleLog(log_guid)
	local attack_cp = LuaMtCapturePointManager():FindGuildCapturePoint(cp:challenger_guild_guid())
	local scene_stage = LuaMtSceneStageManager():GetGroupSceneStageByGroupId(cp:chapter_id())
	local stage_config = LuaMtSceneStageManager():GetConfig(scene_stage:stage_id())
	local scene_id = stage_config:scene_id()
	local battle_ground = LuaServer():GetBattleManager():CreatePureBattle()
	if battle_ground ~= nil then
		local breif = battle_ground:Brief()
		breif:set_scene_id(scene_id)
		breif:set_battle_group_id(1)
		breif:set_time_out(60*3*1000)
		breif:set_type(BattleGroundType_PVP_GUILD_CAPTURE_POINT)
		battle_ground:SetParams64("chapter_id",cp:chapter_id())
		battle_ground:SetScriptId(1025)
		battle_ground:SetParams32("strong_hold_id",battle_index)
		battle_ground:SetClientInsight(false)
		
		if battle_ground:Init() == false then
			LuaServer():LuaLog("[capture_point BattleStart] chapter:"..cp:chapter_id()..",log:"..log_guid..",index:"..battle_index..",init fails",g_log_level.LL_INFO)
			return ResultCode_BattleCreateFailed
		end
		battle_ground:CreateBattleReply()
		battle_ground:BattleEnterNotify()
		battle_ground:OnPlayerReachPos()
		cp:set_strong_hold_status(battle_index, StrongHoldStatus_SHS_FIGHTING)
		LuaServer():LuaLog("[capture_point BattleStart] chapter:"..cp:chapter_id()..",log:"..log_guid..",index:"..battle_index..".init ok",g_log_level.LL_INFO)
	else
		LuaServer():LuaLog("[capture_point BattleStart] chapter:"..cp:chapter_id()..",log:"..log_guid..",index:"..battle_index..".create fails",g_log_level.LL_INFO)
		local attacker_guild = LuaMtGuildManager():GetGuildByid(cp:challenger_guild_guid())
		if attacker_guild ~= nil then
			local chieftain = LuaServer():FindPlayer(attacker_guild:ChieftainGuid())
			x001025_SendGuildChieftainMsg(chieftain, "capture_point_create_battle_fail")
		end
	end
end

function x001025_OnCaptureBattleCheckAllEnd(relate_cp, strong_hold_id, win)
	local log_guid = relate_cp:log_guid()
	local chapter_id = relate_cp:chapter_id()
	LuaServer():LuaLog("[capture_point CheckAllEnd] chapter:"..chapter_id..",log_guid:"..log_guid..",strong_hold_id:"..strong_hold_id..",win:"..tostring(win),g_log_level.LL_INFO)
	local battle_log = LuaMtCapturePointManager():FindBattleLog(log_guid)
	if battle_log == nil then
		LuaServer():LuaLog("[capture_point CheckAllEnd] chapter:"..chapter_id..",log_guid:"..log_guid..",strong_hold_id:"..strong_hold_id..",no log",g_log_level.LL_INFO)
		return
	end
	if battle_log:finish() == true then
		LuaServer():LuaLog("[capture_point CheckAllEnd] chapter:"..chapter_id..",log_guid:"..log_guid..",strong_hold_id:"..strong_hold_id..",already finished",g_log_level.LL_INFO)
		return
	end
	local detail = battle_log:mutable_details(strong_hold_id)
	detail:set_attacker_win(win)
	local battle_finish = true
	local win_count = 0
	for i = 0, battle_log:details_size() - 1  do
		local detail = battle_log:details(i)
		if detail:attacker_win() then
			win_count = win_count + 1
		end
		if detail:reply_guid() == 0 then 
			battle_finish = false 
		end
	end

	if battle_finish ==  true then
		local final_win = false
		if relate_cp:capture_guild_guid() == 0 then --打boss
			if win_count == 5 then final_win = true end
		else --打别人的公会
			if win_count >= 3 then final_win = true end
		end
		battle_log:set_attacker_win(final_win)
		battle_log:set_finish(true)
		if final_win then
			relate_cp:set_winner_guid(relate_cp:challenger_guild_guid())
		else
			relate_cp:set_winner_guid(relate_cp:capture_guild_guid())
		end
		x001025_OnCapturePointBattleOver(relate_cp)
	end
	x001025_SyncCapturePointToGuild(relate_cp:capture_guild_guid(), relate_cp:challenger_guild_guid(), chapter_id)
	--x001025_OnCapturePointPushData(battle_log)
end

function x001025_OnCapturePointBattleOver(cp)
	local log_guid = cp:log_guid()
	local chapter_id = cp:chapter_id()

	local attacker_cp = LuaMtCapturePointManager():FindGuildCapturePoint(cp:challenger_guild_guid())
	local defender_cp = LuaMtCapturePointManager():FindGuildCapturePoint(cp:capture_guild_guid())
	if cp:status() ~= CaptureStatus_CS_FIGHTING then
		LuaServer():LuaLog("[capture_point BattleOver] status not in battle return.status:"..cp:status()..",chapter:"..cp:chapter_id()..",log:"..log_guid,g_log_level.LL_INFO)
		--战斗已经结束了, 无法继续
		return
	end
	local attacker_guild = LuaMtGuildManager():GetGuildByid(cp:challenger_guild_guid())
	local defender_guild = LuaMtGuildManager():GetGuildByid(cp:capture_guild_guid())
	cp:set_status(CaptureStatus_CS_PROTECT)
	cp:set_status_change_time(LuaMtTimerManager():CurrentTime() / 1000)
	LuaServer():LuaLog("[capture_point BattleOver] chapter:"..cp:chapter_id()..",winner:"..cp:winner_guid()..",log:"..cp:log_guid(),g_log_level.LL_INFO)
	if cp:winner_guid() == cp:challenger_guild_guid() then
	--攻击方胜利, 占领据点, 复制挑战数据到防守数据
		cp:set_capture_guild_guid(cp:winner_guid())
		for i=0,cp:capture_hero_datas_size() - 1 do
			local hero = cp:mutable_capture_hero_datas(i)
			hero:CopyFrom(cp:challenger_hero_datas(i))
		end
		if attacker_guild ~= nil then
			local content = "capture_point_attack_result_win_content|capture_point_name_"..math.floor(chapter_id)
			lua_static_message_manager():create_message(attacker_guild:ChieftainGuid(),99, "capture_point_mail_title",content,{})
		end	
		if defender_guild ~= nil then
			local content = "capture_point_defence_result_lose_content|capture_point_name_"..math.floor(chapter_id)
			lua_static_message_manager():create_message(defender_guild:ChieftainGuid(),99, "capture_point_mail_title",content,{})
		end			
	else
	--防守方胜利, 发发邮件
		if attacker_guild ~= nil then
			local content = "capture_point_attack_result_lose_content|capture_point_name_"..math.floor(chapter_id)
			lua_static_message_manager():create_message(attacker_guild:ChieftainGuid(),99, "capture_point_mail_title",content,{})
		end
		if defender_guild ~= nil then
			local content = "capture_point_defence_result_win_content|capture_point_name_"..math.floor(chapter_id)
			lua_static_message_manager():create_message(defender_guild:ChieftainGuid(),99, "capture_point_mail_title",content,{})
		end	
	end
	x001025_SyncCapturePointToGuild(cp:challenger_guild_guid(), cp:capture_guild_guid(), chapter_id)
	--清除临时数据
	cp:set_challenger_guild_guid(0)
	for i=0, cp:challenger_hero_datas_size() - 1 do
		local hero = cp:mutable_challenger_hero_datas(i)
		hero:Clear()
		hero:SetInitialized()
	end
	cp:set_winner_guid(0)
	cp:set_log_guid(0)
	for i=0,cp:strong_hold_status_size() - 1 do
		cp:set_strong_hold_status(i, StrongHoldStatus_SHS_IDLE)
	end
	
	--[[if attacker_guild ~= nil and LuaServer():FindPlayer(attacker_guild:ChieftainGuid()) ~= nil then
		x010005_xOnCG_AllBattleLog(LuaServer():FindPlayer(attacker_guild:ChieftainGuid()),nil)
	end
	if defender_guild ~= nil and LuaServer():FindPlayer(defender_guild:ChieftainGuid()) ~= nil then
		x010005_xOnCG_AllBattleLog(LuaServer():FindPlayer(defender_guild:ChieftainGuid()),nil)
	end]]--
end

function x001025_OnBigTick(battle_log, force)--在tick中检查一下超时
	--[[if (LuaMtTimerManager():CurrentTime() / 1000) - battle_log:battle_time() > 60*10 or force then
		if battle_log:finish() == false then
			local attacker_guild = LuaMtGuildManager():GetGuildByid(battle_log:attacker_guid())
			local chieftain = LuaServer():FindPlayer(attacker_guild:ChieftainGuid())
			local attacker_cp = LuaMtCapturePointManager():FindCapturePoint(battle_log:attacker_guid())
			local defender_cp = LuaMtCapturePointManager():FindCapturePoint(battle_log:defender_guid())
			battle_log:set_finish(true)
			battle_log:set_attacker_win(false)
			attacker_guild:AddGuildMoney(20000, chieftain)
			if attacker_cp and attacker_cp:status() == CaptureStatus_CS_FIGHTING then
				local chapter_cp = LuaMtCapturePointManager():FindChapterData(attacker_cp:target_chapter_id())
				if chapter_cp ~= nil and chapter_cp:guid() == attacker_cp:guid() then
					LuaMtCapturePointManager():RemoveCaptureChapter(attacker_cp:target_chapter_id(), attacker_cp:guid())
					LuaServer():LuaLog("[capture_point] log tick timeout, remove relation,chapter:"..attacker_cp:target_chapter_id()..",guid:"..attacker_cp:guid(),g_log_level.LL_INFO)
				end
				attacker_cp:set_status(CaptureStatus_CS_INITED)
				attacker_cp:set_target_chapter_id(0)
				for i=0, attacker_cp:strong_holds_size() - 1 do
					local sh = attacker_cp:mutable_strong_holds(i)
					sh:set_status(StrongHoldStatus_SHS_IDLE)
				end
			end
			if defender_cp and defender_cp:status() == CaptureStatus_CS_FIGHTING then
				defender_cp:set_status(CaptureStatus_CS_PROTECT)
				for i=0, defender_cp:strong_holds_size() - 1 do
					local sh = defender_cp:mutable_strong_holds(i)
					sh:set_status(StrongHoldStatus_SHS_IDLE)
				end
			end
			LuaServer():LuaLog("[capture_point OnLogTick] time_out,log:"..battle_log:guid()..",attacker_guid:"..battle_log:attacker_guid()..",defender:"..battle_log:defender_guid(),g_log_level.LL_INFO)
			x001025_OnCapturePointBattleOver(battle_log)
			x001025_OnCapturePointPushData(battle_log)
		end
	end]]--
end

function x001025_OnTick(cp)
	--宣战等待30秒钟
	if cp:status() == CaptureStatus_CS_DECLARE_WAR 
		and (LuaMtTimerManager():CurrentTime() / 1000) - cp:status_change_time()  > g_DECLARE_WAR_WAIT_TIME then
		local chapter_id = cp:chapter_id()
		local attacker_guid = cp:challenger_guild_guid()
		local defender_guid = cp:capture_guild_guid()
		local attacker_cp = LuaMtCapturePointManager():FindGuildCapturePoint(attacker_guid)
		local defender_cp = LuaMtCapturePointManager():FindGuildCapturePoint(defender_guid)
		local battle_log = LuaMtCapturePointManager():CreateBattleLog()
		battle_log:set_attacker_guid(attacker_guid)
		battle_log:set_defender_guid(defender_guid)
		battle_log:set_chapter_id(chapter_id)
		battle_log:set_battle_time(LuaMtTimerManager():CurrentTime() / 1000)
		local log_guid = battle_log:guid()
		cp:set_log_guid(log_guid)
		cp:set_status(CaptureStatus_CS_FIGHTING)
		cp:set_status_change_time(LuaMtTimerManager():CurrentTime() / 1000)
		LuaServer():LuaLog("[capture_point OnTick] pvp battle start "..attacker_guid.." vs " .. defender_guid..",log:"..log_guid,g_log_level.LL_INFO)
		for i = 0,4 do
			x001025_OnCaptureBattleStart(cp,i)
		end
		x001025_SyncCapturePointToGuild(attacker_guid, defender_guid, chapter_id)
	end
	--战斗五分钟超时
	if cp:status() == CaptureStatus_CS_FIGHTING
		and (LuaMtTimerManager():CurrentTime() / 1000) - cp:status_change_time() > g_FIGHTING_TIME_OUT then
		local battle_log = LuaMtCapturePointManager():FindBattleLog(cp:log_guid())
		battle_log:set_finish(true)
		battle_log:set_attacker_win(false)
		local attacker_cp = LuaMtCapturePointManager():FindCapturePoint(cp:challenger_guild_guid())
		local defender_cp = LuaMtCapturePointManager():FindCapturePoint(cp:capture_guild_guid())
		local attacker_guild = LuaMtGuildManager():GetGuildByid(cp:challenger_guild_guid())
		if attacker_guild ~= nil then
			local chieftain = LuaServer():FindPlayer(attacker_guild:ChieftainGuid())
			attacker_guild:AddGuildMoney(20000, chieftain)
		end
		LuaServer():LuaLog("[capture_point OnTick] time_out,log:"..battle_log:guid()..",attacker_guid:"..battle_log:attacker_guid()..",defender:"..battle_log:defender_guid(),g_log_level.LL_INFO)
		cp:set_winner_guid(cp:capture_guild_guid())
		x001025_OnCapturePointBattleOver(cp)
	end
	
	--保护期十分钟结束
	if cp:status() == CaptureStatus_CS_PROTECT
		and (LuaMtTimerManager():CurrentTime() / 1000) - cp:status_change_time() > g_PROTECT_TIME then
		cp:set_status(CaptureStatus_CS_INITED)
		LuaServer():LuaLog("[capture_point OnTick] chapter:"..cp:chapter_id()..",protect over",g_log_level.LL_INFO)
		x001025_SyncCapturePointToGuild(cp:capture_guild_guid(),0, cp:chapter_id())
	end
end

function x001025_OnStrongHoldUpdateDone(guild_guid, cg_data)
	local guild = LuaMtGuildManager():GetGuildByid(guild_guid)
	local chieftain = LuaServer():FindPlayer(guild:ChieftainGuid())
	local self_cp = LuaMtCapturePointManager():FindGuildCapturePoint(guild_guid)
	local relate_cp = LuaMtCapturePointManager():RelateCapturePoint(guild_guid)
	if self_cp == nil then
		x001025_SendGuildChieftainMsg(chieftain, "capture_point_update_strong_hold_fail_wrong_status")
		return 
	end
	if relate_cp ~= nil and relate_cp:status() ~= CaptureStatus_CS_INITED then
		x001025_SendGuildChieftainMsg(chieftain, "capture_point_update_strong_hold_fail_wrong_status")
		print("x001025_OnStrongHoldUpdateDone status:"..tostring(relate_cp:status()))
		return
	end
	
	for i = 0, cg_data:strong_holds_size() - 1 do
		self_cp:mutable_strong_holds(i):CopyFrom(cg_data:strong_holds(i))
		if relate_cp ~= nil and relate_cp:capture_guild_guid() == guild_guid then
			relate_cp:mutable_capture_hero_datas(i):CopyFrom(self_cp:strong_holds(i):heros(0))
		end
	end
	
	local self_guild = LuaMtGuildManager():GetGuildByid(guild_guid)
	if self_guild == nil then return end
	local chieftain_guid = self_guild:ChieftainGuid()
	local chieftain = LuaServer():FindPlayer(chieftain_guid)
	if chieftain == nil then return end
	local ret_msg = GC_GuildCapturePoint:new()
	ret_msg:SetInitialized()
	for i = 0, self_cp:strong_holds_size() - 1 do
		ret_msg:add_strong_holds():CopyFrom(self_cp:strong_holds(i))
	end

	if relate_cp ~= nil and relate_cp:capture_guild_guid() == guild_guid then
		ret_msg:set_hook_users(LuaMtCapturePointManager():GetHookUserCount(relate_cp:chapter_id()))
		local scene_stage = LuaMtSceneStageManager():GetGroupSceneStageByGroupId(relate_cp:chapter_id())
		if scene_stage ~= nil then
			ret_msg:set_tax(scene_stage:guild_tax())
			ret_msg:set_tax_rate(scene_stage:guild_tax_rate())
			ret_msg:set_chapter_id(relate_cp:chapter_id())
		end
	end
	x001025_SendGuildChieftainMsg(chieftain, "capture_point_update_strong_hold_success")
	x001025_BroadCastMsgToGuild(guild_guid, ret_msg)
end

function x001025_OnCapturePointInit(chapter_id, cp)
	cp:Clear()
	cp:SetInitialized()
	cp:set_chapter_id(chapter_id)
	for i=0,4 do
		LuaServer():LuaLog("reset chapter_id:"..chapter_id..","..i, g_log_level.LL_INFO)
		cp:add_strong_hold_status(StrongHoldStatus_SHS_IDLE)
		local cp_hero = cp:add_capture_hero_datas()
		local chal_hero = cp:add_challenger_hero_datas()
		cp_hero:SetInitialized()
		chal_hero:SetInitialized()
		local monster_id = LuaMtCapturePointManager():CapturePointGuardBossId(chapter_id,i)
		local monster_config = LuaMtMonsterManager():FindMonster(monster_id)
		if monster_config ~= nil then
			cp_hero:set_player_guid(0)
			cp_hero:set_actor_config_id(monster_id)
			cp_hero:set_level(monster_config:level())
			cp_hero:set_color(1)
			cp_hero:set_star(1)
			cp_hero:set_hair(0)
		end
	end
end

--战斗数据变化同步
function x001025_SyncCapturePointToGuild(guid_1, guid_2, chapter_id)
		local brief = CapturePotinBrief:new()
		x010005_fill_brief_info(brief, chapter_id)
		x001025_BroadCastMsgToGuild(guid_1, brief)
		x001025_BroadCastMsgToGuild(guid_2, brief)
		brief:delete()
		local ret_msg = GC_GuildCapturePoint:new()
		x010005_fill_self_cp_info(guid_1, ret_msg)
		x001025_BroadCastMsgToGuild(guid_1, ret_msg)
		x010005_fill_self_cp_info(guid_2, ret_msg)
		x001025_BroadCastMsgToGuild(guid_2, ret_msg)
		ret_msg:delete()
end
--广播公会数据变动
function x001025_BroadCastMsgToGuild(guild_guid, msg)
	LuaMtGuildManager():BroadcastGuild(guild_guid, msg)
end