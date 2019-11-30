
--工会副本框架
local left_time =2*3600 --结束时间

function x002005_OnCreate(raid)	
end

function x002005_OnRaidTimer(raid, index)
end

function x002005_OnBigTick(raid,elapseTime)	
	local begin_date = raid:GetBeginDate()
	local delta_time =  LuaMtTimerManager():DiffTimeToNow(begin_date)
	if delta_time>=left_time then
		raid:OnDestroy()
		return
	end	

end

function x002005_OnSecondTick(raid)	

end

function x002005_OnMinTick(raid)	

end

function x002005_OnEnter(player,raid)
	
	local scene = raid:Scene()
	if scene == nil then
		return
	end
end

function x002005_OnClientEnterOk(player, raid)
	player:SendCommonReply("CloseAllPanel", {}, {}, {});
	local battle = player:GetBattleManager():CreateFrontBattle(player,nil,
		{
			["script_id"]=1021,
			["group_id"] =1
		})
end

function x002005_ReConnected(raid,player)
	return ResultCode_ResultOK
end

function x002005_OnLeave(player,raid)
	--local guild = player:GetGuild();
	--local copyid = player:GetParams32("copyid")
	--if guild ~= nil then
	--	local guildwetcopy = guild:GetGuildWetCopyInfo(copyid);
	--	if guildwetcopy ~= nil then
	--		guildwetcopy:set_fightname("");
	--	end 
	--end
end

function x002005_OnDestroy(raid)
end

function x002005_OnGenBuff(raid)
end

function x002005_OnBuffPoint(player,raid,posindex)
end