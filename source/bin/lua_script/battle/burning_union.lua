--燃烧军团
--基本配置
-----------------------------------------------------------------------
local activity_type=ActivityType.BurningFight
local count_limit = 3	 --组队要求玩家

--胜利奖励池,暂时没用
local tb_reward_item={
{20236018,1,2000},
{20236019,1,2000},
{20236020,1,2000},
{20236021,1,50},
{20236022,1,50},
{28810002,4,100},
{28810020,4,100},
{28810004,4,100},
{28810018,4,100},
{28810027,4,100},
{20233005,1,500},
{20233004,1,2000},
{28810013,4,200},
{28810015,4,200},
{28810019,4,200},
{28810031,4,200},
}


-----------------------------------------------------------------------
function x001006_OnJoinCheck(player,params)
	return x001006_OnBattleCheck(player,params)
end

function x001006_OnBattleCheck(player,params)	
	
	local ac_config = LuaActivityManager():FindActivity(activity_type)
	if ac_config == nil then
		return ResultCode_InternalResult
	end
	local ac_name = ac_config:name()
	
	--团队检查
	if player:IsTeamGroup() then
		return player:SendClientNotify("ac_notify_003|"..ac_name,-1,1)
	end
	
	local one_p = player:GetParams32("one_p")
	if one_p == 1 then
		count_limit = 1
	else
		count_limit = 3
	end
	
	--队伍人数,允许单人
	local members ={}
	if count_limit>1 then	
		members = player:GetTeamMember(true)	
		if #members < count_limit then
			return player:SendClientNotify("ac_notify_000|"..math.floor(count_limit).."|"..ac_name,-1,1)
		end	
	
		--队长检查
		if player:IsTeamLeader()== false then
			return player:SendClientNotify("Team_NotLeader",-1,1)
		end
	end
	
	--等级检查	
	local scene = player:Scene()
	if scene == nil then
		return player:SendClientNotify("UnknownError",-1,1)
	end
	
	--队长等级
	if player:PlayerLevel()<ac_config:open_level() then	--{0}级才可以进行{1}
		return player:SendClientNotify("ac_notify_002|"..math.floor(ac_config:open_level()).."|"..ac_name,-1,1)
	end
	
	--队员等级
	local names=""
	for i=1,#members,1 do
		if members[i] ~= player:Guid() then		
			local tplayer = LuaServer():FindPlayer(members[i])	--有可能这个时候其他人还在切换场景
			if tplayer == nil then
				return player:SendClientNotify("UnknownError",-1,1)
			end
			if tplayer:PlayerLevel()<ac_config:open_level() then
				names=names..tplayer:Name().." "
			end
		end
	end
	
	if names ~= "" then
		return player:SendClientNotify("ac_notify_001|"..names.."|"..math.floor(ac_config:open_level()).."|"..ac_name,-1,1)
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

	if rtimes > 0 and  done_times >= rtimes then		--今日你已完成{0}，再次进入将不再获取任何奖励
		player:SendClientNotify("ac_notify_008|"..ac_name,-1,1)
	end	
	
	return ResultCode_ResultOK
end


function x001006_OnGoBattle(battle_ground,params)

	local player = battle_ground:GetCreator()
	if player == nil then
		return ResultCode_BattleCreateFailed
	end
	
	local ret = x001006_OnBattleCheck(player,params)
	if ret ~= ResultCode_ResultOK then
		return ret
	end

	local layout = LuaMtMonsterManager():FindScenePos(params["pos_id"])
	if layout == nil then
		return ResultCode_InvalidConfigData
	end
	
	if layout:scene_id() ~= player:GetSceneId() then
		return ResultCode_InvalidConfigData
	end
	params["group_id"]=layout:group_index()		
	local battlegroup = LuaSceneManager():GetSceneBattleGroupById(player:GetSceneId(),params["group_id"]);
	if battlegroup == nil then
		return ResultCode_InvalidConfigData
	end
			
	player:StopMove()	
		
	battle_ground:SetScriptId(1006)		
	battle_ground:SetParams32("relate_npc",params["relate_npc"])		
	battle_ground:SetParams32("npc_index",params["npc_index"])		
	
	local brief = battle_ground:Brief()
	brief:set_scene_id(player:GetSceneId())
	brief:set_battle_group_id(params["group_id"])		
	brief:set_type(BattleGroundType_PVE_SINGLE)
	
	if battle_ground:Init() == false then
		return ResultCode_BattleCreateFailed
	end		
	
	battle_ground:BattleEnterNotify()
	
	return ResultCode_ResultOK
end

function x001006_OnCreateReady(battle_ground,player)

	battle_ground:CreateAttackerPlayerBattleActors(true,true,ActorFigthFormation_AFF_NORMAL_HOOK)
	local npc_index = battle_ground:GetParams32("npc_index")
	--print("npc_index"..npc_index);
	
	local npc_config = LuaMtMonsterManager():FindDynamicNpc(npc_index);	
	local monster_group_size = npc_config:monster_group_size();
	
	--print("monster_group_size"..monster_group_size);

	if monster_group_size <= 0 then
		return ResultCode_UnknownError;	
	end
	
	--每次战斗都要随机一次monster group
	local seed = LuaServer():RandomNum(0,npc_config:monster_group_size() - 1)
	
	--print("seed"..seed);
	local monster_group_id = npc_config:monster_group(seed);
	
	--print("monster_group_id"..monster_group_id);
	
	local monster_group_config = LuaMtMonsterManager():FindMonsterGroup(monster_group_id);
	if monster_group_config == nil then
		return ResultCode_UnknownError;	
	end
	
	local infos={}
	for i = 0,monster_group_config:monsters_size() - 1 do
		local monster_define = monster_group_config:monsters(i)
		if (monster_define:id() > 0) then
			infos[monster_define:loc()]=monster_define:id()
		end
	end
	battle_ground:CreateDefenderMonster(infos)

	return ResultCode_ResultOK
end

function x001006_OnDamageRefix(battle_ground,damage_info,killer)

end

function x001006_OnDamage(battle, attacker, source_guid, target_guid, damage)
	return 0;
end


function x001006_OnCheckBattleEnd(battle_ground)
	if battle_ground:TimeOut() then
		return 2;
	end
	local DeadCampID=battle_ground:GetAllDeadCampID();
	if DeadCampID == 1 then
		return 2;
	elseif DeadCampID == 2 then
		return 1;
	end
	return 0;
end


function x001006_OnBattleReward(battle_ground,winnercamp)	
	
	local winners = nil
	if winnercamp ==1 then
		winners=battle_ground:GetAttackerPlayers()
	else
		return false
	end

	--奖励次数检查
	local ac_config = LuaActivityManager():FindActivity(activity_type)
	if ac_config == nil then
		return false
	end
	local rtimes = ac_config:reward_count()
	
	local all_names = ""
	
	for k, player in pairs(winners)	do		
		if k > 1 then
			all_names = all_names..","
		end
		all_names = all_names..player:Name()
		--超过限制次数就不再获得奖励了
		local done_times = player:OnLuaFunCall_1(501, "GetActivityTimes", activity_type)
		if rtimes > 0 and  done_times >= rtimes then
			--nothing todo
		else			
			--奖励道具		
			
			
			--背包检查
			if player:BagLeftCapacity(ContainerType_Layer_EquipAndItemBag,1) == false then		
				player:SendClientNotify("BagIsFull",-1,-1)
			end
			
			local tb_item = LuaMtDrawManager():GetDrawList(DrawGroup_HellBigDraw,player:PlayerLevel())

			local allweight = 0;
			for key,iter in pairs(tb_item)	do
				allweight=allweight+iter:item_weight()
			end				

			local seed_weight = LuaServer():RandomNum(1,allweight)
			local base_weight = 0;
			for key,iter in pairs(tb_item)	do
				base_weight = base_weight+iter:item_weight()
				if  seed_weight<=base_weight then
					player:AddItemByIdWithNotify(iter:item_id(),iter:item_count(),ItemAddLogType_AddType_Activity,activity_type,1)
					break
				end
			end
	
			
			--经验奖励
			local level_config = LuaMtActorConfig():GetLevelData(player:PlayerLevel())
			if level_config == nil then
				return false
			end
			
			--奖励
			player:AddItemByIdWithNotify(TokenType_Token_Exp,level_config:burning_exp(),ItemAddLogType_AddType_Activity,activity_type,1)
			player:AddItemByIdWithNotify(TokenType_Token_Gold,level_config:burning_gold(),ItemAddLogType_AddType_Activity,activity_type,1)
			
		
			--活动次数更新活跃度
			player:OnLuaFunCall_1(501,"AddActivityTimes",activity_type)
			
			--掉落活动检查
			player:CheckAcrDrop(activity_type)
	
		end		
		
		local npc_series_id = battle_ground:GetParams32("relate_npc")
		player:DelNpc(npc_series_id)
		
		--更新目标
		x000504_xOnBurningBattleWin(player,{["value"]=-1})

	end
	
	
		
	--广播
	if all_name == "" then
		all_name ="???"
	end
	local npc_index = battle_ground:GetParams32("npc_index")
	local npc_config = LuaMtMonsterManager():FindDynamicNpc(npc_index)
	if npc_config ~= nil then
		Lua_send_run_massage("ac_notify_040|"..all_names.."|monster_name_"..math.floor(npc_config:npc_id()))
	end
		
	return true
end

function x001006_OnBattleEnd(battle_ground,force,winnercamp)
	--通知客户端结束战斗
	battle_ground:BattleOverNotify(winnercamp)
	return true
end
