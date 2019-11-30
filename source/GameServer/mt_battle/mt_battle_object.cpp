#include "mt_battle_object.h"
#include "../mt_actor/mt_actor.h"
#include "../mt_skill/mt_skill.h"
#include "../mt_arena/geometry.h"
#include <boost/make_shared.hpp>
#include "mt_battle_ground.h"

namespace Mt
{
	namespace Battle
	{

		bool MoveableObject::MoveToTarget(const boost::shared_ptr<BaseObject>& target)
		{
			move_toward_target_ = target;
			return MoveToPosition(target->Position(), StopRange(target));
		}

		bool MoveableObject::MoveToPosition(const Packet::Position& target_pos, int32 stop_range)
		{
			ZXERO_ASSERT(speed_ > 0) << "speed less or eq to 0, can't move";
			if (zxero::distance2d(pos_, target_pos) <= stop_range) {
				return true;
			}
			else {
				auto dir = zxero::get_direction(pos_, target_pos);
				Direction(dir);
				auto move_range = speed_ * battle_ground_.FrameElapseTime();
				auto new_pos = zxero::move_point(pos_, (int32)move_range, dir);
				Position(new_pos);
				return false;
			}
		}


		void HeroActor::Init(boost::shared_ptr<Mt::MtActor>& source)
		{
			source_ = source;
			std::vector<std::pair<int32, int32>> skill_info;
			std::transform(std::begin(source->DbSkills()), std::end(source->DbSkills()), std::back_inserter(skill_info),
				[=](auto& db_info) {return std::make_pair(db_info->DbInfo()->skill_id(), db_info->DbInfo()->skill_level()); }
			);
			InitSkills(skill_info);
			battle_info_.CopyFrom(source->DbInfo()->battle_info());
			battle_info_max_.CopyFrom(battle_info_);
		}

		bool HeroActor::SelectSkill()
		{
			throw std::logic_error("The method or operation is not implemented.");
		}

		void BattleObject::InitSkills(std::vector<std::pair<int32, int32>>& skill_info)
		{
			std::transform(std::begin(skill_info), std::end(skill_info), std::back_inserter(skills_),
				[&](auto& info_in_pair) {return boost::make_shared<MtSkill>(info_in_pair.first, info_in_pair.second); });
		}

		bool BattleObject::BeSupposeToDead() const
		{
			throw std::logic_error("The method or operation is not implemented.");
		}

	}
}