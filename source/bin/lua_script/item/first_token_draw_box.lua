
--金币二抽魂匣
local item_group={
{Professions_Warrior,	20234022},
{Professions_Paladin,	20234024},
{Professions_Hunter,	20234022},
{Professions_Shaman,	20234022},
{Professions_Rogue,		20234022},
{Professions_Druid,		20234006},
{Professions_Mage,		20234024},
{Professions_Warlock,	20234024},
{Professions_Priest,	20234029},
}


function x001525_LuaCreateItem(player,item_data,config)
	return ResultCode_UnknownError;
end

--添加到背包时候用，create之前，实际上这种道具没有创建实体，只是用来触发一段逻辑
function x001525_LuaAutoUse(player,config,count)
	if config:id() ~=20125024 then	
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

function x001525_LuaUseItem(player,item_data,config,target)
	return ResultCode_UnknownError;
end
