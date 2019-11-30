
--------------------------------------msg-------------------------------------
--购买请求
buy_item_request={}
buy_item_request.data_32={}
buy_item_request.data_32.mtype		=	0	--类型
buy_item_request.data_32.index		=	1	--索引
buy_item_request.data_32.count		=	2	--数量

--商店数据请求
market_request={}
market_request.data_32={}
market_request.data_32.mtype		=	0	--类型

--商店刷新
refresh_market_req={}
refresh_market_req.data_32={}
refresh_market_req.data_32.mtype		=	0	--类型

--npc列表请求
npcs_request={}

--兑换码请求
cdkcode_request={}
cdkcode_request.data_string={}
cdkcode_request.data_string.code		=	0	--类型

--五宝
t5_request={}


--道具加锁
lock_request={}
lock_request.data_64={}
lock_request.data_64.guid		=	0		--guid

--道具解锁
unlock_request={}
unlock_request.data_64={}
unlock_request.data_64.guid		=	0		--guid

--经验点开关
exppoint_request={}
exppoint_request.data_32={}
exppoint_request.data_32.switch		=	0	--类型

--存金币
StoreGold={}
StoreGold.data_64={}
StoreGold.data_64.value		=	0			--值

--取金币
TakeGold={}
TakeGold.data_64={}
TakeGold.data_64.value		=	0			--开关

--使用道具
UseItem={}
UseItem.data_64={}
UseItem.data_64.guid		=	0			--guid

--批量使用道具
UseMultItem={}
UseMultItem.data_32={}
UseMultItem.data_32.config_id		=	0		--id
UseMultItem.data_32.count			=	1		--count

--道具摧毁
DestroyItem={}
DestroyItem.data_64={}
DestroyItem.data_64.guid		=	0		--guid

--卖出道具
SellItem={}
SellItem.data_64={}
SellItem.data_64.guid		=	0			--guid

--采集任务结束
CollectOver={}
CollectOver.data_32={}
CollectOver.data_32.posid	=	0			--地点id

--领取每日活跃度奖励
ActivityRewardBox={}
ActivityRewardBox.data_32={}
ActivityRewardBox.data_32.index		=	0	--索引

--抽卡
DrawRequest={}
DrawRequest.data_32={}
DrawRequest.data_32.dtype		=	0	--抽卡类型

--鼓舞
Inspire={}
Inspire.data_32={}
Inspire.data_32.itype		=	0			--鼓舞类型

--世界boss复活
WorldbossRelive={}

--试炼场复活
HellRelive={}

--进阶任务
UpgradeMission={}
UpgradeMission.data_32={}
UpgradeMission.data_32.hero_id		=	0	--英雄id

--安邦任务
DayKillMission={}

--发布题目
NewPicAnswer={}
NewPicAnswer.data_32={}
NewPicAnswer.data_32.index				=	0	--表索引
NewPicAnswer.data_32.option0			=	1	--option
NewPicAnswer.data_32.option1			=	2	--option
NewPicAnswer.data_32.option2			=	3	--option

NewTextAnswer={}
NewTextAnswer.data_32={}
NewTextAnswer.data_32.index				=	0	--表索引
NewTextAnswer.data_string={}
NewTextAnswer.data_string.option0			=	0	--id
NewTextAnswer.data_string.option1			=	1	--id
NewTextAnswer.data_string.option2			=	2	--id
NewTextAnswer.data_string.option3			=	3	--id

--开启通知
TextAnswersNotify={}
TextAnswersNotify.data_32={}
TextAnswersNotify.data_32.cd				=	0	--cd
TextAnswersNotify.data_string={}
TextAnswersNotify.data_string.text			=	0	--内容

PicAnswersNotify={}
PicAnswersNotify.data_32={}
PicAnswersNotify.data_32.cd					=	0	--cd
PicAnswersNotify.data_string={}
PicAnswersNotify.data_string.text			=	0	--内容

--骑坐骑
RideMount={}
RideMount.data_32={}
RideMount.data_32.index					=	0	--id
RideMount.data_64={}
RideMount.data_64.guid					=	0	--id

--下坐骑
UnRideMount={}
UnRideMount.data_64={}
UnRideMount.data_64.guid					=	0	--id

--解锁坐骑
UnlockMount={}
UnlockMount.data_32={}
UnlockMount.data_32.index			=	0	--id

--指定分配
ArrangeItem={}
ArrangeItem.data_64={}
ArrangeItem.data_64.item_guid			=	0	--道具guid
ArrangeItem.data_64.player_guid			=	1	--玩家guid

--roll点
ConfirmRollItem={}
ConfirmRollItem.data_64={}
ConfirmRollItem.data_64.item_guid			=	0	--guid,0表示任意装备

CancelRollItem={}

--队长关闭分配对话框请求
CloseArrangeUIReq={}
--队长关闭分配对话框通知
CloseArrangeUIRep={}

--new apk
NewApkNotify={}
NewApkNotify.data_string={}
NewApkNotify.data_string.tips			=	0	--提示
NewApkNotify.data_string.url			=	1	--url


--暂停战斗
PauseBattle={}
--继续战斗
ResumeBattle={}
--出发剧情
StoryStage={}
StoryStage.data_32={}
StoryStage.data_32.pause_idx = 0 --剧情id

--获取挂机关卡设置
GetOfflineHookData={}

--当前挂机关卡设置
CurrentOfflineHookData={}
CurrentOfflineHookData.data_32={}
CurrentOfflineHookData.data_32.stage_0	=	0 --关卡id
CurrentOfflineHookData.data_32.time_0	=	1 --玩家设置的时间
CurrentOfflineHookData.data_32.left_time_0 = 2 --当前剩余时间
CurrentOfflineHookData.data_32.stage_1	=	3 --关卡id
CurrentOfflineHookData.data_32.time_1	=	4 --玩家设置的时间
CurrentOfflineHookData.data_32.left_time_1 = 5 --当前剩余时间
CurrentOfflineHookData.data_32.stage_2	=	6 --关卡id
CurrentOfflineHookData.data_32.time_2	=	7 --玩家设置的时间
CurrentOfflineHookData.data_32.left_time_2 = 8 --当前剩余时间

--挂机效率更新
HookDataEfficentUpdate={}
HookDataEfficentUpdate.data_32={}
HookDataEfficentUpdate.data_32.exp = 0 --当前挂机效率,经验每分钟
HookDataEfficentUpdate.data_32.stage_id = 1 --当前的离线挂机关卡

--设置挂机数据
SetOfflineHookData={}
SetOfflineHookData.data_32={}
SetOfflineHookData.data_32.stage_0		=	0 --关卡id
SetOfflineHookData.data_32.time_0		=	1 --挂机时长 单位秒
SetOfflineHookData.data_32.stage_1		=	2
SetOfflineHookData.data_32.time_1		=	3
SetOfflineHookData.data_32.stage_2		=	4
SetOfflineHookData.data_32.time_2		=	5


--分配过滤列表返回包
ATFilterList={}
ATFilterList.data_64={}					--过滤列表

--任务同步返回包
BeginSyncMission={}						--广播开始同步状态
SyncMissionFail={}						--广播给其他人最终失败结果
SyncMissionOk={}						--广播给其他人最终OK结果

AgreeSyncMission={}						--广播给其他人同意结果
AgreeSyncMission.data_64={}
AgreeSyncMission.data_64.guid = 0 		--guid

DisagreeSyncMission={}					--广播给其他人拒绝结果
DisagreeSyncMission.data_64={}
DisagreeSyncMission.data_64.guid = 0 	--guid

--申请切磋
FightRequest={}
FightRequest.data_64={}
FightRequest.data_64.target_guid = 0

--切磋提示
FightNotify={}
FightNotify.data_64={}
FightNotify.data_64.challenger_guid = 0 --发起方的guid
FightNotify.data_string={}
FightNotify.data_string.challenger_name = 0 --发起方的名字

--切磋确认
FightAck={}
FightAck.data_32={}
FightAck.data_32.confirm = 0 -- 0, 拒绝, 1 确认
FightAck.data_64={}
FightAck.data_64.challenger_guid = 0 --发起方的guid

--查看当前设置
PullPlayerSetting={}

--下线
Logout={}

--当前设置
CurrentPlayerSetting={}
CurrentPlayerSetting.data_32={}
CurrentPlayerSetting.data_32.ignore_add_friend = 0;
CurrentPlayerSetting.data_32.reject_fight = 1;

--玩家设置相关
UpdatePlayerSetting={}
UpdatePlayerSetting.data_32={}
UpdatePlayerSetting.data_32.ignore_add_friend = 0
UpdatePlayerSetting.data_32.reject_fight = 1

--设置仆从
SetLackeysReq={}
SetLackeysReq.data_32={}
SetLackeysReq.data_32.index = 0				--index
SetLackeysReq.data_64={}
SetLackeysReq.data_64.actor_guid = 0		--guid

--设置仆从成功
UpdateLackeysRep={}
UpdateLackeysRep.data_32={}
UpdateLackeysRep.data_32.index = 0				--index
UpdateLackeysRep.data_64={}
UpdateLackeysRep.data_64.actor_guid = 0			--guid

--触发buff
OnBuffPointReq={}
OnBuffPointReq.data_32={}
OnBuffPointReq.data_32.index = 0			--posid

--奖励找回协议
MissRewardReq={}
MissRewardReq.data_32={}
MissRewardReq.data_32.index = 0			--索引
MissRewardReq.data_32.times = 1			--找回次数
MissRewardReq.data_32.type = 2			--找回类型

--充值请求
ChargeItemReq={}
ChargeItemReq.data_32={}
ChargeItemReq.data_32.index 				= 0	--posid
ChargeItemReq.data_32.ud 					= 1	--ud
ChargeItemReq.data_string={}
ChargeItemReq.data_string.productid			= 0	--productid
ChargeItemReq.data_string.channelid			= 1	--channelid

--公会捐赠
GuildDonationReq={}
GuildDonationReq.data_32={}
GuildDonationReq.data_32.type = 0 			--类型

--离开副本倒计时
raid_mission_over={}
raid_mission_over.data_32={}
raid_mission_over.data_32.time 				= 0	--倒计时，-1就不显示倒计时确认/取消面板，值显示离开链接

--战歌开启倒计时
BattleFieldNotify={}
BattleFieldNotify.data_32={}
BattleFieldNotify.data_32.time 				= 0	--倒计时
BattleFieldNotify.data_string={}
BattleFieldNotify.data_string.text			= 0	--内容

--开启商店
OpenMarket={}
OpenMarket.data_32={}
OpenMarket.data_32.type = 0 			--类型

--关闭商店
CloseMarket={}
CloseMarket.data_32={}
CloseMarket.data_32.type = 0 			 --类型

--新手引导
guide_step_requet={}
guide_step_requet.data_32={}
guide_step_requet.data_32.group		=	0	--组
guide_step_requet.data_32.step		=	1	--步数


guide_flag_request={}
guide_flag_request.data_64={}
guide_flag_request.data_64.flag = 0 	 	--guide 数据

inner_hook_flag_request={}
inner_hook_flag_request.data_64={}
inner_hook_flag_request.data_64.flag = 0 	 	--内挂 数据

set_mission_auto_buy_item_flag_request={}
set_mission_auto_buy_item_flag_request.data_32={}
set_mission_auto_buy_item_flag_request.data_32.flag = 0 	 	--设置任务自动购买标记

get_mission_auto_buy_item_flag_request={} 			--获取任务自动购买标记

mission_auto_buy_item_flag={}
mission_auto_buy_item_flag.data_32={}
mission_auto_buy_item_flag.data_32.flag = 0 

--新手寻路加速协议
SpeedImpactRequset={}
SpeedImpactRequset.data_32={}
SpeedImpactRequset.data_32.enable = 0 		--开关

--更新宝石大师等级
UpdateGemMasterLv={}
UpdateGemMasterLv.data_32={}
UpdateGemMasterLv.data_32.level = 0			--等级
UpdateGemMasterLv.data_64={}
UpdateGemMasterLv.data_64.guid = 0			--guid

--广播宝石大师等级
BoradCastGemMasterLv={}
BoradCastGemMasterLv.data_32={}
BoradCastGemMasterLv.data_32.level = 0			--等级
BoradCastGemMasterLv.data_64={}
BoradCastGemMasterLv.data_64.guid = 0			--guid

--首次进入场景消息
SceneFirstTimeNotify={}

--试炼场结算请求
HellStatisticsReq={}

--掉落包
DropPackageNotify={}

--充值订单
ChargeOderIdReply={}
ChargeOderIdReply.data_string={}
ChargeOderIdReply.data_string.oderid			= 0	--订单号

--服务器等级相关
RefreshSeverLvRep={}
RefreshSeverLvRep.data_32={}
RefreshSeverLvRep.data_32.serverlv				=	0	--等级
RefreshSeverLvRep.data_32.nextlvday 			=	1	--天数

--藏宝图使用坐标
TreasurePosReq={}
TreasurePosReq.data_64={}
TreasurePosReq.data_64.guid = 0					--guid

--藏宝图坐标
TreasurePosRep={}
TreasurePosRep.data_32={}
TreasurePosRep.data_32.posid = 0 				--posid
TreasurePosRep.data_64={}
TreasurePosRep.data_64.guid = 0					--guid