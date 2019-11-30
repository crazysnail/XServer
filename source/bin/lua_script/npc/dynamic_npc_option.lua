
function x003002_OnNpcOption(player,npc,option_index)	
	local pos_node = npc:PositionConfig()
	if pos_node == nil then
		return player:SendClientNotify("InvalidPosition",-1,-1)	
	end	
	
	local npc_config = npc:NpcDynamicConfig()
	if npc_config == nil then
		return player:SendClientNotify("InvalidConfigData",-1,-1)	
	end
	
	local npc_info = npc:NpcInfo()
	if npc_info == nil then
		return player:SendClientNotify("InvalidConfigData",-1,-1)	
	end
		
	local npc_tag = npc_config:npc_tag()
	--print("npc_tag"..npc_tag)
	if npc_tag == "mission" then
		--任务选项走单独的流程		
	elseif npc_tag == "burning_union" then
			local battle = player:GetBattleManager():CreateFrontBattle(player,nil,
			{
				["script_id"]=1006,
				["pos_id"]=pos_node:id(),
				["relate_npc"]= npc_info:series_id(),
				["npc_index"]=npc_config:index()
			})
				
	elseif npc_tag == "skyfall_union" then
			local battle = player:GetBattleManager():CreateFrontBattle(player,nil,
			{
				["script_id"]=1007,
				["pos_id"]=pos_node:id(),
				["relate_npc"]= npc_info:series_id(),
				["npc_index"]=npc_config:index()
			})
	elseif npc_tag == "guild_boss" then
		--player:OnGoGuildBossBattle();
	elseif npc_tag == "world_boss" then	
		if LuaMtWorldBossManager():WorldBossDead() == true then
			return player:SendClientNotify("wb_dead",-1,-1)	
		end
		if player:GetWorldBossReliveCd() > 0 then
			return player:SendClientNotify("wb_cd",-1,-1)	
		end
		local battle = player:GetBattleManager():CreateFrontBattle(player,nil,
		{
				["script_id"]=1011,
				["group_id"]=3
		})
		--player:OnGoWorldBossBattle();
	elseif npc_tag == "hell_box" then	
		x003002_OnHellBoxReward(player,npc_info,npc_config)
	elseif npc_tag == "guildbattle_box" then	
		--背包检查
		if player:BagLeftCapacity(ContainerType_Layer_EquipAndItemBag,1) == false then
			return player:SendClientNotify("BagIsFull",-1,-1)	
		end
		local flag = player:GetParams32(npc_tag)
		if flag ~= INVALID_INT32 then
			return player:SendClientNotify("GuildBattle_GetBoxOnce",-1,-1)	
		end
		player:SetParams32(npc_tag,1);
		local raid = player:GetRaid();
		if raid ~= nil then
			raid:DelNpc(npc_info:series_id())
		end

		player:AddItemByIdWithNotify(20125022,1,ItemAddLogType_AddType_Guild,ActivityType.UnionBattle,1)
		--player:AddItemByIdWithNotify(TokenType_Token_Exp,500,ItemAddLogType_AddType_Guild,ActivityType.UnionBattle,1)
	else
		--print("npc_tag"..npc_tag)
		return player:SendClientNotify("InvalidNpcOption",-1,-1)	
	end	
	
	return ResultCode_ResultOK
end

function x003002_OnHellBoxReward(player,npc_info,npc_config)

	--检查玩家击杀情况
	local target_guid = player:GetParams64("target_guid")
	local target = player:GetPlayerNpc(target_guid)
	if target:passed() == false then
		return player:SendClientNotify("hell_notify_000",-1,-1)			
	end
	--背包检查
	if player:BagLeftCapacity(ContainerType_Layer_EquipAndItemBag,1) == false then
		return player:SendClientNotify("BagIsFull",-1,-1)	
	end
	--player:DelPlayerNpcByIndex(index)
	player:DelPlayerNpc(target:player():guid())
	player:DelNpc(npc_info:series_id())
	--领奖标记
	target:set_rewarded(true)	
	player:OnLuaFunCall_1(501,"AddActivityTimes",ActivityType.Hell)				
	--奖励经验
	local level_config = LuaMtActorConfig():GetLevelData(player:PlayerLevel())
	if level_config == nil then
		return player:SendClientNotify("InvalidConfigData",-1,-1)	
	end
	player:AddItemByIdWithNotify(TokenType_Token_Gold,level_config:hell_box_gold(),ItemAddLogType_AddType_Activity,ActivityType.Hell,1)
	--道具池子
	if npc_config:group_id() ==19996 then
		--终极宝箱
		player:AddItemByIdWithNotify(20125036,1,ItemAddLogType_AddType_Activity,ActivityType.Hell,1)
		--正义点
		local name = LuaItemManager():GetItemName(TokenType_Token_LawFul)
		if name ~= nil then
			local count = 200
			player:AddItemByIdWithNotify(TokenType_Token_LawFul,count,ItemAddLogType_AddType_Activity,ActivityType.Hell,1)
			Lua_send_run_massage("ac_notify_042|"..player:Name().."|"..name.."|"..math.floor(count))
		end
	elseif npc_config:group_id()==19998 then 
		--小box
		player:AddItemByIdWithNotify(20125021,1,ItemAddLogType_AddType_Activity,ActivityType.Hell,1)
	elseif npc_config:group_id()==19999 then 
		--大box		
		player:AddItemByIdWithNotify(20125022,1,ItemAddLogType_AddType_Activity,ActivityType.Hell,1)
	end		
	--掉落活动检查
	player:CheckAcrDrop(ActivityType.Hell)
	
	--刷新敌人数据
	if player:CheckRefreshNextWave() then
		--加载下一批
		local scene = player:Scene()
		scene:OnPlayerTrialInfoLoad(player:Guid())
	else
		player:RefreshPlayerNpc(false)
		
		--
		local container = player:DataCellContainer()
		if container ~= nil then
			local count = container:get_data_32(CellLogicType_NumberCell,NumberCellIndex_HellDayWaveCount)
			if count >= 5 then
				player:SendLuaPacket("raid_mission_over",{-1},{},{})
			end			
		end
	end	
		
end