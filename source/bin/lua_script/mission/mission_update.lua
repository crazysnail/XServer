
local g_ActionType ={}
--特写
g_ActionType.GetHero = 0;		--招募
g_ActionType.FightHero = 1;		--上阵
g_ActionType.OnEquip = 2;		--穿装备
g_ActionType.Enhance = 3;		--强化
g_ActionType.ColorUp = 4;		--进阶
g_ActionType.SkillUp = 5;		--技能升级
g_ActionType.Hero8Equips =6; 	--8件装备
g_ActionType.ParterEquip =7;  	--两个同伴快捷装备后完成
g_ActionType.RobotTeam =8;  	--创建团队，补齐9个角色
--通用
g_ActionType.SkillLevel 		= 1000;		--n个技能至少升到n级
g_ActionType.HeroEquip 			= 1001;		--n个英雄穿戴n件n品质以上装备
g_ActionType.HeroColor 			= 1002;		--n个英雄至少进阶到n品质
g_ActionType.GuildSkillLevel 	= 1003;		--n个工会技能至少升到n级
g_ActionType.HeroMaster 		= 1004;		--n个英雄激活n级大师等级
g_ActionType.EquipEnchant 		= 1005;		--n件装备至少拥有n条附魔属性

function x000502_OnPrepare(player,config,mission_data)

	--多值进度初始为0
	mission_data:set_param(MissionDB_Param_MultiFinish_Value1,0)
	mission_data:set_param(MissionDB_Param_MultiFinish_Value2,0)
	mission_data:set_param(MissionDB_Param_MultiFinish_Value3,0)
	mission_data:set_param(MissionDB_Param_MultiFinish_Value4,0)	
	mission_data:set_param(MissionDB_Param_Finish_Value,0)
	
	--动态提交npc
	if config:commit_npc_id()>10000 then
		local npc_node = LuaMtMonsterManager():FindDynamicNpc(config:commit_npc_id())
		if npc_node ~= nil then		
			local series_id = player:AddNpc(npc_node:pos_id(),npc_node:index(),-1)
			mission_data:set_param(MissionDB_Param_Commit_Npc_Series,series_id)
		end
	end
	--龙套npc
	if config:other_npc_group()>0 then
		player:AddOtherNpc(config:other_npc_group(),-1);
	end
	
	--副本任务需要记录rtid
	local mission_type = config:type()	
	if	x000501_IsRaidMission(player,mission_type) == 1 then
		local raid = player:GetRaid()
		if raid ~= nil then
			--print("xxxxxxxxxxxxxxxxxxxxx"..raid:RuntimeId())
			mission_data:set_param(MissionDB_Param_CarbonRuntimeId,raid:RuntimeId())
		end
	end
	
	--送信任务
	local finish_type = config:finish_type()
	if finish_type==FinishType_MailItem then	
		player:AddItemByIdWithNotify(config:param(0),1,ItemAddLogType_AddType_Mission,config:id(),1)
		--比较特殊，此时任务还没创建结束，additembyid是不知道这个任务的存在的，特殊处理一下状态
		mission_data:set_state(MissionState_MissionComplete)
	
	--帮助npc击杀怪物任务
	elseif finish_type ==FinishType_KillNpc then
		--print("FinishType_KillNpc")		
		mission_data:set_param(MissionDB_Param_Npc_Index,config:param(0))				--index
		mission_data:set_param(MissionDB_Param_Monster_Group,config:param(1))			--monster_group_id		
		local pos_info = LuaMtMonsterManager():FindStaticNpcPosInfo(config:param(0))
		mission_data:set_param(MissionDB_Param_Pos_Id,pos_info:id())					--pos_id
		mission_data:set_param(MissionDB_Param_Npc_Series,-1)							--动态npc流水号
	
	--随机npc击杀任务	
	elseif 	finish_type ==FinishType_RandomNpc then
		--print("FinishType_RandomNpc")
		local npc_node_list = LuaMtMonsterManager():GenRandomNpc(config:param(0),1)
		mission_data:set_param(MissionDB_Param_Npc_Index,npc_node_list[1]:index())			--index		
		--随机生成monstergroup
		local monster_group_size = npc_node_list[1]:monster_group_size()
		if monster_group_size > 0 then		
			local seed = LuaServer():RandomNum(0, npc_node_list[1]:monster_group_size() - 1)
			local monster_group_id = npc_node_list[1]:monster_group(seed)			
			mission_data:set_param(MissionDB_Param_Monster_Group,monster_group_id)			--monster_group_id			
		end		
		
		local pos_node_list = LuaMtMonsterManager():GenRandomPos(config:param(1),1)		
		mission_data:set_param(MissionDB_Param_Pos_Id,pos_node_list[1]:pos_id())			--pos_id		
		--创建动态npc
		series_id = player:AddNpc(pos_node_list[1]:pos_id(),npc_node_list[1]:index(),-1)
		mission_data:set_param(MissionDB_Param_Npc_Series,series_id)						--动态npc序列号
	
	--巡逻任务
	elseif finish_type ==FinishType_Ratrol then
	
		--print("FinishType_Ratrol")
		mission_data:set_param(MissionDB_Param_Npc_Index,-1)					--index
		local npc_node_list = LuaMtMonsterManager():GenRandomNpc(config:param(0),1)
		--随机生成monstergroup
		local monster_group_size = npc_node_list[1]:monster_group_size()
		if monster_group_size > 0 then
			local seed = LuaServer():RandomNum(0, npc_node_list[1]:monster_group_size() - 1)
			local monster_group_id = npc_node_list[1]:monster_group(seed)			
			mission_data:set_param(MissionDB_Param_Monster_Group,monster_group_id)			--monster_group_id
			--print("monster_group_id"..monster_group_id)
		end		
		
		local pos_node_list = LuaMtMonsterManager():GenRandomPos(config:param(1),1)		
		mission_data:set_param(MissionDB_Param_Pos_Id,pos_node_list[1]:pos_id())			--pos_id	
		
		mission_data:set_param(MissionDB_Param_Npc_Series,-1)								--动态npc序列号
				
	--使用道具
	elseif finish_type ==FinishType_UseItem then
	
		--print("FinishType_UseItem")
		mission_data:set_param(MissionDB_Param_Pos_Id,config:param(1))									--pos_id		
		player:AddItemByIdWithNotify(config:param(0),1,ItemAddLogType_AddType_Mission,config:id(),1)
	--采集道具
	elseif finish_type ==FinishType_CollectItem then	
		--print("FinishType_CollectItem")
		mission_data:set_param(MissionDB_Param_Pos_Id,config:param(1))									--pos_id
		
	--副本传送
	elseif finish_type ==FinishType_EnterRaid then	
		--print("FinishType_EnterRaid")
		mission_data:set_param(MissionDB_Param_Npc_Index,config:param(2))								--静态npc序列号
		mission_data:set_param(MissionDB_Param_CarbonRuntimeId,0)										--保证入口任务每次都是新的，不然类似三环任务会进入前一个销毁状态的副本
	--提交物品
	elseif finish_type ==FinishType_CommitItem then	
		--print("FinishType_CommitItem")
		if player:GetBagItemCount(config:param(0)) >= 1 then
			mission_data:set_param(MissionDB_Param_Finish_Value,config:param(1))
			mission_data:set_state(MissionState_MissionComplete)
		end			
	--升级
	elseif finish_type ==FinishType_LevelUp then	
		--print("FinishType_LevelUp")
		if player:PlayerLevel() >= config:param(0) then
			mission_data:set_state(MissionState_MissionComplete)
		end
	elseif finish_type ==FinishType_FinishText then
		mission_data:set_state(MissionState_MissionComplete)
	elseif finish_type ==FinishType_GenTarget then
		mission_data:set_state(MissionState_MissionComplete)
	elseif finish_type == FinishType_Action then
		--这类任务需要进场景后做resolve检查，不能在创建阶段做完成处理，因为这个时候任务还没创建完
	--特殊任务
	elseif config:id() == 9901 or config:id() == 9951 then	
		local default_stage_id = LuaMtSceneStageManager():DefaultStageId(player:GetCamp());
		local first_stage = player:GetPlayerStage(default_stage_id);
		if first_stage ~= nil then
			first_stage:set_boss(5)
			player:GetPlayerStageProxy():SyncSpecStageToClient(default_stage_id)
		end			
		
	end	
	
	return ResultCode_ResultOK
end



function x000502_OnDrop(player,config,mission_data)
	if config:can_drop() ~= 1 then
		return ResultCode_DropLimit	
	end
	--多值进度初始为0
	mission_data:set_param(MissionDB_Param_MultiFinish_Value1,0)
	mission_data:set_param(MissionDB_Param_MultiFinish_Value2,0)
	mission_data:set_param(MissionDB_Param_MultiFinish_Value3,0)
	mission_data:set_param(MissionDB_Param_MultiFinish_Value4,0)	
	mission_data:set_param(MissionDB_Param_Finish_Value,0)

	
	--动态提交npc
	if config:commit_npc_id()>10000 then
		local series_id = mission_data:param(MissionDB_Param_Commit_Npc_Series)
		player:DelNpc(series_id)
	end
	
	--print("fffffffffffffffffffffffffffffffffff")
	
	--龙套npc
	if config:other_npc_group()>0 then
		player:DelNpcByGroup(config:other_npc_group())
	end
	
	--副本任务需要记录rtid
	local mission_type = config:type();
	if	x000501_IsRaidMission(player,mission_type) == 1 then
		local raid = player:GetRaid()
		if raid ~= nil then
			local scene = raid:Scene()
			if scene:SceneId() == player:GetSceneId() then
				return player:SendClientNotify("CantDropInRaid",-1,1)
			end
			local msg = S2SCommonMessage:new()
			msg:set_request_name("S2SRaidDestroy")
			local rtid = mission_data:param(MissionDB_Param_CarbonRuntimeId)
			--print("dddddddddddd"..rtid)
			msg:add_int64_params(rtid)
			scene:ExecuteMessage(msg)
			--msg:delete()
		end
	end	
	
	local finish_type = config:finish_type()
	if finish_type==FinishType_MailItem then	  	--送信任务
		player:DelItemById(config:param(0),1,ItemDelLogType_DelType_Mission,0)		
	elseif finish_type ==FinishType_KillNpc then   	--帮助npc击杀怪物任务	
		--nothing todo	
	elseif 	finish_type ==FinishType_RandomNpc then --随机npc击杀任务	
		player:DelNpc(series_id)		
	elseif finish_type ==FinishType_Ratrol then		--巡逻任务
		--nothing todo	
	elseif finish_type ==FinishType_UseItem then	--使用道具
		player:DelItemById(config:param(0),1,ItemDelLogType_DelType_Mission,0)		
	elseif finish_type ==FinishType_CollectItem then--采集道具	
		--nothing todo			
	elseif finish_type ==FinishType_EnterRaid then	--副本传送
		--nothing todo			
	elseif finish_type ==FinishType_CommitItem then	--提交物品
		--nothing todo		
	elseif finish_type ==FinishType_LevelUp then	--升级
		--nothing todo		
	elseif finish_type ==FinishType_FinishText then
		--nothing todo		
	elseif finish_type ==FinishType_GenTarget then
		--nothing todo		
	end		
	
	return ResultCode_ResultOK
end


function x000502_OnCollectOver(player,id)

	local mission_map=player:GetMissionProxy():GetMissionMap()

	for k, mission in pairs(mission_map)
	do
		local config = mission:Config()
		local mission_data = mission:MissionDB()		
		
		if config:finish_type()==FinishType_CollectItem and player:ValidPosition(config:param(1),2500) then		
			mission_data:set_param(MissionDB_Param_Finish_Value,1)
			mission_data:set_state(MissionState_MissionComplete)
			if config:commit_npc_id() <0 then
				--没有提交npc立即完成
				player:GetMissionProxy():OnFinishMission(config:id())					
			else
				--需要通过对话提交
				player:GetMissionProxy():OnUpdateMission(config:id())
			end			
		end
	end
	
	return ResultCode_ResultOK

end


------------------------------------------------------------------------
function x000502_OnRegisterFunction(proxy)
	
	if proxy ~= nil then
		proxy:RegFunc("xOnLevelUpTo"		,502)
		proxy:RegFunc("xOnMonsterDead"		,502)
		proxy:RegFunc("xOnUseItem"			,502)
		proxy:RegFunc("xOnAddItem"			,502)
		proxy:RegFunc("xRobotTeamOk"		,502)
		proxy:RegFunc("xOnUpgradeColor"		,502)
		proxy:RegFunc("xAddEquipment"		,502)
		proxy:RegFunc("xAddHero"			,502)
		proxy:RegFunc("xOnSetActorFight"	,502)
		proxy:RegFunc("xOnUpgradeSKill"		,502)
		proxy:RegFunc("xOnEnhenceEquip"		,502)
		proxy:RegFunc("xOnInsetEquip"		,502)
		proxy:RegFunc("xOnAddPracticeLevel"	,502)
		proxy:RegFunc("xOnEnchantEquip"		,502)		
	end
end


function x000502_xOnAddItem(player,pramas)

	local mission_map=player:GetMissionProxy():GetMissionMap()
	local id = pramas["value"]

	for k, mission in pairs(mission_map) do
		local config = mission:Config()
		local mission_data = mission:MissionDB()
		
		if config:finish_type()==FinishType_CommitItem or config:finish_type() ==FinishType_MailItem then		
	
			if id == config:param(0) then
				--print (config:param(0))
				mission_data:set_param(MissionDB_Param_Finish_Value,1)
				mission_data:set_state(MissionState_MissionComplete)
				if config:commit_npc_id() <0 then
					--没有提交npc立即完成
					player:GetMissionProxy():OnFinishMission(config:id())					
				else
					--需要通过对话提交
					player:GetMissionProxy():OnUpdateMission(config:id())
				end	
			end
		end
	end
	
	return ResultCode_ResultOK	

end

function x000502_xOnUseItem(player,pramas)
	local id = pramas["value"]
	local mission_map=player:GetMissionProxy():GetMissionMap()
	for k, mission in pairs(mission_map) do
		local config = mission:Config()
		local mission_data = mission:MissionDB()
		if config:finish_type()==FinishType_UseItem then
			if id == config:param(0) then
				mission_data:set_param(MissionDB_Param_Finish_Value,1)
				mission_data:set_state(MissionState_MissionComplete)
				if config:commit_npc_id() <0 then
					--没有提交npc立即完成
					player:GetMissionProxy():OnFinishMission(config:id())					
				else
					--需要通过对话提交
					player:GetMissionProxy():OnUpdateMission(config:id())
				end	
			end
		end
	end
	
	return ResultCode_ResultOK

end

function x000502_OnDamage(battle, attacker, source_guid, target_guid, damage)
	return 0;
end

function x000502_xOnMonsterDead(player,params)
	if player == nil then
		return ResultCode_InvalidPlayer
	end	
	
	local monster_id=params["monster_id"]
	local stage_id=params["stage_id"]
	local scene_id=params["scene_id"]
	local monster_class=params["monster_class"]
	local battle_type=params["battle_type"]
	local new_count =params["monster_count"]
	
	local mission_map=player:GetMissionProxy():GetMissionMap()
	local base_param_offset= MissionDB_Param_MultiFinish_Value1
    for k, mission in pairs(mission_map)  do
		local config = mission:Config();		
		local missiondb = mission:MissionDB();
		
		----print("xxxxxxlua-----SubFinishType_KillAndGetWithMonster"..config:finish_type())			
		
		if config:finish_type()==FinishType_KillMonster then
			local exconfig = mission:ExConfig();	
			----print("xxxxxxlua-----SubFinishType_KillAndGetWithMonster")			
			if exconfig~=nil then
				----print("xxxxxxlua-----SubFinishType_KillAndGetWithMonster")
				if exconfig:sub_type()== SubFinishType_KillAndGetWithMonster then		--杀指定的怪取物		
					--print("xxxxxxlua-----SubFinishType_KillAndGetWithMonster")
					for i=0,exconfig:monster_size()-1,1 do						
						local old_count =missiondb:param(i+base_param_offset);
						----print("exconfig:monster(i)")
						local rate = 10000
						if exconfig:rate_size()>0 then
							rate = exconfig:rate(i)
						end
						if monster_class == exconfig:monster(i) and old_count < exconfig:count(i) and exconfig:rate_size()>0 then
							if rate<10000 then
								local seed = LuaServer():RandomNum(1,10000)
								if seed<=exconfig:rate(i) then	
									local count = old_count+new_count;
									missiondb:set_param(i+base_param_offset,count);
									player:GetMissionProxy():OnUpdateMission(config:id())
									
									--print("config:id()"..config:id())
								end
							else
								local count = old_count+new_count;
								missiondb:set_param(i+base_param_offset,count);
								player:GetMissionProxy():OnUpdateMission(config:id())
								
								--print("config:id()"..config:id())
							end
						end
					end
				elseif exconfig:sub_type()== SubFinishType_KillAndGetWithStage then		--关卡掉落取物
					if stage_id == exconfig:pos_param() then
						--print("xxxxxxlua-----SubFinishType_KillAndGetWithStage")
						for i=0,exconfig:count_size()-1,1 do
							local old_count =missiondb:param(i+base_param_offset);
							if old_count < exconfig:count(i) then
								if exconfig:rate(i)<10000 then		
									local seed = LuaServer():RandomNum(1,10000)
									if seed<=exconfig:rate(i) then	
										local count = old_count+new_count;
										missiondb:set_param(i+base_param_offset,count);
										player:GetMissionProxy():OnUpdateMission(config:id())
									end
								else
									local count = old_count+new_count;
									missiondb:set_param(i+base_param_offset,count);
									player:GetMissionProxy():OnUpdateMission(config:id())
								end
							end
						end
					end
				elseif exconfig:sub_type()== SubFinishType_KillAndGetWithScene then		--场景掉落取物	
					if scene_id == exconfig:pos_param() then
						--print("xxxxxxlua-----SubFinishType_KillAndGetWithScene")
						for i=0,exconfig:count_size()-1,1 do
							local old_count =missiondb:param(i+base_param_offset);
							if old_count < exconfig:count(i) then
								----print("exconfig:rate(i)"..exconfig:rate(i))
								if exconfig:rate(i)<10000 then					
									local seed = LuaServer():RandomNum(1,10000)
									if seed<=exconfig:rate(i) then	
										local count = old_count+new_count;
										missiondb:set_param(i+base_param_offset,count);
										player:GetMissionProxy():OnUpdateMission(config:id())
									end
								else
									local count = old_count+new_count;
									missiondb:set_param(i+base_param_offset,count);
									player:GetMissionProxy():OnUpdateMission(config:id())
								end
							end
						end
					end
				elseif exconfig:sub_type()== SubFinishType_KillMonsterFinish then		--击杀怪物完成
					--print("xxxxxxlua-----SubFinishType_KillMonsterFinish")
					for i=0,exconfig:monster_size()-1,1 do
						local old_count = missiondb:param(i+base_param_offset);
						if monster_class == exconfig:monster(i) and old_count < exconfig:count(i) then
							local count = old_count +new_count
							missiondb:set_param(i+base_param_offset,count);
							player:GetMissionProxy():OnUpdateMission(config:id())
						end
					end
				elseif exconfig:sub_type()== SubFinishType_KillAndGetOneWithMonster then		--击杀多种怪物获得一种道具
					--print("xxxxxxlua-----SubFinishType_KillAndGetOneWithMonster")
					
					for i=0,exconfig:monster_size()-1,1 do
						local old_count = missiondb:param(base_param_offset);					--参数唯一
						----print("old_count"..old_count)
						----print("exconfig:count(i)"..exconfig:count(0))
						if monster_class == exconfig:monster(i) and old_count < exconfig:count(0) then
							if exconfig:rate(i)<10000 then			
								local seed = LuaServer():RandomNum(1,10000)
								if seed<=exconfig:rate(i) then	
									local count = old_count+new_count;
									missiondb:set_param(base_param_offset,count);
									player:GetMissionProxy():OnUpdateMission(config:id())
								end
							else
								local count = old_count+new_count;
								missiondb:set_param(base_param_offset,count);
								player:GetMissionProxy():OnUpdateMission(config:id())
							end
						end
					end
				elseif exconfig:sub_type()== SubFinishType_KillAnyMonster then					--击杀任意怪物完成
					----print("xxxxxxlua-----SubFinishType_KillAnyMonster")
					for i=0,exconfig:count_size()-1,1 do
						local old_count = missiondb:param(i+base_param_offset);
						if old_count < exconfig:count(i) then
							local count = old_count +new_count
							missiondb:set_param(i+base_param_offset,count);
							player:GetMissionProxy():OnUpdateMission(config:id())
						end
					end
				elseif exconfig:sub_type() == SubFinishType_KillStageAnyMonster then			--击杀关卡任务怪物完成
					if stage_id == exconfig:pos_param() then
						----print("xxxxxxlua-----SubFinishType_KillAnyMonster")
						for i=0,exconfig:count_size()-1,1 do
							local old_count = missiondb:param(i+base_param_offset);
							if old_count < exconfig:count(i) then
								local count = old_count +new_count
								missiondb:set_param(i+base_param_offset,count);
								player:GetMissionProxy():OnUpdateMission(config:id())
							end
						end
					end
				else
					--do nothing
				end			
			
				--完成判定	
				local is_ok = true
				for i=0,exconfig:count_size()-1,1 do
					local count =missiondb:param(i+base_param_offset);
					--print("xxxxxxlua-----count"..count.."/"..exconfig:count(i).."index"..exconfig:index())
					if count < exconfig:count(i) then
						is_ok = false
					end
				end				
			
				if is_ok and missiondb:state() ~= MissionState_MissionComplete then
					
					missiondb:set_state(MissionState_MissionComplete)
					if config:commit_npc_id() <0 and config:type() ~= MissionType_ScrollMission then
						--没有提交npc立即完成
						player:GetMissionProxy():OnFinishMission(config:id())					
					else
						--需要通过对话提交
						player:GetMissionProxy():OnUpdateMission(config:id())
					end				
				end
			
			end
		end
    end
	
	return ResultCode_ResultOK;
end


function x000502_xOnLevelUpTo(player,params)
	local id = params["value"]
	local mission_map=player:GetMissionProxy():GetMissionMap()
	for k, mission in pairs(mission_map) do
		local config = mission:Config()
		local mission_data = mission:MissionDB()
		if config:finish_type()==FinishType_LevelUp then
			if id >= config:param(0) then
				mission_data:set_param(MissionDB_Param_Finish_Value,1)
				mission_data:set_state(MissionState_MissionComplete)
				if config:commit_npc_id() <0 then
					--没有提交npc立即完成
					player:GetMissionProxy():OnFinishMission(config:id())					
				else
					--需要通过对话提交
					player:GetMissionProxy():OnUpdateMission(config:id())
				end	
			end
		end
	end
	
	--等级任务
	player:GetMissionProxy():OnAcceptMission(MissionType_LevelMission)
	
	return ResultCode_ResultOK

end

--function x000502_xOnTargetOk(player,params)
--	local id = params["value"]
--	local mission_map=player:GetMissionProxy():GetMissionMap()
--	for k, mission in pairs(mission_map) do
--		local config = mission:Config()
--		local mission_data = mission:MissionDB()
--		if config:finish_type()==FinishType_GenTarget then
--			if id == config:param(0) then
--				mission_data:set_param(MissionDB_Param_Finish_Value,1)
--				mission_data:set_state(MissionState_MissionComplete)
--				if config:commit_npc_id() <0 then
--					--没有提交npc立即完成
--					player:GetMissionProxy():OnFinishMission(config:id())					
--				else
--					--需要通过对话提交
--					player:GetMissionProxy():OnUpdateMission(config:id())
--				end	
--			end
--		end
--	end
--	
--	return ResultCode_ResultOK
--
--end

function x000502_xRobotTeamOk(player,params)
	local proxy =player:GetMissionProxy()
	local mission_map=proxy:GetMissionMap()
	for k, mission in pairs(mission_map) do
		local config = mission:Config()
		local mission_data = mission:MissionDB()	
		if config:param_size()>0 then
			local action_type = config:param(0)
			--print("action_type"..action_type.."finish_type"..config:finish_type())
			if config:finish_type()==FinishType_Action and action_type == g_ActionType.RobotTeam then -- 10人组队任务
				if  player:CurrentRobotCount() >= 9 then
					mission_data:set_state(MissionState_MissionComplete)					
					proxy:OnUpdateMission(config:id())
				end
			end
		end
	end
	
	return ResultCode_ResultOK
end

--function x000502_xRobotTeamDismiss(player,params)
--	return ResultCode_ResultOK
--end
function x000502_xOnUpgradeColor(player,params)
	return x000502_OnActionResolve(player,params);
end
function x000502_xAddEquipment(player,params)
	return x000502_OnActionResolve(player,params);
end
function x000502_xAddHero(player,params)
	return x000502_OnActionResolve(player,params);
end
function x000502_xOnSetActorFight(player,params)
	return x000502_OnActionResolve(player,params);	
end
function x000502_xOnUpgradeSKill(player,params)
	return x000502_OnActionResolve(player,params);	
end
function x000502_xOnEnhenceEquip(player,params)
	return x000502_OnActionResolve(player,params);	
end
function x000502_xOnInsetEquip(player,params)
	return x000502_OnActionResolve(player,params);	
end
function x000502_xOnAddPracticeLevel(player,params)
	return x000502_OnActionResolve(player,params);	
end
function x000502_xOnEnchantEquip(player,params)
	return x000502_OnActionResolve(player,params);	
end

local ResolveFunction={}
--招募两个英雄
ResolveFunction[g_ActionType.GetHero]	=function(player,mission_data,config)	
	local old = mission_data:param(MissionDB_Param_Finish_Value)
	if old <=0 then
		old = 0
	end
	old=old+1
	mission_data:set_param(MissionDB_Param_Finish_Value,old)
	local actors = player:Actors()
	if actors_size() >=3 then
		mission_data:set_state(MissionState_MissionComplete)	
	end
end

 --上阵招募的英雄
ResolveFunction[g_ActionType.FightHero]	=function(player,mission_data,config)	
	local form = player:GetActorFormation(ActorFigthFormation_AFF_NORMAL_HOOK)
	local count = 0
	for i=0,form:actor_guids_size()-1,1 do
		if form:actor_guids(i) ~= 0 then
			count = count +1
		end
	end
	
	if count >= 3 then
		mission_data:set_state(MissionState_MissionComplete)
	end
end
ResolveFunction[g_ActionType.OnEquip]	=function(player,mission_data,config)	
	local container = player:FindContainer(ContainerType_Layer_Equip);
	local items = container:Items()
	if #items >= 2 then
		mission_data:set_state(MissionState_MissionComplete)
	end
end

--主角进阶至绿色
ResolveFunction[g_ActionType.ColorUp]	=function(player,mission_data,config)	
	local main_actor = player:MainActor()
	if main_actor:ActorDBInfo():color()>=2 then
		mission_data:set_state(MissionState_MissionComplete)
	end
end
-- 主角1个技能升至5级
ResolveFunction[g_ActionType.SkillUp]	=function(player,mission_data,config)
	local main_actor = player:MainActor()
	local skills = main_actor:DbSkills()
	for k,v in pairs( skills ) do 
		local dbinfo = v:DbInfo()
		if dbinfo:skill_level() >= 5 then
			mission_data:set_state(MissionState_MissionComplete)
			break
		end
	end	
end

-- 任意两个角色各穿8件装备
ResolveFunction[g_ActionType.Hero8Equips]	=function(player,mission_data,config)	
	local actors = player:Actors()
	local count = 0
	for k,actor in pairs(actors) do
		local equips = actor:CurrEquipments(false)
		new_count = #equips
		if new_count>=8 then						
			count = count +1						
		end	
	end
	if count >=2 then
		mission_data:set_state(MissionState_MissionComplete)
	end
end

-- 两个好友快捷装备完成
ResolveFunction[g_ActionType.ParterEquip]	=function(player,mission_data,config)	
	local actors = player:Actors()
	local count = 0
	for k,actor in pairs(actors) do
		if actor:ActorType() ~= ActorType_Main then
			local equips = actor:CurrEquipments(false)
			new_count = #equips
			if new_count>=2 then						
				count = count +1						
			end	
		end
	end
	if count >=2 then
		mission_data:set_state(MissionState_MissionComplete)
	end
end

-- 10人组队任务
ResolveFunction[g_ActionType.RobotTeam]	=function(player,mission_data,config)	
	if  player:CurrentRobotCount() >= 9 then
		mission_data:set_state(MissionState_MissionComplete)
	end	
end

--通用
--n个技能至少升到n级
ResolveFunction[g_ActionType.SkillLevel]	=function(player,mission_data,config)		
	local actors = player:Actors()
	local count = 0
	for m,actor in pairs( actors ) do
		local skills = actor:DbSkills()
		for k,v in pairs( skills ) do 
			local dbinfo = v:DbInfo()
			if dbinfo:skill_level() >= config:param(2) then
				count = count +1
			end
		end
	end
	if count >= config:param(1) then
		mission_data:set_state(MissionState_MissionComplete)
		count =  config:param(1) 
	end
	mission_data:set_param(MissionDB_Param_Finish_Value,count)
end

--n个英雄穿戴n件n品质以上装备
ResolveFunction[g_ActionType.HeroEquip]	=function(player,mission_data,config)		
	local actors = player:Actors()
	local count = 0
	local equip_count = 0
	for m,actor in pairs( actors ) do
		
		local equips = actor:CurrEquipments(false)
		for k,v in pairs(equips) do 
			if v:Config():color()>=config:param(3) then
				equip_count = equip_count+1
			end
		end
		if equip_count>=config:param(2) then						
			count = count +1	
		end
	end
	if count >= config:param(1) then
		mission_data:set_state(MissionState_MissionComplete)
		count =  config:param(1) 
	end
	mission_data:set_param(MissionDB_Param_Finish_Value,count)
end

--n个英雄至少进阶到n品质
ResolveFunction[g_ActionType.HeroColor]	=function(player,mission_data,config)		
	local actors = player:Actors()
	local count = 0
	for k,actor in pairs(actors) do
		local dbinfo = actor:ActorDBInfo()
		if dbinfo:color() >= config:param(2) then
			count = count+1
		end
	end	
	if count>=config:param(1) then
		mission_data:set_state(MissionState_MissionComplete)
		count =  config:param(1) 
	end
	mission_data:set_param(MissionDB_Param_Finish_Value,count)				
end

--n个工会技能至少升到n级
ResolveFunction[g_ActionType.GuildSkillLevel]	=function(player,mission_data,config)	
	local result = player:GetAllPracticeInfo()
	local count = 0
	for k,v in pairs(result) do 
		if v:level()>=config:param(2) then				
			count = count +1	
		end
	end	
	if count >= config:param(1) then
		mission_data:set_state(MissionState_MissionComplete)
		count =  config:param(1) 
	end
	mission_data:set_param(MissionDB_Param_Finish_Value,count)
end

--n个英雄激活n级大师等级
ResolveFunction[g_ActionType.HeroMaster]	=function(player,mission_data,config)		
	local actors = player:Actors()
	local count = 0
	for k,actor in pairs(actors) do
		local dbinfo = actor:ActorDBInfo()
		if dbinfo:gem_master_lv() >= config:param(2) then
			count = count+1
		end
	end	
	if count>=config:param(1) then
		mission_data:set_state(MissionState_MissionComplete)
		count =  config:param(1) 
	end
	mission_data:set_param(MissionDB_Param_Finish_Value,count)
end

--n件装备至少拥有n条附魔属性
ResolveFunction[g_ActionType.EquipEnchant]	=function(player,mission_data,config)	

	local result = player:EnchantSlotEquips(true)
	local count = 0

	for k,v in pairs(result) do 
		--print("k"..k.." v"..v)
		if k>=config:param(2) then				
			count = count +v	
		end
	end
	
	--print( "EquipEnchant config:param(2)".. config:param(2) .." config:param(1)"..config:param(1).." count"..count)
	
	if count >= config:param(1) then
		mission_data:set_state(MissionState_MissionComplete)
		count =  config:param(1) 
	end
	mission_data:set_param(MissionDB_Param_Finish_Value,count)
end

-----------------------------------------------------------------------

function x000502_OnActionResolve(player,params)
	
	local mission_map=player:GetMissionProxy():GetMissionMap()
	for k, mission in pairs(mission_map) do
		local config = mission:Config()
		local mission_data = mission:MissionDB()	
		if config:param_size()>0 and config:finish_type()==FinishType_Action then
			local action_type = config:param(0)
			for m,n in pairs(g_ActionType) do
				if n==action_type then
					--print("ddddddddddddddddddd action_type"..action_type)
					ResolveFunction[action_type](player,mission_data,config)	
					break
				end
			end			
			player:GetMissionProxy():OnUpdateMission(config:id())
			if mission_data:state()==MissionState_MissionComplete then
				if config:commit_npc_id() <0 then
					--没有提交npc立即完成
					player:GetMissionProxy():OnFinishMission(config:id())	
				end	
			end			
		end
	end
	return ResultCode_ResultOK;
end
