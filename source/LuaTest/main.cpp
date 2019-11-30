

#include <iostream>
#include <string>
#include <assert.h>
using namespace std;

#include "lua/fflua.h"
//#include "types.h"
//#include "lua/fflua.h"
using namespace zxero;

using namespace ff;

	class test
	{
	public:
		test()
		{
		}
		~test()
		{
		}

		int k = -1;
	};

	class base_t
	{
	public:
		base_t() :v(789) {}
		void dump()
		{
			printf("in %s a:%d\n", __FUNCTION__, v);
		}
		int v;
	};
	class foo_t : public base_t
	{
	public:
		void param_test(foo_t* a)
		{
			a->v++;
		}
	public:
		foo_t(int b) :a(b)
		{
			printf("in %s b:%d this=%p\n", __FUNCTION__, b, this);
		}
		~foo_t()
		{
			printf("in %s\n", __FUNCTION__);
		}
		void print(uint64 a, base_t* p) const
		{
			printf("in foo_t::print a:%lld p:%p\n", (uint64)a, p);
		}

		test* get_test() const
		{
			//m_test.k = 10;
			return &m_test;
		}


		static void dumy()
		{
			printf("in %s\n", __FUNCTION__);
		}
		int a;
		mutable test m_test;
	};



	//! lua talbe 可以自动转换为stl 对象
	void dumy(map<string, string> ret, vector<int> a, list<string> b, set<uint64> c)
	{
		printf("in %s begin ------------\n", __FUNCTION__);
		for (map<string, string>::iterator it = ret.begin(); it != ret.end(); ++it)
		{
			printf("map:%s, val:%s:\n", it->first.c_str(), it->second.c_str());
		}
		printf("in %s end ------------\n", __FUNCTION__);
	}

	class clazz {
	public:
		static void static_func() {
			printf("in clazz::%s end ------------\n", __FUNCTION__);
		}
	};

	static void lua_reg(lua_State* ls)
	{
		//! 注册基类函数, ctor() 为构造函数的类型
		fflua_register_t<base_t, ctor()>(ls, "base_t")  //! 注册构造函数
			.def(&base_t::dump, "dump")     //! 注册基类的函数
			.def(&base_t::v, "v");          //! 注册基类的属性
		//typedef test* (foo_t::*foo_mem_fn)(int);
//! 注册子类，ctor(int) 为构造函数， foo_t为类型名称， base_t为继承的基类名称
		fflua_register_t<foo_t, ctor(int)>(ls, "foo_t", "base_t")
			.def(&foo_t::print, "print")        //! 子类的函数
			//.def<test*(foo_t::*)()const>(&foo_t::get_test, "get_test")        //! 子类的函数
			.def(&foo_t::get_test, "get_test")        //! 子类的函数
			.def(&foo_t::a, "a")					//! 子类的字段
			.def(&foo_t::param_test, "param_test");		//! 带参函数

		//
		fflua_register_t<test, ctor()>(ls, "test")
			.def(&test::k, "k");               //! 子类的字段

		fflua_register_t<>(ls)
			.def(&dumy, "dumy");                //! 注册静态函数


		fflua_register_t<clazz, ctor()>(ls, "clazz")
			.def(&clazz::static_func, "static_func");

	}

	int leastBitPos(int n) {
		return (n & 1) ? 1 : leastBitPos(n >> 1) + 1;
	}

	int main(int argc, char** argv)
	{
		printf("%d", sizeof(unsigned));
		//printf("%d", leastBitPos(1));
		//printf("%d", leastBitPos(7));
		//printf("%d", leastBitPos(8));

		fflua_t fflua;
		//fflua.init(true, "../bin/lua_script/");
		//return 1;

		try
		{
			//fflua.setModFuncFlag(true);
			//! 注册C++ 对象到lua中
			fflua.reg(lua_reg);

			//! 载入lua文件
			fflua.add_package_path("./");
#ifdef _WIN32
			fflua.load_file("../test.lua");
#else
			fflua.load_file("test.lua");
#endif
			//! 获取全局变量
			int var = 0;
			assert(0 == fflua.get_global_variable("test_var", var));
			//! 设置全局变量
			assert(0 == fflua.set_global_variable("test_var", ++var));

			//! 执行lua 语句
			fflua.run_string("print(\"exe run_string!!\")");

			////! 调用lua函数, 基本类型作为参数
			//int	arg1 = 1;
			//float   arg2 = 2;
			//double  arg3 = 3;
			//string  arg4 = "4";
			//fflua.call<bool>("test_func", arg1, arg2, arg3, arg4);
			//fflua.call<bool>("Mod:funcTest1", arg1, arg2);

			////! 调用lua函数，stl类型作为参数， 自动转换为lua talbe
			//vector<int> vec;        vec.push_back(100);
			//list<float> lt;         lt.push_back((float)99.99);
			//set<string> st;         st.insert("OhNIce");
			//map<string, int> mp;    mp["key"] = 200;
			//fflua.call<string>("test_stl", vec, lt, st, mp);

		
			////! 调用lua 函数返回 talbe，自动转换为stl结构
			//vec = fflua.call<vector<int> >("test_return_stl_vector");
			//lt = fflua.call<list<float> >("test_return_stl_list");
			//st = fflua.call<set<string> >("test_return_stl_set");
			auto mp = fflua.call< map<string, int> >("test_return_stl_map");
			
			//
			//map<int, int> mp2;
			//mp2 = fflua.call_fun<map<int, int> >("test_return_stl_map2",20);
			//for (auto child : mp2) {
			//	printf("%d----%d", child.first, child.second);
			//}
			//
			//
			////mp = fflua.call<map<string, int> >("test_return_func_tb_to_map");
			////vec = fflua.call<vector<int> >("test_return_func_tb_to_map");
			//
			//
			//////! 调用lua函数，c++ 对象作为参数, foo_t 必须被注册过
			////foo_t* foo_ptr = new foo_t(456);
			////fflua.call<void>("test_object", foo_ptr);
			//////! 调用lua函数，c++ 对象作为返回值, foo_t 必须被注册过 
			////assert(foo_ptr == fflua.call<foo_t*>("test_ret_object", foo_ptr));
			//////! 调用lua函数，c++ 对象作为返回值, 自动转换为基类
			////base_t* base_ptr = fflua.call<base_t*>("test_ret_base_object", foo_ptr);
			////assert(base_ptr == foo_ptr);
			//
			//vector<foo_t*> vec_objs;
			//for (int i = 0; i < 1; i++)
			//{
			//	foo_t* ptr = new foo_t(456);
			//	vec_objs.push_back(ptr);
			//}
			//
			//fflua.call_fun<void>("test_objects", vec_objs);
		}
		catch (exception& e)
		{
			printf("exception:%s\n", e.what());
		}
#ifdef _WIN32
		system("pause");
#endif
		return 0;
	}
