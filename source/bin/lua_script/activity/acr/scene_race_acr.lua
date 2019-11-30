--通关冲刺

local config = {}
config.switch_open		=	1			--开关
config.script_id		=	002609		--活动id
config.type_name		=	"通关冲刺"	--活动名字
config.ac_group			=	00004		--活动组id
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


--联盟关卡/部落关卡,id,数量
config.items = {
{60105,60115,{20236004,10}},
{60208,60219,{20236004,10}},
{60308,60308,{20236004,10}},
{60406,60406,{20236004,15}},
{60416,60416,{20236004,15}},
{60505,60505,{20236004,20}},
{60606,60606,{20236004,20}},
{60710,60710,{20236004,25}},
{60809,60809,{20236004,25}},
{60827,60827,{20236004,30}},
{60855,60855,{20236004,30}},
{60926,60926,{20236004,35}},
{61017,61017,{20236004,35}},
{61117,61117,{20236004,40}},
{61217,61217,{20236004,40}},
{61308,61308,{20236004,50}}
};

-----------------------------------------------for server---------------------------------------------
function x002609_OnUpdateAcrConfig(params)
	return ResultCode_ResultOK
end

function x002609_IsAcrActive(player)

	--开关
	if config.switch_open == 0 then 
		return ResultCode_Activity_NotStart
	end	
	
	return ResultCode_ResultOK
	
end



function x002609_OnResolve(player)
	local dc_container = player:DataCellContainer()
	if dc_container == nil then		
		return
	end
	local config_check= 0
	if x002609_IsAcrActive(player) == ResultCode_ResultOK then
		config_check = 1
	end
	if config_check == 1 then	
		local count = #config.items
		local camp = player:GetCamp()
		for index=0,count-1,1 do 
			local flag_data=dc_container:get_data_32(CellLogicType_AcRewardCell, g_acr_datacell_index.scene_race_flag)
			if dc_container:check_bit_data_32(flag_data,index) == false then		
				local reward_stage = config.items[index+1][camp+1]	
				local now_stage=dc_container:get_data_32(CellLogicType_AcRewardCell, g_acr_datacell_index.scene_race_data)
				if reward_stage <= now_stage then
					local new_data = dc_container:set_bit_data_32(flag_data,index)
					dc_container:set_data_32(CellLogicType_AcRewardCell, g_acr_datacell_index.scene_race_flag,new_data,true)
				end
			end
		end
	end	
end

function x002609_OnUpdate(player)

	local config_check= 0
	if x002609_IsAcrActive(player) == ResultCode_ResultOK then
		config_check = 1
	end
	if config.items == nil then
		x000011_GAcrstatusUpdate(player,{["value"]=config.script_id,["config_check"]=0,["user_data"]=-1})
	else
		x000011_GAcrstatusUpdate(player,{["value"]=config.script_id,["config_check"]=config_check,["user_data"]=-1})	
	end		
	
end

function x002609_DoReward(player,params)
	
	local index = params["index"]
	local dc_container = player:DataCellContainer()
	if dc_container == nil then
		return ResultCode_Invalid_Request
	end

		--背包检查
	if x000010_CheckBag(player,3,config.items[index+1]) == false then
	--if player:BagLeftCapacity(ContainerType_Layer_EquipAndItemBag,#config.items[index+1]-2) == false  then
		return ResultCode_InternalResult
	end
	
	local count = #config.items
	if index>=count then
		return ResultCode_Invalid_Request
	end
	
	local flag_data=dc_container:get_data_32(CellLogicType_AcRewardCell, g_acr_datacell_index.scene_race_flag)
	if dc_container:check_bit_data_32(flag_data,index)== false then		
		return ResultCode_RewardConditionLimit
	end
	
	--
	if dc_container:check_bit_data_32(flag_data,index+16)== true then		
		return ResultCode_RewardAlready
	end
	
	--标记
	local new_flag_data = dc_container:set_bit_data_32(flag_data,index+16)
	dc_container:set_data_32(CellLogicType_AcRewardCell, g_acr_datacell_index.scene_race_flag,new_flag_data,true)
			
	--给道具
	local items = #config.items[index+1]
	for i=3,items,1 do
		local id = config.items[index+1][i][1]
		local count = config.items[index+1][i][2]
		player:AddItemByIdWithNotify(id,count,ItemAddLogType_AddType_Acr,2609,1)
	end
	
	return ResultCode_ResultOK
end

---------------------------------------------------
function x002609_OnRegisterFunction(proxy)
	
	if proxy ~= nil then
		proxy:RegFunc("xPlayerRaidStage"	,2609)		
	end
end

function x002609_xOnNewDay(player,params)
	x002609_OnUpdate(player)	
	return ResultCode_ResultOK
end

function x002609_xPlayerRaidStage(player,params)
	local dc_container = player:DataCellContainer()
	if dc_container ~= nil then		
		local now_stage=dc_container:get_data_32(CellLogicType_AcRewardCell, g_acr_datacell_index.scene_race_data)
		if params["value"]>now_stage then
			dc_container:set_data_32(CellLogicType_AcRewardCell, g_acr_datacell_index.scene_race_data,params["value"],true)
		end
	end
	x002609_OnResolve(player)	
	return ResultCode_ResultOK
end

--------------------------------------------------------------for client-------------------------------
return config
