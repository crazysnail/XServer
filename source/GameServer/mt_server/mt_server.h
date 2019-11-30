#ifndef ZXERO_GAMESERVER_MT_SERVER_H__
#define ZXERO_GAMESERVER_MT_SERVER_H__

#include "../mt_types.h"
#include "work_pool.h"
#include "date_time.h"
#include "timer_id.h"
#include <boost/thread/shared_mutex.hpp>
#include <boost/ptr_container/ptr_map.hpp>
#include <memory>
#include <atomic>
#include "mutex.h"
#include <zlib.h>
#include "random_generator.h"
#include "../mt_battle/mt_battle_ground.h"
#include "../base/mt_lua_proxy.h"

namespace Mt
{

	struct LogMsg {
		int32 head_len;
		Bytef content[1024];
	};
	using namespace zxero::network;
	class MtLuaCallProxy;
	class Server : public boost::noncopyable
	{
	public:
		static void lua_reg(lua_State* ls);
		enum save_status {
			normal_saving,
			normal_save_done,
			close_saving,
			close_save_done,
		};
		Server();
		~Server();
		void ServerInit();
		int  OnLoad();
		void Run();

		void OnNewConnection(const boost::shared_ptr<tcp_connection> conn);		
		void OnConnetionMessage(const boost::shared_ptr<tcp_connection>& conn,
			const boost::shared_ptr<google::protobuf::Message>& msg,
			zxero::timestamp_t time);

		void OnHttpConnection(const boost::shared_ptr<tcp_connection> conn);
		void OnHttpMessage(const boost::shared_ptr<tcp_connection>& conn,
			const boost::shared_ptr<Packet::HttpData>& msg,
			zxero::timestamp_t time);
		void OnHttpConnectionLoop(const boost::shared_ptr<tcp_connection>& conn);
		void OnHttpMessageLoop(const boost::shared_ptr<tcp_connection>& conn,
			const boost::shared_ptr<Packet::HttpData>& msg,
			zxero::timestamp_t time);
		void SendHttpMessage(const boost::shared_ptr<tcp_connection>&, const std::string& msg, bool bKeepAlive = false);

		void OnLogConnection(const boost::shared_ptr<tcp_connection> conn);
		void OnLogMessage(const boost::shared_ptr<tcp_connection>& conn, buffer* buf, timestamp_t st);
		void OnLogConnectionLoop(const boost::shared_ptr<tcp_connection>& conn);
		void OnLogMessageLoop(const boost::shared_ptr<tcp_connection>& conn, buffer* buf, timestamp_t st);
		void SendLogMessage( const std::string& msg);
		void SendLogMessageEx(LogMsg& msg);
		void OnSendLogMessage(const boost::shared_ptr<tcp_connection>& conn, LogMsg& msg);
		void LogServerConnector();

		/**
		* \brief 无法在Server处理的客户端消息.将消息发送给玩家
		*/
		bool UnRegistedClientMessage(const boost::shared_ptr<client_session>& session, const boost::shared_ptr<google::protobuf::Message>& message, int32 source);
		//下面这个几个消息不需要转发到场景, 因为玩家还没有进入场景
		bool OnAccountNameVerify(const boost::shared_ptr<client_session>& session, const boost::shared_ptr<Packet::AccountNameVerify>& message, int32 /*source*/);
		bool OnAccountRegist(const boost::shared_ptr<client_session>& session, const boost::shared_ptr<Packet::AccountRegist>& message, int32 /*source*/);
		bool OnAccountLogin(const boost::shared_ptr<client_session>& session, const boost::shared_ptr<Packet::LoginRequest>& message, int32 /*source*/);
		bool OnAccountPlayerList(const boost::shared_ptr<client_session>& session, const boost::shared_ptr<Packet::AccountPlayerList>& message, int32 /*source*/);
		bool OnCreatePlayer(const boost::shared_ptr<client_session>& session, const boost::shared_ptr<Packet::CreatePlayer>& message, int32 /*source*/);
		bool OnPlayerEnterGame(const boost::shared_ptr<client_session>& session, const boost::shared_ptr<Packet::PlayerEnterGameRequest>& message, int32 /*source*/);
		bool OnPrivateChatMessageReq(const boost::shared_ptr<client_session>& session, const boost::shared_ptr<Packet::PrivateChatMessageReq>& message, int32 /*source*/);

		bool OnTrialDataLoadDone(const boost::shared_ptr<MtPlayer>& plyaer, const boost::shared_ptr<DB::PlayerTrialInfo>& msg);
		//
		bool OnPlayerSceneInfoReq(const boost::shared_ptr<client_session>& session, const boost::shared_ptr<Packet::PlayerSceneInfoReq>& message, int32 /*source*/);
		//好友&邮件
		bool OnFriendInfotReq(const boost::shared_ptr<client_session>& session, const boost::shared_ptr<Packet::FriendInfotReq>& message, int32 /*source*/);
		bool OnFriendListReq(const boost::shared_ptr<client_session>& session, const boost::shared_ptr<Packet::FriendListReq>& message, int32 /*source*/);
		bool OnAddFriendReq(const boost::shared_ptr<client_session>& session, const boost::shared_ptr<Packet::AddFriendReq>& message, int32 /*source*/);
		bool OnDelFriendReq(const boost::shared_ptr<client_session>& session, const boost::shared_ptr<Packet::DelFriendReq>& message, int32 /*source*/);
		bool OnAskTeamReq(const boost::shared_ptr<client_session>& session, const boost::shared_ptr<Packet::AskTeamReq>& message, int32 /*source*/);
		bool OnAskGuildReq(const boost::shared_ptr<client_session>& session, const boost::shared_ptr<Packet::AskGuildReq>& message, int32 /*source*/);
		bool OnViewZoneReq(const boost::shared_ptr<client_session>& session, const boost::shared_ptr<Packet::ViewZoneReq>& message, int32 /*source*/);
		
		bool OnSetZoneSignatureReq(const boost::shared_ptr<client_session>& session, const boost::shared_ptr<Packet::SetZoneSignatureReq>& message, int32 /*source*/);
		
		bool OnDelClientFriendMessageNotifyRR(const boost::shared_ptr<client_session>& session, const boost::shared_ptr<Packet::DelClientFriendMessageNotifyRR>& message, int32 /*source*/);
		bool OnRecommendReq(const boost::shared_ptr<client_session>& session, const boost::shared_ptr<Packet::RecommendReq>& message, int32 /*source*/);
		bool OnFindFriendByNameReq(const boost::shared_ptr<client_session>& session, const boost::shared_ptr<Packet::FindFriendByNameReq>& message, int32 /*source*/);
		bool OnFindFriendByGuidReq(const boost::shared_ptr<client_session>& session, const boost::shared_ptr<Packet::FindFriendByGuidReq >& message, int32 /*source*/);
		void ask_recommend_req(uint64 selfguid, std::vector<uint64> &outplayer, std::vector<uint64> &myfriendplayerlist);
		//好友&邮件
		//队伍
		bool OnApplyMissionTeamReq(const boost::shared_ptr<client_session>& session, const boost::shared_ptr<Packet::ApplyMissionTeamReq>& message, int32 /*source*/);

		//
		bool OnCheckOffLine(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<S2G::CheckOffLine>& message, int32 /*source*/);
		bool OnXLuaReg(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<S2G::XLuaRegNotify>& message, int32 /*source*/);
		bool OnLuaLoad(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<S2G::LuaLoadNotify>& message, int32 /*source*/);
		bool OnCommonMessage(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<S2G::S2GCommonMessage>& message, int32 /*source*/);
		bool OnServerCommonMessage(const boost::shared_ptr<client_session>& session, const boost::shared_ptr<Packet::ServerCommonRequest>& message, int32 /*source*/);
		bool OnInsertPlayerSceneInfo(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<S2G::InsertScenePlayer>& msg, int32 /*source*/);
		bool OnGetOperateInfo(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<S2G::GetOperateInfo>& msg, int32 /*source*/);
		//lua调用
		void SendG2SCommonMessage(MtPlayer* player, const std::string &name, const std::vector<int32>& int32values, const std::vector<int64>& int64values, const std::vector<std::string>& stringvalues);
		void SendS2GCommonMessage(const std::string &name, const std::vector<int32>& int32values, const std::vector<int64>& int64values, const std::vector<std::string>& stringvalues);
		void BroadcastG2SCommonMessageToPlayer(const std::string &name, const std::vector<int32>& int32values, const std::vector<int64>& int64values, const std::vector<std::string>& stringvalues);
		void BroadcastG2SCommonMessageToScene(const std::string &name, const std::vector<int32>& int32values, const std::vector<int64>& int64values, const std::vector<std::string>& stringvalues);

		void OnDbConnected();
		void OnLoadPlayer(const boost::shared_ptr<MtPlayer>& player);
		//起服阶段,检测sharememory数据并写入DB完成的回调
		void OnStartUpSaveShareMemDataDone();
		void OnAllDataLoaded();
		static Server& Instance();
		zxero::uint32 AddDbLoadTask(zxero::work* work_);
		zxero::uint32 AddDbSaveTask(zxero::work* work_);
		zxero::uint32 AddUrlTask(zxero::work* work_);
		void RunInLoop(const boost::function<void()>& functor);
		void QueueInLoop(const boost::function<void()>& functor);
		boost::shared_ptr<MtPlayer> FindPlayer(uint64 guid);
		bool HasPlayer(uint64 guid);
		boost::shared_ptr<MtPlayer> FindPlayerByName(const std::string& name);
		boost::shared_ptr<Packet::ScenePlayerInfo> FindPlayerSceneInfo(uint64 playerguid);
		boost::shared_ptr<Packet::ScenePlayerInfo> FindPlayerSceneInfoByName(const std::string& name);
		boost::shared_ptr<Packet::ScenePlayerInfo> LoadScenePlayerInfoByGuid(boost::shared_ptr<zxero::dbcontext>& dbctx, uint64 playerguid);
		boost::shared_ptr<Packet::ScenePlayerInfo> LoadScenePlayerInfoByName(boost::shared_ptr<zxero::dbcontext>& dbctx, std::string& playername);
		void InsertScenePlayerInfo(uint64 playerid,boost::shared_ptr<Packet::ScenePlayerInfo> info);
		void GmDeletePlayer(uint64 guid);
		void AddLoadZoneInfoWork(uint64 playerid);
		void AddLoadScenePlayerInfo(uint64 targetid, const std::function<void(boost::shared_ptr<Packet::ScenePlayerInfo>&)>& cb);
		void OnShutdown();
		bool Started() const;
		void EnableServerLevel();
		int32 ServerLevel() { return server_level_; };
		int32 NextServerLvDay() { return next_lv_day_; };
		int32 GetInvestCount();
		void SetInvestCount(int32 count);
		int32 ServerId();
		bool CanUseGmCmd() const;
		void MarkPlayerCached(const uint64 guid);
		void MarkPlayerLogout(const uint64 guid);
		
		void OnMinuteTick();
		void UpdateServerLevel();
		void OnSecondTick();
		void OnTick();
		void OnBigTick();
		void OnCacheAll();
		void OnCacheSelf();
		bool OnLoadSelf(boost::shared_ptr<zxero::dbcontext>& dbctx);
		void CheckCloseSave();
		void CheckNormalSave();
		bool ForceNormalSave();
		void OnWeekTriger(int32 cur_day);
		void OnMonthTriger(int32 cur_month);
		void OnDayTriger(int32 cur_day);
		void OnHourTriger(int32 cur_hour);
		void OnMinTriger(int32 cur_min);
		void OnSecondTriger(int32 cur_second);
		std::map<uint64, boost::shared_ptr<MtPlayer>>& OnlinePlayer(){
			return online_player_;
		}
		//提供给lua用的随机数接口[min,max]
		int32 RandomNum(int32 min, int32 max) {
			return rand_gen->intgen(min, max);
		}
		void LuaLog(const std::string & context, int32 level);

		void OffLineAddSceneInfo(boost::shared_ptr<MtPlayer> player);

		void OffLine(const uint64 guid);

		MtBattleGroundManager& GetBattleManager();
		void SaveDone();
		save_status GetSaveStatus() { return save_status_; }
		void SetSaveStatus(save_status status) { save_status_ = status; }
		/**
		* \brief 场景发送消息给server, server在自己的线程调用该消息的hanlder
		*/
		void SendMessage(const boost::shared_ptr<google::protobuf::Message>& message,
			const boost::shared_ptr<MtPlayer>& player = nullptr);
		/**
		* \brief 模拟客户端发送一个包给自己的session, 用于模拟玩家操作
		*/
		void FakeClientMessage(const boost::shared_ptr<client_session>& session,
			const boost::shared_ptr<google::protobuf::Message>& message);
		bool AllLoadWorkDone();
		void KickAll(uint64 except);
		boost::shared_ptr<Packet::ServerInfo> GetServerInfo();

		void SetGuildBattleTimes(int32 times);
		int32 GetGuildBattleTimes();

		void OnMqPayOrderCallBack(boost::shared_ptr<Packet::PayOrderInfo> payorder);
		MtLuaCallProxy* GetLuaCallProxy();
	private:
		void DispatcherInit();
		bool UnRegistedS2GMessage(const boost::shared_ptr<MtPlayer>& player,
			const boost::shared_ptr<google::protobuf::Message>& message, 
			int32);
		void OnCacheAllPlayer();
		//系统后台命令回调
		void OnServerCommand();
	private:
		uint64 last_tick_;
		std::unique_ptr<event_loop> event_loop_;
		std::unique_ptr<work_pool> db_save_workers_;
		std::unique_ptr<work_pool> db_load_workers_;
		std::unique_ptr<work_pool> url_pool_workers_;
		std::unique_ptr<message_dispatcher<Server, client_session>> client_message_dispatcher_; //处理来自客户端的消息
		std::unique_ptr<message_dispatcher<Server, MtPlayer>> scene_message_dispatcher_;//处理来自场景的消息
		std::unique_ptr<protobuf_codec> codec_;
		std::unique_ptr<http_codec> http_codec_;
		std::unique_ptr<tcp_server> tcp_server_;
		std::list<tcp_connection*> queue_conn_;

		//作为客户端存在的连接池
		std::unique_ptr<tcp_client> tcp_client_;
		std::set<uint64> to_logout_players_;
		std::atomic<int32> connnection_count_{ 0 };
		int32 max_connection_count_ = 10 * 1000; //10k
		int32 max_scene_player_count_ = 1 * 1000; // 5k
		int32 max_queue_player_count_ = 2 * 1000; // 2k
		std::map<std::string, std::vector<uint64>> account_list_;
		std::map<uint64, boost::shared_ptr<MtPlayer>> online_player_;
		//一个集合，在玩家下线之后还是能查询到玩家的一些信息，用于好友系统，排行榜，帮会系统等系统，
		//用到加载，现在还没有增加删除机制
		std::map<uint64, boost::shared_ptr<Packet::ScenePlayerInfo>> player_scene_info;
		boost::shared_mutex player_shared_mutex_;
		boost::shared_mutex playersceneinfo_shared_mutex_;
		std::map<std::string, Packet::AccountModel> account_map_;
		uint64 last_search_tick_ = 0;
		int32 server_level_ = 0;
		int32 next_lv_day_ = 0;
		bool enable_level_ = true;

		save_status save_status_ = save_status::normal_save_done;
		int32 cache_times_ = 0;
		std::vector<uint64> to_cache_players_;
		network::timer_id close_save_timer_ = network::kInvalidTimerId;
		zxero::network::timer_id init_checker_timer_;
		boost::shared_ptr<Packet::ServerInfo> info_;		
		int32 m_http_server_port;
		std::unique_ptr<tcp_server> http_server_;

		//主线程战斗管理
		MtBattleGroundManager battle_manager_;

		bool programer_debug_ = false;
		bool shm_debug_ = false;
		bool closing_ = false;

		MtLuaCallProxy lua_call_;

		//大小端判定
		bool is_little_endian = false;
		bool IsLittleEndian(){
			union w {
				int a;
				char b;
			}w1;
			w1.a = 1;
			return (w1.b == 1);
		}
		int32 LittleEndian2BigInt32(int32 src) {
			int32 dest = 0;
			char* head_ptr = (char*)&dest;
			char* head_len_ptr = (char*)&src;
			head_ptr[3] = head_len_ptr[0];
			head_ptr[2] = head_len_ptr[1];
			head_ptr[1] = head_len_ptr[2];
			head_ptr[0] = head_len_ptr[3];
			return dest;
		}
  };
}

#endif // ZXERO_GAMESERVER_MT_SERVER_H__
