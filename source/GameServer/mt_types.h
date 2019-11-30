#ifndef ZXERO_GAMESERVER_MT_TYPES_H__
#define ZXERO_GAMESERVER_MT_TYPES_H__


#include "types.h"
#include "random_generator.h"
#include "log.h"
#include <algorithm>
#include <utility>
#include <boost/noncopyable.hpp>
#include <boost/serialization/singleton.hpp>
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/thread/tss.hpp>
#include <boost/range/adaptors.hpp>
#include <boost/range/algorithm.hpp>
#include <boost/range/algorithm_ext.hpp>
#include <google/protobuf/message.h>
#include <google/protobuf/repeated_field.h>
#include "lua/fflua.h"
#include <mutex>
#include <stdexcept>


using google::protobuf::RepeatedFieldBackInserter;
namespace google
{
	namespace protobuf
	{
		class Message;
	}
}
namespace DB
{
	class ArenaUser;
	class RobotInfo;
	class PlayerTrialInfo;
}

namespace S2S //Scene to Scene 通信消息
{
	class S2SCommonMessage;
}

namespace G2S //Game to Scene 通信消息
{
	class CacheAllNotify;
	class NewDayNotify;
	class NewWeekNotify;
	class XLuaRegNotify;
	class LuaLoadNotify;
	class PlayerOnline;
	class SyncPlayerTeamData;
	class ResetPlayerTeamData;
	class G2SCommonMessage;
	class SyncPlayerGuildData;
	class GS2SAddMailToPlayer;
	class G2SCodeCheckDone;
}

namespace S2G //Scene to Game 通信消息
{
	class S2GCommonMessage;
	class TeamMemberChangeScene;
	class PlayerCaptureStatge;
	class RobotPlayerIn;
	class RobotPlayerOut;
	class PlayerHookDataUpdate;
	class PlayerUpdateCaptureForm;
	class RefreshTarget;
	class DestroyRoom;
	class CheckOffLine;
	class GuildChat;
	class TeamChat;
	class WorldChat;
	class PlayerAutoMatch;
	class HookStageDistributeUpdate;
	class EndGuildBoss;
	class InsertScenePlayer;
	class GuildBattleWeekRank;
	class GuildBattleAllRank;
	class XLuaRegNotify;
	class LuaLoadNotify;
	class BuffPoint;
	class GetOperateInfo;
	class AddGuildMoney;
}

namespace Config
{
	class ConstConfig;
	class FunctionConfig;
	class Scene;
	class ActivityTimes;
	class ItemDropGroup;
	class SceneBattleGroup;
	class MonsterGroup;
	class MonsterConfig;
	class SkillConfig;
	class BuffDefine;
	class ImpactConfig;
	class EquipmentConfig;
	class SceneStageConfig;
	class ItemConfig;
	class EquipConfig;
	class GemConfig;
	class ItemPackageConfig;
	class TrapConfig;
	class ItemAddLog;
	class EquipFixCost;
	class EquipEnchantCost;
	class EquipEnhenceCost;
	class EquipDismantle;
	class FragmentConfig;
	class SkillBuildConfig;
	class EquipmentSuitConfig;
	class MarketConfig;
	class MarketItemConfig;
	class MissionConfig;
	class ItemDelLog;	
	class EnhanceMasterConfig;
	class GemCostConfig;	
	class SceneData;
	class MissionExConfig;
	class ActivityConfig;	
	class GuildBuildLevelConfig;
	class GuildPracticeConfig;
	class GuildPracticeLevelConfig;
	class DrawCardConfig;
	class MonsterAttribute;
	class GuildCDKConfig;
	class GuildWetCopyConfig;
	class GuildPositionConfig;
	class GuildWetCopyStageConfig;
	class GuildLevelFixConfig;
	class GuildDonateConfig;
	class ActivityRewardConfig;
	class TextAnswerConfig;
	class PicAnswerConfig;
	class ItemPackageElement;	
	class LevelConfig;
	class LackeysConfig;
	class OneKeyConfig;
	class ChargeItemConfig;
	class ChargeList;
	class EquipBookConfig;
}

namespace DB {
	class TrialTarget;
	class PlayerTowerInfo;
}
namespace Packet
{
	class ShmHead;
	class LuaFile;
	class LuaFilesReply;
	class ReNameRequest;
	class ServerTime;
	class PlayerDBInfo;
	class ServerInfo;
	class LootPlayer;
	class OfflineHookData;
	class ActorMoveToTargetFinish;
	class BattleReply;
	class ChangeHair;
	class MountConfig;
	class PlayerSetting;
	class CG_StartChallengeCapturePoint;
	class CG_UpdateStrongHold;
	class SceneStageInfo;
	class RTHC;
	class RobotPlayerInOk;
	class RealTimeHookEfficient;
	class PlayerStageDB;
	class PlayerStages;
	class PlayerStage;
	class CaptureStageToScene;
	class GetSceneStageInfo;
	class CapturePointDB;
	class GuildCapturePointDB;
	class GuildCaptureBattleLogDB;
	class ExitBattleGround;
	class ConstantChallengeBoss;
	class ArenaMainData;
	class ArenaBuyChallengeCount;
	class ArenaChallengePlayer;
	class ArenaPanelOpen;
	class ArenaPanelClose;
	class ArenaRefreshTarget;
	class GetClientSceneStageInfo;
	class ClientSceneStageInfo;
	class PlayerStages;
	class EnterTrialField;
	class ChallengeTrialPlayer;
	class PlayerStartHook;
	class PlayerEndHook;
	class PlayerRunTimeStatus;
	class LoginRequest;
	class CreatePlayer;
	class CaptureStage;
	class UpdateFollowState;
	class CollectAllTax;
	class UpdateStageTax;
	class BattleFormation;
	class OnHookRankUserRuntime;
	class PlayerEnterScene;
	class SessionChanged;
	class PlayerDisconnect;
	class PlayerLeaveScene;
	class ClientPlayerMove;
	class ClientPlayerStop;
	class ScenePlayerInfo;
	class AccountModel;
	class PlayerBasicInfo;
	class ScenePlayerList;
	class SetControlType;
	class ClientActorMove;
	class ClientActorStop;
	class ClientActorUseSkill;
	class Point;
	class Position;
	class AccountNameVerify;
	class AccountRegist;
	class AccountPlayerList;
	class ActorBasicInfo;
	class BattleInfo;
	class ActorFullInfo;
	class ActorList;
	class ChatMessage;
	class BFConfig;
	class BattleGroundBrief;
	class InsightBattleGround;
	class ChallengeBossReq;
	class BattleGroundOverNotify;
	class EnterBattleField;
	class BattleFieldFightTarget;
	class BuffPoint;
	class SkillInfo;
	class BuffInfo;
	class ActorMove;
	class ActorUseSkill;
	class ActorOnDamage;
	class ActorBuffAttach;
	class ActorBuffDettach;
	class ActorDead;
	class ActorRelive;
	class ActorBattleInfo;
	class ActorOnCure;
	class MonsterBattleActor;
	class AllBattleGround;
	class ActorSkill;
	class ActorAddBuff;
	class SceneObject;
	class MapPushStart;
	class MapPushStop;
	class SceneStage;
	class PlayerSceneStage;
	class SceneStageInfoReply;
	class PlayerReachBattlePos;
	class Container;
	class NormalItem;
	class EquipItem;
	class EquipItem_EquipEnchant;
	class GemItem;
	class ClientItemInfo;
	class ClientEquipment;
	class PlayerContainerRequest;
	class PlayerContainerReply;
	class ActorAddEquipRequest;
	class ActorDelEquipRequest;
	class SceneStageInfoRequest;
	class SceneStageNew;
	class NewActorCreated;
	class NewActorCreatedDone;
	class StatisticInfo;
	class PlayerLoadEquip;
	class PlayerUnloadEquip;
	class PrivateChatMessageReq;
	class UpgradeColorReq;
	class UpgradeLevelReq;
	class UpgradeStarReq;
	class UpgradeSkillReq;
	class EnhenceEquipReq;
	class EnchantEquipReq;
	class InsetEquipReq;
	class TakeOutGemReq;
	class EquipInheritReq;
	class AddEnchantSlotReq;
	class BatchEnchantEquipReq;
	class BatchEnchantEquipConfirmReq;
	class BatchEnchantEquipReply;
	class FixEquipReq;
	class GemFusionReq;
	class DismantleEquipReq;
	class FragmentToHeroReq;	
	class Market;
	class PlayerMarketReply;
	class PlayerMarketDelItemNotify;
	class PlayerOperateInfoReq;
	class CommonRequest;
	class CommonReply;
	class ServerCommonRequest;
	class ServerCommonReply;
	class ServerCommonResult;
	class ClientClickMove;
	class ActorFormationPull;
	class SetActorFightReq;
	class ActorFormation;
	class Mission;
	class NpcList;
	class NpcInfo;
	class EquipMagicalReq;
	class CommonResult;
	class PlayerToken;
	class FriendPlayer;
	class MessageListPlayerInfo;
	class MessageInfo;
	class ZoneInfo;
	class SimpleImpactData;
	class MoveItemRequest;
	class ExtendContainerRequest;
	class NotifyItemList;
	class TeamInfo;
	class CreateTeam;
	class CreateRobotTeam;
	class DelTeam;
	class ApplyTeamReq;
	class AgreeApplyTeamReq;
	class LevelTeamReq;
	class AskLevelTeamReq;
	class SetPurposeInfoReq;
	class SetTeamAutoMatchReq;
	class TeamInfoReq;
	class FindTeamListReq;
	class TeamApplyListReq;
	class PlayerAutoMatchReq;
	class PlayerAutoMatchInfo;
	class PurposeInfo;
	class TeamPlayerInfo;
	class TeamPlayerBasicInfo;
	class TeamFullPlayerInfo;
	class TeamAutomatchConfig;
	class ChangeTeamTypeReq;
	class ChangeTeamArrangeTypeReq;
	class PlayerSetTeamStatusReq;
	class SetTeamLeaderReq;
	class ApplyTeamLeaderReq;
	class ApplyMissionTeamReq;
	class AskApplyTeamLeaderReply;
	class TeamInfoReply;
	class SetTeamHeroPositionReq;
	class InviteyAgreeTeamReq;
	class InviteyAddTeamReq;
	class PlayerAutoMatchReply;
	class FriendInfotReq;
	class FriendListReq;
	class AddFriendReq;
	class DelFriendReq;
	class AskTeamReq;
	class StatisticsResult;
	class AskGuildReq;
	class SendGiftReq;
	class ViewZoneReq;
	class MessageBoard;
	class ViewFriendMessageReq;
	class ViewSystemMessageReq;
	class SetZoneSignatureReq;
	class FriendMessageReq;
	class DelMessageReq;
	class DelClientFriendMessageNotifyRR;
	class RecommendReq;
	class FindFriendByNameReq;
	class FindFriendByGuidReq;
	class FriendMessageInfo;
	class NpcDynamicConfig;
	class PositionGroupConfig;
	class SceneLayout;
	class NpcStaticConfig;
	class NpcSession;
	class NoNpcSession;
	class HookOption;
	class ChangeHookInfo;
	class ClickChatMissionReq;
	class SyncMissionRequest;
	class SyncMissionReply;
	class SyncMission;
	class OnNpcSessionReply;
	class ClientEnterSceneOk;
	class PlayerEnterGameRequest;
	class PlayerChangeSceneRequest;
	class PlayerSceneInfoReq;
	class EnterRoomNotify;
	class SimpleImpactConfig;
	class OperativeConfig;
	//guild
	
	class GuildUser;
	class GuildDBInfo;
	class CreateGuildReq;
	class FindGuildReq;
	class ApplyGuildReq;
	class ApplyGuildPlayerInfo;
	class ApplyGuildPlayerListReq;
	class ClearApplyGuildPlayerListReq;
	class InviteyAddGuildReq;
	class InviteyAgreeGuildReq;
	class AgreeApplyGuildReq;
	class SetGuildNameRR;
	class SetGuildNoticeRR;
	class SetGuildIconRR;
	class SetGuildUserPositionRR;
	class GetGuildBonusReq;
	class GuildCDKInfo;
	class CreateGuildCDKReq;
	class GetGuildCDKReq;
	class GetGuildCDKListReq;
	class OneKeyApplyGuildReq;
	class GuildDonationReq;
	class BuildLevelUpReq;
	class PlayerPracticeReq;
	class GetPracticeReq;
	class PracticeInfo;
	class GetGuildListReq;
	class GetGuildUserListInfoReq;
	class GetGuildInfoReq;
	class LeaveGuildReq;
	class GuildBossCompetitionDbInfo;
	class GetGuildBossCompetitionReq;
	class FightGuildBossCompetitionReq;
	class GetGuildUserDamageReq;
	class GetServerUserDamageReq;
	class GuildUserDamage;
	class GuildAllDamage;
	class GuildBossUserDamage;
	class GuildWetCopyInfo;
	class OpenGuildWetCopy;
	class ResetGuildWetCopy;
	class FightGuildWetCopy;
	class GetGuildWetCopyInfosReq;
	class GuildBattleInfo;
	class GuildBattleFightTarget;
	class GuildBattleTeamInfo;
	class GuildBattlePlayerInfo;
	class GuildBattleListReq;
	class GuildWarehouse;
	class GuildWarehouseRecord;
	class GuildWarehouseRecordReq;
	class GuildWarehouseReq;
	class GuildWarehouseReceiveReq;
	class GuildWarehouseSetReceivePlayerReq;
	class GuildBattleWeekRankReq;
	class GuildBattleAllRankReq;
	class GuildSpoilItem;
	class GuildRecord;
	class GuildImpeach;
	class PlayerGuildUserData;
	//guild
	class DataCell;
	class Data64Cell;
	class Activity;
	class ActivityUpdateReply;
	class ActivityListReply;

	class DataCell32ValueUpdateReply;
	class DataCell64ValueUpdateReply;
	class DataCellUpdateReply;
	class DataCell32ListReply;
	class DataCell64ListReply;

	class WorldBossInfo;
	class WorldBossConfig;
	class WorldBossInfoReq;
	class WorldBossRelivenow;
	class WorldBossGoldInspire;
	class WorldBossDiamondInspire;
	class WorldBossStatisticsReq;
	class SetWorldBossAutoFightReq;
	class LeaveWorldBossReq;
	class WorldBossHpReq;

	class PlayerNpcList;
	class PlayerNpc;
	class PlayerNpcSession;

	class JoinBattleRoom;
	class LeaveBattleRoom;
	class LeaveBattleField;
	class AgreeEnterBattleField;
	class DisAgreeEnterBattleField;
	class BeginBattleField;
	class PlayerRaidOver;
	class BattleRoomInfo;
	class BattleFeildProxy;
	class BattleFieldInfo;
	class FlagOprate;
	//
	class HttpData;
	//
	class LuaRequestPacket;
	class LuaReplyPacket;
	class LuaReplyPacketList;

}
namespace ActiveModel
{
	struct FieldSqlProperty;
}
namespace Mt
{
	//  工作环境，这个在工作线程初始化时会创建
	//class MT19937;
	class client_session;
	template <typename OWNER, typename TARGET>class message_dispatcher;
	class MtPlayer;
	class MtActor;
	class MonsterBattleActor;
	class MtActorState;
	class MtBattleGround;
	class MtScene;
	class MtZone;
	class MtGuid;
	class MtConfig;
	class MtActorSkill;
	class ActorLoadWork;
	class BattleCommand;
	struct Event;
	struct SkillTarget;
	class MtItemBase;
	class MtNormalItem;
	class MtEquipItem;
	class MtGemItem;
	class MtContainer;
	class MtSkillPositionEffect;
	class MtTrap;
	class MtItemPackage;
	class PlayerAi;
	class MtMarket;
	class FriendContainer;
	class PlayerZone;
	class MessageContainer;
	class LuaInterface;
	struct NpcData;
	class MtMission;
	class MtMissionProxy;
	class MtArrangeProxy;
	class MtPlayerGuildProxy;
	class MtDataCell;
	class MtData64Cell;
	class MtCellContainer;
	class MtActivity;
	struct UniqueValue;
	class MtSimpleImpactSet;
	class MtWordManager;
	class SkillContext;
	class CMyTimer;
	class MtLuaCallProxy;
	class MtBattleGroundManager;

	namespace BuffImpl {	class Buff; }
	namespace TrapImpl { class Trap; }
	typedef std::function<void(boost::shared_ptr<MtPlayer>, boost::shared_ptr<DB::PlayerTrialInfo>)> TrialPlayerLoadDoneCallBack;
	using namespace zxero;
	//  客户端回话id，运行时唯一id
	union client_session_id;
	
	extern boost::thread_specific_ptr<MT19937> rand_gen;
	extern boost::thread_specific_ptr<ff::fflua_t>  thread_lua;
	
	template <typename Function>
	class __Defer : public boost::noncopyable {
	public:
		__Defer(Function&& f) : f_(std::forward<Function>(f)) {}
		__Defer(__Defer&& other) : f_(std::move(other.f_)) { other.available_ = false; }
		~__Defer()
		{
			if (available_) f_();
		}
	private:
		Function f_;
		bool available_ = true;
	};
	template <typename Function>
	__Defer<Function>
		Defer(Function&& f)
	{
		return __Defer<Function>(std::forward<Function>(f));
	}
	class IteratorException : public std::runtime_error
	{
	public:
		explicit IteratorException(char const* const _Message):
			std::runtime_error(_Message) {}
	};
	class ConfigException : public std::runtime_error
	{
	public:
		explicit ConfigException(char const* const _Message) :
			std::runtime_error(_Message) {}
	};
	struct item_helper
	{
		int32 item_id;
		int32 item_count;

		item_helper()
		{
			item_id = INVALID_INT32;
			item_count = 0;
		}
	};

	struct attribute_set
	{
		std::map<std::string, int32> attribute_set_;
		std::vector<int32> skill_set_;
	};

	struct UniqueValue {
		std::string name;
		uint64	value;

		bool IsInvalid() {
			if (name == "" || value == INVALID_GUID) {
				return true;
			}
			return false;
		}

		bool operator == (const UniqueValue & ct) const
		{
			return name == ct.name && value == ct.value;
		}
		bool operator != (const UniqueValue & ct) const
		{
			return !(name == ct.name && value == ct.value);
		}
	};

	//mail
	const static int32  MAX_MAIL_COUNT = 30;		//一个玩家所拥有最大的邮件数
	const static int32	MAX_MAIL_REWARD_COUNT = 5;	//邮件的奖励数
	const static int32  AUTO_DEL_MAIL_TIME = 7 * 24 * 60 * 60;	//删除邮件的时间
	enum MAIL_OPT_TYPE : int
	{
		INVAILD = -1,
		AUTO_DEL = 0,
		GET_DEL,
		ACTIVE_ADD,
		PVP_ADD,
		LOAD_ADD,
		PAYBACK_ADD,
		FRIEND_ADD,
		ITEMFULL_ADD,
		DAYPVPREWARD_ADD,
		CATCH_ADD,
		LEADERWAR_ADD,
		MAP_COUNTERATTACK,
		GUILD_ADD,
		WORLDBOSS_ADD,
		FULLBAG_ADD,
		ARENA_ADD,
		OFFLINE_HOOK_ADD,
		TOWER_ADD,
		GM_ADD =99
	};
	enum Mail_From_NPC_Guid : uint64
	{
		Mail_From_NPC_PVP = 1,
		Mail_From_ACTIVITY = 2,
		Mail_From_PayBack = 3,
	};

	//从inlist 数据中 随机挑选count个数据 outlist
	template<class T>
	void random_select_form_list(std::vector<T> &inlist, std::vector<T> &outlist, int32 count)
	{
		std::vector<T> copy_inlist;
		std::copy(inlist.begin(), inlist.end(), std::back_inserter(copy_inlist));
		int32 listcout = copy_inlist.size();
		if (listcout == 0)
		{
			return;
		}
		if (listcout <= count)
		{
			std::copy(copy_inlist.begin(), copy_inlist.end(), std::back_inserter(outlist));
		}
		else
		{
			int32 index = 0;
			T meetguid;
			for (int32 i = 0; i < count; i++)
			{
				index = rand_gen->intgen(0, listcout - i - 1);

				meetguid = copy_inlist[index];
				copy_inlist[index] = copy_inlist[listcout - i - 1];	 //从data数组末尾(这个位置在不停前移)拿一个数替换到该位置, 相当于这个元素被剔除了
				outlist.push_back(meetguid);				 //把data数组index位置的元素赋给result[i]  
			}
		}
	}
}
//message的field只能有整数和浮点数
void MessageAdd(google::protobuf::Message& lhs, zxero::real32 factor);
//message的field只能有整数和浮点数
void MessageAdd(google::protobuf::Message& lhs, const google::protobuf::Message& rhs);
//message的field只能有整数和浮点数
void MessageSub(google::protobuf::Message& lhs, const google::protobuf::Message& rhs);
/**
* \brief 设置消息初始化值, 全部都是0或者""
* \param msg
* \return void
*/
void SetMessageDefaultValue(google::protobuf::Message& msg);
//message的field只能有整数和浮点数
void MessageMulti(google::protobuf::Message& lhs, zxero::real32 factor);
// TODO: 在此处引用程序需要的其他头文件
#ifdef ZXERO_OS_WINDOWS
#define __ENTER_FUNCTION {try{
#define __LEAVE_FUNCTION }catch(...){}}
#else
#define __ENTER_FUNCTION {try{
#define __LEAVE_FUNCTION }catch(...){}}
#endif


#ifdef ZXERO_OS_WINDOWS
#define EXIT() exit(0);	
#else
#define EXIT() abort();
#endif

#define BIG_TICK 10 //bigtick 目前为10s
#define MIN_TICK 60 //MIN_TICK 目前为60s
#define INVALID_POS { -1,-1,-1 }


#endif // ZXERO_GAMESERVER_MT_TYPES_H__