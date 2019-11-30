--装备宝箱1

local item_group={
{Professions_Warrior,	11900001},
{Professions_Paladin,	11900001},
{Professions_Hunter,	11900002},
{Professions_Shaman,	11900003},
{Professions_Rogue,		11900002},
{Professions_Druid,		11900004},
{Professions_Mage,		11900003},
{Professions_Warlock,	11900003},
{Professions_Priest,	11900004},
}


function x001530_LuaCreateItem(player,item_data,config)
	return ResultCode_UnknownError;
end

--添加到背包时候用，create之前，实际上这种道具没有创建实体，只是用来触发一段逻辑
function x001530_LuaAutoUse(player,config,count)
	if config:id() ~=20125025 then	
		return ResultCode_UnknownError;
	end
	local actor=player:MainActor()
	if actor ~= nil then
		local profession = actor:Professions()
		for p,group in pairs(item_group) do
			if profession == group[1] then
				player:AddItemByIdWithNotify(group[2],count,ItemAddLogType_AddType_UseItem,config:id(),1)
				break
			end
		end			
	end
		
	return ResultCode_ResultOK;
end

function x001530_LuaUseItem(player,item_data,config,target)
	return ResultCode_UnknownError;
end
