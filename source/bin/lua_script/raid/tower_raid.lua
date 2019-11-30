
--试炼场副本框架
local left_time =2*3600 --结束时间

---------------------------------------------------
function x002009_OnCreate(raid)	
end

function x002009_OnRaidTimer(raid, index)
end

function x002009_OnBigTick(raid,elapseTime)	
	local begin_date =  raid:GetBeginDate()
	local delta_time =  LuaMtTimerManager():DiffTimeToNow(begin_date)
	if delta_time>=left_time then
		raid:OnDestroy()
	end	
end

function x002009_OnSecondTick(raid)	

end

function x002009_OnMinTick(raid)	

end

function x002009_OnEnter(player,raid)

end 

function x002009_GetPostionList(scene)
	return { 1140100001,1140100002,1140100003}
end

function x002009_OnClientEnterOk(player, raid)		
	player:GetMissionProxy():OnAcceptMission(MissionType_TowerMission)
end

function x002009_ReConnected(raid,player)
	return ResultCode_ResultOK
end

function x002009_OnLeave(player,raid)

end

function x002009_OnDestroy(raid)
end

function x002009_OnGenBuff(raid)
end

function x002009_OnBuffPoint(player,raid,posindex)
end