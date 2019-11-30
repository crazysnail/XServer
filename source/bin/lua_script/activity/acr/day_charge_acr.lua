--单日充值

local config = {}
config.switch_open		=	0			--开关
config.script_id		=	002618		--活动id
config.type_name		=	"单日充值"	--活动名字
config.ac_group			=	00005		--活动组id
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


--钱,id,数量
config.items = {
{6,	 {20236010,2 } },
{50, {20139003,2 } },
};

-----------------------------------------------for server---------------------------------------------
function x002618_OnUpdateAcrConfig(params)
	return ResultCode_ResultOK
end

function x002618_IsAcrActive(player)

	--开关
	if config.switch_open == 0 then 
		return ResultCode_Activity_NotStart
	end	
	
	local ds = LuaActivityManager():GetDateStruct()
	if ds == nil then
		return ResultCode_Activity_NotStart
	end
	
	--周循环
	local week_index = math.fmod(ds.cur_week,8)
	if config.during_week[week_index+1] == 0 then
		return ResultCode_Activity_NotStart
	end
	
	--每天
	if config.during_day[ds.week_day+1] == 0 then
		return ResultCode_Activity_NotStart
	end
		
	--日期
	if ds.cur_date<config.begin_date or ds.cur_date > config.end_date then
		return ResultCode_Activity_NotStart
	end
	
	
	return ResultCode_ResultOK
	
end

function x002618_OnResolve(player)	
	local dc_container = player:DataCellContainer()
	if dc_container == nil then		
		return
	end
	local config_check= 0
	if x002618_IsAcrActive(player) == ResultCode_ResultOK then
		config_check = 1
	end
	if config_check == 1 then	
		local count = #config.items
		local check_count = dc_container:get_data_32(CellLogicType_ActionData, g_action_data_index.day_charge_count)
		for index=0,count-1,1 do 
			local flag_data=dc_container:get_data_32(CellLogicType_AcRewardCell, g_acr_datacell_index.day_charge_flag)
			if dc_container:check_bit_data_32(flag_data,index) == false and check_count >= config.items[index+1][1] then
				local new_data = dc_container:set_bit_data_32(flag_data,index)
				dc_container:set_data_32(CellLogicType_AcRewardCell, g_acr_datacell_index.day_charge_flag,new_data,true)
			end
		end
	end

end

function x002618_OnUpdate(player)	
	local config_check= 0
	if x002618_IsAcrActive(player) == ResultCode_ResultOK then
		config_check = 1
	end
	if config.items == nil then
		x000011_GAcrstatusUpdate(player,{["value"]=config.script_id,["config_check"]=0,["user_data"]=-1})
	else
		x000011_GAcrstatusUpdate(player,{["value"]=config.script_id,["config_check"]=config_check,["user_data"]=-1})	
	end		

end

function x002618_DoReward(player,params)
	
	if x002618_IsAcrActive(player)  ~= ResultCode_ResultOK then
		return ResultCode_Activity_NotStart
	end
	local index = params["index"]
	local dc_container = player:DataCellContainer()
	if dc_container == nil then
		return ResultCode_Invalid_Request
	end
	
	--背包检查
	if x000010_CheckBag(player,2,config.items[index+1]) == false then
		return ResultCode_InternalResult
	end

	local count = #config.items
	if index>=count then
		return ResultCode_Invalid_Request
	end
	
	local flag_data=dc_container:get_data_32(CellLogicType_AcRewardCell, g_acr_datacell_index.day_charge_flag)
	if dc_container:check_bit_data_32(flag_data,index)== false then		
		return ResultCode_RewardConditionLimit
	end

	if dc_container:check_bit_data_32(flag_data,index+16)== true then		
		return ResultCode_RewardAlready
	end

	--标记
	local new_flag_data = dc_container:set_bit_data_32(flag_data,index+16)
	dc_container:set_data_32(CellLogicType_AcRewardCell, g_acr_datacell_index.day_charge_flag,new_flag_data,true)
				
	--给道具
	local items = #config.items[index+1]
	for i=2,items,1 do
		local id = config.items[index+1][i][1]
		local count = config.items[index+1][i][2]
		player:AddItemByIdWithNotify(id,count,ItemAddLogType_AddType_Acr,2618,1)
	end
	
	return ResultCode_ResultOK
end

---------------------------------------------------
function x002618_OnRegisterFunction(proxy)
	
	if proxy ~= nil then
		proxy:RegFunc("xOnCharge"			,2618)	
	end
end


function x002618_xOnCharge(player,params)
	x002618_OnResolve(player)	
	return ResultCode_ResultOK
end
function x002618_xOnNewDay(player,params)
	local dc_container = player:DataCellContainer()
	if dc_container ~= nil then	
		dc_container:set_data_32(CellLogicType_AcRewardCell, g_acr_datacell_index.day_charge_flag,0,true)
	end

	x002618_OnUpdate(player)		
	return ResultCode_ResultOK
end

--------------------------------------------------------------for client-------------------------------
return config
