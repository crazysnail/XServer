#ifndef MTONLINE_GAMESERVER_MT_RAID_GUILDBATTLE_H__
#define MTONLINE_GAMESERVER_MT_RAID_GUILDBATTLE_H__
#include "mt_raid.h"
namespace Mt
{
	class MtRaidGuildBattle : public MtRaid
	{
	public:
		MtRaidGuildBattle() {}
		MtRaidGuildBattle(const RaidHelper & helper, const int32 rtid);
		~MtRaidGuildBattle() {}
	public:
		virtual void SyncInfo(const boost::shared_ptr<MtPlayer>& player);
	};
}
#endif //   MTONLINE_GAMESERVER_MT_RAID_GUILDBATTLE_H__