--首充双倍

local config = {}
config.switch_open		=	1			--开关
config.script_id		=	002619		--活动id
config.type_name		=	"首充双倍"	--活动名字
config.ac_group			=	00002		--活动组id
config.group_name		=	"充值活动"	--活动组名字
config.during_day		=	{1,1,1,1,1,1,1}		--每周开放计数
config.during_week		=	{1,1,1,1,1,1,1,1}	--循环周计数，循环周期是8周，自左向右，一个位表示一周(0关，1开)，8周结束后从新循环，英文窜表示特殊情况
config.reset_data		=	1			--重置玩家活动数据开关
config.depend_enum		=	-1			--活动前置依赖（不能相互引用，不能太多，会导致服务器调用栈溢出！）
config.begin_date		=	20000101	--开始时间
config.end_date			=	20991231	--结束时间
config.open_level		=	1			--出现等级
config.off_level		=	99			--消失等级
config.include_channels	=	"all"		--包含渠道列表
config.group_icon		=	"null"		--组icon
config.desc				=	"null"		--描述


-----------------------------------------------for server---------------------------------------------
function x002619_OnUpdateAcrConfig(params)
	return ResultCode_ResultOK
end
function x002619_IsAcrActive(player)

	--开关
	if config.switch_open == 0 then 
		return ResultCode_Activity_NotStart
	end
	
	if x002619_OnCheckClose(player) then
		return ResultCode_Activity_NotStart
	else
		return ResultCode_ResultOK	
	end
	
end

function x002619_OnUpdate(player)
	local config_check= 0
	if x002619_IsAcrActive(player) == ResultCode_ResultOK then
		config_check = 1
	end
	if config.items == nil then
		x000011_GAcrstatusUpdate(player,{["value"]=config.script_id,["config_check"]=0,["user_data"]=-1})
	else
		x000011_GAcrstatusUpdate(player,{["value"]=config.script_id,["config_check"]=config_check,["user_data"]=-1})	
	end		
end

function x002619_DoReward(player,params)	
	return ResultCode_ResultOK	
end


---------------------------------------------------
function x002619_OnRegisterFunction(proxy)
	
	if proxy ~= nil then
		proxy:RegFunc("xOnCharge"			,2619)		
	
	end
end


function x002619_xOnNewDay(player,params)
	x002619_OnUpdate(player)
	return ResultCode_ResultOK
end
function x002619_xOnCharge(player,params)	
	x002619_OnUpdate(player)
	return ResultCode_ResultOK
end

function x002619_OnCheckClose(player)
	--标记检查
	local dc_container = player:DataCellContainer()
	if dc_container == nil then
		return true
	end
	local flag_data = dc_container:get_data_32(CellLogicType_AcRewardCell,g_acr_datacell_index.charge_double_flag)	
	local all_done = true
	for i=2,8,1 do
		if dc_container:check_bit_data_32(flag_data,i) == false then
			all_done = false
			break
		end	
	end
	return all_done
end

--------------------------------------------------------------for client-------------------------------
return config