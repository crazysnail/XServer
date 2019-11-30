#ifndef MTONLINE_GAMESERVER_MT_TARGET_SELECT_H__
#define MTONLINE_GAMESERVER_MT_TARGET_SELECT_H__
#include "../mt_types.h"
#include "../mt_arena/geometry.h"
#include "json/json.h"
#include "SkillConfig.pb.h"
#include <boost/functional/factory.hpp>
#include <map>

namespace Mt
{
	class MtSkillSelectTar
	{
	public:

/*
		//用于初略确定一个目标, 玩家就可以朝着目标移动了
		static auto SelectForMove(const MtActor& owner, const std::vector<boost::shared_ptr<MtActor>>& teammates,
			const ::std::vector<boost::shared_ptr<MtActor>>& enemies,
			Json::Value& skill_config)->std::remove_reference<decltype(teammates)>::type::value_type;
		//技能将会攻击到的所有目标
		//owner参数用来取朝向和位置
		static auto SelectForSkill(const MtActor& owner, const std::vector<boost::shared_ptr<MtActor>>& teammates,
			const ::std::vector<boost::shared_ptr<MtActor>>& enemies,
			Json::Value& skill_config) -> std::remove_reference<decltype(teammates)>::type;
		static void AddExtraTarget(std::vector<boost::shared_ptr<MtActor>>& targets,
			const std::vector<boost::shared_ptr<MtActor>>& actors,
			const Config::TargetSelectExtra* param);
		static void SortTarget(const MtActor& owner, std::vector<boost::shared_ptr<MtActor>>& actors, Config::TargetSortType type);
		static auto AreaSelect(const MtActor& owner, const std::vector<boost::shared_ptr<MtActor>>& actors,
			const Config::TargetSelectLogic& select_param) -> std::remove_reference<decltype(actors)>::type;*/
		static auto CampSelect(const std::vector<MtActor*>& teammates,
			const std::vector<MtActor*>& enemies, Config::TargetCamp camp) -> std::remove_reference<decltype(teammates)>::type;
	};
// 	class MtSkillSelectTarStrategy : public boost::noncopyable
// 	{
// 	public:
// 		virtual bool SelectTarget(const boost::shared_ptr<MtActorSkill>& skill, 
// 			const boost::shared_ptr<MtActor> self,
// 			std::vector<boost::shared_ptr<MtActor>>& result) const = 0;
// 	};
// 
// 	//直接选前面的
// 	class MtSkillSelectTarStrategyQuick : public MtSkillSelectTarStrategy
// 	{
// 	public:
// 		virtual bool SelectTarget(const boost::shared_ptr<MtActorSkill>& skill,
// 			const boost::shared_ptr<MtActor> self,
// 			std::vector<boost::shared_ptr<MtActor>>& result) const override;
// 	};
// 
// 	//距离选取
// 	class MtSkillSelectTarStrategyRange : public MtSkillSelectTarStrategy
// 	{
// 	public:
// 		virtual bool SelectTarget(const boost::shared_ptr<MtActorSkill>& skill,
// 			const boost::shared_ptr<MtActor> self,
// 			std::vector<boost::shared_ptr<MtActor>>& result) const override;
// 	};
// 
// 	//随机选择
// 	class MtSkillSelectTarStrategyRandom : public MtSkillSelectTarStrategy
// 	{
// 	public:
// 		virtual bool SelectTarget(const boost::shared_ptr<MtActorSkill>& skill,
// 			const boost::shared_ptr<MtActor> self,
// 			std::vector<boost::shared_ptr<MtActor>>& result) const override;
// 	};
// 
// 	//固定目标数量选择, 如果目标数量不够, 重复选择当前可攻击对象
// 	class MtSkillSelectTarStrategyStableTargetCount : public MtSkillSelectTarStrategy
// 	{
// 	public:
// 		virtual bool SelectTarget(const boost::shared_ptr<MtActorSkill>& skill,
// 			const boost::shared_ptr<MtActor> self,
// 			std::vector<boost::shared_ptr<MtActor>>& result) const override;
// 	};
// 
// 	//混乱选取
// 	class MtSkillSelectTarStrategyConfusion : public MtSkillSelectTarStrategy
// 	{
// 	public:
// 		virtual bool SelectTarget(const boost::shared_ptr<MtActorSkill>& skill,
// 			const boost::shared_ptr<MtActor> self,
// 			std::vector<boost::shared_ptr<MtActor>>& result) const override;
// 	};
// 	//仇恨选取
// 	class MtSkillSelectTarStrategyHatred : public MtSkillSelectTarStrategy
// 	{
// 	public:
// 		virtual bool SelectTarget(const boost::shared_ptr<MtActorSkill>& skill,
// 			const boost::shared_ptr<MtActor> self,
// 			std::vector<boost::shared_ptr<MtActor>>& result) const override;
// 	};
// 	//低血量选取
// 	class MtSkillSelectTarStrategyLowHp : public MtSkillSelectTarStrategy
// 	{
// 	public:
// 		virtual bool SelectTarget(const boost::shared_ptr<MtActorSkill>& skill,
// 			const boost::shared_ptr<MtActor> self,
// 			std::vector<boost::shared_ptr<MtActor>>& result) const override;
// 	};
// 
// 	//溅射目标选择
// 	class MtSkillSelectTarStrategySpurting : public MtSkillSelectTarStrategy
// 	{
// 	public:
// 		virtual bool SelectTarget(const boost::shared_ptr<MtActorSkill>& skill,
// 			const boost::shared_ptr<MtActor> self,
// 			std::vector<boost::shared_ptr<MtActor>>& result) const override;
// 	};
// 
// 	class MtSkillSelectTarStrategyFlashChain : public MtSkillSelectTarStrategy
// 	{
// 	public:
// 		virtual bool SelectTarget(const boost::shared_ptr<MtActorSkill>& skill,
// 			const boost::shared_ptr<MtActor> self,
// 			std::vector<boost::shared_ptr<MtActor>>& result) const override;
// 	};
// 
// 	class MtSkillSelectTarStrategyFactory : public boost::serialization::singleton<MtSkillSelectTarStrategyFactory>
// 	{
// 	public:
// 		const boost::shared_ptr<MtSkillSelectTarStrategy>& CreateStrategy(const Config::TargetSortType type);
// 		const boost::shared_ptr<MtSkillSelectTarStrategy>& CreateStrategyBySkillType(const MtActorSkill& skill);
// 		MtSkillSelectTarStrategyFactory();
// 	private:
// 		std::map<Config::TargetSortType,boost::shared_ptr<MtSkillSelectTarStrategy>> strategys_;
// 		std::map<Config::TargetSortType, boost::function<MtSkillSelectTarStrategy*()>> factories_;
// 	};
}
#endif // MTONLINE_GAMESERVER_MT_TARGET_SELECT_H__
