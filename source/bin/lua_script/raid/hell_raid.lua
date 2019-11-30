
--试炼场副本框架
local left_time =2*3600 --结束时间

---------------------------------------------------
function x002002_OnCreate(raid)	
end

function x002002_OnRaidTimer(raid, index)
end

function x002002_OnBigTick(raid,elapseTime)	
	local begin_date =  raid:GetBeginDate()
	local delta_time =  LuaMtTimerManager():DiffTimeToNow(begin_date)
	if delta_time>=left_time then
		raid:OnDestroy()
	end	
end

function x002002_OnSecondTick(raid)	

end

function x002002_OnMinTick(raid)	

end

function x002002_OnEnter(player,raid)

end 

function x002002_GetPostionList(scene)
	return { 1140100001,1140100002,1140100003}
end

function x002002_OnClientEnterOk(player, raid)	
	player:DelNpcByType("hell_box")
	if player:CheckRefreshNextWave() then
		--加载下一批
		local scene = player:Scene()
		scene:OnPlayerTrialInfoLoad(player:Guid())
	else
		player:RefreshPlayerNpc(false)
	end	
		
	--重置血量
	local dc_container = player:DataCellContainer()
	if dc_container:check_bit_data(BitFlagCellIndex_DayTriFieldFlag) == false then	
		player:ResetPlayerActorLastHp(LastHpType_HellLastHp)
	end
end

function x002002_ReConnected(raid,player)
	return ResultCode_ResultOK
end

function x002002_OnLeave(player,raid)
	player:DelNpcByType("hell_box")
end

function x002002_OnDestroy(raid)
end

function x002002_OnGenBuff(raid)
end

function x002002_OnBuffPoint(player,raid,posindex)
end