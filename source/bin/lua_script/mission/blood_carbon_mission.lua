
--提尔之手副本任务
local begin_mission = 64001
local time_limit = 2*3600
local activity_type=ActivityType.BloodCarbon
local mission_type=MissionType_BloodCarbonMission

local process = MissionCellIndex.CarbonBlood_MaxMissionId
local last_id = MissionCellIndex.CarbonBlood_Last


local potion_table={
{17,{6.74	,0	,0},{5.39	,1.62	,0},{4.72	,1.62	,0.27	},{4.04		,2.43	,0.54	},{3.37		,1.62	,0.81	},{2.7		,0.81	,1.08	}},
{18,{7		,0	,0},{5.6	,1.68	,0},{4.9	,1.68	,0.28	},{4.2		,2.52	,0.56	},{3.5		,1.68	,0.84	},{2.8		,0.84	,1.12	}},
{19,{7.26	,0	,0},{5.81	,1.74	,0},{5.08	,1.74	,0.29	},{4.35		,2.61	,0.58	},{3.63		,1.74	,0.87	},{2.9		,0.87	,1.16	}},
{20,{7.52	,0	,0},{6.01	,1.8	,0},{5.26	,1.8	,0.3	},{4.51		,2.71	,0.6	},{3.76		,1.8	,0.9	},{3.01		,0.9	,1.2	}},
{21,{7.78	,0	,0},{6.22	,1.87	,0},{5.44	,1.87	,0.31	},{4.67		,2.8	,0.62	},{3.89		,1.87	,0.93	},{3.11		,0.93	,1.24	}},
{22,{8.04	,0	,0},{6.43	,1.93	,0},{5.62	,1.93	,0.32	},{4.82		,2.89	,0.64	},{4.02		,1.93	,0.96	},{3.21		,0.96	,1.29	}},
{23,{8.29	,0	,0},{6.64	,1.99	,0},{5.81	,1.99	,0.33	},{4.98		,2.99	,0.66	},{4.15		,1.99	,1		},{3.32		,1		,1.33	}},
{24,{8.55	,0	,0},{6.84	,2.05	,0},{5.99	,2.05	,0.34	},{5.13		,3.08	,0.68	},{4.28		,2.05	,1.03	},{3.42		,1.03	,1.37	}},
{25,{8.81	,0	,0},{7.05	,2.12	,0},{6.17	,2.12	,0.35	},{5.29		,3.17	,0.71	},{4.41		,2.12	,1.06	},{3.53		,1.06	,1.41	}},
{26,{9.07	,0	,0},{7.26	,2.18	,0},{6.35	,2.18	,0.36	},{5.44		,3.27	,0.73	},{4.54		,2.18	,1.09	},{3.63		,1.09	,1.45	}},
{27,{9.33	,0	,0},{7.46	,2.24	,0},{6.53	,2.24	,0.37	},{5.6		,3.36	,0.75	},{4.67		,2.24	,1.12	},{3.73		,1.12	,1.49	}},
{28,{9.59	,0	,0},{7.67	,2.3	,0},{6.71	,2.3	,0.38	},{5.75		,3.45	,0.77	},{4.8		,2.3	,1.15	},{3.84		,1.15	,1.53	}},
{29,{9.85	,0	,0},{7.88	,2.36	,0},{6.89	,2.36	,0.39	},{5.91		,3.55	,0.79	},{4.92		,2.36	,1.18	},{3.94		,1.18	,1.58	}},
{30,{10.11	,0	,0},{8.09	,2.43	,0},{7.08	,2.43	,0.4	},{6.07		,3.64	,0.81	},{5.05		,2.43	,1.21	},{4.04		,1.21	,1.62	}},
{31,{10.37	,0	,0},{8.29	,2.49	,0},{7.26	,2.49	,0.41	},{6.22		,3.73	,0.83	},{5.18		,2.49	,1.24	},{4.15		,1.24	,1.66	}},
{32,{10.63	,0	,0},{8.5	,2.55	,0},{7.44	,2.55	,0.43	},{6.38		,3.83	,0.85	},{5.31		,2.55	,1.28	},{4.25		,1.28	,1.7	}},
{33,{10.89	,0	,0},{8.71	,2.61	,0},{7.62	,2.61	,0.44	},{6.53		,3.92	,0.87	},{5.44		,2.61	,1.31	},{4.35		,1.31	,1.74	}},
{34,{11.15	,0	,0},{8.92	,2.67	,0},{7.8	,2.67	,0.45	},{6.69		,4.01	,0.89	},{5.57		,2.67	,1.34	},{4.46		,1.34	,1.78	}},
{35,{11.4	,0	,0},{9.12	,2.74	,0},{7.98	,2.74	,0.46	},{6.84		,4.11	,0.91	},{5.7		,2.74	,1.37	},{4.56		,1.37	,1.82	}},
{36,{11.66	,0	,0},{9.33	,2.8	,0},{8.16	,2.8	,0.47	},{7		,4.2	,0.93	},{5.83		,2.8	,1.4	},{4.67		,1.4	,1.87	}},
{37,{11.92	,0	,0},{9.54	,2.86	,0},{8.35	,2.86	,0.48	},{7.15		,4.29	,0.95	},{5.96		,2.86	,1.43	},{4.77		,1.43	,1.91	}},
{38,{12.18	,0	,0},{9.75	,2.92	,0},{8.53	,2.92	,0.49	},{7.31		,4.39	,0.97	},{6.09		,2.92	,1.46	},{4.87		,1.46	,1.95	}},
{39,{12.44	,0	,0},{9.95	,2.99	,0},{8.71	,2.99	,0.5	},{7.46		,4.48	,1		},{6.22		,2.99	,1.49	},{4.98		,1.49	,1.99	}},
{40,{12.7	,0	,0},{10.16	,3.05	,0},{8.89	,3.05	,0.51	},{7.62		,4.57	,1.02	},{6.35		,3.05	,1.52	},{5.08		,1.52	,2.03	}},
{41,{12.96	,0	,0},{10.37	,3.11	,0},{9.07	,3.11	,0.52	},{7.78		,4.67	,1.04	},{6.48		,3.11	,1.56	},{5.18		,1.56	,2.07	}},
{42,{13.22	,0	,0},{10.58	,3.17	,0},{9.25	,3.17	,0.53	},{7.93		,4.76	,1.06	},{6.61		,3.17	,1.59	},{5.29		,1.59	,2.12	}},
{43,{13.48	,0	,0},{10.78	,3.23	,0},{9.43	,3.23	,0.54	},{8.09		,4.85	,1.08	},{6.74		,3.23	,1.62	},{5.39		,1.62	,2.16	}},
{44,{13.74	,0	,0},{10.99	,3.3	,0},{9.62	,3.3	,0.55	},{8.24		,4.95	,1.1	},{6.87		,3.3	,1.65	},{5.5		,1.65	,2.2	}},
{45,{14		,0	,0},{11.2	,3.36	,0},{9.8	,3.36	,0.56	},{8.4		,5.04	,1.12	},{7		,3.36	,1.68	},{5.6		,1.68	,2.24	}},
{46,{14.26	,0	,0},{11.4	,3.42	,0},{9.98	,3.42	,0.57	},{8.55		,5.13	,1.14	},{7.13		,3.42	,1.71	},{5.7		,1.71	,2.28	}},
{47,{14.52	,0	,0},{11.61	,3.48	,0},{10.16	,3.48	,0.58	},{8.71		,5.23	,1.16	},{7.26		,3.48	,1.74	},{5.81		,1.74	,2.32	}},
{48,{14.77	,0	,0},{11.82	,3.55	,0},{10.34	,3.55	,0.59	},{8.86		,5.32	,1.18	},{7.39		,3.55	,1.77	},{5.91		,1.77	,2.36	}},
{49,{15.03	,0	,0},{12.03	,3.61	,0},{10.52	,3.61	,0.6	},{9.02		,5.41	,1.2	},{7.52		,3.61	,1.8	},{6.01		,1.8	,2.41	}},
{50,{15.29	,0	,0},{12.23	,3.67	,0},{10.7	,3.67	,0.61	},{9.18		,5.51	,1.22	},{7.65		,3.67	,1.84	},{6.12		,1.84	,2.45	}},
{51,{15.55	,0	,0},{12.44	,3.73	,0},{10.89	,3.73	,0.62	},{9.33		,5.6	,1.24	},{7.78		,3.73	,1.87	},{6.22		,1.87	,2.49	}},
{52,{15.81	,0	,0},{12.65	,3.79	,0},{11.07	,3.79	,0.63	},{9.49		,5.69	,1.26	},{7.91		,3.79	,1.9	},{6.32		,1.9	,2.53	}},
{53,{16.07	,0	,0},{12.86	,3.86	,0},{11.25	,3.86	,0.64	},{9.64		,5.79	,1.29	},{8.04		,3.86	,1.93	},{6.43		,1.93	,2.57	}},
{54,{16.33	,0	,0},{13.06	,3.92	,0},{11.43	,3.92	,0.65	},{9.8		,5.88	,1.31	},{8.16		,3.92	,1.96	},{6.53		,1.96	,2.61	}},
{55,{16.59	,0	,0},{13.27	,3.98	,0},{11.61	,3.98	,0.66	},{9.95		,5.97	,1.33	},{8.29		,3.98	,1.99	},{6.64		,1.99	,2.65	}},
{56,{16.85	,0	,0},{13.48	,4.04	,0},{11.79	,4.04	,0.67	},{10.11	,6.07	,1.35	},{8.42		,4.04	,2.02	},{6.74		,2.02	,2.7	}},
{57,{17.11	,0	,0},{13.69	,4.11	,0},{11.98	,4.11	,0.68	},{10.26	,6.16	,1.37	},{8.55		,4.11	,2.05	},{6.84		,2.05	,2.74	}},
{58,{17.37	,0	,0},{13.89	,4.17	,0},{12.16	,4.17	,0.69	},{10.42	,6.25	,1.39	},{8.68		,4.17	,2.08	},{6.95		,2.08	,2.78	}},
{59,{17.63	,0	,0},{14.1	,4.23	,0},{12.34	,4.23	,0.71	},{10.58	,6.35	,1.41	},{8.81		,4.23	,2.12	},{7.05		,2.12	,2.82	}},
{60,{17.88	,0	,0},{14.31	,4.29	,0},{12.52	,4.29	,0.72	},{10.73	,6.44	,1.43	},{8.94		,4.29	,2.15	},{7.15		,2.15	,2.86	}}
}

function x000512_xOnDrop(player, params)	
	--放弃归0
	local cell_container= player:DataCellContainer()	
	cell_container:set_data_32(CellLogicType_MissionCell,last_id,-1,true)
	
	return ResultCode_ResultOK
end

function x000512_xOnReset(player, params)
	if params["type"] == mission_type then	
		--重置逻辑
		local cell_container= player:DataCellContainer()	
		cell_container:set_data_32(CellLogicType_MissionCell,process,0,true)
		cell_container:set_data_32(CellLogicType_MissionCell,last_id,-1,true)
		return ResultCode_ResultOK
	end
	return ResultCode_InvalidMissionType
end


function x000512_xOnAccept(player,params)
	if params["type"] == mission_type then	
		local proxy = player:GetMissionProxy()
			
		--检查身上有没有同类型任务，有就不生成新的
		local mission_map=proxy:GetMissionMap()	
		for k, mission in pairs(mission_map) do
			local config = mission:Config()
			if config:type() == mission_type then
				return ResultCode_MissionCountLimit
			end
		end
		
		local next_mission = x000512_GetNextMission(player)
		if next_mission == -1 then
			return ResultCode_NoMoreMission
		end
		proxy:OnAddMission(next_mission)
		
		return ResultCode_ResultOK
	end
	
	return ResultCode_InvalidMissionType
end


function x000512_xOnFinishEnd(player,params)
	local config = LuaMtMissionManager():FindMission(params["id"])
	if config == nil then
		return ResultCode_InvalidConfigData
	end
	---最后一个任务给活跃度
	local last_mission_id = LuaMtMissionManager():GetLastMissionId(config:type(),config:group())
	if last_mission_id == config:id() then
		player:OnLuaFunCall_1(501,"AddActivityTimes",activity_type)
		
		--重置一下lastid，保证能重入
		local cell_container= player:DataCellContainer()	
		cell_container:set_data_32(CellLogicType_MissionCell,last_id,-1,true)	
		player:SendStatisticsResult(activity_type,true)	
		x000010_GRaidMissionOver(player,{["value"]=params["rtid"]})
	end	
	
	return ResultCode_ResultOK
end

function x000512_xOnFinishBegin(player,params)
	local mission = player:GetMissionById( params["id"] )
	if mission == nil then
		return ResultCode_InvalidMission
	end
	
	local config = mission:Config()	
	if config == nil then
		return ResultCode_InvalidConfigData
	end
	
	local circle = config:circle();
	if circle<0 or circle >=32 then
		return ResultCode_InvalidConfigData
	end
	
	local cell_container = player:DataCellContainer()	
	local process_flags = cell_container:get_data_32(CellLogicType_MissionCell, process);
	if cell_container == nil then
		return ResultCode_InvalidPlayerData
	end
	
	local last_mission_id = LuaMtMissionManager():GetLastMissionId(config:type(),config:group())
	--只能领一次
	if cell_container:check_bit_data_32(process_flags,circle) == false then
		
		local new_flag = cell_container:set_bit_data_32(process_flags,circle)
		cell_container:set_data_32(CellLogicType_MissionCell, process,new_flag,true)
	
		--奖励经验
		local level_config = LuaMtActorConfig():GetLevelData(player:PlayerLevel())
		if level_config == nil then
			return ResultCode_InvalidConfigData
		end
		
		local exp = level_config:blood_exp()
		local exp_rate = config:exp_rate()
		if exp_rate>=0 then
			exp = exp_rate*exp;
		end			
		
		--print("eeeeeeeeeeeeeee"..exp)
		
		--月卡
		if cell_container:check_bit_data(BitFlagCellIndex_MonthCardFlag) then
			exp=(1.0+0.05)*exp
		end	
		
		--print("eeeeeeeeeeeeeee"..exp)
		
		player:AddItemByIdWithNotify(TokenType_Token_Exp,exp,ItemAddLogType_AddType_Activity,activity_type,1);
		--队长而外经验
		if player:IsTeamLeader() then
			player:GetMissionProxy():TeamLeaderExtraReward(config,exp);
		end
		
		--金币奖励
		local gold = level_config:blood_gold()
		local gold_rate = config:gold_rate()
		if gold_rate >=0 then
			gold = gold_rate*gold;
		end			
		player:AddItemByIdWithNotify(TokenType_Token_Gold,gold,ItemAddLogType_AddType_Activity,activity_type,1);
		
		--奖励药水
		x000512_PotionReward(player,config)
		
		if last_mission_id == config:id() then			
			--每天首通有特殊奖励,提尔之手
			local times = player:OnLuaFunCall_1(501, "GetActivityTimes", activity_type)
			if times == 1 then
				player:AddItemByIdWithNotify(20236005,1,ItemAddLogType_AddType_Activity,activity_type,1);
			end
		end	
		player:GetMissionProxy():OnMissionReward(params["id"])
		--掉落活动检查
		player:CheckAcrDrop(activity_type)
		
	end
	
	return ResultCode_ResultOK
end

--------------------------------------------------------
function x000512_GetMissionParams(player)
	 return {last_id,process}
end

function x000512_GetNextMission(player)
	
	local cell_container= player:DataCellContainer()	
	local lastmission = cell_container:get_data_32(CellLogicType_MissionCell,last_id)
	
	local next_mission = begin_mission
	if lastmission > 0 then
		next_mission=lastmission+1
	end
	
	--print(" lastmission"..lastmission.." begin_mission"..begin_mission.." next_mission"..next_mission)
	
	local config = LuaMtMissionManager():FindMission(next_mission)
	if config == nil then
		return -1 --说明没任务了
	end		
	
	return next_mission
end

function x000512_OnAddBegin(player,config)
	
	if config:type() ~=mission_type then
		return ResultCode_InvalidMissionType
	end
	local cell_container= player:DataCellContainer()	
	cell_container:set_data_32(CellLogicType_MissionCell,last_id,config:id(),true)

	return ResultCode_ResultOK
end

function x000512_OnClone(player, target, config)

	local target_container = target:DataCellContainer()	
	local last = target_container:get_data_32(CellLogicType_MissionCell, last_id);

	local player_container = player:DataCellContainer()
	player_container:set_data_32(CellLogicType_MissionCell, last_id, last, true);	
	
	--参与标记
	x000501_OnActivityJoin(player,activity_type)
	
	return ResultCode_ResultOK
	
end

function x000512_OnAddEnd(player,config)
	player:GetMissionProxy():Mission2Member(config:id())
			
	--参与标记
	x000501_OnActivityJoin(player,activity_type)
	
	return ResultCode_ResultOK
end	

function x000512_GenPotion(player,rate)
	local count,rand = math.modf(rate)
	local seed = LuaServer():RandomNum(1,10000)
	if seed <= rand*10000 then
		count=count+1
	end
	return count
end

function x000512_PotionReward(player,config)
	--取得波数
	local wave_count = config:circle()
	if wave_count <= 0 then
		return
	end
	local level = player:PlayerLevel()
	local rate = 0
	for k,v in pairs(potion_table)	do
		if v[1]== level then
			rate = v[wave_count+1][1]*2
			local count = x000512_GenPotion(player,rate)	
			if count> 0 then
				player:AddItemByIdWithNotify(20121001,count,ItemAddLogType_AddType_Activity,activity_type,1);
			end			
			rate = v[wave_count+1][2]*2
			local count = x000512_GenPotion(player,rate)	
			if count> 0 then
				player:AddItemByIdWithNotify(20121002,count,ItemAddLogType_AddType_Activity,activity_type,1);
			end		
			rate = v[wave_count+1][3]*2
			local count = x000512_GenPotion(player,rate)	
			if count> 0 then
				player:AddItemByIdWithNotify(20121003,count,ItemAddLogType_AddType_Activity,activity_type,1);
			end		
		end
	end
end

--找回有用
function x000512_ResolvePotionReward(player,less_rate)
	
	local level = player:PlayerLevel()
	local rate = 0

	local count1 = 0
	local count2 = 0
	local count3 = 0
	for wave_count=1,6,1 do
		for k,v in pairs(potion_table)	do
			if v[1]== level then
				rate = v[wave_count+1][1]*2
				count1 = count1+x000512_GenPotion(player,rate)	
				
				rate = v[wave_count+1][2]*2
				count2 = count2+ x000512_GenPotion(player,rate)	
				
				rate = v[wave_count+1][3]*2
				count3 = count3+ x000512_GenPotion(player,rate)	
			end
		end
	end
	
	count1 = math.floor(less_rate*count1)+1
	if count1> 0 then
		player:AddItemByIdWithNotify(20121001,count1,ItemAddLogType_AddType_MissReward,activity_type,1);
	end	
	
	count2 = math.floor(less_rate*count2)+1
	if count2> 0 then
		player:AddItemByIdWithNotify(20121002,count2,ItemAddLogType_AddType_MissReward,activity_type,1);
	end	
	
	count3 = math.floor(less_rate*count3)+1
	if count3> 0 then
		player:AddItemByIdWithNotify(20121003,count3,ItemAddLogType_AddType_MissReward,activity_type,1);
	end	
	
	player:AddItemByIdWithNotify(20236005,1,ItemAddLogType_AddType_MissReward,activity_type,1);
end


function x000512_ResolveAllReward(player)
	
	local level_config = LuaMtActorConfig():GetLevelData(player:PlayerLevel())
	if level_config == nil then
		return 0,0
	end
	local exp = level_config:blood_exp()		
	local gold = level_config:blood_gold()	
	
	local final_exp = 0
	local final_gold = 0
		
	--结算每个任务奖励
	for i=1,6,1 do
		local config = LuaMtMissionManager():FindMission(begin_mission+i) 
		if config ~= nil then
			local exp_rate = config:exp_rate()
			if exp_rate>=0 then
				final_exp = final_exp+exp_rate*exp
			end	
			local gold_rate = config:gold_rate()
			if gold_rate >=0 then
				final_gold = final_gold+gold_rate*gold
			end	
		end
	end
	
	return final_gold,final_exp
end	
