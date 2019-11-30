
--战场
local left_time =3600		--持续时间
local empty_check_time =180 --空时间
local bufftable={"1140200101","1140200102","1140200103","1140200104"}

function x002004_OnCreate(raid)	
end

function x002004_OnRaidTimer(raid,index)	
end

function x002004_OnBigTick(raid,elapseTime)
	for i=1,#bufftable,1 do
		local cdtime = raid:GetParams32(bufftable[i]);
		if cdtime > 0 then
			cdtime = cdtime - 10;
			if cdtime <= 0 then
				cdtime = 0;
				raid:BroadCastLuaPacket("NewBuffPoint",{tonumber(bufftable[i])},{},{});
			end
			raid:SetParams32(bufftable[i],cdtime);
		end
	end
	
	local begin_date = raid:GetBeginDate()
	local delta_time =  LuaMtTimerManager():DiffTimeToNow(begin_date)
	if delta_time>=left_time then
		raid:SetParams32("bfbegin",-1)
		raid:OnDestroy()
	end
		
end

function x002004_OnSecondTick(raid)	
end

function x002004_OnMinTick(raid)	
end

function x002004_OnEnter(player,raid)	
	local scene = raid:Scene()
	if scene == nil then
		return
	end	
	
	if raid:GetParams32("bfbegin")>=1 then	
		for i=1,#bufftable,1 do
			local cdtime = raid:GetParams32(bufftable[i]);
			if cdtime <= 0 then
				player:SendLuaPacket("NewBuffPoint",{tonumber(bufftable[i])},{},{});
			end
		end
	end
	
end

function x002004_OnClientEnterOk(player, raid)
end

function x002004_ReConnected(raid,player)
	return ResultCode_ResultOK
end

function x002004_OnLeave(player,raid)
end

function x002004_OnDestroy(raid)
end

function x002004_OnGenBuff(raid)
	for i=1,#bufftable do
		raid:SetParams32(bufftable[i],1)
	end
	raid:SetParams32("bfbegin",1)
end

function x002004_OnBuffPoint(player,raid,posindex)

	for i=1,#bufftable,1 do
		if tonumber(bufftable[i]) == posindex and ( posindex == 1140200103 or posindex == 1140200104 ) then--加速
			local cdtime = raid:GetParams32(bufftable[i])
			if cdtime <= 0 then
				raid:BroadCastLuaPacket("HideBuffPoint",{tonumber(bufftable[i])},{},{});
				raid:SetParams32(bufftable[i],60);
				local proxy = player:GetSimpleImpactSet()
				if proxy ~= nil then
					proxy:AddSimpleImpact(1001, 10, 2000, 0, false)
				end
			end
		elseif tonumber(bufftable[i]) == posindex and ( posindex == 1140200101 or posindex == 1140200102 ) then --回血
			local cdtime = raid:GetParams32(bufftable[i])
			if cdtime <= 0 then
				player:ResetPlayerActorLastHp(LastHpType_BFLastHp)		
				raid:SetParams32(bufftable[i],60);					
			end
		end
	end
end
