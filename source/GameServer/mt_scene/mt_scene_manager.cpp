#include "module.h"
#include <thread>
#include "event_loop.h"
#include "event_loop_thread.h"
#include "mt_scene_manager.h"
#include "../mt_config/mt_config.h"
#include "../data/data_table.h"
#include "../data/data_manager.h"
#include "../mt_server/mt_server.h"
#include "SceneConfig.pb.h"
#include <G2SMessage.pb.h>
#include <BaseConfig.pb.h>
#include "mt_scene.h"
#include "mt_copy_scene.h"
#include "random_generator.h"
#include <boost/format.hpp>

namespace Mt
{
	using namespace zxero::network;
	static MtSceneManager* __mt_scene_manager = nullptr;
	REGISTER_MODULE(MtSceneManager)
	{
		require("data_manager");

		register_load_function(STAGE_LOAD, boost::bind(&MtSceneManager::OnLoad, boost::ref(MtSceneManager::Instance())));
		register_load_function(STAGE_POST_LOAD, boost::bind(&MtSceneManager::OnPostLoad, boost::ref(MtSceneManager::Instance())));

	}


	MtSceneManager::MtSceneManager()
	{
		__mt_scene_manager = this;
	}

	MtSceneManager::~MtSceneManager()
	{

	}
	void MtSceneManager::lua_reg(lua_State* ls)
	{
		ff::fflua_register_t<MtSceneManager, ctor()>(ls, "MtSceneManager")
			.def(&MtSceneManager::GetSceneById, "GetSceneById")
			.def(&MtSceneManager::GetSceneBattleGroupById,"GetSceneBattleGroupById")
			;
		ff::fflua_register_t<>(ls)
			.def(&MtSceneManager::Instance, "LuaSceneManager");
	}

	void MtSceneManager::OnShutDown()
	{
		LOG_WARN << "[MtSceneManager] OnShudown, stop all scene threads";
		scene_threads_.clear();
		std::for_each(scenes_.begin(), scenes_.end(), 
			[](auto& p) {p.second->OnShutDown(); });
	}

	zxero::int32 MtSceneManager::OnPostLoad()
	{
		auto table = data_manager::instance()->get_table("scene");
		event_loop* loop(nullptr);
		if (table != nullptr) {
			auto scene_per_thread = table->get_rows() / std::thread::hardware_concurrency();
			for (auto i = 0; i < table->get_rows(); ++i) {
				auto row = table->find_row(i);
				boost::shared_ptr<Config::Scene> scene_config = boost::make_shared<Config::Scene>();
				FillMessageByRow(*scene_config, *row);
				scene_configs_.push_back(scene_config);
				if (i  % scene_per_thread == 0) {
					auto loop_thread = boost::make_shared<event_loop_thread>([&](auto /*loop*/) { 
						rand_gen.reset(new MT19937());
						thread_lua.reset(new ff::fflua_t());
					});
					loop = loop_thread->start_loop();
					scene_threads_.push_back(loop_thread);
				}
				scene_tick_[scene_config->id()] = 0;
				if (MtScene::IsRaidScene(scene_config->id())) {
					LOG_INFO << "create copyscene -> " << "scene id:" << scene_config->id() << ",loop:" << loop;
					scenes_[scene_config->id()] = boost::make_shared<MtCopyScene>(scene_config, loop);
				}
				else {
					LOG_INFO << "create scene -> " << "scene id:" << scene_config->id() << ",loop:" << loop;
					scenes_[scene_config->id()] = boost::make_shared<MtScene>(scene_config, loop);
				}

			}
		}
		load_scene_battle_group();

		return 0;
	}

	void MtSceneManager::load_scene_battle_group() {
		auto table = data_manager::instance()->get_table("scenebattlegroup");
		ZXERO_ASSERT(table) << "can not find table";
		for (auto i = 0; i < table->get_rows(); ++i) {
			auto row = table->find_row(i);
			auto scene_config_it = std::find_if(std::begin(scene_configs_), std::end(scene_configs_), [&](auto& it) {
				return it->id() == row->getInt("id");
			});
			ZXERO_ASSERT(scene_config_it != std::end(scene_configs_));
			Config::SceneBattleGroup* battle_group = (*scene_config_it)->add_battle_groups();
			FillMessageByRow(*battle_group, *row);
			auto inner_battle_group = boost::make_shared<Config::SceneBattleGroup>(*battle_group);
			battle_group_map_.insert({ SceneBattleGroupKey((*scene_config_it)->id(), battle_group->index()), inner_battle_group });
		}
	}

	zxero::int32 MtSceneManager::OnLoad()
	{
		return 0;
	}


	boost::shared_ptr<MtScene> MtSceneManager::GetSceneById(zxero::int32 scene_id)
	{
		if (scenes_.find(scene_id) != scenes_.end()) {
			return scenes_[scene_id];
		}
		else {
			return nullptr;
		}
		
	}

	boost::shared_ptr<MtScene> MtSceneManager::DefaultScene(Packet::Camp camp)
	{
		ZXERO_ASSERT(scenes_.size() > 0) << "empty Scene Manager";
		int32 default_scene_id = 1;
		auto& data = MtConfigDataManager::Instance().FindConfigValue("default_scene_id");
		if (data != nullptr){
			if (camp == Packet::Camp::Alliance) {
				default_scene_id = data->value2();
			}
			else if (camp == Packet::Camp::Horde) {
				default_scene_id = data->value1();
			}
		}

		auto main_city_it = scenes_.find(default_scene_id); 
		if (main_city_it != std::end(scenes_)) {
			return main_city_it->second;
		}
		else {
			ZXERO_ASSERT(false)<<"no default scene";
			return nullptr;
		}
	}

	MtSceneManager& MtSceneManager::Instance()
	{
		return *__mt_scene_manager;
	}

	zxero::int32 MtSceneManager::SceneBattleGroupCount(zxero::int32 scene_id)
	{
		return std::count_if(std::begin(battle_group_map_), std::end(battle_group_map_), [=](auto pair) {return pair.first.scene_ == scene_id; });
	}
	const boost::shared_ptr<Config::Scene> MtSceneManager::GetSceneConfig(int32 id)
	{
		auto scene_config_it = std::find_if(std::begin(scene_configs_), std::end(scene_configs_), [&](auto& it) {
			return it->id() == id;
		});
		
		if (scene_config_it != std::end(scene_configs_))
		{
			return *scene_config_it;
		}
		ZXERO_ASSERT(false) << "no scene config : "<< id;
		return nullptr;
	}
	const boost::shared_ptr<Config::SceneBattleGroup> MtSceneManager::GetSceneBattleGroupById(int32 id, int32 index)
	{
		return GetSceneBattleGroup({ id ,index });
	}
	const boost::shared_ptr<Config::SceneBattleGroup> MtSceneManager::GetSceneBattleGroup(const SceneBattleGroupKey& key)
	{
		if (battle_group_map_.find(key) == std::end(battle_group_map_))
			return nullptr;
		return battle_group_map_[key];
	}

	void MtSceneManager::DispatchMessage(
		const boost::shared_ptr<google::protobuf::Message>& msg)
	{
		for (auto& it : scenes_) {
			it.second->DispatchMessage(msg, nullptr, MessageSource::GAME_EXECUTE);
		}
	}

	void MtSceneManager::BroadCastMessage(const boost::shared_ptr<google::protobuf::Message>& msg)
	{
		for (auto& it : scenes_) {
			it.second->DispatchMessage(msg, nullptr, MessageSource::GAME_BROADCAST);
		}
	}

	void MtSceneManager::BroadcastSceneExcuteMessage(const boost::shared_ptr<google::protobuf::Message>& msg)
	{
		for (auto& it : scenes_) {
			it.second->ExecuteMessage(msg,nullptr);
		}
	}

	void MtSceneManager::BroadcastPlayerExcuteMessage(const boost::shared_ptr<google::protobuf::Message>& msg )
	{
		for (auto& it : scenes_) {
			it.second->BroadcastExcuteMessage(msg);
		}
	}

	void MtSceneManager::UpdateSceneTick(int32 id, int32 tick)
	{
		scene_tick_[id] = tick;
	}

	std::map<int32, int32> MtSceneManager::GetSceneTick()
	{
		return scene_tick_;
	}

}
