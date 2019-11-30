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

	//	ģ�������
	class module_base : boost::noncopyable
	{
	public:
		//	����ģ�飬��ʼ������ģ������
		module_base(const char* mod_name);

		//	��ȡģ������
		const std::string& name() const;

	protected:
		//	���ؽ׶�ö��
		enum loading_stage_t
		{
			STAGE_PRE_LOAD,
			STAGE_LOAD,
			STAGE_POST_LOAD,
			//
			LOADING_STAGE_NUMBER,
		};

		//	ж�ؽ׶�ö��
		enum unloading_stage_t
		{
			STAGE_PRE_UNLOAD,
			STAGE_UNLOAD,
			STAGE_POST_UNLOAD,

			//
			UNLOADING_STAGE_NUMBER,
		};

		//	������ж�ؽ׶κ�����������
		typedef boost::function< int(loading_stage_t) >			load_function_t;
		typedef boost::function< void(unloading_stage_t) >		unload_function_t;

		//	ѡ�������������������
		typedef boost::function< void(const boost::program_options::variable_value&) >	option_handler_t;

	protected:
		//	����ģ�������ģ��, �����ر���Ҫִ��˳�򣬷�����Ҫִ�иú���
		void require(const char* mod_name);

		//	ģ�������ע�ắ��
		void register_load_function(loading_stage_t stage, const load_function_t& load_function);
		void register_unload_function(unloading_stage_t stage, const unload_function_t& unload_function);

		//	ע��ģ���ѡ��ͻص�����
		//		ע���޲���ѡ����ڿ���ѡ��������ѡ�����������Ͳ����ã�ѡ������������һ��
		void register_option(const char* name, const char* desc, const option_handler_t& option_handler);
		//		ע�������ѡ���ͨ�Ĳ���ѡ��������ѡ�����
		//		Ĭ��ֻ��һ���������������Ҫ��Ӧ���ò�������
		void register_option(const char* name, const char* desc, boost::program_options::value_semantic* value, const option_handler_t& option_handler);

		//	������������������option_handler_t, ���ڽ�variable_value�Ĳ�����ת��Ϊ�������͵Ĳ����������޲���
		template< typename T >
		option_handler_t opt_delegate(boost::function< void(const T&) > f) {
			return boost::bind(option_handler_delegate<T>, _1, f);
		}
		option_handler_t opt_void_delegate(boost::function< void() > f) {
			return boost::bind(option_handler_void_delegate, _1, f);
		}

	private:
		//	����ѡ�����ת���Ĵ�����
		template< typename T >
		static void option_handler_delegate(const boost::program_options::variable_value& vv, boost::function< void(const T&) > f) {
			f(vv.as<T>());
		}
		static void option_handler_void_delegate(const boost::program_options::variable_value&, boost::function< void() > f) {
			f();
		}

	private:
		//	ģ������
		std::string	name_;
		//	
		load_function_t	load_[LOADING_STAGE_NUMBER];
		unload_function_t unload_[UNLOADING_STAGE_NUMBER];
		//	�����б�
		typedef std::set< std::string > required_module_set_t;
		required_module_set_t required_module_set_;

		//	ģ�鶨���ѡ���
		typedef boost::shared_ptr< boost::program_options::option_description >	option_description_ptr_t;
		typedef std::set< option_description_ptr_t > option_description_set_t;
		option_description_set_t option_description_set_;

		//	ģ�鶨��Ĵ���������
		typedef std::map< std::string, option_handler_t > option_handler_map_t;
		option_handler_map_t option_handler_map_;
	public:
		//	zXero: �û�������������º���
		//	ģ�������

		//	��ʼ������ģ��
		static int init(int argc, char* argv[]);

		//	��������ģ��
		static int load();

		//	ж������ģ��
		static void unload();

		//

	private:
		typedef boost::shared_ptr< module_base > module_base_ptr_t;
		//	���ģ���Ƿ��Ѿ������������
		static bool is_resolved_require(const module_base_ptr_t&);
		//	���ģ���Ƿ����ڴ�������
		static bool is_resolving_require(const module_base_ptr_t&);
		//	��������
		static bool resolve_require(module_base_ptr_t&);
		//	����ģ�����ƻ�ȡģ��
		static module_base_ptr_t get_module(const std::string& mod_name);
		//	�ϲ�ģ���ѡ�һ��ѡ���ڣ�����help��ʾ
		static bool merge_option(module_base_ptr_t&, boost::program_options::options_description& options);
	};

	typedef boost::shared_ptr< module_base > module_base_ptr_t;

	//	ģ�鹤������ڵ�
	struct module_factory_node
	{
		//	�ɹ��캯���Զ�����ģ�鹤������
		module_factory_node();

		//
		module_factory_node*	next_;

		//	�����ӿ�
		virtual module_base_ptr_t create_module() = 0;
	};

	//	ģ��ģ���࣬���ڹ��ɲ�ͬ��ģ�飬ͬʱҲ��Ϊ�˽�ģ�鱾��Ĵ��������ӳٵ�main����֮����main����֮ǰ��ֻ��ģ�鹤������
	template< typename T >
	class module_factory : public module_factory_node, public boost::serialization::singleton< T >
	{
	public:
		virtual module_base_ptr_t create_module() {
			return boost::make_shared<T>();
		}
	};

	//	ģ��ע�ắ����ע��ģ��
	//	REGISTER_MODULE(mod_class)
	//	{
	//		//	������д��ʼ�����������Ե���
	//		//	require						����ģ�������ģ��
	//		//	register_load_function		����ģ��ļ��ص���
	//		//	register_unload_function	����ģ���ж�ص���
	//		//	register_option				����ģ���Ӧ�ó���ѡ�������
	//	}
	#define REGISTER_MODULE( mod )\
		class mod##_register : public zxero::module_base, public mod{\
			public: mod##_register() :module_base(#mod){ init(); }\
			private: void init();\
		};\
		zxero::module_factory<mod##_register> mod##_factory;\
		void mod##_register::init()

	//	��ȡģ������
	//	��ʾ����������ܻᱻ�²�
	#define MODULE_NAME( mod ) #mod
}	//	namespace zxero

#endif	//	#ifndef _zxero_module_h_
