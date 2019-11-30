#ifndef MTONLINE_GAMESERVER_MT_TRIAL_FIELD_H__
#define MTONLINE_GAMESERVER_MT_TRIAL_FIELD_H__
#include "../mt_types.h"

namespace Mt
{
	class TrialFieldManager : public boost::noncopyable
	{
	public:
		static TrialFieldManager& Instance();
		TrialFieldManager();

		/**
		* \brief 进入场景时刷新可调整的目标,由于是异步加载, 需要传递一个call_back
		* \param call_back 加载完毕后回调用该回调
		*/
		bool OnRefreshTarget(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<S2G::RefreshTarget>& message, int32 /*source*/);

		void RefreshTarget(const boost::shared_ptr<MtPlayer>& player, int32 wave);
		void OnDataLoad(int32 wave,
			boost::shared_ptr<DB::PlayerTrialInfo>& trial_player,
			const boost::shared_ptr<MtPlayer>& player);
		
		boost::shared_ptr<DB::PlayerTrialInfo> PlayerTrialInfo(const boost::shared_ptr<MtPlayer>& player);

		int32 OnLoad();
		void OnSaveAll(const boost::shared_ptr<dbcontext>& dbctx);
		void OnCacheAll();
		struct TrialData
		{
			enum class LoadStatus
			{
				LOGING = 1,
				DONE = 2,
			};
			/*TrialData(LoadStatus s, int32 wave, const boost::shared_ptr<DB::PlayerTrialInfo>& info)
				:status_(s), wave_(wave), info_(info) {}*/

			LoadStatus status_;
			int32 wave_;
			boost::shared_ptr<DB::PlayerTrialInfo> info_;
		};
	private:
		//boost::shared_ptr<DB::PlayerTrialInfo> LoadDataFromMemory(const boost::shared_ptr<MtPlayer>& player);
		std::map<zxero::uint64, TrialData> trial_info_map_;
	};
}
#endif // MTONLINE_GAMESERVER_MT_TRIAL_FIELD_H__
