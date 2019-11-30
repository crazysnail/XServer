--开服7天
--活动类型
--登陆奖励
--升到X级
--战力达到XXX
--通关章节
--累计击败X次关卡BOSS
--累计附魔X次
--累计镶嵌至少X颗X级宝石
--任意英雄X个技能升级X级
--累计进行X次初级招募
--累计进行X次高级招募
--累计使用X次遗迹藏宝图
--X个英雄进阶到X品质
--累计完成X次赏金任务
--累计完成X次暮光军团
--累计完成X次地精矿洞
--累计进行X次竞技场
--击败试炼场第X关（1-15）
--累计击败X个天灾军团
--累计完成X次小三环
--累计完成X次提尔之手
--击败世界BOSS X次
--累计进行公会BOSS战斗X次
--半价出售

local config = {}
config.switch_open		=	1			--开关
config.script_id		=	002611		--活动id
config.type_name		=	"7日狂欢"	--活动名字
config.ac_group			=	00001		--活动组id
config.group_name		=	"7日狂欢"	--活动组名字
config.during_day		=	{1,1,1,1,1,1,1}		--每周开放计数
config.during_week		=	{1,1,1,1,1,1,1,1}	--循环周计数,循环周期是8周,自左向右,一个位表示一周(0关,1开),8周结束后从新循环,英文窜表示特殊情况
config.reset_data		=	0			--重置玩家活动数据开关
config.depend_enum		=	-1			--活动前置依赖（不能相互引用,不能太多,会导致服务器调用栈溢出！）
config.begin_date		=	20000101	--开始时间
config.end_date			=	20991231	--结束时间
config.open_level		=	1			--出现等级
config.off_level		=	99			--消失等级
config.include_channels	=	"all"		--包含渠道列表
config.group_icon		=	"null"		--组icon
config.desc				=	"null"		--描述


--组
-----------------------
config.group={}
config.group[1]={"login","level","power"}

config.group[2]={"stage"}																															
config.group[3]={"stage_boss"}																														
config.group[4]={"low_draw","high_draw"}																											
config.group[5]={"skill_level5","skill_level10","skill_level15","skill_level20","skill_level25","skill_level30","skill_level35","skill_level40"}	
config.group[6]={"color2","color3","color4","color5"}																								
config.group[7]={"magic"}																															
config.group[8]={"embed1","embed2","embed3"}																										
																							
config.group[9]={"reward_mission","union_mission"}																									
config.group[10]={"corps_mission","goblin_mission"}																									
config.group[11]={"arena","hell"}																													
config.group[12]={"treasure"}																														
config.group[13]={"skyfall"}																														
config.group[14]={"tinythree","blood"}																												
config.group[15]={"world_boss","union_boss"}																										

config.group[16]={"half"}

--注意，每组奖励档最多15档，16档级以上需要拆分处理！

--每日分布
--------------------------------------
config.days={
 1,{1,2,9, 16},
 2,{1,3,10,16},
 3,{1,4,11,16},
 4,{1,5,12,16},
 5,{1,6,13,16},
 6,{1,7,14,16},
 7,{1,8,15,16},
}

config.activity_group_name={
 1,{'daily_welfare', 'guaji', 'shangjinrenwu', 'half_price'},
 2,{'daily_welfare', 'defeat_boss', 'teampurpose_mgjt', 'half_price'},
 3,{'daily_welfare', 'recruit', 'athletic_trials', 'half_price'},
 4,{'daily_welfare', 'jinengshengji', 'relic_map', 'half_price'},
 5,{'daily_welfare', 'hero_enhance', 'scene_name_37', 'half_price'},
 6,{'daily_welfare', 'equip_enchanting', 'team_activity', 'half_price'},
 7,{'daily_welfare', 'gem_mounting', 'timing_activity', 'half_price'},
}

--奖励要求值,{道具,数量}
config.items={}
config.conditions={}

config.items["login"]={	--登陆奖励

{1,1,{29910002,50 }},
{2,2,{29910002,60 }},
{3,3,{29910002,60 }},
{4,4,{29910002,60 }},
{5,5,{29910002,80 }},
{6,6,{29910002,100}},
{7,7,{29910002,200}},

}

config.items["level"]={	--升到X级

{10,1,{29910002,30}},
{15,1,{29910002,30}},
{20,1,{29910002,30}},
{22,2,{29910002,30}},
{24,2,{29910002,30}},
{26,3,{29910002,60}},
{29,3,{29910002,60}},
{32,4,{29910002,60}},
{34,4,{29910002,60}},
{35,5,{29910002,60}},
{36,5,{29910002,60}},
{37,6,{29910002,60}},
{38,6,{29910002,60}},
{39,7,{29910002,60}},
{40,7,{29910002,60}},

}

config.items["power"]={ --战力达到XXX

{5000 ,1,{29910002,40}},
{10000,1,{29910002,40}},
{15000,2,{29910002,40}},
{20000,2,{29910002,40}},
{24000,3,{29910002,40}},
{28000,3,{29910002,40}},
{32000,4,{29910002,50}},
{36000,4,{29910002,50}},
{39000,5,{29910002,50}},
{42000,5,{29910002,50}},
{45000,6,{29910002,50}},
{48000,6,{29910002,50}},
{50000,7,{29910002,50}},
{52000,7,{29910002,50}},

}

config.items["stage"]={ --通关章节x
                       
{60105,60115,{20233002,5 }},
{60208,60219,{20233002,5 }},
{60308,60308,{20233002,10}},
{60406,60406,{20233002,10}},
{60416,60416,{20233002,20}},
{60505,60505,{20233002,20}},
{60606,60606,{20233002,30}},
{60710,60710,{20233002,30}},

}

config.items["stage_boss"]={ --累计击败X次关卡BOSS

{10, {20235001,10}},
{20, {20235001,20}},
{40, {20235001,30}},
{80, {20235001,40}},
{160,{20235001,50}},
{320,{20235001,60}},
{480,{20235001,70}},
{640,{20235001,80}},
{720,{20235001,90}},
{900,{20235001,100}},

}

config.items["low_draw"]={ --累计进行X次初级招募

{5, {20234001,1}},
{10,{20234001,1}},
{15,{20234001,1}},
{20,{20234001,1}},
{25,{20234001,1}},
{30,{20234001,1}},
{35,{20234001,1}},
{40,{20234001,1}},

}

config.items["high_draw"]={ --累计进行X次高级招募

{2, {20234001,2}},
{4, {20234001,2}},
{6, {20234001,2}},
{8, {20234001,2}},
{10,{20234001,2}},
{12,{20234001,2}},
{14,{20234001,2}},
{16,{20234001,2}},

}

config.items["skill_level5"]={ --任意英雄X个技能升级X级
{6,	{20233002,2} }
}
config.items["skill_level10"]={ --任意英雄X个技能升级X级
{6,	{20233002,2} },
{10,{20233002,2} }
}
config.items["skill_level15"]={ --任意英雄X个技能升级X级
{6,	{20233002,3} },
{10,{20233002,3} },
}
config.items["skill_level20"]={ --任意英雄X个技能升级X级
{6,	{20233002,5} },
{10,{20233002,5} },
}
config.items["skill_level25"]={ --任意英雄X个技能升级X级
{6,	{20233002,5} },
{10,{20233002,5} },
{15,{20233002,5} },
}
config.items["skill_level30"]={ --任意英雄X个技能升级X级
{6,	{20233002,6} },
{10,{20233002,6} },
{15,{20233002,6} },
}
config.items["skill_level35"]={ --任意英雄X个技能升级X级
{6,	{20233002,6} },
{10,{20233002,6} },
{15,{20233002,6} },
}
config.items["skill_level40"]={ --任意英雄X个技能升级X级
{6,	{20233002,8 } },
{10,{20233002,9 } },
{15,{20233002,10} },
}

config.items["color2"]={ --X个英雄进阶到2品质
{1,{20236004,1} },
{2,{20236004,2} },
{3,{20236004,3} },
{4,{20236004,4} },
{5,{20236004,5} },
}

config.items["color3"]={ --X个英雄进阶到3品质
{1,{20236004,1} },
{2,{20236004,2} },
{3,{20236004,3} },
{4,{20236004,4} },
{5,{20236004,5} },
}

config.items["color4"]={ --X个英雄进阶到4品质

{1,{20236004,1}},
{2,{20236004,2}},
{3,{20236004,3}},
{4,{20236004,4}},
{5,{20236004,5}},
}

config.items["color5"]={ --X个英雄进阶到5品质
{1,{20236004,1}},
{2,{20236004,2}},
{3,{20236004,3}},
{4,{20236004,4}},
{5,{20236004,5}},
}

config.items["magic"]={ --累计附魔X次

{10, {20235001,10 }},
{20, {20235001,20 }},
{40, {20235001,30 }},
{80, {20235001,40 }},
{160,{20235001,50 }},
{320,{20235001,60 }},
{480,{20235001,70 }},
{640,{20235001,80 }},
{720,{20235001,90 }},
{900,{20235001,100}},

}
config.items["embed1"]={ --累计镶嵌至少X颗1级宝石

{5, {20234001,1}},
{10,{20234001,1}},
{15,{20234001,1}},
{20,{20234001,1}},
{25,{20234001,1}},
{30,{20234001,1}},
{35,{20234001,1}},
{40,{20234001,1}},

}

config.items["embed2"]={ --累计镶嵌至少X颗2级宝石

{5, {20234001,1}},
{10,{20234001,1}},
{15,{20234001,1}},
{20,{20234001,1}},
{25,{20234001,1}},
{30,{20234001,1}},
{35,{20234001,1}},
{40,{20234001,1}},

}

config.items["embed3"]={ --累计镶嵌至少X颗3级宝石

{5, {20234001,1}},
{10,{20234001,1}},
{15,{20234001,1}},
{20,{20234001,1}},
{25,{20234001,1}},
{30,{20234001,1}},
{35,{20234001,1}},
{40,{20234001,1}},

}

config.items["reward_mission"]={ --累计完成X次赏金任务

{10,{20236004,2}},
{20,{20236004,3}},
{30,{20236004,3}},
{40,{20236004,5}},
{50,{20236004,5}},
{60,{20236004,8}},
{70,{20236004,8}},

}

config.items["union_mission"]={ --累计完成X次工会任务

{10,{20236004,1}},
{20,{20236004,1}},
{30,{20236004,3}},
{40,{20236004,3}},
{50,{20236004,5}},
{60,{20236004,5}},
{70,{20236004,8}},
              
}
config.items["corps_mission"]={ --累计完成X次暮光军团

{10,{20139001,2}},
{20,{20139001,2}},
{30,{20139001,3}},
{40,{20139001,3}},
{50,{20139001,4}},
{60,{20139001,4}},
{70,{20139001,6}},

}


config.items["goblin_mission"]={ --累计完成X次地精矿洞

{1,{20139001,1}},
{2,{20139001,2}},
{3,{20139001,2}},
{4,{20139001,3}},
{5,{20139001,3}},
{6,{20139001,4}},
{7,{20139001,5}},
             
}

config.items["arena"]={ --累计进行X次竞技场

{5, {20233001,1}},
{10,{20233001,1}},
{15,{20233001,3}},
{20,{20233001,3}},
{25,{20233001,5}},
{30,{20233001,5}},
{35,{20233001,7}},

}

config.items["hell"]={ --击败试炼场第X关（1-15）

{3, {20233001,1}},
{6, {20233001,3}},
{9, {20233001,5}},
{12,{20233001,7}},
{15,{20233001,9}},

}


config.items["treasure"]={ --累计使用X次遗迹藏宝图

{10,{20125001,1}},
{20,{20125001,2}},
{30,{20125001,3}},
{40,{20125001,4}},
{50,{20125001,5}},
{60,{20125001,6}},
{70,{20125001,7}},

}

config.items["skyfall"]={ --累计击败X个天灾军团

{10,{20125001,1}},
{20,{20125001,2}},
{30,{20125001,3}},
{40,{20125001,4}},
{50,{20125001,5}},
{60,{20125001,7}},
              
}

config.items["tinythree"]={ --累计完成X次小三环

{1,{20233001,1}},
{2,{20233001,3}},
{3,{20233001,5}},
{4,{20233001,7}},
{5,{20233001,9}},

}

config.items["blood"]={ --累计完成X次提尔之手

{1,{20233001,1}},
{2,{20233001,3}},
{3,{20233001,5}},
{4,{20233001,7}},
{5,{20233001,9}},

}

config.items["world_boss"]={ --击败世界BOSS xxx

{1,{20139001,1}},
{2,{20139001,2}},
{3,{20139001,2}},
{4,{20139001,3}},
{5,{20139001,3}},
{6,{20139001,4}},
{7,{20139001,5}},
}

config.items["union_boss"]={ --累计进行公会BOSS战斗X次

{1,{20139001,1}},
{2,{20139001,2}},
{3,{20139001,2}},
{4,{20139001,3}},
{5,{20139001,3}},
{6,{20139001,5}},

}

config.items["half"]={ --半价出售

{1,84, 420, {20131007,1}},
{2,200,1000,{20131009,1}},
{3,200,400, {20236011,2}},
{4,200,400, {20236004,120}},
{5,180,900, {20131008,1}},
{6,162,405, {20139004,3}},
{7,240,600, {20236001,2}},

}
        

config.conditions["login"] 			= { g_acr_datacell_index.first7_login_flag			, g_action_data_index.login_count			}
config.conditions["level"] 			= { g_acr_datacell_index.first7_level_flag			, g_action_data_index.level_count			}
config.conditions["power"] 			= { g_acr_datacell_index.first7_power_flag			, g_action_data_index.power_count			}
config.conditions["stage"] 			= { g_acr_datacell_index.first7_stage_flag			, g_action_data_index.stage_count			}
config.conditions["stage_boss"] 	= { g_acr_datacell_index.first7_stage_boss_flag		, g_action_data_index.stage_boss_count		}
config.conditions["magic"] 			= { g_acr_datacell_index.first7_magic_flag			, g_action_data_index.magic_count			}
config.conditions["embed1"] 		= { g_acr_datacell_index.first7_embed1_flag			, g_action_data_index.embed1_count			}
config.conditions["embed2"] 		= { g_acr_datacell_index.first7_embed2_flag			, g_action_data_index.embed2_count			}
config.conditions["embed3"] 		= { g_acr_datacell_index.first7_embed3_flag			, g_action_data_index.embed3_count			}
config.conditions["skill_level5"] 	= { g_acr_datacell_index.first7_skill_level5_flag	, g_action_data_index.skill_level5_count	}
config.conditions["skill_level10"] 	= { g_acr_datacell_index.first7_skill_level10_flag	, g_action_data_index.skill_level10_count	}
config.conditions["skill_level15"] 	= { g_acr_datacell_index.first7_skill_level15_flag	, g_action_data_index.skill_level15_count	}
config.conditions["skill_level20"] 	= { g_acr_datacell_index.first7_skill_level20_flag	, g_action_data_index.skill_level20_count	}
config.conditions["skill_level25"] 	= { g_acr_datacell_index.first7_skill_level25_flag	, g_action_data_index.skill_level25_count	}
config.conditions["skill_level30"] 	= { g_acr_datacell_index.first7_skill_level30_flag	, g_action_data_index.skill_level30_count	}
config.conditions["skill_level35"] 	= { g_acr_datacell_index.first7_skill_level35_flag	, g_action_data_index.skill_level35_count	}
config.conditions["skill_level40"] 	= { g_acr_datacell_index.first7_skill_level40_flag	, g_action_data_index.skill_level40_count	}
config.conditions["low_draw"] 		= { g_acr_datacell_index.first7_low_draw_flag		, g_action_data_index.low_draw_count		}
config.conditions["high_draw"] 		= { g_acr_datacell_index.first7_high_draw_flag		, g_action_data_index.high_draw_count		}
config.conditions["treasure"] 		= { g_acr_datacell_index.first7_treasure_flag		, g_action_data_index.treasure_count		}
config.conditions["color2"] 		= { g_acr_datacell_index.first7_color2_flag			, g_action_data_index.color2_count			}
config.conditions["color3"] 		= { g_acr_datacell_index.first7_color3_flag			, g_action_data_index.color3_count			}
config.conditions["color4"] 		= { g_acr_datacell_index.first7_color4_flag			, g_action_data_index.color4_count			}
config.conditions["color5"] 		= { g_acr_datacell_index.first7_color5_flag			, g_action_data_index.color5_count			}
config.conditions["reward_mission"] = { g_acr_datacell_index.first7_reward_mission_flag	, g_action_data_index.reward_mission_count	}
config.conditions["corps_mission"] 	= { g_acr_datacell_index.first7_corps_mission_flag	, g_action_data_index.corps_mission_count	}
config.conditions["goblin_mission"] = { g_acr_datacell_index.first7_goblin_flag			, g_action_data_index.goblin_count			}
config.conditions["arena"] 			= { g_acr_datacell_index.first7_arena_flag			, g_action_data_index.arena_count			}
config.conditions["hell"] 			= { g_acr_datacell_index.first7_hell_flag			, g_action_data_index.hell_stage_count		}
config.conditions["skyfall"] 		= { g_acr_datacell_index.first7_skyfall_flag		, g_action_data_index.skyfall_count			}
config.conditions["tinythree"] 		= { g_acr_datacell_index.first7_tinythree_flag		, g_action_data_index.tinythree_count		}
config.conditions["blood"] 			= { g_acr_datacell_index.first7_blood_flag			, g_action_data_index.blood_count			}
config.conditions["world_boss"] 	= { g_acr_datacell_index.first7_world_boss_flag		, g_action_data_index.world_boss_count		}
config.conditions["union_boss"] 	= { g_acr_datacell_index.first7_union_boss_flag		, g_action_data_index.union_boss_count		}
config.conditions["union_mission"]	= { g_acr_datacell_index.first7_union_mission_flag	, g_action_data_index.union_mission_count  	}
config.conditions["half"] 			= { g_acr_datacell_index.first7_half_flag			, g_action_data_index.login_count		  	}
-----------------------------------------------for server---------------------------------------------
function x002611_OnUpdateAcrConfig(params)
	return ResultCode_ResultOK
end                            
                                                                                                                                                          
function x002611_IsAcrActive(player)

	--开关
	if config.switch_open == 0 then 
		return ResultCode_Activity_NotStart,-1
	end
	
	local dbplayer = player:GetDBPlayerInfo()
	if dbplayer == nil then
		return ResultCode_Activity_NotStart,-1
	end
		
	local cdate = dbplayer:create_time()
	local days = LuaMtTimerManager():DiffDayToNow(cdate)
	if days >= 30 then
		return ResultCode_Activity_NotStart,-1
	end

	return ResultCode_ResultOK,days
	
end

function x002611_OnUpdate(player)
	local active,days = x002611_IsAcrActive(player)
	local config_check= 0
	if active == ResultCode_ResultOK and days >=0 then	
		config_check = 1
	end
	x000011_GAcrstatusUpdate(player,{["value"]=config.script_id,["config_check"]=config_check,["user_data"]=days+1})
end


function x002611_DoReward(player,params)

	local active,days = x002611_IsAcrActive(player)
	--print("active"..active.." days"..days)
	if active ~= ResultCode_ResultOK or days < 0 then	
		return player:SendClientNotify("RewardConditionLimit",-1,-1)
	end
	
	local key ="nil"
	local index = params["index"]
	for k,v in pairs(params) do
		if v==999 then
			key=k
		end
	end
	
	if key == "nil" then
		return player:SendClientNotify("Invalid_Request",-1,-1)
	end
	
	local dc_container = player:DataCellContainer()
	if dc_container == nil then
		return player:SendClientNotify("Invalid_Request",-1,-1)
	end

	--print("index"..index.." key"..key)
	local rewards = config.items[key][index+1]
	local start_index = 2
	if  key =="login" or  key =="level" or  key =="power" or key == "stage" then
		start_index=3
	elseif key =="half" then
		start_index=4
	end
	
	--背包检查
	if x000010_CheckBag(player,start_index,rewards) == false then
		return ResultCode_InternalResult
	end
		

	local count = #config.items[key]
	if index>=count then
		return player:SendClientNotify("Invalid_Request",-1,-1)
	end
	
	local flag_index = config.conditions[key][1]
	
	local flag_data=dc_container:get_data_32(CellLogicType_AcRewardCell, flag_index)
	--print("flag_data"..flag_data)
	
	--半价还得检查钱
	if key =="half" then
		if player:DelItemById(TokenType_Token_Crystal,rewards[2],ItemDelLogType_DelType_Acr,2611) == false then
			return player:SendClientNotify("CrystalNotEnough",-1,-1)
		end
		--
		if dc_container:check_bit_data_32(flag_data,index)== false then		
			return player:SendClientNotify("BuyConditionLimit",-1,-1)
		end
		--
		if dc_container:check_bit_data_32(flag_data,index+16)== true then		
			return player:SendClientNotify("BuyAlready",-1,-1)
		end
	else
		if dc_container:check_bit_data_32(flag_data,index)== false then		
			return player:SendClientNotify("RewardConditionLimit",-1,-1)
		end
		--
		if dc_container:check_bit_data_32(flag_data,index+16)== true then		
			return player:SendClientNotify("RewardAlready",-1,-1)
		end
	end
	
	--标记
	local new_flag_data = dc_container:set_bit_data_32(flag_data,index+16)
	dc_container:set_data_32(CellLogicType_AcRewardCell, flag_index,new_flag_data,true)
			
			
	--给道具
	for i=start_index,#rewards,1 do
		local id = rewards[i][1]
		local count = rewards[i][2]
		player:AddItemByIdWithNotify(id,count,ItemAddLogType_AddType_Acr,2611,1)
	end
	
	return ResultCode_ResultOK
end

---------------------------------------------------

function x002611_OnConditionResolve(player,key)
	--print("-----------------------key"..key)

	local dc_container = player:DataCellContainer()
	if dc_container == nil then
		return ResultCode_Invalid_Request
	end
		
	local items_table = config.items[key]
	if items_table == nil then
		--print("x002611_OnConditionResolve key"..key)
		return ResultCode_ResultOK
	end
	
	local flag_index = config.conditions[key][1]
	local count_index = config.conditions[key][2]	
	local count_data=dc_container:get_data_32(CellLogicType_ActionData, count_index)
	--print(key.." count_data"..count_data)

	--print(key..flag_data)
	for index=0,#items_table-1,1 do		
		local flag_data=dc_container:get_data_32(CellLogicType_AcRewardCell, flag_index)
		if dc_container:check_bit_data_32(flag_data,index) == false then				
			if items_table[index+1][1] <= count_data then
				local new_data = dc_container:set_bit_data_32(flag_data,index)
				--print(key..flag_data)
				dc_container:set_data_32(CellLogicType_AcRewardCell, flag_index,new_data,true)
			end
		end
	end
	return ResultCode_ResultOK
end


function x002611_OnRegisterFunction(proxy)
	
	if proxy ~= nil then	
		proxy:RegFunc("xOnLevelUpTo"		,2611)			
		proxy:RegFunc("xUpdateBattleScore"	,2611)
		proxy:RegFunc("xPlayerRaidStage"	,2611)	
		proxy:RegFunc("xDrawCrystal"		,2611)
		proxy:RegFunc("xDrawGold"			,2611)	
		proxy:RegFunc("xOnMonsterDead"		,2611)
		proxy:RegFunc("xOnEnchantEquip"		,2611)	
		proxy:RegFunc("xCostItem"			,2611)	
		proxy:RegFunc("xOnInsetEquip"		,2611)
		proxy:RegFunc("xOnUpgradeSKill"		,2611)
		proxy:RegFunc("xOnUpgradeColor"		,2611)			
	end
end


function x002611_xOnNewDay(player,params)
		
	x002611_OnConditionResolve(player,"login")
	x002611_OnConditionResolve(player,"half")	

	x002611_OnUpdate(player)	
	
	return ResultCode_ResultOK
end

function x002611_xOnLevelUpTo(player,params)	
	x002611_OnConditionResolve(player,"level")	
	return ResultCode_ResultOK
end
		
function x002611_xUpdateBattleScore(player,params)
	x002611_OnConditionResolve(player,"power")
	return ResultCode_ResultOK
end

function x002611_xPlayerRaidStage(player,params)	
	--x002611_OnConditionResolve(player,"stage")	
	local key = "stage"
	local dc_container = player:DataCellContainer()
	if dc_container == nil then
		return ResultCode_Invalid_Request
	end
		
	local items_table = config.items[key]
	if items_table == nil then
		--print("x002611_OnConditionResolve key"..key)
		return ResultCode_ResultOK
	end
	
	local flag_index = config.conditions[key][1]
	local count_index = config.conditions[key][2]	
	local count_data=dc_container:get_data_32(CellLogicType_ActionData, count_index)

	local camp = player:GetCamp()
	for index=0,#items_table-1,1 do		
		local flag_data=dc_container:get_data_32(CellLogicType_AcRewardCell, flag_index)								
		if dc_container:check_bit_data_32(flag_data,index) == false then				
			if items_table[index+1][camp+1] <= count_data then
				local new_data = dc_container:set_bit_data_32(flag_data,index)
				dc_container:set_data_32(CellLogicType_AcRewardCell, flag_index,new_data,true)
			end
		end
	end
	return ResultCode_ResultOK
end

function x002611_xDrawCrystal(player,params)
x002611_OnConditionResolve(player,"high_draw")	
	return ResultCode_ResultOK
end
function x002611_xDrawGold(player,params)
	x002611_OnConditionResolve(player,"low_draw")	
	return ResultCode_ResultOK
end
function x002611_xOnMonsterDead(player,params)
	x002611_OnConditionResolve(player,"stage_boss")	
	return ResultCode_ResultOK
end
function x002611_xOnEnchantEquip(player,params)
	x002611_OnConditionResolve(player,"magic")	
	return ResultCode_ResultOK
end
function x002611_xCostItem(player,params)
	x002611_OnConditionResolve(player,"treasure")	
	return ResultCode_ResultOK
end

function x002611_xAddActivityTimes(player,params)
	x002611_OnConditionResolve(player,"reward_mission")
	x002611_OnConditionResolve(player,"corps_mission" )	
	x002611_OnConditionResolve(player,"goblin_mission")
	x002611_OnConditionResolve(player,"union_mission")
	x002611_OnConditionResolve(player,"arena"		 )	
	x002611_OnConditionResolve(player,"hell"		 )
	x002611_OnConditionResolve(player,"skyfall"		 )
	x002611_OnConditionResolve(player,"tinythree"	 )	
	x002611_OnConditionResolve(player,"blood"		 )	
	x002611_OnConditionResolve(player,"world_boss"	 )
	x002611_OnConditionResolve(player,"union_boss"	 )
	return ResultCode_ResultOK
end

function x002611_xOnInsetEquip(player,params)	
	x002611_OnConditionResolve(player,"embed1")	
	x002611_OnConditionResolve(player,"embed2")	
	x002611_OnConditionResolve(player,"embed3")	
	return ResultCode_ResultOK
end

function x002611_xOnUpgradeSKill(player,params)
	x002611_OnConditionResolve(player,"skill_level5")	
	x002611_OnConditionResolve(player,"skill_level10")	
	x002611_OnConditionResolve(player,"skill_level15")	
	x002611_OnConditionResolve(player,"skill_level20")	
	x002611_OnConditionResolve(player,"skill_level25")	
	x002611_OnConditionResolve(player,"skill_level30")	
	x002611_OnConditionResolve(player,"skill_level35")	
	x002611_OnConditionResolve(player,"skill_level40")	
	return ResultCode_ResultOK
end

function x002611_xOnUpgradeColor(player,params)
	x002611_OnConditionResolve(player,"color2")	
	x002611_OnConditionResolve(player,"color3")	
	x002611_OnConditionResolve(player,"color4")	
	x002611_OnConditionResolve(player,"color5")	
	return ResultCode_ResultOK
end


--------------------------------------------------------------for client-------------------------------
return config