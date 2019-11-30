#include "../mt_market/mt_market.h"
#include "../mt_server/mt_server.h"
#include "../mt_player/mt_player.h"
#include "mt_player_guild_proxy.h"
#include "pb2json.h"
#include "Guild.pb.h"
#include "G2SMessage.pb.h"

namespace Mt
{
	void MtPlayerGuildProxy::lua_reg(lua_State* ls)
	{
		ff::fflua_register_t<MtPlayerGuildProxy, ctor()>(ls, "MtPlayerGuildProxy")
			.def(&MtPlayerGuildProxy::GetGuildPosition, "GetGuildPosition")
			.def(&MtPlayerGuildProxy::SetGuildPosition, "SetGuildPosition")
			.def(&MtPlayerGuildProxy::Getbdonation, "Getbdonation")
			.def(&MtPlayerGuildProxy::Setbdonation, "Setbdonation")
			.def(&MtPlayerGuildProxy::Getbbonus, "Getbbonus")
			.def(&MtPlayerGuildProxy::Setbbonus, "Setbbonus")
			.def(&MtPlayerGuildProxy::Getcurcontribution, "Getcurcontribution")
			.def(&MtPlayerGuildProxy::Setcurcontribution, "Setcurcontribution")
			.def(&MtPlayerGuildProxy::Getweekcontribution, "Getweekcontribution")
			.def(&MtPlayerGuildProxy::Setweekcontribution, "Setweekcontribution")
			.def(&MtPlayerGuildProxy::Getallcontribution, "Getallcontribution")
			.def(&MtPlayerGuildProxy::Setallcontribution, "Setallcontribution")
			.def(&MtPlayerGuildProxy::Getjointime, "Getjointime")
			.def(&MtPlayerGuildProxy::Setjointime, "Setjointime")
			.def(&MtPlayerGuildProxy::GetBuildLevel, "GetBuildLevel")
			.def(&MtPlayerGuildProxy::SetBuildLevel, "SetBuildLevel")
			.def(&MtPlayerGuildProxy::GetWetcopyMonsterHp,"GetWetcopyMonsterHp")
			.def(&MtPlayerGuildProxy::SetWetcopyMonsterHp, "SetWetcopyMonsterHp")
			.def(&MtPlayerGuildProxy::ResetWetcopy, "ResetWetcopy")
			.def(&MtPlayerGuildProxy::SetWetcopyCopyId, "SetWetcopyCopyId")
			.def(&MtPlayerGuildProxy::GetWetcopyCopyId, "GetWetcopyCopyId")
			.def(&MtPlayerGuildProxy::SetWetcopyStageId, "SetWetcopyStageId")
			.def(&MtPlayerGuildProxy::GetWetcopyStageId, "GetWetcopyStageId")
			;
	}
	MtPlayerGuildProxy::MtPlayerGuildProxy(boost::weak_ptr<MtPlayer> p):player_(p)
	{
		guilddata_ = boost::make_shared<Packet::PlayerGuildUserData>();
		if (guilddata_)
		{
			guilddata_->set_position(Packet::GuildPosition::TRAINEE);
			guilddata_->set_bdonation(0);
			guilddata_->set_bbonus(0);
			guilddata_->set_curcontribution(0);
			guilddata_->set_weekcontribution(0);
			guilddata_->set_allcontribution(0);
			guilddata_->set_jointime(0);
			guilddata_->set_guildlevel(-1);
			guilddata_->set_cashboxlevel(-1);
			guilddata_->set_shoplevel(-1);
			guilddata_->set_storagelevel(-1);
		}
		
	}
	MtPlayerGuildProxy::~MtPlayerGuildProxy()
	{

	}

	void MtPlayerGuildProxy::G2SSyncPlayerGuildData(const boost::shared_ptr<G2S::SyncPlayerGuildData>& message)
	{
		if (player_.expired())
			return;
		if (message)
		{
			guilddata_->CopyFrom(message->guild());
			auto market = player_.lock()->FindMarket(Config::MarketType::MarketType_Union);
			if (market)
			{
				market->Refresh(true);
			}
			if (player_.lock()->GetGuildName() != guilddata_->guildname())
			{
				player_.lock()->SetGuildName(guilddata_->guildname());
			}

			player_.lock()->RunTimeStatus().set_guildposition(guilddata_->position());
			player_.lock()->BroadCastRunTimeStatus();

			Json::Value value;
			FormatToJson(value,*guilddata_.get());
			LOG_INFO << "PlayerId="<< player_.lock()->Guid()<<" SyncPlayerGuildData="<< JsonToString(value);
		}
	}


	int32 MtPlayerGuildProxy::GetGuildPosition()
	{
		if (player_.expired())
			return Packet::GuildPosition::TRAINEE;
		if (player_.lock()->GetGuildID() == INVALID_GUID)
		{
			//ZXERO_ASSERT(false) << " null guild " << player_.lock()->Guid();
			return Packet::GuildPosition::TRAINEE;
		}
		return guilddata_->position();
	}
	void MtPlayerGuildProxy::SetGuildPosition(int32 position)
	{
		if (player_.expired())
			return ;
		if (player_.lock()->GetGuildID() == INVALID_GUID)
		{
			ZXERO_ASSERT(false) << " null guild " << player_.lock()->Guid();
			player_.lock()->RunTimeStatus().set_guildposition(Packet::GuildPosition::TRAINEE);
			return;
		}
		guilddata_->set_position((Packet::GuildPosition)position);
		player_.lock()->RunTimeStatus().set_guildposition((Packet::GuildPosition)position);
		player_.lock()->BroadCastRunTimeStatus();
	}
	int32 MtPlayerGuildProxy::Getbdonation()
	{
		if (player_.expired())
			return 0;
		if (player_.lock()->GetGuildID() == INVALID_GUID)
		{
			ZXERO_ASSERT(false) << " null guild " << player_.lock()->Guid();
			return 0;
		}
		return guilddata_->bdonation();
	}
	void MtPlayerGuildProxy::Setbdonation(int32 bdonation)
	{
		if (player_.expired())
			return ;
		if (player_.lock()->GetGuildID() == INVALID_GUID)
		{
			ZXERO_ASSERT(false) << " null guild " << player_.lock()->Guid();
			return;
		}
		guilddata_->set_bdonation(bdonation);
	}
	int32 MtPlayerGuildProxy::Getbbonus()
	{
		if (player_.expired())
			return 0;
		if (player_.lock()->GetGuildID() == INVALID_GUID)
		{
			ZXERO_ASSERT(false) << " null guild " << player_.lock()->Guid();
			return 0;
		}
		return guilddata_->bbonus();
	}
	void MtPlayerGuildProxy::Setbbonus(int32 bbonus)
	{
		if (player_.expired())
			return ;
		if (player_.lock()->GetGuildID() == INVALID_GUID)
		{
			ZXERO_ASSERT(false) << " null guild " << player_.lock()->Guid();
			return;
		}
		guilddata_->set_bbonus(bbonus);
	}
	int32 MtPlayerGuildProxy::Getcurcontribution()
	{
		if (player_.expired())
			return 0;
		if (player_.lock()->GetGuildID() == INVALID_GUID)
		{
			ZXERO_ASSERT(false) << " null guild " << player_.lock()->Guid();
			return 0;
		}
		return guilddata_->curcontribution();
	}
	void MtPlayerGuildProxy::Setcurcontribution(int32 curcontribution)
	{
		if (player_.expired())
			return;
		if (player_.lock()->GetGuildID() == INVALID_GUID)
		{
			ZXERO_ASSERT(false) << " null guild " << player_.lock()->Guid();
			return;
		}
		guilddata_->set_curcontribution(curcontribution);
		Server::Instance().SendS2GCommonMessage("S2GSyncGuildUserCurcontribution", { curcontribution }, { (int64)player_.lock()->Guid(),(int64)player_.lock()->GetGuildID() }, {});
	}
	int32 MtPlayerGuildProxy::Getweekcontribution()
	{
		if (player_.expired())
			return 0;
		if (player_.lock()->GetGuildID() == INVALID_GUID)
		{
			ZXERO_ASSERT(false) << " null guild " << player_.lock()->Guid();
			return 0;
		}
		return guilddata_->weekcontribution();
	}
	void MtPlayerGuildProxy::Setweekcontribution(int32 weekcontribution)
	{
		if (player_.expired())
			return ;
		if (player_.lock()->GetGuildID() == INVALID_GUID)
		{
			ZXERO_ASSERT(false) << " null guild " << player_.lock()->Guid();
			return;
		}
		guilddata_->set_weekcontribution(weekcontribution);
		Server::Instance().SendS2GCommonMessage("S2GSyncGuildUserWeekcontribution", { weekcontribution }, { (int64)player_.lock()->Guid(),(int64)player_.lock()->GetGuildID() }, {});
	}
	int32 MtPlayerGuildProxy::Getallcontribution()
	{
		if (player_.expired())
			return Packet::GuildPosition::TRAINEE;
		if (player_.lock()->GetGuildID() == INVALID_GUID)
		{
			ZXERO_ASSERT(false) << " null guild " << player_.lock()->Guid();
			return 0;
		}
		return guilddata_->allcontribution();
	}
	void MtPlayerGuildProxy::Setallcontribution(int32 allcontribution)
	{
		if (player_.expired())
			return ;
		if (player_.lock()->GetGuildID() == INVALID_GUID)
		{
			ZXERO_ASSERT(false) << " null guild " << player_.lock()->Guid();
			return;
		}
		guilddata_->set_allcontribution(allcontribution);
		Server::Instance().SendS2GCommonMessage("S2GSyncGuildUserAllcontribution", { allcontribution }, { (int64)player_.lock()->Guid(),(int64)player_.lock()->GetGuildID() }, {});
	}
	uint32 MtPlayerGuildProxy::Getjointime()
	{
		if (player_.expired())
			return 0;
		if (player_.lock()->GetGuildID() == INVALID_GUID)
		{
			ZXERO_ASSERT(false) << " null guild " << player_.lock()->Guid();
			return 0;
		}
		return guilddata_->jointime();
	}
	void MtPlayerGuildProxy::Setjointime(uint32 jointime)
	{
		if (player_.expired())
			return ;
		if (player_.lock()->GetGuildID() == INVALID_GUID)
		{
			ZXERO_ASSERT(false) << " null guild " << player_.lock()->Guid();
			return;
		}
		
		guilddata_->set_jointime(jointime);
	}
	int32 MtPlayerGuildProxy::GetBuildLevel(int32 buildtype)
	{
		if (player_.expired())
			return -1;
		if ( player_.lock()->GetGuildID() == INVALID_GUID)
		{
			ZXERO_ASSERT(false) << " null guild " << player_.lock()->Guid();
			return -1;
		}
		switch (buildtype)
		{
		case Packet::GuildBuildType::GUILD_MAIN:
			return guilddata_->guildlevel();
		case Packet::GuildBuildType::GUILD_CASHBOX:
			return guilddata_->cashboxlevel();
		case Packet::GuildBuildType::GUILD_SHOP:
			return guilddata_->shoplevel();
		case Packet::GuildBuildType::GUILD_STORAGE:
			return guilddata_->storagelevel();
		default:
			break;
		}
		return -1;
	}
	void MtPlayerGuildProxy::SetBuildLevel(int32 buildtype, int32 buildlevel)
	{
		if (player_.expired())
			return ;
		if (player_.lock()->GetGuildID() == INVALID_GUID)
		{
			ZXERO_ASSERT(false) << " null guild " << player_.lock()->Guid();
			return;
		}
		switch (buildtype)
		{
		case Packet::GuildBuildType::GUILD_MAIN:
		{
			guilddata_->set_guildlevel(buildlevel);
		}
		break;
		case Packet::GuildBuildType::GUILD_CASHBOX:
		{
			guilddata_->set_cashboxlevel(buildlevel);
		}
		break;
		case Packet::GuildBuildType::GUILD_SHOP:
		{
			guilddata_->set_shoplevel(buildlevel);
		}
		break;
		case Packet::GuildBuildType::GUILD_STORAGE:
		{
			guilddata_->set_storagelevel(buildlevel);
		}
		break;
		default:
			break;
		}
	}

	void MtPlayerGuildProxy::ResetWetcopy()
	{
		wetcopy_.Reset();
	}

	void MtPlayerGuildProxy::SetWetcopyCopyId(int32 copyid)
	{
		wetcopy_.SetWetcopyCopyId(copyid);
	}
	int32 MtPlayerGuildProxy::GetWetcopyCopyId()
	{
		return wetcopy_.GetWetcopyCopyId();
	}
	void MtPlayerGuildProxy::SetWetcopyStageId(int32 stageid)
	{
		wetcopy_.SetWetcopyStageId(stageid);
	}
	int32 MtPlayerGuildProxy::GetWetcopyStageId()
	{
		return wetcopy_.GetWetcopyStageId();
	}
	int32 MtPlayerGuildProxy::GetWetcopyMonsterHp(int32 index)
	{
		return wetcopy_.GetMonsterHp(index);
	}
	void MtPlayerGuildProxy::SetWetcopyMonsterHp(int32 index, int32 hp)
	{
		wetcopy_.SetMonsterHp(index, hp);
	}
}