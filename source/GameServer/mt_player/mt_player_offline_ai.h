#ifndef MTONLINE_GAMESERVER_MT_PLAYER_OFFLINE_AI_H__
#define MTONLINE_GAMESERVER_MT_PLAYER_OFFLINE_AI_H__

#include "../mt_types.h"

namespace Mt
{
	class PlayerAi : public boost::noncopyable
	{
		public:
			PlayerAi(const boost::shared_ptr<MtPlayer>& owner) :owner_(owner) {}
			virtual void OnTick(uint64 elapseTime) = 0;
			virtual void OnPlayerOffLine() = 0;
			virtual void OnPlayerOnLine() = 0;
			virtual void OnReSetHookData() {};
			virtual void OnPlayerScoreChanged() {}
			virtual void OnLeaveScene() {};
	protected:
		boost::weak_ptr<MtPlayer> owner_;
	};

	enum class PlayerAiType
	{
		AutoHook = 1,
		QuitTeam = 2,
	};
	std::shared_ptr<PlayerAi> CreateOffLineAi(PlayerAiType type, const boost::shared_ptr<MtPlayer>& owner);

}
#endif // MTONLINE_GAMESERVER_MT_PLAYER_OFFLINE_AI_H__
