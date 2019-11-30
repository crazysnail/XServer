--限时抽卡

local config = {}
config.switch_open		=	1			--开关
config.script_id		=	002622		--活动id
config.type_name		=	"限时抽卡"	--活动名字
config.ac_group			=	00003		--活动组id
config.group_name		=	"限时抽卡"	--活动组名字
config.during_day		=	{1,1,1,1,1,1,1}		--每周开放计数
config.during_week		=	{1,1,1,1,1,1,1,1}	--循环周计数，循环周期是8周，自左向右，一个位表示一周(0关，1开)，8周结束后从新循环，英文窜表示特殊情况
config.reset_data		=	0			--重置玩家活动数据开关
config.depend_enum		=	-1			--活动前置依赖（不能相互引用，不能太多，会导致服务器调用栈溢出！）
config.normal_begin_days=	8			--
config.normal_end_days	=	9			--
config.normal_begin_time=	0			--
config.normal_end_time	=	235959		--
config.open_level		=	1			--出现等级
config.off_level		=	99			--消失等级
config.include_channels	=	"all"		--包含渠道列表
config.group_icon		=	"null"		--组icon
config.desc				=	"null"		--描述

config.hero_id			= 	28810026	--活动英雄
config.part_id			=	20234026	--英雄碎片

config.hero_id1			= 	28810019	--活动英雄
config.part_id1			= 	20234019	--英雄碎片
config.hero_id2			= 	28810017	--活动英雄
config.part_id2			= 	20234017	--英雄碎片

config.hero_id3			= 	28810002	--活动英雄
config.part_id3			= 	20234002	--英雄碎片

config.big_icon			= 	"atlas_time_limit_sommon+dadajie"

	
--附加group, id, 数量，权重
config.items = {}
config.items[1] ={

{20121002,1,1500 },
{20121003,1,1500 },
{20233004,1,1500 },
{20233005,1,1500 },
{20121002,2,1000 },
{20121003,2,1000 },
{20233004,2,1000 },
{20233005,2,1000 },

{config.hero_id1,1,5 	 },
{config.part_id1,4,25 	 },
{config.hero_id2,1,5 	 },
{config.part_id2,4,25 	 },
{config.hero_id3,1,2 	 },
{config.part_id3,4,20 	 },

{config.hero_id,1,1 	 },
{config.part_id,4,10 	 },
{20233001,4,10 	 },
}

config.items[2] ={

{config.hero_id1,1,600	},
{config.part_id1,4,3000},
{config.hero_id2,1,600	},
{config.part_id2,4,3000},
{config.hero_id3,1,100	},
{config.part_id3,4,1000},

{config.hero_id,1,50	},
{config.part_id,4,500	},

{20233001,4,1200},
}

config.items[3] ={
{config.hero_id,1,10000}
}
-----------------------------------------------for server---------------------------------------------
function x002622_OnUpdateAcrConfig(params)
	return ResultCode_ResultOK
end

function x002622_IsAcrActive(player)

	--开关
	if config.switch_open == 0 then 
		return ResultCode_Activity_NotStart
	end	
	
	local ds = LuaActivityManager():GetDateStruct()
	if ds == nil then
		return ResultCode_Activity_NotStart
	end
	
	if config.normal_begin_days >=0 and config.normal_end_days >= 0 then
		if ds.past_day >= config.normal_begin_days and ds.past_day <= config.normal_end_days then
			if config.normal_begin_time >= 0 and config.normal_end_time >= 0 then
				local curtime = ds.cur_hour*10000+ds.cur_min*100
				if curtime >= config.normal_begin_time and curtime <= config.normal_end_time then
					return ResultCode_ResultOK
				end
			end
		end
	end
	
	
	return ResultCode_Activity_NotStart
	
end


function x002622_OnResolve(player)
end

function x002622_OnUpdate(player)

	local config_check= 0
	if x002622_IsAcrActive(player) == ResultCode_ResultOK then
		config_check = 1
	end
	
	--player:SendLuaPacket("msg_acr_close", {2622},{},{})	
	if config.items == nil then
		x000011_GAcrstatusUpdate(player,{["value"]=config.script_id,["config_check"]=0,["user_data"]=-1})
	else
		x000011_GAcrstatusUpdate(player,{["value"]=config.script_id,["config_check"]=config_check,["user_data"]=-1})	
	end		
end

function x002622_DoReward(player,params)
	
	--print("dddddddddddddddddddddddddddddd")
		
	if x002622_IsAcrActive(player)  ~= ResultCode_ResultOK then
		return ResultCode_Activity_NotStart
	end
	
	local dc_container = player:DataCellContainer()
	if dc_container == nil then
		return ResultCode_InternalResult
	end
	
	local index = tonumber(params["index"])
	
	--背包检查	
	if player:BagLeftCapacity(ContainerType_Layer_ItemBag,5) == false then
		return ResultCode_BagIsFull
	end
	
	--消耗
	if player:DelItemById(TokenType_Token_Crystal, 400, ItemDelLogType_DelType_Draw, 2622) == false then
		return ResultCode_CrystalNotEnough
	end			
	
	local notify_pool = NotifyItemList:new()
	--先记录luckyvalue
	dc_container:add_data_32(CellLogicType_NumberCell, NumberCellIndex_DrawLuckyValue2,1,true)
	
	x002622_DrawInternal(player,config.items[1],notify_pool,4)
	
	local value2 = dc_container:get_data_32(CellLogicType_NumberCell, NumberCellIndex_DrawLuckyValue2)
	if value2 >= 40 then--大保底40次！
		x002622_DrawInternal(player,config.items[3],notify_pool,1)
		--重置
		dc_container:set_data_32(CellLogicType_NumberCell, NumberCellIndex_DrawLuckyValue2,0,true)
	else--小保底
		x002622_DrawInternal(player,config.items[2],notify_pool,1)
	end
	
	print("dddddddddddddddddddddddddddddddddddddvalue2"..value2)
	
	--保底药水
	player:AddItemByIdWithoutNotify(20121003, 1, ItemAddLogType_AddType_Draw,2622,1)
	
	if notify_pool:item_size()>0 then
		player:SendMessage(notify_pool)
	end
	notify_pool:delete()

	return ResultCode_ResultOK
	
end

function x002622_DrawInternal(player, draw_pool, notify_pool,draw_count)

	local allweight = 0;
	for key,iter in pairs(draw_pool)	do
		allweight=allweight+iter[3]
	end
	
	local dc_container = player:DataCellContainer()
	
	local item_id = -1
	local item_count = 0
	for i=1,draw_count,1 do
		local seed_weight = LuaServer():RandomNum(1,allweight)
		local base_weight = 0;

		for key,iter in pairs(draw_pool)	do
			base_weight = base_weight+iter[3]
			if  seed_weight<=base_weight then
				item_id = iter[1]
				item_count = iter[2]
				break
			end
		end
				
		if item_id<0 or item_count==0 then
			LuaServer():LuaLog("error , x002622_DrawInternal invalid draw data! player guid="..player:Guid(),g_log_level.LL_ERROR)
		end
		
		player:AddItemById(item_id,item_count,ItemAddLogType_AddType_Draw,notify_pool,2622,1)
	end
	
end
---------------------------------------------------
function x002622_OnRegisterFunction(proxy)
	
	if proxy ~= nil then
		proxy:RegFunc("xOnLevelUpTo"		,2622)	
	end
end

function x002622_xOnLevelUpTo(player,params)
	x002622_OnUpdate(player)	
	return ResultCode_ResultOK
end

function x002622_xOnNewDay(player,params)
	x002622_OnUpdate(player)	
	return ResultCode_ResultOK
end

--------------------------------------------------------------for client-------------------------------
return config