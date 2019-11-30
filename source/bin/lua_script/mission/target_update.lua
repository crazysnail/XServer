
function x000504_OnRegisterFunction(proxy)
	
	if proxy ~= nil then
		proxy:RegFunc("xActorLevelUpTo"				,504)
		proxy:RegFunc("xAddEquipment"				,504)
		proxy:RegFunc("xUpdateSuitEquipEffect"		,504)
		proxy:RegFunc("xOnMonsterDead"				,504)
		proxy:RegFunc("xEnhenceLevel"				,504)
		proxy:RegFunc("xActivityBoxReward"			,504)
		proxy:RegFunc("xPlayerRaidStage"			,504)		
		proxy:RegFunc("xCostItem"					,504)
		proxy:RegFunc("xOnSetActorFight"			,504)
		proxy:RegFunc("xOnUpgradeColor"				,504)
		proxy:RegFunc("xOnUpgradeStar"				,504)
		proxy:RegFunc("xOnUpgradeSKill"				,504)
		proxy:RegFunc("xEnhanceMasterLv"			,504)
		proxy:RegFunc("xOnEnchantEquip"				,504)
		proxy:RegFunc("xOnInsetEquip"				,504)
		proxy:RegFunc("xRideMount"					,504)	
		proxy:RegFunc("xOnBurningBattleWin"			,504)
		proxy:RegFunc("xOnJoinGuildOk"				,504)
		proxy:RegFunc("xOnSetLackeys"				,504)
		proxy:RegFunc("xOnHellBattleWin"			,504)
		proxy:RegFunc("xOnAddPracticeLevel"			,504)	
		proxy:RegFunc("xOnAddItem"					,504)	
		proxy:RegFunc("xOnWorldBoss"				,504)			
		proxy:RegFunc("xOnTargetCheck"				,504)
		
	end
end
-----------------------------------------------------------------
function x000504_xOnTargetCheck(player,params)
	--dbfix
	params["finish_type"] = TargetFinishType_AnyStepUp
	params["resolve_type"] = TargetResolveType_Special
	return x000504_OnTargetResolve(player,params)
end

function x000504_xOnWorldBoss(player,params)
	params["finish_type"] = TargetFinishType_WorldBoss
	params["resolve_type"] = TargetResolveType_Special
	return x000504_OnTargetResolve(player,params)
end
function x000504_xOnBurningBattleWin(player,params)
	params["finish_type"] = TargetFinishType_WinBurningBattle
	params["resolve_type"] = TargetResolveType_Special
	return x000504_OnTargetResolve(player,params)
end
function x000504_xOnJoinGuildOk(player,params)
	params["finish_type"] = TargetFinishType_JoinUnion
	params["resolve_type"] = TargetResolveType_Special
	return x000504_OnTargetResolve(player,params)
end
function x000504_xOnSetLackeys(player,params)
	params["finish_type"] = TargetFinishType_LackeysUse
	params["resolve_type"] = TargetResolveType_MoreValue
	return x000504_OnTargetResolve(player,params)
end
function x000504_xOnHellBattleWin(player,params)
	params["finish_type"] = TargetFinishType_WinHell
	params["resolve_type"] = TargetResolveType_MoreValue
	return x000504_OnTargetResolve(player,params)
end
function x000504_xOnAddPracticeLevel(player,params)
	params["finish_type"] = TargetFinishType_UnionSkil
	params["resolve_type"] = TargetResolveType_MoreValue
	return x000504_OnTargetResolve(player,params)
end
function x000504_xOnAddItem(player,params)
	params["finish_type"] = TargetFinishType_GainEquip
	params["resolve_type"] = TargetResolveType_StatisticsValue
	
	local index = params["value"]
	local equip = LuaItemManager():GetEquipmentConfig(index)
	if equip == nil then
		return ResultCode_InternalResult
	end
	params["value"]=equip:color()					
	return x000504_OnTargetResolve(player,params)
end
--
function x000504_xActorLevelUpTo(player,params)
	params["finish_type"] = TargetFinishType_HeroLevelUp
	params["resolve_type"] = TargetResolveType_Special
	return x000504_OnTargetResolve(player,params)
end
function x000504_xAddEquipment(player,params)
	params["finish_type"] = TargetFinishType_GetEquip
	params["resolve_type"] = TargetResolveType_EqualValue
	x000504_OnTargetResolve(player,params)
	
	--数量目标
	params["finish_type"] = TargetFinishType_OnEquip
	params["resolve_type"] = TargetResolveType_Special
	return x000504_OnTargetResolve(player,params)
end
function x000504_xUpdateSuitEquipEffect(player,params)
	params["finish_type"] = TargetFinishType_SuitAttr
	params["resolve_type"] = TargetResolveType_MoreValue
	return x000504_OnTargetResolve(player,params)
end
function x000504_xOnMonsterDead(player,params)
	params["value"] = params["monster_id"]
	params["finish_type"] = TargetFinishType_TFT_KillMonster
	params["resolve_type"] = TargetResolveType_StatisticsValue
	return x000504_OnTargetResolve(player,params)
end
function x000504_xEnhenceLevel(player,params)
	params["finish_type"] = TargetFinishType_EquipEnhanceLv
	params["resolve_type"] = TargetResolveType_MoreValue
	return x000504_OnTargetResolve(player,params)
end
function x000504_xActivityBoxReward(player,params)
	params["finish_type"] = TargetFinishType_DayToken
	params["resolve_type"] = TargetResolveType_EqualValue
	return x000504_OnTargetResolve(player,params)
end
--function x000504_xFirstStageReward(player,params)
function x000504_xPlayerRaidStage(player,params)
	params["finish_type"] = TargetFinishType_StageProcess
	params["resolve_type"] = TargetResolveType_MoreValue
	return x000504_OnTargetResolve(player,params)
end
function x000504_xCostItem(player,params)
	params["finish_type"] = TargetFinishType_DrawCard
	params["resolve_type"] = TargetResolveType_EqualValue
	return x000504_OnTargetResolve(player,params)
end
function x000504_xOnSetActorFight(player,params)
	params["finish_type"] = TargetFinishType_FightHero
	params["resolve_type"] = TargetResolveType_MoreValue
	return x000504_OnTargetResolve(player,params)
end
function x000504_xOnUpgradeColor(player,params)
	params["finish_type"] = TargetFinishType_StepUp
	params["resolve_type"] = TargetResolveType_MoreValue
	x000504_OnTargetResolve(player,params)
	
	params["finish_type"] = TargetFinishType_AnyStepUp
	params["resolve_type"] = TargetResolveType_Special
	return x000504_OnTargetResolve(player,params)
end
function x000504_xOnUpgradeStar(player,params)
	--print("shitddddddddddddddddddddddddddddddddddd")
	params["finish_type"] = TargetFinishType_StarUp
	params["resolve_type"] = TargetResolveType_MoreValue
	return x000504_OnTargetResolve(player,params)
end
function x000504_xOnUpgradeSKill(player,params)
	params["finish_type"] = TargetFinishType_SkillLv
	params["resolve_type"] = TargetResolveType_MoreValue
	return x000504_OnTargetResolve(player,params)
end
function x000504_xEnhanceMasterLv(player,params)
	params["finish_type"] = TargetFinishType_AllEnhanceLv
	params["resolve_type"] = TargetResolveType_MoreValue
	return x000504_OnTargetResolve(player,params)
end

function x000504_xOnEnchantEquip(player,params)
	params["finish_type"] = TargetFinishType_EquipEnchant
	params["resolve_type"] = TargetResolveType_Special
	return x000504_OnTargetResolve(player,params)
end

function x000504_xOnInsetEquip(player,params)
	params["finish_type"] = TargetFinishType_EmbedGem
	params["resolve_type"] = TargetResolveType_Special
	return x000504_OnTargetResolve(player,params)
end
function x000504_xRideMount(player,params)
	params["finish_type"] = TargetFinishType_GetMount
	params["resolve_type"] = TargetResolveType_EqualValue
	return x000504_OnTargetResolve(player,params)
end
function x000504_xAddActivityTimes(player,params)
	params["finish_type"] = TargetFinishType_FinishActivity
	params["resolve_type"] = TargetResolveType_EqualValue
	return x000504_OnTargetResolve(player,params)
end

-----------------------------------------------------
function x000504_OnTargetGroupReward(player,max_level)
	local config = LuaMtMissionManager():GetAllTarget(player:GetCamp(),99)
	local all_ok = 1
	local group_id= -1
	local container = player:DataCellContainer()
	for k,v in pairs( config ) do
		if v:max_level() == max_level then			
			local data = container:get_data_32(CellLogicType_TargetData,v:index())
			--判定领奖标记
			local flag = container:get_high_16_bit(data)
			local count = container:get_low_16_bit(data)
			
			if container:check_bit_data_32(flag,1) == false then	
				all_ok = 0
				break
			end
			group_id = v:group()
		end
	end
	--print("ddddddddddddddddddddddd all_ok "..all_ok.." group_id "..group_id)
	if all_ok == 1 and group_id >=0 and group_id < 16 then
		local flag = container:get_data_32(CellLogicType_NumberCell,NumberCellIndex_TargetRewardFlag)
		local bcan= container:check_bit_data_32(flag,group_id) 
		local bdone= container:check_bit_data_32(flag,group_id+16)
		--print("bcan"..tostring(bcan))
		--print("bdone"..tostring(bdone))
		if bcan== false and bdone == false then	
			local new_flag = container:set_bit_data_32(flag,group_id)
			container:set_data_32(CellLogicType_NumberCell,NumberCellIndex_TargetRewardFlag,new_flag,true)		
		end
	end
	return ResultCode_ResultOK
end
-----------------------------------------------------
function x000504_OnTargetResolve(player,params)
	local value = params["value"]
	local finish_type = params["finish_type"]
	local resolve_type = params["resolve_type"]
	
	--print("value"..value.."finish_type"..finish_type.."resolve_type"..resolve_type)
	
	local config = LuaMtMissionManager():GetAllTarget(player:GetCamp(),99)
	for k,v in pairs( config ) do
		if v:finish_type()==finish_type then
			
			local container = player:DataCellContainer()
			local data = container:get_data_32(CellLogicType_TargetData,v:index())
			--判定领奖标记
			local flag = container:get_high_16_bit(data)
			local count = container:get_low_16_bit(data)
			local bcan = container:check_bit_data_32(flag,0)
			local bdone = container:check_bit_data_32(flag,1)
			--print("bcan"..tonumber(bcan).." bdone"..tonumber(bdone))
			--if bcan then
			--	print("shit 0 true")
			--else
			--	print("shit 0 false")
			--end
			--
			--if bdone then
			--	print("shit 1 true")
			--else
			--	print("shit 1 false")
			--end
			
			if bcan == false and bdone == false then	
				local is_ok = false;
				if resolve_type == TargetResolveType_NoValue then
					local new_flag = container:set_bit_data_32(flag,1)
					local new_data = container:bind_bit_data_32(new_flag,count)
					container:set_data_32(CellLogicType_TargetData,v:index(),new_data,true)
					is_ok = true
				elseif resolve_type == TargetResolveType_EqualValue and value == v:param(0) then
					local new_flag = container:set_bit_data_32(flag,1)
					local new_data = container:bind_bit_data_32(new_flag,count)
					container:set_data_32(CellLogicType_TargetData,v:index(),new_data,true)
					is_ok = true
				elseif resolve_type == TargetResolveType_MoreValue and value >= v:param(0) then
					local new_flag = container:set_bit_data_32(flag,1)
					local new_data = container:bind_bit_data_32(new_flag,count)
					container:set_data_32(CellLogicType_TargetData,v:index(),new_data,true)
					is_ok = true
				elseif resolve_type == TargetResolveType_LessLalue and value < v:param(0) then
					local new_flag = container:set_bit_data_32(flag,1)
					local new_data = container:bind_bit_data_32(new_flag,count)
					container:set_data_32(CellLogicType_TargetData,v:index(),new_data,true)
					is_ok = true
				elseif resolve_type == TargetResolveType_StatisticsValue and value == v:param(0) then
					--print("ddddddddddddddddddddddddddddd"..v:param(0))
					count = count +1
					local new_flag = flag
					if count >= v:param(1) then
						new_flag = container:set_bit_data_32(flag,1)
						is_ok = true
					end
					local new_data = container:bind_bit_data_32(new_flag,count)
					container:set_data_32(CellLogicType_TargetData,v:index(),new_data,true)
				--以下特写	
				elseif resolve_type == TargetResolveType_Special then
					--print("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx2")
					if finish_type == TargetFinishType_HeroLevelUp then
						local actors = player:Actors()
						local new_count = 0
						local new_flag = flag
						for k,actor in pairs(actors) do
							if actor:Level() >= v:param(0) then
								new_count = new_count+1
							end														
							if new_count >= v:param(1) then
								new_flag = container:set_bit_data_32(flag,1)
								is_ok = true
								break
							end		
						end	
						local new_data = container:bind_bit_data_32(new_flag,new_count)
						container:set_data_32(CellLogicType_TargetData,v:index(),new_data,true)
					elseif finish_type == TargetFinishType_EquipEnchant then
						local result = player:EnchantSlotEquips(true)
						local new_count = 0
						local new_flag = flag
						for m,n in pairs(result) do							
							if m>=v:param(0) then
								new_count = new_count+n								
								if new_count>=v:param(1) then
									--print("m "..m.." n"..n.." new_count "..new_count.." v:param(0) "..v:param(0).." v:param(1) "..v:param(1))
									new_flag = container:set_bit_data_32(flag,1)
									is_ok = true
									break
								end
							end		
						end
											
						local new_data = container:bind_bit_data_32(new_flag,new_count)
						container:set_data_32(CellLogicType_TargetData,v:index(),new_data,true)
					elseif finish_type == TargetFinishType_OnEquip then
						local actors = player:Actors()
						local new_count = 0
						local new_flag = flag
						for k,actor in pairs(actors) do
							local equips = actor:CurrEquipments(false)
							new_count = #equips
							if new_count>=v:param(0) then
								new_flag = container:set_bit_data_32(flag,1)
								is_ok = true
														
								local new_data = container:bind_bit_data_32(new_flag,new_count)
								container:set_data_32(CellLogicType_TargetData,v:index(),new_data,true)
								
								break
							end	
						end
					elseif finish_type == TargetFinishType_JoinUnion 
					or	finish_type == TargetFinishType_WorldBoss 
					or	finish_type == TargetFinishType_WinBurningBattle 
					then
						count = count +1
						local new_flag = flag
						if count >= v:param(0) then
							new_flag = container:set_bit_data_32(flag,1)
							is_ok = true
						end
						local new_data = container:bind_bit_data_32(new_flag,count)
						container:set_data_32(CellLogicType_TargetData,v:index(),new_data,true)
					
					elseif finish_type == TargetFinishType_AnyStepUp then
						local actors = player:Actors()
						local new_count = 0
						local new_flag = flag
						for k,actor in pairs(actors) do
							local dbinfo = actor:ActorDBInfo()
							if dbinfo:color() >= v:param(1) then
								new_count = new_count+1
							end
						end	
						if new_count>=v:param(0) then
							new_flag = container:set_bit_data_32(flag,1)
							is_ok = true
						end			
						--print("------------------------new_count "..new_count.." v:param(1)"..v:param(1).." new_flag "..new_flag.." v:param(0) "..v:param(0))
						local new_data = container:bind_bit_data_32(new_flag,0)
						container:set_data_32(CellLogicType_TargetData,v:index(),new_data,true)						
					elseif finish_type == TargetFinishType_EmbedGem then
						--print("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx3")
						local items = player:AllInsetGems()
						local new_count = 0
						for k,item in pairs(items) do
							local gem_config = LuaItemManager():GetGemConfig(item)
							if gem_config:level() >= v:param(0) then
								new_count = new_count+1
							end								
						end	
						--print("ddddddddddddddddddddddddddddd"..v:param(0))
						local new_flag = flag
						--print("dddddddddddddddddddddddddddddddddddTargetFinishType_EmbedGem"..new_count.." v:param(1)"..v:param(1))
						if new_count >= v:param(1) then
							new_flag = container:set_bit_data_32(flag,1)
							is_ok = true
						end
						
						local new_data = container:bind_bit_data_32(new_flag,new_count)
						container:set_data_32(CellLogicType_TargetData,v:index(),new_data,true)
					end
				end
				
				--检查段领奖
				if is_ok  == true then
					x000504_OnTargetGroupReward(player,v:max_level())					
				end
			end
		end
	end
	return ResultCode_ResultOK;
end
