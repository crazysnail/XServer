
--任务副本框架
local left_time =2*3600 --结束时间

function x002001_OnCreate(raid)	
end

function x002001_OnRaidTimer(raid, index)
end

function x002001_OnBigTick(raid,elapseTime)
	
	local begin_date = raid:GetBeginDate()
	local delta_time =  LuaMtTimerManager():DiffTimeToNow(begin_date)
	if delta_time>=left_time then
		raid:OnDestroy()
	end
	
end

function x002001_OnSecondTick(raid)	

end

function x002001_OnMinTick(raid)	

end

function x002001_OnEnter(player,raid)
	
	local scene = raid:Scene()
	if scene == nil then
		return
	end
	
	local sceneid = scene:SceneId();
	local proxy = player:GetMissionProxy();
	local mission_map=proxy:GetMissionMap()

	for k, mission in pairs(mission_map) do
		local config = mission:Config()
		local mission_data = mission:MissionDB()
		if config:finish_type()==FinishType_EnterRaid then				
			--print("config:param(1)"..config:param(1))
			--print("scene:SceneId()"..sceneid)
			if sceneid == config:param(1) then
				mission_data:set_param(MissionDB_Param_Finish_Value,1)
				mission_data:set_state(MissionState_MissionComplete)
				
				--无视提交npc存在
				proxy:OnFinishMission(config:id())	
			end
		end		
	end	
	
	--组队情况下进入副本需要同步队长当前任务进度
	if player:GetTeamID() ~= 0 then	
		proxy:OnCloneMissionFromLeader()
	end

	
end
function x002001_OnClientEnterOk(player, raid)
end

function x002001_ReConnected(raid,player)
	return ResultCode_ResultOK
end

function x002001_OnLeave(player,raid)
end

function x002001_OnDestroy(raid)
end

function x002001_OnGenBuff(raid)
end

function x002001_OnBuffPoint(player,raid,posindex)
end
