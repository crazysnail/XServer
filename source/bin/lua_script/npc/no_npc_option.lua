
function x003003_OnNpcOption(player,string_param,long_param,int_param)
	--print("string_param",string_param)
	if string_param == "guildwetcopy" then
		if player:GetRaid() ~= nil then
			return player:SendClientNotify("nofubentofuben",-1,-1)
		end
		local copyid = int_param;
		local copyconfig = LuaMtGuildManager():GetGuildWetCopyConfig(copyid);
		if copyconfig == nil then
			return ResultCode_ResultOK;
		end
		if player:PlayerLevel() < copyconfig:levellimit() then
			return player:SendClientNotify("fightwetcopyplayerlevel|"..tostring(math.floor(copyconfig:levellimit())),-1,-1)
		end
		local ac_config = LuaActivityManager():FindActivity(ActivityType.GuildWetCopy)
		if ac_config == nil then
			return ResultCode_InternalResult
		end
		
		local ac_name = ac_config:name()
		--队伍检查
		if player:GetTeamID() ~= 0 then
			return player:SendClientNotify("ac_notify_004|"..ac_name,-1,1)
		end
		LuaServer():SendS2GCommonMessage("S2GFightGuildWetCopy",{copyid},{player:Guid(),player:GetGuildID()},{})	--先到主线程离开队伍，再离开场景
	elseif string_param == "guildworldboss" then
		local ret= player:ActivityCheck(ActivityType.UnionBoss)
		if ret ~=  ResultCode_ResultOK then
			return ret;
		end
		player:GoRaid(LuaMtConfigDataManager():FindConfigValue("guild_battle_common"):value2(),2006,0,-1,{-1,-1,-1});
	else
		return player:SendClientNotify("InvalidNpcOption",-1,-1)
	end

	return ResultCode_ResultOK

end