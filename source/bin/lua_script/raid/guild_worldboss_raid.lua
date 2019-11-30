
--竞技场副本框架
local left_time =2*3600 --结束时间

function x002006_OnCreate(raid)	
end

function x002006_OnRaidTimer(raid, index)
end

function x002006_OnBigTick(raid,elapseTime)	
	local begin_date = raid:GetBeginDate()
	local delta_time =  LuaMtTimerManager():DiffTimeToNow(begin_date)
	if delta_time>=left_time then
		raid:OnDestroy()
		return
	end	

end

function x002006_OnSecondTick(raid)	

end

function x002006_OnMinTick(raid)	

end

function x002006_OnEnter(player,raid)
	
	local scene = raid:Scene()
	if scene == nil then
		return
	end
	--print("x002006_OnEnter")
end

function x002006_OnClientEnterOk(player, raid)
	--player:OnGoGuildBossBattle();
	--重置血量
	local dc_container = player:DataCellContainer()
	if dc_container:check_bit_data(BitFlagCellIndex_DayTriGuildBossFlag) == false then	
		player:ResetPlayerActorLastHp(LastHpType_GBOSSLastHp)
	end
	
	player:SendCommonReply("CloseAllPanel", {}, {}, {});
	local battle = player:GetBattleManager():CreateFrontBattle(player,nil,
		{
			["script_id"]=1020,
			["group_id"] =1
		})
end

function x002006_ReConnected(raid,player)
	return ResultCode_ResultOK
end

function x002006_OnLeave(player,raid)
end

function x002006_OnDestroy(raid)
end

function x002006_OnGenBuff(raid)
end

function x002006_OnBuffPoint(player,raid,posindex)
end