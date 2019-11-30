

-------------S2S--------------
--结束场景raid战斗
S2SRaidEndAllBattle={}
S2SRaidEndAllBattle.data_64={}
S2SRaidEndAllBattle.data_64.raid_rtid		=	0	--raidid

S2SRaidDestroy={}
S2SRaidDestroy.data_64={}
S2SRaidDestroy.data_64.raid_rtid			=	0	--raidid

--克隆玩家任务数据
S2SCloneMission={}
S2SCloneMission.data_32={}
S2SCloneMission.data_32.mission_id		=	0	--任务号
S2SCloneMission.data_64={}
S2SCloneMission.data_64.target_guid		=	0	--guid

--生成副本buff
S2SGenRaidBuff={}
S2SGenRaidBuff.data_32={}
S2SGenRaidBuff.data_32.raid_rtid			=	0	--raidid


-------------G2S---------
--创建工会信息
G2SCreateGuildReq={}
G2SCreateGuildReq.data_string={}
G2SCreateGuildReq.data_string.createname		=	0	--工会名
G2SCreateGuildReq.data_string.createnotice		=	0	--工会公告

--更新工会信息
G2SUpdateGuildInfo={}
G2SUpdateGuildInfo.data_64={}
G2SUpdateGuildInfo.data_64.guild_guid			=	0	--guid
G2SUpdateGuildInfo.data_string={}
G2SUpdateGuildInfo.data_string.guild_name		=	0	--工会名
G2SUpdateGuildInfo.data_string.log_from			=	1	--日志

--更新工会信息
G2SUpdateGuildName={}
G2SUpdateGuildName.data_string={}
G2SUpdateGuildName.data_string.guild_name		=	0	--工会名

--公会训练
G2SAddPracticeExp={}
G2SAddPracticeExp.data_32={}
G2SAddPracticeExp.data_32.id			=	0	--id
G2SAddPracticeExp.data_32.type			=	1	--type
G2SAddPracticeExp.data_32.level			=	2	--level
G2SAddPracticeExp.data_32.contribution	=	3	--allcontribution


--更新工会建筑升级数据
G2SGuildBuildLevelUp={}
G2SGuildBuildLevelUp.data_32={}
G2SGuildBuildLevelUp.data_32.type			=	0	--type
G2SGuildBuildLevelUp.data_32.level			=	1	--level

--更新工会职位
G2SGuildUserPosition={}
G2SGuildUserPosition.data_32={}
G2SGuildUserPosition.data_32.position			=	0	--position

--工会红利
G2SGuildBonus={}
G2SGuildBonus.data_32={}
G2SGuildBonus.data_32.bonus						=	0	--bonus

--工会副本战斗
G2SFightGuildWetCopy={}
G2SFightGuildWetCopy.data_32={}
G2SFightGuildWetCopy.data_32.copyid			=	0	--副本ID
G2SFightGuildWetCopy.data_32.stageid		=	1	--stageid
--hp数组

--离开场景
G2SGoingLeaveScene={}

--lua更新
G2SLuaFilesUpdate={}

--商店开关
G2SSwitchMarket={}
G2SSwitchMarket.data_32={}
G2SSwitchMarket.data_32.id			=	0	--商店id
G2SSwitchMarket.data_32.open		=	1	--商店开关

--商店道具
G2SSwitchMarketItem={}
G2SSwitchMarketItem.data_32={}
G2SSwitchMarketItem.data_32.id			=	0	--商店id
G2SSwitchMarketItem.data_32.index		=	1	--道具索引
G2SSwitchMarketItem.data_32.open		=	2	--商店开关

--充值回调物品
G2SChargeItem={}
G2SChargeItem.data_string={}
G2SChargeItem.data_string.orderid		=	0	--订单号
G2SChargeItem.data_string.productid		=	1	--商品名
G2SChargeItem.data_string.ip			=	2	--ip
G2SChargeItem.data_string.extra			=	3	--extra

--关服登出
G2SLogout={}

--奖励活动更新
G2SAcrRefresh={}

--活动更新
G2SActivityRefresh={}

--加入公会成功
G2SJoinGuildOk={}

--服务器日期刷新
G2SUpdateServerLevel={}

--创建战场战斗
G2SCreateBfBattle={}
G2SCreateBfBattle.data_32={}
G2SCreateBfBattle.data_32.battle_group_id			=	0	--战斗组
G2SCreateBfBattle.data_64={}
G2SCreateBfBattle.data_64.target_guid				=	0	--目标guid

--复活
G2SRelive={}
G2SRelive.data_32={}
G2SRelive.data_32.hp_type			=	0	--hp类型

--副本传送
G2SGoRaid={}
G2SGoRaid.data_32={}
G2SGoRaid.data_32.sceneid		=	0	--目标场景
G2SGoRaid.data_32.scriptid		=	1	--副本脚本
G2SGoRaid.data_32.raidid		=	2	--副本id
G2SGoRaid.data_32.camp			=	3	--阵营
G2SGoRaid.data_32.x				=	4	--x
G2SGoRaid.data_32.y				=	5	--y
G2SGoRaid.data_32.z				=	6	--z


--场景传送
G2SGoTo={}
G2SGoTo.data_32={}
G2SGoTo.data_32.sceneid			=	0	--目标场景
G2SGoTo.data_32.x				=	1	--x
G2SGoTo.data_32.y				=	2	--y
G2SGoTo.data_32.z				=	3	--z

--月更新
G2SMonthTriger={}
G2SMonthTriger.data_32={}
G2SMonthTriger.data_32.value   	=   0  --月

--周更新
G2SWeekTriger={}
G2SWeekTriger.data_32={}
G2SWeekTriger.data_32.value   	=   0  --周	

--日更新
G2SDayTriger={}
G2SDayTriger.data_32={}
G2SDayTriger.data_32.value   	=   0  --日	

--时更新
G2SHourriger={}
G2SHourriger.data_32={}
G2SHourriger.data_32.value   	=   0  --时		

--分更新
G2SMinTriger={}
G2SMinTriger.data_32={}
G2SMinTriger.data_32.value   	=   0  --分			

--基金购买
G2SInvest={}

--
G2SPowerAcrCheck={}
G2SPowerAcrCheck.data_32={}
G2SPowerAcrCheck.data_32.result				=	0	--返回值
G2SPowerAcrCheck.data_32.index				=	1	--索引

G2SPowerAcrRefresh={}
G2SPowerAcrRefresh.data_32={}
G2SPowerAcrRefresh.data_32.count   			=   0  --剩余数
G2SPowerAcrRefresh.data_32.index   			=   1  --索引

G2SLevelAcrCheck={}
G2SLevelAcrCheck.data_32={}
G2SLevelAcrCheck.data_32.result				=	0	--返回值
G2SLevelAcrCheck.data_32.index				=	1	--索引

G2SLevelAcrRefresh={}
G2SLevelAcrRefresh.data_32={}
G2SLevelAcrRefresh.data_32.count   			=   0  --剩余数
G2SLevelAcrRefresh.data_32.index   			=   1  --索引

G2SAllBuyAcrJoin={}
G2SAllBuyAcrJoin.data_32={}
G2SAllBuyAcrJoin.data_32.result				=	0	--返回值
G2SAllBuyAcrJoin.data_32.index				=	1	--索引

--秒杀参与
G2SQuikBuyAcrJoin={}
G2SQuikBuyAcrJoin.data_32={}
G2SQuikBuyAcrJoin.data_32.result			=	0	--返回值
G2SQuikBuyAcrJoin.data_32.index				=	1	--索引

--秒杀阶段开始
G2SQuikBuyAcrBegin={}
G2SQuikBuyAcrBegin.data_32={}
G2SQuikBuyAcrBegin.data_32.script_id   	=   0  --脚本号
G2SQuikBuyAcrBegin.data_32.count   		=   1  --剩余数
G2SQuikBuyAcrBegin.data_32.index   		=   2  --索引

--秒杀计数更新
G2SQuikBuyAcrRefresh={}
G2SQuikBuyAcrRefresh.data_32={}
G2SQuikBuyAcrRefresh.data_32.count   	=   0  --剩余数

--秒杀整个结束
G2SQuikBuyAcrOver={}
G2SQuikBuyAcrOver.data_32={}
G2SQuikBuyAcrOver.data_32.script_id   	=   0  --脚本号

--运营活动更新,activity_id==-1表所有
G2SUpdateAcrConfig={}
G2SUpdateAcrConfig.data_32={}
G2SUpdateAcrConfig.data_32.activity_id =   0  --id	

G2SRefreshAcrConfig={}
G2SRefreshAcrConfig.data_32={}
G2SRefreshAcrConfig.data_32.activity_id =   0  --id	
