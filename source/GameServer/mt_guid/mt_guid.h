#ifndef ZXERO_GAMESERVER_MT_GUID_H__
#define ZXERO_GAMESERVER_MT_GUID_H__
#include "../mt_types.h"
#include "../mt_config/mt_config.h"
#include "ClientMove.pb.h"
#include <boost/call_traits.hpp>
#include <boost/type_traits.hpp>
#include <boost/static_assert.hpp>


namespace Mt
{
	enum GuidTypes
	{
		INVALID_GUID_TYPE = -1,
		ACCOUNT_GUID,
		PLAYER_GUID,
		ACTOR_GUID,
		ITEM_GUID,
		SKILL_GUID,
		GUILD_GUID,
		MONSTER_GUID,
		BATTLE_GROUND_GUID,
		SCENE_STAGE_GUID,
		PLAYER_SCENE_STAGE_GUID,
		CONTAINER_GUID,
		NORMAL_ITEM_GUID,
		EQUIP_ITEM_GUID,
		GEM_ITEM_GUID,
		TRAP_GUID,
		ITEM_ADD_LOG_GUID,
		MARKET_GUID,
		ACTOR_FORMATION_GUID,
		MISSION_GUID,
		FRIEND_GUID,
		MESSAGELISTPLAYER_GUID,
		MESSAGE_GUID,
		ZONEINFO_GUID,
		TEAM_GUID,
		DATACELL_GUID,
		CAPTURE_LOG_GUID,
		GUILD_USER_DAMAGE,
		BATTLE_REPLY_GUID,
		MESSAGE_BOARD_GUID,
		FRIEND_MESSAGE_GUID,
		PAYORDER,
		ARENA_USER_GUID,
	};

	template <typename T>
	struct Guid
	{
		enum GuidType {

			Value = GuidTypes::INVALID_GUID_TYPE
		};
	};
	template<>
	struct Guid<Packet::DataCell>
	{
		enum GuidType {
			Value = GuidTypes::DATACELL_GUID
		};
	};
	template<>
	struct Guid<Packet::AccountModel>
	{
		enum GuidType {
			Value = GuidTypes::ACCOUNT_GUID
		};
	};
	template<>
	struct Guid<Packet::PlayerDBInfo>
	{
		enum GuidType
		{
			Value = GuidTypes::PLAYER_GUID
		};
	};
	template<>
	struct Guid<Packet::ActorBasicInfo>
	{
		enum GuidType
		{
			Value = GuidTypes::ACTOR_GUID
		};
	};
	template<>
	struct Guid<MtBattleGround>
	{
		enum GuidType
		{
			Value = GuidTypes::BATTLE_GROUND_GUID
		};
	};
	template<>
	struct Guid<MtActor>
	{
		enum GuidType
		{
			Value = GuidTypes::MONSTER_GUID
		};
	};

	template<>
	struct Guid<Packet::ActorSkill>
	{
		enum GuidType
		{
			Value = GuidTypes::SKILL_GUID
		};
	};

	template<>
	struct Guid<Packet::SceneStage>
	{
		enum GuidType
		{
			Value = GuidTypes::SCENE_STAGE_GUID
		};
	};
	template<>
	struct Guid<Packet::PlayerSceneStage>
	{
		enum GuidType
		{
			Value = GuidTypes::PLAYER_SCENE_STAGE_GUID
		};
	};
	template<>
	struct Guid<Packet::Container>
	{
		enum GuidType
		{
			Value = GuidTypes::CONTAINER_GUID
		};
	};
	template<>
	struct Guid<Packet::NormalItem>
	{
		enum GuidType
		{
			Value = GuidTypes::NORMAL_ITEM_GUID
		};
	};
	template<>
	struct Guid<Packet::EquipItem>
	{
		enum GuidType
		{
			Value = GuidTypes::EQUIP_ITEM_GUID
		};
	};
	template<>
	struct Guid<Packet::GemItem>
	{
		enum GuidType
		{
			Value = GuidTypes::GEM_ITEM_GUID
		};
	};
	template<>
	struct Guid<TrapImpl::Trap>
	{
		enum GuidType
		{
			Value = GuidTypes::TRAP_GUID
		};
	};
	template<>
	struct Guid<Config::ItemAddLog>
	{
		enum GuidType
		{
			Value = GuidTypes::ITEM_ADD_LOG_GUID
		};
	};

	template<>
	struct Guid<Packet::Market>
	{
		enum GuidType
		{
			Value = GuidTypes::MARKET_GUID
		};
	};
	template<>
	struct Guid<Packet::ActorFormation>
	{
		enum GuidType
		{
			Value = GuidTypes::ACTOR_FORMATION_GUID
		};
	};

	template<>
	struct Guid<Packet::Mission>
	{
		enum GuidType
		{
			Value = GuidTypes::MISSION_GUID
		};
	};

	template<>
	struct Guid<Packet::FriendPlayer>
	{
		enum GuidType
		{
			Value = GuidTypes::FRIEND_GUID
		};
	};
	template<>
	struct Guid<Packet::MessageListPlayerInfo>
	{
		enum GuidType
		{
			Value = GuidTypes::MESSAGELISTPLAYER_GUID
		};
	};
	template<>
	struct Guid<Packet::MessageInfo>
	{
		enum GuidType
		{
			Value = GuidTypes::MESSAGE_GUID
		};
	};
	template<>
	struct Guid<Packet::ZoneInfo>
	{
		enum GuidType
		{
			Value = GuidTypes::ZONEINFO_GUID
		};
	};
	template<>
	struct Guid<Packet::TeamInfo>
	{
		enum GuidType
		{
			Value = GuidTypes::TEAM_GUID
		};
	};
	template<>
	struct Guid<Packet::GuildDBInfo>
	{
		enum GuidType
		{
			Value = GuidTypes::GUILD_GUID
		};
	};

	template<>
	struct Guid<Packet::GuildCaptureBattleLogDB>
	{
		enum  GuidType
		{
			Value = GuidTypes::CAPTURE_LOG_GUID
		};
	};
	template<>
	struct Guid<Packet::GuildUserDamage>
	{
		enum  GuidType
		{
			Value = GuidTypes::GUILD_USER_DAMAGE
		};
	};
	template<>
	struct Guid<Packet::BattleReply>
	{
		enum GuidType
		{
			Value = GuidTypes::BATTLE_REPLY_GUID
		};
	};
	template<>
	struct Guid<Packet::MessageBoard>
	{
		enum GuidType
		{
			Value = GuidTypes::MESSAGE_BOARD_GUID
		};
	};
	template<>
	struct Guid<Packet::PayOrderInfo>
	{
		enum GuidType
		{
			Value = GuidTypes::PAYORDER
		};
	};
	template<>
	struct Guid<Packet::FriendMessageInfo>
	{
		enum GuidType
		{
			Value = GuidTypes::FRIEND_MESSAGE_GUID
		};
	};

	template<>
	struct Guid<DB::ArenaUser>
	{
		enum GuidType
		{
			Value = GuidTypes::ARENA_USER_GUID
		};
	};

	class MtGuid : public boost::noncopyable
	{
	public:
		template<typename T>
		uint64 operator()(const T& type)
		{
			type;
			BOOST_STATIC_ASSERT_MSG(!boost::is_pointer<T>::value, " type T should not be pointer");
			return GenGuid<T>();
/*
			BOOST_STATIC_ASSERT_MSG(Guid<T>::GuidType::Value != GuidTypes::INVALID_GUID_TYPE, "this type does not define a guid type");
			uint64 server_id = (uint64)MtConfig::Instance().server_id_ << (64 - 10); //10位服务器id
			uint64 guid_type = (uint64)Guid<T>::GuidType::Value;
			guid_type = (uint64)guid_type << (64 - 10 - 6);	//6位类型id 2^6 = 64
			uint64 random_high = (uint64)rand_gen->gen() << 32 & 0x0000FFFF00000000;
			uint64 random_low = (uint64)rand_gen->gen() & 0x00000000FFFFFFFF;
			return server_id | guid_type | random_high | random_low;*/
		}
		template <typename T>
		zxero::uint64 GenGuid() 
		{
			BOOST_STATIC_ASSERT_MSG(Guid<T>::GuidType::Value != GuidTypes::INVALID_GUID_TYPE, "this type does not define a guid type");
			zxero::uint64 server_id = (zxero::uint64)MtConfig::Instance().server_id_ << (64 - 10); //10位服务器id
			zxero::uint64 guid_type = (zxero::uint64)Guid<T>::GuidType::Value;
			guid_type = (zxero::uint64)guid_type << (64 - 10 - 6);	//6位类型id 2^6 = 64
			zxero::uint64 random_high = (zxero::uint64)rand_gen->gen() << 32 & 0x0000FFFF00000000;
			zxero::uint64 random_low = (zxero::uint64)rand_gen->gen() & 0x00000000FFFFFFFF;
			return server_id | guid_type | random_high | random_low;
		}
		static MtGuid& Instance();
		int OnLoad();
		MtGuid();
	};


	
}

#endif // ZXERO_GAMESERVER_MT_GUID_H__
