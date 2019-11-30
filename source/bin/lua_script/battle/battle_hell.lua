

local activity_type=ActivityType.Hell


function x001013_OnJoinCheck(player,params)
	
	local ac_config = LuaActivityManager():FindActivity(activity_type)
	if ac_config == nil then
		return ResultCode_InternalResult
	end
	
	local ac_name = ac_config:name()

	--等级检查
	if player:PlayerLevel()<ac_config:open_level() then
		return player:SendClientNotify("ac_notify_002|"..math.floor(ac_config:open_level()).."|"..ac_name,-1,1)
	end
	
	--队伍检查
	if player:GetTeamID() ~= 0 then
		return player:SendClientNotify("ac_notify_004|"..ac_name,-1,1)
	end
	--团队检查
	if player:IsTeamGroup() then
		return player:SendClientNotify("ac_notify_003|"..ac_name,-1,1)
	end
	
	--次数判定,只有自己
	local ltimes = ac_config:limit_times()
	local rtimes = ac_config:reward_count();
	local done_times = player:OnLuaFunCall_1(501, "GetActivityTimes", activity_type)
	if ltimes >= 0 then
		if done_times >= ltimes then	--今日你已完成{0}，明日再来吧											
			return player:SendClientNotify("ac_notify_007|"..ac_name,-1,1)			
		end
	end	

	--if rtimes > 0 and  done_times >= rtimes then		--今日你已完成{0}，再次进入将不再获取任何奖励
	--	player:SendClientNotify("ac_notify_008|"..ac_name,-1,1)
	--end		

	return ResultCode_ResultOK
end


function x001013_OnBattleCheck(player,params)
	return ResultCode_ResultOK
end


function x001013_OnGoBattle(battle_ground,params)
	
	local player = battle_ground:GetCreator()
	if player == nil then
		return ResultCode_BattleCreateFailed
	end

	local battlegroup = LuaSceneManager():GetSceneBattleGroupById(player:GetSceneId(),params["group_id"]);
	if battlegroup == nil then
		return ResultCode_InvalidConfigData
	end
	
	player:StopMove()			
			
	battle_ground:SetScriptId(1013)		
	
	local brief = battle_ground:Brief()
	brief:set_scene_id(player:GetSceneId())
	brief:set_battle_group_id(params["group_id"])		
	brief:set_type(BattleGroundType_PVP_TRIAL_FIELD)
	
	if battle_ground:Init() == false then
		return ResultCode_BattleCreateFailed
	end		
	
	battle_ground:BattleEnterNotify()
	
	--记录标记
	local dc_container = player:DataCellContainer()
	dc_container:set_bit_data(BitFlagCellIndex_DayTriFieldFlag,true)
	
	return ResultCode_ResultOK
end

function x001013_OnCreateReady(battle_ground, player)
	
	battle_ground:CreateAttackerPlayerBattleActors(false,false,ActorFigthFormation_AFF_PVP_TRIAL_FIELD)	
	
	--月卡
	local dc_container = player:DataCellContainer()
	
	--血量继承
	local attackers = battle_ground:GetAttackers()
	for i,actor in pairs(attackers) do		
		local lasthp = actor:GetLastHp(LastHpType_HellLastHp)
		if dc_container:check_bit_data(BitFlagCellIndex_MonthCardFlag) then
			lasthp=(1.0+0.05)*lasthp
		end	
	
		actor:SetHp(lasthp)		
	end
	
	battle_ground:CreateDefenderFromHell()
	--血量继承
	local defenders = battle_ground:GetDefenders()
	for i,actor in pairs(defenders) do
		actor:SetHp(actor:GetLastHp(LastHpType_HellLastHp))	
	end
	
	--清理尸体
	battle_ground:CleanDeadBattleActors()
	
	return ResultCode_ResultOK;	
end

function x001013_OnDamageRefix(battle_ground,damage_info,killer)

end

function x001013_OnDamage(battle, attacker, source_guid, target_guid, damage)
	return 0;
end

function x001013_OnCheckBattleEnd(battle_ground)
	if battle_ground:TimeOut() then
		--策划要求超时需要血量置0
		local attackers = battle_ground:GetAttackers()
		for i,actor in pairs(attackers) do
			--受db初始化值0的影响，这里用-1表示死亡状态
			actor:SetHp(0)
		end
		local creator = battle_ground:GetCreator()
		local target = creator:GetPlayerNpc(creator:GetParams64("target_guid"))
		if target ~= nil then target:set_passed(true) end
		return 2;
	end
	local DeadCampID=battle_ground:GetAllDeadCampID()
	if DeadCampID == 1 then
		return 2;
	elseif DeadCampID == 2 then
		return 1;
	end
	return 0;
end


function x001013_SavePlayerLastHp(battle_ground,player)
	local attackers = battle_ground:GetAttackers()
	for i,actor in pairs(attackers) do
		if actor:Hp()<=0 then		
			--受db初始化值0的影响，这里用-1表示死亡状态
			player:SavePlayerLastHp(actor:Guid(),LastHpType_HellLastHp,-1)
		else
			player:SavePlayerLastHp(actor:Guid(),LastHpType_HellLastHp,actor:Hp())
		end
	end
end

function x001013_SavePlayerNpcLastHp(battle_ground,player,target_player)
	local defenders = battle_ground:GetDefenders()
	for i,actor in pairs(defenders) do	
		for k=0,target_player:actors_size()-1,1 do
			local tactor = target_player:mutable_actors(k)
			local dbinfo = tactor:mutable_actor()
			if dbinfo:guid() == actor:Guid() then
				local final_hp =actor:Hp()
				if final_hp < 0 then
					--受db初始化值0的影响，这里用-1表示死亡状态
					final_hp=-1
				end				
				if dbinfo:last_hp_size() >0 then
					for j=0,dbinfo:last_hp_size()-1,1 do
						dbinfo:set_last_hp(j,final_hp)
					end
				else
					for j=0,LastHpType_LastHpMax-1,1 do
						dbinfo:add_last_hp(final_hp)
					end
				end
			end
		end
	end
end

function x001013_OnBattleReward(battle_ground, winnercamp)	
	
	local creator = battle_ground:GetCreator()
	local target = creator:GetPlayerNpc(creator:GetParams64("target_guid"))
	if creator ~= nil and target ~= nil then 
		x001013_SavePlayerNpcLastHp(battle_ground,creator,target)
		x001013_SavePlayerLastHp(battle_ground,creator)
		creator:UpdateFormationLastHpSet(ActorFigthFormation_AFF_PVP_TRIAL_FIELD,LastHpType_HellLastHp)			
		if winnercamp == 1 then
			target:set_passed(true)		

			--目标更新
			local done_times = creator:OnLuaFunCall_1(501, "GetActivityTimes", activity_type)
			x000504_xOnHellBattleWin(creator,{["value"]=done_times+1})			
		end	
		--更新客户端数据
		creator:RefreshPlayerNpc(true)				
	end
	return true
end

function x001013_OnBattleEnd(battle_ground,force, winnercamp)	
	--通知客户端结束战斗
	battle_ground:BattleOverNotify(winnercamp)
	
	return true
end
