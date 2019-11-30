#ifndef _zxero_module_h_
#define _zxero_module_h_

#include "types.h"
#include <boost/noncopyable.hpp>
#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <boost/program_options.hpp>
#include <boost/serialization/singleton.hpp>
#include <boost/make_shared.hpp>

namespace zxero {

	//	模块基础类
	class module_base : boost::noncopyable
	{
	public:
		//	构造模块，初始化设置模块名称
		module_base(const char* mod_name);

		//	获取模块名称
		const std::string& name() const;

	protected:
		//	加载阶段枚举
		enum loading_stage_t
		{
			STAGE_PRE_LOAD,
			STAGE_LOAD,
			STAGE_POST_LOAD,
			//
			LOADING_STAGE_NUMBER,
		};

		//	卸载阶段枚举
		enum unloading_stage_t
		{
			STAGE_PRE_UNLOAD,
			STAGE_UNLOAD,
			STAGE_POST_UNLOAD,

			//
			UNLOADING_STAGE_NUMBER,
		};

		//	加载与卸载阶段函数代理类型
		typedef boost::function< int(loading_stage_t) >			load_function_t;
		typedef boost::function< void(unloading_stage_t) >		unload_function_t;

		//	选项参数处理函数代理类型
		typedef boost::function< void(const boost::program_options::variable_value&) >	option_handler_t;

	protected:
		//	定义模块的依赖模块, 除非特别需要执行顺序，否则不需要执行该函数
		void require(const char* mod_name);

		//	模块管理函数注册函数
		void register_load_function(loading_stage_t stage, const load_function_t& load_function);
		void register_unload_function(unloading_stage_t stage, const unload_function_t& unload_function);

		//	注册模块的选项和回调函数
		//		注册无参数选项，属于开关选项，有则调用选项处理函数，否则就不调用，选项最多允许出现一次
		void register_option(const char* name, const char* desc, const option_handler_t& option_handler);
		//		注册带参数选项，普通的参数选项，有则调用选项处理函数
		//		默认只有一个参数，多参数需要对应设置参数属性
		void register_option(const char* name, const char* desc, boost::program_options::value_semantic* value, const option_handler_t& option_handler);

		//	这两个函数辅助生成option_handler_t, 用于将variable_value的参数，转换为具体类型的参数，或者无参数
		template< typename T >
		option_handler_t opt_delegate(boost::function< void(const T&) > f) {
			return boost::bind(option_handler_delegate<T>, _1, f);
		}
		option_handler_t opt_void_delegate(boost::function< void() > f) {
			return boost::bind(option_handler_void_delegate, _1, f);
		}

	private:
		//	用于选项参数转换的代理函数
		template< typename T >
		static void option_handler_delegate(const boost::program_options::variable_value& vv, boost::function< void(const T&) > f) {
			f(vv.as<T>());
		}
		static void option_handler_void_delegate(const boost::program_options::variable_value&, boost::function< void() > f) {
			f();
		}

	private:
		//	模块名称
		std::string	name_;
		//	
		load_function_t	load_[LOADING_STAGE_NUMBER];
		unload_function_t unload_[UNLOADING_STAGE_NUMBER];
		//	依赖列表
		typedef std::set< std::string > required_module_set_t;
		required_module_set_t required_module_set_;

		//	模块定义的选项集合
		typedef boost::shared_ptr< boost::program_options::option_description >	option_description_ptr_t;
		typedef std::set< option_description_ptr_t > option_description_set_t;
		option_description_set_t option_description_set_;

		//	模块定义的处理函数集合
		typedef std::map< std::string, option_handler_t > option_handler_map_t;
		option_handler_map_t option_handler_map_;
	public:
		//	zXero: 用户不负责调用以下函数
		//	模块管理函数

		//	初始化所有模块
		static int init(int argc, char* argv[]);

		//	加载所有模块
		static int load();

		//	卸载所有模块
		static void unload();

		//

	private:
		typedef boost::shared_ptr< module_base > module_base_ptr_t;
		//	检查模块是否已经处理过依赖了
		static bool is_resolved_require(const module_base_ptr_t&);
		//	检查模块是否正在处理依赖
		static bool is_resolving_require(const module_base_ptr_t&);
		//	处理依赖
		static bool resolve_require(module_base_ptr_t&);
		//	根据模块名称获取模块
		static module_base_ptr_t get_module(const std::string& mod_name);
		//	合并模块的选项到一个选项内，用于help显示
		static bool merge_option(module_base_ptr_t&, boost::program_options::options_description& options);
	};

	typedef boost::shared_ptr< module_base > module_base_ptr_t;

	//	模块工厂链表节点
	struct module_factory_node
	{
		//	由构造函数自动生成模块工厂链表
		module_factory_node();

		//
		module_factory_node*	next_;

		//	工厂接口
		virtual module_base_ptr_t create_module() = 0;
	};

	//	模块模板类，用于构成不同的模块，同时也是为了将模块本身的创建工作延迟到main函数之后，在main函数之前，只有模块工厂链表
	template< typename T >
	class module_factory : public module_factory_node, public boost::serialization::singleton< T >
	{
	public:
		virtual module_base_ptr_t create_module() {
			return boost::make_shared<T>();
		}
	};

	//	模块注册函数，注册模块
	//	REGISTER_MODULE(mod_class)
	//	{
	//		//	在这里写初始化函数，可以调用
	//		//	require						增加模块的依赖模块
	//		//	register_load_function		增加模块的加载调用
	//		//	register_unload_function	增加模块的卸载调用
	//		//	register_option				增加模块的应用程序选项及处理函数
	//	}
	#define REGISTER_MODULE( mod )\
		class mod##_register : public zxero::module_base, public mod{\
			public: mod##_register() :module_base(#mod){ init(); }\
			private: void init();\
		};\
		zxero::module_factory<mod##_register> mod##_factory;\
		void mod##_register::init()

	//	获取模块名称
	//	表示这个做法可能会被吐槽
	#define MODULE_NAME( mod ) #mod
}	//	namespace zxero

#endif	//	#ifndef _zxero_module_h_
