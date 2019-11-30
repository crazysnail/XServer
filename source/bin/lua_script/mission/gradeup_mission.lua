
--进阶任务
---------------------------------------------------
function x000515_xOnDrop(player, params)	
	return ResultCode_ResultOK
end
function x000515_xOnFinishBegin(player, params)	
	player:GetMissionProxy():OnMissionReward(params["id"])
	return ResultCode_ResultOK
end
function x000515_xOnReset(player, params)
	return ResultCode_ResultOK
end
function x000515_xOnAccept(player,params)
	return ResultCode_ResultOK
end
function x000515_xOnFinishEnd(player,params)
	return ResultCode_ResultOK
end

function x000515_GetMissionParams(player)
	 return {}
end
function x000515_GetNextMission(player)
	return -1
end

function x000515_OnAddBegin(player,config)
	return ResultCode_ResultOK
end

function x000515_OnClone(player, target, config)
	return ResultCode_ResultOK
end

function x000515_OnAddEnd(player,config)
	return ResultCode_ResultOK
end	
