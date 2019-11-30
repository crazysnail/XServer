#include "guilduserdamage_ranklist.h"
#include <Guild.pb.h>

namespace Mt
{
	guilduserdamage::guilduserdamage()
	{
		data = boost::make_shared<Packet::GuildUserDamage>();
		if (data)
		{
			data->set_playerid_(INVALID_GUID);
			//data->set_guildid_(INVALID_GUID);
			//data->set_paramid_(0);
			data->set_playername_("");
			data->set_guildname_("");
			data->set_playerhair_(0);
			data->set_damagevalue_(0);

			int32 count = data->GetDescriptor()->FindFieldByName("actor_config_id_")->options().GetExtension(Packet::column_count);
			for (int32 i=0;i<count;i++)
			{
				data->add_actor_config_id_(0);
			}
		}
	}
	guilduserdamage::guilduserdamage(const guilduserdamage & a)
	{
		data = boost::make_shared<Packet::GuildUserDamage>();
		if (data)
		{
			data->CopyFrom(*a.data.get());
		}
	}
	bool guilduserdamage::operator>(const guilduserdamage &a) const
	{
		if (data->damagevalue_() > a.data->damagevalue_())
		{
			return true;
		}
		return false;
	}
	bool guilduserdamage::operator<(const guilduserdamage &a) const
	{
		if (data->damagevalue_() < a.data->damagevalue_())
		{
			return true;
		}
		return false;
	}
	bool guilduserdamage::operator!=(const guilduserdamage &a)
	{
		if (data->damagevalue_() != a.data->damagevalue_())
		{
			return true;
		}
		return false;
	}
	bool guilduserdamage::operator==(const guilduserdamage &a)
	{
		return (data->playerid_() == a.data->playerid_());
	}
	guilduserdamage& guilduserdamage::operator=(const guilduserdamage &a)
	{
		if (data)
		{
			data->CopyFrom(*a.data.get());
		}
		return *this;
	}

	guildalldamage::guildalldamage()
	{
		data = boost::make_shared<Packet::GuildAllDamage>();
		if (data)
		{
			data->set_guildid_(INVALID_GUID);
			data->set_guildname_("");
			data->set_damagevalue_(0);
		}
	}
	guildalldamage::guildalldamage(const guildalldamage & a)
	{
		data = boost::make_shared<Packet::GuildAllDamage>();
		if (data)
		{
			data->CopyFrom(*a.data.get());
		}
	}
	bool guildalldamage::operator>(const guildalldamage &a) const
	{
		if (data->damagevalue_() > a.data->damagevalue_())
		{
			return true;
		}
		return false;
	}
	bool guildalldamage::operator<(const guildalldamage &a) const
	{
		if (data->damagevalue_() < a.data->damagevalue_())
		{
			return true;
		}
		return false;
	}
	bool guildalldamage::operator!=(const guildalldamage &a)
	{
		if (data->damagevalue_() != a.data->damagevalue_())
		{
			return true;
		}
		return false;
	}
	bool guildalldamage::operator==(const guildalldamage &a)
	{
		return (data->guildid_() == a.data->guildid_());
	}
	guildalldamage& guildalldamage::operator=(const guildalldamage &a)
	{
		if (data)
		{
			data->CopyFrom(*a.data.get());
		}
		return *this;
	}

	guildbossuserdamage::guildbossuserdamage()
	{
		data = boost::make_shared<Packet::GuildBossUserDamage>();
		if (data)
		{
			data->set_playerid_(INVALID_GUID);
			data->set_playername_("");
			data->set_playerhair_(0);
			data->set_damagevalue_(0);
		}
	}
	guildbossuserdamage::guildbossuserdamage(const guildbossuserdamage & a)
	{
		data = boost::make_shared<Packet::GuildBossUserDamage>();
		if (data)
		{
			data->CopyFrom(*a.data.get());
		}
	}
	bool guildbossuserdamage::operator>(const guildbossuserdamage &a) const
	{
		if (data->damagevalue_() > a.data->damagevalue_())
		{
			return true;
		}
		return false;
	}
	bool guildbossuserdamage::operator<(const guildbossuserdamage &a) const
	{
		if (data->damagevalue_() < a.data->damagevalue_())
		{
			return true;
		}
		return false;
	}
	bool guildbossuserdamage::operator!=(const guildbossuserdamage &a)
	{
		if (data->damagevalue_() != a.data->damagevalue_())
		{
			return true;
		}
		return false;
	}
	bool guildbossuserdamage::operator==(const guildbossuserdamage &a)
	{
		return (data->playerid_() == a.data->playerid_());
	}
	guildbossuserdamage& guildbossuserdamage::operator=(const guildbossuserdamage &a)
	{
		if (data)
		{
			data->CopyFrom(*a.data.get());
		}
		return *this;
	}

}