--大富翁

local config = {}
config.switch_open		=	1			--开关
config.script_id		=	002602		--活动id
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


--如果配置多组item就以周为周期循环
--id,数量，权重
config.items = {
{20233002,5}	,{20139001,1}	,{20234005,1}		,{20236010,1},
{20236004,1}	,{29910002,10}	,{20233002,10}		,{20139002,1},
{20234005,3}	,{20236011,1}	,{20236004,3}		,{29910002,30},
};

-----------------------------------------------for server---------------------------------------------
function x002602_OnUpdateAcrConfig(params)
	return ResultCode_ResultOK
end
function x002602_IsAcrActive(player)


	--开关
	if config.switch_open == 0 then 
		return ResultCode_Activity_NotStart
	end	
		
	return ResultCode_ResultOK
	
end

function x002602_OnUpdate(player)

	local config_check= 0
	if x002602_IsAcrActive(player) == ResultCode_ResultOK then
		config_check = 1
	end
	if config.items == nil then
		x000011_GAcrstatusUpdate(player,{["value"]=config.script_id,["config_check"]=0,["user_data"]=-1})
	else
		x000011_GAcrstatusUpdate(player,{["value"]=config.script_id,["config_check"]=config_check,["user_data"]=-1})	
	end		
end


function x002602_DoReward(player,params)
	
	if x002602_IsAcrActive(player)  ~= ResultCode_ResultOK then
		return ResultCode_Activity_NotStart
	end
	
	local index = params["index"]
	local dc_container = player:DataCellContainer()
	if dc_container == nil then
		return ResultCode_Invalid_Request
	end

	--次数检查
	local times = dc_container:get_data_32(CellLogicType_AcRewardCell,g_acr_datacell_index.draw_count)
	if times <=0 then
		return ResultCode_TimesLimit 
	end		

	--背包检查
	if player:BagLeftCapacity(ContainerType_Layer_EquipAndItemBag,1) == false then
		return ResultCode_BagIsFull
	end
	
	local seed = LuaServer():RandomNum(1,6)
	local last_index = dc_container:get_data_32(CellLogicType_AcRewardCell,g_acr_datacell_index.draw_index)
	local lcindex = math.fmod(last_index+seed,12)
	dc_container:set_data_32(CellLogicType_AcRewardCell,g_acr_datacell_index.draw_index,lcindex,true)
	--扣抽奖次数
	dc_container:add_data_32(CellLogicType_AcRewardCell,g_acr_datacell_index.draw_count,-1,true)
			
	--给道具
	local id = config.items[lcindex+1][1]
	local count = config.items[lcindex+1][2]	
	player:AddItemByIdWithoutNotify(id,count,ItemAddLogType_AddType_Acr,2602,1)
	
	--print("lcindex"..lcindex.." last_index"..last_index.." seed"..seed)
	
	--发标记
	player:SendLuaPacket("msg_acr_reply",{config.script_id,lcindex,id,count,seed},{},{})

	return ResultCode_ResultOK
end

---------------------------------------------------
function x002602_OnRegisterFunction(proxy)
	
	if proxy ~= nil then
	end
end

function x002602_xOnNewDay(player,params)
	x002602_OnUpdate(player)	
	return ResultCode_ResultOK
end

--------------------------------------------------------------for client-------------------------------
return config