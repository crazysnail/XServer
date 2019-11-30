--金币首抽宝箱

local item_group={
{Professions_Warrior,	28810003},
{Professions_Paladin,	28810009},
{Professions_Hunter,	28810029},
{Professions_Shaman,	28810010},
{Professions_Rogue,		28810029},
{Professions_Druid,		28810024},
{Professions_Mage,		28810006},
{Professions_Warlock,	28810006},
{Professions_Priest,	28810017},
}


function x001508_LuaCreateItem(player,item_data,config)
	return ResultCode_UnknownError;
end

--添加到背包时候用，create之前，实际上这种道具没有创建实体，只是用来触发一段逻辑
function x001508_LuaAutoUse(player,config,count)
	if config:id() ~=20125013 then	
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

function x001508_LuaUseItem(player,item_data,config,target)
	return ResultCode_UnknownError;
end
