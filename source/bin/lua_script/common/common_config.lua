--------------------------------------enum
MissionCellIndex ={}							--对应AllPacketEnum.proto里的MissionCellIndex枚举
MissionCellIndex.MainLast=0						--主线id
MissionCellIndex.Part1Last=1                   	--支线1id
MissionCellIndex.Part2Last=2                   	--支线2id
MissionCellIndex.Part3Last=3                   	--支线3id	

MissionCellIndex.Reward_Circle=4               	--赏金主id
MissionCellIndex.ExtraMission_Id=5              --赏金附加id

MissionCellIndex.Archaeology_LastDrop=6         --考古任务上次掉落

MissionCellIndex.Corps_Circle=7					--暮光军团环数

MissionCellIndex.CarbonGoblin_MaxMissionId=8	--地精矿洞重置周期内的最高任务进度
MissionCellIndex.CarbonGoblin_Last=9			--地精矿洞当前任务进度

MissionCellIndex.CarbonBlack_MaxMissionId=10	--黑色沼泽重置周期内的最高任务进度
MissionCellIndex.CarbonBlack_Last=11			--黑色沼泽当前任务进度

MissionCellIndex.CarbonBlood_MaxMissionId=12	--血色城堡重置周期内的最高任务进度
MissionCellIndex.CarbonBlood_Last=13			--血色城堡当前任务进度

MissionCellIndex.CarbonBigThree_process1=14		--大三环重置周期内的最高任务进度
MissionCellIndex.CarbonBigThree_process2=15		--大三环重置周期内的最高任务进度
MissionCellIndex.CarbonBigThree_process3=16		--大三环重置周期内的最高任务进度
MissionCellIndex.CarbonBigThree_SelfLast=17		--大三环当前环数，不受clone影响

MissionCellIndex.Guild_Circleid=18				--帮会任务
MissionCellIndex.LevelMissionId=19				--等级任务

--7个团队本

MissionCellIndex.Carbon2001_Process=20			--团队本重置周期内的最高任务进度
MissionCellIndex.Carbon2001_Last=21				--团队本当前任务进度

MissionCellIndex.Carbon2002_Process=22			--团队本重置周期内的最高任务进度
MissionCellIndex.Carbon2002_Last=23				--团队本当前任务进度

MissionCellIndex.Carbon2003_Process=24			--团队本重置周期内的最高任务进度
MissionCellIndex.Carbon2003_Last=25				--团队本当前任务进度

MissionCellIndex.Carbon2004_Process=26			--团队本重置周期内的最高任务进度
MissionCellIndex.Carbon2004_Last=27				--团队本当前任务进度

MissionCellIndex.Carbon2005_Process=28			--团队本重置周期内的最高任务进度
MissionCellIndex.Carbon2005_Last=29				--团队本当前任务进度

MissionCellIndex.Carbon2006_Process=30			--团队本重置周期内的最高任务进度
MissionCellIndex.Carbon2006_Last=31				--团队本当前任务进度

MissionCellIndex.Carbon2007_Process=32			--团队本重置周期内的最高任务进度
MissionCellIndex.Carbon2007_Last=33				--团队本当前任务进度

MissionCellIndex.CarbonTinyThree_process1=34	--小三环重置周期内的最高任务进度
MissionCellIndex.CarbonTinyThree_process2=35	--小三环重置周期内的最高任务进度
MissionCellIndex.CarbonTinyThree_process3=36	--小三环重置周期内的最高任务进度
MissionCellIndex.CarbonTinyThree_SelfLast=37	--小三环当前环数，不受clone影响

MissionCellIndex.CommonCarbonMission_Process=38	--主线团队本重置周期内的最高任务进度
MissionCellIndex.CommonCarbonMission_Last=39	--主线团队本当前任务进度


MissionCellIndex.Tower_Process=40				--通天塔重置周期内的最高任务进度
MissionCellIndex.Tower_Last=41					--通天塔当前任务进度
MissionCellIndex.Tower_Time=42					--通天塔累计时间

---------------------------------------------data------------------------------------
--奖励活动脚本
g_reward_activity_script={
2601,--每日签到
2602,--大富翁
2603,--首充礼包
2605,--等级冲刺	
2606,--战力冲刺
2607,--英雄收集
2608,--每日登陆
2609,--关卡冲刺
2611,--7日狂欢
2612,--终生卡福利
2613,--月卡福利
2614,--全民福利	
2615,--开服基金
2616,--累计充值[通服]
2617,--累计消费[通服]
2618,--单日充值
2619,--首充双倍
2620,--终身充值
2621,--终身消费
2622,--限时抽卡
2623,--限时礼包[非rmb]
2624,--特惠礼包
2625,--套餐礼包
2626,--周礼包
2627,--月礼包
2628,--限时礼包[rmb]
2629,--超值礼包1
2630,--永久礼包
2631,--连续充值
2632,--超值兑换
2633,--限时掉落
2634,--三选一
2635,--战力比拼
2636,--等级比拼
2637,--团购
2638,--秒杀
2639,--超值礼包2
2640,--超值礼包3
2641,--超值礼包4
2642,--超值礼包5
2643,--惊喜礼包
}


--function表里会有配置，客户端某些脚本也会用到，对应NumberCell64Index_GuideFirstFlag标记
g_GuideFirstFlag={}
g_GuideFirstFlag.EnchantEquip					=0		--首次附魔
g_GuideFirstFlag.Mission20207					=1		--16级支线任务[挂机任务解锁]
g_GuideFirstFlag.Mission20257					=2		--16级支线任务[挂机任务解锁]
g_GuideFirstFlag.GoblinCarbon					=3		--首次哥布林
g_GuideFirstFlag.RushBoss						=4		--首次扫荡
g_GuideFirstFlag.NiceEquip						=5		--首次掉紫装
g_GuideFirstFlag.GradeUpMission					=6		--首次进阶任务
g_GuideFirstFlag.PreMission1					=7		--特写新手任务1
g_GuideFirstFlag.PreMission2					=8		--特写新手任务2
g_GuideFirstFlag.PreMission3					=9		--特写新手任务3


--行为统计数据索引
g_action_data_index={}					  		--对应AllPacketEnum.proto里的ActionDataIndex枚举
g_action_data_index.login_count					=	1 	----登陆奖励
g_action_data_index.level_count					=	2 	----升到X级
g_action_data_index.power_count					=	3 	----战力达到XXX
g_action_data_index.stage_count					= 	4	----通关章节
g_action_data_index.low_draw_count				= 	5	----累计进行X次初级招募
g_action_data_index.high_draw_count				= 	6	----累计进行X次高级招募
g_action_data_index.stage_boss_count			= 	7	----累计击败X次关卡BOSS
g_action_data_index.magic_count					= 	8	----累计附魔X次
g_action_data_index.treasure_count				= 	9	----累计使用X次遗迹藏宝图
g_action_data_index.reward_mission_count		= 	10	----累计完成X次赏金任务
g_action_data_index.corps_mission_count			= 	11	----累计完成X次暮光军团
g_action_data_index.goblin_count				= 	12	----累计完成X次地精矿洞
g_action_data_index.arena_count					= 	13	----累计进行X次竞技场
g_action_data_index.hell_stage_count			= 	14	----击败试炼场第X关（1-15）
g_action_data_index.skyfall_count				= 	15	----累计击败X个天灾军团
g_action_data_index.tinythree_count				= 	16	----累计完成X次小三环
g_action_data_index.blood_count					= 	17	----累计完成X次提尔之手
g_action_data_index.world_boss_count			= 	18	----击败世界BOSS X次
g_action_data_index.union_boss_count			= 	19	----累计进行公会BOSS战斗X次
g_action_data_index.embed1_count				= 	20	----累计镶嵌至少X颗1级宝石
g_action_data_index.embed2_count				= 	21	----累计镶嵌至少X颗2级宝石
g_action_data_index.embed3_count				= 	22	----累计镶嵌至少X颗3级宝石
g_action_data_index.skill_level5_count			= 	23	----任意英雄X个技能升级5级
g_action_data_index.skill_level10_count			= 	24	----任意英雄X个技能升级10级
g_action_data_index.skill_level15_count			= 	25	----任意英雄X个技能升级15级
g_action_data_index.skill_level20_count			= 	26	----任意英雄X个技能升级20级
g_action_data_index.skill_level25_count			= 	27	----任意英雄X个技能升级25级
g_action_data_index.skill_level30_count			= 	28	----任意英雄X个技能升级30级
g_action_data_index.skill_level35_count			= 	29	----任意英雄X个技能升级35级
g_action_data_index.skill_level40_count			= 	30	----任意英雄X个技能升级40级
g_action_data_index.color2_count				= 	31	----X个英雄进阶到X品质1
g_action_data_index.color3_count				= 	32	----X个英雄进阶到X品质2
g_action_data_index.color4_count				= 	33	----X个英雄进阶到X品质3
g_action_data_index.color5_count				= 	34	----X个英雄进阶到X品质4
g_action_data_index.invest_count				= 	35	----X个购买基金人数
g_action_data_index.all_charge_count			= 	36	----累计充值
g_action_data_index.all_consume_count			= 	37	----累计消费钻石
g_action_data_index.union_mission_count			= 	38	----工会任务次数
g_action_data_index.day_charge_count			= 	39 	----单日充值计数 
g_action_data_index.date_charge_count			= 	40	----周期累计充值
g_action_data_index.date_consume_count			= 	41	----周期累计消费钻石
g_action_data_index.charge_times				=	42	----充值次数
g_action_data_index.day_online_time				=	43	----日累计在线时长
g_action_data_index.nice_equip_value			=	44	----首件紫装掉落值

--找回次数
g_action_data_index.miss_big3_times				=	45
g_action_data_index.miss_tiny3_times			=	46
g_action_data_index.miss_reward_times			=	47
g_action_data_index.miss_corps_times			=	48
g_action_data_index.miss_daykill_times			=	49
g_action_data_index.miss_skyfall_times			=	50
g_action_data_index.miss_black_times			=	51
g_action_data_index.miss_goblin_times			=	52
g_action_data_index.miss_blood_times			=	53
g_action_data_index.miss_ancho_times			=	54
--经验
g_action_data_index.miss_big3_exp			=	55
g_action_data_index.miss_tiny3_exp			=	56
g_action_data_index.miss_reward_exp			=	57
g_action_data_index.miss_corps_exp			=	58
g_action_data_index.miss_daykill_exp		=	59
g_action_data_index.miss_skyfall_exp		=	60
g_action_data_index.miss_black_exp			=	61
g_action_data_index.miss_blood_exp			=	62
--金币
g_action_data_index.miss_big3_gold			=	63
g_action_data_index.miss_tiny3_gold			=	64
g_action_data_index.miss_reward_gold		=	65
g_action_data_index.miss_corps_gold			=	66
g_action_data_index.miss_goblin_gold		=	67
g_action_data_index.miss_skyfall_gold		=	68
g_action_data_index.miss_black_gold			=	69

--
g_action_data_index.quik_buy_count			= 	70	----秒杀人数
g_action_data_index.all_buy_count			= 	71	----团购人数
g_action_data_index.past_days   			= 	72	----开服天数

--上次重置时间
g_reset_time_index={}					  		--对应AllPacketEnum.proto里的ResetTimeIndex枚举
g_reset_time_index.acr_2601_last_id				=	1
g_reset_time_index.acr_2602_last_id				=	2
g_reset_time_index.acr_2603_last_id				=	3
g_reset_time_index.acr_2604_last_id				=	4
g_reset_time_index.acr_2605_last_id				=	5
g_reset_time_index.acr_2606_last_id				=	6
g_reset_time_index.acr_2607_last_id				=   7
g_reset_time_index.acr_2608_last_id				=   8
g_reset_time_index.acr_2609_last_id				=   9
g_reset_time_index.acr_2610_last_id				=   10
g_reset_time_index.acr_2611_last_id				=   11
g_reset_time_index.acr_2612_last_id				=   12
g_reset_time_index.acr_2613_last_id				=   13
g_reset_time_index.acr_2614_last_id				=   14
g_reset_time_index.acr_2615_last_id				=   15
g_reset_time_index.acr_2616_last_id				=   16
g_reset_time_index.acr_2617_last_id				=   17
g_reset_time_index.acr_2618_last_id				=   18
g_reset_time_index.acr_2619_last_id				=   19
g_reset_time_index.acr_2620_last_id				=   20
g_reset_time_index.acr_2621_last_id				=   21
g_reset_time_index.acr_2622_last_id				=   22
g_reset_time_index.acr_2623_last_id				=   23
g_reset_time_index.acr_2624_last_id				=   24
g_reset_time_index.acr_2625_last_id				=   25
g_reset_time_index.acr_2626_last_id				=   26
g_reset_time_index.acr_2627_last_id				=   27
g_reset_time_index.acr_2628_last_id				=   28
g_reset_time_index.acr_2629_last_id				=   29
g_reset_time_index.acr_2630_last_id				=   30
g_reset_time_index.acr_2631_last_id				=   31
g_reset_time_index.acr_2632_last_id				=   32
g_reset_time_index.acr_2633_last_id				=   33
g_reset_time_index.acr_2634_last_id				=   34
g_reset_time_index.acr_2635_last_id				=   35
g_reset_time_index.acr_2636_last_id				=   36
g_reset_time_index.acr_2637_last_id				=   37
g_reset_time_index.acr_2638_last_id				=   38
g_reset_time_index.acr_2639_last_id				=   39
g_reset_time_index.acr_2640_last_id				=   40
g_reset_time_index.acr_2641_last_id				=   41
g_reset_time_index.acr_2642_last_id				=   42
g_reset_time_index.acr_2643_last_id				=   43
	

----活动类型，对应activity.xlxs表列
ActivityType={}
ActivityType.InvalidActivityType=-1	--无效值
ActivityType.GoldReward =1			--赏金任务	
ActivityType.CorpsFight=2			--暮光军团
ActivityType.Ancholage =3			--考古
ActivityType.Arena=4				--竞技场
ActivityType.Hell=5					--试炼场
ActivityType.GoblinCarbon =6		--哥布林
ActivityType.Tranthform=7			--漕运
ActivityType.TinyThreeCarbon =8		--小三环
ActivityType.BigThreeCarbon =9		--大三环
ActivityType.BurningFight =10		--燃烧军团
ActivityType.SkyFallFight =11		--天灾军团
ActivityType.BloodCarbon =12		--血色城堡		
ActivityType.BlackCarbon =13		--黑色沼泽	
ActivityType.WolrdBossCarbon1 =14	--黑龙巢穴
ActivityType.WolrdBossCarbon2 =15	--时光之穴	
ActivityType.WolrdBossCarbon3 =16	--晶红圣殿
ActivityType.WolrdBossCarbon4 =17	--大地神殿
ActivityType.WolrdBossCarbon5 =18	--永恒之眼
ActivityType.WolrdBossCarbon6 =19	--翡翠梦境
ActivityType.WolrdBossCarbon7 =20	--龙眠神殿	
ActivityType.BattleCarbon1=21		--奥山战场
ActivityType.BattleCarbon2=22		--战歌峡谷	
ActivityType.GuildWetCopy=23		--战歌峡谷
ActivityType.DayKill=24				--安邦任务
ActivityType.UnionBattle=25			--工会战
ActivityType.UnionBoss=26			--工会竞赛
ActivityType.TextAnswer=27			--文字答题
ActivityType.PicAnswer=28			--看图答题
ActivityType.AcUnionMission=29		--工会任务
ActivityType.TeamCarbon2001=30		--祖尔格拉布
ActivityType.TeamCarbon2002=31		--熔火之心
ActivityType.TeamCarbon2003=32		--黑翼之巢
ActivityType.TeamCarbon2004=33		--黑龙巢穴
ActivityType.TeamCarbon2005=34		--安其拉废墟
ActivityType.TeamCarbon2006=35		--安其拉神殿
ActivityType.TeamCarbon2007=36		--纳克萨玛斯
ActivityType.UnionDonate=37			--公会捐赠
ActivityType.Tower=38				--通天塔


ActivityDataIndex ={}							--对应AllPacketEnum.proto里的ActivityDataIndex枚举
ActivityDataIndex.RewardDayTimes 		=0		--赏金任务	
ActivityDataIndex.CorpsFightDayTimes	=1		--暮光军团
ActivityDataIndex.AncholageDayTimes 	=2		--考古
ActivityDataIndex.ArenaTimes			=3		--竞技场
ActivityDataIndex.HellTimes				=4		--试炼场	
ActivityDataIndex.GoblinDayTimes 		=5      --哥布林	
ActivityDataIndex.TranshformDayTimes 	=6     	--漕运	
ActivityDataIndex.TinyThreeDayTimes 	=7     	--小三环
ActivityDataIndex.BigThreeDayTimes 		=8    	--大三环
ActivityDataIndex.BurningDayTimes 		=9		--燃烧军团
ActivityDataIndex.SkyFallDayTimes 		=10		--天灾军团
ActivityDataIndex.BloodDayTimes 		=11		--血色城堡
ActivityDataIndex.BlackDayTimes 		=12		--黑色沼泽	
ActivityDataIndex.WorldBossTimes		=13		--世界boss
ActivityDataIndex.FlagBattleFieldTimes 	=14		--战歌
ActivityDataIndex.HillBattleFieldTimes 	=15		--奥山
ActivityDataIndex.DayKillTimes 			=17 	--安邦任务次数
ActivityDataIndex.UnionBattleTimes		=18 	--工会战次数
ActivityDataIndex.UnionBossTimes		=19		--公会竞赛		
ActivityDataIndex.TextAnswerTimes 		=20 	--文字答题数
ActivityDataIndex.PicAnswerTimes		=21		--看图答题数
ActivityDataIndex.TextAnswerOkTimes 	=22 	--文字答题ok数
ActivityDataIndex.PicAnswerOkTimes		=23		--看图答题ok数
ActivityDataIndex.pic_reward_flag 		=24 	--看图答题礼包
ActivityDataIndex.text_reward_flag 		=25 	--文字答题礼包
ActivityDataIndex.UnionMissionTimes		=26 	--工会任务次数
ActivityDataIndex.Carbon2001Times		=27		--祖尔法拉克团队本次数
ActivityDataIndex.Carbon2002Times		=28		--熔火之心团队本次数
ActivityDataIndex.Carbon2003Times		=29		--黑翼之巢团队本次数
ActivityDataIndex.Carbon2004Times		=30		--黑龙巢穴团队本次数
ActivityDataIndex.Carbon2005Times		=31		--安其拉废墟团队本次数
ActivityDataIndex.Carbon2006Times		=32		--安其拉神殿团队本次数
ActivityDataIndex.Carbon2007Times		=33		--纳克萨玛斯团队本次数
ActivityDataIndex.UnionDonateTimes		=34		--公会捐赠
ActivityDataIndex.TowerMissionTimes		=35 	--通天塔次数


--Impact类型
PotionPhysicDefendImpact = 0; 		 		--物防buff		[持续]
PotionHpImpact = 1; 		 				--药水加血buff	[持续]
PotionMagicAttackImpact = 2; 		 		--魔攻buff		[持续]
PotionPhysicsAttackImpact = 3; 		 		--物攻buff		[持续]
PotionMagicDefendImpact = 4; 		 		--魔防buff		[持续]
BfSpeedUpImpact = 1001; 	 				--战场提速buff	[持续]
BfSpeedDownImpact =1002;					--战场减速buff	[持续]
ProtectRobImpact = 1003; 	 	 			--免劫掠		[持续]
MountSpeedImpact1 = 1004; 	 				--坐骑速度buff	[持续]
PlayerEquipDamageImpact = 1005; 	 	 	--装备耐久为0[持续]
PlayerEquipWarningImpact = 1006; 	 		--装备耐久过低[持续]
MountSpeedImpact2 = 1007; 	 				--坐骑速度buff	[持续]
MountSpeedImpact3 = 1008; 	 				--坐骑速度buff	[持续]
MountSpeedImpact4 = 1009; 	 				--坐骑速度buff	[持续]
MountSpeedImpact5 = 1010; 	 				--坐骑速度buff	[持续]
MountSpeedImpact6 = 1011; 	 				--坐骑速度buff	[持续]
MountSpeedImpact7 = 1012; 	 				--坐骑速度buff	[持续]
MountSpeedImpact8 = 1013; 	 				--坐骑速度buff	[持续]
MountSpeedImpact9 = 1014; 	 				--坐骑速度buff	[持续]
MountSpeedImpact10 = 1015; 	 				--坐骑速度buff	[持续]
PlayerLackeysImpact = 1016; 	 			--仆从守护buff	[持续]
NewSpeedUpImpact = 1017; 	 				--新手提速buff	[持续]
MountSpeedImpact11 = 1018; 	 				--坐骑速度buff	[持续]
ServerLevelImpact = 1019; 	 				--服务器等级	[持续]


--卡池
DrawGroup_CrystalFree = 0;		--钻石免费
DrawGroup_CrystalMust = 1;		--钻石保底
DrawGroup_HellBigDraw = 2;		--试炼大宝箱
DrawGroup_CoinFree = 3;			--金币免费
DrawGroup_CoinMust = 4;			--金币保底
DrawGroup_HellSmallDraw = 5;	--试炼小宝箱
DrawGroup_CrystalBigMust = 6;	--高级招募3星保底


--坐骑收集解锁属性
--收集数量，效果，值
g_MountCountEffect ={
{2,"hp",100},
{5,"hp",100},
{8,"hp",100},
{11,"hp",100},
{14,"hp",100},
{17,"hp",100},
{20,"hp",100},
}

--目标段奖励
--id,数量
g_TargetSegReward={                                              
{{20233001,10},	{20234001,	10	},	{20131001, 5}, {20236003,10} },
{{20125014,1 },	{20121003,	5	},	{20131001, 5}, {20125035,1}  },
{{20125032,1 },	{20233001,	10	},	{20131002,10}, {20236003,10} },
{{20125032,1 },	{20233001,	10	},	{20131002,10}, {20236003,10} },
{{20234001,5 },	{20121003,	2	},	{20131003,10}, {20236003,10} },
{{20234001,5 },	{20121003,	2	},	{20131003,10}, {20236003,10} },
{{20234001,5 },	{20121003,	2	},	{20131004,10}, {20236003,10} },
{{20234001,5 },	{20121003,	2	},	{20131004,10}, {20236003,10} },
{{20234001,5 },	{20121003,	2	},	{20131005,10}, {20236003,10} },
{{20234001,5 },	{20121003,	2	},	{20131005,10}, {20236003,10} },
}


--每日参与活动的标记组，对应NumberCell64Index_MissRewardFlag 标记
g_MissRewardFlag={
{ActivityType.GoldReward,		g_action_data_index.miss_reward_times		,	g_action_data_index.miss_reward_exp		,g_action_data_index.miss_reward_gold 	,{}										},
{ActivityType.CorpsFight,		g_action_data_index.miss_corps_times		,	g_action_data_index.miss_corps_exp		,g_action_data_index.miss_corps_gold	,{}										},
{ActivityType.GoblinCarbon,		g_action_data_index.miss_goblin_times		,	-1										,g_action_data_index.miss_goblin_gold 	,{}										},
{ActivityType.TinyThreeCarbon,	g_action_data_index.miss_tiny3_times		,	g_action_data_index.miss_tiny3_exp		,g_action_data_index.miss_tiny3_gold 	,{}										},
{ActivityType.BigThreeCarbon,	g_action_data_index.miss_big3_times			,	g_action_data_index.miss_big3_exp		,g_action_data_index.miss_big3_gold 	,{20236017}								},
{ActivityType.SkyFallFight,     g_action_data_index.miss_skyfall_times	 	,	g_action_data_index.miss_skyfall_exp 	,g_action_data_index.miss_skyfall_gold 	,{}										},
{ActivityType.BloodCarbon,      g_action_data_index.miss_blood_times		,	g_action_data_index.miss_blood_exp		,-1									 	,{20121001,20121002,20121003,20236005}	},
{ActivityType.BlackCarbon,      g_action_data_index.miss_black_times		,	g_action_data_index.miss_black_exp		,g_action_data_index.miss_black_gold	,{20233001}								},
{ActivityType.DayKill,			g_action_data_index.miss_daykill_times	 	,	g_action_data_index.miss_daykill_exp	,-1										,{}										},
{ActivityType.Ancholage,      	g_action_data_index.miss_ancho_times		,	-1										,-1										,{20125001}								}
}

--5宝
g_Treasure5 ={20236018,20236019,20236020,20236021,20236022}

--
-- 红点状态
g_reddot_state = {};
g_reddot_state.can_get = 1;
g_reddot_state.not_reach = 2;
g_reddot_state.already_got = 3;
g_reddot_state.no_chance = 4;
