--触发类型权重
local tb_trigger_rate = {
{"gold",3000},
{"item",3152},
{"monster",3000},
{"nothing",400},
{"skyfall",400}
}

--藏宝图金币奖励池
local tb_gold={
{2500,300},
{2600,300},
{2700,300},
{2800,300},
{2900,300},
{3000,300},
{3100,300},
{3200,300},
{3300,300},
{3400,300},
}


--藏宝图奖励道具池
local tb_item={
{20121001,1,300},
{20230001,1,300},
{20234001,1,6 },
{20234002,1,1 },
{20234003,1,6 },
{20234004,1,1 },
{20234005,1,1 },
{20234006,1,6 },
{20234007,1,1 },
{20234008,1,1 },
{20234009,1,6 },
{20234010,1,6 },
{20234011,1,1 },
{20234012,1,1 },
{20234013,1,3 },
{20234014,1,6 },
{20234015,1,3 },
{20234016,1,1 },
{20234017,1,3 },
{20234018,1,1 },
{20234019,1,3 },
{20234020,1,3 },
{20234021,1,3 },
{20234022,1,6 },
{20234023,1,1 },
{20234024,1,6 },
{20234025,1,3 },
{20234026,1,1 },
{20234027,1,1 },
{20234028,1,1 },
{20234029,1,6 },
{20234030,1,1 },
{20234031,1,3 },
{20231001,1,40},
{20231002,1,40},
{20231003,1,40},
{20231004,1,40},
{20231005,1,40},
{20231006,1,40},
{20231007,1,40},
{20231008,1,40},
{20231009,1,40},
{20231010,1,40},
{20231011,1,40},
{20231012,1,40},
{20231013,1,40},
{20231014,1,40},
{20231015,1,40},
{20231016,1,40},
{20231017,1,40},
{20231018,1,40},
{20231019,1,40},
{20231020,1,40},
{20231021,1,40},
{20231022,1,40},
{20231023,1,40},
{20231024,1,40},
{20231025,1,40},
{20231026,1,40},
{20231027,1,40},
{20231028,1,40},
{20231029,1,40},
{20231030,1,40},
{20231031,1,40},
{20231032,1,40},
{20231033,1,40},
{20231034,1,40},
{20231035,1,40},
{20231036,1,40},
{20231037,1,40},
{20231038,1,40},
{20231039,1,40},
{20231040,1,40},
{20231041,1,40},
{20231042,1,40},
{20231043,1,40},
{20231044,1,40},
{20231045,1,40},
{20231046,1,40},
{20231047,1,40},
{20231048,1,40},
{20231049,1,40},
{20231050,1,40},
{20231051,1,40},
{20231052,1,40},
{20231053,1,40},
{20231054,1,40},
}

--怪物组权重
local tb_monster={
{6101,125},
{6102,125},
{6103,125},
{6104,125},
{6105,125},
{6106,125},
{6107,125},
{6108,125},
{6109,125},
{6110,125},
{6111,125},
{6112,125},
{6113,125},
{6114,125},
{6115,125},
{6116,125},
{6117,125},
{6118,125},
{6119,125},
{6120,125},
{6121,125},
{6122,125},
{6123,125},
{6124,125},
}


function x001505_GenTrigger(player)
	local allweight = 0;
	for key,iter in pairs(tb_trigger_rate)	do
		allweight=allweight+iter[2];
	end
	
	--print("all_weight"..allweight)
	local trigger = "";
	local seed_weight = LuaServer():RandomNum(1,allweight)
	
	--print("seed_weight"..seed_weight)
	local base_weight = 0;
	for key,iter in pairs(tb_trigger_rate)	do
		base_weight = base_weight+iter[2];
		if  seed_weight<=base_weight then
			trigger = iter[1];
			break;
		end
	end
	
	return trigger;
end


function x001505_GenGold(player)
	local allweight = 0;
	for key,iter in pairs(tb_gold)	do
		allweight=allweight+iter[2];
	end
	
	local seed_weight = LuaServer():RandomNum(1,allweight)
	local base_weight = 0;
	for key,iter in pairs(tb_gold)	do
		base_weight = base_weight+iter[2];
		if  seed_weight<=base_weight then
			return iter[1]
		end
	end

	return -1
end

function x001505_GenItem(player)
	local allweight = 0;
	for key,iter in pairs(tb_item)	do
		allweight=allweight+iter[3];
	end

	local seed_weight = LuaServer():RandomNum(1,allweight)
	
	local base_weight = 0;
	for key,iter in pairs(tb_item)	do
		base_weight = base_weight+iter[3];
		if  seed_weight<=base_weight then
			return iter[1],iter[2];
		end
	end

	return -1,-1
end

function x001505_GenSkyFallNpc(player)

	--服务器等级
	--local server_level = LuaServer():ServerLevel()
	--print("server_level"..server_level)
	local ok_list ={}
	for key,group in pairs(g_skyfall_rule) do
		--if server_level<group[5] then
		table.insert(ok_list,group)
		--end
	end

	----print("ok_list"..ok_list)
	--随机生成一组就行
	local size = #ok_list
	--print("size"..size)
	
	if size >0 then
		
		local seed = LuaServer():RandomNum(1,size)
		
		--print("seed"..seed)
		
		local binggo_group = ok_list[seed]
		local npc_node = LuaMtMonsterManager():GenRandomNpc(binggo_group[3],1);
		local pos_node_list = LuaMtMonsterManager():GenRandomPos(binggo_group[2], binggo_group[4]);
			
		--创建动态npc
		for key,node in pairs(pos_node_list) do
			--scene有跨线程问题
			player:AddSceneNpc(node:pos_id(),npc_node[1]:index(),600);
		end
		
		return true
	end
	
	return false
end

function x001505_GenMonsterGroup(player)

	local allweight = 0;
	for key,iter in pairs(tb_monster)	do
		allweight=allweight+iter[2];
	end

	local seed_weight = LuaServer():RandomNum(1,allweight)
	
	local base_weight = 0;
	for key,iter in pairs(tb_monster)	do
		base_weight = base_weight+iter[2];
		if  seed_weight<=base_weight then
			return iter[1];
		end
	end

	return -1;
end

--------------------------------------		
function x001505_LuaCreateItem(player,item_data,config)
	
	--local pos_list = LuaMtMonsterManager():GenRandomPos(config:param(0),1);		
	--item_data:set_param(pos_list[1]:pos_id())
	
	return ResultCode_ResultOK;
end

--添加到背包时候用，create之前，实际上这种道具没有创建实体，只是用来触发一段逻辑
function x001505_LuaAutoUse(player,config,count)
	return ResultCode_UnknownError;
end


function x001505_LuaUseItem(player,item_data,config,target)
	if config:id()==20125002 then	
		--位置校验
		--print("item_data:param()"..item_data:param())
		local treasue_pos = player:GetParams32("treasue_pos")
		if player:ValidPosition(treasue_pos,2500)==false then
			return ResultCode_InvalidPosition
		end
		
		--背包检查
		if player:BagLeftCapacity(ContainerType_Layer_EquipAndItemBag,1) == false then
			return ResultCode_BagIsFull
		end		
		
		if player:DelItemById(config:id(), 1, ItemDelLogType_DelType_UseItem, config:id()) == false then
			return ResultCode_CostItemNotEnough;
		end
	
		local trigger = x001505_GenTrigger(player);
		--print("trigger"..trigger)
		if trigger == "gold" then
			local count = x001505_GenGold(player)
			player:AddItemByIdWithNotify(TokenType_Token_Gold,count,ItemAddLogType_AddType_UseItem,20125002,1)
		elseif trigger == "item" then
			local itemid, count = x001505_GenItem(player)
			player:AddItemByIdWithNotify(itemid,count,ItemAddLogType_AddType_UseItem,20125002,1)
		elseif trigger == "skyfall" then
			if x001505_GenSkyFallNpc(player) then
				player:SendClientNotify("treasure_map_notify_01",-1,-1); --你放出了天灾军团
				--系统公告				
				Lua_send_run_massage("treasure_map_notify_06|"..player:Name().."|"..player:SceneId()); --AAAA在挖掘遗迹时，不小心惊动了天灾军团，天灾军团出现在BBBB
			end
		elseif trigger == "monster" then
			local monster_group = x001505_GenMonsterGroup(player)
			local battle = player:GetBattleManager():CreateFrontBattle(player,nil,
			{
				["script_id"]=1010,
				["pos_id"]=item_data:param(),
				["mission_id"]=-1,
				["relate_npc"]=-1,
				["monster_group_id"]=monster_group
			})
			player:SendClientNotify("treasure_map_notify_02",-1,-1);--你遇到了遗迹守卫
		elseif trigger == "trap" then
			actors = player:Actors()
			for k,actor in pairs(actors) do
				actor:OnHpCure(-200)
			end
			player:SendClientNotify("treasure_map_notify_03",-1,-1); --你遇到了遗迹陷阱，扣除当前所有生命值
		else
			player:SendClientNotify("treasure_map_notify_04",-1,-1); --你挖塌了遗迹
			--系统公告			
			Lua_send_run_massage("treasure_map_notify_05|"..player:Name()); --AAAA在挖掘遗迹时，挖塌了遗迹
		end

		--返回一个包，表示使用成功
		player:SendLuaPacket("TreasureMapOk",{},{item_data:guid()},{trigger})
		
		return ResultCode_ResultOK;
	end
	
	return ResultCode_InvalidItemType;
end
