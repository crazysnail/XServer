#include "ActivityConfig.pb.h"
#include "ActivityConfig.proto.fflua.h"
#include "ActivityPacket.pb.h"
#include "mt_activity.h"
#include "module.h"
#include "../data/data_manager.h"
#include "../mt_chat/mt_chatmessage.h"
#include "../mt_player/mt_player.h"
#include "../base/mt_timer.h"
#include "../mt_config/mt_config.h"
#include "utils.h"

//必须在最后include！-------------------
#include "../zxero/mem_debugger.h"
//------------------------------------

namespace Mt
{

	DateStruct::DateStruct()
	{
		cur_week = 0;
		cur_hour = 0;
		cur_day = 0;
		week_day = 0;
		cur_min = 0;
		cur_date = 0;
		past_day = 0;
	}

	void DateStruct::Init()
	{
		cur_week = MtTimerManager::Instance().GetWeekNumber();
		cur_hour = MtTimerManager::Instance().GetHour();
		cur_day = MtTimerManager::Instance().GetDay();
		week_day = MtTimerManager::Instance().GetWeekDay();
		cur_min = MtTimerManager::Instance().GetMinute();
		cur_date = MtTimerManager::Instance().Date2Num();
		past_day = MtTimerManager::Instance().DiffDayToNowByNumerDate(MtConfig::Instance().open_server_date_);
	}

	void DateStruct::lua_reg(lua_State* state)
	{
		ff::fflua_register_t<DateStruct, ctor()>(state, "DateStruct")
			.def(&DateStruct::cur_week, "cur_week")
			.def(&DateStruct::cur_hour, "cur_hour")
			.def(&DateStruct::cur_day, "cur_day")
			.def(&DateStruct::week_day, "week_day")
			.def(&DateStruct::cur_min, "cur_min")
			.def(&DateStruct::cur_date, "cur_date")
			.def(&DateStruct::past_day, "past_day")
			;
	}

	static MtActivityManager* __mt_activity_mamager = nullptr;

	void MtActivityManager::lua_reg(lua_State* ls)
	{
		ff::fflua_register_t<MtActivityManager, ctor()>(ls, "MtActivityManager")
			.def(&MtActivityManager::FindActivity, "FindActivity")
			.def(&MtActivityManager::FindAnswerConfig,"FindAnswerConfig")
			.def(&MtActivityManager::FindPicAnswerConfig, "FindPicAnswerConfig")
			.def(&MtActivityManager::GetActivityTimes, "GetActivityTimes")
			.def(&MtActivityManager::FindPicAnswerConfigByIndex, "FindPicAnswerConfigByIndex")
			.def(&MtActivityManager::LoadActivityTB, "LoadActivityTB")
			.def(&MtActivityManager::LoadAnswerTB, "LoadAnswerTB")
			.def(&MtActivityManager::GetDateStruct, "GetDateStruct")
			.def(&MtActivityManager::CheckLevel,"CheckLevel")
			.def(&MtActivityManager::IsActive, "IsActive");
			

		ff::fflua_register_t<>(ls)
			.def(&MtActivityManager::Instance, "LuaActivityManager");
	}

	REGISTER_MODULE(MtActivityManager)
	{
		require("data_manager");
		require("MtTimerManager");
		register_load_function(module_base::STAGE_LOAD, boost::bind(&MtActivityManager::OnLoad, __mt_activity_mamager));
	}

	MtActivityManager::MtActivityManager()
	{
		__mt_activity_mamager = this;
	}

	MtActivityManager::~MtActivityManager()
	{
		for (auto child : activity_configs_) {
			SAFE_DELETE(child.second);
		}
		activity_configs_.clear();		
		activity_times_.clear();

		for (auto child : text_answers_) {
			SAFE_DELETE(child.second);
		}
		text_answers_.clear();

		for (auto child : pic_answers_) {
			for (auto i : child.second) {
				SAFE_DELETE(i);
			}
		}
		pic_answers_.clear();
	}

	MtActivityManager& MtActivityManager::Instance()
	{
		return *__mt_activity_mamager;
	}

	int32 MtActivityManager::OnLoad()
	{
		LoadActivityTB();
		LoadAnswerTB();

		ds_.Init();
		return 0;
	}

	void MtActivityManager::LoadActivityTB(bool reload)
	{
		for (auto child : activity_configs_) {
			SAFE_DELETE(child.second);
		}
		activity_configs_.clear();
		activity_times_.clear();

		data_table* table = nullptr;
		if (reload) {
			table = data_manager::instance()->on_reload_file("activity");
		}
		else {
			table = data_manager::instance()->get_table("activity");
		}
		for (auto i = 0; i < table->get_rows(); ++i) {
			auto config = SAFE_NEW Config::ActivityConfig();
			auto row = table->find_row(i);
			FillMessageByRow(*config, *row);
			if (config->switch_open() != 0) {
				activity_configs_.insert({ config->id(), config });
			} else {
				SAFE_DELETE(config);
				continue;
			}

			std::vector<int32> begin_times;
			utils::split_string(config->day_begin_time().c_str(), "|", begin_times);
			std::vector<int32> end_times;
			utils::split_string(config->day_end_time().c_str(), "|", end_times);

			if (begin_times.size() != end_times.size()) {
				ZXERO_ASSERT(false);
				return;
			}

			for (uint32 k = 0; k < begin_times.size(); k++) {
				auto time_config = boost::make_shared<Config::ActivityTimes>();
				time_config->set_begin_time(begin_times[k]);
				time_config->set_end_time(end_times[k]);

				if (activity_times_.find(config->id()) == activity_times_.end()) {
					std::vector<boost::shared_ptr<Config::ActivityTimes>> times;
					times.push_back(time_config);
					activity_times_.insert({ config->id(), times });
				}
				else {
					activity_times_[config->id()].push_back(time_config);
				}
			}
		}

	}

	void MtActivityManager::LoadAnswerTB(bool reload)
	{
		for (auto child : text_answers_) {
			SAFE_DELETE(child.second);
		}
		text_answers_.clear();
		data_table* table = nullptr;
		if (reload) {
			table = data_manager::instance()->on_reload_file("text_answer");
		}
		else {
			table = data_manager::instance()->get_table("text_answer");
		}
		text_answers_weight_ = 0;
		for (auto i = 0; i < table->get_rows(); ++i) {
			auto config = SAFE_NEW Config::TextAnswerConfig();
			auto row = table->find_row(i);
			FillMessageByRow(*config, *row);

			text_answers_weight_ += config->weight();
			text_answers_.insert({ config->index(),config });
		}


		/////////////////////////////////////////////////////////////////
		for (auto child : pic_answers_) {
			for (auto i : child.second) {
				SAFE_DELETE(i);
			}
		}
		pic_answers_.clear();
		if (reload) {
			table = data_manager::instance()->on_reload_file("pic_answer");
		}
		else {
			table = data_manager::instance()->get_table("pic_answer");
		}
		pic_answers_weight_ = 0;
		for (auto i = 0; i < table->get_rows(); ++i) {
			auto config = SAFE_NEW Config::PicAnswerConfig();
			auto row = table->find_row(i);
			FillMessageByRow(*config, *row);

			if (pic_answers_.find(config->group()) != pic_answers_.end()) {
				pic_answers_[config->group()].push_back(config);
			}
			else {
				pic_answers_weight_ += config->weight();
				std::vector<Config::PicAnswerConfig*> vecs;
				vecs.push_back(config);
				pic_answers_.insert({ config->group(),vecs });
			}
		}
		//

	}
	std::vector<int32> MtActivityManager::GenTextAnswers( const int32 count ) 
	{
		std::map<int32, int32> temp;
		std::vector<int32> outer;
		int32 all_weight = text_answers_weight_;
		for (auto child : text_answers_) {
			temp.insert({ child.first,child.second->weight() });
		}
		if ((int32)temp.size() < count) {
			ZXERO_ASSERT(false) << "text_ansewers count is not enough!,size=" << temp.size() << " count=" << count;
			return outer;
		}		
		
		for (int32 i = 0; i < count; i++) {
			int32 seed = (int32)rand_gen->intgen(1, all_weight);
			int32 basic_weight = 0;
			for (auto iter = temp.begin(); iter != temp.end(); ) {
				basic_weight += iter->second;
				if (seed <= basic_weight) {
					//命中
					all_weight -= iter->second;
					outer.push_back(iter->first);
					iter = temp.erase(iter);	//抽牌移除
					break;
				}
				else {
					++iter;
				}
			}
		}
		return outer;
	}

	std::vector<int32> MtActivityManager::GenPicAnswers(const int32 count)
	{
		std::map<int32, int32> temp;
		std::vector<int32> outer;
		int32 all_weight = pic_answers_weight_;
		for (auto child : pic_answers_) {
			temp.insert({ child.first,child.second[0]->weight() });
		}
		if ((int32)temp.size() < count) {
			ZXERO_ASSERT(false) << "pic_ansewers count is not enough!,size=" << temp.size() << " count=" << count;
			return outer;
		}

		for (int32 i = 0; i < count; i++) {
			int32 seed = (int32)rand_gen->intgen(1, all_weight);
			int32 basic_weight = 0;
			for (auto iter = temp.begin(); iter != temp.end(); ) {
				basic_weight += iter->second;
				if (seed <= basic_weight) {
					//命中
					all_weight -= iter->second;
					outer.push_back(iter->first);
					iter = temp.erase(iter);	//抽牌移除
					break;
				}
				else {
					++iter;
				}
			}
		}
		return outer;
	}

	const Config::TextAnswerConfig* MtActivityManager::FindAnswerConfig(const int32 id) {
		if (text_answers_.find(id) != text_answers_.end()) {
			return text_answers_[id];
		}
		return nullptr;
	}
	
	std::vector<Config::PicAnswerConfig*> MtActivityManager::FindPicAnswerConfig(const int32 group)
	{
		if (pic_answers_.find(group) != pic_answers_.end()) {
			return pic_answers_[group];
		}
		std::vector<Config::PicAnswerConfig*> temp;
		return temp;
	}

	const Config::PicAnswerConfig* MtActivityManager::FindPicAnswerConfigByIndex(const int32 index)
	{
		for (auto child : pic_answers_) {
			for (auto sub : child.second) {
				if (sub->index() == index) {
					return sub;
				}
			}
		}
		return nullptr;
	}

	Config::ActivityTimes * MtActivityManager::GetActivityTimes(const int32 type)
	{
		auto time_iter = activity_times_.find(type);
		if (time_iter == activity_times_.end())
			return nullptr;

		auto times = time_iter->second;
		return times[0].get();
	}
	
	bool MtActivityManager::CheckDate(const Config::ActivityConfig* config)
	{
		if (config == nullptr) {
			return false;
		}

		if (ds_.cur_date < (uint32)config->begin_date() || ds_.cur_date >(uint32)config->end_date()) {
			return false;
		}

		std::string during_week_flag = config->during_week_flag();//位表示第几周激活
		int32 week_index = ds_.cur_week % 8;
		if (during_week_flag[week_index] == '0') {//本周不激活
			return false;
		}
		else {//本周激活		
			std::string week_day_flag = config->week_day_flag();//位表示周几激活
			if (ds_.week_day < 0 || ds_.week_day >= 7) {
				return false;
			}
			if (week_day_flag[ds_.week_day] == '0') {//周几关闭
				return false;
			}
		}

		return true;
	}

	bool MtActivityManager::CheckTime(const Config::ActivityConfig* config)
	{
		if (config == nullptr) {
			return false;
		}
		
		auto time_iter = activity_times_.find(config->id());
		if (time_iter == activity_times_.end()){
			return false;
		}

			////当前小时是否激活
		int32 cur_time = ds_.cur_hour * 100 + ds_.cur_min;
		bool is_ok = false;
		for (auto& child : time_iter->second) {
			if (cur_time >= child->begin_time() && cur_time < child->end_time()) {
				is_ok = true;
				break;
			}
		}
		if (!is_ok) {
			return false;
		}
		
		return true;

	}
	bool MtActivityManager::CheckLevel(const Config::ActivityConfig* config, const int32 player_level)
	{
		if (config == nullptr ) {
			return false;
		}

		if (player_level < config->open_level()) {
			return false;
		}

		if (player_level >= config->close_level()) {
			return false;
		}

		return true;
	}


	int32  MtActivityManager::IsActive(const Config::ActivityConfig* config, const int32 player_level)
	{
		if (config == nullptr) {
			return Packet::ResultCode::InvalidConfigData;
		}

		if (!CheckDate(config)) {
			return Packet::ResultCode::Activity_NotStart;
		}

		if (!CheckTime(config)) {
			return Packet::ResultCode::Activity_NotStart;
		}
		if (player_level != -1) {
			if (!CheckLevel(config,player_level)) {
				return Packet::ResultCode::LevelLimit;
			}
		}
		return Packet::ResultCode::ResultOK;
	}

	bool MtActivityManager::CheckPreLoad(const Config::ActivityConfig* config)
	{
		if (config == nullptr) {
			return false;
		}

		if (!CheckDate(config)) {
			return false;
		}

		////当前小时是否激活
		int32 cur_time = ds_.cur_hour * 100 + ds_.cur_min;
		if (cur_time != -1 && cur_time != config->pre_notify()) {
			return false;
		}

		return true;
	}

	void MtActivityManager::OnWeekTriger(int32 week)
	{
		ds_.cur_week = week;
	}

	void MtActivityManager::OnDayTriger(int32 day)
	{
		ds_.cur_day = day;
		ds_.week_day = MtTimerManager::Instance().GetWeekDay();
		ds_.cur_date = MtTimerManager::Instance().Date2Num();
		ds_.past_day = MtTimerManager::Instance().DiffDayToNowByNumerDate(MtConfig::Instance().open_server_date_);
		ClearNotifyFlag();
	}

	void MtActivityManager::OnHourTriger(int32 hour)
	{
		ds_.cur_hour = hour;
	}

	void MtActivityManager::OnMinTriger(int32 min)
	{
		ds_.cur_min = min;

		for (auto child : activity_configs_) {
			if (activity_notify_flag_.find(child.first) != activity_notify_flag_.end()) {
				continue;
			}
			if (CheckPreLoad(child.second)) {
				//send_system_message_toworld(child.second->notify_text());
				send_run_massage(child.second->notify_text());
				activity_notify_flag_.insert(child.first);
			}
		}
	}

	bool MtActivityManager::LoadPlayerActivity(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<zxero::dbcontext>& dbctx)
	{
		dbctx;
		for (auto child : activity_configs_) {
			auto activity_db = boost::make_shared<Packet::Activity>();
			auto config = activity_db->mutable_config();
			config->CopyFrom(*(child.second));

			auto activity = boost::make_shared<MtActivity>(*player, activity_db);
			if (!player->AddActivity(activity)) {
				ZXERO_ASSERT(false) << "AddActivity failed! player guid=" << player->Guid() << " activity type=" << activity->GetType();
			}
		}
				
		return true;
	}


	//
	Config::ActivityConfig* MtActivityManager::FindActivity(const int32 type)
	{
		if (activity_configs_.find(type) != activity_configs_.end()) {
			return activity_configs_[type];
		}

		return nullptr;
	}

	//--------------------------------------------------
	//------------------------------------------------------
	void MtActivity::lua_reg(lua_State* ls)
	{
		ff::fflua_register_t<MtActivity, ctor()>(ls, "MtActivity")
			//.def(&MtActivity::Reset, "Reset");
			.def(&MtActivity::GetType, "GetType")
			.def(&MtActivity::ActivityDB, "ActivityDB")
			.def(&MtActivity::OnRefresh2Client,"OnRefresh2Client")
			;
	}

	void MtActivity::Reset()
	{
		activity_->set_times(0);
	}

	void MtActivity::FillMessage(Packet::ActivityUpdateReply& message)
	{
		auto  activity = message.mutable_activity();
		activity->CopyFrom(*activity_);
	}
	
	int32 MtActivity::GetType() {
		return activity_->config().id();
	}

	void MtActivity::OnRefresh2Client()
	{
		auto player = player_.lock();
		if (player == nullptr) {
			LOG_ERROR << "[activity] player is null";
			return;
		}
		int32 new_status = 1;
		if ( (Packet::ResultCode)MtActivityManager::Instance().IsActive(&(activity_->config()), player->PlayerLevel() ) != Packet::ResultCode::ResultOK) {
			new_status = 0;
		}

		activity_->set_is_active(new_status);
		int32 times = player->OnLuaFunCall_1(501, "GetActivityTimes", GetType());
		activity_->set_times(times < 0 ? 0 : times);

		Packet::ActivityUpdateReply msg;

		auto ac = msg.mutable_activity();
		ac->CopyFrom(*activity_);

		player->SendMessage(msg);
	}

	void MtActivity::OnRefresh()
	{
		auto player = player_.lock();
		if (player == nullptr) {
			LOG_ERROR << "[activity] player is null";
			return;
		}
		int32 old_status = activity_->is_active();
		int32 new_status = 1;

		if ((Packet::ResultCode)MtActivityManager::Instance().IsActive(&(activity_->config()), player->PlayerLevel()) != Packet::ResultCode::ResultOK) {
			new_status = 0;
			activity_->set_is_active(0);
		}
		else {
			activity_->set_is_active(1);
		}

		if (old_status != new_status) {
			OnRefresh2Client();

			if (old_status == 0) {
				//说明是刚刚开启
				player->OnLuaFunCall_1(501, "OnActivityOpen", GetType());
			}
			else {
				//说明刚刚结束
 				player->OnLuaFunCall_1(501, "OnActivityClose", GetType());
			}
		}
	}

	MtActivity::MtActivity(MtPlayer& player,
		const boost::shared_ptr<Packet::Activity>& activity) 
		:player_(player.weak_from_this())
		, activity_(activity)
	{

	}

}
