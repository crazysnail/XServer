--终生卡奖励

local config = {}
config.switch_open		=	1				--开关
config.script_id		=	002612			--活动id
config.type_name		=	"终生卡奖励"	--活动名字
config.ac_group			=	00001			--活动组id
config.group_name		=	"终生卡奖励"	--活动组名字
config.during_day		=	{1,1,1,1,1,1,1}		--每周开放计数
config.during_week		=	{1,1,1,1,1,1,1,1}	--循环周计数，循环周期是8周，自左向右，一个位表示一周(0关，1开)，8周结束后从新循环，英文窜表示特殊情况
config.reset_data		=	0			--重置玩家活动数据开关
config.depend_enum		=	-1			--活动前置依赖（不能相互引用，不能太多，会导致服务器调用栈溢出！）
config.begin_date		=	20000101	--开始时间
config.end_date			=	20991231	--结束时间
config.open_level		=	1			--出现等级
config.off_level		=	99			--消失等级
config.include_channels	=	"all"		--包含渠道列表
config.group_icon		=	"null"		--组icon
config.desc				=	"null"		--描述

--id,数量
config.items = {
{{29910002,80},{20125001,2},{20236003,100}}
};

config.once_reward = {
{20140002,4},{20140001,2},{20140005,2}
}
-----------------------------------------------for server---------------------------------------------
function x002612_OnUpdateAcrConfig(params)
	return ResultCode_ResultOK
end

function x002612_IsAcrActive(player)

	--开关
	if config.switch_open == 0 then 
		return ResultCode_Activity_NotStart
	end
		
	return ResultCode_ResultOK
	
end

function x002612_OnResolve(player)
end

function x002612_OnUpdate(player)

	local config_check= 0
	if x002612_IsAcrActive(player) == ResultCode_ResultOK then
		config_check = 1
	end
	if config.items == nil then
		x000011_GAcrstatusUpdate(player,{["value"]=config.script_id,["config_check"]=0,["user_data"]=-1})
	else
		x000011_GAcrstatusUpdate(player,{["value"]=config.script_id,["config_check"]=config_check,["user_data"]=-1})	
	end		
end

function x002612_DoReward(player,params)
	
	local index = 0
	local dc_container = player:DataCellContainer()
	if dc_container == nil then
		return ResultCode_Invalid_Request
	end
	
	--背包检查
	if x000010_CheckBag(player,1,config.items[index+1]) == false then
		return ResultCode_InternalResult
	end
	
	if dc_container:check_bit_data(BitFlagCellIndex_LifeCardFlag) == false then
		return ResultCode_RewardConditionLimit
	end		
	
	local flag_data=dc_container:get_data_32(CellLogicType_AcRewardCell, g_acr_datacell_index.life_card_flag)
	if dc_container:check_bit_data_32(flag_data,0) then
		return ResultCode_RewardAlready
	end

	local new_flag = dc_container:set_bit_data_32(flag_data,0)
	dc_container:set_data_32(CellLogicType_AcRewardCell, g_acr_datacell_index.life_card_flag, new_flag,true)
	
	--给道具
	local items = #config.items[index+1]
	for i=1,items,1 do
		local id = config.items[index+1][i][1]
		local count = config.items[index+1][i][2]
		player:AddItemByIdWithNotify(id,count,ItemAddLogType_AddType_Acr,2612,1)
	end

	return ResultCode_ResultOK
end

----------------------------------------------------------------------------
function x002612_OnRegisterFunction(proxy)
	
	if proxy ~= nil then
	end
end

function x002612_xOnNewDay(player,params)
	local dc_container = player:DataCellContainer()
	if dc_container == nil then
		return
	end
	
	if dc_container:check_bit_data(BitFlagCellIndex_LifeCardFlag) then
		dc_container:set_data_32(CellLogicType_AcRewardCell, g_acr_datacell_index.life_card_flag, 0,true)
		x002612_OnUpdate(player)	
	end		
	
	return ResultCode_ResultOK
end
--------------------------------------------------------------for client-------------------------------
return config
