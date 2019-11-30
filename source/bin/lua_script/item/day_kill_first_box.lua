--无尽试炼首杀宝箱

local item_group={
{Professions_Warrior,	11810025},
{Professions_Paladin,	11810025},
{Professions_Hunter,	11810026},
{Professions_Shaman,	11810027},
{Professions_Rogue,		11810026},
{Professions_Druid,		11810028},
{Professions_Mage,		11810027},
{Professions_Warlock,	11810027},
{Professions_Priest,	11810028},
}


function x001524_LuaCreateItem(player,item_data,config)
	return ResultCode_UnknownError;
end

--添加到背包时候用，create之前，实际上这种道具没有创建实体，只是用来触发一段逻辑
function x001524_LuaAutoUse(player,config,count)
	if config:id() ~=20125023 then	
		return ResultCode_UnknownError;
	end
	local actor=player:MainActor()
	if actor ~= nil then
		local profession = actor:Professions()
		for p,group in pairs(item_group) do
			if profession == group[1] then
				player:AddItemByIdWithNotify(group[2],count,ItemAddLogType_AddType_UseItem,20125023,1)
				break
			end
		end			
	end
		
	return ResultCode_ResultOK;
end

function x001524_LuaUseItem(player,item_data,config,target)
	return ResultCode_UnknownError;
end
