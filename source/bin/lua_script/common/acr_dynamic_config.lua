--用来放置全局的脚本数据定义
--json = loadfile("../../lua_script/common/json.lua")()
g_acr_server_index={}								--对应ServerInfo acr_data字段索引
g_acr_server_index.all_power_last_serial		=	0 	--战力比拼活动流水id
g_acr_server_index.all_power_count_1			=	1 	--人数
g_acr_server_index.all_power_count_2			=	2 	--人数
g_acr_server_index.all_power_count_3			=	3 	--人数
g_acr_server_index.all_power_count_4			=	4 	--人数
g_acr_server_index.all_power_count_5			=	5 	--人数
g_acr_server_index.all_power_count_6			=	6 	--人数
g_acr_server_index.all_power_count_7			=	7 	--人数
g_acr_server_index.all_level_last_serial		=	8 	--等级比拼活动流水id
g_acr_server_index.all_level_count_1			=	9 	--人数
g_acr_server_index.all_level_count_2			=	10 	--人数
g_acr_server_index.all_level_count_3			=	11 	--人数
g_acr_server_index.all_level_count_4			=	12 	--人数
g_acr_server_index.all_level_count_5			=	13 	--人数
g_acr_server_index.all_level_count_6			=	14	--人数
g_acr_server_index.all_level_count_7			=	15 	--人数
g_acr_server_index.quik_buy_last_serial			=	16 	--秒杀活动流水id
g_acr_server_index.quik_buy_index				=	17 	--索引
g_acr_server_index.quik_buy_count				=	18 	--余额
g_acr_server_index.all_buy_last_serial			=	19 	--团购活动流水id
g_acr_server_index.all_buy_index				=	20 	--索引
g_acr_server_index.all_buy_count				=	21 	--人数
g_acr_server_index.all_buy_time					=	22 	--剩余时间

--奖励活动索引
g_acr_datacell_index={}					  			--对应AllPacketEnum.proto里的AcRewardIndex枚举
g_acr_datacell_index.draw_index					=	0 --大富翁最后的位置
g_acr_datacell_index.sign_flag					=	1 --签到标记
g_acr_datacell_index.login_flag					=	2 --登录标记
g_acr_datacell_index.draw_count					=	3 --抽奖次数
g_acr_datacell_index.charge_flag				=	4 --首充标记
g_acr_datacell_index.buy_count					=	5 --每周优惠购买次数
g_acr_datacell_index.level_race_flag			=	6 --等级比拼标记
g_acr_datacell_index.power_race_flag			=	7 --战力比拼标记
g_acr_datacell_index.hero_race_flag				=	8 --收集比拼标记
g_acr_datacell_index.time_reward_flag			=	9 --倒计时领奖标记
g_acr_datacell_index.scene_race_flag			=  10 --场景冲刺领奖标记
g_acr_datacell_index.scene_race_data			=  11 --场景冲刺数据
--创角7天
g_acr_datacell_index.first7_login_flag			=	12 	----登陆奖励
g_acr_datacell_index.first7_level_flag			=	13 	----升到X级
g_acr_datacell_index.first7_power_flag			=	14 	----战力达到XXX
g_acr_datacell_index.first7_stage_flag			= 	15	----通关章节
g_acr_datacell_index.first7_stage_boss_flag		= 	16	----累计击败X次关卡BOSS
g_acr_datacell_index.first7_magic_flag			= 	17	----累计附魔X次
g_acr_datacell_index.first7_embed1_flag			= 	18	----累计镶嵌至少X颗X级宝石1
g_acr_datacell_index.first7_embed2_flag			= 	19	----累计镶嵌至少X颗X级宝石2
g_acr_datacell_index.first7_embed3_flag			= 	20	----累计镶嵌至少X颗X级宝石3
g_acr_datacell_index.first7_skill_level5_flag	= 	21	----任意英雄X个技能升级X级1
g_acr_datacell_index.first7_skill_level10_flag	= 	22	----任意英雄X个技能升级X级2
g_acr_datacell_index.first7_skill_level15_flag	= 	23	----任意英雄X个技能升级X级1
g_acr_datacell_index.first7_skill_level20_flag	= 	24	----任意英雄X个技能升级X级2
g_acr_datacell_index.first7_skill_level25_flag	= 	25	----任意英雄X个技能升级X级1
g_acr_datacell_index.first7_skill_level30_flag	= 	26	----任意英雄X个技能升级X级2
g_acr_datacell_index.first7_skill_level35_flag	= 	27	----任意英雄X个技能升级X级1
g_acr_datacell_index.first7_skill_level40_flag	= 	28	----任意英雄X个技能升级X级2
g_acr_datacell_index.first7_low_draw_flag		= 	29	----累计进行X次初级招募
g_acr_datacell_index.first7_high_draw_flag		= 	30  ----累计进行X次高级招募
g_acr_datacell_index.first7_treasure_flag		= 	31	----累计使用X次遗迹藏宝图
g_acr_datacell_index.first7_color2_flag			= 	32	----X个英雄进阶到X品质2
g_acr_datacell_index.first7_color3_flag			= 	33	----X个英雄进阶到X品质3
g_acr_datacell_index.first7_color4_flag			= 	34	----X个英雄进阶到X品质4
g_acr_datacell_index.first7_color5_flag			= 	35	----X个英雄进阶到X品质5
g_acr_datacell_index.first7_reward_mission_flag	= 	36	----累计完成X次赏金任务
g_acr_datacell_index.first7_corps_mission_flag	= 	37	----累计完成X次暮光军团
g_acr_datacell_index.first7_goblin_flag			= 	38	----累计完成X次地精矿洞
g_acr_datacell_index.first7_arena_flag			= 	39	----累计进行X次竞技场
g_acr_datacell_index.first7_hell_flag			= 	40	----击败试炼场第X关（1-15）
g_acr_datacell_index.first7_skyfall_flag		= 	41	----累计击败X个天灾军团
g_acr_datacell_index.first7_tinythree_flag		= 	42	----累计完成X次小三环
g_acr_datacell_index.first7_blood_flag			= 	43	----累计完成X次提尔之手
g_acr_datacell_index.first7_world_boss_flag		= 	44	----击败世界BOSS X次
g_acr_datacell_index.first7_union_boss_flag		= 	45	----累计进行公会BOSS战斗X次
g_acr_datacell_index.first7_half_flag			= 	46	----半价出售
--
g_acr_datacell_index.life_card_flag				=   47 --终身卡奖励标记
g_acr_datacell_index.month_card_flag			=   48 --月卡奖励标记   
g_acr_datacell_index.invest_flag			    =   49 --基金奖励标记    
g_acr_datacell_index.invest_all_flag			=   50 --全民福利奖励标记   
g_acr_datacell_index.all_charge_flag			=   51 --累计充值    
g_acr_datacell_index.all_consume_flag			=   52 --累计消费  
g_acr_datacell_index.first7_union_mission_flag	= 	53 --工会任务次数       
g_acr_datacell_index.day_charge_flag			= 	54 --单日充值  
g_acr_datacell_index.charge_double_flag			= 	55 --首充双倍      
g_acr_datacell_index.date_charge_flag			=   56 --周期累计充值    
g_acr_datacell_index.date_consume_flag			=   57 --周期累计消费          
g_acr_datacell_index.invest_type				=   58 --基金类型[0固定,1浮动]      
g_acr_datacell_index.time_package_flag			=   59 --限时礼包标记            
g_acr_datacell_index.time_package_time			=   60 --限时礼包时长       
--
g_acr_datacell_index.thlb_1_times				=   61 --1元特惠礼包次数      
g_acr_datacell_index.thlb_2_times				=   62 --3元特惠礼包次数   
g_acr_datacell_index.thlb_3_times				=   63 --6元特惠礼包次数  
g_acr_datacell_index.tclb_1_times				=   64 --30元套餐礼包 次数      
g_acr_datacell_index.tclb_2_times				=   65 --98元套餐礼包 次数   
g_acr_datacell_index.tclb_3_times				=   66 --198元套餐礼包次数  
g_acr_datacell_index.tclb_4_times				=   67 --328元套餐礼包次数      
g_acr_datacell_index.tclb_5_times				=   68 --648元套餐礼包次数   
g_acr_datacell_index.zlb_1_times			 	=   69 --30元周礼包   次数
g_acr_datacell_index.zlb_2_times			 	=   70 --98元周礼包   次数      
g_acr_datacell_index.zlb_3_times			 	=   71 --198元周礼包  次数   
g_acr_datacell_index.zlb_4_times			 	=   72 --328元周礼包  次数 
g_acr_datacell_index.zlb_5_times			 	=   73 --648元周礼包  次数      
g_acr_datacell_index.ylb_1_times			 	=   74 --30元月礼包   次数   
g_acr_datacell_index.ylb_2_times			 	=   75 --98元月礼包   次数    
g_acr_datacell_index.ylb_3_times			 	=   76 --198元月礼包  次数
g_acr_datacell_index.ylb_4_times			 	=   77 --328元月礼包  次数
g_acr_datacell_index.ylb_5_times			 	=   78 --648元月礼包  次数
g_acr_datacell_index.czlb_1_flag				=   79 --超值礼包标记
g_acr_datacell_index.czlb_2_flag				=   80 --超值礼包标记
g_acr_datacell_index.czlb_3_flag				=   81 --超值礼包标记
g_acr_datacell_index.czlb_4_flag				=   82 --超值礼包标记
g_acr_datacell_index.czlb_5_flag				=   83 --超值礼包标记
g_acr_datacell_index.czlb_6_flag				=   84 --超值礼包标记
g_acr_datacell_index.czlb_7_flag				=   85 --超值礼包标记
g_acr_datacell_index.czlb_8_flag				=   86 --超值礼包标记
g_acr_datacell_index.czlb_9_flag				=   87 --超值礼包标记
g_acr_datacell_index.yjlb_1_times				=   88 --永久礼包次数
g_acr_datacell_index.yjlb_2_times				=   89 --永久礼包次数
g_acr_datacell_index.yjlb_3_times				=   90 --永久礼包次数
g_acr_datacell_index.yjlb_4_times				=   91 --永久礼包次数
g_acr_datacell_index.yjlb_5_times				=   92 --永久礼包次数
g_acr_datacell_index.yjlb_6_times				=   93 --永久礼包次数
g_acr_datacell_index.yjlb_7_times				=   94 --永久礼包次数
g_acr_datacell_index.yjlb_8_times				=   95 --永久礼包次数
g_acr_datacell_index.yjlb_9_times				=   96 --永久礼包次数
g_acr_datacell_index.continue_charge_flag		=   97 --连续充值标记位
g_acr_datacell_index.continue_charge_days		=   98 --充值天数
g_acr_datacell_index.choose_one_flag			=   99 --三选一标记
g_acr_datacell_index.xslb_flag					=   100 --限时礼包开启标记
g_acr_datacell_index.xslb_1_times			 	=   101 --等级限时礼包剩余时间      
g_acr_datacell_index.xslb_2_times			 	=   102 --等级限时礼包剩余时间   
g_acr_datacell_index.xslb_3_times			 	=   103 --等级限时礼包剩余时间    
g_acr_datacell_index.xslb_4_times			 	=   104 --等级限时礼包剩余时间
g_acr_datacell_index.xslb_5_times			 	=   105 --等级限时礼包剩余时间
g_acr_datacell_index.xslb_6_times			 	=   106 --等级限时礼包剩余时间
g_acr_datacell_index.xslb_7_times			 	=   107 --招募限时礼包剩余时间      
g_acr_datacell_index.xslb_8_times			 	=   108 --限时礼包剩余时间   
g_acr_datacell_index.xslb_9_times			 	=   109 --限时礼包剩余时间    
g_acr_datacell_index.xslb_10_times			 	=   110 --限时礼包剩余时间

g_acr_datacell_index.exchange_1_times			=   111 --兑换道具次数      
g_acr_datacell_index.exchange_2_times			=   112 --兑换道具次数   
g_acr_datacell_index.exchange_3_times			=   113 --兑换道具次数    
g_acr_datacell_index.exchange_4_times			=   114 --兑换道具次数
g_acr_datacell_index.exchange_5_times			=   115 --兑换道具次数
g_acr_datacell_index.exchange_6_times			=   116 --兑换道具次数
g_acr_datacell_index.exchange_7_times			=   117 --兑换道具次数      
g_acr_datacell_index.exchange_8_times			=   118 --兑换道具次数
g_acr_datacell_index.exchange_9_times			=   119 --兑换道具次数

g_acr_datacell_index.all_power_acr_flag			=   120 --战力比拼标记
g_acr_datacell_index.all_level_acr_flag			=   121 --等级比拼标记
g_acr_datacell_index.all_buy_acr_flag			=   122 --团购参与标记
g_acr_datacell_index.quik_buy_acr_flag			=   123 --秒杀参与标记

g_acr_datacell_index.yklb_1_times				=   124 --月卡礼包1购买次数
g_acr_datacell_index.yklb_2_times				=   125 --月卡礼包2购买次数
g_acr_datacell_index.zzlb_1_times				=   126 --至尊礼包1购买次数
g_acr_datacell_index.zzlb_2_times				=   127 --至尊礼包2购买次数
								  
g_acr_datacell_index.thlb_reward_flag			=   128	--特惠礼包标记  
g_acr_datacell_index.tclb_reward_flag			=   129 --套餐礼包标记     
g_acr_datacell_index.zlb_reward_flag			=   130 --周礼包标记 
g_acr_datacell_index.ylb_reward_flag			=   131 --月礼包标记
g_acr_datacell_index.xslb_reward_flag			=   132 --限时礼包标记

--上限256
----------------------------------------------------------------------------------
acr_dynamic_config = {}


acr_dynamic_config[2616]={
[1]=g_acr_datacell_index.date_charge_flag,
}
acr_dynamic_config[2617]={
[1]=g_acr_datacell_index.date_consume_flag,
} 

acr_dynamic_config[2624]={
[1]=g_acr_datacell_index.thlb_1_times,
[2]=g_acr_datacell_index.thlb_2_times,
[3]=g_acr_datacell_index.thlb_3_times,
}
acr_dynamic_config[2625]={
[1]=g_acr_datacell_index.tclb_1_times,
[2]=g_acr_datacell_index.tclb_2_times,
[3]=g_acr_datacell_index.tclb_3_times,
[4]=g_acr_datacell_index.tclb_4_times,
[5]=g_acr_datacell_index.tclb_5_times,
}
acr_dynamic_config[2626]={
[1]=g_acr_datacell_index.zlb_1_times,
[2]=g_acr_datacell_index.zlb_2_times,
[3]=g_acr_datacell_index.zlb_3_times,
[4]=g_acr_datacell_index.zlb_4_times,
[5]=g_acr_datacell_index.zlb_5_times,
}
acr_dynamic_config[2627]={
[1]=g_acr_datacell_index.ylb_1_times,
[2]=g_acr_datacell_index.ylb_2_times,
[3]=g_acr_datacell_index.ylb_3_times,
[4]=g_acr_datacell_index.ylb_4_times,
[5]=g_acr_datacell_index.ylb_5_times,
}
acr_dynamic_config[2628]={
[1]=g_acr_datacell_index.xslb_1_times,
[2]=g_acr_datacell_index.xslb_2_times,
[3]=g_acr_datacell_index.xslb_3_times,
[4]=g_acr_datacell_index.xslb_4_times,
[5]=g_acr_datacell_index.xslb_5_times,
[6]=g_acr_datacell_index.xslb_6_times,
[7]=g_acr_datacell_index.xslb_7_times,
}
acr_dynamic_config[2629]={
[1]=g_acr_datacell_index.czlb_1_flag,
}

acr_dynamic_config[2630]={
[1]=g_acr_datacell_index.yjlb_1_times,
[2]=g_acr_datacell_index.yjlb_2_times,
[3]=g_acr_datacell_index.yjlb_3_times,
[4]=g_acr_datacell_index.yjlb_4_times,
[5]=g_acr_datacell_index.yjlb_5_times,
[6]=g_acr_datacell_index.yjlb_6_times,
--[7]=g_acr_datacell_index.yjlb_7_times,
--[8]=g_acr_datacell_index.yjlb_8_times,
--[9]=g_acr_datacell_index.yjlb_9_times,
}

acr_dynamic_config[2631]={
[1]=g_acr_datacell_index.continue_charge_flag
}

acr_dynamic_config[2632]={
[1]=g_acr_datacell_index.exchange_1_times,
[2]=g_acr_datacell_index.exchange_2_times,
[3]=g_acr_datacell_index.exchange_3_times,
[4]=g_acr_datacell_index.exchange_4_times,
[5]=g_acr_datacell_index.exchange_5_times,
[6]=g_acr_datacell_index.exchange_6_times,
[7]=g_acr_datacell_index.exchange_7_times,
[8]=g_acr_datacell_index.exchange_8_times,
[9]=g_acr_datacell_index.exchange_9_times,
}

acr_dynamic_config[2633]={}
acr_dynamic_config[2634]={}
acr_dynamic_config[2635]={
[1]=g_acr_server_index.all_power_count_1,
[2]=g_acr_server_index.all_power_count_2,
[3]=g_acr_server_index.all_power_count_3,
[4]=g_acr_server_index.all_power_count_4,
[5]=g_acr_server_index.all_power_count_5,
[6]=g_acr_server_index.all_power_count_6,
[7]=g_acr_server_index.all_power_count_7,
}
acr_dynamic_config[2636]={
[1]=g_acr_server_index.all_level_count_1,
[2]=g_acr_server_index.all_level_count_2,
[3]=g_acr_server_index.all_level_count_3,
[4]=g_acr_server_index.all_level_count_4,
[5]=g_acr_server_index.all_level_count_5,
[6]=g_acr_server_index.all_level_count_6,
[7]=g_acr_server_index.all_level_count_7,
}
acr_dynamic_config[2637]={
}
acr_dynamic_config[2638]={
}
acr_dynamic_config[2639]={
[1]=g_acr_datacell_index.czlb_2_flag,
}
acr_dynamic_config[2640]={
[1]=g_acr_datacell_index.czlb_3_flag,
}
acr_dynamic_config[2641]={
[1]=g_acr_datacell_index.czlb_4_flag,
}
acr_dynamic_config[2642]={
[1]=g_acr_datacell_index.czlb_5_flag,
}

acr_dynamic_config[2643]={
[1]=g_acr_datacell_index.yklb_1_times,
[2]=g_acr_datacell_index.yklb_2_times,
[3]=g_acr_datacell_index.zzlb_1_times,
[4]=g_acr_datacell_index.zzlb_2_times,
}
------------------------------------------------------------------------------------------------------------------------------------------------


--辅助lua OperativeInstance对象解析出可用的json参数
function x000011_GOperateJsonDecodeToLua( json_string )
	local node = json.decode(json_string)
	local params = x000011_GOperateJsonDecodeBase(node)	
	local node = json.decode(json_string)
	
	params["name"]				= ""
	params["icon"]				= ""
	params["desc"]				= ""
	params["content"]			= ""
	
	if node["Name"] ~= nil then
		params["name"] 		= node["Name"]
	end	
	if node["Icon"] ~= nil then
		params["icon"]	 		= node["Icon"]
	end	
	if node["Desc"] ~= nil then
		params["desc"]			= node["Desc"]
	end
	if node["Content"] ~= nil then
		params["content"] 		= node["Content"]
	end	
	
	return params
end

--辅助c++ OperativeInstance对象解析出可用的json参数
function x000011_GOperateJsonDecodeToC( json_string )
	local node = json.decode(json_string)
	return x000011_GOperateJsonDecodeBase(node)	
end


--通用数据的解析逻辑
function x000011_GOperateJsonDecodeBase( node )
	local params ={}
	params["switch_open"]			= 0	
	params["serial"] 				=-1	
	params["common"]				= 0
	params["script_id"]				=-1
	params["show_begin_date"]       =-1
	params["show_end_date"]         =-1
	params["show_begin_days"]       =-1
	params["show_end_days"]         =-1
	params["show_begin_time"]       =-1
	params["show_end_time"]         =-1
	params["normal_begin_date"]     =-1
	params["normal_end_date"]       =-1
	params["normal_begin_days"]     =-1
	params["normal_end_days"]       =-1
	params["normal_begin_time"]     =-1
	params["normal_end_time"]       =-1	
	
	--local node = json.decode(json_string)
	if node["Open"] ~= nil then
		local value = tonumber(node["Open"])
		if value~= nil then
			params["switch_open"]= value
		end
	end
	
	if node["Id"] ~= nil then
		local value = tonumber(node["Id"])
		if value~= nil then
			params["serial"]= value
		end
	end
	
	if node["Common"] ~= nil then
		local value = tonumber(node["Common"])
		if value~= nil then
			params["common"]= value
		end
	end

	if node["ActivityEnumId"] ~= nil then
		local value = tonumber(node["ActivityEnumId"])
		if value~= nil then
			params["script_id"]= value
		end
	end	
	
	if node["ShowStartDate"] ~= nil then
		local str = string.split(node["ShowStartDate"]," ")
		local date = string.gsub(str[1], "-", "")
		local time = string.gsub(str[2], ":", "")
		
		local value = tonumber(date)	
		if value~= nil then
			params["show_begin_date"]= value
		end
		value = tonumber(time)
		if value~= nil then
			params["show_begin_time"]= value
		end		
	end
	
	if node["ShowEndDate"] ~= nil then
		local str 	= string.split(node["ShowEndDate"]," ")
		local date = string.gsub(str[1], "-", "")
		local time = string.gsub(str[2], ":", "")
		
		local value = tonumber(date)	
		if value~= nil then
			params["show_end_date"]= value
		end
		value = tonumber(time)
		if value~= nil then
			params["show_end_time"]= value
		end	
	end
	
	if node["StartDate"] ~= nil then
		local str 	= string.split(node["StartDate"]," ")
		local date = string.gsub(str[1], "-", "")
		local time = string.gsub(str[2], ":", "")
		
		local value = tonumber(date)	
		if value~= nil then
			params["normal_begin_date"]= value
		end
		value = tonumber(time)
		if value~= nil then
			params["normal_begin_time"]=value
		end	
	end
	
	if node["EndDate"] ~= nil then
	
		local str		= string.split(node["EndDate"]," ")
		local date = string.gsub(str[1], "-", "")
		local time = string.gsub(str[2], ":", "")

		local value = tonumber(date)	
		if value~= nil then
			params["normal_end_date"]=value
		end
		value = tonumber(time)
		if value~= nil then
			params["normal_end_time"]=value
		end	
	end	
	
	if node["ShowStartDays"] ~= nil then
		--print("ShowStartDays"..node["ShowStartDays"])
		local value = tonumber(node["ShowStartDays"])
		if value~= nil then
			params["show_begin_days"]=value
		end
	end
	
	if node["ShowEndDays"] ~= nil then
		--print("ShowEndDays"..node["ShowEndDays"])
		local value = tonumber(node["ShowEndDays"])
		if value~= nil then
			params["show_end_days"]=value
		end
	end
	
	if node["ShowStartTime"] ~= nil then
		local time = string.gsub(node["ShowStartTime"], ":", "")	
		--print("ShowEndTime"..time)		
		local value = tonumber(time)	
		if value~= nil then
			params["show_begin_time"]=value
		end
	end
	
	if node["ShowEndTime"] ~= nil then
		local time = string.gsub(node["ShowEndTime"], ":", "")	
		--print("ShowEndTime"..time)		
		local value = tonumber(time)		
		if value~= nil then
			params["show_end_time"]=value
		end
	end
	
	if node["StartDays"] ~= nil then
		--print("StartDays"..node["StartDays"])
		local value = tonumber(node["StartDays"])
		if value~= nil then
			params["normal_begin_days"]=value
		end
	end
	if node["EndDays"] ~= nil then
		--print("EndDays"..node["EndDays"])
		local value = tonumber(node["EndDays"])
		if value~= nil then
			params["normal_end_days"]=value
		end
	end
	
	if node["StartTime"] ~= nil then
		local time = string.gsub(node["StartTime"], ":", "")			
		--print("StartTime"..time)		
		local value = tonumber(time)
		if value~= nil then
			params["normal_begin_time"]=value
		end
	end
	if node["EndTime"] ~= nil then		
		local time = string.gsub(node["EndTime"], ":", "")	
		--print("EndTime"..time)		
		local value = tonumber(time)
		if value~= nil then
			params["normal_end_time"]=value
		end
	end
	
	return params
end


--辅助lua OperativeInstance对象做判定
function x000011_GOperateCheckShowToLua( object )
	if object == nil then
		return false
	end
	if table.nums(object)==0 then
		return false
	end
	return x000011_GOperateCheckShowBase( object ) or x000011_GOperateCheckOpenToLua( object )
end

--辅助c++ OperativeInstance对象做判定
function x000011_GOperateCheckShowToC( object )
	if object == nil then
		return false
	end

	return x000011_GOperateCheckShowBase( object ) or x000011_GOperateCheckOpenToC( object )	
end

--通用判定逻辑
function x000011_GOperateCheckShowBase( object )
	--开关
	if object.switch_open == 0 then 
		return false
	end	
	
	local ds = LuaActivityManager():GetDateStruct()
	if ds == nil then
		return false
	end
	
	if object.common == 1 then	
		if object.show_begin_date >=0 and object.show_end_date >= 0 then
			if ds.cur_date >= object.show_begin_date and ds.cur_date <= object.show_end_date then
				if object.show_begin_time >= 0 and object.show_end_time >= 0 then
					local curtime = ds.cur_hour*10000+ds.cur_min*100
					if curtime >= object.show_begin_time and curtime <= object.show_end_time then
						return true
					end
				end
			end
		end
	else	
		--print("ds.past_day"..ds.past_day)
		if object.show_begin_days >=0 and object.show_end_days >= 0 then
			if ds.past_day >= object.show_begin_days and ds.past_day <= object.show_end_days then
				if object.show_begin_time >= 0 and object.show_end_time >= 0 then
					local curtime = ds.cur_hour*10000+ds.cur_min*100
					--print("object.script_id "..object.script_id.." curtime"..curtime.." object.show_begin_time"..object.show_begin_time.." object.show_end_time"..object.show_end_time)
					if curtime >= object.show_begin_time and curtime <= object.show_end_time then
						return true
					end
				end
			end
		end
	end	
	
	return false
end

--辅助lua OperativeInstance对象做判定
function x000011_GOperateCheckOpenToLua(object)	
	if object == nil then
		--print("shit1")
		return false
	end
	if table.nums(object) == 0 then
		--print("shit2")
		return false
	end
	
	return x000011_GOperateCheckOpenBase(object)
end

--辅助c++ OperativeInstance对象做判定
function x000011_GOperateCheckOpenToC(object)	
	if object == nil then
		return false
	end
	return x000011_GOperateCheckOpenBase(object)
end

--通用判定逻辑
function x000011_GOperateCheckOpenBase(object)	
	--debug
	--acr_static_config.dumpinfo(object.script_id)
	if object.switch_open == 0 then 
		return false
	end	
	
	local ds = LuaActivityManager():GetDateStruct()
	if ds == nil then
		return false
	end
	
	if object.common == 1 then	
		if object.normal_begin_date >=0 and object.normal_end_date >= 0 then
			if ds.cur_date >= object.normal_begin_date and ds.cur_date <= object.normal_end_date then
				if object.normal_begin_time >= 0 and object.normal_end_time >= 0 then
					local curtime = ds.cur_hour*10000+ds.cur_min*100
					if curtime >= object.normal_begin_time and curtime <= object.normal_end_time then
						return true
					end
				end
			end
		end
	else	
		--print("object.script_id"..talble.nums(object))
		--print("object.normal_begin_days"..object.normal_begin_days.." object.normal_end_days"..object.normal_end_days)
		if object.normal_begin_days >=0 and object.normal_end_days >= 0 then
			if ds.past_day >= object.normal_begin_days and ds.past_day <= object.normal_end_days then
				if object.normal_begin_time >= 0 and object.normal_end_time >= 0 then
					local curtime = ds.cur_hour*10000+ds.cur_min*100
					if curtime >= object.normal_begin_time and curtime <= object.normal_end_time then
						return true
					end
				end
			end
		end
	end
	return false
end


------------------------------------------------------------------------------------------------------------------------------------------------
acr_static_config={}

acr_static_config.content_decode = function(self,json_string)	
	
	local node = json.decode(json_string)
	
	--print("acr_static_config["..self.script_id.."].decode"..table.nums(node))
	
	local i = 1		--还是从1开始吧
	
	local id = 0
	local count = 0
		
	for m,n in pairs(node) do
		
		local j = 1	--还是从1开始吧
		
		self.items[i]={}		
		self.items[i][1]={}
		local conditios = self.items[i][1]
		
		self.items[i][2]={}
		local items =self.items[i][2]
		
		self.items[i][3]={}
		local rateitem =self.items[i][3]
		
		self.items[i][4]={}
		local extraitems =self.items[i][4]
		
		if n["c"] ~= nil then
			for k,v in pairs(n["c"]) do		
				if v~= nil then
					local con = tonumber(v)		
					if con == nil then	--非数字字符串转换会失败
						conditios[j]=v
					else
						conditios[j]=con
					end
				end
				j = j+1
			end
		end
		
		j = 1		--还是从1开始吧
		
		if n["item"] ~= nil then
			for k,v in pairs(n["item"]) do
				if v["id"] ~= nil and v["count"] ~= nil then
					id = tonumber(v["id"])
					count = tonumber(v["count"])						
					if id ~= nil and count ~= nil then		
						items[j]={}
						items[j][1]=id
						items[j][2]=count					
					end
					j = j+1
				end
			end
		end
		
		j = 1		--还是从1开始吧
		
		if n["rateitem"] ~= nil then
			for k,v in pairs(n["rateitem"]) do
				if v["id"] ~= nil and v["count"] ~= nil and v["rate"] ~= nil then
					id = tonumber(v["id"])
					count = tonumber(v["count"])
					rate = tonumber(v["rate"])							
					if id ~= nil and count ~= nil and rate ~= nil then		
						rateitem[j]={}
						rateitem[j][1]=id
						rateitem[j][2]=count
						rateitem[j][3]=rate					
					end
					j = j+1
				end
			end
		end
		
		j = 1		--还是从1开始吧
		
		if n["extraitem"] ~= nil then
			for k,v in pairs(n["extraitem"]) do
				if v["id"] ~= nil and v["count"] ~= nil then
					id = tonumber(v["id"])
					count = tonumber(v["count"])					
					if id ~= nil and count ~= nil then	
						extraitems[j]={}
						extraitems[j][1]=id
						extraitems[j][2]=count						
					end
					j = j+1
				end
			end
		end
		
		i= i+1
	end
end

acr_static_config.dumpitem = function(self)

	for m,n in pairs(self.items) do 
		print("condition"..table.nums(n[1]))
		for k,v in pairs(n[1]) do
			print(v)
		end
		
		print("item"..table.nums(n[2]))
		for k,v in pairs(n[2]) do
			print("id:"..v[1])
			print("count:"..v[2])
		end
		
		print("rateitem"..table.nums(n[3]))
		for k,v in pairs(n[3]) do
			print("id:"..v[1])
			print("count:"..v[2])
			print("rate:"..v[3])
		end
		
		print("extraitem"..table.nums(n[4]))
		for k,v in pairs(n[4]) do
			print("id:"..v[1])
			print("count:"..v[2])
		end
	end
	return ResultCode_ResultOK
end


acr_static_config.dumpinfo = function(script_id)
	local object = acr_static_config[script_id]
	if object== nil then
		return
	end
	print("serial				"..object.serial				)
	print("switch_open			"..object.switch_open			)
	print("common				"..object.common				)
	print("script_id			"..object.script_id		 		)
	print("type_name			"..object.type_name		 		)
	print("icon					"..object.icon					)
	print("desc					"..object.desc					)
	print("show_begin_days		"..object.show_begin_days		)
	print("show_end_days		"..object.show_end_days	 		)
	print("normal_begin_days	"..object.normal_begin_days 	)
	print("normal_end_days		"..object.normal_end_days		)
	print("show_begin_date		"..object.show_begin_date		)
	print("show_end_date		"..object.show_end_date	 		)
	print("normal_begin_date	"..object.normal_begin_date 	)
	print("normal_end_date		"..object.normal_end_date		)
	print("show_begin_time		"..object.show_begin_time		)
	print("show_end_time		"..object.show_end_time	 		)
	print("normal_begin_time	"..object.normal_begin_time 	)
	print("normal_end_time		"..object.normal_end_time		)
	
	acr_static_config.dumpitem(object)
end

acr_static_config.decode = function(script_id,json_string)
	local object = acr_static_config[script_id]
	if object== nil then
		return nil
	end
	
	local params = x000011_GOperateJsonDecodeToLua(json_string)
	
	object.switch_open 			= params["switch_open"]
	object.serial 				= params["serial"]
	object.common 				= params["common"]
	object.script_id 			= params["script_id"]
	
	object.show_begin_date 		= params["show_begin_date"]
	object.show_end_date 		= params["show_end_date"]
	object.show_begin_days 		= params["show_begin_days"]
	object.show_end_days 		= params["show_end_days"]
	object.show_begin_time 		= params["show_begin_time"]
	object.show_end_time 		= params["show_end_time"]
	
	object.normal_begin_date 	= params["normal_begin_date"]
	object.normal_end_date 		= params["normal_end_date"]
	object.normal_begin_days 	= params["normal_begin_days"]
	object.normal_end_days 		= params["normal_end_days"]
	object.normal_begin_time 	= params["normal_begin_time"]
	object.normal_end_time 		= params["normal_end_time"]
	
	object.type_name 			= params["name"]
	object.icon 				= params["icon"]
	object.desc 				= params["desc"]

	acr_static_config.content_decode(object,params["content"])
	
	return object
end



------------------------------------------------------------------------------------------------------

acr_static_config[2616]={}
acr_static_config[2616].serial 					=	-1			--活动流水id
acr_static_config[2616].switch_open				=	1			--开关
acr_static_config[2616].common					=	0			--通服标记
acr_static_config[2616].script_id				=	002616		--活动id
acr_static_config[2616].type_name				=	"每日特惠"	--活动名字
acr_static_config[2616].icon					=	"null"		--组icon
acr_static_config[2616].desc					=	"null"		--描述
acr_static_config[2616].show_begin_days			=	-1			--开服后多少天显示开始
acr_static_config[2616].show_end_days			=	-1			--开服后多少天显示结束
acr_static_config[2616].normal_begin_days		=	-1			--开服后多少天开始
acr_static_config[2616].normal_end_days			=	-1			--开服后多少天结束
acr_static_config[2616].show_begin_date			=	-1			--显示开始日期
acr_static_config[2616].show_end_date			=	-1			--显示结束日期
acr_static_config[2616].normal_begin_date		=	-1			--正式开始日期
acr_static_config[2616].normal_end_date			=	-1			--正式结束日期
acr_static_config[2616].show_begin_time			=	-1			--显示开始时间
acr_static_config[2616].show_end_time			=	-1			--显示结束时间
acr_static_config[2616].normal_begin_time		=	-1			--正式开始时间
acr_static_config[2616].normal_end_time			=	-1			--正式结束时间
acr_static_config[2616].items = {}--商品id, 次数，{id,数量}
--acr_static_config[2616].items [1]={{6			}, {{20139001,1}, {20139002,1} },{},{} }
--acr_static_config[2616].items [2]={{30		}, {{20139001,2}, {20139002,2} },{},{} }
--acr_static_config[2616].items [3]={{50		}, {{20139001,3}, {20139002,3} },{},{} }
--acr_static_config[2616].items [4]={{100		}, {{20139001,1}, {20139002,1} },{},{} }
--acr_static_config[2616].items [5]={{300		}, {{20139001,2}, {20139002,2} },{},{} }
--acr_static_config[2616].items [6]={{500		}, {{20139001,3}, {20139002,3} },{},{} }
--acr_static_config[2616].items [7]={{1000		}, {{20139001,1}, {20139002,1} },{},{} }
--acr_static_config[2616].items [8]={{2000		}, {{20139001,2}, {20139002,2} },{},{} }
--acr_static_config[2616].items [9]={{5000		}, {{20139001,3}, {20139002,3} },{},{} }
--
--------------------------------------------------------------------------------------------------------
acr_static_config[2617]={}
acr_static_config[2617].serial 					=	-1			--活动流水id
acr_static_config[2617].switch_open				=	1			--开关
acr_static_config[2617].common					=	0			--通服标记
acr_static_config[2617].script_id				=	2617		--活动id
acr_static_config[2617].type_name				=	"累计消费"	--活动名字
acr_static_config[2617].icon					=	"null"		--组icon
acr_static_config[2617].desc					=	"null"		--描述
acr_static_config[2617].show_begin_days			=	-1			--开服后多少天显示开始
acr_static_config[2617].show_end_days			=	-1			--开服后多少天显示结束
acr_static_config[2617].normal_begin_days		=	-1			--开服后多少天开始
acr_static_config[2617].normal_end_days			=	-1			--开服后多少天结束
acr_static_config[2617].show_begin_date			=	-1			--显示开始日期
acr_static_config[2617].show_end_date			=	-1			--显示结束日期
acr_static_config[2617].normal_begin_date		=	-1			--正式开始日期
acr_static_config[2617].normal_end_date			=	-1			--正式结束日期
acr_static_config[2617].show_begin_time			=	-1			--显示开始时间
acr_static_config[2617].show_end_time			=	-1			--显示结束时间
acr_static_config[2617].normal_begin_time		=	-1			--正式开始时间
acr_static_config[2617].normal_end_time			=	-1			--正式结束时间
acr_static_config[2617].items = {}--商品id, 次数, 百分比, 总价值, rmb价值, {id,数量}
--acr_static_config[2617].items [1]={{60		}, {{20139001,1}, {20139002,1} },{},{} }
--acr_static_config[2617].items [2]={{300		}, {{20139001,2}, {20139002,2} },{},{} }
--acr_static_config[2617].items [3]={{500		}, {{20139001,3}, {20139002,3} },{},{} }
--acr_static_config[2617].items [4]={{1000		}, {{20139001,1}, {20139002,1} },{},{} }
--acr_static_config[2617].items [5]={{3000		}, {{20139001,2}, {20139002,2} },{},{} }
--acr_static_config[2617].items [6]={{5000		}, {{20139001,3}, {20139002,3} },{},{} }
--acr_static_config[2617].items [7]={{10000		}, {{20139001,1}, {20139002,1} },{},{} }
--acr_static_config[2617].items [8]={{20000		}, {{20139001,2}, {20139002,2} },{},{} }
--acr_static_config[2617].items [9]={{50000		}, {{20139001,3}, {20139002,3} },{},{} }

acr_static_config[2624]={}
acr_static_config[2624].serial 					=	-1			--活动流水id
acr_static_config[2624].switch_open				=	1			--开关
acr_static_config[2624].common					=	0			--通服标记
acr_static_config[2624].script_id				=	002624		--活动id
acr_static_config[2624].type_name				=	"每日特惠"	--活动名字
acr_static_config[2624].icon					=	"null"		--组icon
acr_static_config[2624].desc					=	"null"		--描述
acr_static_config[2624].show_begin_days			=	-1			--开服后多少天显示开始
acr_static_config[2624].show_end_days			=	-1			--开服后多少天显示结束
acr_static_config[2624].normal_begin_days		=	-1			--开服后多少天开始
acr_static_config[2624].normal_end_days			=	-1			--开服后多少天结束
acr_static_config[2624].show_begin_date			=	-1			--显示开始日期
acr_static_config[2624].show_end_date			=	-1			--显示结束日期
acr_static_config[2624].normal_begin_date		=	-1			--正式开始日期
acr_static_config[2624].normal_end_date			=	-1			--正式结束日期
acr_static_config[2624].show_begin_time			=	-1			--显示开始时间
acr_static_config[2624].show_end_time			=	-1			--显示结束时间
acr_static_config[2624].normal_begin_time		=	-1			--正式开始时间
acr_static_config[2624].normal_end_time			=	-1			--正式结束时间
acr_static_config[2624].items = {}--商品id, 次数，{id,数量}
--acr_static_config[2624].items [1]={{10, 5}, {{20139001,1}, {20139002,1} },{},{} }
--acr_static_config[2624].items [2]={{11, 5}, {{20139001,2}, {20139002,2} },{},{} }
--acr_static_config[2624].items [3]={{12, 5}, {{20139001,3}, {20139002,3} },{},{} }

------------------------------------------------------------------------------------------------------
acr_static_config[2625]={}
acr_static_config[2625].serial 					=	-1			--活动流水id
acr_static_config[2625].switch_open				=	1			--开关
acr_static_config[2625].common					=	0			--通服标记
acr_static_config[2625].script_id				=	002625		--活动id
acr_static_config[2625].type_name				=	"套餐礼包"	--活动名字
acr_static_config[2625].icon					=	"null"		--组icon
acr_static_config[2625].desc					=	"null"		--描述
acr_static_config[2625].show_begin_days			=	-1			--开服后多少天显示开始
acr_static_config[2625].show_end_days			=	-1			--开服后多少天显示结束
acr_static_config[2625].normal_begin_days		=	-1			--开服后多少天开始
acr_static_config[2625].normal_end_days			=	-1			--开服后多少天结束
acr_static_config[2625].show_begin_date			=	-1			--显示开始日期
acr_static_config[2625].show_end_date			=	-1			--显示结束日期
acr_static_config[2625].normal_begin_date		=	-1			--正式开始日期
acr_static_config[2625].normal_end_date			=	-1			--正式结束日期
acr_static_config[2625].show_begin_time			=	-1			--显示开始时间
acr_static_config[2625].show_end_time			=	-1			--显示结束时间
acr_static_config[2625].normal_begin_time		=	-1			--正式开始时间
acr_static_config[2625].normal_end_time			=	-1			--正式结束时间
acr_static_config[2625].items = {}--商品id, 次数, 百分比, 总价值, rmb价值, {id,数量}
--acr_static_config[2625].items[1]={{13, 5, 1200, 5088, 1165}, {{20139001,1}, {20139002,1} },{},{} }
--acr_static_config[2625].items[2]={{14, 5, 1200, 5088, 1165}, {{20139001,2}, {20139002,2} },{},{} }
--acr_static_config[2625].items[3]={{15, 5, 1200, 5088, 1165}, {{20139001,3}, {20139002,3} },{},{} }
--acr_static_config[2625].items[4]={{16, 5, 1200, 5088, 1165}, {{20139001,2}, {20139002,2} },{},{} } 
--acr_static_config[2625].items[5]={{17, 5, 1200, 5088, 1165}, {{20139001,3}, {20139002,3} },{},{} }

------------------------------------------------------------------------------------------------------
acr_static_config[2626]={}
acr_static_config[2626].serial 					=	-1			--活动流水id
acr_static_config[2626].switch_open				=	1			--开关
acr_static_config[2626].common					=	0			--通服标记
acr_static_config[2626].script_id				=	002626		--活动id
acr_static_config[2626].type_name				=	"周礼包"	--活动名字
acr_static_config[2626].icon					=	"null"		--组icon
acr_static_config[2626].desc					=	"null"		--描述
acr_static_config[2626].show_begin_days			=	-1			--开服后多少天显示开始
acr_static_config[2626].show_end_days			=	-1			--开服后多少天显示结束
acr_static_config[2626].normal_begin_days		=	-1			--开服后多少天开始
acr_static_config[2626].normal_end_days			=	-1			--开服后多少天结束
acr_static_config[2626].show_begin_date			=	-1			--显示开始日期
acr_static_config[2626].show_end_date			=	-1			--显示结束日期
acr_static_config[2626].normal_begin_date		=	-1			--正式开始日期
acr_static_config[2626].normal_end_date			=	-1			--正式结束日期
acr_static_config[2626].show_begin_time			=	-1			--显示开始时间
acr_static_config[2626].show_end_time			=	-1			--显示结束时间
acr_static_config[2626].normal_begin_time		=	-1			--正式开始时间
acr_static_config[2626].normal_end_time			=	-1			--正式结束时间
acr_static_config[2626].items = {}--商品id, 次数，{id,数量}
--acr_static_config[2626].items[1]= {{18, 5}, {{20139001,1}, {20139002,1} },{},{} }
--acr_static_config[2626].items[2]= {{19, 5}, {{20139001,2}, {20139002,2} },{},{} }
--acr_static_config[2626].items[3]= {{20, 5}, {{20139001,3}, {20139002,3} },{},{} }
--acr_static_config[2626].items[4]= {{21, 5}, {{20139001,2}, {20139002,2} },{},{} } 
--acr_static_config[2626].items[5]= {{22, 5}, {{20139001,3}, {20139002,3} },{},{} }

------------------------------------------------------------------------------------------------------
acr_static_config[2627]={}
acr_static_config[2627].serial 					=	-1			--活动流水id
acr_static_config[2627].switch_open				=	1			--开关
acr_static_config[2627].common					=	0			--通服标记
acr_static_config[2627].script_id				=	002627		--活动id
acr_static_config[2627].type_name				=	"月礼包"	--活动名字
acr_static_config[2627].icon					=	"null"		--组icon
acr_static_config[2627].desc					=	"null"		--描述
acr_static_config[2627].show_begin_days			=	-1			--开服后多少天显示开始
acr_static_config[2627].show_end_days			=	-1			--开服后多少天显示结束
acr_static_config[2627].normal_begin_days		=	-1			--开服后多少天开始
acr_static_config[2627].normal_end_days			=	-1			--开服后多少天结束
acr_static_config[2627].show_begin_date			=	-1			--显示开始日期
acr_static_config[2627].show_end_date			=	-1			--显示结束日期
acr_static_config[2627].normal_begin_date		=	-1			--正式开始日期
acr_static_config[2627].normal_end_date			=	-1			--正式结束日期
acr_static_config[2627].show_begin_time			=	-1			--显示开始时间
acr_static_config[2627].show_end_time			=	-1			--显示结束时间
acr_static_config[2627].normal_begin_time		=	-1			--正式开始时间
acr_static_config[2627].normal_end_time			=	-1			--正式结束时间
acr_static_config[2627].items = {}--商品id, 次数，{id,数量}
--acr_static_config[2627].items[1]= {{23, 5}, {{20139001,1}, {20139002,1} },{},{} }
--acr_static_config[2627].items[2]= {{24, 5}, {{20139001,2}, {20139002,2} },{},{} }
--acr_static_config[2627].items[3]= {{25, 5}, {{20139001,3}, {20139002,3} },{},{} }
--acr_static_config[2627].items[4]= {{26, 5}, {{20139001,2}, {20139002,2} },{},{} } 
--acr_static_config[2627].items[5]= {{27, 5}, {{20139001,3}, {20139002,3} },{},{} }

------------------------------------------------------------------------------------------------------
acr_static_config[2628]={}
acr_static_config[2628].serial 					=	-1			--活动流水id
acr_static_config[2628].switch_open				=	1			--开关
acr_static_config[2628].common					=	0			--通服标记
acr_static_config[2628].script_id				=	002628		--活动id
acr_static_config[2628].type_name				=	"限时礼包"	--活动名字
acr_static_config[2628].icon					=	"null"		--组icon
acr_static_config[2628].desc					=	"null"		--描述
acr_static_config[2628].show_begin_days			=	-1			--开服后多少天显示开始
acr_static_config[2628].show_end_days			=	-1			--开服后多少天显示结束
acr_static_config[2628].normal_begin_days		=	-1			--开服后多少天开始
acr_static_config[2628].normal_end_days			=	-1			--开服后多少天结束
acr_static_config[2628].show_begin_date			=	-1			--显示开始日期
acr_static_config[2628].show_end_date			=	-1			--显示结束日期
acr_static_config[2628].normal_begin_date		=	-1			--正式开始日期
acr_static_config[2628].normal_end_date			=	-1			--正式结束日期
acr_static_config[2628].show_begin_time			=	-1			--显示开始时间
acr_static_config[2628].show_end_time			=	-1			--显示结束时间
acr_static_config[2628].normal_begin_time		=	-1			--正式开始时间
acr_static_config[2628].normal_end_time			=	-1			--正式结束时间
acr_static_config[2628].items = {}--商品id, 推送类型[1等级推送,2招募推送], 推送参数1,推送参数2, 价格,存在时长 {id,数量}
--acr_static_config[2628].items[1]={{28, 1, 10, 19, 120, 3600, "xx礼包"}, {{20139001,1}, {20139002,1} },{},{} }
--acr_static_config[2628].items[2]={{29, 1, 20, 29, 120, 3600, "xx礼包"}, {{20139001,2}, {20139002,2} },{},{} }
--acr_static_config[2628].items[3]={{30, 1, 30, 39, 120, 3600, "xx礼包"}, {{20139001,3}, {20139002,3} },{},{} }
--acr_static_config[2628].items[4]={{31, 1, 40, 49, 120, 3600, "xx礼包"}, {{20139001,2}, {20139002,2} },{},{} } 
--acr_static_config[2628].items[5]={{32, 1, 50, 59, 120, 3600, "xx礼包"}, {{20139001,3}, {20139002,3} },{},{} }
--acr_static_config[2628].items[6]={{33, 1, 60, 60, 120, 3600, "xx礼包"}, {{20139001,2}, {20139002,2} },{},{} }
--acr_static_config[2628].items[7]={{34, 2, -1, -1, 120, 3600, "xx礼包"}, {{20139001,3}, {20139002,3} },{},{} }
                                                                                       
------------------------------------------------------------------------------------------------------
acr_static_config[2629]={}
acr_static_config[2629].serial 					=	-1			--活动流水id
acr_static_config[2629].switch_open				=	1			--开关
acr_static_config[2629].common					=	0			--通服标记
acr_static_config[2629].script_id				=	002629		--活动id
acr_static_config[2629].type_name				=	"超值礼包1"	--活动名字
acr_static_config[2629].icon					=	"null"		--组icon
acr_static_config[2629].desc					=	"null"		--描述
acr_static_config[2629].show_begin_days			=	-1			--开服后多少天显示开始
acr_static_config[2629].show_end_days			=	-1			--开服后多少天显示结束
acr_static_config[2629].normal_begin_days		=	-1			--开服后多少天开始
acr_static_config[2629].normal_end_days			=	-1			--开服后多少天结束
acr_static_config[2629].show_begin_date			=	-1			--显示开始日期
acr_static_config[2629].show_end_date			=	-1			--显示结束日期
acr_static_config[2629].normal_begin_date		=	-1			--正式开始日期
acr_static_config[2629].normal_end_date			=	-1			--正式结束日期
acr_static_config[2629].show_begin_time			=	-1			--显示开始时间
acr_static_config[2629].show_end_time			=	-1			--显示结束时间
acr_static_config[2629].normal_begin_time		=	-1			--正式开始时间
acr_static_config[2629].normal_end_time			=	-1			--正式结束时间
acr_static_config[2629].items = {}--价格, 原价, {id,数量}
--acr_static_config[2629].items={}
--acr_static_config[2629].items[1]={{60 ,120}, { {20139001,1}, {20139002,1} },{},{} }
--acr_static_config[2629].items[2]={{120,120}, { {20139001,2}, {20139002,2} },{},{} }
--acr_static_config[2629].items[3]={{240,120}, { {20139001,3}, {20139002,3} },{},{} }
--acr_static_config[2629].items[4]={{480,120}, { {20139001,4}, {20139002,4} },{},{} }  

------------------------------------------------------------------------------------------------------
acr_static_config[2630]={}
acr_static_config[2630].serial 					=	-1			--活动流水id
acr_static_config[2630].switch_open				=	1			--开关
acr_static_config[2630].common					=	0			--通服标记
acr_static_config[2630].script_id				=	002630		--活动id
acr_static_config[2630].type_name				=	"永久礼包"	--活动名字
acr_static_config[2630].icon					=	"null"		--组icon
acr_static_config[2630].desc					=	"null"		--描述
acr_static_config[2630].show_begin_days			=	-1			--开服后多少天显示开始
acr_static_config[2630].show_end_days			=	-1			--开服后多少天显示结束
acr_static_config[2630].normal_begin_days		=	-1			--开服后多少天开始
acr_static_config[2630].normal_end_days			=	-1			--开服后多少天结束
acr_static_config[2630].show_begin_date			=	-1			--显示开始日期
acr_static_config[2630].show_end_date			=	-1			--显示结束日期
acr_static_config[2630].normal_begin_date		=	-1			--正式开始日期
acr_static_config[2630].normal_end_date			=	-1			--正式结束日期
acr_static_config[2630].show_begin_time			=	-1			--显示开始时间
acr_static_config[2630].show_end_time			=	-1			--显示结束时间
acr_static_config[2630].normal_begin_time		=	-1			--正式开始时间
acr_static_config[2630].normal_end_time			=	-1			--正式结束时间
acr_static_config[2630].items = {}--符石价格, 次数, {id,数量}
--acr_static_config[2630].items[1]= {{60,5}, {{20139001,1}, {20139002,1} },{},{} }
--acr_static_config[2630].items[2]= {{60,5}, {{20139001,2}, {20139002,2} },{},{} }
--acr_static_config[2630].items[3]= {{60,5}, {{20139001,3}, {20139002,3} },{},{} }
--acr_static_config[2630].items[4]= {{60,5}, {{20139001,2}, {20139002,2} },{},{} } 
--acr_static_config[2630].items[5]= {{60,5}, {{20139001,3}, {20139002,3} },{},{} }

------------------------------------------------------------------------------------------------------
acr_static_config[2631]={}
acr_static_config[2631].serial 					=	-1			--活动流水id
acr_static_config[2631].switch_open				=	1			--开关
acr_static_config[2631].common					=	0			--通服标记
acr_static_config[2631].script_id				=	002631		--活动id
acr_static_config[2631].type_name				=	"连续充值"	--活动名字
acr_static_config[2631].icon					=	"null"		--组icon
acr_static_config[2631].desc					=	"null"		--描述
acr_static_config[2631].show_begin_days			=	-1			--开服后多少天显示开始
acr_static_config[2631].show_end_days			=	-1			--开服后多少天显示结束
acr_static_config[2631].normal_begin_days		=	-1			--开服后多少天开始
acr_static_config[2631].normal_end_days			=	-1			--开服后多少天结束
acr_static_config[2631].show_begin_date			=	-1			--显示开始日期
acr_static_config[2631].show_end_date			=	-1			--显示结束日期
acr_static_config[2631].normal_begin_date		=	-1			--正式开始日期
acr_static_config[2631].normal_end_date			=	-1			--正式结束日期
acr_static_config[2631].show_begin_time			=	-1			--显示开始时间
acr_static_config[2631].show_end_time			=	-1			--显示结束时间
acr_static_config[2631].normal_begin_time		=	-1			--正式开始时间
acr_static_config[2631].normal_end_time			=	-1			--正式结束时间
acr_static_config[2631].items = {}--充值数,奖励道具{id,数量},额外奖励道具{id,数量}
--acr_static_config[2631].items[1]={{30},{{20139001,1}, {20139002,1} },{}, { }}
--acr_static_config[2631].items[2]={{30},{{20139001,1}, {20139002,1} },{}, { }}
--acr_static_config[2631].items[3]={{30},{{20139001,1}, {20139002,1} },{}, {{20139001,1}}}
--acr_static_config[2631].items[4]={{30},{{20139001,1}, {20139002,1} },{}, { }}
--acr_static_config[2631].items[5]={{30},{{20139001,1}, {20139002,1} },{}, {{20139001,1}}}
--acr_static_config[2631].items[6]={{30},{{20139001,1}, {20139002,1} },{}, { }}
--acr_static_config[2631].items[7]={{30},{{20139001,1}, {20139002,1} },{}, {{20139001,1}}}

------------------------------------------------------------------------------------------------------
acr_static_config[2632]={}
acr_static_config[2632].serial 					=	-1			--活动流水id
acr_static_config[2632].switch_open				=	1			--开关
acr_static_config[2632].common					=	0			--通服标记
acr_static_config[2632].script_id				=	002632		--活动id
acr_static_config[2632].type_name				=	"超值兑换"	--活动名字
acr_static_config[2632].icon					=	"null"		--组icon
acr_static_config[2632].desc					=	"null"		--描述
acr_static_config[2632].show_begin_days			=	-1			--开服后多少天显示开始
acr_static_config[2632].show_end_days			=	-1			--开服后多少天显示结束
acr_static_config[2632].normal_begin_days		=	-1			--开服后多少天开始
acr_static_config[2632].normal_end_days			=	-1			--开服后多少天结束
acr_static_config[2632].show_begin_date			=	-1			--显示开始日期
acr_static_config[2632].show_end_date			=	-1			--显示结束日期
acr_static_config[2632].normal_begin_date		=	-1			--正式开始日期
acr_static_config[2632].normal_end_date			=	-1			--正式结束日期
acr_static_config[2632].show_begin_time			=	-1			--显示开始时间
acr_static_config[2632].show_end_time			=	-1			--显示结束时间
acr_static_config[2632].normal_begin_time		=	-1			--正式开始时间
acr_static_config[2632].normal_end_time			=	-1			--正式结束时间
acr_static_config[2632].items = {}--兑换次数，兑换消耗道具{id,数量}，兑换获得道具{id,数量}
--acr_static_config[2632].items[1]={{5}, {{20139001,1}, {20139002,1} } ,{}, {{20139001,1}, {20139002,1} } }
--acr_static_config[2632].items[2]={{5}, {{20139001,2}, {20139002,2} } ,{}, {{20139001,2}, {20139002,2} } }
--acr_static_config[2632].items[3]={{5}, {{20139001,3}, {20139002,3} } ,{}, {{20139001,3}, {20139002,3} } }
--acr_static_config[2632].items[4]={{5}, {{20139001,2}, {20139002,2} } ,{}, {{20139001,2}, {20139002,2} } }
--acr_static_config[2632].items[5]={{5}, {{20139001,3}, {20139002,3} } ,{}, {{20139001,3}, {20139002,3} } }
--acr_static_config[2632].items[6]={{5}, {{20139001,1}, {20139002,1} } ,{}, {{20139001,1}, {20139002,1} } }
--acr_static_config[2632].items[7]={{5}, {{20139001,2}, {20139002,2} } ,{}, {{20139001,2}, {20139002,2} } }
--acr_static_config[2632].items[8]={{5}, {{20139001,3}, {20139002,3} } ,{}, {{20139001,3}, {20139002,3} } }
--acr_static_config[2632].items[9]={{5}, {{20139001,2}, {20139002,2} } ,{}, {{20139001,2}, {20139002,2} } }

------------------------------------------------------------------------------------------------------
acr_static_config[2633]={}
acr_static_config[2633].serial 					=	-1			--活动流水id
acr_static_config[2633].switch_open				=	1			--开关
acr_static_config[2633].common					=	0			--通服标记
acr_static_config[2633].script_id				=	002633		--活动id
acr_static_config[2633].type_name				=	"限时掉落"	--活动名字
acr_static_config[2633].icon					=	"null"		--组icon
acr_static_config[2633].desc					=	"null"		--描述
acr_static_config[2633].show_begin_days			=	-1			--开服后多少天显示开始
acr_static_config[2633].show_end_days			=	-1			--开服后多少天显示结束
acr_static_config[2633].normal_begin_days		=	-1			--开服后多少天开始
acr_static_config[2633].normal_end_days			=	-1			--开服后多少天结束
acr_static_config[2633].show_begin_date			=	-1			--显示开始日期
acr_static_config[2633].show_end_date			=	-1			--显示结束日期
acr_static_config[2633].normal_begin_date		=	-1			--正式开始日期
acr_static_config[2633].normal_end_date			=	-1			--正式结束日期
acr_static_config[2633].show_begin_time			=	-1			--显示开始时间
acr_static_config[2633].show_end_time			=	-1			--显示结束时间
acr_static_config[2633].normal_begin_time		=	-1			--正式开始时间
acr_static_config[2633].normal_end_time			=	-1			--正式结束时间
--掉落活动, 掉落道具{id, 数量, 掉落几率}
acr_static_config[2633].items = {}
--acr_static_config[2633].items[1]={{1}, {},{{20139001,1,100}, {20139002,1,100} } ,{} }
--acr_static_config[2633].items[2]={{2}, {},{{20139001,2,100}, {20139002,2,100} } ,{} }
--acr_static_config[2633].items[3]={{3}, {},{{20139001,3,100}, {20139002,3,100} } ,{} }
--acr_static_config[2633].items[4]={{4}, {},{{20139001,2,100}, {20139002,2,100} } ,{} }
--acr_static_config[2633].items[5]={{5}, {},{{20139001,3,100}, {20139002,3,100} } ,{} }
--acr_static_config[2633].items[6]={{6}, {},{{20139001,1,100}, {20139002,1,100} } ,{} }

------------------------------------------------------------------------------------------------------
acr_static_config[2634]={}
acr_static_config[2634].serial 					=	-1			--活动流水id
acr_static_config[2634].switch_open				=	1			--开关
acr_static_config[2634].common					=	0			--通服标记
acr_static_config[2634].script_id				=	002634		--活动id
acr_static_config[2634].type_name				=	"三选一"	--活动名字
acr_static_config[2634].icon					=	"null"		--组icon
acr_static_config[2634].desc					=	"null"		--描述
acr_static_config[2634].show_begin_days			=	-1			--开服后多少天显示开始
acr_static_config[2634].show_end_days			=	-1			--开服后多少天显示结束
acr_static_config[2634].normal_begin_days		=	-1			--开服后多少天开始
acr_static_config[2634].normal_end_days			=	-1			--开服后多少天结束
acr_static_config[2634].show_begin_date			=	-1			--显示开始日期
acr_static_config[2634].show_end_date			=	-1			--显示结束日期
acr_static_config[2634].normal_begin_date		=	-1			--正式开始日期
acr_static_config[2634].normal_end_date			=	-1			--正式结束日期
acr_static_config[2634].show_begin_time			=	-1			--显示开始时间
acr_static_config[2634].show_end_time			=	-1			--显示结束时间
acr_static_config[2634].normal_begin_time		=	-1			--正式开始时间
acr_static_config[2634].normal_end_time			=	-1			--正式结束时间
--商品id, 充值数, 3选一道具{id,数量}
acr_static_config[2634].items = {}
--acr_static_config[2634].items[1]={{6},{{20139001,1}, {20139002,1}, {20139002,1} },{},{}}

------------------------------------------------------------------------------------------------------
acr_static_config[2635]={}
acr_static_config[2635].serial 					=	-1			--活动流水id
acr_static_config[2635].switch_open				=	1			--开关
acr_static_config[2635].common					=	0			--通服标记
acr_static_config[2635].script_id				=	002635		--活动id
acr_static_config[2635].type_name				=	"战力比拼"	--活动名字
acr_static_config[2635].icon					=	"null"		--组icon
acr_static_config[2635].desc					=	"null"		--描述
acr_static_config[2635].show_begin_days			=	-1			--开服后多少天显示开始
acr_static_config[2635].show_end_days			=	-1			--开服后多少天显示结束
acr_static_config[2635].normal_begin_days		=	-1			--开服后多少天开始
acr_static_config[2635].normal_end_days			=	-1			--开服后多少天结束
acr_static_config[2635].show_begin_date			=	-1			--显示开始日期
acr_static_config[2635].show_end_date			=	-1			--显示结束日期
acr_static_config[2635].normal_begin_date		=	-1			--正式开始日期
acr_static_config[2635].normal_end_date			=	-1			--正式结束日期
acr_static_config[2635].show_begin_time			=	-1			--显示开始时间
acr_static_config[2635].show_end_time			=	-1			--显示结束时间
acr_static_config[2635].normal_begin_time		=	-1			--正式开始时间
acr_static_config[2635].normal_end_time			=	-1			--正式结束时间
--战力,人数，奖励道具{id,数量}
acr_static_config[2635].items = {}
--acr_static_config[2635].items[1]={	{30000, 3000}, 	{{20139001,1}, {20139002,1} },{},{} }
--acr_static_config[2635].items[2]={	{40000, 1500}, 	{{20139001,1}, {20139002,1} },{},{} }
--acr_static_config[2635].items[3]={	{50000, 500	}, 	{{20139001,1}, {20139002,1} },{},{} }
--acr_static_config[2635].items[4]={	{60000, 200	}, 	{{20139001,1}, {20139002,1} },{},{} } 
--acr_static_config[2635].items[5]={	{70000, 100	}, 	{{20139001,1}, {20139002,1} },{},{} }
--acr_static_config[2635].items[6]={	{80000, 50	}, 	{{20139001,1}, {20139002,1} },{},{} }
--acr_static_config[2635].items[7]={	{90000, 10	}, 	{{20139001,1}, {20139002,1} },{},{} }
                                                                              
------------------------------------------------------------------------------------------------------
acr_static_config[2636]={}
acr_static_config[2636].serial 					=	-1			--活动流水id
acr_static_config[2636].switch_open				=	1			--开关
acr_static_config[2636].common					=	0			--通服标记
acr_static_config[2636].script_id				=	002636		--活动id
acr_static_config[2636].type_name				=	"等级比拼"	--活动名字
acr_static_config[2636].icon					=	"null"		--组icon
acr_static_config[2636].desc					=	"null"		--描述
acr_static_config[2636].show_begin_days			=	-1			--开服后多少天显示开始
acr_static_config[2636].show_end_days			=	-1			--开服后多少天显示结束
acr_static_config[2636].normal_begin_days		=	-1			--开服后多少天开始
acr_static_config[2636].normal_end_days			=	-1			--开服后多少天结束
acr_static_config[2636].show_begin_date			=	-1			--显示开始日期
acr_static_config[2636].show_end_date			=	-1			--显示结束日期
acr_static_config[2636].normal_begin_date		=	-1			--正式开始日期
acr_static_config[2636].normal_end_date			=	-1			--正式结束日期
acr_static_config[2636].show_begin_time			=	-1			--显示开始时间
acr_static_config[2636].show_end_time			=	-1			--显示结束时间
acr_static_config[2636].normal_begin_time		=	-1			--正式开始时间
acr_static_config[2636].normal_end_time			=	-1			--正式结束时间
--等级,人数，奖励道具{id,数量}
acr_static_config[2636].items = {}
--acr_static_config[2636].items[1]={{10, 3000}, {{20139001,1}, {20139002,1} },{},{} }
--acr_static_config[2636].items[2]={{20, 1500}, {{20139001,1}, {20139002,1} },{},{} }
--acr_static_config[2636].items[3]={{30, 500 }, {{20139001,1}, {20139002,1} },{},{} }
--acr_static_config[2636].items[4]={{40, 200 }, {{20139001,1}, {20139002,1} },{},{} } 
--acr_static_config[2636].items[5]={{50, 100 }, {{20139001,1}, {20139002,1} },{},{} }
--acr_static_config[2636].items[6]={{55, 50	}, 	{{20139001,1}, {20139002,1} },{},{} }
--acr_static_config[2636].items[7]={{60, 10	}, 	{{20139001,1}, {20139002,1} },{},{} }
                                                              
------------------------------------------------------------------------------------------------------
acr_static_config[2637]={}
acr_static_config[2637].serial 					=	-1			--活动流水id
acr_static_config[2637].switch_open				=	1			--开关
acr_static_config[2637].common					=	0			--通服标记
acr_static_config[2637].script_id				=	002637		--活动id
acr_static_config[2637].type_name				=	"团购"		--活动名字
acr_static_config[2637].icon					=	"null"		--组icon
acr_static_config[2637].desc					=	"null"		--描述
acr_static_config[2637].show_begin_days			=	-1			--开服后多少天显示开始
acr_static_config[2637].show_end_days			=	-1			--开服后多少天显示结束
acr_static_config[2637].normal_begin_days		=	-1			--开服后多少天开始
acr_static_config[2637].normal_end_days			=	-1			--开服后多少天结束
acr_static_config[2637].show_begin_date			=	-1			--显示开始日期
acr_static_config[2637].show_end_date			=	-1			--显示结束日期
acr_static_config[2637].normal_begin_date		=	-1			--正式开始日期
acr_static_config[2637].normal_end_date			=	-1			--正式结束日期
acr_static_config[2637].show_begin_time			=	-1			--显示开始时间
acr_static_config[2637].show_end_time			=	-1			--显示结束时间
acr_static_config[2637].normal_begin_time		=	-1			--正式开始时间
acr_static_config[2637].normal_end_time			=	-1			--正式结束时间
--价格,原价,人数，奖励道具{id,数量}
acr_static_config[2637].items = {}
--acr_static_config[2637].items[1]={{30000, 30000, 3000 }, 	{{20139001,1}, {20139002,1} },{},{} }
--acr_static_config[2637].items[2]={{40000, 40000, 1500 }, 	{{20139001,1}, {20139002,1} },{},{} }
--acr_static_config[2637].items[3]={{50000, 50000, 500	}, 	{{20139001,1}, {20139002,1} },{},{} }
--acr_static_config[2637].items[4]={{60000, 60000, 200	}, 	{{20139001,1}, {20139002,1} },{},{} } 
--acr_static_config[2637].items[5]={{70000, 70000, 100	}, 	{{20139001,1}, {20139002,1} },{},{} }
--acr_static_config[2637].items[6]={{80000, 80000, 50	}, 	{{20139001,1}, {20139002,1} },{},{} }
--acr_static_config[2637].items[7]={{90000, 90000, 10	}, 	{{20139001,1}, {20139002,1} },{},{} }
                                                                        
------------------------------------------------------------------------------------------------------
acr_static_config[2638]={}
acr_static_config[2638].serial 					=	-1			--活动流水id
acr_static_config[2638].switch_open				=	1			--开关
acr_static_config[2638].common					=	0			--通服标记
acr_static_config[2638].script_id				=	002638		--活动id
acr_static_config[2638].type_name				=	"秒杀"		--活动名字
acr_static_config[2638].icon					=	"null"		--组icon
acr_static_config[2638].desc					=	"null"		--描述
acr_static_config[2638].show_begin_days			=	-1			--开服后多少天显示开始
acr_static_config[2638].show_end_days			=	-1			--开服后多少天显示结束
acr_static_config[2638].normal_begin_days		=	-1			--开服后多少天开始
acr_static_config[2638].normal_end_days			=	-1			--开服后多少天结束
acr_static_config[2638].show_begin_date			=	-1			--显示开始日期
acr_static_config[2638].show_end_date			=	-1			--显示结束日期
acr_static_config[2638].normal_begin_date		=	-1			--正式开始日期
acr_static_config[2638].normal_end_date			=	-1			--正式结束日期
acr_static_config[2638].show_begin_time			=	-1			--显示开始时间
acr_static_config[2638].show_end_time			=	-1			--显示结束时间
acr_static_config[2638].normal_begin_time		=	-1			--正式开始时间
acr_static_config[2638].normal_end_time			=	-1			--正式结束时间
--价格,剩余次数，倒计时，奖励道具{id,数量}
acr_static_config[2638].items = {}
--acr_static_config[2638].items[1]={	{300, 5, 1800	}, 		{{20139001,1}, {20139002,1} },{},{} }
--acr_static_config[2638].items[2]={	{400, 5, 1800	}, 		{{20139001,1}, {20139002,1} },{},{} }
--acr_static_config[2638].items[3]={	{500, 5, 1800	}, 		{{20139001,1}, {20139002,1} },{},{} }
--acr_static_config[2638].items[4]={	{600, 5, 1800	}, 		{{20139001,1}, {20139002,1} },{},{} } 
--acr_static_config[2638].items[5]={	{700, 5, 1800	}, 		{{20139001,1}, {20139002,1} },{},{} }
--acr_static_config[2638].items[6]={	{800, 5, 1800	}, 		{{20139001,1}, {20139002,1} },{},{} }
--acr_static_config[2638].items[7]={	{900, 5, 1800	}, 		{{20139001,1}, {20139002,1} },{},{} }
----------------------------------------------------------------------------------------
acr_static_config[2639]={}
acr_static_config[2639].serial 					=	-1			--活动流水id
acr_static_config[2639].switch_open				=	1			--开关
acr_static_config[2639].common					=	0			--通服标记
acr_static_config[2639].script_id				=	002639		--活动id
acr_static_config[2639].type_name				=	"超值礼包2"	--活动名字
acr_static_config[2639].icon					=	"null"		--组icon
acr_static_config[2639].desc					=	"null"		--描述
acr_static_config[2639].show_begin_days			=	-1			--开服后多少天显示开始
acr_static_config[2639].show_end_days			=	-1			--开服后多少天显示结束
acr_static_config[2639].normal_begin_days		=	-1			--开服后多少天开始
acr_static_config[2639].normal_end_days			=	-1			--开服后多少天结束
acr_static_config[2639].show_begin_date			=	-1			--显示开始日期
acr_static_config[2639].show_end_date			=	-1			--显示结束日期
acr_static_config[2639].normal_begin_date		=	-1			--正式开始日期
acr_static_config[2639].normal_end_date			=	-1			--正式结束日期
acr_static_config[2639].show_begin_time			=	-1			--显示开始时间
acr_static_config[2639].show_end_time			=	-1			--显示结束时间
acr_static_config[2639].normal_begin_time		=	-1			--正式开始时间
acr_static_config[2639].normal_end_time			=	-1			--正式结束时间
acr_static_config[2639].items = {}--价格, 原价, {id,数量}
--acr_static_config[2639].items[1]={{60 ,120}, { {20139001,1}, {20139002,1} } ,{},{} }
--acr_static_config[2639].items[2]={{120,120}, { {20139001,2}, {20139002,2} } ,{},{} }
--acr_static_config[2639].items[3]={{240,120}, { {20139001,3}, {20139002,3} } ,{},{} }
--acr_static_config[2639].items[4]={{480,120}, { {20139001,4}, {20139002,4} } ,{},{} } 


------------------------------------------------------------------------------------------------------
acr_static_config[2640]={}
acr_static_config[2640].serial 					=	-1			--活动流水id
acr_static_config[2640].switch_open				=	1			--开关
acr_static_config[2640].common					=	0			--通服标记
acr_static_config[2640].script_id				=	002640		--活动id
acr_static_config[2640].type_name				=	"超值礼包3"	--活动名字
acr_static_config[2640].icon					=	"null"		--组icon
acr_static_config[2640].desc					=	"null"		--描述
acr_static_config[2640].show_begin_days			=	-1			--开服后多少天显示开始
acr_static_config[2640].show_end_days			=	-1			--开服后多少天显示结束
acr_static_config[2640].normal_begin_days		=	-1			--开服后多少天开始
acr_static_config[2640].normal_end_days			=	-1			--开服后多少天结束
acr_static_config[2640].show_begin_date			=	-1			--显示开始日期
acr_static_config[2640].show_end_date			=	-1			--显示结束日期
acr_static_config[2640].normal_begin_date		=	-1			--正式开始日期
acr_static_config[2640].normal_end_date			=	-1			--正式结束日期
acr_static_config[2640].show_begin_time			=	-1			--显示开始时间
acr_static_config[2640].show_end_time			=	-1			--显示结束时间
acr_static_config[2640].normal_begin_time		=	-1			--正式开始时间
acr_static_config[2640].normal_end_time			=	-1			--正式结束时间
acr_static_config[2640].items = {}--价格, 原价, {id,数量}
--acr_static_config[2640].items[1]={{60 ,120}, { {20139001,1}, {20139002,1} } ,{},{} }
--acr_static_config[2640].items[2]={{120,120}, { {20139001,2}, {20139002,2} } ,{},{} }
--acr_static_config[2640].items[3]={{240,120}, { {20139001,3}, {20139002,3} } ,{},{} }
--acr_static_config[2640].items[4]={{480,120}, { {20139001,4}, {20139002,4} } ,{},{} } 

------------------------------------------------------------------------------------------------------
acr_static_config[2641]={}
acr_static_config[2641].serial 					=	-1			--活动流水id
acr_static_config[2641].switch_open				=	1			--开关
acr_static_config[2641].common					=	0			--通服标记
acr_static_config[2641].script_id				=	002641		--活动id
acr_static_config[2641].type_name				=	"超值礼包4"	--活动名字
acr_static_config[2641].icon					=	"null"		--组icon
acr_static_config[2641].desc					=	"null"		--描述
acr_static_config[2641].show_begin_days			=	-1			--开服后多少天显示开始
acr_static_config[2641].show_end_days			=	-1			--开服后多少天显示结束
acr_static_config[2641].normal_begin_days		=	-1			--开服后多少天开始
acr_static_config[2641].normal_end_days			=	-1			--开服后多少天结束
acr_static_config[2641].show_begin_date			=	-1			--显示开始日期
acr_static_config[2641].show_end_date			=	-1			--显示结束日期
acr_static_config[2641].normal_begin_date		=	-1			--正式开始日期
acr_static_config[2641].normal_end_date			=	-1			--正式结束日期
acr_static_config[2641].show_begin_time			=	-1			--显示开始时间
acr_static_config[2641].show_end_time			=	-1			--显示结束时间
acr_static_config[2641].normal_begin_time		=	-1			--正式开始时间
acr_static_config[2641].normal_end_time			=	-1			--正式结束时间
acr_static_config[2641].items = {}--价格, 原价, {id,数量}
--acr_static_config[2641].items[1]={{60 ,120}, { {20139001,1}, {20139002,1} } ,{},{} }
--acr_static_config[2641].items[2]={{120,120}, { {20139001,2}, {20139002,2} } ,{},{} }
--acr_static_config[2641].items[3]={{240,120}, { {20139001,3}, {20139002,3} } ,{},{} }
--acr_static_config[2641].items[4]={{480,120}, { {20139001,4}, {20139002,4} } ,{},{} } 

------------------------------------------------------------------------------------------------------
acr_static_config[2642]={}
acr_static_config[2642].serial 					=	-1			--活动流水id
acr_static_config[2642].switch_open				=	1			--开关
acr_static_config[2642].common					=	0			--通服标记
acr_static_config[2642].script_id				=	002642		--活动id
acr_static_config[2642].type_name				=	"超值礼包5"	--活动名字
acr_static_config[2642].icon					=	"null"		--组icon
acr_static_config[2642].desc					=	"null"		--描述
acr_static_config[2642].show_begin_days			=	-1			--开服后多少天显示开始
acr_static_config[2642].show_end_days			=	-1			--开服后多少天显示结束
acr_static_config[2642].normal_begin_days		=	-1			--开服后多少天开始
acr_static_config[2642].normal_end_days			=	-1			--开服后多少天结束
acr_static_config[2642].show_begin_date			=	-1			--显示开始日期
acr_static_config[2642].show_end_date			=	-1			--显示结束日期
acr_static_config[2642].normal_begin_date		=	-1			--正式开始日期
acr_static_config[2642].normal_end_date			=	-1			--正式结束日期
acr_static_config[2642].show_begin_time			=	-1			--显示开始时间
acr_static_config[2642].show_end_time			=	-1			--显示结束时间
acr_static_config[2642].normal_begin_time		=	-1			--正式开始时间
acr_static_config[2642].normal_end_time			=	-1			--正式结束时间
acr_static_config[2642].items = {}--价格, 原价, {id,数量}
--acr_static_config[2642].items[1]={{60 ,120}, { {20139001,1}, {20139002,1} } ,{},{} }
--acr_static_config[2642].items[2]={{120,120}, { {20139001,2}, {20139002,2} } ,{},{} }
--acr_static_config[2642].items[3]={{240,120}, { {20139001,3}, {20139002,3} } ,{},{} }
--acr_static_config[2642].items[4]={{480,120}, { {20139001,4}, {20139002,4} } ,{},{} } 

------------------------------------------------------------------------------------------------------
acr_static_config[2643]={}
acr_static_config[2643].serial 					=	-1			--活动流水id
acr_static_config[2643].switch_open				=	1			--开关
acr_static_config[2643].common					=	0			--通服标记
acr_static_config[2643].script_id				=	002643		--活动id
acr_static_config[2643].type_name				=	"惊喜礼包"	--活动名字
acr_static_config[2643].icon					=	"null"		--组icon
acr_static_config[2643].desc					=	"null"		--描述
acr_static_config[2643].show_begin_days			=	-1			--开服后多少天显示开始
acr_static_config[2643].show_end_days			=	-1			--开服后多少天显示结束
acr_static_config[2643].normal_begin_days		=	-1			--开服后多少天开始
acr_static_config[2643].normal_end_days			=	-1			--开服后多少天结束
acr_static_config[2643].show_begin_date			=	-1			--显示开始日期
acr_static_config[2643].show_end_date			=	-1			--显示结束日期
acr_static_config[2643].normal_begin_date		=	-1			--正式开始日期
acr_static_config[2643].normal_end_date			=	-1			--正式结束日期
acr_static_config[2643].show_begin_time			=	-1			--显示开始时间
acr_static_config[2643].show_end_time			=	-1			--显示结束时间
acr_static_config[2643].normal_begin_time		=	-1			--正式开始时间
acr_static_config[2643].normal_end_time			=	-1			--正式结束时间
acr_static_config[2643].items = {}--类型, 价格,次数, {id,数量}
--acr_static_config[2643].items[1]={{1,120,5}, { {20139001,1}, {20139002,1} },{},{} }
--acr_static_config[2643].items[2]={{1,120,5}, { {20139001,2}, {20139002,2} },{},{} }
--acr_static_config[2643].items[3]={{2,120,5}, { {20139001,3}, {20139002,3} },{},{} }
--acr_static_config[2643].items[4]={{2,120,5}, { {20139001,4}, {20139002,4} },{},{} } 
--------------------------------------------------------------------------------------------------------
return acr_static_config
