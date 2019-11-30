function x003001_OnNpcOption(player,npc_config,option_index)
	
	local talk_option = npc_config:talk_option(option_index)
	print(talk_option);
	if talk_option == "npc_op_cont_119" then		
		player:OnNpcOptionReply(npc_config:index(),"showpanel","StorePanel",0)--宝石商店
	elseif talk_option == "npc_op1_cont_118" then		
		--是否加入公会
		if player:GetGuildID() == 0 then
			return player:SendClientNotify("Guild_NoExist",-1,-1)
		end
		player:OnNpcOptionReply(npc_config:index(),"showpanel","StorePanel",1)--工会商店
	elseif talk_option == "npc_op1_cont_128" or talk_option == "npc_op3_cont_110" then		
		--player:SendClientNotify("MarketClose",-1,-1)
		player:OnNpcOptionReply(npc_config:index(),"showpanel","StorePanel",2)--装备商店
	elseif talk_option == "npc_op1_cont_122" then		
		player:OnNpcOptionReply(npc_config:index(),"showpanel","StorePanel",3)--正义商店
	elseif talk_option == "npc_op1_cont_101" then	
		player:OnNpcOptionReply(npc_config:index(),"showpanel","StorePanel",4)--荣誉商店
	elseif talk_option == "npc_op1_cont_102" then	
		player:OnNpcOptionReply(npc_config:index(),"showpanel","StorePanel",4)--荣誉商店
	elseif talk_option == "npc_op2_cont_122" then
		player:OnNpcOptionReply(npc_config:index(),"showpanel","StorePanel",5)--任务商店
	elseif talk_option == "npc_op_cont_116" then
		player:OnNpcOptionReply(npc_config:index(),"showpanel","ChargeStorePanel",0)--商城
	elseif talk_option == "npc_op3_cont_122" then
		player:OnNpcOptionReply(npc_config:index(),"showpanel","StorePanel",8)--杂货商店
	elseif talk_option == "npc_op_cont_104" then
		player:OnNpcOptionReply(npc_config:index(),"showpanel","StorePanel",10)--积分商店
	elseif talk_option == "npc_op_cont_138" then
		player:OnNpcOptionReply(npc_config:index(),"showpanel","StorePanel",11)--卷轴商店
	elseif talk_option == "npc_op_cont_109" then
		player:OnNpcOptionReply(npc_config:index(),"showpanel","DrawCardPanel",-1)--招募
	elseif talk_option == "npc_op_cont_113" then
		--进入地精矿洞
		player:GetMissionProxy():OnAcceptMission(MissionType_GoblinCarbonMission)
	elseif talk_option == "npc_op_cont_114" then
		--工会战
		local ret= player:ActivityCheck(ActivityType.UnionBattle)
		if ret ~=  ResultCode_ResultOK then
			return ret
		end
		player:OnLuaFunCall_1(1023, "GoGuildWorldBattle", npc_config:index())	
	elseif talk_option == "npc_op_cont_136" then		
		--传入竞技场
		player:GoRaid(1301,2003,0,-1,{-1,-1,-1})
	elseif talk_option == "npc_op_cont_137" then		
		--公会竞赛
		player:OnNpcOptionReply(npc_config:index(),"showpanel","UnionCompetePanel",-1)--招募
	elseif talk_option == "npc_op_cont_101" then
		--显示竞技场UI
		local ret= player:ActivityCheck(ActivityType.Arena)
		if ret ~=  ResultCode_ResultOK then
			return ret
		end		
		player:OnNpcOptionReply(npc_config:index(),"showpanel","ArenaPanel",-1)		
	elseif talk_option == "npc_op_cont_105" then	
		--暮光军团任务
		player:GetMissionProxy():OnAcceptMission(MissionType_CorpsMission)		
	elseif talk_option == "npc_op_cont_110" then
		--小三环
		--return player:SendClientNotify("notopen",-1,1)
		player:GetMissionProxy():OnAcceptMission(MissionType_TinyThreeCarbonMission)
	elseif talk_option == "npc_op2_cont_110" then
		--大三环
		--return player:SendClientNotify("notopen",-1,1)
		player:GetMissionProxy():OnAcceptMission(MissionType_BigThreeCarbonMission)
	elseif talk_option == "npc_op_cont_117" then
		--赏金任务
		player:GetMissionProxy():OnAcceptMission(MissionType_RewardMission)
	elseif talk_option == "npc_op_cont_121" then	
		--考古任务
		player:GetMissionProxy():OnAcceptMission(MissionType_ArchaeologyMission)
	elseif talk_option == "npc_op_cont_102" then	
		--进入试炼场
		local ret= player:ActivityCheck(ActivityType.Hell)
		if ret ~=  ResultCode_ResultOK then
			return ret
		end
		player:GoRaid(1401,2002,0,-1,{-1,-1,-1})
	elseif talk_option == "npc_op_cont_12" then	
		--世界boss
		local activity_type = player:OnLuaFunCall_n(501, "GetWorldBossAc",{} )		
		local ret= player:ActivityCheck(activity_type)
		if ret ~=  ResultCode_ResultOK then
			return ret
		end
		player:GoTo(LuaMtWorldBossManager():GetCurWorldBossConfig():sceneid(),{ -1,-1,-1 });
	elseif talk_option == "npc_op_cont_118" then	
		--工会任务
		player:GetMissionProxy():OnAcceptMission(MissionType_UnionMission)
	elseif talk_option == "npc_op1_cont_105" then
		--暮光军团 便捷组队
		player:OnNpcOptionReply(npc_config:index(),"showpanel","TeamFastMatchPanel",1)
	elseif talk_option == "npc_op1_cont_110" then
		--小三环/大三环 便捷组队
		if player:PlayerLevel() >= 50 then
			player:OnNpcOptionReply(npc_config:index(),"showpanel","TeamFastMatchPanel",5)
		else
			player:OnNpcOptionReply(npc_config:index(),"showpanel","TeamFastMatchPanel",4)
		end
	elseif talk_option == "npc_op1_cont_103" then
		--燃烧军团 便捷组队
		player:OnNpcOptionReply(npc_config:index(),"showpanel","TeamFastMatchPanel",2)
	elseif talk_option == "npc_op1_cont_126" then
		--黑色沼泽 便捷组队
		player:OnNpcOptionReply(npc_config:index(),"showpanel","TeamFastMatchPanel",7)
	elseif talk_option == "npc_op_cont_107" then	
		--战歌
		--报名阶段才activityCheck,报名已经计算活动次数了！
		local bf_proxy = player:GetBattleFeildProxy()
		if bf_proxy:raid_rtid() == 0 then
			local ret= player:ActivityCheck(ActivityType.BattleCarbon2)
			if ret ~=  ResultCode_ResultOK then
				return ret
			end
		end
		player:OnJoinBattleRoom(1402,2004)
	elseif talk_option == "npc_op_cont_126" then	
		--黑色沼泽
		player:GetMissionProxy():OnAcceptMission(MissionType_BlackCarbonMission)	
	elseif talk_option == "npc_op_cont_128" then	
		--提尔之手	
		player:GetMissionProxy():OnAcceptMission(MissionType_BloodCarbonMission)	
	elseif talk_option == "npc_op_cont_2359" then
		--木桩
		local battle = player:GetBattleManager():CreateFrontBattle(player,nil,
		{
			["script_id"]=1003,
			["pos_id"]=npc_config:pos_id(),
			["mission_id"]=-1,
			["relate_npc"]= npc_config:index(),
			["monster_group_id"]=99
		})
	elseif talk_option == "npc_op_cont_125" then
		--祖尔格拉布
		player:GetMissionProxy():OnAcceptMission(MissionType_Carbon2001Mission)	
	elseif talk_option == "npc_op_cont_123" then
		--熔火之心
		player:GetMissionProxy():OnAcceptMission(MissionType_Carbon2002Mission)	
	elseif talk_option == "npc_op1_cont_123" then
		--黑翼之巢
		player:GetMissionProxy():OnAcceptMission(MissionType_Carbon2003Mission)	
	elseif talk_option == "npc_op_cont_134" then
		--奥妮克希亚的巢穴
		player:GetMissionProxy():OnAcceptMission(MissionType_Carbon2004Mission)	
	elseif talk_option == "npc_op_cont_130" then
		--安其拉废墟
		player:GetMissionProxy():OnAcceptMission(MissionType_Carbon2005Mission)	
	elseif talk_option == "npc_op1_cont_130" then
		--安其拉神殿
		player:GetMissionProxy():OnAcceptMission(MissionType_Carbon2006Mission)	
	elseif talk_option == "npc_op_cont_124" then
		--纳克萨玛斯
		player:GetMissionProxy():OnAcceptMission(MissionType_Carbon2007Mission)	
	elseif talk_option == "npc_op1_cont_112" then
		--5宝
		player:OnNpcOptionReply(npc_config:index(),"showpanel","TreasureExchangePanel",-1)
	elseif talk_option == "npc_op_cont_139" then
		--进入通天塔
		local ret = x001003_OnJoinCheck(player,	{ ["mission_type"]=MissionType_TowerMission })
		if  ret == ResultCode_ResultOK then
			player:GoRaid(1404,2009,0,-1,{-1,-1,-1})
		end
	else
		return player:SendClientNotify("InvalidNpcOption",-1,-1)
	end	
	
	return ResultCode_ResultOK
	
end
