#ifndef __RANK_RANKLIST_H__
#define __RANK_RANKLIST_H__

#include "ranklist.h"
namespace Mt
{
	struct guilduserdamage
	{
		guilduserdamage();
		guilduserdamage(const guilduserdamage & a);

		bool operator>(const guilduserdamage &a) const;

		bool operator<(const guilduserdamage &a) const;
		
		bool operator!=(const guilduserdamage &a);
		
		bool operator==(const guilduserdamage &a);

		guilduserdamage& operator=(const guilduserdamage &a);

		//void Filldata(Packet::GuildUserDamage &data);
		boost::shared_ptr<Packet::GuildUserDamage> data;
	};

	struct guildalldamage
	{
		guildalldamage();
		guildalldamage(const guildalldamage & a);

		bool operator>(const guildalldamage &a) const;

		bool operator<(const guildalldamage &a) const;

		bool operator!=(const guildalldamage &a);

		bool operator==(const guildalldamage &a);

		guildalldamage& operator=(const guildalldamage &a);

		//void Filldata(Packet::GuildUserDamage &data);
		boost::shared_ptr<Packet::GuildAllDamage> data;
	};

	struct guildbossuserdamage
	{
		guildbossuserdamage();
		guildbossuserdamage(const guildbossuserdamage & a);

		bool operator>(const guildbossuserdamage &a) const;

		bool operator<(const guildbossuserdamage &a) const;

		bool operator!=(const guildbossuserdamage &a);

		bool operator==(const guildbossuserdamage &a);

		guildbossuserdamage& operator=(const guildbossuserdamage &a);

		//void Filldata(Packet::GuildUserDamage &data);
		boost::shared_ptr<Packet::GuildBossUserDamage> data;
	};

	struct worldbossuserdamage
	{
		worldbossuserdamage()
		{
			playerid_ = INVALID_GUID;
			playername_ = "";
			damagevalue_ = 0;
			allbosshp_ = 0;
		}

		worldbossuserdamage(const worldbossuserdamage & a)
		{
			playerid_ = a.playerid_;
			playername_ = a.playername_;
			damagevalue_ = a.damagevalue_;
			allbosshp_ = a.allbosshp_;
		}

		bool operator>(const worldbossuserdamage &a) const
		{
			if (damagevalue_ > a.damagevalue_)
			{
				return true;
			}
			return false;
		}

		bool operator<(const worldbossuserdamage &a) const
		{
			if (damagevalue_ < a.damagevalue_)
			{
				return true;
			}
			return false;
		}

		bool operator!=(const worldbossuserdamage &a)
		{
			if (damagevalue_ != a.damagevalue_)
			{
				return true;
			}
			return false;
		}

		bool operator==(const worldbossuserdamage &a)
		{
			return (playerid_ == a.playerid_);
		}

		worldbossuserdamage& operator=(const worldbossuserdamage &a)
		{
			playerid_ = a.playerid_;
			playername_ = a.playername_;
			damagevalue_ = a.damagevalue_;
			allbosshp_ = a.allbosshp_;
			return *this;
		}

		//void Filldata(Packet::GuildUserDamage &data);
		uint64 playerid_;
		std::string playername_;
		int32 damagevalue_;
		uint64 allbosshp_;
		
	};

	struct guildbattleweekrankinfo
	{
		guildbattleweekrankinfo()
		{
			guildid_ = INVALID_GUID;
			guildname_ = "";
			level_ = 1;
			score_ = 0;
		}
		guildbattleweekrankinfo(const guildbattleweekrankinfo & a)
		{
			guildid_ = a.guildid_;
			guildname_ = a.guildname_;
			level_ = a.level_;
			score_ = a.score_;
		}

		bool operator>(const guildbattleweekrankinfo &a) const
		{
			if (score_ > a.score_)
			{
				return true;
			}
			return false;
		}

		bool operator<(const guildbattleweekrankinfo &a) const
		{
			if (score_ < a.score_)
			{
				return true;
			}
			return false;
		}

		bool operator!=(const guildbattleweekrankinfo &a)
		{
			if (score_ != a.score_)
			{
				return true;
			}
			return false;
		}

		bool operator==(const guildbattleweekrankinfo &a)
		{
			return (guildid_ == a.guildid_);
		}

		guildbattleweekrankinfo& operator=(const guildbattleweekrankinfo &a)
		{
			guildid_ = a.guildid_;
			guildname_ = a.guildname_;
			level_ = a.level_;
			score_ = a.score_;
			return *this;
		}
		uint64 guildid_;
		std::string guildname_;
		int32 level_;
		int32 score_;
	};

	struct guildbattleallrankinfo
	{
		guildbattleallrankinfo()
		{
			guildid_ = INVALID_GUID;
			guildname_ = "";
			level_ = 1;
			battlewintimes_ = 0;
			battletimes_ = 0;
		}
		guildbattleallrankinfo(const guildbattleallrankinfo & a)
		{
			guildid_ = a.guildid_;
			guildname_ = a.guildname_;
			level_ = a.level_;
			battletimes_ = a.battletimes_;
			battlewintimes_ = a.battlewintimes_;
		}

		bool operator>(const guildbattleallrankinfo &a) const
		{
			if (battlewintimes_ > a.battlewintimes_)
			{
				return true;
			}
			return false;
		}

		bool operator<(const guildbattleallrankinfo &a) const
		{
			if (battlewintimes_ < a.battlewintimes_)
			{
				return true;
			}
			return false;
		}

		bool operator!=(const guildbattleallrankinfo &a)
		{
			if (battlewintimes_ != a.battlewintimes_)
			{
				return true;
			}
			return false;
		}

		bool operator==(const guildbattleallrankinfo &a)
		{
			return (guildid_ == a.guildid_);
		}

		guildbattleallrankinfo& operator=(const guildbattleallrankinfo &a)
		{
			guildid_ = a.guildid_;
			guildname_ = a.guildname_;
			level_ = a.level_;
			battlewintimes_ = a.battlewintimes_;
			battletimes_ = a.battletimes_;
			return *this;
		}
		uint64 guildid_;
		std::string guildname_;
		int32 level_;
		int32 battlewintimes_;
		int32 battletimes_;
	};
}


#endif