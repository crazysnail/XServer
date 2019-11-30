--属性合剂

function x001502_LuaCreateItem(player,item_data,config)
	return ResultCode_ResultOK
end

--添加到背包时候用，create之前，实际上这种道具没有创建实体，只是用来触发一段逻辑
function x001502_LuaAutoUse(player,config,count)
	return ResultCode_UnknownError
end

function x001502_LuaUseItem(player,item_data,config,target)	
	local id = config:id()
	if id == 20122001 or id == 20122002 or id == 20122003 or id == 20122004 then
		if target == nil then
			return ResultCode_InvalidTargetPlayer;
		end
		
		if player:DelItemById(id, 1, ItemDelLogType_DelType_UseItem, config:id()) == false then
			return ResultCode_CostItemNotEnough;
		end
		local maxhp = target:MaxHp()
		local cure = maxhp*config:param(0)/100+config:param(1)
		target:OnHpCure(cure)
	else
		if player:DelItemById(id, 1, ItemDelLogType_DelType_UseItem, config:id()) == false then
			return ResultCode_CostItemNotEnough;
		end

		local actor = player:MainActor()
		local proxy = actor:GetSimpleImpactSet()
		local property = config:param(0)
		
		if property == Property_PERCENT_HP then
			proxy:AddSimpleImpact(PotionHpImpact,config:value(),property,config:param(1),true)
		elseif property == Property_PERCENT_PHYSICAL_ATTACK then
			proxy:AddSimpleImpact(PotionPhysicsAttackImpact,config:value(),property,config:param(1),true)
		elseif property == Property_PERCENT_PHYSICAL_ARMOR then
			proxy:AddSimpleImpact(PotionPhysicDefendImpact,config:value(),property,config:param(1),true)
		elseif property == Property_PERCENT_MAGIC_ATTACK then
			proxy:AddSimpleImpact(PotionMagicAttackImpact,config:value(),property,config:param(1),true)
		elseif property == Property_PERCENT_MAGIC_ARMOR then
			proxy:AddSimpleImpact(PotionMagicDefendImpact,config:value(),property,config:param(1),true)
		end
	end	
		
	
	player:SendClientNotify("item_notify_001",-1,-1)
			
	return ResultCode_ResultOK
end
