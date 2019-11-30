#include "mt_draw.h"
#include "module.h"
#include "../data/data_manager.h"
#include <DrawCard.pb.h>
#include <DrawCard.proto.fflua.h>


namespace Mt
{

	static MtDrawManager* __mt_draw_mamager = nullptr;

	void MtDrawManager::lua_reg(lua_State* ls)
	{
		ff::fflua_register_t<MtDrawManager, ctor()>(ls, "MtDrawManager")
			.def(&MtDrawManager::LoadDrawTB, "LoadDrawTB")
			.def(&MtDrawManager::LuaGetDrawList,"GetDrawList")
			;
		ff::fflua_register_t<>(ls)
			.def(&MtDrawManager::Instance, "LuaMtDrawManager");
	}

	REGISTER_MODULE(MtDrawManager)
	{
		require("data_manager");
		register_load_function(module_base::STAGE_LOAD, boost::bind(&MtDrawManager::OnLoad, __mt_draw_mamager));
	}

	MtDrawManager::MtDrawManager()
	{
		__mt_draw_mamager = this;
	}

	MtDrawManager& MtDrawManager::Instance()
	{
		return *__mt_draw_mamager;
	}

	int32 MtDrawManager::OnLoad()
	{
		LoadDrawTB();
		return 0;
	}

	void MtDrawManager::LoadDrawTB(bool reload) 
	{
		draw_configs_.clear();
		data_table* table = nullptr;
		if (reload) {
			table = data_manager::instance()->on_reload_file("draw_card");
		}
		else {
			table = data_manager::instance()->get_table("draw_card");
		}

		for (auto i = 0; i < table->get_rows(); ++i)
		{
			auto config = boost::make_shared<Config::DrawCardConfig>();
			auto row = table->find_row(i);
			FillMessageByRow(*config, *row);
			if (config->switch_open() != 0) {
				auto draw_group = config->draw_group();
				auto  iter = draw_configs_.find(draw_group);

				if (iter != draw_configs_.end()) {
					iter->second.push_back(config);
				}
				else {
					std::vector<boost::shared_ptr<Config::DrawCardConfig>> groups;
					groups.push_back(config);
					draw_configs_.insert({ draw_group, groups });
				}
			}
		}

	}

	std::vector<Config::DrawCardConfig *> MtDrawManager::LuaGetDrawList(const int32 function_group, const int32 level)
	{
		std::vector<Config::DrawCardConfig *> pool;

		auto iter = draw_configs_.find(function_group);
		if (iter == draw_configs_.end()) {
			return pool;
		}
				
		for (auto child : draw_configs_[function_group])
		{
			if (child->in_level() > level)
				continue;

			if (child->out_level() <= level)
				continue;


			pool.push_back(child.get());
		}
		return pool;
	}

	void MtDrawManager::GetDrawList(const int32 function_group, const int32 level, std::vector<boost::shared_ptr<Config::DrawCardConfig>> &pool, int32 &all_weiht)
	{
		all_weiht = 0;
		auto iter = draw_configs_.find(function_group);
		if (iter == draw_configs_.end()) {
			return;
		}

		for (auto child : draw_configs_[function_group])
		{
			if(child->in_level()>level)
				continue;

			if (child->out_level() <= level)
				continue;

			all_weiht += child->item_weight();
			pool.push_back(child);
		}
	}

}