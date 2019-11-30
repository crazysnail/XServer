#ifndef ZXERO_GAMESERVER_MT_SCENE_RELATE_H__
#define ZXERO_GAMESERVER_MT_SCENE_RELATE_H__

#include "../mt_types.h"
#include <boost/ptr_container/ptr_vector.hpp>
#include "AllPacketEnum.pb.h"
#include "AllConfigEnum.pb.h"
namespace Mt
{
		struct SceneBattleGroupKey
		{
			SceneBattleGroupKey(zxero::int32 scene, zxero::int32 battle_group)
				:scene_(scene), battle_group_(battle_group) {}
			bool operator<(const SceneBattleGroupKey& rhs) const {
				return (scene_ < rhs.scene_) || (scene_ == rhs.scene_ && battle_group_ < rhs.battle_group_);
			}
			zxero::int32 scene_ = 0;
			zxero::int32 battle_group_ = 0;
		};
		class MtScene;
		class MtSceneManager : public boost::noncopyable {
		public:
			static void lua_reg(lua_State* ls);
		public:
			static MtSceneManager& Instance();
			MtSceneManager();
			~MtSceneManager();
			zxero::int32	OnLoad();
			zxero::int32	OnPostLoad();
			void UpdateSceneTick(int32 id, int32 tick);
			std::map<int32, int32> GetSceneTick();
			boost::shared_ptr<MtScene> GetSceneById(zxero::int32 scene_id);
			boost::shared_ptr<MtScene> DefaultScene(Packet::Camp camp);
			/**
			* \brief 关服时调用, 停止掉所有event_loop
			*/
			void OnShutDown();

			const boost::shared_ptr<Config::Scene> GetSceneConfig(int32 id);
			/**
			* \brief 给所有场景派发一个消息 消息一般用来被执行
			*/
			void DispatchMessage(const boost::shared_ptr<google::protobuf::Message>& msg);
			/**
			* \brief  给所有场景派发一个消息, 场景给所有玩家广播这个消息
			*/
			void BroadCastMessage(const boost::shared_ptr<google::protobuf::Message>& msg);

			/**
			* \brief  给所有场景派发一个消息, 场景执行这个消息
			*/
			void BroadcastSceneExcuteMessage(const boost::shared_ptr<google::protobuf::Message>& msg);

			/**
			* \brief  给所有场景派发一个消息, 场景给所有玩家广播并执行这个消息
			*/
			void BroadcastPlayerExcuteMessage(const boost::shared_ptr<google::protobuf::Message>& msg);

			const boost::shared_ptr<Config::SceneBattleGroup> GetSceneBattleGroupById(int32 id, int32 index);
			const boost::shared_ptr<Config::SceneBattleGroup> GetSceneBattleGroup(const SceneBattleGroupKey& key);
			zxero::int32 SceneBattleGroupCount(zxero::int32 scene_id);
		private:
			void load_scene_battle_group();
			std::map<int32, boost::shared_ptr<MtScene>> scenes_;
			std::map<int32, int32> scene_tick_;
			std::vector<boost::shared_ptr<Config::Scene>> scene_configs_;
			std::map<SceneBattleGroupKey, boost::shared_ptr<Config::SceneBattleGroup>> battle_group_map_;
			std::vector<boost::shared_ptr<network::event_loop_thread>> scene_threads_;
		};

}

#endif // ZXERO_GAMESERVER_MT_SCENE_RELATE_H__