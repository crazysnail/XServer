--工会任务
x000506_reward_money = {
							{3120,3440,3760,4080,4400,4720,5040,5360,5680,6000},
							{3560,3920,4280,4640,5000,5360,5720,6080,6440,6800},
							{4000,4400,4800,5200,5600,6000,6400,6800,7200,7600},
							{4440,4880,5320,5760,6200,6640,7080,7520,7960,8400},
							{4880,5360,5840,6320,6800,7280,7760,8240,8720,9200},
							{5320,5840,6360,6880,7400,7920,8440,8960,9480,10000},
						};

x000506_reward_cntribute = {3,4,5,6,7,8,9,10,11,17};
x000506_cashbox_fix = {0.1, 0.18, 0.26, 0.34, 0.42, 0.5};

local mission_type = MissionType_UnionMission
local activity_type = ActivityType.AcUnionMission

----------------------------------------------------
function x000506_xOnDrop(player, params)	
	return ResultCode_ResultOK
end
function x000506_xOnFinishEnd(player, params)	
	return ResultCode_ResultOK
end
function x000506_xOnReset(player, params)
	if params["type"] == mission_type then
		local cell_container= player:DataCellContainer()
		cell_container:set_data_32(CellLogicType_MissionCell,MissionCellIndex.Guild_Circleid,0,true)
		return ResultCode_ResultOK
	end
	return ResultCode_InvalidMissionType
end

function x000506_xOnAccept(player, params)	
	if params["type"] == mission_type then
		local guildid = player:GetGuildID();
		if guildid == INVALID_GUID then
			return ResultCode_Guild_NoHave
		end
		--检查身上有没有同类型任务，有就不生成新的
		local mission_map=player:GetMissionProxy():GetMissionMap()	
		for k, mission in pairs(mission_map) do
			local config = mission:Config()
			if config:type() == MissionType_UnionMission then
				return ResultCode_MissionCountLimit
			end
		end
		local cell_container= player:DataCellContainer();
		local missioncricleid = cell_container:get_data_32(CellLogicType_MissionCell,MissionCellIndex.Guild_Circleid);
		missioncricleid = missioncricleid + 1;
		if missioncricleid > 10 then
			missioncricleid = 1;
		end
		local missionid = LuaMtMissionManager():GenGuildMission(missioncricleid);
		if missionid == -1 then
			return ResultCode_InvalidMissionType
		end
		player:GetMissionProxy():OnAddMission(missionid);
		
		
		return ResultCode_ResultOK
	end
	return ResultCode_InvalidMissionType
end

function x000506_xOnFinishBegin(player,params)
	local config = LuaMtMissionManager():FindMission(params["id"])
	if config:type() ==mission_type then	
		local cashboxlevel = player:GetPlayerGuildProxy():GetBuildLevel(GuildBuildType_GUILD_CASHBOX);
		local cell_container= player:DataCellContainer();
		local missioncricleid = cell_container:get_data_32(CellLogicType_MissionCell,MissionCellIndex.Guild_Circleid)+1
		if missioncricleid>10 then
			missioncricleid = 1
		end
		
		local playerlevel = player:PlayerLevel();
		local addmoney = 0;
		if playerlevel >= 10 and playerlevel <=19 then
			addmoney = x000506_reward_money[1][missioncricleid];
		elseif playerlevel >= 20 and playerlevel <= 29 then
			addmoney = x000506_reward_money[2][missioncricleid];
		elseif playerlevel >= 30 and playerlevel <= 39 then
			addmoney = x000506_reward_money[3][missioncricleid];
		elseif playerlevel >= 40 and playerlevel <= 49 then
			addmoney = x000506_reward_money[4][missioncricleid];
		elseif playerlevel >= 50 and playerlevel <= 59 then
			addmoney = x000506_reward_money[5][missioncricleid];
		elseif playerlevel >= 60 then
			addmoney = x000506_reward_money[6][missioncricleid];
		end
		player:AddItemByIdWithNotify(TokenType_Token_GuildMoney,addmoney,ItemAddLogType_AddType_Activity,0,1);
		player:AddItemByIdWithNotify(TokenType_Token_GuildContribute,x000506_reward_cntribute[missioncricleid],ItemAddLogType_AddType_Activity,activity_type,1);
		player:AddItemByIdWithNotify(TokenType_Token_GuildBonus,addmoney*x000506_cashbox_fix[cashboxlevel+1],ItemAddLogType_AddType_Activity,0,1);
		
		--更新环数
		cell_container:set_data_32(CellLogicType_MissionCell,MissionCellIndex.Guild_Circleid,missioncricleid,true);
		
		local level_config = LuaMtActorConfig():GetLevelData(player:PlayerLevel())
		if level_config == nil then
			return ResultCode_InvalidConfigData
		end
		--个人奖励
		local exp = level_config:guild_exp()
		local exp_rate = config:exp_rate()
		if exp_rate>=0 then
			exp = exp_rate*exp;
		end			
		player:AddItemByIdWithNotify(TokenType_Token_Exp,exp,ItemAddLogType_AddType_Activity,activity_type,1);
		
		--队长而外经验
		if player:IsTeamLeader() then
			player:GetMissionProxy():TeamLeaderExtraReward(config,exp);
		end
		
		--金币奖励
		local gold = level_config:guild_gold()
		local gold_rate = config:gold_rate()
		if gold_rate >=0 then
			gold = gold_rate*gold;
		end			
		player:AddItemByIdWithNotify(TokenType_Token_Gold,gold,ItemAddLogType_AddType_Activity,activity_type,1);
		
		player:GetMissionProxy():OnMissionReward(params["id"])
		
		--任务次数	
		player:OnLuaFunCall_1(501,"AddActivityTimes",activity_type);
		
		--十个一结算
		if missioncricleid == 10 then
			player:SendStatisticsResult(activity_type,true)
		end

		return ResultCode_ResultOK
	end
	return ResultCode_InvalidMissionType	
end

------------------------------------------------------------------------

function x000506_GetMissionParams(player)
	 return {}
end
function x000506_GetNextMission(player)
	return -1
end

function x000506_OnAddBegin(player,config)
	return ResultCode_ResultOK
end

function x000506_OnClone(player, target, config)	
	return ResultCode_ResultOK
end

function x000506_OnAddEnd(player,config)	
	return ResultCode_ResultOK
end


