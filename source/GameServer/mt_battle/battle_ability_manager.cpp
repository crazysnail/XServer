#include "battle_ability_manager.h"
#include "module.h"
#include "../data/data_manager.h"

namespace Mt
{

	static BattleAbilityManager* __battle_ability_manager = nullptr;
	REGISTER_MODULE(BattleAbilityManager) 
	{
		require("data_manager");
		register_load_function(module_base::STAGE_LOAD, boost::bind(&BattleAbilityManager::OnLoad, __battle_ability_manager));
	}
	BattleAbilityManager& BattleAbilityManager::Instance()
	{
		return *__battle_ability_manager;
	}

	zxero::int32 BattleAbilityManager::StatusToAbility(Config::BattleObjectStatus status) {
		if (ability_map_.find(status) == std::end(ability_map_)){
			ZXERO_ASSERT(false) << "status " << status <<"ability_map_ size "<< ability_map_.size();
			return 0xffffff;
		} else {
			return ability_map_[status];
		}
	}

	zxero::int32 BattleAbilityManager::OnLoad()
	{
		auto table = data_manager::instance()->get_table("ability");
		for (auto i = 0; i < table->get_rows(); ++i) {
			auto row = table->find_row(i);
			Config::AbilityAndStatusConfig config;
			FillMessageByRow(config, *row);
			int32 ability = 0x0;
			for (int32 j = 0; j < config.can_do_size(); ++j) {
				ability |= int32(config.can_do(j)) << (config.can_do_size()  - 1 - j);
			}
			ability_map_.insert({config.status(), ability});
		}
		return 0;
	}

	BattleAbilityManager::BattleAbilityManager()
	{
		__battle_ability_manager = this;
	}

}