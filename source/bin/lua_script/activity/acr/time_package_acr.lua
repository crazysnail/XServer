--限时礼包

local config = {}
config.switch_open		=	1			--开关
config.script_id		=	002623		--活动id
config.type_name		=	"限时礼包"	--活动名字
config.ac_group			=	00005		--活动组id
config.group_name		=	"开服活动"	--活动组名字
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
config.triger_time		=	1800		--触发在线时长
config.live_time		=	7200		--礼包持续时长

--唯一索引 {道具id，数量，价格}
config.items = {
{1001,{20121001,5	,8}},
{1002,{20121002,5	,40}},     
{1003,{20121003,1	,40}},
{1004,{20233002,30	,30}},
{1005,{20233002,60	,60}},     
{1006,{20235001,100	,60}},
{1007,{20236001,1	,200}},
{1008,{20139001,5	,20}},     
{1009,{20139002,2	,20}},
{1010,{20235002,100	,300}},
{1011,{20235002,50	,150}},   
{1012,{20139001,10	,50}},
{1013,{20139002,10	,150}},    
--魂魄

};

--最小等级，最高等级, 累计最低充值额，累计最高充值额，{组}
config.conditions = {
{1	,29	,0	,648	,{1001, 1002}},
{30	,60	,0	,648	,{1001, 1002, 1002}},
{1	,60	,648,999999	,{1001, 1002, 1002, 1002}},
};

-----------------------------------------------for server---------------------------------------------
function x002623_OnUpdateAcrConfig(params)
	return ResultCode_ResultOK
end

function x002623_IsAcrActive(player)

	--开关
	if config.switch_open == 0 then 
		return ResultCode_Activity_NotStart
	end
	
	if x002623_GetDayOnlineTime(player) < config.triger_time then
		return ResultCode_Activity_NotStart
	end
	
	return ResultCode_ResultOK
	
end

function x002623_GetDayOnlineTime(player)
	local dc_container = player:DataCellContainer()
	if dc_container == nil then
		return 0
	end
	
	local dbinfo = player:GetDBPlayerInfo()
	local login_time = dbinfo:last_logout_time()
	local online_time = LuaMtTimerManager():DiffTimeToNow(login_time)		
	local zerotonow = LuaMtTimerManager():ZeorToNow()
	
	--跨天的情况
	if zerotonow < online_time then
		online_time = zerotonow
	end
		--重置每日在线时间
	local day_online_time = dc_container:get_data_32(CellLogicType_ActionData, g_action_data_index.day_online_time)
	
	--print("online_time"..online_time.." day_online_time"..day_online_time.." zerotonow"..zerotonow)
	
	return online_time+day_online_time
end

function x002623_GenByGroupRandom(player,dc_container,new_data)
	local temp ={}
	for i=0,#config.items-1,1 do 
		temp[i+1]=i
	end

	for i=1,8,1 do
		local seed = LuaServer():RandomNum(1,#temp)		
		new_data= dc_container:set_bit_data_32(new_data,temp[seed])
		table.remove(temp, seed)
	end
	dc_container:set_data_32(CellLogicType_AcRewardCell, g_acr_datacell_index.time_package_flag,new_data,true)	

end

--function x002623_GenByGroup(player,dc_container)
--	local group={}
--	local all_charge = player:GetAllCharge()
--	
--	local online_time = x002623_GetDayOnlineTime(player,dc_container)
--	
--	for key,iter in pairs(config.conditions) do 
--		if online_time >= config.triger_time then
--			if all_charge >= iter[3] and all_charge < iter[4] and level>= iter[1] and level <= iter[2] then
--				group= iter[5]					
--				--活动开启，开始计时
--				local begin_time = LuaMtTimerManager():CurrentDate()
--				player:SetParams32("time_package_begin",begin_time)
--				dc_container:set_data_32(CellLogicType_AcRewardCell, g_acr_datacell_index.time_package_time,0,true)
--				--print("bingo")
--				break
--			end
--		end
--	end
--	
--	local new_data = dc_container:get_data_32(CellLogicType_AcRewardCell, g_acr_datacell_index.time_package_flag)
--	for k,v in pairs(group) do				
--		for i=0,#config.items-1,1 do
--			if v == config.items[i+1][1] then						
--				new_data= dc_container:set_bit_data_32(new_data,i)
--				--print("666666666666666666666".."offset:"..i)	
--			end
--		end
--	end		
--	dc_container:set_data_32(CellLogicType_AcRewardCell, g_acr_datacell_index.time_package_flag,new_data,true)		
--	
--	return new_data
--end

function x002623_OnUpdate(player)
	local config_check= 0
	if x002623_IsAcrActive(player) == ResultCode_ResultOK then
		config_check = 1
	end	
	
	--还需要检查时间
	local dc_container = player:DataCellContainer()
	local left_time = dc_container:get_data_32(CellLogicType_AcRewardCell, g_acr_datacell_index.time_package_time)		
	if left_time <= 0 then
		config_check= 0
	end
		
	if config.items == nil then
		x000011_GAcrstatusUpdate(player,{["value"]=config.script_id,["config_check"]=0,["user_data"]=-1})
	else
		x000011_GAcrstatusUpdate(player,{["value"]=config.script_id,["config_check"]=config_check,["user_data"]=-1})	
	end		
end

function x002623_DoReward(player,params)
	if x002623_IsAcrActive(player) ~= ResultCode_ResultOK then
		return ResultCode_Activity_NotStart
	end
	
	--print("index"..index.." offset"..offset)
	
	local index = tonumber(params["index"])
	local dc_container = player:DataCellContainer()
	if dc_container == nil then
		return ResultCode_Invalid_Request
	end
	
	--背包检查
	if player:BagLeftCapacity(ContainerType_Layer_ItemBag,1) == false then
		return ResultCode_BagIsFull
	end


	local offset = -1
	local cost=0
	for i=0,#config.items-1,1 do
		if index == config.items[i+1][1] then
			offset = i
			cost = config.items[i+1][2][3]
			break
		end
	end
	
	--print("index"..index.." offset"..offset)
	if offset == -1 then
		return ResultCode_Invalid_Request
	end
	
	local flag_data=dc_container:get_data_32(CellLogicType_AcRewardCell, g_acr_datacell_index.time_package_flag)
	if dc_container:check_bit_data_32(flag_data,offset)== false then		
		return player:SendClientNotify("BuyConditionLimit",-1,-1)
	end

	if dc_container:check_bit_data_32(flag_data,offset+16)== true then		
		return player:SendClientNotify("BuyAlready",-1,-1)
	end
	
	--消耗
	if player:DelItemById(TokenType_Token_Crystal, cost, ItemDelLogType_DelType_Acr, 2623) == false then
		return ResultCode_CrystalNotEnough;
	end	

	--标记
	local new_flag_data = dc_container:set_bit_data_32(flag_data,offset+16)
	dc_container:set_data_32(CellLogicType_AcRewardCell, g_acr_datacell_index.time_package_flag,new_flag_data,true)
				
	--给道具
	local id = config.items[offset+1][2][1]
	local count = config.items[offset+1][2][2]
	player:AddItemByIdWithNotify(id,count,ItemAddLogType_AddType_Acr,2623,1)

	return ResultCode_ResultOK
end

---------------------------------------------------
function x002623_OnRegisterFunction(proxy)
	
	if proxy ~= nil then
	end
end

function x002623_xOnNewDay(player,params)

	local dc_container = player:DataCellContainer()
	if dc_container == nil then
		return ResultCode_Invalid_Request
	end
	
	dc_container:set_data_32(CellLogicType_AcRewardCell, g_acr_datacell_index.time_package_flag,0,true)
	dc_container:set_data_32(CellLogicType_AcRewardCell, g_acr_datacell_index.time_package_time,0,true)

	x002623_OnUpdate(player)	
	return ResultCode_ResultOK
end


function x002623_xOnBigTick(player, params)

	if x002623_IsAcrActive(player) ~= ResultCode_ResultOK then
		return ResultCode_InternalResult
	end

	local dc_container =player:DataCellContainer()
	if dc_container == nil then
		return ResultCode_InternalResult
	end
	
	local left_time = dc_container:get_data_32(CellLogicType_AcRewardCell, g_acr_datacell_index.time_package_time)		
	if left_time < 0 then
		--print("left_time"..left_time)
		return	ResultCode_InternalResult
	else	
		--print("left_time"..left_time)
		--触发规则处理
		local flag_data=dc_container:get_data_32(CellLogicType_AcRewardCell, g_acr_datacell_index.time_package_flag)
		if flag_data == 0 and left_time == 0 then
			x002623_GenByGroupRandom(player,dc_container,flag_data)
			dc_container:set_data_32(CellLogicType_AcRewardCell, g_acr_datacell_index.time_package_time,config.live_time,true)
			
			--print("left_time"..left_time)
			x000011_GAcrstatusUpdate(player,{["value"]=config.script_id,["config_check"]=1,["user_data"]=-1})
			
			return ResultCode_ResultOK
		end
		
		dc_container:add_data_32(CellLogicType_AcRewardCell, g_acr_datacell_index.time_package_time,-10,false)
		if left_time-10 <0 then
			--print("left_time"..left_time)
			x000011_GAcrstatusUpdate(player,{["value"]=config.script_id,["config_check"]=0,["user_data"]=-1})	
		end			
	end
	
end


function x002623_TimeUpdate(player)

	local dc_container = player:DataCellContainer()
	if dc_container == nil then
		return ResultCode_Invalid_Request
	end
	
	local new = dc_container:get_data_32(CellLogicType_AcRewardCell, g_acr_datacell_index.time_package_time)	
	dc_container:set_data_32(CellLogicType_AcRewardCell, g_acr_datacell_index.time_package_time,new,true)
	
	--print("ddddddddddddddddddddddddddd")

	return ResultCode_ResultOK
end


--------------------------------------------------------------for client-------------------------------
return config
