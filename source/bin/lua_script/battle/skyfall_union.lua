--天灾军团
--基本配置
-----------------------------------------------------------------------
local count_limit = 1	  							--组队要求玩家任务
local activity_type=ActivityType.SkyFallFight

--衰减经验系数
local reward_less_rate = 0.05							--每低一段衰减5%
--人数经验系数
local count_exp_rate=0.05

--20级胜利奖励池--20级胜利奖励池
local step1_reward_item={
{20230001,1,500},
{20230002,1,500},
{20230003,1,0},
{20236004,1,1000},
{20231001,1,100},
{20231002,1,100},
{20231003,1,100},
{20231004,1,100},
{20231005,1,100},
{20231006,1,100},
{20231007,1,100},
{20231008,1,100},
{20231009,1,100},
{20231010,1,100},
{20231011,1,100},
{20231012,1,100},
{20231013,1,100},
{20231014,1,100},
{20231015,1,100},
{20231016,1,100},
{20231017,1,100},
{20231018,1,100},
{20231019,1,100},
{20231020,1,100},
{20231021,1,100},
{20231022,1,100},
{20231023,1,100},
{20231024,1,100},
{20231025,1,100},
{20231026,1,100},
{20231027,1,100},
{30101001,1,100},
{30102001,1,100},
{30103001,1,100},
{30104001,1,100},
{30105001,1,100},
{30106001,1,100},
{30107001,1,100},
{30108001,1,100},
{30109001,1,100},
{20121001,1,1000},
}
--30级胜利奖励池
local step2_reward_item={
{20230001,1,200},
{20230002,1,400},
{20230003,1,400},
{20236004,1,1000},
{20231001,1,50},
{20231002,1,50},
{20231003,1,50},
{20231004,1,50},
{20231005,1,50},
{20231006,1,50},
{20231007,1,50},
{20231008,1,50},
{20231009,1,50},
{20231010,1,50},
{20231011,1,50},
{20231012,1,50},
{20231013,1,50},
{20231014,1,50},
{20231015,1,50},
{20231016,1,50},
{20231017,1,50},
{20231018,1,50},
{20231019,1,100},
{20231020,1,100},
{20231021,1,100},
{20231022,1,100},
{20231023,1,100},
{20231024,1,100},
{20231025,1,100},
{20231026,1,100},
{20231027,1,100},
{20231028,1,100},
{20231029,1,100},
{20231030,1,100},
{20231031,1,100},
{20231032,1,100},
{20231033,1,100},
{20231034,1,100},
{20231035,1,100},
{20231036,1,100},
{30101001,1,100},
{30102001,1,100},
{30103001,1,100},
{30104001,1,100},
{30105001,1,100},
{30106001,1,100},
{30107001,1,100},
{30108001,1,100},
{30109001,1,100},
{20121001,1,1000},
}
--40级胜利奖励池
local step3_reward_item={
{20230001,1,200},
{20230002,1,400},
{20230003,1,400},
{20236004,1,1000},
{20231001,1,30},
{20231002,1,30},
{20231003,1,30},
{20231004,1,30},
{20231005,1,30},
{20231006,1,30},
{20231007,1,30},
{20231008,1,30},
{20231009,1,30},
{20231010,1,30},
{20231011,1,30},
{20231012,1,30},
{20231013,1,30},
{20231014,1,30},
{20231015,1,30},
{20231016,1,30},
{20231017,1,30},
{20231018,1,30},
{20231019,1,40},
{20231020,1,40},
{20231021,1,40},
{20231022,1,40},
{20231023,1,40},
{20231024,1,40},
{20231025,1,40},
{20231026,1,40},
{20231027,1,40},
{20231028,1,100},
{20231029,1,100},
{20231030,1,100},
{20231031,1,100},
{20231032,1,100},
{20231033,1,100},
{20231034,1,100},
{20231035,1,100},
{20231036,1,100},
{20231037,1,100},
{20231038,1,100},
{20231039,1,100},
{20231040,1,100},
{20231041,1,100},
{20231042,1,100},
{20231043,1,100},
{20231044,1,100},
{20231045,1,100},
{30101001,1,100},
{30102001,1,100},
{30103001,1,100},
{30104001,1,100},
{30105001,1,100},
{30106001,1,100},
{30107001,1,100},
{30108001,1,100},
{30109001,1,100},
{20121001,1,1000},
}

--50级胜利奖励池
local step4_reward_item={
{20230001,1,200},
{20230002,1,400},
{20230003,1,400},
{20236004,1,1000},
{20231001,1,20},
{20231002,1,20},
{20231003,1,20},
{20231004,1,20},
{20231005,1,20},
{20231006,1,20},
{20231007,1,20},
{20231008,1,20},
{20231009,1,20},
{20231010,1,20},
{20231011,1,20},
{20231012,1,20},
{20231013,1,20},
{20231014,1,20},
{20231015,1,20},
{20231016,1,20},
{20231017,1,20},
{20231018,1,20},
{20231019,1,30},
{20231020,1,30},
{20231021,1,30},
{20231022,1,30},
{20231023,1,30},
{20231024,1,30},
{20231025,1,30},
{20231026,1,30},
{20231027,1,30},
{20231028,1,30},
{20231029,1,30},
{20231030,1,30},
{20231031,1,30},
{20231032,1,30},
{20231033,1,30},
{20231034,1,30},
{20231035,1,30},
{20231036,1,30},
{20231037,1,100},
{20231038,1,100},
{20231039,1,100},
{20231040,1,100},
{20231041,1,100},
{20231042,1,100},
{20231043,1,100},
{20231044,1,100},
{20231045,1,100},
{20231046,1,100},
{20231047,1,100},
{20231048,1,100},
{20231049,1,100},
{20231050,1,100},
{20231051,1,100},
{20231052,1,100},
{20231053,1,100},
{20231054,1,100},
{30101001,1,100},
{30102001,1,100},
{30103001,1,100},
{30104001,1,100},
{30105001,1,100},
{30106001,1,100},
{30107001,1,100},
{30108001,1,100},
{30109001,1,100},
{20121001,1,1000},
}

local reward_point = {2,2,2,2,2,2,2,2,2,2}

-----------------------------------------------------------------------

function CheckLevel(level,sceneid)
	local scenelevel = 0
	for k, group in pairs(g_skyfall_rule)	do	
		if sceneid == group[1] then
			scenelevel = group[5]
			break
		end
	end
	
	if level < scenelevel then
		return false,scenelevel
	end
	
	return true,scenelevel
end

function x001007_OnJoinCheck(player,params)
	return x001007_OnBattleCheck(player,params)
end

function x001007_OnBattleCheck(player,params)	

	local ac_config = LuaActivityManager():FindActivity(activity_type)
	if ac_config == nil then
		return ResultCode_InternalResult
	end
	local ac_name = ac_config:name()
	
	--团队检查
	if player:IsTeamGroup() then
		return player:SendClientNotify("ac_notify_003|"..ac_name,-1,1)
	end
	
	--队伍人数,允许单人
	local members= player:GetTeamMember(true)	
	if count_limit>1 then	
		--members = player:GetTeamMember(true)	
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
	local ok,scene_level= CheckLevel(player:PlayerLevel(),player:GetSceneId())
	if ok==false then	--{0}级才可以进行{1}
		return player:SendClientNotify("ac_notify_002|"..math.floor(scene_level).."|"..ac_name,-1,1)
	end
	
	--队员等级
	local names=""
	for i=1,#members,1 do
		if members[i] ~= player:Guid() then		
			local tplayer = LuaServer():FindPlayer(members[i])	--有可能这个时候其他人还在切换场景
			if tplayer == nil then
				return player:SendClientNotify("UnknownError",-1,1)
			end
			
			ok,scene_level= CheckLevel(tplayer:PlayerLevel(),tplayer:GetSceneId())
			if ok==false then
				names=names..tplayer:Name().." "
			end
		end
	end
	
	if names ~= "" then
		return player:SendClientNotify("ac_notify_001|"..names.."|"..math.floor(scene_level).."|"..ac_name,-1,1)
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



function x001007_OnGoBattle(battle_ground,params)
	
	local player = battle_ground:GetCreator()
	if player == nil then
		return ResultCode_BattleCreateFailed
	end

	local ret = x001007_OnBattleCheck(player,params)
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
		
	battle_ground:SetScriptId(1007)		
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
	
	
	--参与标记
	x000501_OnActivityJoin(player,activity_type)
	
	return ResultCode_ResultOK
end


function x001007_OnCreateReady(battle_ground,player)

	battle_ground:CreateAttackerPlayerBattleActors(true,false,ActorFigthFormation_AFF_NORMAL_HOOK)
		
	local npc_index = battle_ground:GetParams32("npc_index")
	--print("npc_index"..npc_index);
	
	local npc_config = LuaMtMonsterManager():FindDynamicNpc(npc_index);	
	local monster_group_size = npc_config:monster_group_size();
	
	--print("monster_group_size"..monster_group_size);

	if monster_group_size <= 0 then
		return ResultCode_UnknownError
	end
	
	--每次战斗都要随机一次monster group
	local seed = LuaServer():RandomNum(0, npc_config:monster_group_size() - 1)
	
	--print("seed"..seed);
	local monster_group_id = npc_config:monster_group(seed);
	
	--print("monster_group_id"..monster_group_id);
	
	local monster_group_config = LuaMtMonsterManager():FindMonsterGroup(monster_group_id);
	if monster_group_config == nil then
		return ResultCode_UnknownError
	end
	
	local infos={}
	for i = 0,monster_group_config:monsters_size() - 1 do
		local monster_define = monster_group_config:monsters(i)
		if (monster_define:id() > 0) then
			infos[monster_define:loc()]=monster_define:id()
			--battle_ground:CreateDefenderMonster(monster_define:loc(), monster_define:id())
		end
	end
	
	battle_ground:CreateDefenderMonster(infos)

	return ResultCode_ResultOK
end

function x001007_OnCheckBattleEnd(battle_ground)
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


function x001007_OnBattleReward(battle_ground,winnercamp)
	
	local winners = nil
	if winnercamp ==1 then
		winners=battle_ground:GetAttackerPlayers()
	else
		return false;
	end

	--计算奖励池
	local item_reward = GetItemReward(battle_ground:GetSceneId())
	if item_reward == nil then
		return false;
	end

	--奖励次数检查
	local ac_config = LuaActivityManager():FindActivity(activity_type)
	if ac_config == nil then
		return false
	end
	local rtimes = ac_config:reward_count()
	
	for k, player in pairs(winners)	do
		--超过限制次数就不再获得奖励了
		local done_times = player:OnLuaFunCall_1(501, "GetActivityTimes", activity_type)
		if rtimes > 0 and  done_times >= rtimes then
			--nothing todo
		else		
			--奖励道具
			local allweight = 0;
			for item, element in pairs(item_reward) do
				allweight=allweight+element[3];
			end

			local seed_weight = LuaServer():RandomNum(1,allweight)
			
			local base_weight = 0;
			for item, element in pairs(item_reward) do
				base_weight = base_weight+element[3]
				--print("lllllllllllllllllllllllllll..seed_weight "..seed_weight.." base_weight "..base_weight)
				if seed_weight <= base_weight  then
					player:AddItemByIdWithNotify(element[1],element[2],ItemAddLogType_AddType_Activity,activity_type,1)
					break;
				end
			end			

			--奖励
			local level_config = LuaMtActorConfig():GetLevelData(player:PlayerLevel())
			if level_config == nil then
				return false
			end
			
			--奖励衰减
			local less_value = CalcLessValue(player);		
			
			--
			local final_exp = less_value*level_config:skyfall_exp()			
			
			--人数加成
			local count_extra = #player:GetTeamMember(true)-1
			if count_extra<0 then
				count_extra=0
			end		
			final_exp = (1+count_extra*count_exp_rate) * final_exp			
			player:AddItemByIdWithNotify(TokenType_Token_Exp,final_exp,ItemAddLogType_AddType_Activity,activity_type,1)
			
			
			local final_gold = less_value*level_config:skyfall_gold()		
			player:AddItemByIdWithNotify(TokenType_Token_Gold,final_gold,ItemAddLogType_AddType_Activity,activity_type,1)			
				
			--	print("final_exp"..final_exp)
			--	print("final_gold"..final_gold)
				
			--积分
			if done_times>0 and done_times<=10 then
				player:AddItemByIdWithNotify(TokenType_Token_Reward,reward_point[done_times],ItemAddLogType_AddType_Activity,activity_type,1)
			end
			
			--活动次数更新活跃度
			player:OnLuaFunCall_1(501,"AddActivityTimes",activity_type)
			
			--掉落活动检查
			player:CheckAcrDrop(activity_type)
		end
				
		local npc_series_id = battle_ground:GetParams32("relate_npc")
		player:DelNpc(npc_series_id)
	end
	return true
end

function x001007_OnDamageRefix(battle_ground,damage_info,killer)

end


function x001007_OnDamage(battle, attacker, source_guid, target_guid, damage)
	return 0;
end

function x001007_OnBattleEnd(battle_ground,force,winnercamp)
	--通知客户端结束战斗
	battle_ground:BattleOverNotify(winnercamp)
	return true
end

function Level2Step(level)
	if level >=20 and level <=29 then
		return 1
	elseif level >=30 and level <=39 then
		return 2
	elseif level >=40 and level <=49 then
		return 3
	elseif level >=50 and level <=60 then
		return 4
	else
		return -1
	end
end

function GetSceneStep(sceneid)
	local scenelevel = 0
	for k, group in pairs(g_skyfall_rule)	do	
		if sceneid == group[1] then
			scenelevel = group[5]
			break
		end
	end
	
	return Level2Step(scenelevel)	
end

function CalcLessValue(player)
	local player_step = Level2Step(player:PlayerLevel())
	local scene_step = GetSceneStep(player:GetSceneId())
	
	if scene_step == -1 then
		scene_step = player_step
	end
	
	if player_step==-1 or scene_step == -1 then
		return 0
	end
	
	if player_step <= scene_step then
		return 1
	end
	
	--衰减计算
	local final = (1 - reward_less_rate)*(player_step-scene_step)
	--print("final"..final)
	if final <=0 then
		final=0
	end
	
	return final	
end

function GetItemReward(sceneid)
	local step = GetSceneStep(sceneid)
	if step == 1 then
		return step1_reward_item
	elseif step == 2 then
		return step2_reward_item
	elseif step == 3 then
		return step3_reward_item
	elseif step == 4 then
		return step4_reward_item
	else
		return nil
	end
end



function x001007_ResolveAllReward(player)
	
	local level_config = LuaMtActorConfig():GetLevelData(player:PlayerLevel())
	if level_config == nil then
		return 0,0
	end
	
	local ac_config = LuaActivityManager():FindActivity(activity_type)
	if ac_config == nil then
		return 0,0
	end
		
	--奖励衰减
	local less_value = CalcLessValue(player)	
	local exp = level_config:skyfall_exp()		
	local gold = level_config:skyfall_gold()	
	
	local final_gold = 0
	local final_exp = 0
	
	local reward_count = ac_config:reward_count();
	for i=1,reward_count,1 do
		final_exp = final_exp + less_value*exp
		final_gold = final_gold + less_value*gold
	end
	
	return final_gold,final_exp
end	
