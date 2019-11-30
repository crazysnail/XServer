
--竞技场副本框架
local left_time =2*3600 --结束时间

function x002007_OnCreate(raid)	
end

function x002007_OnRaidTimer(raid, index)
end

function x002007_OnBigTick(raid,elapseTime)	
	local begin_date = raid:GetBeginDate()
	local delta_time =  LuaMtTimerManager():DiffTimeToNow(begin_date)
	if delta_time>=left_time then
		raid:OnDestroy()
		return
	end	

end

function x002007_OnSecondTick(raid)	

end

function x002007_OnMinTick(raid)	

end

function x002007_OnEnter(player,raid)
	
	local scene = raid:Scene()
	if scene == nil then
		return
	end
	--print("x002007_OnEnter")
end

function x002007_OnClientEnterOk(player, raid)
	player:OnSendWorldBossBattleReply();
	if player:GetWorldBossReliveCd() == 0 then
		local battle = player:GetBattleManager():CreateFrontBattle(player,nil,
		{
				["script_id"]=1011,
				["group_id"]=3
		})
		--player:OnGoWorldBossBattle();
	end
end

function x002007_ReConnected(raid,player)
	return ResultCode_ResultOK
end


function x002007_OnLeave(player,raid)
end

function x002007_OnDestroy(raid)
end

function x002007_OnGenBuff(raid)
end

function x002007_OnBuffPoint(player,raid,posindex)
end