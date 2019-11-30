
--主线任务

---------------------------------------------------------

function x000503_xOnDrop(player, params)	
	return ResultCode_ResultOK
end
function x000503_xOnFinishBegin(player, params)	
	
	local mission = player:GetMissionById( params["id"] )
	if mission == nil then
		return ResultCode_InvalidMission
	end
	
	player:GetMissionProxy():OnMissionReward(params["id"])
	local config = mission:Config()	
	local mission_type = config:type()
	if mission_type == MissionType_MainMission or mission_type == MissionType_PartMission then	
					
		--保存进度
		local index = LuaMtMissionManager():GetMissionIndex(config:id())
		if index < 0 or index>3 then
			return ResultCode_InvalidMissionId
		end	
		local cell_container= player:DataCellContainer()
		
		--保证不能高于历史最高进度，防止刷任务
		local old = cell_container:get_data_32(CellLogicType_MissionCell,index)
		if old<config:id() then
			cell_container:set_data_32(CellLogicType_MissionCell,index,config:id(),true)
		end
		
		--首次
		local new_flag = 0
		local flag = cell_container:get_data_64(Cell64LogicType_NumberCell64, NumberCell64Index_GuideFirstFlag)
		if  config:id() == 20207 then
			new_flag = cell_container:set_bit_data_64(flag,g_GuideFirstFlag.Mission20207)
			cell_container:set_data_64(Cell64LogicType_NumberCell64, NumberCell64Index_GuideFirstFlag,new_flag,true)
		elseif  config:id() == 20257 then
			new_flag = cell_container:set_bit_data_64(flag,g_GuideFirstFlag.Mission20257)
			cell_container:set_data_64(Cell64LogicType_NumberCell64, NumberCell64Index_GuideFirstFlag,new_flag,true)
		elseif  config:id() == 9901 or config:id() == 9951 then
			new_flag = cell_container:set_bit_data_64(flag,g_GuideFirstFlag.PreMission1)
			cell_container:set_data_64(Cell64LogicType_NumberCell64, NumberCell64Index_GuideFirstFlag,new_flag,true)
		elseif  config:id() == 10001 or config:id() == 10051 then
			new_flag = cell_container:set_bit_data_64(flag,g_GuideFirstFlag.PreMission2)
			cell_container:set_data_64(Cell64LogicType_NumberCell64, NumberCell64Index_GuideFirstFlag,new_flag,true)
		end
	end
	return ResultCode_ResultOK

end

function x000503_xOnReset(player, params)	
	local mission_type = params["type"]
	if mission_type == MissionType_MainMission or mission_type == MissionType_PartMission then	
		local cell_container= player:DataCellContainer()	
		cell_container:set_data_32(CellLogicType_MissionCell,MissionCellIndex.MainLast,-1,true)
		cell_container:set_data_32(CellLogicType_MissionCell,MissionCellIndex.Part1Last,-1,true)
		cell_container:set_data_32(CellLogicType_MissionCell,MissionCellIndex.Part2Last,-1,true)
		cell_container:set_data_32(CellLogicType_MissionCell,MissionCellIndex.Part3Last,-1,true)
		
		player:GetMissionProxy():OnNextMainMission()	
		return ResultCode_ResultOK		
	end
	return ResultCode_InvalidMissionType
end

function x000503_xOnAccept(player, params)
	local mission_type = params["type"]
	if mission_type == MissionType_MainMission or mission_type == MissionType_PartMission then	
		--触发新的后续任务
		player:GetMissionProxy():OnNextMainMission()	
		return ResultCode_ResultOK
	end
	return ResultCode_InvalidMissionType
end	

function x000503_xOnFinishEnd(player,params)
	--特殊任务处理
	local id = tonumber(params["id"])
	if id == 10006 or id == 10055 then
		local form = player:GetActorFormation(ActorFigthFormation_AFF_NORMAL_HOOK)
		local scene = player:Scene()
		if form ~= nil and scene ~= nil then
			local actors = player:Actors()
			for k,v in pairs(actors) do
				if v ~= nil then
					form:set_actor_guids(k-1,v:Guid())
					if k>=5 then
						break
					end
				end
			end

			player:SendMessage(form)
			player:OnLuaFunCall_x("xOnSetActorFight",{["value"]=form:actor_guids_size()})
			player:UpdateBattleScore(false)
		end
	elseif id == 10011 then
		--if player:GetTeamID() == 0 then
		--	player:CreateRobotTeam(1)		
		--else
		--	LuaServer():SendS2GCommonMessage("S2GLeaveTeam", {}, {player:Guid()}, {});
		--end
		--生成副本任务
		x000513_xOnReset(player, {["mission_type"]=MissionType_CommonCarbon})	
		player:GetMissionProxy():OnAddMission(10012)
	elseif id == 10060 then
		--if player:GetTeamID() == 0 then
		--	player:CreateRobotTeam(1)		
		--else
		--	LuaServer():SendS2GCommonMessage("S2GLeaveTeam", {}, {player:Guid()}, {});
		--end
		--生成副本任务
		x000513_xOnReset(player, {["mission_type"]=MissionType_CommonCarbon})	
		player:GetMissionProxy():OnAddMission(10061)
	end
	return ResultCode_ResultOK
end

-------------------------------------
-------------------------------------
function x000503_GetMissionParams(player)
	 return {MissionCellIndex.MainLast,MissionCellIndex.Part1Last,MissionCellIndex.Part2Last,MissionCellIndex.Part3Last}
end

function x000503_OnAddBegin(player,config)
	
	if config:type() ~= MissionType_MainMission and config:type() ~= MissionType_PartMission then
		return ResultCode_InvalidMissionType
	end
	
	local mission_id = config:id()
	--不允许重复接任务
	local index = LuaMtMissionManager():GetMissionIndex(mission_id)
	--print(index)
	if index < 0 or index>3 then
		return ResultCode_InvalidMissionId
	end

	return ResultCode_ResultOK
end

function x000503_OnClone(player, target, config)
	return ResultCode_ResultOK
end

function x000503_OnAddEnd(player,config)
		
	--行为任务完成检查
	if config:finish_type()==FinishType_Action then
		player:SetParams32("CheckMission",1)
	end
	
	return ResultCode_ResultOK
end


