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
		//���ڳ���ȷ��һ��Ŀ��, ��ҾͿ��Գ���Ŀ���ƶ���
		static auto SelectForMove(const MtActor& owner, const std::vector<boost::shared_ptr<MtActor>>& teammates,
			const ::std::vector<boost::shared_ptr<MtActor>>& enemies,
			Json::Value& skill_config)->std::remove_reference<decltype(teammates)>::type::value_type;
		//���ܽ��ṥ����������Ŀ��
		//owner��������ȡ�����λ��
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
// 	//ֱ��ѡǰ���
// 	class MtSkillSelectTarStrategyQuick : public MtSkillSelectTarStrategy
// 	{
// 	public:
// 		virtual bool SelectTarget(const boost::shared_ptr<MtActorSkill>& skill,
// 			const boost::shared_ptr<MtActor> self,
// 			std::vector<boost::shared_ptr<MtActor>>& result) const override;
// 	};
// 
// 	//����ѡȡ
// 	class MtSkillSelectTarStrategyRange : public MtSkillSelectTarStrategy
// 	{
// 	public:
// 		virtual bool SelectTarget(const boost::shared_ptr<MtActorSkill>& skill,
// 			const boost::shared_ptr<MtActor> self,
// 			std::vector<boost::shared_ptr<MtActor>>& result) const override;
// 	};
// 
// 	//���ѡ��
// 	class MtSkillSelectTarStrategyRandom : public MtSkillSelectTarStrategy
// 	{
// 	public:
// 		virtual bool SelectTarget(const boost::shared_ptr<MtActorSkill>& skill,
// 			const boost::shared_ptr<MtActor> self,
// 			std::vector<boost::shared_ptr<MtActor>>& result) const override;
// 	};
// 
// 	//�̶�Ŀ������ѡ��, ���Ŀ����������, �ظ�ѡ��ǰ�ɹ�������
// 	class MtSkillSelectTarStrategyStableTargetCount : public MtSkillSelectTarStrategy
// 	{
// 	public:
// 		virtual bool SelectTarget(const boost::shared_ptr<MtActorSkill>& skill,
// 			const boost::shared_ptr<MtActor> self,
// 			std::vector<boost::shared_ptr<MtActor>>& result) const override;
// 	};
// 
// 	//����ѡȡ
// 	class MtSkillSelectTarStrategyConfusion : public MtSkillSelectTarStrategy
// 	{
// 	public:
// 		virtual bool SelectTarget(const boost::shared_ptr<MtActorSkill>& skill,
// 			const boost::shared_ptr<MtActor> self,
// 			std::vector<boost::shared_ptr<MtActor>>& result) const override;
// 	};
// 	//���ѡȡ
// 	class MtSkillSelectTarStrategyHatred : public MtSkillSelectTarStrategy
// 	{
// 	public:
// 		virtual bool SelectTarget(const boost::shared_ptr<MtActorSkill>& skill,
// 			const boost::shared_ptr<MtActor> self,
// 			std::vector<boost::shared_ptr<MtActor>>& result) const override;
// 	};
// 	//��Ѫ��ѡȡ
// 	class MtSkillSelectTarStrategyLowHp : public MtSkillSelectTarStrategy
// 	{
// 	public:
// 		virtual bool SelectTarget(const boost::shared_ptr<MtActorSkill>& skill,
// 			const boost::shared_ptr<MtActor> self,
// 			std::vector<boost::shared_ptr<MtActor>>& result) const override;
// 	};
// 
// 	//����Ŀ��ѡ��
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
