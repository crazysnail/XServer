--每日签到

local config = {}
config.switch_open		=	1			--开关
config.script_id		=	002601		--活动id
config.type_name		=	"每日签到"	--活动名字
config.ac_group			=	00001		--活动组id
config.group_name		=	"每日签到"	--活动组名字
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
config.cost				=	10			--补签钻石

--id,数量，奖励抽奖次数
config.items = {
{20233002,10,0},{29910002,100,0},{20236004,5,0},{20234005,5 ,1},
{20236003,20,0},{29910002,100,0},{20236011,2,0},{20234005,5 ,1},
{20233002,10,0},{29910002,100,0},{20236001,1,0},{20234005,5 ,1},
{20236003,20,0},{29910002,100,0},{20236004,5,0},{20234005,5 ,1},
{20233002,10,0},{29910002,100,0},{20236011,2,0},{20234005,10,1},
{20236003,20,0},{29910002,100,0},{20236001,1,0},{20234005,10,1},
{20233002,10,0},{29910002,100,0},{20236004,5,0},{20234005,10,1},
{20236003,20,0},{29910002,100,0},{20236011,2,0}
};

-----------------------------------------------for server---------------------------------------------
function x002601_OnUpdateAcrConfig(params)
	return ResultCode_ResultOK
end

function x002601_IsAcrActive(player)

	--开关
	if config.switch_open == 0 then 
		return ResultCode_Activity_NotStart
	end	
	
	
	return ResultCode_ResultOK
	
end

function x002601_OnResolve(player)
end


function x002601_OnUpdate(player)

	local config_check= 0
	if x002601_IsAcrActive(player) == ResultCode_ResultOK then
		config_check = 1
	end
	
	if config.items == nil then
		x000011_GAcrstatusUpdate(player,{["value"]=config.script_id,["config_check"]=0,["user_data"]=-1})
	else
		x000011_GAcrstatusUpdate(player,{["value"]=config.script_id,["config_check"]=config_check,["user_data"]=-1})	
	end		
end

function x002601_DoReward(player,params)
	
	if x002601_IsAcrActive(player)  ~= ResultCode_ResultOK then
		return ResultCode_Activity_NotStart
	end
	local index = params["index"]
	local dc_container = player:DataCellContainer()
	if dc_container == nil then
		return ResultCode_Invalid_Request
	end
	
		--背包检查
	if x000010_CheckBagOne(player,config.items[index+1]) == false then
		return ResultCode_InternalResult
	end
	
	local login_flag = dc_container:get_data_32(CellLogicType_AcRewardCell, g_acr_datacell_index.login_flag) 
	local flag_data=dc_container:get_data_32(CellLogicType_AcRewardCell, g_acr_datacell_index.sign_flag)
		
	local count = #config.items
	local bit_index = 0
	for i=0,count,1 do		
		if dc_container:check_bit_data_32(login_flag,i)== false then		
			break
		end		
		bit_index = i
	end
	
	--print(count,bit_index,index)
	
	if bit_index == index then			--正常签到
		if dc_container:check_bit_data_32(flag_data,index) then
			return ResultCode_RewardAlready
		end
	elseif index < bit_index then		--补签
	
		----月卡
		--if dc_container:check_bit_data(BitFlagCellIndex_MonthCardFlag) == false then
		--	return player:SendClientNotify("ac_notify_024",-1,1)
		--end	

		if dc_container:check_bit_data_32(login_flag,index) == false then
			return ResultCode_RewardConditionLimit
		end
		
		if dc_container:check_bit_data_32(flag_data,index) then
			return ResultCode_RewardAlready
		end
		--扣钻石
		if player:DelItemById(TokenType_Token_Crystal,config.cost,ItemDelLogType_DelType_Acr,2601) == false then
			return ResultCode_CrystalNotEnough
		end		
	else
		return ResultCode_Invalid_Request
	end
								
	--标记
	local new_flag_data = dc_container:set_bit_data_32(flag_data,index)
	dc_container:set_data_32(CellLogicType_AcRewardCell, g_acr_datacell_index.sign_flag,new_flag_data,true)
	
			
	--给道具
	local id = config.items[index+1][1]
	local count = config.items[index+1][2]
	local times = config.items[index+1][3]		

	if id == 20233002 and dc_container:check_bit_data(BitFlagCellIndex_MonthCardFlag) then	  --金币
		count = count *2
	elseif id == 20235001 and dc_container:check_bit_data(BitFlagCellIndex_LifeCardFlag) then --奇异之尘
		count = count *2
	end
	
	player:AddItemByIdWithNotify(id,count,ItemAddLogType_AddType_Acr,2601,1)

	--奖励抽奖次数
	if times>0 then
		dc_container:add_data_32(CellLogicType_AcRewardCell,g_acr_datacell_index.draw_count,1,true)
	end	
	
	return ResultCode_ResultOK
end

----------------------------------------------------------------------------
function x002601_OnRegisterFunction(proxy)
	
	if proxy ~= nil then
		proxy:RegFunc("xOnNewMonth"			,2601)	
	end
end

function x002601_xOnNewMonth(player,params)
	local dc_container = player:DataCellContainer()
	if dc_container == nil then
		return ResultCode_Invalid_Request
	end
	dc_container:set_data_32(CellLogicType_AcRewardCell, g_acr_datacell_index.sign_flag,0,true)
	dc_container:set_data_32(CellLogicType_AcRewardCell, g_acr_datacell_index.login_flag,0,true)
	
	return ResultCode_ResultOK
end

function x002601_xOnNewDay(player,params)
	local dc_container = player:DataCellContainer()
	if dc_container == nil then
		return ResultCode_Invalid_Request
	end
	local day = LuaMtTimerManager():GetDay()
	if day == 1 then
		dc_container:set_data_32(CellLogicType_AcRewardCell, g_acr_datacell_index.sign_flag,0,true)
		dc_container:set_data_32(CellLogicType_AcRewardCell, g_acr_datacell_index.login_flag,0,true)
	end	
	
	local login_flag = dc_container:get_data_32(CellLogicType_AcRewardCell, g_acr_datacell_index.login_flag) 
	local count = #config.items
	local bit_index = -1
	for i=0,count,1 do		
		if dc_container:check_bit_data_32(login_flag,i) == false then		
			break
		end		
		bit_index = i
	end
	
	bit_index = bit_index+1
	--print("ddddddddddddddddddddddddddd"..bit_index.." day"..day.." login_flag"..login_flag)
	local new_flag = dc_container:set_bit_data_32(login_flag,bit_index)
	--print("ddddddddddddddddddddddddddd"..bit_index.." day"..day.." new_flag"..new_flag)
	dc_container:set_data_32(CellLogicType_AcRewardCell, g_acr_datacell_index.login_flag,new_flag,true)
	
	x002601_OnUpdate(player)	
	
	return ResultCode_ResultOK
end
--------------------------------------------------------------for client-------------------------------
return config
