--一星宝箱

local item_group={
{Professions_Warrior,	28810010},
{Professions_Paladin,	28810006},
{Professions_Hunter,	28810003},
{Professions_Shaman,	28810003},
{Professions_Rogue,		28810003},
{Professions_Druid,		28810009},
{Professions_Mage,		28810009},
{Professions_Warlock,	28810009},
{Professions_Priest,	28810003},
}


function x001506_LuaCreateItem(player,item_data,config)
	return ResultCode_UnknownError;
end

--添加到背包时候用，create之前，实际上这种道具没有创建实体，只是用来触发一段逻辑
function x001506_LuaAutoUse(player,config,count)
	if config:id() ~=20125011 then	
		return ResultCode_UnknownError;
	end
	local actor=player:MainActor()
	if actor ~= nil then
		local profession = actor:Professions()
		for p,group in pairs(item_group) do
			if profession == group[1] then
				--注意，--首个礼包英雄不展示config:id()作为sourceid有用!
				player:AddItemByIdWithNotify(group[2],count,ItemAddLogType_AddType_UseItem,config:id(),1)
				break
			end
		end			
	end
		
	return ResultCode_ResultOK;
end

function x001506_LuaUseItem(player,item_data,config,target)
	return ResultCode_UnknownError;
end
