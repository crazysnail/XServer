
--------------------------------------msg-------------------------------------
--删除玩家
S2GDeletePlayer={}
S2GDeletePlayer.data_64={}
S2GDeletePlayer.data_64.guid		=	0	--guid

--踢掉玩家
S2GKickAll={}
S2GKickAll.data_64={}
S2GKickAll.data_64.guid		=	0	--guid

--newday
S2GDayTriger={}
S2GDayTriger.data_32={}
S2GDayTriger.data_32.day		=	0	--天

--new week
S2GWeekTriger={}
S2GWeekTriger.data_32={}
S2GWeekTriger.data_32.week		=	0	--周

S2GMonthTriger={}
S2GMonthTriger.data_32={}
S2GMonthTriger.data_32.week		=	0	--月


--解锁服务器
S2GServerLevel={}

--关服
S2GShutdown={}

--lua更新
S2GLuaFilesUpdate={}

--离开战场
S2GQuitBattleField={}
S2GQuitBattleField.data_64={}
S2GQuitBattleField.data_64.guid		=	0	--guid

--进入战场
S2GEnterBattleField={}
S2GEnterBattleField.data_64={}
S2GEnterBattleField.data_64.guid		=	0	--guid

--基金
S2GInvest={}
S2GInvest.data_32={}
S2GInvest.data_32.count					=  0  --人数

--战力比拼人数添加
S2GAllPower={}
S2GAllPower.data_32={}
S2GAllPower.data_32.index					=  0  --索引
S2GAllPower.data_32.count					=  1  --人数

--等级比拼人数添加
S2GAllLevel={}
S2GAllLevel.data_32={}
S2GAllLevel.data_32.index					=  0  --索引
S2GAllLevel.data_32.count					=  1  --人数

--server opendate
S2GOpenDate={}
S2GOpenDate.data_32={}
S2GOpenDate.data_32.num					=  0  --日期

--全服战力比拼
S2GPowerAcrCheck={}
S2GPowerAcrCheck.data_32={}
S2GPowerAcrCheck.data_32.index		 	=  0 	 --档位
S2GPowerAcrCheck.data_32.limit_count 	=  1 	 --人数
S2GPowerAcrCheck.data_64={}
S2GPowerAcrCheck.data_64.guid			=  0	--guid

--全服等级比拼
S2GLevelAcrCheck={}
S2GLevelAcrCheck.data_32={}
S2GLevelAcrCheck.data_32.index		 	=  0 	 --档位
S2GLevelAcrCheck.data_32.limit_count 	=  1 	 --人数
S2GLevelAcrCheck.data_64={}
S2GLevelAcrCheck.data_64.guid			=	0	--guid

--团购
S2GAllBuyAcrJoin={}
S2GAllBuyAcrJoin.data_32={}
S2GAllBuyAcrJoin.data_32.index		 	=  0 	 --档位
S2GAllBuyAcrJoin.data_64={}
S2GAllBuyAcrJoin.data_64.guid			=	0	--guid

--抢购
S2GQuikBuyAcrJoin={}
S2GQuikBuyAcrJoin.data_32={}
S2GQuikBuyAcrJoin.data_32.index		 	=  0 	 --档位
S2GQuikBuyAcrJoin.data_64={}
S2GQuikBuyAcrJoin.data_64.guid			=	0	--guid

--离开队伍之后离开场景
S2GLeaveTeamAndScene={}
S2GLeaveTeamAndScene.data_64={}
S2GLeaveTeamAndScene.data_64.guid	=	0	--guid

--离开队伍
S2GLeaveTeam={}
S2GLeaveTeam.data_64={}
S2GLeaveTeam.data_64.guid	=	0	--guid

--更新Team中成员的等级
S2GSyncTeamMemberLevel={}
S2GSyncTeamMemberLevel.data_32={}
S2GSyncTeamMemberLevel.data_32.playerlevel			=	0	--playerlevel
S2GSyncTeamMemberLevel.data_64={}
S2GSyncTeamMemberLevel.data_64.playerguid			=	0	--playerguid
S2GSyncTeamMemberLevel.data_64.teamid				=	1	--teamid

--创建工会信息
S2GCreateGuildReq={}
S2GCreateGuildReq.data_64={}
S2GCreateGuildReq.data_64.playerguid	=	0	--playerguid
S2GCreateGuildReq.data_string={}
S2GCreateGuildReq.data_string.createname		=	0	--工会名
S2GCreateGuildReq.data_string.createnotice		=	0	--工会公告

--公会副本战斗开始
S2GFightGuildWetCopy={}
S2GFightGuildWetCopy.data_64={}
S2GFightGuildWetCopy.data_64.playerguid	=	0	--playerguid
S2GFightGuildWetCopy.data_32={}
S2GFightGuildWetCopy.data_32.copyid		=	0	--副本ID

--战歌战斗创建结果
S2GBfCreateBattleResult={}
S2GBfCreateBattleResult.data_32={}
S2GBfCreateBattleResult.data_32.isok		=	0	--结果
S2GBfCreateBattleResult.data_64={}
S2GBfCreateBattleResult.data_64.player_guid	=	0	--player_guid
S2GBfCreateBattleResult.data_64.target_guid	=	1	--target_guid

--公会副本战斗结束伤害
S2GEndGuildWetCopy={}
S2GEndGuildWetCopy.data_64={}
S2GEndGuildWetCopy.data_64.guid		=	0	--guid
S2GEndGuildWetCopy.data_32={}
S2GEndGuildWetCopy.data_32.copyid	=	0	--副本ID
S2GEndGuildWetCopy.data_32.damage	=	1	--伤害

--load玩家的空间信息
LoadZoneInfo={}
LoadZoneInfo.data_64={}
LoadZoneInfo.data_64.guid		=	0	--guid
---
S2GTbReload={}
S2GTbReload.data_string={}
S2GTbReload.data_string.name		=	0	--表名

S2GSwitchMarket={}
S2GSwitchMarket.data_32={}
S2GSwitchMarket.data_32.id			=	0	--商店id
S2GSwitchMarket.data_32.open		=	1	--商店开关

S2GSwitchMarketItem={}
S2GSwitchMarketItem.data_32={}
S2GSwitchMarketItem.data_32.id			=	0	--商店id
S2GSwitchMarketItem.data_32.index		=	1	--物品索引
S2GSwitchMarketItem.data_32.open		=	2	--商店开关

--增加帮会资金
S2GAddGuildMoney={}
S2GAddGuildMoney.data_32={}
S2GAddGuildMoney.data_32.money			=	0	--money
S2GAddGuildMoney.data_64={}
S2GAddGuildMoney.data_64.playerid		=	0	--guid

--增加帮战次数
S2GAddGuildBattleTimes={}
S2GAddGuildBattleTimes.data_64={}
S2GAddGuildBattleTimes.data_64.guildid		=	0	--guid
S2GAddGuildBattleTimes.data_64.playerid		=	1	--guid

--帮会战结束结算
S2GGuildBattleWin={}
S2GGuildBattleWin.data_32={}
S2GGuildBattleWin.data_32.win			=	0	--胜利/失败
S2GGuildBattleWin.data_32.score			=	1	--分数
S2GGuildBattleWin.data_32.playercount 	= 	2	--剩余玩家数
S2GGuildBattleWin.data_32.allplayercount=	3	--所有玩家数
S2GGuildBattleWin.data_64={}
S2GGuildBattleWin.data_64.guid			=	0	--guid

--玩家重连后去主线程更新玩家的公会数据
S2GLoginSyncGuildUserData={}
S2GLoginSyncGuildUserData.data_32={}
S2GLoginSyncGuildUserData.data_32.playerlevel			=	0	--playerlevel
S2GLoginSyncGuildUserData.data_64={}
S2GLoginSyncGuildUserData.data_64.playerguid			=	0	--playerguid
S2GLoginSyncGuildUserData.data_64.guildid				=	1	--guildid
S2GLoginSyncGuildUserData.data_string={}
S2GLoginSyncGuildUserData.data_string.playername		=	0	--玩家名

S2GSyncGuildUserName={}
S2GSyncGuildUserName.data_64={}
S2GSyncGuildUserName.data_64.playerguid			=	0	--playerguid
S2GSyncGuildUserName.data_64.guildid			=	1	--guildid
S2GSyncGuildUserName.data_string={}
S2GSyncGuildUserName.data_string.playername		=	0	--玩家名

S2GSyncGuildUserLevel={}
S2GSyncGuildUserLevel.data_32={}
S2GSyncGuildUserLevel.data_32.playerlevel			=	0	--playerlevel
S2GSyncGuildUserLevel.data_64={}
S2GSyncGuildUserLevel.data_64.playerguid			=	0	--playerguid
S2GSyncGuildUserLevel.data_64.guildid				=	1	--guildid

S2GLoginOutSyncGuildUserData={}
S2GLoginOutSyncGuildUserData.data_64={}
S2GLoginOutSyncGuildUserData.data_64.playerguid			=	0	--playerguid
S2GLoginOutSyncGuildUserData.data_64.guildid				=	1	--guildid

S2GSyncGuildUserCurcontribution={}
S2GSyncGuildUserCurcontribution.data_32={}
S2GSyncGuildUserCurcontribution.data_32.curcontribution		=	0	--curcontribution
S2GSyncGuildUserCurcontribution.data_64={}
S2GSyncGuildUserCurcontribution.data_64.playerguid			=	0	--playerguid
S2GSyncGuildUserCurcontribution.data_64.guildid				=	1	--guildid

S2GSyncGuildUserWeekcontribution={}
S2GSyncGuildUserWeekcontribution.data_32={}
S2GSyncGuildUserWeekcontribution.data_32.weekcontribution		=	0	--weekcontribution
S2GSyncGuildUserWeekcontribution.data_64={}
S2GSyncGuildUserWeekcontribution.data_64.playerguid				=	0	--playerguid
S2GSyncGuildUserWeekcontribution.data_64.guildid				=	1	--guildid

S2GSyncGuildUserAllcontribution={}
S2GSyncGuildUserAllcontribution.data_32={}
S2GSyncGuildUserAllcontribution.data_32.allcontribution		=	0	--allcontribution
S2GSyncGuildUserAllcontribution.data_64={}
S2GSyncGuildUserAllcontribution.data_64.playerguid			=	0	--playerguid
S2GSyncGuildUserAllcontribution.data_64.guildid				=	1	--guildid

S2GAddGuildBonus={}
S2GAddGuildBonus.data_32={}
S2GAddGuildBonus.data_32.add_count				=	0	--add_count
S2GAddGuildBonus.data_64={}
S2GAddGuildBonus.data_64.guildid				=	0	--guildid

--acr活动更新
S2GAcrRefresh={}

--
S2GUpdateOperate={}
S2GUpdateOperate.data_32={}
S2GUpdateOperate.data_32.script_id				=	0	--脚本id

S2GFlushDelay={}

--玩家下线
S2GLogout={}
S2GLogout.data_64={}
S2GLogout.data_64.guid				=	0	--guid

--玩家缓存
S2GCacheOk={}
S2GCacheOk.data_64={}
S2GCacheOk.data_64.guid				=	0	--guid

-----------------------

GuildSpoilItemReq={}
GuildSpoilItemReq.data_32={}
GuildSpoilItemReq.data_32.copyid		=	0	--copyid

GetGuildCopyDamageReq={}
GetGuildCopyDamageReq.data_32={}
GetGuildCopyDamageReq.data_32.copyid		=	0	--copyid

GuildSpoilApplyItemReq={}
GuildSpoilApplyItemReq.data_32={}
GuildSpoilApplyItemReq.data_32.index	=	0	--index

FireGuildUserReq={}
FireGuildUserReq.data_64={}
FireGuildUserReq.data_64.guid		=	0	--guid

SetGuildUserPositionReq={}
SetGuildUserPositionReq.data_32={}
SetGuildUserPositionReq.data_32.position	=	0	--position
SetGuildUserPositionReq.data_64={}
SetGuildUserPositionReq.data_64.guid		=	0	--guid

MessageBoardReq={}
MessageBoardReq.data_64={}
MessageBoardReq.data_64.targetguid	=	0	--targetguid
MessageBoardReq.string={}
MessageBoardReq.string.message		=	0	--message
MessageBoardReq.string.replyplayer  =	1	--replyplayer


-----
--玩家重连通知主线程
S2GPlayerReConnected={}
S2GPlayerReConnected.data_64={}
S2GPlayerReConnected.data_64.guid		=	0	--guid

--更新通天塔数据
S2GUpdateTowerInfo={}
S2GUpdateTowerInfo.data_32={}
S2GUpdateTowerInfo.data_32.level	=	0	--层数
S2GUpdateTowerInfo.data_32.time	=	1	--时间
S2GUpdateTowerInfo.data_64={}
S2GUpdateTowerInfo.data_64.guid		=	0		--guid