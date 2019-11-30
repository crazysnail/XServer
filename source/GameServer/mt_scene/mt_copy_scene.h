#ifndef MTONLINE_GAMESERVER_MT_COPY_SCENE_H__
#define MTONLINE_GAMESERVER_MT_COPY_SCENE_H__

#include "mt_scene.h"

namespace Mt
{
	class MtCopyScene : public MtScene
	{
	public:
		MtCopyScene(const boost::shared_ptr<Config::Scene>& scene_config, network::event_loop* event_loop_)
			:MtScene(scene_config, event_loop_) {}
		MtCopyScene() = delete;
		virtual bool BroadcastMessage(const google::protobuf::Message& message, const std::vector<uint64>& except_guids = {}) override;

		virtual bool OnPlayerEnterBegin(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<Packet::PlayerEnterScene>& message) override;
		virtual bool OnPlayerEnterEnd(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<Packet::PlayerEnterScene>& message) override;
		virtual LogicArea GetInitLogicArea(const boost::shared_ptr<Packet::PlayerEnterScene>& message) override;
		virtual LogicArea GetPlayerLogicArea(const boost::shared_ptr<MtPlayer>& player) override;
		virtual LogicArea GetCurrentLogicArea(const boost::shared_ptr<MtPlayer>& player) override;
		virtual bool Transport(const boost::shared_ptr<MtPlayer>& player, const Packet::Position & init_pos) override;
	};

}

#endif // MTONLINE_GAMESERVER_MT_COPY_SCENE_H__
