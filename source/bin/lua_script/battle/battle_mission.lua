


function x001003_OnJoinCheck(player,params)
	local mission_type = params["mission_type"]
	if mission_type == -1 then
		return player:SendClientNotify("InvalidMissionType",-1,-1)
	end
			
	local mission_config = LuaMtMissionManager():Type2Mission(mission_type);
	if mission_config == nil then
		return ResultCode_InternalResult
	end
		
	params["mission_id"]=mission_config:id()
	
	return x001003_OnBattleCheck(player, params)
end

function x001003_OnBattleCheck(player,params)
	
	local mission_config = LuaMtMissionManager():FindMission(params["mission_id"])
	if mission_config == nil then
		return ResultCode_InternalResult
	end

	--人数相关检查
	local cimit = mission_config:count_limit()	
	local one_p = player:GetParams32("one_p")
	if one_p == 1 then
		cimit =1
	end
		
	local activity_type = player:OnLuaFunCall_1(501, "MissTypeToActivityType", mission_config:type())
	local ac_name ="!"
	if activity_type ~= -1 then
		local ac_config = LuaActivityManager():FindActivity(activity_type)
		if ac_config == nil then
			return ResultCode_InternalResult
		end
		ac_name = ac_config:name()
	end	
	if cimit==-1 then--必须单人
		if player:GetTeamID() ~= 0 or player:IsTeamGroup() then
			return player:SendClientNotify("ac_notify_005|"..ac_name,-1,1)
		end
	elseif cimit>=6 then		
		if player:IsTeamGroup() == false then 	--团队状态下才可参与{0}
			return player:SendClientNotify("ac_notify_006|"..ac_name,-1,1)
		end
	elseif cimit>=1 and one_p ~= 1 then 
		if player:IsTeamGroup() then 			--团队状态下不可参与{0}
			return player:SendClientNotify("ac_notify_003|"..ac_name,-1,1)
		end 
	end
	
	local members = {}
	if cimit>1 then								--人数检查
		members = player:GetTeamMember(true)
		if #members < cimit and  player:CurrentRobotCount() < cimit then
			return player:SendClientNotify("ac_notify_000|"..math.floor(cimit).."|"..ac_name,-1,1)
		end
		
		if player:IsTeamLeader()==false then	--队长检查
			return player:SendClientNotify("Team_NotLeader"-1,1)
		end		
	end

	
	--活动相关检查	
	local ac_config = LuaActivityManager():FindActivity(activity_type)
	if ac_config ~= nil then
		local scene = player:Scene()
		if scene == nil then
			return player:SendClientNotify("UnknownError",-1,1)
		end
		if player:PlayerLevel()<ac_config:open_level() then	--{0}级才可以进行{1}
			return player:SendClientNotify("ac_notify_002|"..math.floor(ac_config:open_level()).."|"..ac_name,-1,1)
		end
		
		--队员等级
		local pnames=""
		for i=1,#members,1 do
			if members[i] ~= player:Guid() then		
				local tplayer = LuaServer():FindPlayer(members[i])	--有可能这个时候其他人还在切换场景
				if tplayer == nil then
					return player:SendClientNotify("UnknownError",-1,1)
				end
				if tplayer:PlayerLevel()<ac_config:open_level() then
					--print(pnames)
					pnames= pnames..tplayer:Name().." "
				end
			end
		end
		
		if pnames ~= "" then	-- {0}等级不足{1}级，无法参加{2}
			--print("ac_notify_001|"..pnames.."|"..math.floor(ac_config:open_level()).."|"..ac_name)
			return player:SendClientNotify("ac_notify_001|"..pnames.."|"..math.floor(ac_config:open_level()).."|"..ac_name,-1,1)
		end
		
		
		--次数判定,只有自己
		local ltimes = ac_config:limit_times()
		local rtimes = ac_config:reward_count();
		local done_times = player:OnLuaFunCall_1(501, "GetActivityTimes", activity_type)
		print("ltimes"..ltimes.." rtimes"..rtimes.. " done_times"..done_times)
		if ltimes >= 0 and mission_config:type() ~= MissionType_ExtraRewardMission then --额外赏金任务不受次数影响
			if done_times >= ltimes then	
				if activity_type == ActivityType.Ancholage then				--ac_notify_009,你已经获得{0}张藏宝图，明天再来吧。
					return player:SendClientNotify("ac_notify_009|"..math.floor(ltimes),-1,1)
				elseif activity_type == ActivityType.AcUnionMission then	--ac_notify_010,你已完成{0}次公会任务，下周再来吧。
					return player:SendClientNotify("ac_notify_010|"..math.floor(ltimes),-1,1)
				else														--今日你已完成{0}，明日再来吧
					return player:SendClientNotify("ac_notify_007|"..ac_name,-1,1)
				end
			end
		end	
	
		--if rtimes > 0 and done_times >= rtimes then						--今日你已完成{0}，再次进入将不再获取任何奖励
		--	player:SendClientNotify("ac_notify_008|"..ac_name,-1,1)
		--end
	end
	
	if cimit>1 then	
		if mission_config:type() == MissionType_CorpsMission then
			--no todo!
		else
			if player:GetMissionProxy():OnTeamSyncCheck(mission_config:type()) == false then--任务同步检查
				return player:SendClientNotify("DiffMissionProcess",-1,-1)			
			end
		end
	end
	
	return ResultCode_ResultOK;	
end



function x001003_OnGoBattle(battle_ground,params)

	local player = battle_ground:GetCreator()
	if player == nil then
		return ResultCode_BattleCreateFailed
	end
	
	if params["group_id"] == nil then
		local layout = LuaMtMonsterManager():FindScenePos(params["pos_id"])
		if layout == nil then
			return ResultCode_InvalidConfigData
		end
		
		if layout:scene_id() ~= player:GetSceneId() then
			return ResultCode_InvalidConfigData
		end
		params["group_id"]=layout:group_index()
	end
	
	if params["mission_id"] ~=nil and params["mission_id"] >= 0 then
		local ret = x001003_OnBattleCheck(player,params)
		if ret ~= ResultCode_ResultOK then
			return ret
		end
	else
		params["mission_id"] = 0
	end
	
	if params["relate_npc"] == nil then  params["relate_npc"] = -1 end
	
	local battlegroup = LuaSceneManager():GetSceneBattleGroupById(player:GetSceneId(),params["group_id"]);
	if battlegroup == nil then
		return ResultCode_InvalidConfigData
	end
		
	player:StopMove()	
	
	battle_ground:SetScriptId(1003)		
	battle_ground:SetParams32("mission_id",params["mission_id"])		
	battle_ground:SetParams32("relate_npc",params["relate_npc"])		
	battle_ground:SetParams32("monster_group_id",params["monster_group_id"])		
	
	local brief = battle_ground:Brief()
	brief:set_scene_id(player:GetSceneId())
	brief:set_mission_id(params["mission_id"])
	brief:set_battle_group_id(params["group_id"])		
	brief:set_type(BattleGroundType_PVE_SINGLE)

	--团队本超时10分钟
	if x000501_IsTeamCarbonMission(player,params["mission_id"]) then
		brief:set_time_out(60*10*1000)
	end
	
	if battle_ground:Init() == false then
		return ResultCode_BattleCreateFailed
	end		
	
	battle_ground:BattleEnterNotify()
	return ResultCode_ResultOK
end

function x001003_OnCreateReady(battle_ground,player)
		
	--根据不同的脚本类型创建不同的attacker	
	local monster_group_id = battle_ground:GetParams32("monster_group_id")
	local monster_group_config = LuaMtMonsterManager():FindMonsterGroup(monster_group_id);
	if monster_group_config == nil then
		return ResultCode_UnknownError;	
	end
	
	--组队，只上主角的战斗
	local team_main = false	
	local mission_id =battle_ground:GetParams32("mission_id")
	local default_formation = ActorFigthFormation_AFF_NORMAL_HOOK
	if mission_id > 0 then
		local mission = player:GetMissionById(mission_id)
		local config = mission:Config()
		if config:count_limit() >1 then
			team_main = true
		end
		if player:PlayerLevel() >= LuaMtConfigDataManager():FindConfigValue("inner_hook_start_level"):value1() then
			if config:type() == MissionType_RewardMission or config:type() == MissionType_ExtraRewardMission then --日常
				default_formation = ActorFigthFormation_AFF_PVE_REWARD_MISSION
			elseif config:type() == MissionType_ArchaeologyMission then --考古
				default_formation = ActorFigthFormation_AFF_PVE_ARCHAEOLOGY_MISSION
			elseif config:type() == MissionType_GoblinCarbonMission then --地精矿洞
				default_formation = ActorFigthFormation_AFF_PVE_GOBLIN_CARBON
			elseif config:type() == MissionType_BloodCarbonMission then --提尔之手
				default_formation = ActorFigthFormation_AFF_PVE_BLOOD_CARBON
			end
		end
	end

	if player:IsTeamLeader() then
		if team_main == true then
			battle_ground:CreateAttackerPlayerBattleActors(true,true,0)
		else
			battle_ground:CreateAttackerPlayerBattleActors(true,false,default_formation)
		end
	else
		battle_ground:CreateAttackerPlayerBattleActors(false,false,default_formation)
	end
	
	
	local infos ={}
	for i = 0,monster_group_config:monsters_size() - 1 do
		local monster_define = monster_group_config:monsters(i)
		if (monster_define:id() > 0) then
			infos[monster_define:loc()]=monster_define:id()
			--battle_ground:CreateDefenderMonster(monster_define:loc(), monster_define:id())
		end
	end
	battle_ground:CreateDefenderMonster(infos)
	return ResultCode_ResultOK;	
end

function x001003_OnDamageRefix(battle_ground,damage_info,killer)

end

function x001003_OnDamage(battle, attacker, source_guid, target_guid, damage)
	return 0;
end

function x001003_OnCheckBattleEnd(battle_ground)
	if battle_ground:TimeOut() then
		return 2;
	end
	local DeadCampID=battle_ground:GetAllDeadCampID();
	if DeadCampID == 1 then
		return 2;
	elseif DeadCampID == 2 then
		return 1;
	end
	return 0;
end

function x001003_ResultResolve(player, battle_ground)
	
	local monster_group_id = battle_ground:GetParams32("monster_group_id")
	local mission_id =battle_ground:GetParams32("mission_id")
	
	--print("----------------mission_id"..mission_id)	

	--print("----------------monster_group_id"..monster_group_id);
	
	local mission_map=player:GetMissionProxy():GetMissionMap();
	for k, mission in pairs(mission_map) do
		local mission_data = mission:MissionDB()
		local config = mission:Config();
		--print("xxxxxxxxxxxxxxxxxxmission_data:param(MissionDB_Param_Monster_Group)"..mission_data:param(MissionDB_Param_Monster_Group))
		--print("xxxxxxxxxxxxxxxxxxmonster_group_id"..monster_group_id)
		if config:type() == MissionType_TowerMission then				
			local dc_container = player:DataCellContainer()
			if dc_container ~= nil then
				local breif = battle_ground:Brief()
				local delta_time = breif:elapse_time()/1000
				--print("sssssssssssssss mission_type:"..config:type().." delta_time"..delta_time)
				dc_container:add_data_32(CellLogicType_MissionCell,MissionCellIndex.Tower_Time,delta_time,true)	
			end			
		end
			
		if mission_data:param(MissionDB_Param_Monster_Group) == monster_group_id then
			mission_data:set_state(MissionState_MissionComplete)
			if  config:finish_type()==FinishType_RandomNpc then
				player:DelNpc(mission_data:param(MissionDB_Param_Npc_Series))
				LuaServer():LuaLog("guid="..player:Guid().." level="..math.floor(player:PlayerLevel()).." mission_id="..math.floor(config:id()),g_log_level.LL_INFO)
			end
			if config:commit_npc_id() <0 and config:type() ~= MissionType_ScrollMission then
				--没有提交npc立即完成
				player:GetMissionProxy():OnFinishMission(config:id())					
			else
				--需要通过对话提交
				player:GetMissionProxy():OnUpdateMission(config:id())
			end	
		end	

		
	end	
end

function x001003_OnBattleReward(battle_ground,winnercamp)
	
	local winners = nil
	if winnercamp ==1 then
		winners=battle_ground:GetAttackerPlayers()
	else		
		return false
	end

	if winners == nil then
		return false
	end
	
	--团队奖励优先
	--团队分配逻辑
	local leader = nil
	local all_names =""
	for k, player in pairs(winners) do
		--print("shitshitshitshitshitshitshitshitshit"..player:Name())
		if k > 1 then
			all_names = all_names..","
		end
		all_names = all_names..player:Name()
		
		if player:IsTeamLeader() then
			leader = player
		end
	end
	
	--广播	
	local creator = battle_ground:GetCreator()
	if creator ~= nil then
		local mission_id =battle_ground:GetParams32("mission_id")
		local relate_npc = battle_ground:GetParams32("relate_npc")
		--print("----------------mission_id"..mission_id)	
		--print("----------------relate_npc"..relate_npc)	
		if x000501_IsTeamCarbonMission(creator,mission_id) then
			local npc_config = LuaMtMonsterManager():FindDynamicNpc(relate_npc)
			local mission_config = LuaMtMissionManager():FindMission(mission_id)
			if npc_config ~= nil and mission_config ~= nil then				
				local activity_type = creator:OnLuaFunCall_1(501, "MissTypeToActivityType", mission_config:type())
				local ac_config = LuaActivityManager():FindActivity(activity_type)
				if ac_config ~= nil then
					ac_name = ac_config:name()
					Lua_send_run_massage("ac_notify_029|"..all_names.."|"..ac_name.."|monster_name_"..math.floor(npc_config:npc_id()))
				end
			end
		end
	end	

	--这里涉及任务进度同步问题，保证最后结算队长奖励就行，
	--队长奖励结算结束就会同步任务给队员，
	--如果先结算队长会导致队员无法正常结算本次战斗结果	
	for k, player in pairs(winners) do
		if player:IsTeamLeader() == false then
			x001003_ResultResolve(player,battle_ground)
		end		
	end	
	if leader ~= nil then
		x001003_ResultResolve(leader,battle_ground)
	end
	
	return true
end	
	
function x001003_OnBattleEnd(battle_ground,force,winnercamp)
	--通知客户端结束战斗
	battle_ground:BattleOverNotify(winnercamp)
	
	if force ~= true then
		--主线副本失败特殊处理
		local mission_id =battle_ground:GetParams32("mission_id")
		if mission_id == 10013 or mission_id == 10062 then
			local player=battle_ground:GetCreator()
			if player ~= nil then
				player:DismissTeam()	
			end
			local mission = player:GetMissionProxy():GetMissionById(mission_id)
			if mission ~= nil then
				local mission_data = mission:MissionDB()	
				mission_data:set_state(MissionState_MissionComplete)
				player:GetMissionProxy():OnFinishMission(mission_id)					
			end
			if mission_id==10013 then
				player:GetMissionProxy():OnAddMission(30001)
			elseif mission_id== 10062 then
				player:GetMissionProxy():OnAddMission(40051)
			end		
		end
	end
		
	return true;
end
