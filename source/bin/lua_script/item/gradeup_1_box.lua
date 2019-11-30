--进阶材料箱1

local item_group={
{Professions_Warrior,	20231002},
{Professions_Paladin,	20231001},
{Professions_Hunter,	20231003},
{Professions_Shaman,	20231004},
{Professions_Rogue,		20231007},
{Professions_Druid,		20231009},
{Professions_Mage,		20231008},
{Professions_Warlock,	20231005},
{Professions_Priest,	20231006},
}


function x001510_LuaCreateItem(player,item_data,config)
	return ResultCode_UnknownError;
end

--添加到背包时候用，create之前，实际上这种道具没有创建实体，只是用来触发一段逻辑
function x001510_LuaAutoUse(player,config,count)
	if config:id() ~=20125015 then	
		return ResultCode_UnknownError;
	end
	local actor=player:MainActor()
	if actor ~= nil then
		local profession = actor:Professions()
		for p,group in pairs(item_group) do
			if profession == group[1] then
				player:AddItemByIdWithNotify(group[2],count,ItemAddLogType_AddType_UseItem,20125015,1)
				break
			end
		end			
	end
		
	return ResultCode_ResultOK;
end

function x001510_LuaUseItem(player,item_data,config,target)
	return ResultCode_UnknownError;
end
