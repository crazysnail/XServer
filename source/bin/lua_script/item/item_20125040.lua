--40级紫色装备包

local item_group={
10130001,	--炽炎战斧
10130002,	--斩首者康恩
10330001,	--夜刃
10430001,	--灼热弓
10530001,	--精确校准过的火枪
10630001,	--乔丹法杖
10630002,	--典狱官法杖
10730001,	--风暴战斧
10830001,	--热情监护者
10930001,	--断肠
11030001,	--眩光
11430001,	--绿塔
11430002,	--黑颅
12030001,	--救世之戒
12430001,	--冰鳞甲
12430002,	--洞察法袍
12630001,	--闪避之靴
}

function x001548_LuaCreateItem(player,item_data,config)
	return ResultCode_UnknownError;
end

--添加到背包时候用，create之前，实际上这种道具没有创建实体，只是用来触发一段逻辑
function x001548_LuaAutoUse(player,config,count)
	if config:id() ~=20125040 then	
		return ResultCode_UnknownError;
	end
	local seed = LuaServer():RandomNum(1,table.nums(item_group))
	player:AddItemByIdWithNotify(item_group[seed],1*count,ItemAddLogType_AddType_UseItem,config:id(),1)
	
	return ResultCode_ResultOK;
end

function x001548_LuaUseItem(player,item_data,config,target)
	return ResultCode_UnknownError;
end