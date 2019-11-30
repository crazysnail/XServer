
local activity_type = ActivityType.UnionBattle


function x001023_GuildBattleCheckPlayer(player,battlestage,guildid,guilbbattle)
	local playerguildid = player:GetGuildID(); 
	if playerguildid ~= guildid then
		return "Guild_Battle_TeamNoGuild";
	end
	local qualify = player:CheckGuildBattleQualify();
	if qualify == false then
		return "Guild_Battle_NoQualify|"..player:Name();
	end
	local over = guilbbattle:GetWinGuild();
	if over ~= 0 then
		return "Guild_Battle_Stop";
	end
	local battleplayer = guilbbattle:GetGuildBattlePlayer(player:Guid());
	if battleplayer ~= nil then
		if battlestage == GuildBattle_Stage_GuildBattle_Stage_War then
			return "Guild_Battle_TwiceEnter|"..player:Name();
		end
		if battleplayer:mobility() <= 0 then
			return "GuildBattle_Mobility_ZERO|"..player:Name();
		end
	end
	return "ok";
end	

function x001023_CheckPlayerQualify(player,playerid)
	if player == nil then
		return "UnknownError";
	end

	local guildid = player:GetGuildID();
	local target_player = LuaServer():FindPlayer(playerid);
	if target_player == nil then
		return "PLAYER_OFFLINE";
	end

	local battlestage = LuaMtGuildManager():GetGuildBattleStage();
	if battlestage ~= GuildBattle_Stage_GuildBattle_Stage_Prepare then
		return "Guild_Battle_NoInPre";
	end
	
	
	local guilbbattle = LuaMtGuildManager():GetGuildBattle(guildid);
	if guilbbattle == nil then
		return "UnknownError";
	end
	
	local ret = x001023_GuildBattleCheckPlayer(target_player,battlestage,guildid,guilbbattle);
	return ret;
end

function x001023_OnJoinCheck(player,params)
	local ac_config = LuaActivityManager():FindActivity(activity_type)
	if ac_config == nil then
		return ResultCode_InternalResult
	end
	local ac_name = ac_config:name()	
		
	--等级检查
	if player:PlayerLevel()<ac_config:open_level() then
		return player:SendClientNotify("ac_notify_002|"..math.floor(ac_config:open_level()).."|"..ac_name,-1,1)
	end
	
	--活动开启性判定
	--local now_time = LuaMtTimerManager():Time2Num()
	--local end_time = ac_config:day_end_time()
	local begin_time = ac_config:day_begin_time()
	--if now_time> tonumber(end_time) then				--ac_notify_011,{0}活动已结束
	--	return player:SendClientNotify("ac_notify_011|"..ac_name,-1,1)
	--end
	local newtime = x000010_TimeToString(begin_time)
	--local ret= LuaActivityManager():IsActive(ac_config, player:PlayerLevel()) 
	--if ret ~= ResultCode_ResultOK then		--ac_notify_013,{0}活动未开启，晚上{1}才可以进行
	--	return player:SendClientNotify("ac_notify_013|"..ac_name.."|"..begin_time,-1,1)
	--end

	--团队检查
	if player:IsTeamGroup() then
		return player:SendClientNotify("ac_notify_003|"..ac_name,-1,1)
	end
	
	--次数判定,只有自己
	--local ltimes = ac_config:limit_times()
	--local rtimes = ac_config:reward_count();
	--local done_times = player:OnLuaFunCall_1(501, "GetActivityTimes", activity_type)
	--if ltimes >= 0 then
	--	if done_times >= ltimes then	--今日你已完成{0}，明日再来吧											
	--		return player:SendClientNotify("ac_notify_007|"..ac_name,-1,1)			
	--	end
	--end	

	--if rtimes > 0 and rtimes >= done_times then		--今日你已完成{0}，再次进入将不再获取任何奖励
	--	player:SendClientNotify("ac_notify_008|"..ac_name,-1,1)
	--end		
		
	--是否加入公会	
	local battlestage = LuaMtGuildManager():GetGuildBattleStage();
	if battlestage == GuildBattle_Stage_GuildBattle_Stage_None then
		return player:SendClientNotify("ac_notify_013|"..ac_name.."|"..newtime,-1,1)
	elseif  battlestage == GuildBattle_Stage_GuildBattle_Stage_War then
		return player:SendClientNotify("ac_notify_021|"..ac_name,-1,1)
	elseif  battlestage == GuildBattle_Stage_GuildBattle_Stage_End then --battlestage == GuildBattle_Stage_GuildBattle_Stage_War or
		return player:SendClientNotify("ac_notify_011|"..ac_name,-1,1)
	end
	
	local guildid = player:GetGuildID()
	local guilbbattle = LuaMtGuildManager():GetGuildBattle(guildid);
	if guilbbattle == nil then
		return player:SendClientNotify("Guild_Battle_NoEnter",-1,-1)
	end
	local guilbbattle_info =  guilbbattle:GetBattleInfo();
	if guilbbattle_info == nil then
		return player:SendClientNotify("Guild_Battle_NoEnter",-1,-1)
	end

	if player:GetTeamID() == 0 then
		local ret = x001023_GuildBattleCheckPlayer(player,battlestage,guildid,guilbbattle);
		if ret ~= "ok" then
			return player:SendClientNotify(ret,-1,-1)		
		end
	else
		local teammemberlist = player:GetTeamMember(false);
		for i,teammemberid in pairs(teammemberlist) do
			local teamplayer = LuaServer():FindPlayer(teammemberid);
			if teamplayer == nil then
				return player:SendClientNotify("",-1,-1)
			end
			local ret = x001023_GuildBattleCheckPlayer(teamplayer,battlestage,guildid,guilbbattle);
			if ret ~= "ok" then
				return player:SendClientNotify(ret,-1,-1)
			end
		end
	end
	
	return ResultCode_ResultOK
end

function x001023_OnBattleCheck(player,params)
	return ResultCode_ResultOK
end
function x001023_testplayers(player)
	LuaMtGuildManager():testplayer(player);
	return 0;
end
function x001023_testplayer(player)
	LuaMtGuildManager():testplayer(player);
	return 0;
end


function x001023_OnGoBattle(battle_ground,params)
	
	local player = battle_ground:GetCreator()
	local target = battle_ground:GetTarget()
	if player == nil or player:BattleGround() ~=nil  then
		return ResultCode_BattleCreateFailed
	end
	
	if target == nil or target:BattleGround() ~=nil  then
		return ResultCode_InvalidTargetPlayer
	end
	
	local battlegroup = LuaSceneManager():GetSceneBattleGroupById(player:GetSceneId(),params["group_id"]);
	if battlegroup == nil then
		return ResultCode_InvalidConfigData
	end

	player:StopMove()
	target:StopMove()
		
	battle_ground:SetScriptId(1023)				
	local brief = battle_ground:Brief()
	brief:set_scene_id(player:GetSceneId())
	brief:set_battle_group_id(params["group_id"])		
	brief:set_type(BattleGroundType_PVP_GUILD_BATTLE)
	
	if battle_ground:Init() == false then
		return ResultCode_BattleCreateFailed
	end		
	
	battle_ground:BattleEnterNotify()	
	
	return ResultCode_ResultOK
end

function x001023_GoGuildWorldBattle(player,npcindex)
	print('x001023_GoGuildWorldBattle');
	local guildid = player:GetGuildID();
	local guilbbattle = LuaMtGuildManager():GetGuildBattle(guildid);
	if guilbbattle == nil then
		return player:SendClientNotify("Guild_Battle_NoEnter",-1,-1)
	end	
	local guilbbattle_info =  guilbbattle:GetBattleInfo();
	if guilbbattle_info == nil then
		return player:SendClientNotify("Guild_Battle_NoEnter",-1,-1)
	end
	
	local battlesceneid = LuaMtConfigDataManager():FindConfigValue("guild_battle_common"):value1();
	local raidid = guilbbattle:GetGuildBattleRaidID(battlesceneid);
	if raidid == 0 then
		raidid = guilbbattle:CreateGuildBattleRaid(battlesceneid,2008,npcindex,player:GetSceneId());
		local raid = guilbbattle:GetGuildBattleRaid();
		if raid == nil then
			return player:SendClientNotify("Guild_Battle_NoEnter",-1,-1)
		end
		raid:SetParams64("guildid",guildid)
	end
	if raidid == 0 then
		return player:SendClientNotify("Guild_Battle_NoEnter",-1,-1)
	end
	LuaMtTeamManager():OnClearPlayerAutoMatch(player:Guid());
	local camp = 1;
	if guildid == guilbbattle_info:a_guildid() then
		camp = 2;
	end
	player:GoRaidByPosIndex(battlesceneid,2008,raidid,camp,camp);
	
	return ResultCode_ResultOK
end


function x001023_OnCreateReady(battle_ground, player)
	--battle_ground:RefreshAttacker()
	battle_ground:CreateAttackerPlayerBattleActors(true,true,0)
	--battle_ground:AddDefendertargetTeam();
	battle_ground:CreateDefenderPlayerBattleActors(true,true,0)
	return ResultCode_ResultOK;
end

function x001023_OnDamageRefix(battle_ground,damage_info,killer)

end

function x001023_OnDamage(battle, attacker, source_guid, target_guid, damage)
	return 0;
end

function x001023_OnCheckBattleEnd(battle_ground)
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

function x001023_OnBattleReward(battle_ground, winnercamp)
	
end
function x001023_OnEndPlayer(gbinfo,player,win,downmobility)
	local playerguid = player:Guid();
	local battleplayer = gbinfo:GetPlayerinfo(playerguid);
	if battleplayer == nil then
		return
	end
	battleplayer:set_fighting(false);
	local level_config = LuaMtActorConfig():GetLevelData(player:PlayerLevel())
	if level_config == nil then
		return 
	end
	if battleplayer:battlereward() < 10 then
		player:AddItemByIdWithNotify(TokenType_Token_Gold, level_config:guildbattle_battle_gold(),ItemAddLogType_AddType_Guild,activity_type,1);
		player:AddItemByIdWithNotify(TokenType_Token_Exp, level_config:guildbattle_battle_exp(), ItemAddLogType_AddType_Guild,activity_type,1);
	end
	local score = battleplayer:score();
	local mobility = battleplayer:mobility();
	local streakwin = battleplayer:streakwin();
	
	if win == true then
		score = score + 60;
		streakwin = streakwin+1;
		
		--月卡
		local reward = 10;		
		player:AddItemByIdWithNotify(TokenType_Token_GuildContribute, reward, ItemAddLogType_AddType_Guild,activity_type,1);
		player:SendClientNotify("GuildBattle_Win", -1, -1);
		player:SendClientNotify("GuildBattle_getscore|60", -1, -1);
	else
		score = score + 15;
		streakwin = 0;
		
		--月卡
		local reward = 5;
		player:AddItemByIdWithNotify(TokenType_Token_GuildContribute, reward, ItemAddLogType_AddType_Guild,activity_type,1);
		player:SendClientNotify("GuildBattle_Lost", -1, -1);
		player:SendClientNotify("GuildBattle_getscore|15", -1, -1);
		
		if player:IsTeamLeader() or player:GetTeamID() == 0 then
			local deadindex = LuaServer():RandomNum(1, 32)
			local battlegroup = LuaSceneManager():GetSceneBattleGroupById(1501,deadindex);
			local pos = battlegroup:player_pos(1);
			player:GoTo(1501,{pos:x(),pos:y(),pos:z()})
		end
	end
	mobility = mobility - downmobility;
	if mobility < 0 then
		mobility = 0;
	end
	player:SendClientNotify("GuildBattle_downmobility|"..math.floor(tostring(downmobility)), -1, -1);
	battleplayer:set_score(score);
	battleplayer:set_mobility(mobility);
	battleplayer:set_protectcd(20);
	battleplayer:set_streakwin(streakwin);
	
	gbinfo:UpdatePlayerByGuid(playerguid);
	if streakwin == 5 then
		player:AddItemByIdWithNotify(20139002, 1, ItemAddLogType_AddType_Guild,activity_type,1);
		Lua_send_run_massage(("GuildBattle_StreakWin_Run|"..player:Name().."|"..tostring(streakwin).."|"..LuaItemManager():GetItemName(20139002).."|1"));
	end
	if mobility <= 0 then
		gbinfo:AddLeavePlayerExp(player,false);
		player:SendStatisticsResult(ActivityType.UnionBattle,true);	
		LuaServer():SendS2GCommonMessage("S2GLeaveTeamAndScene",{},{player:Guid()},{})	--先到主线程离开队伍，再离开场景
	end
	if gbinfo:GetWinGuild() ~= 0 then
		if gbinfo:GetWinGuild() == g_max_guild_id then
			if player:IsTeamLeader() or player:GetTeamID() == 0 then
				player:GoingLeaveScene();
			end
		elseif gbinfo:GetWinGuild() ~= player:GetGuildID() then
			if player:IsTeamLeader() or player:GetTeamID() == 0 then
				player:GoingLeaveScene();
			end
		end
	end
end
function x001023_OnBattleEnd(battle_ground,force, winnercamp)
	--通知客户端结束战斗
	battle_ground:BattleOverNotify(winnercamp)

	local creator = battle_ground:GetCreator()
	local target = battle_ground:GetTarget()
	if creator ~= nil and target~=nil then
		local guildid = creator:GetGuildID()
		local gbinfo = LuaMtGuildManager():GetGuildBattle(guildid);
		if gbinfo == nil then
			return ;
		end
		local winners = nil
		local losers = nil
		local winerguildid,loserguildid
		local winplayer = nil;
		local loserplayer = nil;
		local downmobility = 0;
		if winnercamp == 1 then
			--攻击方获胜
			winerguildid=creator:GetGuildID()
			loserguildid=target:GetGuildID()
			winners=battle_ground:GetAttackerPlayers()
			losers=battle_ground:GetDefenderPlayers()
			winplayer = creator;
			loserplayer = target;
		else
			--防御方获胜
			winerguildid=target:GetGuildID()
			loserguildid=creator:GetGuildID()
			winners=battle_ground:GetDefenderPlayers()
			losers=battle_ground:GetAttackerPlayers()
			winplayer = target;
			loserplayer = creator;
		end
		for k, player in pairs(winners) do
			if guildid == winerguildid then
				downmobility = 1;
			end
			x001023_OnEndPlayer(gbinfo,player,true,downmobility);
		end
		downmobility = 0
		updownmobility = 8
		local losersize = #losers;
		if losersize == 1 then
			updownmobility = 20;
		elseif losersize == 2 then
			updownmobility = 17;
		elseif losersize == 3 then
			updownmobility = 14;
		elseif losersize == 4 then
			updownmobility = 11;
		end
		for k, player in pairs(losers) do
			if guildid == loserguildid then
				downmobility = 1;
			end
			x001023_OnEndPlayer(gbinfo,player,false,(downmobility+updownmobility));
		end	
		
		local streakwin = 0;
		local wingbplayer = gbinfo:GetPlayerinfo(winplayer:Guid());
		if wingbplayer ~= nil then
			streakwin = wingbplayer:streakwin();
		end

		local winnameset = x000010_NewSet();
		for k, player in pairs(winners) do
			if player:IsTeamLeader() then
				winnameset:insert(player:Name())
			end
		end
		for k, player in pairs(winners) do
			winnameset:insert(player:Name())
		end
		local losernameset = x000010_NewSet();
		for k, player in pairs(losers) do
			if player:IsTeamLeader() then
				losernameset:insert(player:Name())
			end
		end
		for k, player in pairs(losers) do
			losernameset:insert(player:Name())
		end
		local winnamestr = "";
		local playercount = #winnameset;
		local i=0;
		for _,name in ipairs(winnameset) do
			--if #winnameset > 1 then
			--	winnamestr = winnamestr..","
			--end
		    winnamestr = winnamestr..name;
			i=i+1;
			if i < playercount then
				winnamestr = winnamestr..","
			end
		end
		local losernamestr = "";
		playercount = #losernameset;
		i=0;
		for _,name in ipairs(losernameset) do
			--if #losernameset > 1 then
			--	losernamestr = losernamestr..","
			--end
		    losernamestr = losernamestr..name;
			i=i+1;
			if i < playercount then
				losernamestr = losernamestr..","
			end
		end
		local winstr = "GuildBattle_WinNotify|"..winnamestr.."|"..losernamestr;
		local loserstr = "GuildBattle_LostNotify|"..losernamestr.."|"..winnamestr;
		winplayer:SendClientNotify(winstr,ChatChannel_GUILD,-1);
		loserplayer:SendClientNotify(loserstr,ChatChannel_GUILD,-1);
		if streakwin >= 3 then
			winplayer:SendClientNotify("GuildBattle_StreakWin|"..winnamestr.."|"..math.floor(tostring(streakwin)),ChatChannel_GUILD,-1);
		end
	end
	return true
end
