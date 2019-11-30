
--钻石首抽宝箱
local item_group={
{Professions_Warrior,	28810013},
{Professions_Paladin,	28810015},
{Professions_Hunter,	28810013},
{Professions_Shaman,	28810013},
{Professions_Rogue,		28810013},
{Professions_Druid,		28810015},
{Professions_Mage,		28810015},
{Professions_Warlock,	28810015},
{Professions_Priest,	28810013},
}


function x001509_LuaCreateItem(player,item_data,config)
	return ResultCode_UnknownError;
end

--添加到背包时候用，create之前，实际上这种道具没有创建实体，只是用来触发一段逻辑
function x001509_LuaAutoUse(player,config,count)
	if config:id() ~=20125014 then	
		return ResultCode_UnknownError;
	end
	
	local actor=player:MainActor()
	if actor ~= nil then
		local profession = actor:Professions()
		for p,group in pairs(item_group) do
			if profession == group[1] then
				player:AddItemByIdWithNotify(group[2],count,ItemAddLogType_AddType_UseItem,20125014,1)
				break
			end
		end			
	end
		
	return ResultCode_ResultOK;
end

function x001509_LuaUseItem(player,item_data,config,target)
	return ResultCode_UnknownError;
end
