#ifndef MTONLINE_GAMESERVER_MT_BATTLE_TRAP_H__
#define MTONLINE_GAMESERVER_MT_BATTLE_TRAP_H__
#include "../mt_types.h"
#include "json/json.h"
#include "Base.pb.h"

namespace Mt
{
	namespace TrapImpl
	{
		class Trap : public boost::noncopyable, public boost::enable_shared_from_this<Trap>
		{
		public:
			Trap();
			MtActor* Creator() const;
			void Creator(MtActor* creator);
			void Config(const Json::Value* config) { config_ = config; }
			const Json::Value& Config() const { return *config_; }
			void ActorPositionChanged(MtActor& actor);
			virtual void OnTick(uint64 elapseTime);
			void Position(Packet::Position& pos);
			void TrapLevel(int32 level) {level_ = level;}
			void SetTrapId(int32 id) { id_ = id; }
			int32 GetTrapId() const { return id_; }
			int32 TrapLevel() const { return level_;}
			void SendCreateMsg() const;
			virtual bool NeedDestroy();
			virtual void OnDestroy();
			virtual void AfterCreate();
			virtual void OnActorEnter(MtActor& actor);
			virtual void OnActorLeave(MtActor& actor);
			void Range(int32 range) { range_ = range; }
		protected:
			uint64 guid_;
			bool triggerd_ = false;
			const Json::Value* config_ = nullptr;
			int32 id_ = 0;
			int32 level_ = 0;
			int32 life_time_ = 0;
			int32 range_ = 0;
			Packet::Position pos_;
			boost::shared_ptr<MtActor> creator_;
			std::vector<uint64> in_trap_actors_;
		};
		class Bomb : public Trap {
		public:
			/**
			* \brief 爆炸有溅射
			* \return void
			*/
			virtual void AfterCreate() override;
		};
		boost::shared_ptr<Trap> CreateTrap(const Json::Value& config,
			Packet::Position& target_pos,
			SkillContext& ctx,
			MtActor* creator);
	}
	/*class MtTrap : public boost::noncopyable
	{
	public:
		MtTrap(boost::shared_ptr<Config::TrapConfig>& config, const Packet::Position& pos, const boost::shared_ptr<MtActor>& creator, int32 damage);
		MtTrap(boost::shared_ptr<Config::TrapConfig>& config, int32 battle_group_index, const boost::shared_ptr<MtActor>& creator, int32 damage);
		//返回trap是否继续存活 true : 存活, false : 该消失了
		virtual bool OnTick(uint64 elapseTime);
	private:
		uint64 guid_;
		int64 left_time_;
		boost::shared_ptr<Config::TrapConfig> config_;
		Packet::Position pos_;
		int32 battle_group_index_ = 0;
		boost::shared_ptr<MtActor> creator_;
		int32 damage_ = 0;
	};*/

}
#endif // MTONLINE_GAMESERVER_MT_BATTLE_TRAP_H__
