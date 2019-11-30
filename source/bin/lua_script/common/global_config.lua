
--require("json")
g_log_level={}
g_log_level.LL_TRACE =0
g_log_level.LL_DEBUG =1
g_log_level.LL_INFO  =2
g_log_level.LL_WARN  =3
g_log_level.LL_ERROR =4
g_log_level.LL_FATAL =5
g_log_level.LL_BI    =6
g_log_level.LL_DB    =7
		
--领土争夺
g_DECLARE_WAR_WAIT_TIME = 30
g_FIGHTING_TIME_OUT = 60 * 10
g_PROTECT_TIME = 60 * 10
--scene date 数据索引，不超过64个int
g_scene_data={}
g_scene_data.last_date=0		
g_scene_data.skyfall_flag=1
g_scene_data.burning_flag=2
g_scene_data.worldboss_flag=3
g_scene_data.guildboss_flag=4
g_scene_data.worldboss_notify=5
--无效GUID
INVALID_GUID=0
INVALID_INT32=-1
-------------------------------------
--天灾军团配置表
--
--沼剃刀泽
--血色修道院
--剃刀高地
--奥达曼
--祖尔法拉克
--玛拉顿
--沉没的神庙
--黑石深渊
--厄运之槌
--斯坦索姆
--黑石塔下层
--通灵学院
--黑石塔上层
--

--地图，位置组，怪物组,每次刷出数量,等级
g_skyfall_rule={
{7, 656,12201,15,20},
{8, 657,12201,15,25},
{9, 658,12211,15,30},
{10,659,12211,15,35},
{11,660,12221,12,40},
{12,661,12221,12,45},
{13,662,12221,12,47},
{14,663,12231,9 ,50},
{15,664,12231,9 ,52},
{16,665,12231,9 ,55},
{17,666,12231,9 ,57},
{18,667,12231,9 ,58},
{19,668,12231,9 ,60},
}
--注意，目前位置组和场景id对应有问题！！！
-----------------------------------------------------

--三环，暮光完成环数特殊奖励
--id，数量，权重
g_special_reward={
{-1		 ,0,5000},  --无	
{20236004,1,1200},  --训练药剂	
{20121001,1,1800},  --初级经验药水
{20121003,1,200	},  --强效经验药水
{30101001,1,200	},  --1级红宝石	
{30102001,1,200	},  --1级虎眼石	
{30103001,1,200	},  --1级猫眼石	
{30104001,1,200	},  --1级月亮石	
{30105001,1,200	},  --1级翡翠石	
{30106001,1,200	},  --1级绿玛瑙	
{30107001,1,200	},  --1级暗影石	
{30108001,1,200	},  --1级孔雀石	
{30109001,1,200	},  --1级黄水晶	
}
---------------------------

--燃烧军团刷怪规则
--地图，位置组，怪物组,每次刷出数量
g_burnning_union_rule={
{3,3,12301,5},
{2,4,12301,5},
{4,5,12301,5},
}

g_world_boss={
{day=0,npcid=14107,monstergourp=3961},
{day=1,npcid=14101,monstergourp=3901},
{day=2,npcid=14102,monstergourp=3911},
{day=3,npcid=14103,monstergourp=3921},
{day=4,npcid=14104,monstergourp=3931},
{day=5,npcid=14105,monstergourp=3941},
{day=6,npcid=14106,monstergourp=3951},
}

g_guild_boss_npcid = 14001
g_guild_boss_monstergourp = 6001
g_guild_battlelist_week = {1,3}
g_guild_battle_begin = 2030
g_guild_battle_prepare = 2100
g_guild_battle_stop = 2110
g_guild_battle_end = 2130

g_max_guild_id = 999999999
--raid timer定义
g_raid_timer={}
g_raid_timer.common_destroy=999



