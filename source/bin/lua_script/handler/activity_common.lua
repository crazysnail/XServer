
--------------------------------------msg-------------------------------------
--�����
msg_acr_data={}
msg_acr_data.data_32 = {}
msg_acr_data.data_32.script_id		=	0	--��������
msg_acr_data.data_32.ud				=	1	--ud
msg_acr_data.data_32.active			=	2	--�������(0Ԥ��,1����)

--��ر�
msg_acr_close={}
msg_acr_close.data_32 = {}
msg_acr_close.data_32.script_id		=	0	--��������

--�콱
msg_acr_request={}
msg_acr_request.data_32 = {}
msg_acr_request.data_32.script_id		=	0	--��ű�id
msg_acr_request.data_32.reward_index	=	1	--��������
msg_acr_request.data_32.sub_index		=	2	--����������
msg_acr_request.data_string={}
msg_acr_request.data_string.tag			=	0	--��

--�콱����
msg_acr_reply={}
msg_acr_reply.data_32 = {}
msg_acr_reply.data_32.script_id		=	0	--��������
msg_acr_reply.data_32.reward_index	=	1	--��������
msg_acr_reply.data_32.item_id		=	2	--����id
msg_acr_reply.data_32.item_count	=	3	--��������
msg_acr_reply.data_32.value			=	4	--����

--��ʼ����
BeginAnswer={}
BeginAnswer.data_32={}
BeginAnswer.data_32.type			=	0	--����

--��������
ContinueAnswer={}
ContinueAnswer.data_32={}
ContinueAnswer.data_32.type			=	0	--����

--�ύ��
ConfirmTextAnswer={}
ConfirmTextAnswer.data_32={}
ConfirmTextAnswer.data_32.index			=	0	--id
ConfirmTextAnswer.data_string={}
ConfirmTextAnswer.data_string.option	=	0	--ѡ��

ConfirmPicAnswer={}
ConfirmPicAnswer.data_32={}
ConfirmPicAnswer.data_32.index			=	0	--id
ConfirmPicAnswer.data_32.option			=	1	--ѡ��

--������ʾ
TextAnswerWrong={}
TextAnswerWrong.data_32={}
TextAnswerWrong.data_32.index			=	0	--id
TextAnswerWrong.data_string={}
TextAnswerWrong.data_string.option		=	0	--��ȷѡ��

PicAnswerWrong={}
PicAnswerWrong.data_32={}
PicAnswerWrong.data_32.index			=	0	--id
PicAnswerWrong.data_32.option			=	1	--��ȷѡ��

--���
PicAnswerReward={}
PicAnswerReward.data_32={}
PicAnswerReward.data_32.index			=	0	--����

--���
TextAnswerReward={}
TextAnswerReward.data_32={}
TextAnswerReward.data_32.index			=	0	--����

--��ȡĿ�꽱��
GetTargetReward={}
GetTargetReward.data_32={}
GetTargetReward.data_32.index			=	0	--index

--��ȡĿ��ν���
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

