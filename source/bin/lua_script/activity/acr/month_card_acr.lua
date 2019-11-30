--月卡奖励

local config = {}
config.switch_open		=	1			--开关
config.script_id		=	002613		--活动id
config.type_name		=	"月卡奖励"	--活动名字
config.ac_group			=	00001		--活动组id
config.group_name		=	"月卡奖励"	--活动组名字
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
{{29910002,100},{20236010,2},{20236003,100}}
};

-----------------------------------------------for server---------------------------------------------
function x002613_OnUpdateAcrConfig(params)
	return ResultCode_ResultOK
end

function x002613_IsAcrActive(player)

	--开关
	if config.switch_open == 0 then 
		return ResultCode_Activity_NotStart
	end
		
	return ResultCode_ResultOK
	
end


function x002613_OnResolve(player)

	local config_check= 0
	if x002613_IsAcrActive(player) == ResultCode_ResultOK then
		config_check = 1
	end
	if config.items == nil then
		x000011_GAcrstatusUpdate(player,{["value"]=config.script_id,["config_check"]=0,["user_data"]=-1})
	else
		x000011_GAcrstatusUpdate(player,{["value"]=config.script_id,["config_check"]=config_check,["user_data"]=-1})	
	end		
end

function x002613_OnUpdate(player)

	local config_check= 0
	if x002613_IsAcrActive(player) == ResultCode_ResultOK then
		config_check = 1
	end
	if config.items == nil then
		x000011_GAcrstatusUpdate(player,{["value"]=config.script_id,["config_check"]=0,["user_data"]=-1})
	else
		x000011_GAcrstatusUpdate(player,{["value"]=config.script_id,["config_check"]=config_check,["user_data"]=-1})	
	end		
end

function x002613_DoReward(player,params)
	
	local index = 0
	local dc_container = player:DataCellContainer()
	if dc_container == nil then
		return ResultCode_Invalid_Request
	end
	
	--背包检查
	if x000010_CheckBag(player,1,config.items[index+1]) == false then
		return ResultCode_InternalResult
	end
	
	if dc_container:check_bit_data(BitFlagCellIndex_MonthCardFlag) == false then
		return ResultCode_RewardConditionLimit
	end		
	
	local flag_data=dc_container:get_data_32(CellLogicType_AcRewardCell, g_acr_datacell_index.month_card_flag)
	local flag = dc_container:get_high_16_bit(flag_data)
	local count = dc_container:get_low_16_bit(flag_data)
		
	if dc_container:check_bit_data_32(flag,0) then
		return ResultCode_RewardAlready
	end

	local new_flag = dc_container:set_bit_data_32(flag,0)
	local new_data = dc_container:bind_bit_data_32(new_flag,count)
	dc_container:set_data_32(CellLogicType_AcRewardCell, g_acr_datacell_index.month_card_flag, new_data,true)
				
	--给道具
	local items = #config.items[index+1]
	for i=1,items,1 do
		local id = config.items[index+1][i][1]
		local count = config.items[index+1][i][2]
		player:AddItemByIdWithNotify(id,count,ItemAddLogType_AddType_Acr,2613,1)
	end

	return ResultCode_ResultOK
end

----------------------------------------------------------------------------
function x002613_OnRegisterFunction(proxy)
	
	if proxy ~= nil then
	end
end

function x002613_xOnNewDay(player,params)
	local dc_container = player:DataCellContainer()
	if dc_container == nil then
		return
	end
	
	if dc_container:check_bit_data(BitFlagCellIndex_MonthCardFlag) then

		local flag_data=dc_container:get_data_32(CellLogicType_AcRewardCell, g_acr_datacell_index.month_card_flag)
		local flag = dc_container:get_high_16_bit(flag_data)
		local count = dc_container:get_low_16_bit(flag_data)			
		local new_flag = dc_container:reset_bit_data_32(flag,0) 		
		if count >= 30 then
			dc_container:set_data_32(CellLogicType_AcRewardCell, g_acr_datacell_index.month_card_flag, 0,true)
			dc_container:reset_bit_data(BitFlagCellIndex_MonthCardFlag,true)
		else
			count = count +1
			local new_data = dc_container:bind_bit_data_32(new_flag,count)
			dc_container:set_data_32(CellLogicType_AcRewardCell, g_acr_datacell_index.month_card_flag, new_data,true)
		end
		
		x002613_OnUpdate(player)	
	end
	
	return ResultCode_ResultOK
end
--------------------------------------------------------------for client-------------------------------
return config
