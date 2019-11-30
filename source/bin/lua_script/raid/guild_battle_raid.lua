
--公会战
--
local left_time = 5400		--持续时间 一个半小时
local empty_check_time =180 --空时间
local bufftable={"1150100101","1150100102","1150100103","1150100104"}

function x002008_OnCreate(raid)	
	for i=1,#bufftable do
		raid:SetParams32(bufftable[i],1)
	end
end

function x002008_OnRaidTimer(raid, index)

end

function x002008_OnBigTick(raid,elapseTime)	
	for i=1,#bufftable do
		local cdtime = raid:GetParams32(bufftable[i]);
		if cdtime > 0 then
			cdtime = cdtime - 10;
			if cdtime <= 0 then
				cdtime = 0;
				raid:BroadCastLuaPacket("NewBuffPoint",{tonumber(bufftable[i])},{},{});
			end
			raid:SetParams32(bufftable[i],cdtime);
		end
	end
	
	local begin_date = raid:GetBeginDate()
	local delta_time =  LuaMtTimerManager():DiffTimeToNow(begin_date)
	if delta_time>=left_time then
		raid:OnDestroy()
	end

end

function x002008_OnSecondTick(raid)	
	local guildid = raid:GetParams64("guildid");
	local guildbattle = LuaMtGuildManager():GetGuildBattle(guildid);
	if guildbattle == nil then
		return
	end
	guildbattle:OnSecondTick();
end

function x002008_OnMinTick(raid)
	local guildid = raid:GetParams64("guildid");
	local guildbattle = LuaMtGuildManager():GetGuildBattle(guildid);
	if guildbattle == nil then
		return
	end
	local curTime = LuaMtTimerManager():Time2Num()
	local curstage = LuaMtGuildManager():GetGuildBattleStage();
	if curstage == GuildBattle_Stage_GuildBattle_Stage_Prepare then
		x002008_OnGuildWarMinTick(raid,guildbattle);
	elseif curstage == GuildBattle_Stage_GuildBattle_Stage_War then
		x002008_OnGuildWarMinTick(raid,guildbattle);
		if curTime ~= x000007_GuildBattle_War then
			x002008_OnGuildWarFiveMinTick(raid,guildbattle);
		end
	elseif curstage == GuildBattle_Stage_GuildBattle_Stage_End then
		x002008_OnGuildWarStageEnd(raid,guildbattle);
	end
end

function x002008_OnGuildWarMinTick(raid,guildbattle)
	--还未结束每分钟都要给经验
	local gbinfo = guildbattle:GetBattleInfo()
	if gbinfo:win_guildid() == 0 then
		local attackers = raid:GetAttackers()
		local attackersize = #attackers;

		for i,player in pairs(attackers) do
			local level_config = LuaMtActorConfig():GetLevelData(player:PlayerLevel())
			if level_config ~= nil then
				player:AddItemByIdWithNotify(TokenType_Token_Exp,level_config:guildbattle_min_exp(),ItemAddLogType_AddType_Guild,ActivityType.UnionBattle,1)
			end
			--player:AddItemByIdWithNotify(TokenType_Token_GuildContribute,500,ItemAddLogType_AddType_Guild,ActivityType.UnionBattle,1)
		end
		local defenders = raid:GetDefenders()
		local defendersize = #defenders;

		for i,player in pairs(defenders) do
			local level_config = LuaMtActorConfig():GetLevelData(player:PlayerLevel())
			if level_config ~= nil then
				player:AddItemByIdWithNotify(TokenType_Token_Exp,level_config:guildbattle_min_exp(),ItemAddLogType_AddType_Guild,ActivityType.UnionBattle,1)
			end
			--player:AddItemByIdWithNotify(TokenType_Token_GuildContribute,500,ItemAddLogType_AddType_Guild,ActivityType.UnionBattle,1)
		end
	end
end

function x002008_OnGuildWarFiveMinTick(raid,guildbattle)
	
	local gbinfo = guildbattle:GetBattleInfo()
	if gbinfo:win_guildid() == 0 then
		local attackers = raid:GetAttackers()
		local attackersize = #attackers;

		for i,player in pairs(attackers) do
			local gb_player = guildbattle:GetPlayerinfo(player:Guid());
			if gb_player ~= nil then
				local score = gb_player:score();
				score = score + 30;
				gb_player:set_score(score);
			end
		end
		local defenders = raid:GetDefenders()
		local defendersize = #defenders;

		for i,player in pairs(defenders) do
			local gb_player = guildbattle:GetPlayerinfo(player:Guid());
			if gb_player ~= nil then
				local score = gb_player:score();
				score = score + 30;
				gb_player:set_score(score);
			end
		end
		guildbattle:UpdatePlayerList();

		local attackerscore = guildbattle:GetAttackerScore();
		local defenderscore = guildbattle:GetDefenderScore();
		--LuaServer():LuaLog("x002008_OnGuildWarFiveMinTick "..tostring(attackersize).." "..tostring(defendersize).." "..tostring(attackerscore).." "..tostring(defenderscore),g_log_level.LL_INFO)
		if attackersize == 0 and defendersize == 0 then
			if attackerscore > defenderscore then
				x002008_OnGuildWarEnd(raid,guildbattle,1);
			elseif attackerscore == defenderscore then
				x002008_OnGuildWarEnd(raid,guildbattle,0);
			else
				x002008_OnGuildWarEnd(raid,guildbattle,2);
			end
		elseif attackersize == 0 then
			x002008_OnGuildWarEnd(raid,guildbattle,2);
		elseif defendersize == 0 then
			x002008_OnGuildWarEnd(raid,guildbattle,1);
		end
	end
end

function x002008_OnGuildWarStageEnd(raid,guildbattle)
	local gbinfo = guildbattle:GetBattleInfo()
	if gbinfo:win_guildid() == 0 then
		local attackerscore = guildbattle:GetAttackerScore();
		local defenderscore = guildbattle:GetDefenderScore();
		if attackerscore > defenderscore then
			x002008_OnGuildWarEnd(raid,guildbattle,1);
		elseif attackerscore == defenderscore then
			x002008_OnGuildWarEnd(raid,guildbattle,0);
		else
			x002008_OnGuildWarEnd(raid,guildbattle,2);
		end
	end
end

function x002008_OnGuildWarEnd(raid,guildbattle,camp)
	guildbattle:SortPlayerRank();
	raid:SetRaidTimer(999, 600, -1);
	if camp == 0 then
		guildbattle:GetBattleInfo():set_win_guildid(g_max_guild_id);
		local attackers = raid:GetAttackers()
		for i,player in pairs(attackers) do
			guildbattle:PlayerOnGuildWarEnd(player,false);
			player:SendStatisticsResult(ActivityType.UnionBattle,true);	
			guildbattle:SendGuildBattleInfo(player);
		end
		local defenders = raid:GetDefenders()
		for i,player in pairs(defenders) do
			guildbattle:PlayerOnGuildWarEnd(player,false);
			player:SendStatisticsResult(ActivityType.UnionBattle,true);	
			guildbattle:SendGuildBattleInfo(player);
		end
		
		LuaServer():SendS2GCommonMessage("S2GGuildBattleWin", { 0,guildbattle:GetAttackerScore(),#attackers,guildbattle:GetAttackerCount() }, {guildbattle:GetAttackerGuid()}, {});
		LuaServer():SendS2GCommonMessage("S2GGuildBattleWin", { 0,guildbattle:GetDefenderScore(),#defenders,guildbattle:GetDefenderCount() }, {guildbattle:GetDefenderGuid()}, {});
		LuaServer():LuaLog("GuildWarEnd Lost="..tostring(guildbattle:GetAttackerGuid()).. " Lost="..tostring(guildbattle:GetDefenderGuid()),g_log_level.LL_INFO)
	elseif camp == 1 then
		guildbattle:GetBattleInfo():set_win_guildid(guildbattle:GetAttackerGuid());
		local attackers = raid:GetAttackers()
		for i,player in pairs(attackers) do
			guildbattle:PlayerOnGuildWarEnd(player,true);
			player:SendStatisticsResult(ActivityType.UnionBattle,true);	
			guildbattle:SendGuildBattleInfo(player);		
		end
		local defenders = raid:GetDefenders()
		for i,player in pairs(defenders) do
			guildbattle:PlayerOnGuildWarEnd(player,false);
			player:SendStatisticsResult(ActivityType.UnionBattle,true);
			guildbattle:SendGuildBattleInfo(player);			
		end
		LuaServer():SendS2GCommonMessage("S2GGuildBattleWin", { 1,guildbattle:GetAttackerScore(),#attackers,guildbattle:GetAttackerCount() }, {guildbattle:GetAttackerGuid()}, {});
		LuaServer():SendS2GCommonMessage("S2GGuildBattleWin", { 0,guildbattle:GetDefenderScore(),#defenders,guildbattle:GetDefenderCount() }, {guildbattle:GetDefenderGuid()}, {});
		LuaServer():LuaLog("GuildWarEnd Win="..tostring(guildbattle:GetAttackerGuid()).. " Lost="..tostring(guildbattle:GetDefenderGuid()),g_log_level.LL_INFO)
	elseif camp == 2 then
		guildbattle:GetBattleInfo():set_win_guildid(guildbattle:GetDefenderGuid());
		local attackers = raid:GetAttackers()
		for i,player in pairs(attackers) do
			guildbattle:PlayerOnGuildWarEnd(player,false);
			player:SendStatisticsResult(ActivityType.UnionBattle,true);	
			guildbattle:SendGuildBattleInfo(player);
		end
		local defenders = raid:GetDefenders()
		for i,player in pairs(defenders) do
			guildbattle:PlayerOnGuildWarEnd(player,true);
			player:SendStatisticsResult(ActivityType.UnionBattle,true);	
			guildbattle:SendGuildBattleInfo(player);
		end
		
		LuaServer():SendS2GCommonMessage("S2GGuildBattleWin", { 0,guildbattle:GetAttackerScore(),#attackers,guildbattle:GetAttackerCount() }, {guildbattle:GetAttackerGuid()}, {});
		LuaServer():SendS2GCommonMessage("S2GGuildBattleWin", { 1,guildbattle:GetDefenderScore(),#defenders,guildbattle:GetDefenderCount() }, {guildbattle:GetDefenderGuid()}, {});
		LuaServer():LuaLog("GuildWarEnd Win="..tostring(guildbattle:GetDefenderGuid()).." Lost="..tostring(guildbattle:GetAttackerGuid()),g_log_level.LL_INFO)
	end
	
	local pos_node_list = LuaMtMonsterManager():GenRandomPos(34, 100);
	--创建动态npc
	for k, node in pairs(pos_node_list)	do
		raid:AddNpc(node:pos_id(),19997,-1);
	end
end


function x002008_OnEnter(player,raid)

end

function x002008_OnLeave(player,raid)
	
end

function x002008_SendInfoToPlayer(player,raid)
	for i=1,#bufftable do
		local cdtime = raid:GetParams32(bufftable[i])
		if cdtime <= 0 then
			player:SendLuaPacket("NewBuffPoint",{tonumber(bufftable[i])},{},{});
		end
	end
end

function x002008_OnClientEnterOk(player, raid)
	local guildid = player:GetGuildID();
	local guildbattle = LuaMtGuildManager():GetGuildBattle(guildid);
	if guildbattle == nil then
		return;
	end
	local battlestage = LuaMtGuildManager():GetGuildBattleStage();
	local mobility = 40;
	player:SendGuildBattleStage(battlestage,false);
	local frist = guildbattle:EnterPlayer(player:Guid(),player:Name(),mobility,guildid,player:GetScenePlayerInfo():init_actor_id());
	if frist == true then
		player:OnLuaFunCall_1(501, "AddActivityTimes", ActivityType.UnionBattle);
	end
	guildbattle:SendGuildBattleInfo(player);
	guildbattle:UpdatePlayerList();
	x002008_SendInfoToPlayer(player,raid);

end

function x002008_ReConnected(raid,player)
	local guildid = player:GetGuildID();
	local guildbattle = LuaMtGuildManager():GetGuildBattle(guildid);
	if guildbattle == nil then
		return ResultCode_ResultOK;
	end
	guildbattle:SendGuildBattleInfo(player);
	guildbattle:UpdatePlayerList();
	x002008_SendInfoToPlayer(player,raid);
	return ResultCode_ResultOK
end

function x002008_OnDestroy(raid)
end

function x002008_OnGenBuff(raid)
end

function x002008_OnBuffPoint(player,raid,posindex)
	if player then
		for i=1,#bufftable do
			if tonumber(bufftable[i]) == posindex then
				local cdtime = raid:GetParams32(bufftable[i])
				if cdtime <= 0 then
					local proxy = player:GetSimpleImpactSet();
					if proxy then
						raid:BroadCastLuaPacket("HideBuffPoint",{tonumber(bufftable[i])},{},{});
						raid:SetParams32(bufftable[i],60);
						proxy:AddSimpleImpact(1001, 10, 2000, 0, false);
					end
				end
			end
		end
	end
end