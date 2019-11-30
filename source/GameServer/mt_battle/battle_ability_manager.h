#ifndef MTONLINE_GAMESERVER_BATTLE_ABILITY_MANAGER_H__
#define MTONLINE_GAMESERVER_BATTLE_ABILITY_MANAGER_H__

#include "../mt_types.h"
#include <bitset>
#include "AllConfigEnum.pb.h"
#include <AbilityAndStatus.pb.h>

namespace Mt
{
	class BattleAbilityManager : public boost::noncopyable
	{
	public:
		static  BattleAbilityManager& Instance();
		BattleAbilityManager();
		int32 OnLoad();
		int32 StatusToAbility(Config::BattleObjectStatus status);
	private:
		std::map<Config::BattleObjectStatus, int32> ability_map_;
	};
}
#endif // MTONLINE_GAMESERVER_BATTLE_ABILITY_MANAGER_H__
