
function x000501_IsWorldBoss(player,ac_type)
	if 	ac_type==ActivityType.WolrdBossCarbon1 or
		ac_type==ActivityType.WolrdBossCarbon2 or
		ac_type==ActivityType.WolrdBossCarbon3 or
		ac_type==ActivityType.WolrdBossCarbon4 or
		ac_type==ActivityType.WolrdBossCarbon5 or
		ac_type==ActivityType.WolrdBossCarbon6 or
		ac_type==ActivityType.WolrdBossCarbon7 then		
		return true
	else
		return false
	end	
end

function x000501_IsTeamCarbon(player,ac_type)
	if 	ac_type==ActivityType.TeamCarbon2001 or
		ac_type==ActivityType.TeamCarbon2002 or
		ac_type==ActivityType.TeamCarbon2003 or
		ac_type==ActivityType.TeamCarbon2004 or
		ac_type==ActivityType.TeamCarbon2005 or
		ac_type==ActivityType.TeamCarbon2006 or
		ac_type==ActivityType.TeamCarbon2007 then		
		return true
	else
		return false
	end	
end


function x000501_IsTeamCarbonMission(player,mission_id)
	local mission_config = LuaMtMissionManager():FindMission(mission_id)
	if mission_config == nil then
		return false
	end
	local type = mission_config:type()	
	if 	type==MissionType_Carbon2001Mission or
		type==MissionType_Carbon2002Mission or
		type==MissionType_Carbon2003Mission or
		type==MissionType_Carbon2004Mission or
		type==MissionType_Carbon2005Mission or
		type==MissionType_Carbon2006Mission or
		type==MissionType_Carbon2007Mission then		
		return true
	else
		return false
	end	
end

--常量表
g_world_boss_index={
{0,ActivityType.WolrdBossCarbon1,"activity_name_14"},
{1,ActivityType.WolrdBossCarbon2,"activity_name_15"},
{2,ActivityType.WolrdBossCarbon3,"activity_name_16"},
{3,ActivityType.WolrdBossCarbon4,"activity_name_17"},
{4,ActivityType.WolrdBossCarbon5,"activity_name_18"},
{5,ActivityType.WolrdBossCarbon6,"activity_name_19"},
{6,ActivityType.WolrdBossCarbon7,"activity_name_20"},
}
function x000501_GetWorldBossAc(player,params)
	local current_weekday = LuaMtTimerManager():GetWeekDay()	
	local world_boss_type = ActivityType.WolrdBossCarbon1
	for k,index in pairs(g_world_boss_index) do
		if current_weekday==index[1] then
			world_boss_type =index[2]
			break
		end
	end
	return world_boss_type;
end
	
function x000501_OnActivityNotify(player,params)
	local type = params["type"]
	return ResultCode_InternalResult
end


 TypeToScriptId ={
{MissionType_MainMission,503},
{MissionType_PartMission,503},
{MissionType_ArchaeologyMission,505},
{MissionType_UnionMission,506},
{MissionType_CorpsMission,507},
{MissionType_RewardMission,508},
{MissionType_ExtraRewardMission,508},
{MissionType_GoblinCarbonMission,509},
{MissionType_TinyThreeCarbonMission,510},
{MissionType_BlackCarbonMission,511},
{MissionType_BloodCarbonMission,512},
{MissionType_CommonCarbonMission,513},
{MissionType_BigThreeCarbonMission,514},
{MissionType_GradeUpMission,515},
{MissionType_DayKillMission,516},
{MissionType_LevelMission,517},
{MissionType_Carbon2001Mission,521},
{MissionType_Carbon2002Mission,522},
{MissionType_Carbon2003Mission,523},
{MissionType_Carbon2004Mission,524},
{MissionType_Carbon2005Mission,525},
{MissionType_Carbon2006Mission,526},
{MissionType_Carbon2007Mission,527},
{MissionType_ScrollMission,528},
{MissionType_TowerMission,529},
}
function x000501_TypeToScriptId(player,mission_type)
	for k,v in pairs(TypeToScriptId) do
		if v[1] == mission_type then
			return v[2]
		end
	end		
	return -1
end

function x000501_IsRaidMission(player,mission_type)
	--print("ddddddddgdddddddddddddddddddddd"..mission_type)
	if mission_type == MissionType_GoblinCarbonMission 
		or mission_type == MissionType_TinyThreeCarbonMission
		or mission_type == MissionType_BlackCarbonMission
		or mission_type == MissionType_BloodCarbonMission		
		or mission_type == MissionType_BigThreeCarbonMission
		or mission_type == MissionType_CommonCarbonMission
		or mission_type == MissionType_Carbon2001Mission
		or mission_type == MissionType_Carbon2002Mission
		or mission_type == MissionType_Carbon2003Mission
		or mission_type == MissionType_Carbon2004Mission
		or mission_type == MissionType_Carbon2005Mission
		or mission_type == MissionType_Carbon2006Mission
		or mission_type == MissionType_Carbon2007Mission
		or mission_type == MissionType_TowerMission	then
			return 1
	end
	return 0
end

----------------------------------------------------------------------------------
 MissionTypeToAcType ={
{MissionType_MainMission,			ActivityType.InvalidActivityType },
{MissionType_PartMission,			ActivityType.InvalidActivityType },
{MissionType_ArchaeologyMission,	ActivityType.Ancholage			 },
{MissionType_UnionMission,			ActivityType.AcUnionMission		 },
{MissionType_CorpsMission,			ActivityType.CorpsFight			 },
{MissionType_RewardMission,			ActivityType.GoldReward			 },
{MissionType_ExtraRewardMission,	ActivityType.GoldReward			 },
{MissionType_GoblinCarbonMission,	ActivityType.GoblinCarbon		 },
{MissionType_TinyThreeCarbonMission,ActivityType.TinyThreeCarbon	 },
{MissionType_BlackCarbonMission,	ActivityType.BlackCarbon		 },
{MissionType_BloodCarbonMission,	ActivityType.BloodCarbon		 },
{MissionType_BigThreeCarbonMission,	ActivityType.BigThreeCarbon		 },
{MissionType_GradeUpMission,		ActivityType.InvalidActivityType },
{MissionType_DayKillMission,		ActivityType.DayKill			 },
{MissionType_LevelMission,			ActivityType.InvalidActivityType },
{MissionType_CommonCarbonMission,	ActivityType.InvalidActivityType },
{MissionType_Carbon2001Mission,		ActivityType.TeamCarbon2001		 },
{MissionType_Carbon2002Mission,		ActivityType.TeamCarbon2002		 },
{MissionType_Carbon2003Mission,		ActivityType.TeamCarbon2003		 },
{MissionType_Carbon2004Mission,		ActivityType.TeamCarbon2004		 },
{MissionType_Carbon2005Mission,		ActivityType.TeamCarbon2005		 },
{MissionType_Carbon2006Mission,		ActivityType.TeamCarbon2006		 },
{MissionType_Carbon2007Mission,		ActivityType.TeamCarbon2007		 },
{MissionType_ScrollMission,			ActivityType.InvalidActivityType },
{MissionType_TowerMission,			ActivityType.Tower				 },
}
function x000501_MissTypeToActivityType(player,mission_type)
	for k,v in pairs(MissionTypeToAcType) 
	do
		if v[1] == mission_type then
			return v[2]
		end
	end		
	return -1
end

----------------------------------------------------------------------------------
AcTypeToConfig ={
{ActivityType.GoldReward,		MissionType_RewardMission 			,001003		},
{ActivityType.CorpsFight,		MissionType_CorpsMission			,001003		},
{ActivityType.Ancholage,		MissionType_ArchaeologyMission 		,001003		},
{ActivityType.Arena,			MissionType_InvalidMission			,001012		},
{ActivityType.Hell,				MissionType_InvalidMission			,001013		},
{ActivityType.GoblinCarbon,		MissionType_GoblinCarbonMission 	,001003		},
{ActivityType.Tranthform,		MissionType_InvalidMission 			,-1	   		},
{ActivityType.TinyThreeCarbon,	MissionType_TinyThreeCarbonMission 	,001003		},
{ActivityType.BigThreeCarbon,	MissionType_BigThreeCarbonMission 	,001003		},
{ActivityType.BurningFight,		MissionType_InvalidMission 			,001006		},
{ActivityType.SkyFallFight,     MissionType_InvalidMission 			,001007		},
{ActivityType.BloodCarbon,      MissionType_BloodCarbonMission 		,001003		},
{ActivityType.BlackCarbon,      MissionType_BlackCarbonMission		,001003		},
{ActivityType.WolrdBossCarbon1, MissionType_InvalidMission 			,001011		},
{ActivityType.WolrdBossCarbon2, MissionType_InvalidMission 			,001011		},
{ActivityType.WolrdBossCarbon3, MissionType_InvalidMission 			,001011		},
{ActivityType.WolrdBossCarbon4, MissionType_InvalidMission 			,001011		},
{ActivityType.WolrdBossCarbon5, MissionType_InvalidMission 			,001011		},
{ActivityType.WolrdBossCarbon6, MissionType_InvalidMission 			,001011		},
{ActivityType.WolrdBossCarbon7, MissionType_InvalidMission 			,001011		},
{ActivityType.BattleCarbon1,	MissionType_InvalidMission 			,-1	   		},
{ActivityType.BattleCarbon2,	MissionType_InvalidMission 			,001022		},
{ActivityType.GuildWetCopy,		MissionType_InvalidMission 			,001021		},
{ActivityType.DayKill,			MissionType_DayKillMission 			,001003		},
{ActivityType.UnionBattle,		MissionType_InvalidMission 			,001023		},
{ActivityType.UnionBoss,        MissionType_InvalidMission			,001020		},
{ActivityType.TextAnswer,		MissionType_InvalidMission			,002610		},
{ActivityType.PicAnswer,        MissionType_InvalidMission			,002610		},
{ActivityType.AcUnionMission,   MissionType_UnionMission			,001003		},
{ActivityType.TeamCarbon2001,   MissionType_Carbon2001Mission		,001003		},
{ActivityType.TeamCarbon2002,   MissionType_Carbon2002Mission		,001003		},
{ActivityType.TeamCarbon2003,   MissionType_Carbon2003Mission		,001003		},
{ActivityType.TeamCarbon2004,   MissionType_Carbon2004Mission		,001003		},
{ActivityType.TeamCarbon2005,   MissionType_Carbon2005Mission		,001003		},
{ActivityType.TeamCarbon2006,   MissionType_Carbon2006Mission		,001003		},
{ActivityType.TeamCarbon2007,   MissionType_Carbon2007Mission		,001003		},
{ActivityType.UnionDonate,   	MissionType_InvalidMission			,-1			},
{ActivityType.Tower,  			MissionType_TowerMission			,001003		},
}

function x000501_AcTypeToMissionType(player,ac_type)
	for k,v in pairs(AcTypeToConfig) do
		if v[1] == ac_type then
			return v[2]
		end
	end		
	return -1
end

function x000501_AcTypeToAcScriptId(player,ac_type)
	for k,v in pairs(AcTypeToConfig) do
		if v[1] == ac_type then
			return v[3]
		end
	end		
	return -1
end

function x000501_MissionTypeToAcScriptId(player,mission_type)
	for k,v in pairs(AcTypeToConfig) do
		if v[2] == mission_type then
			return v[3]
		end
	end		
	return -1
end

----------------------------------------------------------------------------------	
AcTypeToTimes ={
{ActivityType.GoldReward,		ActivityDataIndex.RewardDayTimes 		},
{ActivityType.CorpsFight,		ActivityDataIndex.CorpsFightDayTimes	},
{ActivityType.Ancholage,		ActivityDataIndex.AncholageDayTimes 	},
{ActivityType.Arena,			ActivityDataIndex.ArenaTimes			},
{ActivityType.Hell,				ActivityDataIndex.HellTimes				},
{ActivityType.GoblinCarbon,		ActivityDataIndex.GoblinDayTimes 		},
{ActivityType.Tranthform,		ActivityDataIndex.TranshformDayTimes 	},
{ActivityType.TinyThreeCarbon,	ActivityDataIndex.TinyThreeDayTimes 	},
{ActivityType.BigThreeCarbon,	ActivityDataIndex.BigThreeDayTimes 		},
{ActivityType.BurningFight,		ActivityDataIndex.BurningDayTimes 		},
{ActivityType.SkyFallFight,     ActivityDataIndex.SkyFallDayTimes 		},
{ActivityType.BloodCarbon,      ActivityDataIndex.BloodDayTimes 		},
{ActivityType.BlackCarbon,      ActivityDataIndex.BlackDayTimes			},
{ActivityType.WolrdBossCarbon1, ActivityDataIndex.WorldBossTimes 		},
{ActivityType.WolrdBossCarbon2, ActivityDataIndex.WorldBossTimes 		},
{ActivityType.WolrdBossCarbon3, ActivityDataIndex.WorldBossTimes 		},
{ActivityType.WolrdBossCarbon4, ActivityDataIndex.WorldBossTimes 		},
{ActivityType.WolrdBossCarbon5, ActivityDataIndex.WorldBossTimes 		},
{ActivityType.WolrdBossCarbon6, ActivityDataIndex.WorldBossTimes 		},
{ActivityType.WolrdBossCarbon7, ActivityDataIndex.WorldBossTimes 		},
{ActivityType.BattleCarbon1,	ActivityDataIndex.FlagBattleFieldTimes 	},
{ActivityType.BattleCarbon2,	ActivityDataIndex.HillBattleFieldTimes 	},
{ActivityType.DayKill,			ActivityDataIndex.DayKillTimes 			},
{ActivityType.UnionBattle,		ActivityDataIndex.UnionBattleTimes		},
{ActivityType.UnionBoss,        ActivityDataIndex.UnionBossTimes		},
{ActivityType.TextAnswer,		ActivityDataIndex.TextAnswerTimes		},
{ActivityType.PicAnswer,        ActivityDataIndex.PicAnswerTimes 		},
{ActivityType.AcUnionMission,   ActivityDataIndex.UnionMissionTimes		},
{ActivityType.TeamCarbon2001,	ActivityDataIndex.Carbon2001Times		},
{ActivityType.TeamCarbon2002,	ActivityDataIndex.Carbon2002Times		},
{ActivityType.TeamCarbon2003,	ActivityDataIndex.Carbon2003Times		},
{ActivityType.TeamCarbon2004,	ActivityDataIndex.Carbon2004Times		},
{ActivityType.TeamCarbon2005,	ActivityDataIndex.Carbon2005Times		},
{ActivityType.TeamCarbon2006,	ActivityDataIndex.Carbon2006Times		},
{ActivityType.TeamCarbon2007,	ActivityDataIndex.Carbon2007Times		},
{ActivityType.UnionDonate,		ActivityDataIndex.UnionDonateTimes		},
{ActivityType.Tower,			ActivityDataIndex.TowerMissionTimes		}
}


function x000501_GetActivityTimes(player,ac_type)
	local data_container = player:DataCellContainer()
	local index = -1
	for k,v in pairs(AcTypeToTimes) 
	do
		if v[1] == ac_type then
			index= v[2]
			break
		end
	end	
	--print("dddddddddddddddddddac_type="..ac_type)
	if index <0 then		
		return -1
	end
	
	return data_container:get_data_32(CellLogicType_ActivityCell, index);
end

function x000501_ResetActivityTimes(player,ac_type)
	--if ac_type == -1 then
	--	local data_container = player:DataCellContainer()
	--	local index = -1
	--	for k,v in pairs(AcTypeToTimes) 
	--	do
	--		index= v[2]
	--		data_container:set_data_32(CellLogicType_ActivityCell, index,0,true);
	--	end	
	--else
		local data_container = player:DataCellContainer()
		local index = -1
		for k,v in pairs(AcTypeToTimes) 
		do
			if v[1] == ac_type then
				index= v[2]
				break
			end
		end	
		
		if index <0 then
			return -1
		end
		
		data_container:set_data_32(CellLogicType_ActivityCell, index,0,true);
	--end
		
	--return 0
end

function x000501_InitActivityTimes(player,ac_type)
	if ac_type == -1 then
		local data_container = player:DataCellContainer()
		local index = -1
		for k,v in pairs(AcTypeToTimes) 
		do
			index= v[2]
			data_container:try_init_int32_data(CellLogicType_ActivityCell, index,0);
		end	
	else
		local data_container = player:DataCellContainer()
		local index = -1
		for k,v in pairs(AcTypeToTimes) 
		do
			if v[1] == ac_type then
				index= v[2]
				break
			end
		end	
		
		if index <0 then
			return -1
		end
		
		data_container:try_init_int32_data(CellLogicType_ActivityCell, index,0);
	end
		
	return 0
end

function x000501_AddActivityTimes(player,ac_type)
	local data_container = player:DataCellContainer()
	local index = -1
	for k,v in pairs(AcTypeToTimes) 
	do
		if v[1] == ac_type then
			index= v[2]
			break
		end
	end	
	if index <0 then		
		return -1
	end
	
	data_container:add_data_32(CellLogicType_ActivityCell, index,1,true);
	
	local activity = player:GetActivity(ac_type)
	local activityDB = activity:ActivityDB()
	if activityDB:is_active() then
		local ac_config = activityDB:config();
		local reward_count = ac_config:reward_count();
		local reward_toke = ac_config:token_day_once();
		local done_times = x000501_GetActivityTimes(player,ac_type)
		if reward_count >= done_times then
			player:AddItemByIdWithNotify(TokenType_Token_Day, reward_toke, ItemAddLogType_AddType_Activity, ac_type,1);
		end
		
		--if done_times >=ac_config:display_times() then
		--	player:OnLuaFunCall_n(501,"OnActivityNotify",{["type"]=ac_type,["result"]=ResultCode_RewardLimit,["reward_count"]=ac_config:display_times()});
		--end
	end	
	
	
	--刷新活动状态
	activity:OnRefresh2Client()
	local logstr = tostring(ac_type);
	player:DBLog("ActivityTimes",logstr,ac_type);	
	
	x000100_xAddActivityTimes(player,{["value"]=ac_type})
	x000504_xAddActivityTimes(player,{["value"]=ac_type})
	x002611_xAddActivityTimes(player,{["value"]=ac_type})
	return 0
end


function x000501_OnActivityJoin(player,ac_type)

	local cell_container = player:DataCellContainer()
	if cell_container == nil then
		return false
	end
	--参加过置为1
	local flag = cell_container:get_data_64(Cell64LogicType_NumberCell64, NumberCell64Index_MissRewardFlag)
	if ac_type >0 and ac_type <64 then
		local new_flag = cell_container:set_bit_data_64(flag,ac_type)
		cell_container:set_data_64(Cell64LogicType_NumberCell64, NumberCell64Index_MissRewardFlag, new_flag,false)
		return true
	end	
	
	return false
end

function x000501_OnActivityOpen(player,ac_type)

	if ac_type ~= -1 then
		local ac_config = LuaActivityManager():FindActivity(ac_type)
		if ac_config == nil then
			return ResultCode_InternalResult
		end
		local ac_name = ac_config:name()
		ac_name = "activity_open_notify|"..ac_name	
	
		if ac_type == ActivityType.TextAnswer then
			player:OnResetAnswer("text")		
			player:SendLuaPacket("TextAnswersNotify",{300},{},{ac_name})
		elseif ac_type == ActivityType.PicAnswer then
			player:OnResetAnswer("pic")
			player:SendLuaPacket("PicAnswersNotify",{300},{},{ac_name})
		elseif ac_type == ActivityType.BattleCarbon2 then
			player:SendLuaPacket("BattleFieldNotify",{300},{},{ac_name})
			
			Lua_send_run_massage("ac_notify_025|"..ac_config:name()) 
			
		else
			----do nothing
		end	
	end
	
	return ResultCode_ResultOK
end


function x000501_OnActivityClose(player,ac_type)
	return ResultCode_ResultOK
end

function x000501_OnFinishAcceptMission(player,params)
	
	local config = LuaMtMissionManager():FindMission(params["id"])
	if config == nil then
		return ResultCode_InvalidConfigData
	end	
	
	local last_mission_id = LuaMtMissionManager():GetLastMissionId(config:type(),config:group())
	local continue = 1
	
	local activity_type= x000501_MissTypeToActivityType(player,config:type())
	local done_times = 0
	if activity_type ~= -1 then
		done_times = x000501_GetActivityTimes(player,activity_type)
	end
	local circle = 0
	if config:type() == MissionType_RewardMission then
		local cell_container = player:DataCellContainer()
		circle = cell_container:get_data_32(CellLogicType_MissionCell,MissionCellIndex.Reward_Circle)		
		local cur_circle =math.fmod( circle, 10)
		if cur_circle == 0 then
			continue =0
		end	
	elseif config:type() == MissionType_CorpsMission then
		local cell_container = player:DataCellContainer()
		circle = cell_container:get_data_32(CellLogicType_MissionCell,MissionCellIndex.Corps_Circle)		
		local cur_circle =math.fmod( circle, 10)
		if cur_circle == 0 then
			continue =0
		end	
	elseif last_mission_id == config:id() then		
		if config:type() == MissionType_BigThreeCarbonMission or config:type() == MissionType_TinyThreeCarbonMission then
			if math.fmod( done_times, 3) ==0 then
				continue =0
			end
		elseif config:type() == MissionType_MainMission 			--循环任务需要持续，如果存在多轮的限制，每种任务逻辑自身会处理中断
			or config:type() == MissionType_PartMission 
			or config:type() == MissionType_UnionMission
			or config:type() == MissionType_ArchaeologyMission	then
			continue =1					
		else
			continue =0
		end		
	end
		
	--print("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx continue"..continue.." last_mission_id"..last_mission_id.." config:id()"..config:id().." done_times"..done_times.. " circle"..circle)
	--还有任务
	if continue ==1 then
		if player:GetTeamID() ~= 0 and player:IsTeamLeader() == false and player:GetTeamStatus()==PlayerTeamStatus_Team_Follow then		
			--队长获得任务时自动同步给所有跟随的人，所以这里不处理
		else
			--检查队伍中只有队长完成任务运行接续阶任务			
			if activity_type ~= -1 then
				local ac_config = LuaActivityManager():FindActivity(activity_type)
				if ac_config ~= nil then
					--次数判定,没了就不再继续获取任务了
					local limit_times = ac_config:limit_times()				
					if limit_times >= 0 and done_times >= limit_times then
						return ResultCode_InternalResult
					end
				end
			end
			player:GetMissionProxy():OnAcceptMission(config:type())
		end
	end
	
	--
	player:SendCommonReply("MissionFinishOver", { config:id() }, {}, {})
	
	return ResultCode_ResultOK
end

---------------------------------------------------
function x000501_OnRegisterFunction(proxy)
	
	if proxy ~= nil then
		proxy:RegFunc("xOnNewWeek"			,501)
	end
end


function x000501_xOnNewWeek(player,params)
	local data_container = player:DataCellContainer()
	if data_container ~= nil then	
		data_container:set_data_32(CellLogicType_ActivityCell, ActivityDataIndex.UnionMissionTimes,0,true)
		data_container:set_data_32(CellLogicType_ActivityCell, ActivityDataIndex.TowerMissionTimes,0,true)
		
		--团队本
		data_container:set_data_32(CellLogicType_ActivityCell, ActivityDataIndex.Carbon2001Times,0,true)
		data_container:set_data_32(CellLogicType_ActivityCell, ActivityDataIndex.Carbon2002Times,0,true)
		data_container:set_data_32(CellLogicType_ActivityCell, ActivityDataIndex.Carbon2003Times,0,true)
		data_container:set_data_32(CellLogicType_ActivityCell, ActivityDataIndex.Carbon2004Times,0,true)
		data_container:set_data_32(CellLogicType_ActivityCell, ActivityDataIndex.Carbon2005Times,0,true)
		data_container:set_data_32(CellLogicType_ActivityCell, ActivityDataIndex.Carbon2006Times,0,true)
		data_container:set_data_32(CellLogicType_ActivityCell, ActivityDataIndex.Carbon2007Times,0,true)
	end	
	
	
	--团队本任务相关
	local proxy = player:GetMissionProxy()
	proxy:OnResetMission(MissionType_Carbon2001Mission)
	proxy:OnResetMission(MissionType_Carbon2002Mission)
	proxy:OnResetMission(MissionType_Carbon2003Mission)
	proxy:OnResetMission(MissionType_Carbon2004Mission)
	proxy:OnResetMission(MissionType_Carbon2005Mission)
	proxy:OnResetMission(MissionType_Carbon2006Mission)
	proxy:OnResetMission(MissionType_Carbon2007Mission)	
	--工会任务
	proxy:OnResetMission(MissionType_UnionMission)
	proxy:OnResetMission(MissionType_TowerMission)
	
	--结算重置
	player:ResetStatistics(ActivityType.TeamCarbon2001)
	player:ResetStatistics(ActivityType.TeamCarbon2002)
	player:ResetStatistics(ActivityType.TeamCarbon2003)
	player:ResetStatistics(ActivityType.TeamCarbon2004)
	player:ResetStatistics(ActivityType.TeamCarbon2005)
	player:ResetStatistics(ActivityType.TeamCarbon2006)
	player:ResetStatistics(ActivityType.TeamCarbon2007)
	
	player:ResetStatistics(ActivityType.AcUnionMission)
	
	----更新活动
	player:ResetActivityData()
	return ResultCode_ResultOK
end

function x000501_xOnNewDay(player,params)
	local cell_container = player:DataCellContainer()
	if cell_container ~= nil then				
	
		--每日活动参与标记
		cell_container:set_data_64(Cell64LogicType_NumberCell64, NumberCell64Index_MissRewardFlag	,0,true)			
		
		cell_container:set_data_32(CellLogicType_NumberCell  , NumberCellIndex_ActivityRewardFlag	,0,true)			
		cell_container:set_data_32(CellLogicType_ActivityCell, ActivityDataIndex.pic_reward_flag	,0,true)
		cell_container:set_data_32(CellLogicType_ActivityCell, ActivityDataIndex.text_reward_flag	,0,true)		
		cell_container:set_data_32(CellLogicType_ActivityCell, ActivityDataIndex.RewardDayTimes		,0,true)
		cell_container:set_data_32(CellLogicType_ActivityCell, ActivityDataIndex.CorpsFightDayTimes	,0,true)
		cell_container:set_data_32(CellLogicType_ActivityCell, ActivityDataIndex.AncholageDayTimes	,0,true)
		cell_container:set_data_32(CellLogicType_ActivityCell, ActivityDataIndex.ArenaTimes			,0,true)
		cell_container:set_data_32(CellLogicType_ActivityCell, ActivityDataIndex.HellTimes			,0,true)
		cell_container:set_data_32(CellLogicType_ActivityCell, ActivityDataIndex.GoblinDayTimes 	,0,true)
		cell_container:set_data_32(CellLogicType_ActivityCell, ActivityDataIndex.TranshformDayTimes ,0,true)
		cell_container:set_data_32(CellLogicType_ActivityCell, ActivityDataIndex.TinyThreeDayTimes 	,0,true)
		cell_container:set_data_32(CellLogicType_ActivityCell, ActivityDataIndex.BigThreeDayTimes 	,0,true)
		cell_container:set_data_32(CellLogicType_ActivityCell, ActivityDataIndex.BurningDayTimes 	,0,true)
		cell_container:set_data_32(CellLogicType_ActivityCell, ActivityDataIndex.SkyFallDayTimes 	,0,true)
		cell_container:set_data_32(CellLogicType_ActivityCell, ActivityDataIndex.BloodDayTimes 		,0,true)
		cell_container:set_data_32(CellLogicType_ActivityCell, ActivityDataIndex.BlackDayTimes		,0,true)
		cell_container:set_data_32(CellLogicType_ActivityCell, ActivityDataIndex.WorldBossTimes 	,0,true)
		cell_container:set_data_32(CellLogicType_ActivityCell, ActivityDataIndex.FlagBattleFieldTimes ,0,true)
		cell_container:set_data_32(CellLogicType_ActivityCell, ActivityDataIndex.HillBattleFieldTimes ,0,true)
		cell_container:set_data_32(CellLogicType_ActivityCell, ActivityDataIndex.DayKillTimes 		,0,true)
		cell_container:set_data_32(CellLogicType_ActivityCell, ActivityDataIndex.UnionBattleTimes	,0,true)
		cell_container:set_data_32(CellLogicType_ActivityCell, ActivityDataIndex.UnionBossTimes		,0,true)
		cell_container:set_data_32(CellLogicType_ActivityCell, ActivityDataIndex.TextAnswerTimes	,0,true)
		cell_container:set_data_32(CellLogicType_ActivityCell, ActivityDataIndex.PicAnswerTimes 	,0,true)
		cell_container:set_data_32(CellLogicType_ActivityCell, ActivityDataIndex.TextAnswerOkTimes	,0,true)
		cell_container:set_data_32(CellLogicType_ActivityCell, ActivityDataIndex.PicAnswerOkTimes 	,0,true)
		cell_container:set_data_32(CellLogicType_ActivityCell, ActivityDataIndex.UnionDonateTimes 	,0,true)
	end
	
	--任务相关
	local proxy = player:GetMissionProxy()
	proxy:OnResetMission(MissionType_RewardMission)
	proxy:OnResetMission(MissionType_CorpsMission)
	proxy:OnResetMission(MissionType_ArchaeologyMission)	
	proxy:OnResetMission(MissionType_GoblinCarbonMission)
	proxy:OnResetMission(MissionType_TinyThreeCarbonMission)
	proxy:OnResetMission(MissionType_BlackCarbonMission)
	proxy:OnResetMission(MissionType_BloodCarbonMission)
	proxy:OnResetMission(MissionType_BigThreeCarbonMission)
	
	--
	local data_container = player:DataCellContainer()
	local index = -1
	for k,v in pairs(AcTypeToTimes) do
		--工会活动按周来
		if v[1] ~= ActivityType.AcUnionMission and x000501_IsTeamCarbon(v[1]) then
			index= v[2]
			data_container:set_data_32(CellLogicType_ActivityCell, index,0,true)
		end
	end	
	
	----更新活动
	player:OnResetAnswer("text")		
	player:OnResetAnswer("pic")
	player:ResetActivityData()
	
	player:SendCommonReply("NewDay",{},{},{})
	
	--结算重置
	player:ResetStatistics(ActivityType.GoldReward)
	player:ResetStatistics(ActivityType.CorpsFight)
	player:ResetStatistics(ActivityType.Ancholage)	
	player:ResetStatistics(ActivityType.GoblinCarbon)
	player:ResetStatistics(ActivityType.TinyThreeCarbon)
	player:ResetStatistics(ActivityType.BlackCarbon)
	player:ResetStatistics(ActivityType.BloodCarbon)
	player:ResetStatistics(ActivityType.BigThreeCarbon)		
	player:ResetStatistics(ActivityType.TextAnswer)
	player:ResetStatistics(ActivityType.PicAnswer)	
	player:ResetStatistics(ActivityType.Hell)
	
	return ResultCode_ResultOK
end

