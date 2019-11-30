
--------------------------------------msg-------------------------------------
--活动更新
msg_acr_data={}
msg_acr_data.data_32 = {}
msg_acr_data.data_32.script_id		=	0	--配置索引
msg_acr_data.data_32.ud				=	1	--ud
msg_acr_data.data_32.active			=	2	--活动激活标记(0预览,1激活)

--活动关闭
msg_acr_close={}
msg_acr_close.data_32 = {}
msg_acr_close.data_32.script_id		=	0	--配置索引

--领奖
msg_acr_request={}
msg_acr_request.data_32 = {}
msg_acr_request.data_32.script_id		=	0	--活动脚本id
msg_acr_request.data_32.reward_index	=	1	--奖励索引
msg_acr_request.data_32.sub_index		=	2	--奖励子索引
msg_acr_request.data_string={}
msg_acr_request.data_string.tag			=	0	--组

--领奖数据
msg_acr_reply={}
msg_acr_reply.data_32 = {}
msg_acr_reply.data_32.script_id		=	0	--配置索引
msg_acr_reply.data_32.reward_index	=	1	--奖励索引
msg_acr_reply.data_32.item_id		=	2	--道具id
msg_acr_reply.data_32.item_count	=	3	--道具数量
msg_acr_reply.data_32.value			=	4	--参数

--开始答题
BeginAnswer={}
BeginAnswer.data_32={}
BeginAnswer.data_32.type			=	0	--类型

--继续答题
ContinueAnswer={}
ContinueAnswer.data_32={}
ContinueAnswer.data_32.type			=	0	--类型

--提交答案
ConfirmTextAnswer={}
ConfirmTextAnswer.data_32={}
ConfirmTextAnswer.data_32.index			=	0	--id
ConfirmTextAnswer.data_string={}
ConfirmTextAnswer.data_string.option	=	0	--选项

ConfirmPicAnswer={}
ConfirmPicAnswer.data_32={}
ConfirmPicAnswer.data_32.index			=	0	--id
ConfirmPicAnswer.data_32.option			=	1	--选项

--错误提示
TextAnswerWrong={}
TextAnswerWrong.data_32={}
TextAnswerWrong.data_32.index			=	0	--id
TextAnswerWrong.data_string={}
TextAnswerWrong.data_string.option		=	0	--正确选项

PicAnswerWrong={}
PicAnswerWrong.data_32={}
PicAnswerWrong.data_32.index			=	0	--id
PicAnswerWrong.data_32.option			=	1	--正确选项

--礼包
PicAnswerReward={}
PicAnswerReward.data_32={}
PicAnswerReward.data_32.index			=	0	--索引

--礼包
TextAnswerReward={}
TextAnswerReward.data_32={}
TextAnswerReward.data_32.index			=	0	--索引

--领取目标奖励
GetTargetReward={}
GetTargetReward.data_32={}
GetTargetReward.data_32.index			=	0	--index

--领取目标段奖励
GetTargetSegReward={}
GetTargetSegReward.data_32={}
GetTargetSegReward.data_32.index		=	0	--index

msg_activity={}
msg_activity["msg_acr_data"]=msg_acr_data
msg_activity["msg_acr_close"]=msg_acr_close
msg_activity["msg_acr_request"]=msg_acr_request
msg_activity["msg_acr_reply"]=msg_acr_reply
msg_activity["BeginAnswer"]=BeginAnswer
msg_activity["ContinueAnswer"]=ContinueAnswer
msg_activity["ConfirmTextAnswer"]=ConfirmTextAnswer
msg_activity["ConfirmPicAnswer"]=ConfirmPicAnswer
msg_activity["TextAnswerWrong"]=TextAnswerWrong
msg_activity["PicAnswerWrong"]=PicAnswerWrong
msg_activity["PicAnswerReward"]=PicAnswerReward
msg_activity["TextAnswerReward"]=TextAnswerReward
msg_activity["GetTargetReward"]=GetTargetReward
msg_activity["GetTargetSegReward"]=GetTargetSegReward

