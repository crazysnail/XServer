#ifndef MTONLINE_GAMESERVER_MT_ACTIVITY_H__
#define MTONLINE_GAMESERVER_MT_ACTIVITY_H__
#include "../mt_types.h"
#include "date_time.h"

namespace Mt
{
	class DateStruct
	{
	public:
		static void lua_reg(lua_State* ls);

		DateStruct();
		~DateStruct() {};
		void Init();
	public:
		uint32 cur_week;
		uint32 cur_hour;
		uint32 cur_day;
		uint32 week_day;
		uint32 cur_min;
		uint32 cur_date;
		uint32 past_day;
	};


	class MtActivityManager : public boost::noncopyable
	{
	public:
		static void lua_reg(lua_State* ls);
	public:
		static MtActivityManager& Instance();
		MtActivityManager();
		virtual ~MtActivityManager();
		int32 OnLoad();
		bool LoadPlayerActivity(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<zxero::dbcontext>& dbctx);
		
		Config::ActivityConfig* FindActivity(const int32 type);
		std::vector<int32> GenTextAnswers(const int32 count);
		std::vector<int32> GenPicAnswers(const int32 count);
		const Config::TextAnswerConfig* FindAnswerConfig(const int32 id);
		std::vector<Config::PicAnswerConfig*> FindPicAnswerConfig(const int32 group);
		const Config::PicAnswerConfig* FindPicAnswerConfigByIndex(const int32 index);
		Config::ActivityTimes * GetActivityTimes(const int32 type);
				
		void OnWeekTriger(int32 week);
		void OnHourTriger(int32 hour);
		void OnDayTriger(int32 day);
		void OnMinTriger(int32 min);

		void LoadActivityTB(bool reload = false);
		void LoadAnswerTB(bool reload = false);
				
		bool CheckDate(const Config::ActivityConfig* config);
		bool CheckTime(const Config::ActivityConfig* config);
		bool CheckPreLoad(const Config::ActivityConfig* config);
		bool CheckLevel(const Config::ActivityConfig* config, const int32 player_level);
		int32 IsActive(const Config::ActivityConfig* config, const int32 player_level);
		const DateStruct* GetDateStruct() { return &ds_; };

		void ClearNotifyFlag() { activity_notify_flag_.clear(); }
	private:
		std::map<int32, Config::ActivityConfig*> activity_configs_;
		std::map<int32, std::vector<boost::shared_ptr<Config::ActivityTimes>>> activity_times_;
		std::map<int32, Config::TextAnswerConfig*> text_answers_;
		int32 text_answers_weight_{ 0 };
		std::map<int32, std::vector<Config::PicAnswerConfig*>> pic_answers_;
		int32 pic_answers_weight_{ 0 };
		std::set<int32> activity_notify_flag_;

		DateStruct ds_;
		
	};

	class MtActivity : public boost::noncopyable, public boost::enable_shared_from_this<MtActivity>
	{
	public:
		static void lua_reg(lua_State* ls);
	public:
		MtActivity( MtPlayer& player,
			const boost::shared_ptr<Packet::Activity>& activity);

		MtActivity() {
		};

		const boost::shared_ptr<Packet::Activity>& ActivityDB() const {
			return activity_;
		}
		int32 GetType();

		void FillMessage(Packet::ActivityUpdateReply& message);
		void Reset();
		void OnRefresh2Client();
		void OnRefresh();

	private:
		const boost::weak_ptr<MtPlayer> player_;
		mutable boost::shared_ptr<Packet::Activity> activity_;
	};

}
#endif // MTONLINE_GAMESERVER_MT_ACTIVITY_H__
