
function x002610_OnJoinCheck(player,params)

	local activity_type =params["activity_type"]
	--��������ж�
	local ac_config = LuaActivityManager():FindActivity(activity_type)
	if ac_config == nil then
		return ResultCode_InternalResult
	end
	
	local ac_name = ac_config:name()
		
	--�ȼ����
	if player:PlayerLevel()<ac_config:open_level() then
		return player:SendClientNotify("ac_notify_002|"..math.floor(ac_config:open_level()).."|"..ac_name,-1,1)
	end
	
	--��������ж�
	local now_time = LuaMtTimerManager():Time2Num()
	local end_time = ac_config:day_end_time()
	local begin_time = ac_config:day_begin_time()
	
	
	local newtime = x000010_TimeToString(begin_time)
	
	--print("now_time"..now_time.." end_time"..end_time.. " begin_time"..newtime)
	
	if now_time> tonumber(end_time) then				--ac_notify_011,{0}��ѽ���
		return player:SendClientNotify("ac_notify_011|"..ac_name,-1,1)
	end
	
	local ret= LuaActivityManager():IsActive(ac_config, player:PlayerLevel()) 
	if ret ~= ResultCode_ResultOK then					--ac_notify_013,{0}�δ����������{1}�ſ��Խ���
		return player:SendClientNotify("ac_notify_013|"..ac_name.."|"..newtime,-1,1)
	end
	
	
	--�����ж�,ֻ���Լ�
	local ltimes = ac_config:limit_times()
	local rtimes = ac_config:reward_count();
	local done_times = player:OnLuaFunCall_1(501, "GetActivityTimes", activity_type)
	print("ltimes"..ltimes.." rtimes"..rtimes.. " done_times"..done_times)
	if ltimes >= 0 then
		if done_times >= ltimes then	
			if activity_type == ActivityType.Ancholage then				--ac_notify_009,���Ѿ����{0}�Ųر�ͼ�����������ɡ�
				return player:SendClientNotify("ac_notify_009|"..ac_name,-1,1)
			elseif activity_type == ActivityType.AcUnionMission then	--ac_notify_010,�������{0}�ι����������������ɡ�
				return player:SendClientNotify("ac_notify_010|"..ac_name,-1,1)
			else														--�����������{0}������������
				return player:SendClientNotify("ac_notify_007|"..ac_name,-1,1)
			end
		end
	end	
	
	
	return ResultCode_ResultOK
end
