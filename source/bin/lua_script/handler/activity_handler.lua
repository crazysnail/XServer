---------------------------------------------------
function x010001_OnRegisterFunction(proxy)
	
	if proxy ~= nil then
		proxy:RegFunc("xOnLuaRequestPacket"		,10001)	
	end
end
---------------------------------------------msg handle------------------------------------
local msg_lua_request={}
msg_lua_request["BeginAnswer"]	=function(player, packet )

	local type = packet:int32_params(BeginAnswer.data_32.type)
	local ret = player:ActivityCheck(type);
	if ret ~= ResultCode_ResultOK then
		return ret;	
	end
	
	if type == ActivityType.PicAnswer then
		return player:OnBeginAnswer("pic",10)
	else 
		return player:OnBeginAnswer("text",20)
	end
end

msg_lua_request["ContinueAnswer"]	=function(player, packet )

	local type = packet:int32_params(ContinueAnswer.data_32.type)
	local ret = player:ActivityCheck(type);
	if ret ~= ResultCode_ResultOK then
		return ret;	
	end
	
	if type == ActivityType.PicAnswer then
		return player:OnNextAnswer("pic")
	else 
		return player:OnNextAnswer("text")
	end
end


msg_lua_request["ConfirmTextAnswer"]	=function(player, packet )

	local ret= player:ActivityCheck(ActivityType.TextAnswer)
	if ret ~=  ResultCode_ResultOK then
		return ret
	end
	
	local index = packet:int32_params(ConfirmTextAnswer.data_32.index)
	if player:ValidAnswerIndex("text",index) == false then
		return ResultCode_Invalid_Request
	end
	
	local option = packet:string_params(ConfirmTextAnswer.data_string.option)
	local config = LuaActivityManager():FindAnswerConfig(index)
	if config == nil then
		return ResultCode_InvalidConfigData
	end
	local level_config = LuaMtActorConfig():GetLevelData(player:PlayerLevel())
	if level_config == nil then
		return ResultCode_InvalidConfigData
	end
	local exp = level_config:text_answer_exp();
	local gold = level_config:text_answer_gold();

	local dc_container = player:DataCellContainer()
	if dc_container == nil	 then
		return ResultCode_Invalid_Request
	end		
	
	if option == config:option(0) then
		player:AddItemByIdWithNotify(TokenType_Token_Exp,exp,ItemAddLogType_AddType_Activity,ActivityType.TextAnswer,1)
		player:AddItemByIdWithNotify(TokenType_Token_Gold,gold,ItemAddLogType_AddType_Activity,ActivityType.TextAnswer,1)				
		dc_container:add_data_32(CellLogicType_ActivityCell,ActivityDataIndex.TextAnswerOkTimes,1,true)			
	else
		player:AddItemByIdWithNotify(TokenType_Token_Exp,exp/2,ItemAddLogType_AddType_Activity,ActivityType.TextAnswer,1)
		player:AddItemByIdWithNotify(TokenType_Token_Gold,gold/2,ItemAddLogType_AddType_Activity,ActivityType.TextAnswer,1)				
		--通知结果
		player:SendLuaPacket("TextAnswerWrong",{index},{},{config:option(0)})	
	end		
	
	player:CheckAcrDrop(ActivityType.TextAnswer)
	player:OnLuaFunCall_1(501, "AddActivityTimes", ActivityType.TextAnswer)		
	
	--活动奖励	
	local times = dc_container:get_data_32(CellLogicType_ActivityCell,ActivityDataIndex.TextAnswerOkTimes)
	local flag_data  = dc_container:get_data_32(CellLogicType_ActivityCell, ActivityDataIndex.text_reward_flag)	
	
	if times == 10 and dc_container:check_bit_data_32(flag_data,0) == false then
		local new_flag = dc_container:set_bit_data_32(flag_data,0)
		dc_container:set_data_32(CellLogicType_ActivityCell, ActivityDataIndex.text_reward_flag,new_flag,true)
		player:AddItemByIdWithNotify(20139001,1,ItemAddLogType_AddType_Activity,ActivityType.TextAnswer,1)
	elseif times == 20 and dc_container:check_bit_data_32(flag_data,1) == false then
		local new_flag = dc_container:set_bit_data_32(flag_data,1)
		dc_container:set_data_32(CellLogicType_ActivityCell, ActivityDataIndex.text_reward_flag,new_flag,true)
		player:AddItemByIdWithNotify(20139002,1,ItemAddLogType_AddType_Activity,ActivityType.TextAnswer,1)
	end		
	
	player:OnConfirmAnswer("text",index)
	if player:IsLastAnswer("text") then
		player:SendStatisticsResult(ActivityType.TextAnswer, true)
		--player:SendClientNotify("ac_notify_022", -1, 1);
	else
		player:OnNextAnswer("text")
	end
	
	return ResultCode_ResultOK
end

msg_lua_request["ConfirmPicAnswer"]	=function(player, packet )

	local ret= player:ActivityCheck(ActivityType.PicAnswer)
	if ret ~=  ResultCode_ResultOK then
		return ret
	end
	
	local index = packet:int32_params(ConfirmPicAnswer.data_32.index)
	local option = packet:int32_params(ConfirmPicAnswer.data_32.option)
	local config = LuaActivityManager():FindPicAnswerConfigByIndex(index)
	if config == nil then
		return ResultCode_InvalidConfigData
	end
	
	----print("shit1!!!!!!!!!")
	if player:ValidAnswerIndex("pic",config:group()) == false then
		return ResultCode_Invalid_Request
	end
	
	local group = LuaActivityManager():FindPicAnswerConfig(config:group())
	if group == nil then
		return ResultCode_InvalidConfigData
	end
	
	local level_config = LuaMtActorConfig():GetLevelData(player:PlayerLevel())
	if level_config == nil then
		return ResultCode_InvalidConfigData
	end
	local exp = level_config:pic_answer_exp()
	local gold = level_config:pic_answer_gold()
	
	local dc_container = player:DataCellContainer()
	if dc_container == nil	 then
		return ResultCode_Invalid_Request
	end
	
	local finish = true
	if option ~= config:option(0) then		--错误		
		player:RecordPicResult(index, 1)
		player:AddItemByIdWithNotify(TokenType_Token_Exp,exp/2,ItemAddLogType_AddType_Activity,ActivityType.PicAnswer,1)
		player:AddItemByIdWithNotify(TokenType_Token_Gold,gold/2,ItemAddLogType_AddType_Activity,ActivityType.PicAnswer,1)		
		--通知结果
		player:SendLuaPacket("PicAnswerWrong",{index,config:option(0)},{},{})	
	else									--正确
		player:RecordPicResult(index, 0)	
		if player:CheckPicResult() == false then --说明是最后一题	--全部答对	
			player:AddItemByIdWithNotify(TokenType_Token_Exp,exp,ItemAddLogType_AddType_Activity,ActivityType.PicAnswer,1)
			player:AddItemByIdWithNotify(TokenType_Token_Gold,gold,ItemAddLogType_AddType_Activity,ActivityType.PicAnswer,1)		
			dc_container:add_data_32(CellLogicType_ActivityCell,ActivityDataIndex.PicAnswerOkTimes,1,true)
		else
			finish = false
		end	
	end
	
	if finish then
		player:CheckAcrDrop(ActivityType.PicAnswer)
		player:OnLuaFunCall_1(501, "AddActivityTimes", ActivityType.PicAnswer)
		--奖励包判定
		local times = dc_container:get_data_32(CellLogicType_ActivityCell,ActivityDataIndex.PicAnswerOkTimes)
		if times >= 6 then			
			local flag_data  = dc_container:get_data_32(CellLogicType_ActivityCell, ActivityDataIndex.pic_reward_flag)
			if flag_data ==0 then
				flag_data=1
				dc_container:set_data_32(CellLogicType_ActivityCell, ActivityDataIndex.pic_reward_flag,flag_data,true)
				player:AddItemByIdWithNotify(20139001,1,ItemAddLogType_AddType_Activity,ActivityType.PicAnswer,1);
			end
		end
	end
	
	player:OnConfirmAnswer("pic",config:group())
	if player:IsLastAnswer("pic") then
		player:SendStatisticsResult(ActivityType.PicAnswer, true)
		--player:SendClientNotify("ac_notify_022", -1, 1);
	else
		player:OnNextAnswer("pic")
	end
	
	return ResultCode_ResultOK
end

msg_lua_request["msg_acr_request"]	=function(player, packet )

	if packet:int32_params_size() ~= table.nums(msg_acr_request.data_32) then
		return player:SendClientNotify("Invalid_Params",-1,-1)
	end
	
	local script_id = packet:int32_params(msg_acr_request.data_32.script_id)
	local reward_index = packet:int32_params(msg_acr_request.data_32.reward_index)
	local sub_index = packet:int32_params(msg_acr_request.data_32.sub_index) 

	local tag ="nil"
	if packet:string_params_size() >0 then
		tag = packet:string_params(msg_acr_request.data_string.tag)
	end
	
	print("dddddddddddddd script_id="..script_id.." reward_index="..reward_index.." sub_index="..sub_index)
	
	local ok = false
	for k,v in pairs(g_reward_activity_script) do
		if v == script_id then
			 ok= true
			 break
		end
    end
	
	--print("dddddddddddddd"..reward_index)
	if ok==false then
		player:SendClientNotify("lua script do not exist!",-1,-1)
		return ResultCode_Invalid_Request
	end
		
	
	return player:OnLuaFunCall_n(script_id,"DoReward",{["index"]=reward_index,["value"]=sub_index,[tag]=999})
end
		
msg_lua_request["PicAnswerReward"]	=function(player, packet )
	
	local dc_container = player:DataCellContainer()
	if dc_container == nil then	
		return ResultCode_Invalid_Request
	end
	local times = dc_container:get_data_32(CellLogicType_ActivityCell,ActivityDataIndex.PicAnswerOkTimes)
	if times < 6 then
		return ResultCode_RewardConditionLimit
	end
	return ResultCode_RewardAlready
	
end

msg_lua_request["TextAnswerReward"]	=function(player, packet )
		
	local dc_container = player:DataCellContainer()
	if dc_container == nil then	
		return ResultCode_Invalid_Request
	end
	local reward_index = packet:int32_params(TextAnswerReward.data_32.index)
	local times = dc_container:get_data_32(CellLogicType_ActivityCell,ActivityDataIndex.PicAnswerOkTimes)
	if reward_index ==0 then
		if times < 10 then
			return ResultCode_RewardConditionLimit
		end
	elseif reward_index == 1 then
		if times < 20 then
			return ResultCode_RewardConditionLimit
		end
	end
	return ResultCode_RewardAlready
end

msg_lua_request["GetTargetReward"]	=function(player, packet )
	local index = packet:int32_params(GetTargetReward.data_32.index)
	
	--print("index"..index)
	
	local config = LuaMtMissionManager():FindTarget(index)
	if config == nil then
		return ResultCode_InvalidConfigData
	end
	
	
	--背包检查
	local dic = {}
	local item_id = 0
	local item_count = 0
	for i=0,config:reward_item_size()-1,1 do 
		item_id=config:reward_item(i)
		item_count = config:count(i)
		if item_id>0 and item_count > 0 then
			dic[item_id]=item_count
		end
	end
	if player:EquipAndItemBagLeftCheck(dic) == false then
		return ResultCode_InternalResult
	end

	local container = player:DataCellContainer()
	local data = container:get_data_32(CellLogicType_TargetData,index)
	--判定领奖标记
	local flag = container:get_high_16_bit(data)
	local count = container:get_low_16_bit(data)
	--已经领奖
	if container:check_bit_data_32(flag,0) then	
		return ResultCode_RewardAlready
	end
	--条件不足
	if container:check_bit_data_32(flag,1) == false then	
		return ResultCode_RewardConditionLimit
	end
	
	local new_flag = container:set_bit_data_32(flag,0)
	local new_data = container:bind_bit_data_32(new_flag,count)
	container:set_data_32(CellLogicType_TargetData,index,new_data,true)
	
	for i=0,config:reward_item_size()-1,1 do
		if config:reward_item(i) >0 and config:count(i) > 0 then
			player:AddItemByIdWithNotify(config:reward_item(i),config:count(i),ItemAddLogType_AddType_TargetReward,0,1)
		end
	end	
	return ResultCode_ResultOK
end


msg_lua_request["GetTargetSegReward"]	=function(player, packet )
	local index = packet:int32_params(GetTargetSegReward.data_32.index)
	
	--print("index"..index)
	
	if index<0 or index >=16 then
		return ResultCode_Invalid_Request
	end
				
	local rewards = g_TargetSegReward[index+1]
	--背包检查
	if x000010_CheckBagEx(player,rewards,{}) == false then
		return ResultCode_InternalResult
	end

	local container = player:DataCellContainer()	
	--判定领奖标记
	local flag = container:get_data_32(CellLogicType_NumberCell,NumberCellIndex_TargetRewardFlag)
	--已经领奖
	if container:check_bit_data_32(flag,index+16) == true then	
		return ResultCode_RewardAlready
	end
	--条件不足
	if container:check_bit_data_32(flag,index) == false then	
		return ResultCode_RewardConditionLimit
	end	
	local new_flag = container:set_bit_data_32(flag,index+16)
	container:set_data_32(CellLogicType_NumberCell,NumberCellIndex_TargetRewardFlag,new_flag,true)
	
	--奖励	
	for i=1,#rewards,1 do
		local id=rewards[i][1]
		local count=rewards[i][2]
		player:AddItemByIdWithNotify(id,count,ItemAddLogType_AddType_TargetReward,0,1)
	end
	return ResultCode_ResultOK
end

------------------------------------------------------------------------
function x010001_xOnLuaRequestPacket( player, packet )	
	if packet == nil then
		return ResultCode_Invalid_Request
	end

	if x000010_LuaProtoCheck(msg_activity,msg_lua_request,packet,player) then	
		return msg_lua_request[packet:request_name()]( player, packet )
	end
	
	return ResultCode_ResultOK
end
---------------------------------------------------------------------