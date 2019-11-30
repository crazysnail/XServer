#ifndef MTONLINE_GAMESERVER_MT_SCENE_OBJECT_H__
#define MTONLINE_GAMESERVER_MT_SCENE_OBJECT_H__
#include "../mt_types.h"

namespace Mt
{
	class SceneObject : public boost::noncopyable, public boost::enable_shared_from_this<SceneObject>
	{
	public:
		virtual zxero::uint64 Guid() const = 0;
		virtual bool SendMessage(const google::protobuf::Message& message) const = 0;
		virtual void FillSceneObjectMessage(Packet::SceneObject& message) const = 0;
		virtual void OnTick(zxero::uint64 elapseTime) = 0;
		virtual const boost::shared_ptr<MtScene>& Scene() const {
			return scene_;
		}
		virtual void Scene(const boost::shared_ptr<MtScene>& scene) {
			scene_ = scene;
		}
	protected:
		boost::shared_ptr<MtScene> scene_;
	};

	class Npc : public SceneObject
	{
	public:
		explicit Npc(const boost::shared_ptr<MtScene>& scene,
			boost::shared_ptr<Config::MonsterConfig>& config_info)
			:Npc(scene, config_info, MtGuid::Instance()(*this)) {}

		virtual bool SendMessage(const google::protobuf::Message& /*message*/) const override{
			return true;
		}
		virtual void FillSceneObjectMessage(Packet::SceneObject& /*message*/) const override {

		}
		virtual zxero::uint64 Guid() const { return guid_; }
		virtual void OnTick(zxero::uint64 /*elapseTime*/) override {}
	protected:
		explicit Npc(const boost::shared_ptr<MtScene>& scene,
			boost::shared_ptr<Config::MonsterConfig>& config_info,
			const zxero::uint64 guid) :scene_(scene), config_info_(config_info), guid_(guid) {}
	private:
		const boost::shared_ptr<Config::MonsterConfig> config_info_;
		const zxero::uint64 guid_ = INVALID_GUID;
	};
}
#endif // MTONLINE_GAMESERVER_MT_SCENE_OBJECT_H__
