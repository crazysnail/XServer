// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: AllConfigEnum.proto

#ifndef PROTOBUF_AllConfigEnum_2eproto_2efflua_2eh__INCLUDED
#define PROTOBUF_AllConfigEnum_2eproto_2efflua_2eh__INCLUDED

#include <google/protobuf/stubs/common.h>

#if GOOGLE_PROTOBUF_VERSION < 2005000
#error This file was generated by a newer version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please update
#error your headers.
#endif
#if 2005000 < GOOGLE_PROTOBUF_MIN_PROTOC_VERSION
#error This file was generated by an older version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please
#error regenerate this file with a newer version of protoc.
#endif

#include<lua/fflua_register.h>

#include "AllConfigEnum.pb.h"
namespace ff{
template<>
struct basetype_ptr_traits_t<::Config::BFState>
{
	typedef int32_t arg_type_t;
};

template<>
struct p_t<::Config::BFState>
{
	static ::Config::BFState r(int32_t a) { return (::Config::BFState)a; }
};
template<>
struct basetype_ptr_traits_t<::Config::BattleObjectAbility>
{
	typedef int32_t arg_type_t;
};

template<>
struct p_t<::Config::BattleObjectAbility>
{
	static ::Config::BattleObjectAbility r(int32_t a) { return (::Config::BattleObjectAbility)a; }
};
template<>
struct basetype_ptr_traits_t<::Config::BattleObjectStatus>
{
	typedef int32_t arg_type_t;
};

template<>
struct p_t<::Config::BattleObjectStatus>
{
	static ::Config::BattleObjectStatus r(int32_t a) { return (::Config::BattleObjectStatus)a; }
};
template<>
struct basetype_ptr_traits_t<::Config::ActivityGroup>
{
	typedef int32_t arg_type_t;
};

template<>
struct p_t<::Config::ActivityGroup>
{
	static ::Config::ActivityGroup r(int32_t a) { return (::Config::ActivityGroup)a; }
};
template<>
struct basetype_ptr_traits_t<::Config::DrawType>
{
	typedef int32_t arg_type_t;
};

template<>
struct p_t<::Config::DrawType>
{
	static ::Config::DrawType r(int32_t a) { return (::Config::DrawType)a; }
};
template<>
struct basetype_ptr_traits_t<::Config::EquipmentSlot>
{
	typedef int32_t arg_type_t;
};

template<>
struct p_t<::Config::EquipmentSlot>
{
	static ::Config::EquipmentSlot r(int32_t a) { return (::Config::EquipmentSlot)a; }
};
template<>
struct basetype_ptr_traits_t<::Config::EquipmentType>
{
	typedef int32_t arg_type_t;
};

template<>
struct p_t<::Config::EquipmentType>
{
	static ::Config::EquipmentType r(int32_t a) { return (::Config::EquipmentType)a; }
};
template<>
struct basetype_ptr_traits_t<::Config::WeaponType>
{
	typedef int32_t arg_type_t;
};

template<>
struct p_t<::Config::WeaponType>
{
	static ::Config::WeaponType r(int32_t a) { return (::Config::WeaponType)a; }
};
template<>
struct basetype_ptr_traits_t<::Config::EquipmentColor>
{
	typedef int32_t arg_type_t;
};

template<>
struct p_t<::Config::EquipmentColor>
{
	static ::Config::EquipmentColor r(int32_t a) { return (::Config::EquipmentColor)a; }
};
template<>
struct basetype_ptr_traits_t<::Config::ItemType>
{
	typedef int32_t arg_type_t;
};

template<>
struct p_t<::Config::ItemType>
{
	static ::Config::ItemType r(int32_t a) { return (::Config::ItemType)a; }
};
template<>
struct basetype_ptr_traits_t<::Config::ItemAddLogType>
{
	typedef int32_t arg_type_t;
};

template<>
struct p_t<::Config::ItemAddLogType>
{
	static ::Config::ItemAddLogType r(int32_t a) { return (::Config::ItemAddLogType)a; }
};
template<>
struct basetype_ptr_traits_t<::Config::ItemDelLogType>
{
	typedef int32_t arg_type_t;
};

template<>
struct p_t<::Config::ItemDelLogType>
{
	static ::Config::ItemDelLogType r(int32_t a) { return (::Config::ItemDelLogType)a; }
};
template<>
struct basetype_ptr_traits_t<::Config::MarketType>
{
	typedef int32_t arg_type_t;
};

template<>
struct p_t<::Config::MarketType>
{
	static ::Config::MarketType r(int32_t a) { return (::Config::MarketType)a; }
};
template<>
struct basetype_ptr_traits_t<::Config::MarketItemGroup>
{
	typedef int32_t arg_type_t;
};

template<>
struct p_t<::Config::MarketItemGroup>
{
	static ::Config::MarketItemGroup r(int32_t a) { return (::Config::MarketItemGroup)a; }
};
template<>
struct basetype_ptr_traits_t<::Config::FinishType>
{
	typedef int32_t arg_type_t;
};

template<>
struct p_t<::Config::FinishType>
{
	static ::Config::FinishType r(int32_t a) { return (::Config::FinishType)a; }
};
template<>
struct basetype_ptr_traits_t<::Config::TargetFinishType>
{
	typedef int32_t arg_type_t;
};

template<>
struct p_t<::Config::TargetFinishType>
{
	static ::Config::TargetFinishType r(int32_t a) { return (::Config::TargetFinishType)a; }
};
template<>
struct basetype_ptr_traits_t<::Config::TargetResolveType>
{
	typedef int32_t arg_type_t;
};

template<>
struct p_t<::Config::TargetResolveType>
{
	static ::Config::TargetResolveType r(int32_t a) { return (::Config::TargetResolveType)a; }
};
template<>
struct basetype_ptr_traits_t<::Config::SubFinishType>
{
	typedef int32_t arg_type_t;
};

template<>
struct p_t<::Config::SubFinishType>
{
	static ::Config::SubFinishType r(int32_t a) { return (::Config::SubFinishType)a; }
};
template<>
struct basetype_ptr_traits_t<::Config::MissionType>
{
	typedef int32_t arg_type_t;
};

template<>
struct p_t<::Config::MissionType>
{
	static ::Config::MissionType r(int32_t a) { return (::Config::MissionType)a; }
};
template<>
struct basetype_ptr_traits_t<::Config::NpcType>
{
	typedef int32_t arg_type_t;
};

template<>
struct p_t<::Config::NpcType>
{
	static ::Config::NpcType r(int32_t a) { return (::Config::NpcType)a; }
};
template<>
struct basetype_ptr_traits_t<::Config::VisibleType>
{
	typedef int32_t arg_type_t;
};

template<>
struct p_t<::Config::VisibleType>
{
	static ::Config::VisibleType r(int32_t a) { return (::Config::VisibleType)a; }
};
template<>
struct basetype_ptr_traits_t<::Config::MonsterType>
{
	typedef int32_t arg_type_t;
};

template<>
struct p_t<::Config::MonsterType>
{
	static ::Config::MonsterType r(int32_t a) { return (::Config::MonsterType)a; }
};
template<>
struct basetype_ptr_traits_t<::Config::AttackType>
{
	typedef int32_t arg_type_t;
};

template<>
struct p_t<::Config::AttackType>
{
	static ::Config::AttackType r(int32_t a) { return (::Config::AttackType)a; }
};
template<>
struct basetype_ptr_traits_t<::Config::ItemPackageType>
{
	typedef int32_t arg_type_t;
};

template<>
struct p_t<::Config::ItemPackageType>
{
	static ::Config::ItemPackageType r(int32_t a) { return (::Config::ItemPackageType)a; }
};
template<>
struct basetype_ptr_traits_t<::Config::SkillType>
{
	typedef int32_t arg_type_t;
};

template<>
struct p_t<::Config::SkillType>
{
	static ::Config::SkillType r(int32_t a) { return (::Config::SkillType)a; }
};
template<>
struct basetype_ptr_traits_t<::Config::AreaType>
{
	typedef int32_t arg_type_t;
};

template<>
struct p_t<::Config::AreaType>
{
	static ::Config::AreaType r(int32_t a) { return (::Config::AreaType)a; }
};
template<>
struct basetype_ptr_traits_t<::Config::TargetCamp>
{
	typedef int32_t arg_type_t;
};

template<>
struct p_t<::Config::TargetCamp>
{
	static ::Config::TargetCamp r(int32_t a) { return (::Config::TargetCamp)a; }
};
template<>
struct basetype_ptr_traits_t<::Config::PropertyReviseValueType>
{
	typedef int32_t arg_type_t;
};

template<>
struct p_t<::Config::PropertyReviseValueType>
{
	static ::Config::PropertyReviseValueType r(int32_t a) { return (::Config::PropertyReviseValueType)a; }
};
template<>
struct basetype_ptr_traits_t<::Config::BuffType>
{
	typedef int32_t arg_type_t;
};

template<>
struct p_t<::Config::BuffType>
{
	static ::Config::BuffType r(int32_t a) { return (::Config::BuffType)a; }
};
template<>
struct basetype_ptr_traits_t<::Config::TargetSortType>
{
	typedef int32_t arg_type_t;
};

template<>
struct p_t<::Config::TargetSortType>
{
	static ::Config::TargetSortType r(int32_t a) { return (::Config::TargetSortType)a; }
};
template<>
struct basetype_ptr_traits_t<::Config::ImpactEffectType>
{
	typedef int32_t arg_type_t;
};

template<>
struct p_t<::Config::ImpactEffectType>
{
	static ::Config::ImpactEffectType r(int32_t a) { return (::Config::ImpactEffectType)a; }
};
}
namespace Config {
bool AllConfigEnum_fflua_regist_all(lua_State* state);
}
#endif //PROTOBUF_AllConfigEnum_2eproto_2efflua_2eh__INCLUDED
