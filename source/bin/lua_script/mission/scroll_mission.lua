
--卷轴任务
---------------------------------------------------
function x000528_xOnDrop(player, params)	
	return ResultCode_ResultOK
end
function x000528_xOnFinishBegin(player, params)	
	player:GetMissionProxy():OnMissionReward(params["id"])
	return ResultCode_ResultOK
end
function x000528_xOnReset(player, params)
	return ResultCode_ResultOK
end
function x000528_xOnAccept(player,params)
	return ResultCode_ResultOK
end
function x000528_xOnFinishEnd(player,params)
	return ResultCode_ResultOK
end

function x000528_GetMissionParams(player)
	 return {}
end
function x000528_GetNextMission(player)
	return -1
end

function x000528_OnAddBegin(player,config)
	return ResultCode_ResultOK
end

function x000528_OnClone(player, target, config)
	return ResultCode_ResultOK
end

function x000528_OnAddEnd(player,config)
	return ResultCode_ResultOK
end	
