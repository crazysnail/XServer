--首次充值

local config = {}
config.switch_open		=	1			--开关
config.script_id		=	002603		--活动id
config.type_name		=	"首充奖励"	--活动名字
config.ac_group			=	00002		--活动组id
config.group_name		=	"充值活动"	--活动组名字
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
{6,888,{{28810002,1},{10520001,1},{30102002,2},{20235001,20}}},
--{30,1888,{{28810002,1},{10520001,1},{10530001,1},{30102003,2}}},
--{98,6666,{{28810002,1},{10520001,1},{10530001,1},{30102004,3},{20236024,1},{20234002,50},{20233001,50}}}
};

-----------------------------------------------for server---------------------------------------------
function x002603_OnUpdateAcrConfig(params)
	return ResultCode_ResultOK
end
function x002603_IsAcrActive(player)

	--开关
	if config.switch_open == 0 then 
		return ResultCode_Activity_NotStart
	end
		
	local dc_container = player:DataCellContainer()
	if dc_container == nil then
		return ResultCode_Activity_NotStart
	end
    
	--任一一档领过都不行
	local flag_data = dc_container:get_data_32(CellLogicType_AcRewardCell,g_acr_datacell_index.charge_flag)
	for i=0,#config.items-1,1 do 
		if dc_container:check_bit_data_32(flag_data,16+i) then
			return ResultCode_Activity_NotStart
		end
	end
		
	return ResultCode_ResultOK
	
end


function x002603_OnResolve(player)
end

function x002603_OnUpdate(player)

	local config_check= 0
	if x002603_IsAcrActive(player) == ResultCode_ResultOK then
		config_check = 1
	end
	
	if config.items == nil then
		x000011_GAcrstatusUpdate(player,{["value"]=config.script_id,["config_check"]=0,["user_data"]=-1})
	else
		x000011_GAcrstatusUpdate(player,{["value"]=config.script_id,["config_check"]=config_check,["user_data"]=-1})	
	end		
end

function x002603_DoReward(player,params)


	local dc_container = player:DataCellContainer()
	if dc_container == nil then
		return ResultCode_Invalid_Request
	end
	
	local index =0
		--背包检查	
	if x000010_CheckBag(player,1,config.items[index+1][3]) == false then
		return ResultCode_InternalResult
	end
	
	--标记检查
	local flag_data = dc_container:get_data_32(CellLogicType_AcRewardCell,g_acr_datacell_index.charge_flag)
	if dc_container:check_bit_data_32(flag_data,0) == false then
		return ResultCode_RewardConditionLimit
	end	
	if dc_container:check_bit_data_32(flag_data,16) then
		return ResultCode_RewardAlready
	end
		
	--打标记
	local new_flag = dc_container:set_bit_data_32(flag_data,16)
	dc_container:set_data_32(CellLogicType_AcRewardCell, g_acr_datacell_index.charge_flag,new_flag,true)	
			
			
	--给道具
	for k,v in pairs(config.items[index+1][3])	do
		player:AddItemByIdWithNotify(v[1],v[2],ItemAddLogType_AddType_Acr,2603,1)
	end

	--更新状态
	x002603_OnUpdate(player)
	
	
	return ResultCode_ResultOK
	
end


---------------------------------------------------
function x002603_OnRegisterFunction(proxy)
	
	if proxy ~= nil then
		proxy:RegFunc("xOnCharge"			,2603)	
	end
end


function x002603_xOnNewDay(player,params)
	x002603_OnUpdate(player)	
	return ResultCode_ResultOK
end

function x002603_xOnCharge(player,params)

	local index = tonumber(params["index"])
	--基金，月卡，终生卡不激活
	if index == 0 or index == 1 or index == 6 then
		return ResultCode_InternalResult
	end
	
	local dc_container = player:DataCellContainer()
	if dc_container == nil then
		return ResultCode_Invalid_Request
	end
	local flag_data = dc_container:get_data_32(CellLogicType_AcRewardCell,g_acr_datacell_index.charge_flag)
	if dc_container:check_bit_data_32(flag_data,16) or dc_container:check_bit_data_32(flag_data,0) then
		return ResultCode_ResultOK
	end
	
	local new_flag = dc_container:set_bit_data_32(flag_data,0)
	dc_container:set_data_32(CellLogicType_AcRewardCell, g_acr_datacell_index.charge_flag,new_flag,true)	
	
	x002603_OnUpdate(player)	
	
	
	return ResultCode_ResultOK
end

--------------------------------------------------------------for client-------------------------------
return config