--

local stage_0skills = {211010,211011,211012}
local stage_1skills = {211015}


function x001102_AfterHpChange(boss)
	return ResultCode_ResultOK
end

--时间控制的ai在tick里做
function x001102_OnTick(boss, elapse_time)
	local bg = boss:battle_ground()
	local boss_stage = boss:get_meta("boss_stage")
	if boss_stage == 0 then
		boss:set_meta("boss_stage", 1)
		boss:clear_runtime_skill()
		for _, skill_id in pairs(stage_0skills) do
			boss:add_runtime_skill(skill_id, boss:level())
		end
		return ResultCode_ResultOK
	end
	if bg ~= nil and bg:Brief():elapse_time() > 30 * 1000 and boss_stage == 1 then
		boss:set_meta("boss_stage", 2)
		boss:clear_runtime_skill()
		for _, skill_id in pairs(stage_1skills) do
			boss:add_runtime_skill(skill_id, boss:level())
		end
	end
	return ResultCode_ResultOK
end