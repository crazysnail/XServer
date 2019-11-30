#include "mt_player_offline_ai.h"
#include "mt_player.h"
#include "../mt_battle/mt_battle_ground.h"
#include "../mt_stage/mt_scene_stage.h"
#include "../mt_monster/mt_monster_manager.h"
#include "../mt_item/mt_item_package.h"
#include "../mt_scene/mt_scene.h"
#include "../mt_stage/mt_player_stage.h"
#include "../mt_actor/mt_actor.h"
#include "../mt_team/mt_team_manager.h"
#include <PlayerBasicInfo.pb.h>
#include <SceneStageConfig.pb.h>
#include <S2GMessage.pb.h>

namespace Mt
{
	class QuitGameAi : public PlayerAi
	{
	public:
		QuitGameAi(const boost::shared_ptr<MtPlayer>& owner) : PlayerAi(owner)
		{
		}
		virtual void OnTick(uint64 /*elapseTime*/) override
		{
			if (owner_.expired()) return;
			auto player = owner_.lock();
			if (offline_time_ > 0 
				&& offline_time_ + minutes(10).total_milliseconds() <= MtTimerManager::Instance().CurrentTime()
				&& player->GetPlayerStageProxy()->FirstNeedCalcId() == 0
				&& player->LogoutStatus() != eLogoutStatus::eCanLogout ) {
				auto offline_data = player->GetPlayerStageDB()->mutable_configs();
				for (auto& config : (*offline_data)) {
					LOG_INFO << "[quit_game_ai] player:" << player->Guid()
						<< ",hook. stage:" << config.stage_id()
						<< ",done:" << config.calc_done()
						<< ",total_time:" << config.total_time()
						<< ",hook_time:" << config.hook_time()
						<< ",wave_mill_sec:" << config.wave_mill_sec()
						<< ",wav_exp:" << config.wave_exp()
						<< ",wav_gold:" << config.wave_gold()
						<< ",dead_mill_sec" << config.dead_mill_sec();
				}
				player->LogoutStatus(eLogoutStatus::eCanLogout);
				LOG_INFO << "[quit_game_ai] player:" << player->Guid() << " leave game";
			}
		}
		virtual void OnPlayerOnLine() override
		{
			offline_time_ = 0;
		}
		virtual void OnPlayerOffLine() override
		{
			offline_time_ = MtTimerManager::Instance().CurrentTime();
		}

	private:
		uint64  offline_time_ = 0;
	};
	class AutoHookAi : public PlayerAi
	{
	public:
		enum class HookStatus;
		const uint64 BATTLE_INTERVAL = seconds(15).total_milliseconds(); //战斗间隔
		AutoHookAi(const boost::shared_ptr<MtPlayer>& owner) :PlayerAi(owner) {}
		virtual void OnPlayerOnLine() override
		{
		}
		virtual void OnTick(uint64 elapseTime) override
		{
			if (owner_.expired()) return;
			auto player = owner_.lock();
			if (player->CanAutoHook() == false || player->GetPlayerStageProxy() == nullptr)
				return;
			auto first_stage = player->GetPlayerStageProxy()->FirstNeedCalcId();
			auto hook_battle = player->HookBattle();
			if (first_stage > 0) {//还需要继续模拟
				give_item_timer_ = 0;
				if (hook_battle == nullptr || (hook_battle->GetClientInsight() == false
					&& hook_battle->Brief().stage_id() != first_stage)) {
					//没有战斗, 或有模拟战斗, 但是关卡id不对, 重新启动
					LOG_INFO << "[player_stage] player:" << player->Guid()
						<< ", start new hook battle:" << first_stage;
					player->GetPlayerStageDB()->set_current_stage(first_stage);
					for (auto& config : (*player->GetPlayerStageDB()->mutable_configs())) {
						if (config.stage_id() == first_stage) {
							config.set_hook_time(0);
						}
					}
					player->GetPlayerStageProxy()->ResetStageTime(first_stage);
					player->GetPlayerStageProxy()->ClearEfficentLog();
					player->StartHookBattle(first_stage, 1, 0, 0);
					return;
				} else {
					//其余为玩家手动挂机, 不用处理
				}
			} else {//模拟已完成
				if (hook_battle != nullptr) {//战斗查看中,不管
					return;
				}
				give_item_timer_ += elapseTime;
				if (player->GetPlayerStageProxy()->GiveRealTimeHookItem(give_item_timer_)){
					give_item_timer_ = 0;
				}
			}
		}

		virtual void OnPlayerScoreChanged() override
		{
			if (owner_.expired()) return;
			auto player = owner_.lock();
			auto offline_data = player->GetPlayerStageDB()->mutable_configs();
			auto new_score = player->BattleScore();
			auto need_recalc = false;
			for (auto& config : (*offline_data)) {
				if (config.stage_id() > 0 && config.calc_done() && (config.relate_score() < new_score * 0.95
					|| new_score <= config.relate_score() * 0.95)) {
					need_recalc = true;
					break;
				}
			}
			if (need_recalc == false) return;
			LOG_INFO << "[realtime_hook] player:" << player->Guid() << " OnPlayerScoreChanged Need Recalc";
			player->GetPlayerStageProxy()->RecalcRealTimeHookData();
			Clear();
		}

/*
		void HookEfficentChangedNotify(const Packet::RTHC& offline_hook_data)
		{
			if (owner_.expired()) return;
			auto player = owner_.lock();
			auto exp_efficent = offline_hook_data.main_actor_exp_freq();
			player->SendLuaPacket("HookDataEfficentUpdate", { int32(exp_efficent * 60), offline_hook_data.stage_id() }, {}, {});
		}*/

// 		void UpdateOfflineHookData(Packet::RTHC& realtime_hook_data,
// 			uint64 simulate_time, bool finish)
// 		{
// 			if (owner_.expired()) return;
// 			auto player = owner_.lock();
// 			LOG_INFO << "[realtime_hook] UpdateOfflineHookData player:" << player->Guid()
// 				<< " stage_id" << realtime_hook_data.stage_id()
// 				<< "simulate_time:" << simulate_time
// 				<< " finish:" << finish;
// 			realtime_hook_data.set_relate_score(player->BattleScore());
// 			realtime_hook_data.set_calc_done(finish);
// 			realtime_hook_data.set_hook_time(int32(simulate_time / 1000));
// 			auto battle_times = battle_time_costs_.size();
// 			float total_battle_seconds = simulate_time / 1000.f;
// 			if (battle_times <= 0) return;
// 
// 			float main_actor_exp_freq = statistics_->player_exp_ / total_battle_seconds;
// 			float hero_actor_exp_freq = statistics_->actor_exp_ / total_battle_seconds;
// 			float kill_monster_freq = statistics_->kill_monsters_ / total_battle_seconds;
// 			float battle_time_freq = battle_times / total_battle_seconds;
// 			float dead_freq = statistics_->self_dead_times_ / total_battle_seconds;
// 			float gold_freq = 0;
// 			auto gold_it = std::find_if(statistics_->items_.begin(), 
// 				statistics_->items_.end(), 
// 				[](auto& pair) {return pair.first == Packet::TokenType::Token_Gold; });
// 			if (gold_it != statistics_->items_.end() && gold_it->second > 0) {
// 				gold_freq = gold_it->second / total_battle_seconds;
// 			}
// 			realtime_hook_data.set_main_actor_exp_freq(main_actor_exp_freq);
// 			realtime_hook_data.set_hero_actor_exp_freq(hero_actor_exp_freq);
// 			realtime_hook_data.set_kill_monster_freq(kill_monster_freq);
// 			realtime_hook_data.set_battle_time_freq(battle_time_freq);
// 			realtime_hook_data.set_dead_freq(dead_freq);
// 			realtime_hook_data.set_gold_freq(gold_freq);
// 			HookEfficentChangedNotify(realtime_hook_data);
// 		}

		virtual void OnPlayerOffLine() override
		{
			if (owner_.expired()) return;
			auto player = owner_.lock();
			if (player->HookBattle() != nullptr) {
				player->HookBattle()->SetClientInsight(false);
			}
		}
		void Clear()
		{
			status_ = HookStatus::NEED_INIT;
			battle_time_costs_.clear();
			battle_wait_ = 0;
			stage_id_ = 0;
			hook_stage_step_ = -1;
			statistics_.reset();
			total_battle_count_ = 0;
			total_exp_ = 0;
			total_gold_ = 0;
			total_kill_monster_ = 0;
			total_dead_count = 0;
			
		}

		enum class HookStatus
		{
			NEED_INIT = 0, //需要初始战斗
			WAIT_LEAVE_TEAM, //等待离开队伍
			WAIT_BATTLE_FINISH, //等待之前的战斗退出
			WAIT_OFFLINE_INTERVAL, //掉线等待
			READY_TO_BATTLE, //可以开始挂机了
			WAIT_BATTLE_CREATE, //等外战斗创建完成
			WAITING_REACH_POS, //等待跑位
			BATTLE_STARTED, //战斗进行中
			STAGE_CALC_DONE, //计算收益完成
			ALL_CALC_DONE, //全部关卡计算完毕
			GIVE_EXP_AND_ITEM, //间歇性给东西
			UNKNOWN_ERROR,
		};
	private:
		uint64 battle_wait_ = 0;
		int64 add_exp_timer_ = 0;
		int64 kill_monster_timer_ = 0;
		uint64 total_hook_time_ = 0;
		int32 battle_stage_id_ = 0;
		int32 stage_id_ = 0;
		HookStatus status_ = HookStatus::NEED_INIT;
		int32 hook_stage_step_ = -1; //最多三个关卡
		std::vector<int32> battle_time_costs_;
		boost::shared_ptr<Statistics> statistics_;
		int32 total_battle_count_ = 0;
		int32 total_exp_ = 0;
		int32 total_gold_ = 0;
		int32 total_kill_monster_ = 0;
		int32 total_dead_count = 0;
		int64 give_item_timer_ = 0;
	};
	std::shared_ptr<PlayerAi> CreateOffLineAi(PlayerAiType type, const boost::shared_ptr<MtPlayer>& owner)
	{
		switch (type)
		{
		case Mt::PlayerAiType::AutoHook:
			return std::make_shared<AutoHookAi>(owner);
			break;
		case Mt::PlayerAiType::QuitTeam:
			return std::make_shared<QuitGameAi>(owner);
			break;
		default:
			ZXERO_ASSERT(false) << "invalid ai type";
			break;
		}
		return nullptr;
	}

}