#ifndef ZXERO_GAMESERVER_MT_DB_LOAD_WORK_H__
#define ZXERO_GAMESERVER_MT_DB_LOAD_WORK_H__

#include "../mt_types.h"
#include "LoginPacket.pb.h"
#include "CreatePlayerPacket.pb.h"
#include "GuildCapturePoint.pb.h"
#include "FriendMail.pb.h"
#include "ArenaDb.pb.h"
#include "dbwork.h"

namespace Mt
{
	using namespace zxero;
	
	class AccountNameVerifyWork : public dbwork
	{
		DECLARE_DBWORK();
	public:
		AccountNameVerifyWork(const boost::shared_ptr<client_session>& session,
			const boost::shared_ptr<Packet::AccountNameVerify>& message);;
		virtual void done() override;
	private:
		const boost::shared_ptr<client_session> session_;
		const boost::shared_ptr<Packet::AccountNameVerify> message_;
		Packet::AccountNameVerifyReply reply_;
	};
	class AccountRegistWork : public dbwork
	{
		DECLARE_DBWORK();
	public:
		AccountRegistWork(const boost::shared_ptr<client_session>& session,
			const boost::shared_ptr<Packet::AccountRegist>& message);;
		virtual void done() override;
	private:
		const boost::shared_ptr<client_session> session_;
		const boost::shared_ptr<Packet::AccountRegist> message_;
		Packet::AccountRegistReply reply_;
	};
	class AccountLoadWork : public dbwork
	{
		DECLARE_DBWORK();
	public:
		AccountLoadWork(const boost::shared_ptr<client_session>& session,
			const boost::shared_ptr<Packet::LoginRequest>& message);
		virtual void done() override;
	private:
		const boost::shared_ptr<client_session> session_;
		const boost::shared_ptr<Packet::LoginRequest> message_;
		Packet::LoginReply reply_;
		boost::shared_ptr<Packet::AccountModel> account_info_;
	};

  //加载当前session_ account_guid队员的玩家basic数据
	class PlayerLoginLoadTask : public dbwork
	{
		DECLARE_DBWORK();
	public:
		PlayerLoginLoadTask(const boost::shared_ptr<client_session>& session);
		virtual void done() override;
	private:
		Packet::AccountPlayerListReply reply_;
		const boost::shared_ptr<client_session> session_;
		std::vector<boost::shared_ptr<Packet::PlayerDBInfo>> player_db_infos_;
	};

  //根据guid加载某个玩家并回调
	class PlayerLoadTask : public dbwork
	{
		DECLARE_DBWORK();
	public:
		PlayerLoadTask(uint64 guid, const std::function<void(boost::shared_ptr<MtPlayer>&)>& cb);
		virtual void done() override;
	private:
		uint64 guid_;
		const std::function<void(boost::shared_ptr<MtPlayer>&)> cb_;
		boost::shared_ptr<MtPlayer> player_;
	};
	

	class CreatePlayerWork : public dbwork
	{
		DECLARE_DBWORK();
	public:
		CreatePlayerWork(const boost::shared_ptr<client_session>& session,
			const boost::shared_ptr<Packet::CreatePlayer>& message, const std::string& icon)
			:session_(session), message_(message), icon_(icon) {}
		virtual void done() override;
	private:
		const boost::shared_ptr<client_session> session_;
		const boost::shared_ptr<Packet::CreatePlayer> message_;
		Packet::Notify notify_;
		Packet::CreatePlayerReply_Result create_result_ = Packet::CreatePlayerReply_Result::CreatePlayerReply_Result_UNKNOWN_ERROR;
		boost::shared_ptr<MtPlayer> player_;
		boost::shared_ptr<Packet::PlayerDBInfo> db_info_;
		std::string icon_;
	};

	/*class PlayerStagesLoadWork : public dbwork
	{
		DECLARE_DBWORK();
	public:
		PlayerStagesLoadWork(uint64 player_guid) :player_guid_(player_guid) {}
		virtual void done() override;
	private:
		uint64 player_guid_ = INVALID_GUID;
		boost::shared_ptr<Packet::PlayerStages> data_;
	};*/

	class ActorLoadWork : public dbwork
	{
		DECLARE_DBWORK();
	public:
		ActorLoadWork(const boost::shared_ptr<MtPlayer>& player)
			:player_(player) {}
		virtual void done() override;
		std::vector<boost::shared_ptr<Packet::ActorBasicInfo>> actor_db_infos_;
		std::vector<boost::shared_ptr<Packet::ActorSkill>> actor_skill_db_infos_;
	private:
		const boost::shared_ptr<MtPlayer> player_;
	};

	class SceneStageLoader : public dbwork
	{
		DECLARE_DBWORK();
	public:
		virtual void done() override;
	};

	class DeleteOnePlayer : public dbwork
	{
		DECLARE_DBWORK();
	public:
		DeleteOnePlayer(uint64 guid, const boost::shared_ptr<client_session> session);
    virtual void done() override;
	private:
		uint64 guid_;
    const boost::shared_ptr<client_session> session_;
	};

	class ServerLoader : public dbwork
	{
		DECLARE_DBWORK();
	public:
		virtual void done() override;
	};

	class GuildLoader : public dbwork
	{
		DECLARE_DBWORK();
	public:
		virtual void done() override;
	};
	class  CapturePointLoader : public dbwork
	{
		DECLARE_DBWORK();
	public:
		CapturePointLoader() {};
		virtual void done() override;;
		std::vector<boost::shared_ptr<Packet::CapturePointDB>> cps_;
		std::vector<boost::shared_ptr<Packet::GuildCapturePointDB>> guild_cps_;
		std::vector<boost::shared_ptr<Packet::GuildCaptureBattleLogDB>> logs_;
	};

	class TopListLoader : public dbwork
	{
		DECLARE_DBWORK();
	public:
		virtual void done() override;
	};

	class LoadArenaDailyRewardGuids : public dbwork
	{
		DECLARE_DBWORK();
	public:
		LoadArenaDailyRewardGuids() {}
		virtual void done() override;
	private:
		std::vector<std::pair<int32,uint64>> rank_and_guids_;
	};

	class LoadPlayerZoneInfo : public dbwork
	{
		DECLARE_DBWORK();
	public:
		LoadPlayerZoneInfo(uint64 playerid, const std::function<void(boost::shared_ptr<PlayerZone>&)>& cb);
		virtual void done() override;
	private:
		uint64 playerid_;
		const std::function<void(boost::shared_ptr<PlayerZone>&)> cb_;
		boost::shared_ptr<PlayerZone> playerzonedata_;
	};
	 
	class LoadPlayerScenePlayerInfo : public dbwork
	{
		DECLARE_DBWORK();
	public:
		LoadPlayerScenePlayerInfo(uint64 targetid, const std::function<void(boost::shared_ptr<Packet::ScenePlayerInfo>&)>& cb);
		virtual void done() override;
	private:
		uint64 targetid_;
		const std::function<void(boost::shared_ptr<Packet::ScenePlayerInfo>&)> cb_;
		boost::shared_ptr<Packet::ScenePlayerInfo> playersceneinfo_;
	};

	class LoadPlayerScenePlayerInfoByName : public dbwork
	{
		DECLARE_DBWORK();
	public:
		LoadPlayerScenePlayerInfoByName(std::string targetname, const std::function<void(boost::shared_ptr<Packet::ScenePlayerInfo>&)>& cb);
		virtual void done() override;
	private:
		std::string targetname_;
		const std::function<void(boost::shared_ptr<Packet::ScenePlayerInfo>&)> cb_;
		boost::shared_ptr<Packet::ScenePlayerInfo> playersceneinfo_;
	};

	class LoadPlayerArenaMainData : public dbwork
	{
		DECLARE_DBWORK();
	public:
		LoadPlayerArenaMainData(const boost::shared_ptr<MtPlayer>& self,
			const boost::shared_ptr<DB::ArenaUser>& main_user,
			int32 new_score = 0);
		virtual void done() override;
	private:
		boost::shared_ptr<MtPlayer> self_;
		boost::shared_ptr<DB::ArenaUser> main_user_;
		bool copy_formation_ = false;
		int32 new_score_;
		std::vector<boost::shared_ptr<DB::ArenaUser>> targets_;
		std::vector<boost::shared_ptr<MtPlayer>> load_players_;
		DB::PlayerTrialCopy trial_copy_;
	};

	class LoadPlayerGuildApplyData : public dbwork
	{
		DECLARE_DBWORK();
	public:
		LoadPlayerGuildApplyData(const boost::shared_ptr<MtPlayer>& self, uint64 applyplayerid);
		virtual void done() override;
	private:
		boost::shared_ptr<MtPlayer> self_;
		uint64 applyplayerid_;
		boost::shared_ptr<MtPlayer> load_player_;
	};

	class WorldBossLoader : public dbwork
	{
		DECLARE_DBWORK();
	public:
		virtual void done() override;
	};

	class LoadTrialPlayerInfoWork : public dbwork
	{
		DECLARE_DBWORK();
	public:
		LoadTrialPlayerInfoWork(const boost::shared_ptr<MtPlayer>& player, 
			int32 wave, int32 player_power_count);
		virtual void done() override;
	private:
		const boost::shared_ptr<MtPlayer> player_;
		int32 wave_ = 0;
		int32 player_power_count_ = 0;
		boost::shared_ptr<DB::PlayerTrialInfo> info_;
	};

/*
	class RefreshTrialPlayerInfoWork : public dbwork
	{
		DECLARE_DBWORK();
	public:
		RefreshTrialPlayerInfoWork(const boost::shared_ptr<MtPlayer>& player,
			const boost::shared_ptr<DB::PlayerTrialInfo>& info):player_(player),info_(info){}
		virtual void done() override;
	private:
		const boost::shared_ptr<MtPlayer> player_;
		boost::shared_ptr<DB::PlayerTrialInfo> info_;
	};*/

	class UpdateStrongHoldWork : public dbwork
	{
		DECLARE_DBWORK();
	public:
		UpdateStrongHoldWork(uint64 guild_guid, const Packet::CG_UpdateStrongHold* data)
			:guild_guid_(guild_guid), data_(*data){}
		virtual void done() override;
	private:
		bool done_ = false;
		uint64 guild_guid_;
		Packet::CG_UpdateStrongHold data_;
	};

	class LoadCapturePlayerAndActor : public dbwork
	{
		DECLARE_DBWORK();
	public:
		LoadCapturePlayerAndActor(uint64 attacker_guid, uint64 defender_guid,
			int32 chapter_id, std::set<uint64>& player_guids);
		virtual void done() override;
	private:
		uint64 attacker_guid_;
		uint64 defender_guid_;
		int32 chapter_id_;
		std::set<uint64> player_guids_;
		bool success_ = false;
		std::vector<boost::shared_ptr<MtPlayer>> players_;
	};

	class LoadAllPlayerMailPay : public dbwork
	{
		DECLARE_DBWORK();
	public:
		LoadAllPlayerMailPay(std::string title, std::string content, google::protobuf::RepeatedPtrField<Packet::ItemCount> items);
		virtual void done() override;
	private:
		std::vector<uint64> allplayerguild;
		std::string title_;
		std::string content_;
		google::protobuf::RepeatedPtrField<Packet::ItemCount> items_;
	};

	//	邮件流水号获取操作
	class serial_load_work : public dbwork
	{
		DECLARE_DBWORK();

	public:
		serial_load_work(uint32 server_id);

		virtual void done();

	private:
		uint32 server_id_;
		uint64 max_serial_;
	};

	class LoadBattleReply : public dbwork
	{
		DECLARE_DBWORK();
	public:
		LoadBattleReply(uint64 reply_guid, uint64 player_guid) :reply_guid_(reply_guid), player_guid_(player_guid) {}
		virtual void done() override;
	private:
		uint64 reply_guid_ = 0;
		uint64 player_guid_ = 0;
		boost::shared_ptr<Packet::BattleReply> reply_;
	};

	class TowerInfoLoader : public dbwork
	{
		DECLARE_DBWORK();
	public:
		virtual void done() override;

	private:
		std::vector<boost::shared_ptr<DB::PlayerTowerInfo>> infos_;
	};
}
#endif // ZXERO_GAMESERVER_MT_DB_LOAD_WORK_H__
