#ifndef __FIGHTVALUE_RANKLIST_H__
#define __FIGHTVALUE_RANKLIST_H__
#include "RankList.pb.h"
#include "ranklist.h"

namespace Mt
{
	struct TopData
	{
		TopData()
		{
			data.set_playerguid(INVALID_GUID);
			data.set_playername("");
			data.set_init_actor_id(1);
			data.set_level(1);
		}

		TopData(const TopData & a)
		{
			data = a.data;
		}

		bool operator>(const TopData &a) const
		{
			for (int32 i=0;i<data.value_size() && i<a.data.value_size(); i++)
			{
				if (data.value(i) > a.data.value(i))
				{
					return true;
				}
				else if (data.value(i) < a.data.value(i))
				{
					return false;
				}
			}
			return false;
		}

		bool operator<(const TopData &a) const
		{
			for (int32 i = 0; i < data.value_size() && i < a.data.value_size(); i++)
			{
				if (data.value(i) < a.data.value(i))
				{
					return true;
				}
				else if (data.value(i) > a.data.value(i))
				{
					return false;
				}
			}
			return false;
		}

		bool operator!=(const TopData &a)
		{
			for (int32 i = 0; i < data.value_size() && i < a.data.value_size(); i++)
			{
				if (data.value(i) != a.data.value(i))
				{
					return true;
				}
			}
			return false;
		}

		bool operator==(const TopData &a)
		{
			return (data.playerguid() == a.data.playerguid());
		}

		TopData& operator=(const TopData &a)
		{
			data = a.data;
			return *this;
		}
		
		Packet::TopRankData data;
	};

	class MtTopRankManager : public boost::noncopyable
	{
	public:
		MtTopRankManager() ;
		~MtTopRankManager() {};
		static MtTopRankManager& Instance();
		zxero::int32 OnLoad();
		bool OnLoadAll(boost::shared_ptr<zxero::dbcontext>& dbctx);
		bool OnLoadArena(boost::shared_ptr<zxero::dbcontext>& dbctx, std::set<uint64>& loadplayersceneinfo);
		void OnPlayerScoreChanged(const boost::shared_ptr<MtPlayer>& player);
		void OnPlayerLevelChanged(const boost::shared_ptr<MtPlayer> player);
		void OnMainActorScoreChanged(const boost::shared_ptr<MtPlayer>& player);
		void OnPlayerArenaChanged(const boost::shared_ptr<MtPlayer>& challenger,
			int32 userrank, int32 user_score, int32 targetrank, 
			int32 target_score, uint64 targetguid,
			std::string targetname,int32 targetactorid,int32 targetlevel);
		void OnPlayerTowerChanged(const boost::shared_ptr<MtPlayer>& player,int32 towerlevel,int32 towertime);
		void OnWeekCleanPlayerTower();
		bool OnTopRankDataReq(const boost::shared_ptr<client_session>& session,
			const boost::shared_ptr<Packet::TopRankDataReq>& message,
			int32 /*source*/);
	private:
		
		boost::shared_mutex professions_shared_mutex_;
		ranklist_top<TopData, 50> fightvaluelist_[Packet::Professions_ARRAYSIZE];
		boost::shared_mutex level_shared_mutex_;
		ranklist_top<TopData, 50> playerlevellist_;
		boost::shared_mutex fightvalue_shared_mutex_;
		ranklist_top<TopData, 50> playerfightvaluelist_;
		boost::shared_mutex arena_shared_mutex_;
		ranklist_top<TopData, 50> arenalist_;
		boost::shared_mutex tower_shared_mutex_;
		ranklist_top<TopData, 50> towerlist_;
	};
}


#endif