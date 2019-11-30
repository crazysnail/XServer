#ifndef MTPLAYER_GUILD_PROXY_H__
#define MTPLAYER_GUILD_PROXY_H__

#include "../mt_types.h"
namespace Mt
{
#define MaxWetCopyMonsterCount 10
	class FightWetCopyCache
	{
	public:
		FightWetCopyCache() {}
		~FightWetCopyCache() {}
		void Reset()
		{
			copyid = 0;
			stageid = 0;
			for (int32 i=0;i<MaxWetCopyMonsterCount;i++)
			{
				monsterhp[i] = 0;
			}
		}

		void SetWetcopyCopyId(int32 id)
		{
			copyid = id;
		}
		int32 GetWetcopyCopyId()
		{
			return copyid;
		}
		void SetWetcopyStageId(int32 id)
		{
			stageid = id;
		}
		int32 GetWetcopyStageId()
		{
			return stageid;
		}
		int32 GetMonsterHp(int32 index)
		{
			if (index >= 0 && index<MaxWetCopyMonsterCount)
			{
				return monsterhp[index];
			}
			return 0;
		}

		void SetMonsterHp(int32 index,int32 hp)
		{
			if (index >= 0 && index < MaxWetCopyMonsterCount)
			{
				monsterhp[index] = hp;
			}
		}
	private:
		int32 copyid;
		int32 stageid;
		int32 monsterhp[MaxWetCopyMonsterCount];
	};
	class MtPlayerGuildProxy {
	public:
		static void lua_reg(lua_State* ls);
	public:
		MtPlayerGuildProxy() {};
		MtPlayerGuildProxy(boost::weak_ptr<MtPlayer> p);
		~MtPlayerGuildProxy();
	public:
		void G2SSyncPlayerGuildData(const boost::shared_ptr<G2S::SyncPlayerGuildData>& message);

		int32 GetGuildPosition();
		void SetGuildPosition(int32 position);
		int32 Getbdonation();
		void Setbdonation(int32 bdonation);
		int32 Getbbonus();
		void Setbbonus(int32 bbonus);
		int32 Getcurcontribution();
		void Setcurcontribution(int32 curcontribution);
		int32 Getweekcontribution();
		void Setweekcontribution(int32 weekcontribution);
		int32 Getallcontribution();
		void Setallcontribution(int32 allcontribution);
		uint32 Getjointime();
		void Setjointime(uint32 jointime);
		int32 GetBuildLevel(int32 buildtype);
		void SetBuildLevel(int32 buildtype, int32 buildlevel);

		void ResetWetcopy();
		void SetWetcopyCopyId(int32 copyid);
		int32 GetWetcopyCopyId();
		void SetWetcopyStageId(int32 stageid);
		int32 GetWetcopyStageId();
		int32 GetWetcopyMonsterHp(int32 index);
		void SetWetcopyMonsterHp(int32 index, int32 hp);
	private:
		boost::weak_ptr<MtPlayer> player_;
		boost::shared_ptr<Packet::PlayerGuildUserData> guilddata_;
		FightWetCopyCache wetcopy_;
	};
}
#endif