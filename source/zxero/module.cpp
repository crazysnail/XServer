#include "module.h"
#include "log.h"
#include "app.h"
#include <iostream>
#include <boost/foreach.hpp>
#include <boost/assert.hpp>

namespace zxero
{
	///////////////////////////////////////////////////////////////////////////
	//	ģ�鹤��������ڵ�
	static module_factory_node*	sg_module_factory_node = nullptr;

	///////////////////////////////////////////////////////////////////////////
	//	�Զ�������ƣ����ڽ�����ģ�����ӵ�һ��
	module_factory_node::module_factory_node()
		:next_(sg_module_factory_node)
	{
		sg_module_factory_node = this;
	}

	///////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////

	module_base::module_base(const char* mod_name)
		:name_(mod_name)
	{

	}

	const std::string& module_base::name() const
	{
		return name_;
	}

	///////////////////////////////////////////////////////////////////////////
	void module_base::require(const char* name)
	{
		required_module_set_.insert(name);
	}

	void module_base::register_load_function(loading_stage_t stage, const load_function_t& load_function)
	{
		ZXERO_ASSERT(stage < LOADING_STAGE_NUMBER);
		ZXERO_ASSERT(load_function);

		load_[stage] = load_function;
	}

	void module_base::register_unload_function(unloading_stage_t stage, const unload_function_t& unload_function)
	{
		ZXERO_ASSERT(stage < UNLOADING_STAGE_NUMBER);
		ZXERO_ASSERT(unload_function);

		unload_[stage] = unload_function;
	}

	void module_base::register_option(const char* name, const char* desc, const option_handler_t& option_handler)
	{
		ZXERO_ASSERT(name);
		//ZXERO_ASSERT(desc);
		ZXERO_ASSERT(option_handler);

		auto option = boost::make_shared< boost::program_options::option_description >(name, new boost::program_options::untyped_value(true), desc);
		option_description_set_.insert(option);
		option_handler_map_.insert(std::make_pair(option->long_name(), option_handler));
	}

	void module_base::register_option(const char* name, const char* desc, boost::program_options::value_semantic* value, const option_handler_t& option_handler)
	{
		ZXERO_ASSERT(name);
		//ZXERO_ASSERT(desc);
		ZXERO_ASSERT(value);
		ZXERO_ASSERT(option_handler);

		auto option = boost::make_shared< boost::program_options::option_description >(name, value, desc);
		option_description_set_.insert(option);
		option_handler_map_.insert(std::make_pair(option->long_name(), option_handler));
	}


	///////////////////////////////////////////////////////////////////////////
	//	��������ģ����б�
	typedef std::map< std::string, module_base_ptr_t > module_map_t;
	typedef std::vector< module_base_ptr_t > module_list_t;
	typedef std::set< module_base_ptr_t > module_set_t;

	module_list_t sg_module_list;

#define sg_module_list_foreach(name) BOOST_FOREACH(module_base_ptr_t& name, sg_module_list)
#define sg_module_list_reverse_foreach(name) BOOST_REVERSE_FOREACH(module_base_ptr_t& name, sg_module_list)

	//	ȫ����ʱ������init����ͻᱻ���
	module_map_t sg_module_map;
	module_set_t sg_resolved_require_module_set;
	module_set_t sg_resolving_require_module_set;

	///////////////////////////////////////////////////////////////////////////
	//	��������ģ���ѡ���ѡ�����
	typedef std::vector< boost::function< void(const boost::program_options::variable_value&) > >	option_handler_list_t;
	typedef std::map< std::string, option_handler_list_t >	option_handler_map_t;
	typedef std::map< std::string, boost::shared_ptr< boost::program_options::option_description > > options_description_map_t;

	//	ȫ��ѡ��������������, init֮��Ҳ��ȫ���������
	option_handler_map_t sg_option_handler_map;
	options_description_map_t sg_option_description_map;

	//
	int module_base::init(int argc, char* argv[])
	{
		//	����ģ�鹤������������ģ��	
		while (sg_module_factory_node) {
			module_base_ptr_t module = sg_module_factory_node->create_module();
			if (!module){
				return 1;
			}
			//	����ģ��ӳ���������Ҫ�������ģ���б��б��е�ģ��������������ϵ�ģ�����������ģ�齫���б��ǰ��
			sg_module_map.insert(std::make_pair(module->name(), module));
			sg_module_factory_node = sg_module_factory_node->next_;
		}

		//	��ʼ����ģ���������ϵ
		//for (module_map_t::iterator iter = sg_module_map.begin(); iter != sg_module_map.end(); ++iter)
		for (auto& str_module : sg_module_map)
		{
			//	������û�н��������ϵ��ģ�飬���������ϵ
			if (!is_resolved_require(str_module.second) && !resolve_require(str_module.second)) {
				//	�޷����������ϵ��ģ�齫�˳�
				return 1;
			}
		}

		//	
		sg_module_map.clear();
		sg_resolved_require_module_set.clear();

		//	����������ϵ�ϲ�����ģ���ѡ��
		boost::program_options::options_description	options("app");
		sg_module_list_foreach(module)
		{
			if (!merge_option(module, options)) {
				//	�޷��ϲ�ģ��ѡ��Ҳ���˳�
				return 1;
			}
		}

		//	�������в�����֧���޷�ʶ��Ĳ�����������ڲ�ʶ��Ĳ�������ʾ֮�����ҷ���
		boost::shared_ptr< boost::program_options::basic_parsed_options<char> > parsed_options;
		try {
			parsed_options = boost::make_shared< boost::program_options::basic_parsed_options<char> >(boost::program_options::command_line_parser(argc, argv).options(options).allow_unregistered().run());
		}
		catch (std::exception& e) {
			LOG_FATAL << "option args invalid: " << e.what();
			return 1;
		}


		//	��ȡδ�������
#ifdef ZXERO_OS_LINUX
		using namespace std;
		std::vector< std::string > ur_opts = boost::program_options::collect_unrecognized(parsed_options->options, boost::program_options::include_positional);
		if (ur_opts.size())
		{
			//	����Ƿ���help����
			if (ur_opts.size() == 1 && (ur_opts[0] == "--help" || ur_opts[0] == "-h"))
			{
				std::cout << options ;
				//	�����Ƴ�
				return -1;
			}

			//	������δ����������֣���ʾ֮
			LOG_ERROR << "unrecognized options:";
			for (auto& str : ur_opts)
				LOG_ERROR << " " << str;

			LOG_FATAL << "use --help to check all options, program exit";
			//	�����˳�
			return 1;
		}
#endif

		//	��ȡ�������
		boost::program_options::variables_map vm;
		try {
			boost::program_options::store(*parsed_options, vm);
		}
		catch (std::exception& e)
		{
			LOG_FATAL << "option args invalid: " << e.what();
			return 1;
		}
		

		//	ִ�����в���������
		for (auto& opt : vm)
		{
			option_handler_list_t& handlers = sg_option_handler_map[opt.first];
			for (auto handler : handlers)
				handler(opt.second);
		}

		//	�����������
		sg_option_description_map.clear();
		
		return 0;
	}

	int module_base::load()
	{
		//	Ԥ���ؽ׶�
		//for (module_list_t::iterator iter = sg_module_list.begin(); iter != sg_module_list.end(); ++iter)
		//for (auto module : sg_module_list)							//	c++11 ��д��
		sg_module_list_foreach(module)									//	boost ��д��
		{
			if (!app::is_running()) return -1;
			load_function_t pre_load = module->load_[STAGE_PRE_LOAD];
			if (pre_load)
			{
				int ret = pre_load(STAGE_PRE_LOAD);
				if (ret)
					return ret;
			}
			else
				LOG_DEBUG << module->name() << " default pre load";
		}

		//	����lambda����д���ĺ���, �����������⡣����
		//std::for_each(sg_module_list.begin(), sg_module_list.end(), [](module_base_ptr_t module){
		//	load_function_t pre_load = module->load_[STAGE_PRE_LOAD];
		//	if (pre_load)
		//	{
		//		int ret = pre_load(STAGE_PRE_LOAD);
		//		if (ret)
		//			return ret;
		//	}
		//	else
		//		std::cout << module->name() << " default pre load" ;
		//});

		//	���ؽ׶�
		//for (module_list_t::iterator iter = sg_module_list.begin(); iter != sg_module_list.end(); ++iter)
		sg_module_list_foreach(module)
		{
			if (!app::is_running()) return -1;
			load_function_t load = module->load_[STAGE_LOAD];
			if (load)
			{
				LOG_INFO << "load module:" << module->name();
				int ret = load(STAGE_LOAD);
				if (ret)
					return ret;
			}
			else
				LOG_DEBUG << module->name() << " default load";
		}

		//	����ؽ׶�
		//for (module_list_t::iterator iter = sg_module_list.begin(); iter != sg_module_list.end(); ++iter)
		sg_module_list_foreach(module)
		{
			if (!app::is_running()) return -1;
			load_function_t post_load = module->load_[STAGE_POST_LOAD];
			if (post_load)
			{
				int ret = post_load(STAGE_POST_LOAD);
				if (ret)
					return ret;
			}
			else
				LOG_DEBUG << module->name() << " default post load";
		}

		return 0;
	}

	void module_base::unload()
	{
		//	Ԥж�ؽ׶�
		//for (module_list_t::reverse_iterator iter = sg_module_list.rbegin(); iter != sg_module_list.rend(); ++iter)
		sg_module_list_reverse_foreach(module)
		{
			unload_function_t pre_unload = module->unload_[STAGE_PRE_UNLOAD];
			if (pre_unload)
				pre_unload(STAGE_PRE_UNLOAD);
			else
				LOG_DEBUG << module->name() << " default pre unload";
		}

		//	ж�ؽ׶�
		//for (module_list_t::reverse_iterator iter = sg_module_list.rbegin(); iter != sg_module_list.rend(); ++iter)
		sg_module_list_reverse_foreach(module)
		{
			unload_function_t unload = module->unload_[STAGE_UNLOAD];
			if (unload)
				unload(STAGE_UNLOAD);
			else
				LOG_DEBUG << module->name() << " default unload";
		}

		//	��ж�ؽ׶�
		//for (module_list_t::reverse_iterator iter = sg_module_list.rbegin(); iter != sg_module_list.rend(); ++iter)
		sg_module_list_reverse_foreach(module)
		{
			unload_function_t post_unload = module->unload_[STAGE_POST_UNLOAD];
			if (post_unload)
				post_unload(STAGE_POST_UNLOAD);
			else
				LOG_DEBUG << module->name() << " default post unload";
		}

		//	�������ģ��
		sg_module_list.clear();
	}

	bool module_base::merge_option(module_base_ptr_t& module, boost::program_options::options_description& options)
	{
		//	�ϲ�ѡ������
		for (auto& desc : module->option_description_set_)
		{
			//	���ѡ���Ƿ��Ѿ�����
			auto iter = sg_option_description_map.find(desc->long_name());
			if (iter != sg_option_description_map.end())
			{
				//	���ڴ��ڵ�ѡ�Ҫ�жϵ�ǰѡ���ǰѡ���Ƿ��Ǽ��ݵ�
				const boost::program_options::typed_value_base* current_type = dynamic_cast<const boost::program_options::typed_value_base*>(desc->semantic().get());
				const boost::program_options::typed_value_base* exist_type = dynamic_cast<const boost::program_options::typed_value_base*>(iter->second->semantic().get());
				if (!current_type && !exist_type) {
					//	zXero: ����������һ��˵��������untyped_value�ⶼ��typed_value_base�����Զ�̬ת��ʧ�ܵĶ���Ϊ��untyped_value
					continue;
				}
				if (!current_type || !exist_type || current_type->value_type() != exist_type->value_type())
				{
					//	ѡ��������Ͳ�ƥ��
					return false;
				}
			}
			else
			{
				//	����ѡ��
				sg_option_description_map.insert(std::make_pair(desc->long_name(), desc));
				options.add(desc);
			}
		}

		//	�ϲ�ѡ�����
		for (auto& handler : module->option_handler_map_)
		{
			//	
			auto iter = sg_option_handler_map.find(handler.first);
			if (iter != sg_option_handler_map.end())
			{
				//	����ѡ������б�������к����б�
				iter->second.push_back(handler.second);
			}
			else
			{
				//	�½��б�
				option_handler_list_t list;
				list.push_back(handler.second);
				//	����ӳ���
				sg_option_handler_map.insert(std::make_pair(handler.first, list));
			}
		}

		//	�������Ҫ���м�����
		module->option_description_set_.clear();
		module->option_handler_map_.clear();

		return true;
	}

	///////////////////////////////////////////////////////////////////////////
	//
	bool module_base::is_resolved_require(const module_base_ptr_t& module)
	{
		return sg_resolved_require_module_set.find(module) != sg_resolved_require_module_set.end();
	}

	bool module_base::is_resolving_require(const module_base_ptr_t& module)
	{
		return sg_resolving_require_module_set.find(module) != sg_resolving_require_module_set.end();
	}

	module_base_ptr_t module_base::get_module(const std::string& mod_name)
	{
		module_map_t::iterator iter = sg_module_map.find(mod_name);
		if (iter != sg_module_map.end())
			return iter->second;

		return nullptr;
	}

	bool module_base::resolve_require(module_base_ptr_t& module)
	{
		//	�Ѿ���������ˣ��ͷ�����ȷ
		if (is_resolved_require(module))
			return true;

		//	ѭ�������ˣ������ͻʧ��
		if (is_resolving_require(module))
        {
            LOG_FATAL << "module " << module->name() << " require recursive";
			return false;
        }

		//	�������ڽ����ͻ��ģ�鼯��
		sg_resolving_require_module_set.insert(module);

		//	��������require_set��������
		for (required_module_set_t::iterator iter = module->required_module_set_.begin(); iter != module->required_module_set_.end(); ++iter)
		{
			module_base_ptr_t require = get_module(*iter);
			if (!require)
			{
                LOG_FATAL << "module " << module->name() << " require " << *iter << "not exist";
				//	ģ�鲻����
				return false;
			}
			if (!resolve_require(require))
			{
                LOG_FATAL << "module " << module->name() << " require resolve failed";
				//	�������ʧ��
				return false;
			}
		}

		//	��ͻ������
		sg_resolving_require_module_set.erase(module);
		sg_resolved_require_module_set.insert(module);

		//	����ģ���б�
		sg_module_list.push_back(module);
        LOG_DEBUG << "add module: " << module->name();

		return true;
	}

}	//	namespace zXero
