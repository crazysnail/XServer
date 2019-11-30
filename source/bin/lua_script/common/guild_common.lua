x000007_GuildBattle_List=2000
x000007_GuildBattle_Prepare=2050
x000007_GuildBattle_War =2100
x000007_GuildBattle_End =2140
x000007_GuildBattle_None =2150

x000007_GuildBoss_Start=2100
x000007_GuildBoss_End=2130

function x000007_OnRegisterFunction(proxy)
	
	if proxy ~= nil then
		proxy:RegFunc("xReConnected"		,7)				
	end
end

function x000007_xReConnected(player,params)
	local battlestage = LuaMtGuildManager():GetGuildBattleStage();
	if battlestage == GuildBattle_Stage_GuildBattle_Stage_Prepare then
		if player:CheckGuildBattleQualify() then
			player:SendGuildBattleStage(battlestage, true);
		end
	end
	return ResultCode_ResultOK
end

function x000007_GuildBattleFinish()
	local alltimes = LuaServer():GetGuildBattleTimes();
	if alltimes > 0 then
		local count,rand = math.modf(alltimes/2);--每8周为一轮
		if rand == 0 then -- 每周四公会战结束
			--LuaServer():SendS2GCommonMessage("S2GGuildBattleRoundReward",{},{},{})
			LuaMtGuildManager():GuildBattleRoundReward();
		end
		if rand == 8 then --每8周为一轮
			--LuaServer():SendS2GCommonMessage("S2GClearGuildBattleRoundScore",{},{},{})	--清空本轮帮会战绩
			LuaMtGuildManager():ClearGuildBattleRoundScore();
		end
		if count == 24 then -- 24周
			--
			--LuaServer():SendS2GCommonMessage("S2GClearGuildBattleScore",{},{},{})	--清空总成绩帮会战绩
			LuaMtGuildManager():ClearGuildBattleScore();
		end
	end
end
--这个先放这里吧
function x000007_OnMinTriger(guild,mintime)
	local curTime = LuaMtTimerManager():Time2Num()
	local current_weekday = LuaMtTimerManager():GetWeekDay();
	if current_weekday == 2 or current_weekday == 4 then
		if curTime == x000007_GuildBattle_List then
			LuaMtGuildManager():GenGuildBattleList();
		elseif curTime == x000007_GuildBattle_Prepare then											--准备阶段
			LuaMtGuildManager():SetGuildBattleStage(GuildBattle_Stage_GuildBattle_Stage_Prepare);
		elseif curTime == x000007_GuildBattle_War then											--战斗阶段
			LuaMtGuildManager():SetGuildBattleStage(GuildBattle_Stage_GuildBattle_Stage_War);
		elseif curTime == x000007_GuildBattle_End then
			LuaMtGuildManager():SetGuildBattleStage(GuildBattle_Stage_GuildBattle_Stage_End);
		elseif curTime == x000007_GuildBattle_None then
			LuaMtGuildManager():SetGuildBattleStage(GuildBattle_Stage_GuildBattle_Stage_None);
			x000007_GuildBattleFinish();
		end
	end
	
	--工会boss
	--每天8点
	if current_weekday == 1 or current_weekday == 3 then
		if curTime == x000007_GuildBoss_Start then
			LuaMtGuildManager():OnStartGuildBoss();
		elseif curTime == x000007_GuildBoss_End then
			LuaMtGuildManager():OnEndGuildBoss();
		end
	end
end
