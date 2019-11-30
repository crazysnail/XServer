
--竞技场副本框架
local left_time =2*3600 --结束时间

function x002003_OnCreate(raid)	
end

function x002003_OnRaidTimer(raid,index)	
end

function x002003_OnBigTick(raid,elapseTime)	
	local begin_date = raid:GetBeginDate()
	local delta_time =  LuaMtTimerManager():DiffTimeToNow(begin_date)
	if delta_time>=left_time then
		raid:OnDestroy()
		return
	end	
end

function x002003_OnSecondTick(raid)	

end

function x002003_OnMinTick(raid)	

end

function x002003_OnEnter(player,raid)
	
	local scene = raid:Scene()
	if scene == nil then
		return
	end
end

function x002003_OnClientEnterOk(player, raid)

end

function x002003_ReConnected(raid,player)
	return ResultCode_ResultOK
end

function x002003_OnLeave(player,raid)
end

function x002003_OnDestroy(raid)
end

function x002003_OnGenBuff(raid)
end

function x002003_OnBuffPoint(player,raid,posindex)
end
