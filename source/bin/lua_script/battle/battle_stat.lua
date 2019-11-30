
function x001026_OnDamageRefix(battle_ground,damage_info,killer)

end

function x001026_OnDamage(battle, is_attacker, source_guid, target_guid, damage_value)
	local stat = battle:GetStat()
	local find = false
	for i=0,stat:damages_size() - 1 do
		local damage = stat:damages(i)
		if damage:guid() == source_guid then
			damage:set_value(damage:value() + damage_value)
			find = true
		end
	end
	if find == false then
		local damage = stat:add_damages()
		damage:set_guid(source_guid)
		damage:set_value(damage_value)
	end

	local find = false
	for i=0,stat:be_damages_size() - 1 do
		local damage = stat:be_damages(i)
		if damage:guid() == target_guid then
			damage:set_value(damage:value() + damage_value)
			find = true
		end
	end
	if find == false then
		local damage = stat:add_be_damages()
		damage:set_guid(target_guid)
		damage:set_value(damage_value)
	end
	--统计伤害总量
	if is_attacker then
		stat:set_attacker_damage(stat:attacker_damage() + damage_value)
	else
		stat:set_defender_damage(stat:defender_damage() + damage_value)
	end
	return ResultCode_ResultOK
end

function x001026_OnCure(battle, is_attacker, source_guid, target_guid, cure_value)
	local stat = battle:GetStat()
	local find = false
	for i=0,stat:cures_size() - 1 do
		local cure = stat:cures(i)
		if cure:guid() == source_guid then
			cure:set_value(cure:value() + cure_value)
			find = true
		end
	end
	if find == false then
		local cure = stat:add_cures()
		cure:set_guid(source_guid)
		cure:set_value(cure_value)
	end
	--统计治疗总量
	if is_attacker then
		stat:set_attacker_cure(stat:attacker_cure() + cure_value)
	else
		stat:set_defender_cure(stat:defender_cure() + cure_value)
	end
	return ResultCode_ResultOK
end