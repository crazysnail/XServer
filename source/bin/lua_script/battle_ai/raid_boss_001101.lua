
-- 奥妮克希亚血量改变时的ai

local stage_1skills = {211330, 211331, 211332}
local stage_2_skills = {211333, 211334, 211335}
local stage_3_skills = {211330, 211331, 211332, 211333, 211334, 211335, 211336}


function x001101_AfterHpChange(boss)
	local boss_stage = boss:get_meta("boss_stage")
	local hp_percent = boss:hp() / boss:max_hp()
	if (boss_stage == 0) then
		boss:set_meta("boss_stage",1)
		boss:clear_runtime_skill()
		for _, skill_id in pairs(stage_1skills) do
			boss:add_runtime_skill(skill_id, boss:level())
		end
	elseif boss_stage == 1 then
		if (hp_percent <= 0.66) then -- 进入第二阶段
			boss:set_meta("boss_stage", 2)
			boss:clear_runtime_skill()
			for _,skill_id in pairs(stage_2_skills) do
				boss:add_runtime_skill(skill_id, boss:level())
			end
		end
	elseif (boss_stage == 2) then
		if (hp_percent <= 0.36) then -- 进入第三阶段
			boss:set_meta("boss_stage", 3)
			boss:clear_runtime_skill()
			for _,skill_id in pairs(stage_3_skills) do
				boss:add_runtime_skill(skill_id, boss:level())
			end
		end
	end
	return ResultCode_ResultOK
end

--时间控制的ai在tick里做
function x001101_OnTick(boss, elapse_time)
	return ResultCode_ResultOK
end