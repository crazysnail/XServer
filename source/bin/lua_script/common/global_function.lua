
function x000010_LuaProtoCheck(ProtoSet,HandlerSet,packet,player)
	--协议检查
	if ProtoSet[packet:request_name()]==nil then
		--协议表没有统一，发散派发必然存在找不到协议的情况，这种是合法逻辑,不处理就好了
		--LuaServer():LuaLog("error, unregister lua protocol! "..packet:request_name() ,g_log_level.LL_ERROR)
		--player:SendClientNotify("Invalid_Request",-1,-1)
		return false
	end
	
	--回调检查
	if x000010_IsTableHasKey(HandlerSet,packet:request_name()) == false then
		--有协议，没有注册handler也是合法逻辑，不处理就好了
		--LuaServer():LuaLog("error, unregister lua protocol handler! "..packet:request_name() ,g_log_level.LL_ERROR)
		--player:SendClientNotify("Invalid_Request",-1,-1)
		return false
	end	
	
	--参数检查
	if packet:int32_params_size() ~= table.nums(ProtoSet[packet:request_name()].data_32) then
		LuaServer():LuaLog("error, lua protocol invalid int32_params_size! "..packet:request_name() ,g_log_level.LL_ERROR)
		player:SendClientNotify("Invalid_Params",-1,-1)
		return false
	end	
	if packet:int64_params_size() ~= table.nums(ProtoSet[packet:request_name()].data_64) then
		LuaServer():LuaLog("error, lua protocol invalid int64_params_size! "..packet:request_name() ,g_log_level.LL_ERROR)
		player:SendClientNotify("Invalid_Params",-1,-1)
		return false
	end	
	--if packet:string_params_size() ~= table.nums(ProtoSet[packet:request_name()].data_string) then
	--	LuaServer():LuaLog("error, lua protocol invalid string_params_size! "..packet:request_name() ,g_log_level.LL_ERROR)
	--	player:SendClientNotify("Invalid_Params",-1,-1)
	--	return false
	--end	

	return true
end
	
function x000010_dump_json( json_value )
	for k,v in pairs(json_value) do
		stype = type(v)
		if stype=="table" then
			print("k="..k.." table value=")
			x000010_dump_json(v)
		elseif stype =="boolean" then
			if v then
				print("k="..k.." boolean value="..1)
			else
				print("k="..k.." boolean value="..0)
			end
		elseif stype =="string" then
			print("k="..k.." string value="..v)
		elseif stype =="number" then
			print("k="..k.." number value="..v)
		else
			print("k="..k.." unkown value!" )
		end
	end
end

function table.nums( t )
	if t == nil then
		return 0
	end
    local count = 0
    for k, _ in pairs( t ) do
        count = count + 1
    end
    return count
end

--字符串分割函数
--传入字符串和分隔符，返回分割后的table
function string.split(str, delimiter)
	if str==nil or str=='' or delimiter==nil then
		return nil
	end
	
    local result = {}
    for match in (str..delimiter):gmatch("(.-)"..delimiter) do
        table.insert(result, match)
    end
    return result
end

function x000010_GetMountCountEffect( count )
	local effect={}
	for k,v in pairs(g_MountCountEffect) do
		----print(k,v,v[0],v[1],v[2])
		if count >= v[1] then 
			local has_key = false
			for k2,v2 in pairs(effect) do
				--print(k2,v2)
				if k2 == v[2] then
					has_key = true					
				end
			end			
			----print("v[2]"..v[2])
			if has_key then			
				effect[v[2]] = effect[v[2]]+v[3]
			else
				effect[v[2]] = v[3]
			end
		end
	end
	return effect
end


function x000010_GenSpecialReward( )
	local allweight = 0
	for key,iter in pairs(g_special_reward)	do
		allweight=allweight+iter[3]
	end
	
	local seed_weight = LuaServer():RandomNum(1,allweight)
	local base_weight = 0
	for key,iter in pairs(g_special_reward)	do
		base_weight = base_weight+iter[3]
		if  seed_weight<=base_weight then
			return iter[1],iter[2]
		end
	end
	
	return -1,0
end

function x000010_IsTableHasKey(tableT,key )
	for k,v in pairs(tableT)	do
		if k == key then
			return true
		end
	end	
	return false
end

function x000010_CheckBagEx(player,items1,items2)
	local dic={}
	for i=1, table.nums(items1),1 do
		if items1[i][1] >0 and items1[i][2] >0 then
			dic[items1[i][1]]= items1[i][2]
		end
	end	
	for i=1, table.nums(items2),1 do
		if items2[i][1] >0 and items2[i][2] >0 then
			dic[items2[i][1]]= items2[i][2]
		end
	end	
	if player:EquipAndItemBagLeftCheck(dic) == false then
		return false
	end
	return true
end

function x000010_CheckBag(player,start,items)
	local dic={}
	for i=start, table.nums(items),1 do
		if items[i][1] >0 and items[i][2] >0 then
			dic[items[i][1]]= items[i][2]
		end
	end	
	if player:EquipAndItemBagLeftCheck(dic) == false then
		return false
	end
	return true
end

function x000010_CheckBagOne(player,items)
	local dic={}
	if items[1] >0 and items[2] >0 then
		dic[items[1]]= items[2]
	end	
	if player:EquipAndItemBagLeftCheck(dic) == false then
		return false
	end
	return true
end

function x000010_NewSet()
    local reverse = {} --以数据为key，数据在set中的位置为value
    local set = {} --一个数组，其中的value就是要管理的数据
    return setmetatable(set,{__index = {
          insert = function(set,value)
              if not reverse[value] then
                    table.insert(set,value)
                    reverse[value] = #set
              end
          end,

          remove = function(set,value)
              local index = reverse[value]
              if index then
                    reverse[value] = nil
                    local top = table.remove(set) --删除数组中最后一个元素
                    if top ~= value then
                        --若不是要删除的值，则替换它
                        reverse[top] = index
                        set[index] = top
                    end
              end
          end,

          find = function(set,value)
              local index = reverse[value]
              return (index and true or false)
          end,
    }})
end

function x000010_GetAcrIndex( script_id )
	for k,v in pairs(g_reward_activity_script) do
		if v == script_id then
			return k
		end
    end
	return -1
end

function x000010_GRaidMissionOver(player,params)
	local raid = player:GetRaid();
	if raid ~= nil then
		--说明没任务了，可以通知离开了
		if raid:RuntimeId() == params["value"] then
			local end_opt = false
			if player:GetTeamID() ~= 0 then
				if player:IsTeamLeader() then
					end_opt = true
				end
			else
				end_opt = true
			end					
			if end_opt then
				player:SendLuaPacket("raid_mission_over",{90},{},{})
				raid:SetRaidTimer(g_raid_timer.common_destroy,90,-1)
			else
				player:SendLuaPacket("raid_mission_over",{-1},{},{})
			end
		end
	end	
	return ResultCode_ResultOK
end



--此处配置可注册脚本id
function x000010_GetRegisterableId( )
	return {
			10000,
			10001,
			7,
			100,
			501,
			502,
			504,
			2601,
			2602,
			2603,
			--2604,
			2605,
			2606,
			2607,
			2608,
			2609,
			2611,
			2612,
			2613,
			2614,
			2615,
			2616,
			2617,
			2618,
			2619,
			2620,
			2621,
			2622,
			2623,
			2624,
			2625,
			2626,
			2627,
			2628,
			2629,
			2630,
			2631,
			2632,
			2633,
			2634,
			2635,
			2636,
			2637,
			2638,
			2639,
			2640,
			2641,
			2642,
			2643,
			}
end


function x000010_TimeToString( time )
	local number_time = tonumber(time)
	local number_hour,round = math.modf(number_time/100)
	local number_minute = math.fmod(number_time,100)
	local newtime = ""
	if number_hour<10 then
		newtime=newtime.."0"
	end
	newtime = newtime..math.floor(math.modf(number_time/100))..":"
	if number_minute<10 then
		newtime=newtime.."0"
	end
	newtime = newtime..math.floor(math.fmod(number_time,100))
	return newtime
end

function x000010_BattleType2Formation( type )
		if  type == BattleGroundType_PVE_HOOK 
			or type == BattleGroundType_PVE_STAGE_BOSS 
			or type == BattleGroundType_PVE_SINGLE then
			return ActorFigthFormation_AFF_NORMAL_HOOK
		elseif type == BattleGroundType_PVP_PVE_CAPTURE_STAGE then
			return ActorFigthFormation_AFF_STAGE_CAPTURE
		elseif type == BattleGroundType_PVP_LOOT then
			return ActorFigthFormation_AFF_PVP_LOOT_FORMATION
		elseif type == BattleGroundType_PVP_ARENA then
			return ActorFigthFormation_AFF_PVP_ARENA
		elseif type == BattleGroundType_PVP_TRIAL_FIELD then
			return ActorFigthFormation_AFF_PVP_TRIAL_FIELD
		elseif type == BattleGroundType_PVE_GUILD_BOSS then
			return ActorFigthFormation_AFF_PVE_GUILD_BOSS
		elseif type == BattleGroundType_PVP_WORLDBOSS_BATTLE then
			local weekday = LuaMtTimerManager():GetWeekDay()
			return ActorFigthFormation_AFF_PVP_WORLDBOSS_Sun + weekday
		elseif type == BattleGroundType_PVP_GUILD_BATTLE then
			return ActorFigthFormation_AFF_PVE_GUILD_BOSS
		elseif type == BattleGroundType_PVE_GUILD_COPY then
			return ActorFigthFormation_AFF_PVE_GUILD_COPY
		elseif type == BattleGroundType_PVP_BATTEL_FIELD then
			return ActorFigthFormation_AFF_PVP_BATTLE_FIELD
		else
			return ActorFigthFormation_AFF_NORMAL_HOOK
		end
end

function x000010_Formation2LastHpType( type )
	if type == ActorFigthFormation_AFF_PVP_TRIAL_FIELD then
		return LastHpType_HellLastHp;
	elseif type == ActorFigthFormation_AFF_PVE_GUILD_BOSS then
		return LastHpType_GBOSSLastHp;
	else
		return LastHpType_LastHpMax;
	end
end

function x000010_GetLuaTBSize( name )
	if name == "g_statistics_type" then
		return ActivityType.TeamCarbon2007+1
	end
	return 0
end


--服务器等级换算
local ServerLvDays={
0  ,
1  ,
2  ,
3  ,
4  ,
5  ,
6  ,
7  ,
8  ,
9 ,
10 ,
11 ,
12 ,
14 ,
16 ,
18 ,
21 ,
24 ,
28 ,
33 ,
39 ,
47 ,
55 ,
65 ,
76 ,
89
}

function x000010_CalcNextServerLvDays( openday )

	if openday >= 90 then	
		return 999
	else	
		for i=1,#ServerLvDays, 1 do  		
			if openday < ServerLvDays[i] then	
				return ServerLvDays[i]-openday
			elseif openday == ServerLvDays[i] then	
				return ServerLvDays[i+1]-openday
			end
		end
	end
	return 999
end

