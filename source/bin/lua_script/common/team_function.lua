x000008_TeamLevelLimit = 10
x000008_GroupTeamLevelLimit = 50

--创建队伍
function x000008_OnCreateTeam(player_)
	if player_ == nil then
		return INVALID_GUID;
	end
	if player_:PlayerLevel() < x000008_TeamLevelLimit then
		player_:SendClientNotify("Team_LevelLimitTen", -1,-1);
		return INVALID_GUID;
	end
	
	if player_:GetTeamID() ~= INVALID_GUID then
		player_:SendClientNotify("Team_In", -1,-1);
		return INVALID_GUID;
	end

	if player_:GetCurBattleType() ~= BattleGroundType_NO_BATTLE then
		player_:SendClientNotify("Team_CreateInBattle", -1,-1);
		return INVALID_GUID;
	end
	local ret = x000008_IsInBattleField(player_);
	if ret == true then
		player_:SendClientNotify("Team_InBattleField", -1,-1);
		return INVALID_GUID;
	end
	local ret = x000008_IsNoTeamScene(player_);
	if ret == true then
		player_:SendClientNotify("Team_NoInScene", -1,-1);
		return INVALID_GUID;
	end
	return LuaMtTeamManager():CreateTeam(player_, true);
end

--邀请玩家入队
function x000008_OnInviteyPlayer(player,targetguid)
	local targetplayer=LuaServer():FindPlayer(targetguid);
	if targetplayer == nil then
		return player:SendClientNotify("PlayerNoExist",-1,-1)
	end
	if targetplayer:PlayerLevel() < x000008_TeamLevelLimit then
		return player:SendClientNotify("Team_TargetLevelLimitTen",-1,-1)
	end
	if targetplayer:GetTeamID()  ~= 0 then
		return player:SendClientNotify("Team_TargetHave",-1,-1)
	end
	
	if x000008_IsInGuildBattle(player) then
		if x000008_IsInGuildBattle(targetplayer) then
			if targetplayer:GetGuildID() ~= player:GetGuildID() then
				return player:SendClientNotify("Team_TargetNotSelfGuild",-1,-1) 
			end
		else
			local ret = x001023_CheckPlayerQualify(player,targetguid)	
			if ret ~= "ok" then
				return player:SendClientNotify(ret,-1,-1)
			end
		end
	end
	
	local ret = x000008_IsInBattleField(targetplayer);
	if ret == true then
		player:SendClientNotify("Team_TargetInBattleField", -1,-1);
		return INVALID_GUID;
	end
	local teamid = player:GetTeamID();
	if teamid == 0 then
		teamid = x000008_OnCreateTeam(player);
	end
	local teaminfo = LuaMtTeamManager():GetTeamInfo(teamid);
	if teaminfo == nil then
		return player:SendClientNotify("UnknownError",-1,-1)
	end
	if teaminfo:robot_team() then
		return player:SendClientNotify("Team_CannotAddPlayerToTeam", -1, -1);
	end
	
	LuaMtTeamManager():SendInviteyTeamToPlayer(targetplayer,player,teaminfo);
end

--回复-邀请玩家入队
function x000008_OnInviteyAgreeTeamReq(player,message)
	local agree = message:agree();
	if agree == false then
		local inviteguid = message:teaminviteguid();
		local inviteplayer = LuaServer():FindPlayer(inviteguid);
		if inviteplayer ~= nil then
			inviteplayer:SendClientNotify("jujueyaoqingrudui|"..player:Name(),-1,-1)
		end
	else
		local teaminfo = LuaMtTeamManager():GetTeamInfo(message:teamid())
		local ret = LuaMtTeamManager():OnAgreeApplyTeam(player,teaminfo,-1)
		--player:
	end
end

--玩家申请入队
function x000008_OnPlayerApplyTeam(player,teamid)
	if player:PlayerLevel() < x000008_TeamLevelLimit then
		return player:SendClientNotify("Team_LevelLimitTen",-1,-1)
	end
	if player:GetTeamID() ~= 0 then
		return player:SendClientNotify("Team_In",-1,-1)
	end
	local ret = x000008_IsInBattleField(player);
	if ret == true then
		player_:SendClientNotify("Team_InBattleField", -1,-1);
		return INVALID_GUID;
	end
	local teaminfo = LuaMtTeamManager():GetTeamInfo(teamid);
	if teaminfo == nil then
		return player:SendClientNotify("Team_NoExist",-1,-1)
	end
	if LuaMtTeamManager():IsFull(teaminfo) == true then
		return player:SendClientNotify("Team_Full",-1,-1)
	end
	local playerid = player:Guid();
	local count = teaminfo:applylist_size();
	for index=0,count-1,1 do 
		local apply = teaminfo:applylist(index)
		if apply == playerid then
			return player:SendClientNotify("Team_InApply",-1,-1)
		end
	end

	
	if teaminfo:auto_match() == true then
		local checkautoindex = LuaMtTeamManager():ApplyCheckAutoMatch(teaminfo,player)
		--if checkautoindex == -1 then
		--	return player:SendClientNotify("LevelLimit",-1,-1)
		--elseif checkautoindex == -2 then
		--	return player:SendClientNotify("Team_NotPurpose",-1,-1)
		--elseif checkautoindex == -3 then
		--	return player:SendClientNotify("Team_NotHeroPostion",-1,-1)
		--end
		if checkautoindex >= 0 then
			LuaMtTeamManager():OnAgreeApplyTeam(player,teaminfo,checkautoindex);
			return;
		end
	end
	teaminfo:add_applylist(playerid);
	LuaMtTeamManager():SendTeamApplyList(player,teaminfo);
end

--队长同意玩家入队申请
function x000008_OnAgreeApplyTeamReq(player_,message)
	local targetplayerid = message:playerguid();
	local targetplayer = LuaServer():FindPlayer(targetplayerid);
	if targetplayer == nil then
		return ;
	end
	local ret = x000008_IsInBattleField(targetplayer);
	if ret == true then
		player_:SendClientNotify("Team_TargetInBattleField", -1,-1);
		return ;
	end
	if x000008_IsInGuildBattle(player_) then
		if x000008_IsInGuildBattle(targetplayer) then
			if targetplayer:GetGuildID() ~= player_:GetGuildID() then
				return player_:SendClientNotify("Team_TargetNotSelfGuild",-1,-1) 
			end
		else
			local ret = x001023_CheckPlayerQualify(player_,targetplayerid)	
			if ret ~= "ok" then
				return player_:SendClientNotify(ret,-1,-1)
			end
			
		end
	end
	
	local teaminfo = LuaMtTeamManager():GetTeamInfo(player_:GetTeamID());
	if teaminfo == nil then
		return true; 
	end
	
	local inapply = false;
	local count = teaminfo:applylist_size();
	for index=0,count-1,1 do 
		local applyid = teaminfo:applylist(index)
		if applyid == targetplayerid then
			teaminfo:set_applylist(index, INVALID_GUID);
			inapply = true;
		end
	end
	if inapply == false then
		return ;
	end
	LuaMtTeamManager():OnAgreeApplyTeam(targetplayer,teaminfo,-1)
end
--队长设置自动匹配
function x000008_OnSetTeamAutoMatchReq(player_,message)
	local playerid = player_:Guid();
	local teamid = player_:GetTeamID();
	if teamid == INVALID_GUID then
		player_:SendClientNotify("Team_Out", -1, -1);
		return ;
	end

	local teaminfo = LuaMtTeamManager():GetTeamInfo(teamid);
	if teaminfo == nil then
		player_:SendClientNotify("Team_NoExist", -1, -1);
		return ;
	end
	--如果不是队长
	if teaminfo:leader() ~= playerid then 
		player_:SendClientNotify("Team_NotLeader", -1, -1);
		return ;
	end
	if teaminfo:robot_team() then
		player_:SendClientNotify("Team_CannotChangeSetting", -1, -1);
		return ;
	end
	local ret = x000008_IsInGuildBattle(player_);
	if ret == true then
		player_:SendClientNotify("Team_InGuildBattle", -1,-1);
		return;
	end
	teaminfo:set_auto_match(message:auto_match());
	LuaMtTeamManager():SendTeamAutoMatch(teaminfo,player_);
	if teaminfo:auto_match() == true then
		player_:SendClientNotify("kaishipipeitishi", -1,-1);
		LuaMtTeamManager():AutomatchPosition(teaminfo);
		local count = teaminfo:applylist_size();
		for index=0,count-1,1 do 
			local applyid = teaminfo:applylist(index)
			local applyplayer=LuaServer():FindPlayer(applyid);
			if applyplayer ~= nil then
				local checkautoindex = LuaMtTeamManager():ApplyCheckAutoMatch(teaminfo,applyplayer)
				if checkautoindex >= 0 then
					LuaMtTeamManager():OnAgreeApplyTeam(applyplayer,teaminfo,checkautoindex);
				end
			end
		end
	else
		player_:SendClientNotify("quxiaopipeitishi", -1,-1);
	end
end
--玩家设置自动匹配
function x000008_OnPlayerAutoMatchReq(player_,message)
	local ret = x000008_IsInBattleField(player_);
	if ret == true then
		player_:SendClientNotify("Team_InBattleField", -1,-1);
		return;
	end
	local ret = x000008_IsInGuildBattle(player_);
	if ret == true then
		player_:SendClientNotify("Team_InGuildBattle", -1,-1);
		return;
	end
	local purpose = message:purpose();
	local setauto = message:auto_match();
	local playerid = player_:Guid();
	local lastset = LuaMtTeamManager():PlayerIsAutoMatch(playerid, purpose);
	if lastset == true and setauto == false then
		LuaMtTeamManager():OnClearPlayerAutoMatch(playerid);
	elseif lastset == false and setauto == true then
		LuaMtTeamManager():AddPlayerAutoMatch(purpose,player_)
	end
	LuaMtTeamManager():SendPlayerAutoMatch(player_,setauto,purpose);
end

--心跳中自动匹配队伍
function x000008_TickAutoMatchTeam(player,teaminfo,checkautoindex)
	local ret = x000008_IsInBattleField(player);
	if ret == true then
		return;
	end
	LuaMtTeamManager():OnAgreeApplyTeam(player,teaminfo,checkautoindex);
	LuaMtTeamManager():SendPlayerAutoMatch(player,false,-1);
end

function x000008_CheckFollowLeader(member_,leaderid)
	if member_ == nil then
		return 0;
	end
	local leader_ = LuaServer():FindPlayer(leaderid);
	if leader_ == nil then
		return 0;
	end
	if x000008_IsInGuildBattle(leader_) == true then
		if x000008_IsInGuildBattle(member_) then
			if leader_:GetGuildID() ~= member_:GetGuildID() then
				member_:SendClientNotify("Team_TargetNotSelfGuild",-1,-1) 
				return 0;
			end
		else
			local ret = x001023_CheckPlayerQualify(leader_,member_:Guid())	
			if ret ~= "ok" then
				member_:SendClientNotify(ret,-1,-1)
				return 0;
			end
		end
	end
	
	return 1;
end

--玩家是否在战场中
function x000008_IsInBattleField(player)
	if player == nil then
		return false;
	end
	local bf_proxy = player:GetBattleFeildProxy()
	if bf_proxy ~= nil then
		if bf_proxy:raid_rtid() ~= 0 then
			if	bf_proxy:bf_state() ~= BFState_Bf_Leave  then		
				return true;
			end
		end
	end
	return false;
end
--玩家是否在帮会战中
function x000008_IsInGuildBattle(player)
	if player == nil then
		return false;
	end
	local battlesceneid = LuaMtConfigDataManager():FindConfigValue("guild_battle_common"):value1();
	if player:GetSceneId() == battlesceneid then
		return true;
	end
	return false;
end

--不可组队的场景
function x000008_IsNoTeamScene(player)
	if player == nil then
		return false;
	end
	local sceneid = player:GetSceneId();
	if sceneid == 1401 then -- or LuaMtGuildManager():InGuildWetCopyScene(sceneid) == true 
		return true;
	end
	return false;
end