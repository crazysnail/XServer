
--安邦任务
local activity_type=ActivityType.DayKill
local mission_type=MissionType_DayKillMission

-------------------------------------------------------
function x000516_xOnDrop(player, params)	
	return ResultCode_ResultOK
end
function x000516_xOnReset(player, params)
	return ResultCode_ResultOK
end

function x000516_xOnAccept(player, params)
	if params["type"] == mission_type then
		--检查身上有没有同类型任务，有就不生成新的
		local mission_map=player:GetMissionProxy():GetMissionMap()
		for k, mission in pairs(mission_map) do
			config = mission:Config()
			if config:type() == mission_type then
				return ResultCode_MissionCountLimit
			end
		end

		player:GetMissionProxy():OnAddMission(80000)
		
		return ResultCode_ResultOK
	end
	return ResultCode_InvalidMissionType
end


function x000516_xOnFinishEnd(player,params)
	return ResultCode_ResultOK
end

function x000516_xOnFinishBegin(player,params)
	local config = LuaMtMissionManager():FindMission(params["id"])
	if config:type() ==mission_type then	
		local container = player:DataCellContainer()
		if container ~= nil then
			--if container:check_bit_data(BitFlagCellIndex_FirstDayKillFlag) == false then
			--	container:set_bit_data(BitFlagCellIndex_FirstDayKillFlag,true)			
			--	--首杀奖励
			--	player:AddItemByIdWithNotify(20125023,1,ItemAddLogType_AddType_Activity,0,1)
			--	
			--	--获得下一个任务
			--	--local proxy = player:GetMissionProxy()
			--	--return proxy:OnAcceptDayKillMission()	
			--else
			local level_config = LuaMtActorConfig():GetLevelData(player:PlayerLevel())
			if level_config == nil then
				return ResultCode_InvalidConfigData
			end
			player:AddItemByIdWithNotify(TokenType_Token_Exp,level_config:daykill_exp(),ItemAddLogType_AddType_Activity,0,1)	
			--end
		end
		
		player:GetMissionProxy():OnMissionReward(params["id"])
				
		return ResultCode_ResultOK
	end
	return ResultCode_InvalidMissionType	
end

--------------------------------------------------------

function x000516_GetMissionParams(player)
	 return {}
end
function x000516_GetNextMission(player)
	return -1
end

function x000516_OnAddBegin(player,config)
	return ResultCode_ResultOK
end

function x000516_OnClone(player, target, config)
	--参与标记
	x000501_OnActivityJoin(player,activity_type)
	
	return ResultCode_ResultOK
end

function x000516_OnAddEnd(player,config)
				
	--接取成功时加次数
	if config:id() == 80000 then
		player:OnLuaFunCall_1(501,"AddActivityTimes",activity_type)	
	end
	--参与标记
	x000501_OnActivityJoin(player,activity_type)
	
	return ResultCode_ResultOK
end	


function x000516_ResolveAllReward(player)
	
	local level_config = LuaMtActorConfig():GetLevelData(player:PlayerLevel())
	if level_config == nil then
		return 0,0
	end

	local final_exp = level_config:daykill_exp()
	local final_gold = 0
		
	return final_gold,final_exp
end	
