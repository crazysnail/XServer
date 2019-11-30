--装备宝箱2

local item_group={
{Professions_Warrior,	11410004},
{Professions_Paladin,	11610001},
{Professions_Hunter,	11410001},
{Professions_Shaman,	11410001},
{Professions_Rogue,		11410001},
{Professions_Druid,		11410001},
{Professions_Mage,		11410001},
{Professions_Warlock,	11410001},
{Professions_Priest,	11410001},
}


function x001514_LuaCreateItem(player,item_data,config)
	return ResultCode_UnknownError;
end

--添加到背包时候用，create之前，实际上这种道具没有创建实体，只是用来触发一段逻辑
function x001514_LuaAutoUse(player,config,count)
	if config:id() ~=20125019 then	
		return ResultCode_UnknownError;
	end
	local actor=player:MainActor()
	if actor ~= nil then
		local profession = actor:Professions()
		for p,group in pairs(item_group) do
			if profession == group[1] then
				player:AddItemByIdWithNotify(group[2],count,ItemAddLogType_AddType_UseItem,20125019,1)
				break
			end
		end			
	end
		
	return ResultCode_ResultOK;
end

function x001514_LuaUseItem(player,item_data,config,target)
	return ResultCode_UnknownError;
end
