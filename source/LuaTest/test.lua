
test_var = 99

function dump_table(tb, str)
    if nil == str then str = "" end
    for k, v in pairs(tb)
    do
        print(str, k, v)
    end
end

Mod = {}
function Mod:funcTest1(arg1, arg2)
    print("in funcTest1:", self, arg1, arg2)
    return true
end
Mod.funcTest1()
-- 测试调用lua
function test_func(arg1, arg2, arg3, arg4)
    print("in test_func:", arg1, arg2, arg3, arg4)
    mp = {["k"] = "v"}
    vc = {1,2,3}
    lt = {4,5,6}
    st = {7,8,9}
    dumy(mp, vc, lt, st)
end

-- 接受stl参数
function test_stl(vec, lt, st, mp)
    print("--------------dump_table begin ----------------")

	--dump_table(vec)
    dump_table(vec, "vec")
    dump_table(lt, "lt")
    dump_table(st, "st")
    dump_table(mp, "mp")
    print("--------------dump_table end ----------------")
    return "ok"
end

-- 返回stl 参数
function test_return_stl_vector()
    return {1,2,3,4}
end
function test_return_stl_list()
    return {1,2,3,4}
end
function test_return_stl_set()
    return {1,2,3,4}
end
function test_return_stl_map()
    return {
        [1231565] = 124,
		[1231563] = 124
    }
end

Property_HP = 999
Property_HP2 =888
g_MountCountEffect ={
{2,Property_HP,100},
{5,Property_HP,100},
{8,Property_HP,100},
{11,Property_HP,100},
{14,Property_HP2,100},
{17,Property_HP,100},
{20,Property_HP2,100},
}

function test_return_stl_map2(count)
	local tb={}
	tb["aa"]=0
	tb["bb"]=1
	tb["vv"]=2
	tb["dd"]="ddd"
	return tb
	--local effect={}
	--for k,v in pairs(g_MountCountEffect) do
	--	--print(k,v,v[0],v[1],v[2])
	--	if count >= v[1] then 
	--		local has_key = false
	--		for k2,v2 in pairs(effect) do
	--			print(k2,v2)
	--			if k2 == v[2] then
	--				has_key = true					
	--			end
	--		end			
	--		--print("v[2]"..v[2])
	--		if has_key then			
	--			effect[v[2]] = effect[v[2]]+v[3]
	--		else
	--			effect[v[2]] = v[3]
	--		end
	--	end
	--end
	--return effect
end

-- 接受stl参数
function test_objects(vec)
	print("--------------dump_table begin ----------------")
	for k, m in pairs(vec) do
		print(k, m)
		m:dump()
		
		--t = test:new();
		--t = m:get_test();
		
		--print("test 32property", t.k)
		
		m:print(k, m)
		m:param_test(m)
		--print("foo 64property", m.b)
	end
	print("--------------dump_table end ----------------") 
end
-- 测试接受C++对象
function test_object(foo_obj)
    --测试构造
    --base = base_t:new()
    -- 每个对象都有一个get_pointer获取指针
    --print("base ptr:", base:get_pointer())
    -- 测试C++对象函数
    --foo_obj:print(12333, base)
    --base:delete()
    --基类的函数
    --foo_obj:dump()
    -- 测试C++ 对象属性
    --print("foo property", foo_obj.a)
    --print("base property", foo_obj.v)
end

-- 测试返回C++对象
function test_ret_object(foo_obj)
    return foo_obj
end

-- 测试返回C++对象
function test_ret_base_object(foo_obj)
    return foo_obj
end

local mission_func_tb = { 1,23,4,5};
--[[
function mission_func_tb.OnInit()
    --args_to_table(args_map);
end

function mission_func_tb.OnAccept()

end

function mission_func_tb.OnFinish()
end

function mission_func_tb.OnReShow()

end
--]]
function test_return_func_tb_to_map()
    return 	mission_func_tb;
end



    
--clazz:static_func()