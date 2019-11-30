#include "mt_server.h"
#include "utils.h"
#include "log_file.h"
#include "../mt_config/mt_config.h"
#include "../mt_guid/mt_guid.h"
#include "../mt_scene/mt_scene.h"
#include "../mt_battle/mt_battle_ground.h"
#include "../mt_capture_point/mt_capture_point.h"
#include "../mt_scene/mt_scene_manager.h"
#include "../mt_actor/mt_actor_config.h"
#include "../base/mt_db_load_work.h"
#include "../mt_battle_reply/mt_battle_reply_manager.h"
#include "../mt_lua/mt_lua_initializer.h"
#include "../mt_skill/mt_skill.h"
#include "../base/mt_db_save_work.h"
#include "../base/mt_timer.h"
#include "app.h"
#include "module.h"
#include "tcp_server.h"
#include "tcp_client.h"
#include "connector.h"
#include "event_loop.h"
#include "inet_address.h"
#include "tcp_connection.h"
#include "random_generator.h"
#include "../mt_player/mt_player.h"
#include "../mt_actor/mt_actor.h"
#include "../mt_trial_field/mt_tower.h"
#include "../mt_activity/mt_activity.h"
#include "../base/client_session.h"
#include "../mt_stage/mt_scene_stage.h"
#include "../mail/static_message_manager.h"
#include "../base/client_protocol_dispatcher.h"
#include "../base/mt_game_urlwork.h"
#include "../mt_team/mt_team_manager.h"
#include "../mt_guild/mt_guild_manager.h"
#include "../mt_battle/mt_worldboss_manager.h"
#include "../mt_scene/mt_raid.h"
#include "../mt_actor/mt_actor_config.h"
#include "../mt_arena/mt_arena.h"
#include "../mt_trial_field/mt_trial_field.h"
#include "../mt_cache/mt_shm_manager.h"
#include "../mt_battle_field/mt_battle_field_manager.h"
#include "../ranklist/top_ranklist.h"
#include "../mt_chat/mt_chatmessage.h"
#include "../mt_trial_field/mt_trial_field.h"
#include "../friend/mt_friend_manager.h"
#include "../mt_activity/mt_operative_config.h"
#include "../data/luafile_manager.h"

#include "Base.pb.h"
#include "LoginPacket.pb.h"
#include "EnterScene.pb.h"
#include "protobuf_codec.h"
#include "http_codec.h"
#include "SceneCommon.pb.h"
#include "Team.pb.h"
#include "Guild.pb.h"
#include <ServerCommon.pb.h>
#include <G2SMessage.pb.h>
#include <S2GMessage.pb.h>
#include <TrialFieldDB.pb.h>
#include <ChatMessage.pb.h>
#include <EnterScene.pb.h>
#include <LimitMessage.pb.h>
#include <BattleField.pb.h>
#include <ActivityPacket.pb.h>
#include <W2GMessage.pb.h>

#include "active_model.h"

#include <google/protobuf/message.h>
#include <boost/make_unique.hpp>

namespace Mt
{
	#define SYSTEM_COMMAND_DIR "../../sys_command/"

	static Server* server = nullptr;
	using namespace zxero;
	REGISTER_MODULE(Server)
	{
		require("data_manager");
		require("MtSceneManager");
		
		register_load_function(module_base::STAGE_LOAD, boost::bind(&Server::OnLoad, boost::ref(Server::Instance())));
		register_option("log_level", "set log level 0 trace, 1 debug, 2 info, 3 warn, 4 error, 5 fatal",
			boost::program_options::value<int>()->default_value(2),
			opt_delegate<int>([](int level) {
			zxero::log_message::log_level(LogLevel(level));
		}));
#ifdef ZXERO_OS_WINDOWS
		register_option("rewrite_assert", "rewrite assert to console to avoid debug server stuck, default is false",
			boost::program_options::value<bool>()->default_value(false),
			opt_delegate<bool>([](bool rewrite) {
			if (rewrite)
				_CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_DEBUG);
		}));
#endif // ZXERO_OS_WINDOWS
	}

	void Server::lua_reg(lua_State* ls)
	{
		ff::fflua_register_t<Server, ctor()>(ls, "Server")
			.def(&Server::FindPlayer, "FindPlayer")
			.def(&Server::ServerLevel, "ServerLevel")
			.def(&Server::NextServerLvDay, "NextServerLvDay")
			.def(&Server::LuaLog,"LuaLog")
			.def(&Server::GetLuaCallProxy,"GetLuaCallProxy")
			.def(&Server::EnableServerLevel,"EnableServerLevel")
			.def(&Server::RandomNum, "RandomNum")
			.def(&Server::OnWeekTriger, "OnWeekTriger")
			.def(&Server::OnMonthTriger,"OnMonthTriger")
			.def(&Server::OnDayTriger, "OnDayTriger")
			.def(&Server::OnHourTriger, "OnHourTriger")
			.def(&Server::OnMinTriger, "OnMinTriger")
			.def(&Server::OnShutdown, "OnShutdown")
			.def(&Server::KickAll, "KickAll")
			.def(&Server::SendS2GCommonMessage,"SendS2GCommonMessage")
			.def(&Server::SendG2SCommonMessage,"SendG2SCommonMessage")
			.def(&Server::BroadcastG2SCommonMessageToPlayer,"BroadcastG2SCommonMessageToPlayer")
			.def(&Server::BroadcastG2SCommonMessageToScene, "BroadcastG2SCommonMessageToScene")
			.def(&Server::GmDeletePlayer, "GmDeletePlayer")
			.def(&Server::GetInvestCount, "GetInvestCount")
			.def(&Server::SetInvestCount, "SetInvestCount")
			.def(&Server::GetBattleManager, "GetBattleManager")
			.def(&Server::GetGuildBattleTimes,"GetGuildBattleTimes")
			.def(&Server::AddLoadZoneInfoWork,"AddLoadZoneInfoWork")
			.def(&Server::MarkPlayerCached,"MarkPlayerCached")
			.def(&Server::MarkPlayerLogout, "MarkPlayerLogout")
			.def(&Server::SendLogMessage, "SendLogMessage")
			.def(&Server::GetServerInfo,"GetServerInfo")
			;

		ff::fflua_register_t<>(ls)
			.def(&Server::Instance, "LuaServer");
	}

	void Server::DispatcherInit()
	{
		scene_message_dispatcher_ = boost::make_unique<message_dispatcher<Server, MtPlayer>>(this, event_loop_.get());
		client_message_dispatcher_ = boost::make_unique<message_dispatcher<Server, client_session>>(this, event_loop_.get());
		scene_message_dispatcher_->default_handle(&Server::UnRegistedS2GMessage);
		client_message_dispatcher_->default_handle(&Server::UnRegistedClientMessage);

		//注册 Client ----> Server 的消息处理接口
		client_message_dispatcher_->add_handler(&Server::OnAccountLogin).
			add_handler(&Server::OnCreatePlayer).
			add_handler(&Server::OnPlayerEnterGame).
			add_handler(&Server::OnAccountPlayerList).
			add_handler(&Server::OnAccountNameVerify).
			add_handler(&Server::OnAccountRegist).
			add_handler(&Server::OnPrivateChatMessageReq).
			add_handler(&Server::OnFriendInfotReq).
			add_handler(&Server::OnPlayerSceneInfoReq).
			add_handler(&Server::OnFriendListReq).
			add_handler(&Server::OnFindFriendByGuidReq).
			add_handler(&Server::OnFindFriendByNameReq).
			add_handler(&Server::OnRecommendReq).
			add_handler(&Server::OnAddFriendReq).
			add_handler(&Server::OnDelFriendReq).
			add_handler(&Server::OnAskTeamReq).
			add_handler(&Server::OnAskGuildReq).
			add_handler(&Server::OnViewZoneReq).
			add_handler(&Server::OnSetZoneSignatureReq).
			add_handler(&Server::OnDelClientFriendMessageNotifyRR).
			add_handler(&MtGuildManager::OnCreateGuildReq).
			add_handler(&MtGuildManager::OnGetGuildBonusReq).
			add_handler(&MtGuildManager::OnFindGuildReq).
			add_handler(&MtGuildManager::OnApplyGuildReq).
			add_handler(&MtGuildManager::OnOneKeyApplyGuildReq).
			add_handler(&MtGuildManager::OnAgreeApplyGuildReq).
			add_handler(&MtGuildManager::OnApplyGuildPlayerListReq).
			add_handler(&MtGuildManager::OnClearApplyGuildPlayerListReq).
			add_handler(&MtGuildManager::OnInviteyAddGuildReq).
			add_handler(&MtGuildManager::OnInviteyAgreeGuildReq).
			add_handler(&MtGuildManager::OnSetGuildNameRR).
			add_handler(&MtGuildManager::OnSetGuildNoticeRR).
			add_handler(&MtGuildManager::OnSetGuildIconRR).
			add_handler(&MtGuildManager::OnCreateGuildCDKReq).
			add_handler(&MtGuildManager::OnGetGuildCDKReq).
			add_handler(&MtGuildManager::OnGetGuildCDKListReq).
			add_handler(&MtGuildManager::OnBuildLevelUpReq).
			add_handler(&MtGuildManager::OnGetPracticeReq).
			add_handler(&MtGuildManager::OnPlayerPracticeReq).
			add_handler(&MtGuildManager::OnGetGuildListReq).
			add_handler(&MtGuildManager::OnGetGuildUserListInfoReq).
			add_handler(&MtGuildManager::OnGetGuildInfoReq).
			add_handler(&MtGuildManager::OnGetGuildBossCompetitionReq).
			add_handler(&MtGuildManager::OnGetGuildUserDamageReq).
			add_handler(&MtGuildManager::OnGetServerUserDamageReq).
			add_handler(&MtGuildManager::OnOpenGuildWetCopyReq).
			add_handler(&MtGuildManager::OnResetGuildWetCopyReq).
			add_handler(&MtGuildManager::OnGetGuildWetCopyInfosReq).
			add_handler(&MtGuildManager::GetGuildBattleList).
			add_handler(&MtGuildManager::SendGuildBattleWeekRankList).
			add_handler(&MtGuildManager::SendGuildBattleAllRankList).
			add_handler(&MtGuildManager::OnGetGuildWarehouseInfosReq).
			add_handler(&MtGuildManager::OnGetGuildWarehouseRecordReq).
			add_handler(&MtGuildManager::OnGuildWarehouseReceiveReq).
			add_handler(&MtGuildManager::OnGuildWarehouseSetReceivePlayerReq).
			add_handler(&Server::OnApplyMissionTeamReq).
			add_handler(&Server::OnServerCommonMessage).
			add_handler(&MtBattleFieldManager::OnJoinBattleRoom).
			add_handler(&MtBattleFieldManager::OnQuitBattleRoom).
			add_handler(&MtBattleFieldManager::OnQuitBattleField).
			add_handler(&MtBattleFieldManager::OnAgreeEnterBattleField).
			add_handler(&MtBattleFieldManager::OnDisAgreeEnterBattleField).
			add_handler(&MtBattleFieldManager::OnFlagOprate).
			add_handler(&MtBattleFieldManager::OnBattleFieldFightTarget).				
			add_handler(&MtTopRankManager::OnTopRankDataReq).	
			add_handler(&MtTeamManager::OnChangeTeamTypeReq).
			add_handler(&MtTeamManager::OnChangeTeamArrangeTypeReq).
			add_handler(&MtTeamManager::OnInviteyAddTeamReq).
			add_handler(&MtTeamManager::OnInviteyAgreeTeamReq).
			add_handler(&MtTeamManager::OnCreateTeam).
			add_handler(&MtTeamManager::OnDelTeam).
			add_handler(&MtTeamManager::OnPlayerApplyTeam).
			add_handler(&MtTeamManager::OnAgreeApplyTeamReq).
			add_handler(&MtTeamManager::OnPlayerLeaveTeam).
			add_handler(&MtTeamManager::OnAskLevelTeamReq).
			add_handler(&MtTeamManager::OnSetPurposeInfoReq).
			add_handler(&MtTeamManager::OnSetTeamAutoMatchReq).
			add_handler(&MtTeamManager::OnTeamInfoReq).
			add_handler(&MtTeamManager::OnFindTeamListReq).
			add_handler(&MtTeamManager::OnTeamApplyListReq).
			add_handler(&MtTeamManager::OnPlayerAutoMatchReq).
			add_handler(&MtTeamManager::OnPlayerSetTeamStatusReq).
			add_handler(&MtTeamManager::OnSetTeamLeaderReq).
			add_handler(&MtTeamManager::OnApplyTeamLeaderReq).
			add_handler(&MtTeamManager::OnAskApplyTeamLeaderReply).
			add_handler(&MtTeamManager::OnSetTeamHeroPositionReq).
			add_handler(&MtSceneStageManager::OnCollectTax).
			add_handler(&MtSceneStageManager::OnUpdateStageTax).
			add_handler(&MtSceneStageManager::OnGetSceneStageInfo).
			add_handler(&MtSceneStageManager::OnCaptureStage).
			add_handler(&MtBattleReplyManager::OnCG_GetBattleReply).
			add_handler(&MtFriendManager::OnFriendMessageReq)
			;


		//注册 Scene ----> Server 的消息处理接口
		scene_message_dispatcher_->add_handler(&MtTeamManager::OnTeamMemberChangeScene).
			add_handler(&MtTeamManager::OnCreateRobotTeam).
			add_handler(&MtTeamManager::OnChatMessage).
			add_handler(&MtTeamManager::OnPlayerAutoMatch).
			add_handler(&MtGuildManager::OnGuildChatMessage). 
			add_handler(&MtGuildManager::OnS2GEndGuildBoss).
			add_handler(&MtGuildManager::OnAddGuildMoney).
			add_handler(&Server::OnCheckOffLine).
			add_handler(&Server::OnXLuaReg).
			add_handler(&Server::OnLuaLoad).
			add_handler(&TrialFieldManager::OnRefreshTarget).
			add_handler(&MtBattleFieldManager::OnDestroyRoom).
			//add_handler(&MtBattleFieldManager::OnBuffPoint).
			add_handler(&Server::OnInsertPlayerSceneInfo).
			add_handler(&Server::OnCommonMessage).
			add_handler(&Server::OnGetOperateInfo).
			add_handler(&MtSceneStageManager::OnPlayerCaptureStage).
			add_handler(&MtSceneStageManager::OnPlayerHookDataUpdate).
			add_handler(&MtSceneStageManager::OnPlayerUpdateCaptureForm).
			add_handler(&MtCapturePointManager::OnHookStageDistributeUpdate).
			add_handler(&MtBattleReplyManager::OnSaveBattleReply).
			add_handler(&static_message_manager::s2g_worldchat_g)
			;
	}
	int Server::OnLoad()
	{
		is_little_endian = IsLittleEndian();
		LOG_INFO << "Server::OnLoadBegin, system endian is =" << is_little_endian;

		rand_gen.reset(new Mt::MT19937());
		if (thread_lua.get() == nullptr) {
			thread_lua.reset(new ff::fflua_t());
		}
		MtSkillManager::Instance().InitLuaContext();
		MtLuaInitializer::Instance().ServerDataInit(*thread_lua);

		event_loop_ = boost::make_unique<event_loop>();
		//	激活服务器
		db_save_workers_ = boost::make_unique<work_pool>(event_loop_.get(), "db_save");
		db_load_workers_ = boost::make_unique<work_pool>(event_loop_.get(), "db_load");
		url_pool_workers_ = boost::make_unique<work_pool>(event_loop_.get(), "url_pool");
		app::set_main_work(boost::bind(&Server::Run, boost::ref(Server::Instance())));

		UpdateServerLevel();

		lua_call_.Init();
		//更新主线程的ls
		MtOperativeConfigManager::Instance().FlushCacheConfig();
		return 0;
	}

	void Server::Run()
	{
		db_save_workers_->start(MtConfig::Instance().db_save_thread_count_,
			boost::bind(&MtConfig::CreateDbContext, boost::ref(MtConfig::Instance()), true));
		db_load_workers_->start(MtConfig::Instance().db_load_thread_count_,
			boost::bind(&MtConfig::CreateDbContext, boost::ref(MtConfig::Instance()), true));
		url_pool_workers_->start(MtConfig::Instance().url_thread_count_,
			boost::bind(&MtConfig::CreateUrlContext, boost::ref(MtConfig::Instance()), _1));
		codec_ = boost::make_unique<protobuf_codec>(false,
			[&](const boost::shared_ptr<tcp_connection>& conn,
				const boost::shared_ptr<google::protobuf::Message>& msg,
				zxero::timestamp_t time){
			event_loop_->run_in_loop(boost::bind(&Server::OnConnetionMessage, this, conn, msg, time));
		});
		http_codec_ = boost::make_unique<http_codec>(boost::bind(&Server::OnHttpMessage, this, _1, _2, _3));

		tcp_client_ = boost::make_unique<tcp_client>(event_loop_.get(),
			inet_address(MtConfig::Instance().log_host_, MtConfig::Instance().log_port_),
			"log_client");
		tcp_client_->connect();
		event_loop_->run_every(seconds(5), boost::bind(&Server::LogServerConnector, this));

		tcp_client_->connection_callback([&](const boost::shared_ptr<tcp_connection> & conn) {
			event_loop_->run_in_loop(boost::bind(&Server::OnLogConnection, this, conn));
		});

		tcp_server_ = boost::make_unique<tcp_server>(event_loop_.get(),
			inet_address("0.0.0.0", MtConfig::Instance().server_port_),
			"connection_server");
		max_connection_count_ = MtConfig::Instance().max_player_;
/*
		tcp_server_->connection_callback([&](const boost::shared_ptr<tcp_connection> & conn) {
			event_loop_->run_in_loop(boost::bind(&Server::OnNewConnection, this, conn));
		});*/
		tcp_server_->connection_callback([&](const boost::shared_ptr<tcp_connection> & conn) {
			event_loop_->run_in_loop(boost::bind(&Server::OnNewConnection, this, conn));
		});
		http_server_ = boost::make_unique<tcp_server>(event_loop_.get(),
			inet_address("0.0.0.0", MtConfig::Instance().http_port_),
			"http_server");
		http_server_->connection_callback([&](const boost::shared_ptr<tcp_connection> & conn) {
			event_loop_->run_in_loop(boost::bind(&Server::OnHttpConnection, this, conn));
		});


		DispatcherInit();
		app::set_exit([]() {
			server->RunInLoop(
				[]() {
				server->OnShutdown();
			});
		});
		tcp_server_->message_callback(boost::bind(&protobuf_codec::on_message, codec_.get(), _1, _2, _3));
		http_server_->message_callback(boost::bind(&http_codec::on_message, http_codec_.get(), _1, _2, _3));
		tcp_client_->message_callback(boost::bind(&Server::OnLogMessage, this, _1, _2, _3));

		// 每20秒激活一次，后台系统命令检查
		event_loop_->run_every(seconds(20), boost::bind(&Server::OnServerCommand, this));

		event_loop_->loop();
	}

	void Server:: LuaLog(const std::string & context, int32 level)
	{
		//
		//LL_TRACE,
		//LL_DEBUG,
		//LL_INFO,
		//LL_WARN,
		//LL_ERROR,
		//LL_FATAL,
		//LL_BI,
		//LL_DB,
		switch (level)
		{
		case LL_ERROR:
			LOG_ERROR << "LuaLog::" << context;
			break;
		case LL_WARN:
			LOG_WARN << "LuaLog::" << context;
			break;
		case LL_BI:
			LOG_BI << "LuaLog::" << context;
			break;
		default:
			LOG_INFO << "LuaLog::" << context;
		};
		
	}

	bool Server::UnRegistedClientMessage(
		const boost::shared_ptr<client_session>& session,
		const boost::shared_ptr<google::protobuf::Message>& message, 
		int32 /*source*/)
	{
		auto& player = session->player();
		if (player != nullptr){
			player->DispatchMessage(message);
			return true;
		}
		else {
			LOG_WARN << "transmit message " << message->GetTypeName() << " to scene. Target scene empty";
		}
		return false;
	}

	void Server::OnLogConnection(const boost::shared_ptr<tcp_connection> conn)
	{
		RunInLoop(bind(&Server::OnLogConnectionLoop, this, conn));
	}

	void Server::OnLogMessage(const boost::shared_ptr<tcp_connection>& conn, buffer* buf, timestamp_t st)
	{
		if (buf == nullptr)
			return;

		RunInLoop(bind(&Server::OnLogMessageLoop, this, conn, buf, st));
	}

	void Server::OnLogConnectionLoop(const boost::shared_ptr<tcp_connection>& conn)
	{
		if (conn->connected()) { //建立连接
			LOG_INFO << "log connection ok! " << conn->peer_address().ip_port();
		}
		else {
			//ZXERO_ASSERT(false) << "log connection error!";

			if (tcp_client_ != nullptr) {
				tcp_client_->connect();

				auto connector = tcp_client_->get_connector();
				if (connector != nullptr) {
					LOG_INFO << "ReConnectLogServer try! ip=" << connector->server_address().ip() << " port=" << connector->server_address().ip_port();
				}
			}
		}
	}

	void Server::LogServerConnector()
	{
		if (tcp_client_ != nullptr) {
			auto conn = tcp_client_->connection();
			if (conn != nullptr && conn->connected()) {
				//心跳包
				SendLogMessage("");
				//LOG_INFO << "ReConnectLogServer heart beat!";
			}
		}
	}

	void Server::OnLogMessageLoop(const boost::shared_ptr<tcp_connection>& conn, buffer* buf, timestamp_t st)
	{
		conn;
		buf;
		st;
	}

	void Server::SendLogMessage(const std::string& msg)
	{
		if (tcp_client_ == nullptr) {
			return;
		}
		auto conn = tcp_client_->connection();

		if (conn != nullptr) {
			Json::FastWriter writer;
			Json::Value root;
			try {
				root["ServerGroupId"] = MtConfig::Instance().server_group_id_;
				root["Sql"] = msg;
				uLongf blen = 0;
				LogMsg log_msg;
				if (msg != "") {

					std::string content = writer.write(root);
					uLong tlen = content.length();// +1;  //需要把字符串的结束符'\0'也一并处理

					if (tlen >= sizeof(log_msg.content)) {
						ZXERO_ASSERT(false) << "SendLogMessage failed! no enough memory! content=" << msg;
						return;
					}

					blen = compressBound(tlen); //压缩后的长度是不会超过blen的


					if (compress(log_msg.content, &blen, (const Bytef *)(content.c_str()), tlen) != Z_OK) {
						ZXERO_ASSERT(false) << "SendLogMessage failed! compress error! content=" << msg;
						return;
					}
				}
				log_msg.head_len = blen;
				RunInLoop(bind(&Server::OnSendLogMessage, this, conn, log_msg));
			}
			catch (std::exception& e)
			{
				ZXERO_ASSERT(false) << "SendLogMessage exception! " << e.what();
			}
		}
		
	}

	void Server::SendLogMessageEx(LogMsg& msg)
	{
		if (tcp_client_ == nullptr) {
			return;
		}
		auto conn = tcp_client_->connection();
		RunInLoop(bind(&Server::OnSendLogMessage, this, conn, msg));
	}

	void Server::OnSendLogMessage(const boost::shared_ptr<tcp_connection>&conn, LogMsg& msg)
	{
		if (conn != nullptr) {

			if (msg.head_len==0) {
				conn->send(&msg, sizeof(msg.head_len));
			}
			else {
				//长度的内容存在大小端问题!!!
				int32 len = sizeof(msg.head_len) + msg.head_len;
				//msg.head_len = htonl(len);
				conn->send(&msg, len);
			}
		}
	}

	void Server::OnNewConnection(const boost::shared_ptr<tcp_connection> conn)
	{
		if (conn->connected()) { //建立连接
			LOG_INFO << "Incoming Connection " << conn
				<< conn->peer_address().ip_port() << 
				". current connection:" << connnection_count_;
			connnection_count_++;
			auto session = boost::make_shared<client_session>(conn, *codec_.get());
			conn->context(session);
			event_loop_->run_after(seconds(20), [=]() {//20秒后还未验证成功, 断开连接
				if (session->get_state() == Mt::client_session_state::kVerifing) {
					session->close(true);
				}
			});
			session->state(client_session_state::kVerifing); //进入验证状态
			if (connnection_count_ >= max_connection_count_ || programer_debug_ ) { //超过最大限制了 需要排队
				Packet::ServerFull notify;
				notify.set_queue_user_count(queue_conn_.size());
				codec_->send(conn, notify);
				
				queue_conn_.push_back(conn.get());
				LOG_WARN << "max connection:" << connnection_count_ << " reached. queue player:" << queue_conn_.size();
				session->state(Mt::client_session_state::kInQueue);;
				event_loop_->run_after(seconds(10), [=]() {
					session->close(true); // 先关了,	以后再实现排队
				});
			}
		}
		else {//断开连接
			auto& context = conn->context();
			queue_conn_.remove_if([&](tcp_connection* c) {return c == conn.get();});
			if (context.empty()) { //逻辑层没有建立完整的连接, 可以直接关闭
				conn->shutdown();
				LOG_ERROR << "connection closed:" << conn->peer_address().ip_port();
				return;
			}
			connnection_count_--;
			ZXERO_ASSERT(connnection_count_ >= 0) << "connection close more times!";
			LOG_INFO << "Client Close Session:" << conn->peer_address().ip_port() << ". current connection:" << connnection_count_;
			try
			{
				auto& session_ = boost::any_cast<const boost::shared_ptr<client_session>&>(context);
				if (session_->player() != nullptr) {
					auto player = session_->player();
					if (player->GetScenePlayerInfo()->player_status() == Packet::PlayerSceneStatus::WAIT_FOR_ENTER) {
						//onsave之后会自动erase掉
					} else if (player->LogoutStatus()== eLogoutStatus::eOnLine) {
						//避免冗余的断线msg
						player->DispatchMessage(boost::make_shared<Packet::PlayerDisconnect>());
					}
					boost::shared_ptr<MtPlayer> empty(nullptr);
					session_->bind_player(empty);
					player->Session(nullptr);
				}
				session_->close(true);
				conn->context().clear();
			}
			catch (boost::bad_any_cast& e)
			{
				e;
				conn->force_close();
				conn->context().clear();
			}
			return;
		}
	}

	bool Server::OnAccountLogin(const boost::shared_ptr<client_session>& session, const boost::shared_ptr<Packet::LoginRequest>& message, int32 /*source*/)
	{
		//auto& player = session->player();
		//if (player == nullptr)
		//{
		//	ZXERO_ASSERT(false) << "player status invalid";
		//	return true;
		//}
		
		if (message->type() == Packet::AccountType::THIRD)
		{
			if (MtConfig::Instance().serverGroupDB_.GroupState == 2)
			{
				Packet::Notify notify;
				notify.set_notify(MtConfig::Instance().serverGroupDB_.MaintainMessage);
				notify.set_show_type(1);
				session->send(notify);
				return true;
			}
			auto ServerDb = MtConfig::Instance().GetServerItem(message->partnerid(),MtConfig::Instance().server_group_id_);
			if (ServerDb == nullptr)
			{
				ZXERO_ASSERT(false) << "no Server data " << MtConfig::Instance().server_group_id_;
				return true;
			}
			if (message->gameversionid() < ServerDb->MinGameVersionID)
			{
				auto Partner =  MtConfig::Instance().GetPartnerItem(message->partnerid());
				if (Partner != nullptr)
				{
					Packet::CommonReply reply;
					reply.set_reply_name("NewApkNotify");
					reply.add_string_params("NewApkNotify");
					reply.add_string_params(Partner->GameVersionUrl);
					session->send(reply);
					LOG_ERROR << "NewApkNotify check gameversionid,"<< message->gameversionid()<<","<< ServerDb->MinGameVersionID<<","<< Partner->GameVersionUrl;
				}
				else
				{
					ZXERO_ASSERT(false) << "no Partner data " << message->partnerid()<<","<< message->gameversionid() << "," << ServerDb->MinGameVersionID;
				}
				
				return true;
			}
			
			url_pool_workers_->add_work(new AccountLoginWork(session, message));
		}
		else
		{
			db_load_workers_->add_work(new AccountLoadWork(session, message));
		}
		return true;
	}

	bool Server::OnCreatePlayer(const boost::shared_ptr<client_session>& session, const boost::shared_ptr<Packet::CreatePlayer>& message, int32 /*source*/)
	{
		auto account = session->get_account();
		if (account == nullptr) {
			return true;
		}
		auto config = MtActorConfig::Instance().GetBaseConfig(message->player_actor_base_id());
		Packet::CreatePlayerReply reply;
		if (config == nullptr) {
			reply.set_result(Packet::CreatePlayerReply_Result::CreatePlayerReply_Result_INVALID_CONFIG);
			session->send(reply);
			return true;
		}
		const std::string::size_type MIN_PLAYER_NAME_LEN = 6;
		const std::string::size_type MAX_PLAYER_NAME_LEN = 32;
		if (message->name().size() < MIN_PLAYER_NAME_LEN || message->name().size() > MAX_PLAYER_NAME_LEN) {
			reply.set_result(Packet::CreatePlayerReply_Result::CreatePlayerReply_Result_INVALID_NAME);
			session->send(reply);
			Packet::Notify notify;
			notify.set_notify("InvalidPlayerName");
			session->send(notify);
			return true;
		}
		db_load_workers_->add_work(new CreatePlayerWork(session, message, config->icon));

		LOG_INFO << "create player worker created, account:" << account->guid();
		return true;
	}

	bool Server::OnPlayerEnterGame(const boost::shared_ptr<client_session>& session,
		const boost::shared_ptr<Packet::PlayerEnterGameRequest>& message,
		int32 /*source*/)
	{

		if (session->player() == nullptr) {//第一次发送, 请求进入场景
			auto player = FindPlayer(message->guid());
			if (player == nullptr) {//从db中加载玩家后再重新调用EnterGame
				db_load_workers_->add_work(
					new PlayerLoadTask(message->guid(),
						[=](auto& /*p*/) { Server::Instance().OnPlayerEnterGame(session, message, MessageSource::GAME_EXECUTE); })
				);
			} 
			else {
			  auto old_session = player->Session();
			  if (old_session) {
					player->SendClientNotify("new_login_client", -1, 1);
					boost::shared_ptr<MtPlayer> empty(nullptr);
					old_session->bind_player(empty);
					old_session->close(true);
			  }
			  session->bind_player(player);
			  player->Session(session);
			  player->Refresh2ClientActorList();
			  auto scene = MtSceneManager::Instance().GetSceneById(player->GetSceneId());
			  if (scene == nullptr) {
			    scene = MtSceneManager::Instance().DefaultScene(player->GetCamp());
			    if (scene == nullptr) {
			      ZXERO_ASSERT(false) << "default player scene is null! player guid=" << player->Guid() << " player scene id=" << player->GetSceneId();
			      session->close(true);
				  return true;
			    }
			  }		

			  ///////////////

			  if (player->Scene() == scene) {//下线后,玩家还在场景中待10分钟, 这期间再次上线, 或者账号被他人使用了
				player->DispatchMessage(boost::make_shared<Packet::SessionChanged>());
				return true;
			  }

			  // 副本场景是不允许直接传送的 [4/18/2017 SEED]
			  if (MtScene::IsRaidScene(player->GetSceneId())) {
				  scene = MtSceneManager::Instance().DefaultScene(player->GetCamp());
				  if (scene == nullptr || scene->Config() == nullptr) {
					  ZXERO_ASSERT(false) << "default player scene is null! player guid=" << player->Guid() << " player scene id=" << player->GetSceneId();
					  session->close(true);
					  return true;
				  }
				  player->Position(scene->Config()->spawn_pos(0));
			  }
			  player->GetScenePlayerInfo()->set_player_status(Packet::PlayerSceneStatus::WAIT_FOR_ENTER);
			  auto enter_msg = boost::make_shared<Packet::PlayerEnterScene>();
			  enter_msg->set_scene_id(scene->SceneId());
			  auto initpos = enter_msg->mutable_init_pos();
			  //initpos->CopyFrom(player->Position());
			  //强制出生点吧，免得掉地下
			  initpos->CopyFrom(scene->SpawnPos(0));
			  scene->DispatchMessage(enter_msg, player, MessageSource::GAME_EXECUTE);
			  return true;
			}
		}
		else
		{//连续发送或切换场景
			LOG_INFO << "session [" << session.get() << "] repeat enter game message";
		}
		return true;
	}

	bool Server::OnPlayerSceneInfoReq(const boost::shared_ptr<client_session>& session, const boost::shared_ptr<Packet::PlayerSceneInfoReq>& message, int32 /*source*/)
	{
		auto player = session->player();
		if (player == nullptr) {
			ZXERO_ASSERT(false);
			return true;
		}
		uint64 targetguid = message->guid();
		auto targetplayer = FindPlayer(targetguid);
		if (targetplayer != nullptr)
		{
			Packet::PlayerSceneInfoReply reply;
			targetplayer->FillScenePlayerInfo(*reply.mutable_optinfo());
			player->SendMessage(reply);
		}
		else
		{
			auto targetsceneinfo =Server::Instance().FindPlayerSceneInfo(targetguid);
			if (targetsceneinfo != nullptr)
			{
				Packet::PlayerSceneInfoReply reply;
				reply.mutable_optinfo()->CopyFrom(*targetsceneinfo.get());
				player->SendMessage(reply);
			}
		}
		return true;
	}

	bool Server::OnFriendInfotReq(const boost::shared_ptr<client_session>& session, const boost::shared_ptr<Packet::FriendInfotReq>& message, int32 /*source*/)
	{
		auto player = session->player();
		if (player == nullptr) {
			ZXERO_ASSERT(false);
			return true;
		}
		message;
		auto player_friend_c = player->Friend_Container();
		if (player_friend_c == nullptr)
		{
			ZXERO_ASSERT(false);
			return true;
		}
		//fill send FriendInfotReply
		Packet::FriendInfotReply reply;
		player->fill_friend_protocol(reply);
		player->SendMessage(reply);
		return true;
	}

	bool Server::OnFriendListReq(const boost::shared_ptr<client_session>& session, const boost::shared_ptr<Packet::FriendListReq>& message, int32 /*source*/)
	{
		message;
		auto player = session->player();
		if (player == nullptr) {
			ZXERO_ASSERT(false);
			return true;
		}
		player->send_friendlistupdate();
		return true;
	}
	bool Server::OnRecommendReq(const boost::shared_ptr<client_session>& session, const boost::shared_ptr<Packet::RecommendReq>& message, int32 /*source*/)
	{
		message;
		auto player = session->player();
		if (player == nullptr) {
			ZXERO_ASSERT(false);
			return true;
		}
		Packet::RecommendsUpdate reply;
		player->fill_recommend_protocol(reply);
		player->SendMessage(reply);
		return true;
	}
	bool Server::OnFindFriendByNameReq(const boost::shared_ptr<client_session>& session, const boost::shared_ptr<Packet::FindFriendByNameReq>& message, int32 /*source*/)
	{
		auto player = session->player();
		if (player == nullptr) {
			ZXERO_ASSERT(false);
			return true;
		}
		std::string playername = message->name();
		if (last_search_tick_ + (uint64)milliseconds(100).total_milliseconds()
			>= MtTimerManager::Instance().CurrentTime()) {
			player->SendCommonResult(Packet::ResultOption::Friend_Opt, Packet::ResultCode::GameServerBusy);
			return true;
		}
		last_search_tick_ = MtTimerManager::Instance().CurrentTime();
		auto target_player_ = FindPlayerByName(playername);
		if (target_player_ != nullptr)
		{
			Packet::FindFriendReply reply;
			MtFriendManager::Fill_FindFriendInfo_SceneInfo(*(reply.mutable_info()), target_player_->GetScenePlayerInfo());
			player->SendMessage(reply);
			return true;
		}
		else
		{
			auto SendFriendReply = [=](boost::shared_ptr<Packet::ScenePlayerInfo>& psi) {
				if (psi)
				{
					Packet::FindFriendReply reply;
					MtFriendManager::Fill_FindFriendInfo_SceneInfo(*(reply.mutable_info()), &psi->basic_info());
					player->SendMessage(reply);
				}
				else
				{

				}
			};
			auto target_player_sceneinfo = FindPlayerSceneInfoByName(playername);
			if (target_player_sceneinfo)
			{
				SendFriendReply(target_player_sceneinfo);
			}
			else
			{
				db_load_workers_->add_work(new LoadPlayerScenePlayerInfoByName(playername, SendFriendReply));
			}
		}

		return true;
	}
	bool Server::OnFindFriendByGuidReq(const boost::shared_ptr<client_session>& session, const boost::shared_ptr<Packet::FindFriendByGuidReq >& message, int32 /*source*/)
	{
		auto player = session->player();
		if (player == nullptr) {
			ZXERO_ASSERT(false);
			return true;
		}
		
		uint64 playerguid = message->guid();
		auto target_player_=FindPlayer(playerguid);
		if (target_player_ != nullptr)
		{
			Packet::FindFriendReply reply;
			MtFriendManager::Fill_FindFriendInfo_SceneInfo(*(reply.mutable_info()), target_player_->GetScenePlayerInfo());
			player->SendMessage(reply);
			return true;
		}
		else
		{
			auto SendFriendReply = [=](boost::shared_ptr<Packet::ScenePlayerInfo>& psi) {
				if (psi)
				{
					Packet::FindFriendReply reply;
					MtFriendManager::Fill_FindFriendInfo_SceneInfo(*(reply.mutable_info()), &psi->basic_info());
					player->SendMessage(reply);
				}
				else
				{

				}
			};
			auto target_player_sceneinfo = FindPlayerSceneInfo(playerguid);
			if (target_player_sceneinfo)
			{
				SendFriendReply(target_player_sceneinfo);
			}
			else
			{
				db_load_workers_->add_work(new LoadPlayerScenePlayerInfo(playerguid, SendFriendReply));
			}
		}
		
		return true;
	} 

	void Server::ask_recommend_req(uint64 selfguid,std::vector<uint64> &outplayer, std::vector<uint64> &myfriendplayerlist)
	{
		for (auto& player : online_player_)
		{
			if (outplayer.size() >= 100)
				return;
			uint64 playerguid = player.first;
			if (selfguid != playerguid && std::find(outplayer.begin(), outplayer.end(), playerguid) == outplayer.end() && std::find(myfriendplayerlist.begin(), myfriendplayerlist.end(), playerguid) == myfriendplayerlist.end())
			{
				outplayer.push_back(playerguid);
			}
		}
	}

	bool Server::OnAddFriendReq(const boost::shared_ptr<client_session>& session, const boost::shared_ptr<Packet::AddFriendReq>& message, int32 /*source*/)
	{
		auto player = session->player();
		if (player == nullptr) {
			ZXERO_ASSERT(false);
			return true;
		}
		uint64 playerguid = message->player_guid();
		auto target_player = FindPlayer(playerguid);
		if (target_player != nullptr)
		{
			Packet::ResultCode ret = player->add_friend(playerguid, target_player->GetScenePlayerInfo());
			player->SendCommonResult(Packet::ResultOption::Friend_Opt, ret);
		}
		else
		{
			auto AddFriendFunc = [=](boost::shared_ptr<Packet::ScenePlayerInfo>& spi)
			{
				if (spi)
				{
					Packet::ResultCode ret = player->add_friend(playerguid, &spi->basic_info());
					player->SendCommonResult(Packet::ResultOption::Friend_Opt, ret);
				}
			};
			auto target_player_scene = FindPlayerSceneInfo(playerguid);
			if (target_player_scene != nullptr)
			{
				AddFriendFunc(target_player_scene);
			}
			else
			{
				db_load_workers_->add_work(new LoadPlayerScenePlayerInfo(playerguid, AddFriendFunc));
			}
		}
		return true;
	}

	bool Server::OnDelFriendReq(const boost::shared_ptr<client_session>& session, const boost::shared_ptr<Packet::DelFriendReq>& message, int32 /*source*/)
	{
		auto player = session->player();
		if (player == nullptr) {
			ZXERO_ASSERT(false);
			return true;
		}
		
		uint64 playerguid = message->player_guid();
		auto ret = player->del_friend(playerguid);
		player->SendCommonResult(Packet::ResultOption::Friend_Opt, ret);
		return true;
	}
	bool Server::OnAskTeamReq(const boost::shared_ptr<client_session>& session, const boost::shared_ptr<Packet::AskTeamReq>& message, int32 /*source*/)
	{
		message;
		auto player = session->player();
		if (player == nullptr) {
			ZXERO_ASSERT(false);
			return true;
		}
		return true;
	}

	bool Server::OnAskGuildReq(const boost::shared_ptr<client_session>& session, const boost::shared_ptr<Packet::AskGuildReq>& message, int32 /*source*/)
	{
		message;
		session;
		return true;
	}

	bool Server::OnViewZoneReq(const boost::shared_ptr<client_session>& session, const boost::shared_ptr<Packet::ViewZoneReq>& message, int32 /*source*/)
	{
		auto player = session->player();
		if (player == nullptr) {
			ZXERO_ASSERT(false);
			return true;
		}

		uint64 targer_guid=message->player_guid();
		
		auto OnViewPlayerZoneFunc = [=] (const boost::shared_ptr<PlayerZone>& z,bool load = true){
			if (player != nullptr && z != nullptr )
			{
				if (load)
				{
					MtFriendManager::Instance().AddZoneInfo(targer_guid, z);
				}
				auto target_player_ = FindPlayer(targer_guid);
				if (target_player_ != nullptr)//当玩家在内存里时
				{
					Packet::ViewZoneReply reply;
					MtFriendManager::Fill_ViewZoneInfo_Player(reply, target_player_.get(), z.get());
					player->SendMessage(reply);
				}
				else
				{
					auto OnViewPlayerZoneBySceneFunc = [=](boost::shared_ptr<Packet::ScenePlayerInfo>& psi) {
						if (psi)
						{
							Packet::ViewZoneReply reply;
							MtFriendManager::Fill_ViewZoneInfo_SceneInfo(reply, psi.get(), z.get());
							player->SendMessage(reply);
						}
					};
					auto playersceneinfo = FindPlayerSceneInfo(targer_guid);
					if (playersceneinfo)//当玩家只有sceneinfo时
					{
						OnViewPlayerZoneBySceneFunc(playersceneinfo);
					}
					else
					{
						db_load_workers_->add_work(new LoadPlayerScenePlayerInfo(targer_guid, OnViewPlayerZoneBySceneFunc));
					}
				}
			}
		};
		auto zoneinfo = MtFriendManager::Instance().GetPlayerZone(targer_guid);
		if (zoneinfo == nullptr)
		{
			db_load_workers_->add_work(new LoadPlayerZoneInfo(targer_guid, OnViewPlayerZoneFunc));
		}
		else
		{
			OnViewPlayerZoneFunc(zoneinfo,false);
		}
		return true;
	}
	
	bool Server::OnSetZoneSignatureReq(const boost::shared_ptr<client_session>& session, const boost::shared_ptr<Packet::SetZoneSignatureReq>& message, int32 /*source*/)
	{
		auto player = session->player();
		if (player == nullptr) {
			ZXERO_ASSERT(false);
			return true;
		}
		std::string signature = message->signature();
		player->set_zone_signature(signature);
		return true;
	}

	bool Server::OnDelClientFriendMessageNotifyRR(const boost::shared_ptr<client_session>& session, const boost::shared_ptr<Packet::DelClientFriendMessageNotifyRR>& message, int32 /*source*/)
	{
		auto player = session->player();
		if (player == nullptr) {
			ZXERO_ASSERT(false);
			return true;
		}
		player->del_notify_playerinfo(message->source_guid());
		return true;
	}

	bool Server::OnApplyMissionTeamReq(const boost::shared_ptr<client_session>& session, const boost::shared_ptr<Packet::ApplyMissionTeamReq>& message, int32 /*source*/)
	{
		auto player = session->player();
		if (player == nullptr) {
			ZXERO_ASSERT(false);
			return true;
		}
		auto src_player = FindPlayer(message->srcplayer_guid());
		if (src_player == nullptr)
		{
			player->SendCommonResult(Packet::ResultOption::Team_Opt, Packet::ResultCode::PlayerNoExist);
			return true;
		}
		int32 missionid = message->missionid();
		if (missionid != 0)
		{
			auto player_mission = src_player->GetMissionById(missionid);
			if (player_mission == nullptr)
			{
				player->SendCommonResult(Packet::ResultOption::Team_Opt, Packet::ResultCode::MISSION_NOT_EXIT);
				return true;
			}
		}

		uint64 teamid = src_player->GetTeamID();
		if (teamid == INVALID_GUID)
		{
			player->SendCommonResult(Packet::ResultOption::Team_Opt, Packet::ResultCode::Team_NoExist);
			return true;
		}
		MtTeamManager::Instance().PlayerApplyTeam(player.get(), teamid);
		return true;
	}

	void Server::SetGuildBattleTimes(int32 times)
	{
		info_->set_guildbattle_times(times);
	}
	int32 Server::GetGuildBattleTimes()
	{
		return info_->guildbattle_times();
	}

	bool Server::OnCheckOffLine(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<S2G::CheckOffLine>& message, int32 /*source*/)
	{
		player;
		//真正的离线玩家处理策略
		for (int i = 0; i < message->guid_size(); i++) {
			if (online_player_.find(message->guid(i)) != online_player_.end()) {
				auto online_player = online_player_[message->guid(i)];
				if ((online_player->LogoutStatus() == eLogoutStatus::eOffLine 
					&& online_player->CacheStamp() == message->stamp(i) )) {
					OffLineAddSceneInfo(online_player);
					LOG_INFO << "[OnCheckOffLine],player:" << online_player->Guid();
					OffLine(message->guid(i));
				}
			}
		}
		return true;
	}

	void Server::OffLineAddSceneInfo(boost::shared_ptr<MtPlayer> player)
	{
		if (player)
		{
			boost::shared_ptr<Packet::ScenePlayerInfo> sceneinfo = boost::make_shared<Packet::ScenePlayerInfo>();
			if (sceneinfo != nullptr)
			{
				player->FillScenePlayerInfo(*sceneinfo.get());
				InsertScenePlayerInfo(player->Guid(), sceneinfo);
			}
		}
	}
	bool Server::OnXLuaReg(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<S2G::XLuaRegNotify>& message, int32 /*source*/)
	{
		message;
		player;
		lua_call_.Init();
		//更新主线程的ls
		MtOperativeConfigManager::Instance().FlushCacheConfig();

		LOG_INFO << "server OnXLuaReg done! ";
		return true;
	}
	bool Server::OnLuaLoad(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<S2G::LuaLoadNotify>& message, int32 /*source*/)
	{
		player;
		if (message->ids_size() == 0) {
			thread_lua->load_all();
			LOG_INFO << "server OnLuaLoad done! ";
		}
		else {
			for (auto id : message->ids()) {
				thread_lua->reload(id);
				LOG_INFO << "server OnLuaLoad done! "<< ". id:" << id;
			}
		}
		//更新主线程的ls
		MtOperativeConfigManager::Instance().FlushCacheConfig();

		return true;
	}
	
	bool Server::OnCommonMessage(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<S2G::S2GCommonMessage>& message, int32 /*source*/)
	{		
		player;
		//player不是必须的，player可以是null
		try {
			thread_lua->call<int32>(10002, "HandleS2GCommonMessage", this, message);
			return true;
		}
		catch (ff::lua_exception& e) {
			ZXERO_ASSERT(false) << "lua exception:" << e.what();
			return true;
		}
	}

	bool Server::OnServerCommonMessage(const boost::shared_ptr<client_session>& session, const boost::shared_ptr<Packet::ServerCommonRequest>& message, int32 /*source*/)
	{
		auto player = session->player();
		if (player == nullptr) {
			ZXERO_ASSERT(false);
			return true;
		}
		try {
			thread_lua->call<int32>(10002, "HandleServerCommonMessage", this, player, message);
			return true;
		}
		catch (ff::lua_exception& e) {
			ZXERO_ASSERT(false) << "lua exception:" << e.what();
			return true;
		}
	}
	bool Server::OnInsertPlayerSceneInfo(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<S2G::InsertScenePlayer>& msg, int32 /*source*/)
	{
		if (msg == nullptr || player == nullptr) {
			return true;
		}

		boost::shared_ptr<Packet::ScenePlayerInfo> sceneplayer = boost::make_shared<Packet::ScenePlayerInfo>();
		sceneplayer->CopyFrom(*(msg->mutable_msg()));
		InsertScenePlayerInfo(player->Guid(), sceneplayer);
		return true;
	}
	bool Server::OnGetOperateInfo(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<S2G::GetOperateInfo>& msg, int32 /*source*/)
	{
		if (msg == nullptr || player == nullptr) {
			return true;
		}
		auto targetguid = msg->playerid();
		auto call_funcyion = [=](auto& p) {
			if (p && player)
			{
				uint64 guildid = p->basic_info().guildid();
				auto guilduser = MtGuildManager::Instance().GetGuildUser(guildid, targetguid);
				Packet::GuildPosition guildposition = Packet::GuildPosition::TRAINEE;
				if (guilduser)
				{
					guildposition = guilduser->position();
				}
				Packet::PlayerOperateInfoReply reply;
				MtPlayer::FillPlayerOperateInfoBySceneInfo(*reply.mutable_optinfo(), targetguid, p.get(), guildposition);
				player->SendMessage(reply);
			}
		};

		auto targetsceneinfo = FindPlayerSceneInfo(targetguid);
		if (targetsceneinfo)
		{
			call_funcyion(targetsceneinfo);
		}
		else
		{
			db_load_workers_->add_work(new LoadPlayerScenePlayerInfo(targetguid, call_funcyion));
		}
		return true;
	}
	
	void Server::InsertScenePlayerInfo(uint64 playerid, boost::shared_ptr<Packet::ScenePlayerInfo> info)
	{
		auto sceneplayer = FindPlayerSceneInfo(playerid);
		{
			boost::shared_lock<boost::shared_mutex> l(playersceneinfo_shared_mutex_);
			if (sceneplayer != nullptr)
			{
				sceneplayer->CopyFrom(*(info.get()));
				return ;
			}
			sceneplayer = boost::make_shared<Packet::ScenePlayerInfo>();
			sceneplayer->CopyFrom(*(info.get()));
			player_scene_info.insert({ playerid ,sceneplayer });
		}
		
	}
	Server::Server() 
	{
		server = this;
		info_ = boost::make_shared<Packet::ServerInfo>();
	}
	
	void Server::ServerInit()
	{
		info_->set_serverid(MtConfig::Instance().server_id_);
		info_->set_db_refresh_time(0);
		info_->set_active_gm_command(false);
		info_->set_invest_count(0);
		info_->set_guildbattle_times(0);

		auto count = info_->GetDescriptor()->FindFieldByName("acr_data")->options().GetExtension(Packet::column_count);
		for (int32 i = 0; i < count; i++) {
			info_->add_acr_data(0);
		}

	}
	Server& Server::Instance()
	{
		return  *server;
	}

	zxero::uint32 Server::AddDbLoadTask(zxero::work* work_)
	{
		return db_load_workers_->add_work(work_);
	}

	zxero::uint32 Server::AddDbSaveTask(zxero::work* work_)
	{
		return db_save_workers_->add_work(work_);
	}

	zxero::uint32 Server::AddUrlTask(zxero::work* work_)
	{
		return url_pool_workers_->add_work(work_);
	}

	void Server::OnLoadPlayer(const boost::shared_ptr<MtPlayer>& player)
	{
		LOG_INFO << "[Server LoadPlayer], player:" << player->Guid()
			<< ",addr:" << player.get()
			<< ",fresh:" << (online_player_.find(player->Guid()) == online_player_.end());
		if (player->MainActor() == nullptr) {
			ZXERO_ASSERT(false) << "main actor is null ! player guid=" << player->Guid();
			return;
		}
		boost::upgrade_lock<boost::shared_mutex> g(player_shared_mutex_);
		if (online_player_.find(player->Guid()) == online_player_.end()) {
			online_player_.insert({ player->Guid(), player });
		}
		else {
			LOG_INFO << "player already in memory" << player->Guid();
		}
	}


	void Server::RunInLoop(const boost::function<void()>& functor)
	{
		event_loop_->run_in_loop(functor);
	}
	void Server::QueueInLoop(const boost::function<void()>& functor)
	{
		event_loop_->queue_in_loop(functor);
	}
	void Server::AddLoadZoneInfoWork(uint64 playerid)
	{
		auto addzoneinfo = [=](auto &z) {
			if (z)
			{
				MtFriendManager::Instance().AddZoneInfo(playerid, z);
			}
		};
		db_load_workers_->add_work(new LoadPlayerZoneInfo(playerid, addzoneinfo));
	}
	
	void Server::AddLoadScenePlayerInfo(uint64 targetid, const std::function<void(boost::shared_ptr<Packet::ScenePlayerInfo>&)>& cb)
	{
		db_load_workers_->add_work(new LoadPlayerScenePlayerInfo(targetid, cb));
	}
	void Server::GmDeletePlayer(uint64 guid)
	{
		auto player_it = online_player_.find(guid);
		LOG_INFO << "[gm delete], player:" << guid
			<< ",empty:" << (player_it == online_player_.end());
		if (player_it == online_player_.end())
			return;
		auto player = player_it->second;
		OffLineAddSceneInfo(player);
		online_player_.erase(guid);
		auto msg = player->GetDBPlayerInfo();
		msg->SetDeleted();
		MtShmManager::Instance().Cache2Shm(msg.get(),guid);
		auto session = player->Session();
		session->bind_player(nullptr);
		player->Session(nullptr);
		db_load_workers_->add_work(new DeleteOnePlayer(guid, session));
	}

	void Server::CheckNormalSave()
	{
		cache_times_++;
		if (cache_times_ >= 4) {//120s

			//////
			LOG_INFO << "[#OnNormalSave#] save_status_=" << save_status_;

			if (save_status_ != normal_save_done) {
				return;
			}
			save_status_ = normal_saving;
			db_save_workers_->add_work(new MessageSaveAllWork());

			/////////////
			cache_times_ = 0;
		}
	}

	bool Server::ForceNormalSave()
	{
		//////
		LOG_INFO << "[#ForceNormalSave#] save_status_=" << save_status_;
		if (save_status_ != normal_save_done) {
			return false;
		}
		save_status_ = normal_saving;
		db_save_workers_->add_work(new MessageSaveAllWork());
		/////////////
		cache_times_ = 0;
		return true;
	}

	void Server::CheckCloseSave()
	{		
		if (save_status_ == save_status::normal_save_done)
		{
			LOG_INFO << "[#OnCloseSave#] save_status_=" << save_status_;
					
			save_status_ = close_saving;

			//强制缓存一次数据，保证关服最新！这里就做异步player处理了，因为scene线程已经停了
			MtShmManager::Instance().OnCacheShmBegin();
			MtSceneStageManager::Instance().OnCacheAll();
			MtCapturePointManager::Instance().OnCacheAll();
			MtGuildManager::Instance().OnCacheAll();
			MtWorldBossManager::Instance().OnCacheAll();
			MtArenaManager::Instance().OnCacheAll();
			TrialFieldManager::Instance().OnCacheAll();
			MtTowerManager::Instance().OnCacheAll();
			MtFriendManager::Instance().OnCacheAll();
			MtBattleReplyManager::Instance().OnCacheAll();
			static_message_manager::Instance().OnCacheAll();
			OnCacheSelf();
			for (auto child : online_player_) {
				child.second->OnCache2Shm();
			}		
			online_player_.clear();
			MtShmManager::Instance().OnCacheShmEnd();
			db_save_workers_->add_work(new MessageSaveAllWork());
			event_loop_->cancel(close_save_timer_);
		}
	}

	void Server::SaveDone()
	{
		if (save_status_ == normal_saving) {
			save_status_ = normal_save_done;
			////真正的离线玩家处理策略,这里不需要再处理了，checkoffline已经处理过
			//for (auto logout_iter = to_logout_players_.begin(); logout_iter != to_logout_players_.end(); ) {
			//	auto player = FindPlayer(*logout_iter);
			//	if (player != nullptr && player->Scene() == nullptr) {
			//		OffLineAddSceneInfo(player);
			//		LOG_INFO << "[Server Save Done], scene player:" << player->Guid();
			//		OffLine(*logout_iter);
			//		logout_iter = to_logout_players_.erase(logout_iter);
			//	}
			//	else {
			//		++logout_iter;
			//	}
			//}
			//LOG_INFO << "SaveDone to_logout_players size=" << to_logout_players_.size();
			LOG_INFO << "SaveDone! save_status_="<< save_status_;
		}
		else{
			save_status_ = close_save_done;

			LOG_INFO << "SaveDone! all saved exit! save_status_=" << save_status_;

			url_pool_workers_->stop();
			db_load_workers_->stop();
			db_save_workers_->stop();
			event_loop_->quit();

			//释放shm
			MtShmManager::Instance().OnRelease(); 
			
			//EXIT();
		}
	}

	void Server::OnShutdown()
	{
		closing_ = true;
		if (tcp_server_->started()) {
			//关网
			LOG_INFO << "[Server] tcp acceptor closing";
			tcp_server_->stop_acceptor();
			//停止所有场景线程, 这样玩家数据除了db线程和主线程, 没有人访问了
			MtSceneManager::Instance().OnShutDown();
			LOG_INFO << "[Server] all scene thread quit";
			auto default_scene = MtSceneManager::Instance().DefaultScene(Packet::Camp::Alliance);
			//thread_lua.reset(default_scene->Lua());
			//踢掉玩家
			for (auto child: online_player_ ) {
				child.second->SendCommonReply("Shutdown", {}, {}, {});
				SendG2SCommonMessage(child.second.get(), "G2SLogout", {}, {}, {});
			}
			LOG_INFO << "[Server] online player size:" << online_player_.size();
			close_save_timer_ = event_loop_->run_every(seconds(1), boost::bind(&Server::CheckCloseSave, this));
		}
		else {
			if (event_loop_) {
				event_loop_->quit();
			}
		}
	}

	void Server::OnMinuteTick()
	{
		std::string sql =
			(boost::format("call pro_update_serverstatus(%1%,'%2%','%3%',%4%,%5%,%6%,%7%,'%8%');") %
			MtConfig::Instance().server_id_ % MtConfig::Instance().server_name_ %
			MtConfig::Instance().server_host_% MtConfig::Instance().server_port_%
			MtConfig::Instance().http_port_ % (int32)(MtConfig::Instance().serverstatus_) %
			connnection_count_ % MtTimerManager::Instance().NowTimeString()).str();
		AddUrlTask(new serverstatus_require_work(sql));
		//get_gaming_player_num(), get_cold_player_num(), get_online_player_num()
	}

	void Server::EnableServerLevel()
	{
		enable_level_ = !enable_level_;
		if(enable_level_){
			UpdateServerLevel();
		}
		else {
			server_level_ = 999;
		}
	}
	void Server::UpdateServerLevel() 
	{
		if (enable_level_) {
			//更新服务器等级
			uint32 delta_day = MtTimerManager::Instance().DiffDayToNowByNumerDate(MtConfig::Instance().open_server_date_);
			server_level_ = MtActorConfig::Instance().CalcServerLevel(delta_day);
			try {
				next_lv_day_ = thread_lua->call<int32>(10, "CalcNextServerLvDays", delta_day);
			}
			catch (ff::lua_exception& e) {
				LOG_ERROR << "UpdateServerLevel error!" << e.what();
			}
		}
		//BroadcastG2SCommonMessage("G2SRefreshSeverLvInfo", { server_level_,next_lv_day_ }, {}, {});
	}
	void Server::OnSecondTick()
	{
		uint32 db_refresh_time_ = info_->db_refresh_time();
		if (db_refresh_time_==0){
			MtTimerManager::Instance().SetTime();
			uint32 nowtime = MtTimerManager::Instance().CurrentDate();
			info_->set_db_refresh_time(nowtime);
			return;
		}
		int32 refresh_year = MtTimerManager::Instance().GetYearFromUT(db_refresh_time_);
		int32 refresh_month = MtTimerManager::Instance().GetMonthFromUT(db_refresh_time_);
		int32 refresh_day = MtTimerManager::Instance().GetDayFromUT(db_refresh_time_);
		int32 refresh_week = MtTimerManager::Instance().GetWeekFromUT(db_refresh_time_);
		int32 refresh_hour = MtTimerManager::Instance().GetHourFromUT(db_refresh_time_);
		int32 refresh_minute = MtTimerManager::Instance().GetMinuteFromUT(db_refresh_time_);
		//int32 refresh_second = MtTimerManager::Instance().GetSecondFromUT(db_refresh_time_);
		MtTimerManager::Instance().SetTime();
		uint32 nowtime = MtTimerManager::Instance().CurrentDate();
		info_->set_db_refresh_time(nowtime);

		int32 now_year = MtTimerManager::Instance().GetYear();
		int32 now_month = MtTimerManager::Instance().GetMonth();
		int32 now_day = MtTimerManager::Instance().GetDay();
		int32 now_hour = MtTimerManager::Instance().GetHour();
		int32 now_minute = MtTimerManager::Instance().GetMinute();
		//int32 now_second = MtTimerManager::Instance().GetSecond();
		int32 now_week = MtTimerManager::Instance().GetWeekNumber();
		
		if (refresh_year != now_year)
		{
		}
		if (refresh_month != now_month)
		{
			OnMonthTriger(now_month);
		}
		if (refresh_week != now_week)
		{
			OnWeekTriger(now_week);
		}
		if (refresh_day != now_day)
		{
			//更新服务器等级
			UpdateServerLevel();

			OnDayTriger(now_day);
		}
		if (refresh_hour != now_hour)
		{
			OnHourTriger(now_hour);
		}
		if (refresh_minute != now_minute)
		{
			OnMinTriger(now_minute);
		}
		//if (refresh_second != now_second)
		//{
		//	OnSecondTriger(now_second);
		//}

		auto elapsesecond = MtTimerManager::Instance().DiffTimeToNow(db_refresh_time_);
		MtBattleFieldManager::Instance().OnSecondTick(elapsesecond);
		MtGuildManager::Instance().OnSecondTick(elapsesecond);
	}

	void Server::OnMonthTriger(int32 cur_month)
	{
		Server::Instance().BroadcastG2SCommonMessageToScene("G2SMonthTriger", { cur_month }, {}, {});
	}

	void Server::OnWeekTriger(int32 cur_week)
	{
		MtActivityManager::Instance().OnWeekTriger(cur_week);
		MtGuildManager::Instance().OnWeekTriger(cur_week);
		MtTowerManager::Instance().ClearPlayerTowerInfo();
		Server::Instance().BroadcastG2SCommonMessageToScene("G2SWeekTriger", { cur_week }, {}, {});
	}
	void Server::OnDayTriger(int32 cur_day)
	{
		luafile_manager::instance().on_update_luafile();
		db_load_workers_->add_work(new LoadArenaDailyRewardGuids());

		MtActivityManager::Instance().OnDayTriger(cur_day);
		//需要在MtActivityManager::Instance().OnDayTriger(cur_day);先更新时间逻辑，
		try
		{
			thread_lua->call<void>(00005, "OnDayTriger", cur_day);
		}
		catch (ff::lua_exception& e)
		{
			LOG_ERROR << "handle message:" << " OnDayTriger error:" << e.what();
		}
		MtGuildManager::Instance().OnDayTriger(cur_day);	
		MtBattleReplyManager::Instance().ClearLog();
		Server::Instance().BroadcastG2SCommonMessageToScene("G2SDayTriger", { cur_day }, {}, {});
	}
	void Server::OnHourTriger(int32 cur_hour)
	{
		MtActivityManager::Instance().OnHourTriger(cur_hour);
		//需要在MtActivityManager::Instance().OnHourTriger(cur_day);先更新时间逻辑，
		try
		{
			thread_lua->call<void>(00005, "OnHourTriger", cur_hour);
		}
		catch (ff::lua_exception& e)
		{
			LOG_ERROR << "handle message:"	<< " OnHourTriger error:" << e.what();
		}
		MtGuildManager::Instance().OnHourTriger(cur_hour);
		

		Server::Instance().BroadcastG2SCommonMessageToScene("G2SHourriger", { cur_hour }, {}, {});
	}
	void Server::OnMinTriger(int32 cur_min)
	{
		MtActivityManager::Instance().OnMinTriger(cur_min);
		try
		{
			thread_lua->call<void>(00005, "OnMinTriger", cur_min);
		}
		catch (ff::lua_exception& e)
		{
			LOG_ERROR << "handle message:" << " OnMinTriger error:" << e.what();
		}

		MtGuildManager::Instance().OnMinTriger(cur_min);
		Server::Instance().BroadcastG2SCommonMessageToScene("G2SMinTriger", { cur_min }, {}, {});

	}
	void Server::OnSecondTriger(int32 cur_second)
	{
		cur_second;
	}
	void Server::OnTick()
	{
		auto current_time = MtTimerManager::Instance().CurrentTime();
		auto elapseTime = current_time - last_tick_;
		last_tick_ = current_time;
		battle_manager_.OnTick(elapseTime);
		MtCapturePointManager::Instance().OnTick();
	}
	void Server::OnBigTick()
	{
		//低频tick
		auto current_time = MtTimerManager::Instance().CurrentTime();
		static uint64 last_big_tick_ = current_time;
		if (seconds_transform_date(last_big_tick_).day_of_year() != seconds_transform_date(current_time).day_of_year()) {
			//OnNewDay();
		}
		auto elapseTime = current_time - last_big_tick_;
		last_big_tick_ = current_time;
		MtCapturePointManager::Instance().OnBigTick();
		MtTeamManager::Instance().OnBigTick(elapseTime);
		MtBattleFieldManager::Instance().OnBigTick(elapseTime);
		LOG_WARN << "[Server] online player:" << online_player_.size();
	}
	void Server::OnCacheSelf()
	{
		std::set<google::protobuf::Message *> msg_set;
		msg_set.insert(info_.get());
		MtShmManager::Instance().Cache2Shm(msg_set, "MtServer");
	}
	bool Server::OnLoadSelf(boost::shared_ptr<zxero::dbcontext>& dbctx)
	{
		auto dbserver = ActiveModel::Base<Packet::ServerInfo>(dbctx).FindAll({});
		for(auto db : dbserver){
			info_ = db;
		}
		return true;
	}
	void Server::OnCacheAll()
	{		
		if (save_status_ != Server::save_status::close_saving && !closing_){
			MtShmManager::Instance().OnCacheShmBegin();
			MtSceneStageManager::Instance().OnCacheAll();
			MtCapturePointManager::Instance().OnCacheAll();
			MtGuildManager::Instance().OnCacheAll();
			MtWorldBossManager::Instance().OnCacheAll();
			MtArenaManager::Instance().OnCacheAll();
			TrialFieldManager::Instance().OnCacheAll();
			MtTowerManager::Instance().OnCacheAll();
			MtFriendManager::Instance().OnCacheAll();
			MtBattleReplyManager::Instance().OnCacheAll();
			OnCacheSelf();
			if (online_player_.empty()) {
				MtShmManager::Instance().OnCacheShmEnd();
			}
			else{
				OnCacheAllPlayer();
			}
		}
	}
	

	void Server::MarkPlayerCached(const uint64 guid)
	{
		if (to_cache_players_.empty())
			return;

		to_cache_players_.erase(
			std::remove_if(to_cache_players_.begin(), to_cache_players_.end(), [guid](auto& iter) {	return iter == guid; })
			,to_cache_players_.end());
		if (to_cache_players_.empty() && !shm_debug_) {
			MtShmManager::Instance().OnCacheShmEnd();
			LOG_INFO << "MarkPlayerCached end!";
		}
	}

	void Server::MarkPlayerLogout(const uint64 guid)
	{
		to_logout_players_.insert(guid);
	}
	
	void Server::OnCacheAllPlayer()
	{
		to_cache_players_.clear();
		boost::shared_lock<boost::shared_mutex> l(player_shared_mutex_);
		for (auto child : online_player_) {
			to_cache_players_.push_back(child.first);
			//主线程cache
			child.second->OnServerCache();
			if (child.second->Scene() == nullptr) {//没scene的player由主线程强制cache,
				child.second->OnCache2Shm();
			}
		}
		//这里需要做异步结束判定，因为MtSceneManager::Instance().OnCacheAll()在进行异步player 存储
		MtSceneManager::Instance().DispatchMessage(boost::make_shared<G2S::CacheAllNotify>());
		LOG_INFO << "MarkPlayerCached begin! count=" << to_cache_players_.size();
	}
	int32 Server::ServerId() { return info_->serverid(); }
	int32 Server::GetInvestCount() 
	{ 
		return info_->invest_count(); 
	};
	void Server::SetInvestCount(int32 count)
	{
		info_->set_invest_count(count);
	}
	
	bool Server::OnAccountNameVerify(const boost::shared_ptr<client_session>& session, const boost::shared_ptr<Packet::AccountNameVerify>& message, int32 /*source*/)
	{
		db_load_workers_->add_work(new AccountNameVerifyWork(session, message));
		return true;
	}

	bool Server::OnAccountRegist(const boost::shared_ptr<client_session>& session, const boost::shared_ptr<Packet::AccountRegist>& message, int32 /*source*/)
	{
		db_load_workers_->add_work(new AccountRegistWork(session, message));
		return true;
	}

	bool Server::OnAccountPlayerList(const boost::shared_ptr<client_session>& session, const boost::shared_ptr<Packet::AccountPlayerList>& /*message*/, int32 /*source*/)
	{
		auto account = session->get_account();
		if (account == nullptr) {
			return true;
		}
		Packet::AccountPlayerListReply reply;
		for (auto& kv : online_player_) {
			if (kv.second->GetDBPlayerInfo()->account_guid() == account->guid()) {
				auto db_info = reply.add_player_list();
				db_info->CopyFrom(*(kv.second->GetDBPlayerInfo()));
			}
		}
		if (reply.player_list_size() > 0) {
			session->send(reply);
		}  else {
			db_load_workers_->add_work(new PlayerLoginLoadTask(session));
		}
		return true;
	}

	void Server::OnDbConnected()
	{
		AddDbSaveTask(new ShareMemoryCrashRecover());
		AddDbLoadTask(new serial_load_work(0));
	}

	void Server::OnStartUpSaveShareMemDataDone()
	{
		MtSceneStageManager::Instance().OnDbConnected();
		AddDbLoadTask(new ServerLoader());
		AddDbLoadTask(new GuildLoader());
		AddDbLoadTask(new CapturePointLoader());
		AddDbSaveTask(new TowerInfoLoader());
		AddDbLoadTask(new WorldBossLoader());
		AddDbSaveTask(new ArenaUserCheckInit());
		AddDbSaveTask(new TopListLoader());		

		init_checker_timer_ = event_loop_->run_every(seconds(1), [&]() {
			if (Server::Instance().AllLoadWorkDone()) {
				Server::Instance().OnAllDataLoaded();
				event_loop_->cancel(init_checker_timer_);
			}
		});
	}

	bool Server::AllLoadWorkDone()
	{
		return db_load_workers_->working() == false
			&& db_save_workers_->working() == false;
	}

	void Server::OnAllDataLoaded()
	{
		//event_loop_->run_every(seconds(100), boost::bind(&Server::OnSaveAll, this));
		event_loop_->run_every(seconds(60), boost::bind(&Server::OnMinuteTick, this));
		event_loop_->run_every(seconds(1), boost::bind(&Server::OnSecondTick, this));
		event_loop_->run_every(milliseconds(100), boost::bind(&Server::OnTick, this));
		event_loop_->run_every(seconds(BIG_TICK), boost::bind(&Server::OnBigTick, this));
		event_loop_->run_every(seconds(30), boost::bind(&Server::OnCacheAll, this));
		event_loop_->run_in_loop([&]() {
			tcp_server_->start(4);
			LOG_INFO << "tcp_server " << tcp_server_ << " listen @ " << tcp_server_->hostport();
		});
		event_loop_->run_in_loop([&]() {
			http_server_->start(1);
			LOG_INFO << "http_server_ " << http_server_ << " listen @ " << http_server_->hostport();
		});
		last_tick_ = MtTimerManager::Instance().CurrentTime();
	}

	boost::shared_ptr<MtPlayer> Server::FindPlayer(uint64 guid)
	{
		boost::shared_lock<boost::shared_mutex> l(player_shared_mutex_);
		auto it = online_player_.find(guid);
		if (it == std::end(online_player_)) {
			return nullptr;
		}
		else {
			return it->second;
		}
	}

	boost::shared_ptr<MtPlayer> Server::FindPlayerByName(const std::string& name)
	{
		boost::shared_lock<boost::shared_mutex> l(player_shared_mutex_);
		for (auto& player_ : online_player_)
		{
			if (player_.second->GetScenePlayerInfo()->name() == name)
			{
				return player_.second;
			}
		}
		return nullptr;
	}
	boost::shared_ptr<Packet::ScenePlayerInfo> Server::FindPlayerSceneInfo(uint64 playerguid)
	{
		boost::shared_lock<boost::shared_mutex> l(playersceneinfo_shared_mutex_);
		auto iter_ = player_scene_info.find(playerguid);
		if (iter_ != player_scene_info.end())
		{
			return iter_->second;
		}
		return nullptr;
	}
	boost::shared_ptr<Packet::ScenePlayerInfo> Server::FindPlayerSceneInfoByName(const std::string& name)
	{
		boost::shared_lock<boost::shared_mutex> l(playersceneinfo_shared_mutex_);
		for (auto& player_ : player_scene_info)
		{
			if (player_.second->basic_info().name() == name)
			{
				return player_.second;
			}
		}
		return nullptr;
	}
	boost::shared_ptr<Packet::ScenePlayerInfo> Server::LoadScenePlayerInfoByGuid(boost::shared_ptr<zxero::dbcontext>& dbctx, uint64 playerguid)
	{
		if (ActiveModel::Base<Packet::PlayerDBInfo>(dbctx).CheckExist({ { "guid", playerguid } })) {
			auto db_info = ActiveModel::Base<Packet::PlayerDBInfo>(dbctx).FindOne({ { "guid", playerguid } });
			if (db_info != nullptr) {
				if (ActiveModel::Base<Packet::Container>(dbctx).CheckExist({ { "player_guid", playerguid },{ "type", Packet::ContainerType::Layer_Equip } }) == true) {
					auto container_db_info = ActiveModel::Base < Packet::Container>(dbctx).FindOne({ { "player_guid", playerguid },{ "type", Packet::ContainerType::Layer_Equip } });
					if (container_db_info != nullptr) {
						boost::shared_ptr<Packet::ScenePlayerInfo> info = boost::make_shared<Packet::ScenePlayerInfo>();
						if (info != nullptr) {
							info->mutable_basic_info()->CopyFrom(db_info->basic_info());
							auto equips = ActiveModel::Base<Packet::EquipItem>(dbctx).FindAll({ { "container_guid", container_db_info->guid() } });
							for (auto& equip : equips) {
								info->add_main_equips()->CopyFrom(*equip.get());
							}
							return info;
						}
					}
				}
			}
		}
		return nullptr;
	}
	
	boost::shared_ptr<Packet::ScenePlayerInfo> Server::LoadScenePlayerInfoByName(boost::shared_ptr<zxero::dbcontext>& dbctx, std::string& playername)
	{
		if (ActiveModel::Base<Packet::PlayerDBInfo>(dbctx).CheckExist({ { "basic_info.name", playername } })) {
			auto db_info = ActiveModel::Base<Packet::PlayerDBInfo>(dbctx).FindOne({ { "basic_info.name", playername } });
			if (db_info != nullptr) {
				auto playerguid = db_info->guid();
				if (ActiveModel::Base<Packet::Container>(dbctx).CheckExist({ { "player_guid", playerguid },{ "type", Packet::ContainerType::Layer_Equip } }) == true) {
					auto container_db_info = ActiveModel::Base < Packet::Container>(dbctx).FindOne({ { "player_guid", playerguid },{ "type", Packet::ContainerType::Layer_Equip } });
					if (container_db_info != nullptr) {
						boost::shared_ptr<Packet::ScenePlayerInfo> info = boost::make_shared<Packet::ScenePlayerInfo>();
						if (info != nullptr) {
							info->mutable_basic_info()->CopyFrom(db_info->basic_info());
							auto equips = ActiveModel::Base<Packet::EquipItem>(dbctx).FindAll({ { "container_guid", container_db_info->guid() } });
							for (auto& equip : equips) {
								info->add_main_equips()->CopyFrom(*equip.get());
							}
							return info;
						}
					}
				}
			}
		}
		return nullptr;
	}
	
	bool Server::OnPrivateChatMessageReq(
		const boost::shared_ptr<client_session>& session,
		const boost::shared_ptr<Packet::PrivateChatMessageReq>& message,
		int32 /*source*/)
	{
		auto player = session->player();
		if (player == nullptr) {
			ZXERO_ASSERT(false);
			return true;
		}
		uint64 targetguid = message->target_guid();
		auto target_player = FindPlayer(targetguid);
		Packet::PrivateChatMessageReply reply;
		if (target_player != nullptr)
		{
			reply.mutable_playerinfo()->CopyFrom(target_player->GetDBPlayerInfo()->basic_info());
		}
		player->SendMessage(reply);
		return true;
	}
	
	bool Server::Started() const
	{
		return tcp_server_->started();
	}
	void Server::SendMessage(const boost::shared_ptr<google::protobuf::Message>& message,
		const boost::shared_ptr<MtPlayer>& player /*= nullptr*/)
	{
		if (scene_message_dispatcher_)
			scene_message_dispatcher_->handle_message(player, message, MessageSource::SCENE);
	}
	void Server::FakeClientMessage(const boost::shared_ptr<client_session>& session,
		const boost::shared_ptr<google::protobuf::Message>& message)
	{
		if (session == nullptr) {
			//空session 透传后是会宕机的额!
			ZXERO_ASSERT(false) << "invalid session! message name=" << message->GetTypeName();
			return;
		}
		if (client_message_dispatcher_)
			client_message_dispatcher_->handle_message(session,message, MessageSource::CLIENT);
	}

	bool Server::UnRegistedS2GMessage(
		const boost::shared_ptr<MtPlayer>& player,
		const boost::shared_ptr<google::protobuf::Message>& message,
		int32)
	{
		LOG_INFO << "message(S2G) unregisted";
		if (player != nullptr && player->Session() != nullptr) {
			try
			{
				thread_lua->call<void>(10005, ("xOn" + message->GetDescriptor()->name()).c_str(), player.get(), message.get());
			}
			catch (ff::lua_exception& e)
			{
				LOG_ERROR << "handle message:"
					<< message->GetDescriptor()->name() << "error:" << e.what();
			}
			
			LOG_INFO << "unregisted message received " << player->Session()->get_state() << "," << message->GetTypeName();
		}
		return false;
	}

	bool Server::HasPlayer(uint64 guid)
	{
		boost::shared_lock<boost::shared_mutex> l(player_shared_mutex_);
		return online_player_.find(guid) != std::end(online_player_);
	}

	bool Server::OnTrialDataLoadDone(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<DB::PlayerTrialInfo>& msg)
	{
		player->DispatchMessage(msg);
		return true;
	}

  void Server::KickAll(uint64 except)
  {
    for (auto& p : online_player_ | boost::adaptors::map_values) {
		if (p->Guid() != except){
			auto message = boost::make_shared<Packet::LuaRequestPacket>();
			message->set_request_name("KickOut");
			p->DispatchMessage(message);
		}
    }
    programer_debug_ = true;
  }

  boost::shared_ptr<Packet::ServerInfo> Server::GetServerInfo()
  {
	  return info_;
  }
  void Server::OnHttpConnection(const boost::shared_ptr<tcp_connection> conn)
  {
	  RunInLoop(bind(&Server::OnHttpConnectionLoop, this, conn));
  }
  void Server::OnHttpMessage(const boost::shared_ptr<tcp_connection>& conn,
	  const boost::shared_ptr<Packet::HttpData>& msg,
	  zxero::timestamp_t time)
  {
	  RunInLoop(bind(&Server::OnHttpMessageLoop, this, conn, msg, time));
  }
  void Server::OnHttpConnectionLoop(const boost::shared_ptr<tcp_connection>& /*conn*/)
  {

  }

  void Server::OnHttpMessageLoop(const boost::shared_ptr<tcp_connection>& conn,
	  const boost::shared_ptr<Packet::HttpData>& msg,
	  zxero::timestamp_t time)
  {
	  conn;
	  time;
	  try
	  {
		  LOG_INFO << "OnHttpMessageLoop ";
		  if (msg->url() == "//mqpayorder")
		  {
			  boost::shared_ptr<Packet::PayOrderInfo> order = boost::make_shared<Packet::PayOrderInfo>();
			  if (order == nullptr)
			  {
				  ZXERO_ASSERT(false) << "PayOrderInfo order make null";
				  SendHttpMessage(conn, "-1");
				  conn->shutdown();
				  return;
			  }
			  std::string ordersign = "";
			  for (int32 i = 0; i < msg->post_size(); i++)
			  {
				  auto postdata = msg->post(i);
				  if (postdata.key() == "OrderId")
				  {
					  order->set_orderid(postdata.value());
				  }
				  else if (postdata.key() == "PartnerId")
				  {
					  order->set_partnerid(boost::lexical_cast<int32>(postdata.value()));
				  }
				  else if (postdata.key() == "ServerId")
				  {
					  order->set_serverid(boost::lexical_cast<int32>(postdata.value()));
				  }
				  else if (postdata.key() == "PlayerId")
				  {
					  order->set_playerid(boost::lexical_cast<uint64>(postdata.value()));
				  }
				  else if (postdata.key() == "ProductId")
				  {
					  order->set_productid(postdata.value());
				  }
				  else if (postdata.key() == "ChargeMoney")
				  {
					  order->set_chargemoney(boost::lexical_cast<float>(postdata.value()));
				  }
				  else if (postdata.key() == "ActivityChargeMoney")
				  {
					  order->set_activitychargemoney(boost::lexical_cast<float>(postdata.value()));
				  }
				  else if (postdata.key() == "DeviceIdentifier")
				  {
					  order->set_deviceldentifier(postdata.value());
				  }
				  else if (postdata.key() == "IP")
				  {
					  order->set_ip(postdata.value());
				  }
				  else if (postdata.key() == "Extra")
				  {
					  order->set_extra(postdata.value());
				  }
				  else if (postdata.key() == "FinalChargeMoney")
				  {
					  order->set_finalchargemoney(boost::lexical_cast<float>(postdata.value()));
				  }
				  else if (postdata.key() == "Sign")
				  {
					  ordersign = postdata.value();
				  }

			  }
			  order->set_ismonthcard(false);
			  order->set_guid(MtGuid::Instance()(*order));
			  if (order->IsInitialized() == false)
			  {
				  ZXERO_ASSERT(false) << "PayOrderInfo missing required fields: " << order->InitializationErrorString();
				  SendHttpMessage(conn, "-1");
				  conn->shutdown();
				  return;
			  }
			  //先save吧。
			  //auto player_ = FindPlayer(order->playerid());
			  //if (player_ == nullptr)
			  //{
				 // ZXERO_ASSERT(false) << "PayOrderInfo Player null " << order->playerid();
				 // SendHttpMessage(conn, "-2");
				 // return;
			  //}
			  SendHttpMessage(conn, "0");
			  auto partner = MtConfig::Instance().GetPartnerItem(order->partnerid());
			  if (partner == nullptr)
			  {
				  ZXERO_ASSERT(false) << "PayOrderInfo partner null : " << order->playerid() << " orderid=" << order->orderid() << " serverid=" << order->orderid() << " partnerid()=" << order->partnerid();
				  SendHttpMessage(conn, "-1");
				  conn->shutdown();
				  return;
			  }
			  std::string rawstring = (boost::format("%1%%2%%3%%4%%5%%6%%7%") %
				  order->partnerid() % order->serverid() % order->playerid() % order->orderid() %
				  order->productid() % order->deviceldentifier() % partner->LoginKey).str();
			  std::string rawmd5string = utils::md5(rawstring);
			  if (rawmd5string != ordersign)
			  {
				  ZXERO_ASSERT(false) << "PayOrderInfo md5 error : " << order->playerid() << " orderid=" << order->orderid() << " serverid=" << order->serverid()<<" rawstring="<< rawstring<<" rawmd5string="<< rawmd5string<<" ordersign="<< ordersign;
				  SendHttpMessage(conn, "-1");
				  conn->shutdown();
				  return;
			  }
			  
			  AddDbSaveTask(new SaveMqPayOrder(order));
			  LOG_INFO << "PayOrderInfo begin SaveMqPayOrder playerid=" << order->playerid() << " orderid=" << order->orderid() << " serverid=" << order->serverid();
		  }
		  else if (msg->url() == "//mcrefresh")
		  {
			  bool ret = MtConfig::Instance().ServerActive();  
			  if (ret)
			  {
				  SendHttpMessage(conn, "0");
				  LOG_INFO << "ServerActive Fresh True";
				  if (MtConfig::Instance().serverGroupDB_.GroupState == 2)
				  {
					  LOG_INFO << "ServerActive GroupState To 2";
					  KickAll(INVALID_GUID);
				  }
				  else
				  {
					  LOG_INFO << "ServerActive GroupState To !2";
					  programer_debug_ = false;
				  }
				  MtConfig::Instance().ConvertOpenServerDate();
				  UpdateServerLevel();
				  BroadcastG2SCommonMessageToPlayer("G2SUpdateServerLevel", {}, {}, {});
			  }
			  else
			  {
				  SendHttpMessage(conn, "-1");
				  ZXERO_ASSERT(false) << "ServerActive Fresh False ";
			  }
		  }
		  else if (msg->url() == "/mailpay")
		  {
			try
			{
				std::string playerguid_str = "";
				std::string title = "";
				std::string content = "";
				google::protobuf::RepeatedPtrField<Packet::ItemCount> items;
				for (int32 i = 0; i < msg->post_size(); i++)
				{
					auto postdata = msg->post(i);
					if (postdata.key() == "guid")
					{
						playerguid_str = postdata.value();
					}
					else if (postdata.key() == "title")
					{
						title = postdata.value();
					}
					else if (postdata.key() == "content")
					{
						content = postdata.value();
					}
				}
				if (playerguid_str == "" || title == "" || content == "")
				{
					ZXERO_ASSERT(false) << "mailpay nil guid="<< playerguid_str<<" title="<< title<<" content="<< content;
					SendHttpMessage(conn, "error");
					conn->shutdown();
					return;
				}
				for (int32 i = 0; i < MAX_MAIL_REWARD_COUNT; i++)
				{
					char Member[64] = { 0 };
					zprintf(Member, sizeof(Member), "itemid_%d", i);
					int32 itemid = 0;
					for (int32 j = 0; j < msg->post_size(); j++)
					{
						auto postdata = msg->post(j);
						if (postdata.key() == Member)
						{
							itemid = boost::lexical_cast<int32>(postdata.value());
							break;
						}
					}
					if (itemid == 0)
					{
						continue;
					}
					zprintf(Member, sizeof(Member), "itemcount_%d", i);
					for (int32 j = 0; j < msg->post_size(); j++)
					{
						auto postdata = msg->post(j);
						if (postdata.key() == Member)
						{
							auto item = items.Add();
							item->set_itemid(itemid);
							item->set_itemcount(boost::lexical_cast<int32>(postdata.value()));
							break;
						}
					}
				}
				uint64 playerguid = utils::guid_string_to_uint64(playerguid_str);
				if (playerguid == 0)
				{
		  			AddDbLoadTask(new LoadAllPlayerMailPay(title, content, items));
				}
				else
				{
		  			boost::shared_ptr<MtPlayer> player_ = FindPlayer(playerguid);
		  			static_message_manager::Instance().create_message(playerguid, player_,  MAIL_OPT_TYPE::GM_ADD, title, content, &items);
				}	
				SendHttpMessage(conn, "ok");
				LOG_INFO << "MAILPAY str guid = "<< playerguid_str<<" title = "<< title<<" content = "<< content;
			}
			catch (...)
			{
				ZXERO_ASSERT(false) << "#MAILPAYBACK# error ";
			}
			//conn->force_close();
		  }
		  else if (msg->url() == "//testret")
		  {
			  LOG_ERROR << "testret";
			  SendHttpMessage(conn, "testmt");
		  }
		  else if (msg->url() == "/activity")
		  {
			  bool ret = MtConfig::Instance().EnableActive( false );
			  if (ret)
			  {
				  SendHttpMessage(conn, "ok");
			  }
			  else
			  {
				  SendHttpMessage(conn, "failed");
			  }
			  LOG_INFO << "gm send activity "<<ret;
		  }
		  else if (msg->url() == "/coveractivity")
		  {
			  int32 activityid = 0;
			  std::string activitystr = "";
			  for (int32 i = 0; i < msg->post_size(); i++)
			  {
				  auto postdata = msg->post(i);
				  if (postdata.key() == "id")
				  {
					  activityid = boost::lexical_cast<int32>(postdata.value());
				  }
				  else if (postdata.key() == "activity")
				  {
					  activitystr = postdata.value();
				  }
			  }
			  if (activityid ==0 || activitystr == "")
			  {
				  ZXERO_ASSERT(false)<<"coveractivity null "<< activityid<<","<< activitystr;
				  SendHttpMessage(conn, "failed");
				  return;
			  }
			  SendHttpMessage(conn, "ok");
			  std::map<int32, std::string> activitymap;
			  activitymap.insert({ activityid,activitystr });
			  MtOperativeConfigManager::Instance().UpdateCurrentData(activitymap);
		  }
		  else if (msg->url() == "//ticks") {
			  std::string tick;
			  auto scene_tick = MtSceneManager::Instance().GetSceneTick();
			  for (auto& pair : scene_tick) {
				  tick += (boost::format("scene:%d, tick:%d. ") % pair.first % pair.second).str();
			  }
			  tick += (boost::format("server loop:%d. ") % event_loop_->efficency()).str();
			  SendHttpMessage(conn, tick);
		  }
		  else if (msg->url() == "//log_thread_safe")
		  {
			  boost::scoped_ptr<zxero::LogFile> g_logFile;
			  boost::scoped_ptr<zxero::LogFile> g_error_logFile;
			  boost::scoped_ptr<zxero::LogFile> g_db_logFile;
			  boost::scoped_ptr<zxero::LogFile> g_bi_logFile;
			  g_logFile->set_thread_safe(g_logFile->get_thread_safe());
			  g_error_logFile->set_thread_safe(g_error_logFile->get_thread_safe());
			  g_db_logFile->set_thread_safe(g_db_logFile->get_thread_safe());
			  g_bi_logFile->set_thread_safe(g_bi_logFile->get_thread_safe());
			  std::string safe = (boost::format("log:%d, error_log:%d,db_log:%d,bi_log:%d")
				  % g_logFile->get_thread_safe()
				  % g_error_logFile->get_thread_safe()
				  % g_db_logFile->get_thread_safe()
				  % g_bi_logFile->get_thread_safe()
				  ).str();
			  SendHttpMessage(conn, safe);
		  }
		  else
		  {
			  ZXERO_ASSERT(false) << "#HTTP# no url !" << msg->url();
		  }
	  }
	  catch (boost::bad_any_cast& e)
	  {
		  ZXERO_ASSERT(false) << "OnHttpMessageLoop [" << conn << "] have catch: " << e.what();
		  SendHttpMessage(conn, "0");
	  }
	  conn->shutdown();
	  //std::string result = buf->to_stdstring();

	  //http_parser_settings parser_settings;
	  //parser_settings.on_message_begin = on_message_begin;
	  //parser_settings.on_message_complete = on_message_complete;
	  //parser_settings.on_headers_complete = on_headers_complete;
	  //parser_settings.on_header_field = header_field_cb;
	  //parser_settings.on_header_value = header_value_cb;
	  //parser_settings.on_url = url_cb;
	  //parser_settings.on_body = body_cb;
	  //test_http_post(result,&parser_settings);
	  //Json::Reader reader;
	  //Json::Value root;
	  //try
	  //{
		 // if (reader.parse(result.c_str(), root))
		 // {
			//  if (root.isMember("from"))
			//  {
			//	  std::string from_ = root["from"].asString();
			//	  LOG_TRACE << "http message 1:" << from_;
			//	  if (from_ == "webgm")
			//	  {
			//		  if (!root.isMember("guid"))
			//		  {
			//			  LOG_ERROR << "http message no guid :" << from_;
			//			  return;
			//		  }
			//		  if (!root.isMember("commandstr"))
			//		  {
			//			  LOG_ERROR << "http message no command :" << from_;
			//			  return;
			//		  }
			//		  std::string commandstr_ = root["commandstr"].asString();
			//		  std::string playerguid_str = root["guid"].asString();
			//		  uint64 playerguid = utils::guid_string_to_uint64(playerguid_str);
			//		  boost::shared_ptr<MtPlayer> player_ = FindPlayer(playerguid);
			//		  if (player_ != nullptr)
			//		  {
			//			  auto scene_ = player_->Scene();
			//			  if (scene_ != nullptr)
			//			  {
			//				  scene_->QueueInLoop(boost::bind(gm_command, scene_, player_, commandstr_));
			//			  }
			//		  }
			//		  else
			//		  {
			//			  gm_command(nullptr,nullptr, commandstr_);
			//		  }
			//	  }
			//	  else if (from_ == "mailpay")
			//	  {
			//		  try
			//		  {
			//			  if (!root.isMember("guid"))
			//			  {
			//				  LOG_ERROR << "http message no guid :" << from_;
			//				  return;
			//			  }
			//			  if (!root.isMember("title"))
			//			  {
			//				  LOG_ERROR << "http message no title :" << from_;
			//				  return;
			//			  }
			//			  std::string title = root["title"].asString();

			//			  if (!root.isMember("content"))
			//			  {
			//				  LOG_ERROR << "http message no content :" << from_;
			//				  return;
			//			  }
			//			  std::string content = root["content"].asString();

			//			  google::protobuf::RepeatedPtrField<Packet::ItemCount> items;
			//			  for (int32 i = 0; i < MAX_MAIL_REWARD_COUNT; i++)
			//			  {
			//				  char Member[64] = { 0 };
			//				  zprintf(Member, sizeof(Member), "itemid_%d", i);
			//				  int32 itemid = 0;
			//				  if (root.isMember(Member))
			//				  {
			//					  itemid = root[Member].asInt();
			//				  }
			//				  if (itemid == 0)
			//				  {
			//					  continue;
			//				  }
			//				  zprintf(Member, sizeof(Member), "itemcount_%d", i);
			//				  if (root.isMember(Member))
			//				  {
			//					  auto item = items.Add();
			//					  item->set_itemid(itemid);
			//					  item->set_itemcount(root[Member].asInt());
			//				  }
			//			  }
			//			  std::string playerguid_str = root["guid"].asString();
			//			  uint64 playerguid = utils::guid_string_to_uint64(playerguid_str);
			//			  if (playerguid == 0)
			//			  {
			//				  AddDbLoadTask(new LoadAllPlayerMailPay(title, content, items));
			//			  }
			//			  else
			//			  {
			//				  boost::shared_ptr<MtPlayer> player_ = FindPlayer(playerguid);
			//				  static_message_manager::instance()->create_message(playerguid, nullptr,  MAIL_OPT_TYPE::GM_ADD, title, content, &items);
			//			  }		
			//			  LOG_INFO << "MAILPAY str " << playerguid_str << " id "<< playerguid;
			//		  }
			//		  catch (...)
			//		  {
			//			  ZXERO_ASSERT(false) << "#MAILPAYBACK# error ";
			//		  }
			//	  }
			//	  else if (from_ == "mqpay")
			//	  {
			//		  if (!root.isMember("PlayerId"))
			//		  {
			//			  LOG_ERROR << "http message no guid :" << from_;
			//			  return;
			//		  }
			//		  std::string playerguid_str = root["PlayerId"].asString();
			//		  uint64 playerguid = utils::guid_string_to_uint64(playerguid_str);
			//		  auto player_ = FindPlayer(playerguid);
			//		  if (player_ == nullptr)
			//		  {
			//			  LOG_ERROR << "http message no player :" << from_ << " playerguid " << playerguid_str;
			//			  return;
			//		  }
			//		  if (!root.isMember("products"))
			//		  {
			//			  LOG_ERROR << "http message no order :" << from_ << " playerguid " << playerguid_str;
			//			  return;
			//		  }
			//		  bool isproductarray = root["products"].isArray();
			//		  if (isproductarray == false)
			//		  {
			//			  LOG_ERROR << "http message is not productarray :" << from_ << " playerguid " << playerguid_str;
			//			  return;
			//		  }
			//		  int32 productsize = root["products"].size();
			//		  for (int32 i=0;i<productsize;i++)
			//		  {
			//			  if (root["products"][i].isMember("id"))
			//			  {
			//				  int32 productid = root["products"][i]["id"].asInt();
			//				  Server::Instance().SendG2SCommonMessage(player_.get(), "G2SChargeItem", { productid }, {}, {});
			//			  }
			//		  }

			//	  }
			//  }
		 // }
	  //}
	  //catch (...)
	  //{
		 // ZXERO_ASSERT(false) << "#HTTP# cache !" << result;
		 // SendHttpMessage(conn, "error");
	  //}
  }
  
  void Server::SendHttpMessage(const boost::shared_ptr<tcp_connection>& conn, const std::string& msg,bool bKeepAlive)
  {
	  if (conn != nullptr)
	  {
		  // 返回响应
		  std::string szResponse =  (boost::format("HTTP/1.0 %1%\r\nServer: %2%:%3%\r\nAccept-Ranges: bytes\r\nContent-Length: %4%\r\nConnection: %5%\r\nContent-Type: %6%\r\n\r\n%7%") %
			  "200 OK" % MtConfig::Instance().login_host_% MtConfig::Instance().http_port_ % msg.length() %
			  (bKeepAlive ? "Keep-Alive" : "close") % "text/html; charset=UTF-8" % msg).str();
		  //Date: %2%\r\n  % "Mon, 05 Mar 2018 12:31:59 GMT"
		  conn->send(szResponse);
	  }
	  else
	  {
		  ZXERO_ASSERT(false) << "http connect null msg: " << msg;
	  }
  }

  bool Server::CanUseGmCmd() const
  {
	  return MtConfig::Instance().gm_mode_;
  }

  void Server::OnConnetionMessage(
	  const boost::shared_ptr<tcp_connection>& conn, 
	  const boost::shared_ptr<google::protobuf::Message>& msg,
	  zxero::timestamp_t /*time*/)
  {
	  boost::shared_ptr<client_session> session;
	  //  获取会话环境
	  boost::any& c = conn->context();
	  if (c.empty())
	  {
		  LOG_ERROR << "connection [" << conn << "] has no context for protocol: " << msg->GetTypeName();
		  conn->force_close();
		  return;
	  } else
	  {
		  //  安全第一
		  try
		  {
			  session = boost::any_cast<boost::shared_ptr<client_session>>(c);
		  }
		  catch (boost::bad_any_cast& e)
		  {
			  LOG_ERROR << "connection [" << conn << "] have invalid context: " << e.what();
			  conn->force_close();
			  return;
		  }
		  if (session == nullptr) {
			ZXERO_ASSERT(session) << "connection [" << conn << "] get context failed";
			return;
		  }
	  }
	  //  验证状态处理消息
	  switch (session->get_state()) {
	  case kUnauthorization:
		  if (msg->GetDescriptor() != Packet::LoginRequest::descriptor()) {
			  LOG_ERROR << "kUnauthorization connection [" << conn << "] receive  unauthorization protocol : " << msg->GetTypeName();
			  conn->force_close();
			  return;
		  }
		  break;
		  /*case kVerifing:
		  {
			  if (msg->GetDescriptor() != Packet::PlayerLoginRequest::descriptor()) {
				  LOG_ERROR << "kVerifing connection [" << conn << "] receive  unauthorization protocol : " << msg->GetTypeName();
				  conn->force_close();
				  return;
			  }
		  }
		  break; */
		  case kInQueue:
		  case kDisconnected:
		  case kDisconnecting://  不处理不合时宜的协议，直接跳过			  
		  {
			  LOG_WARN << "connection [" << conn << "] receive invalid protocol : " << msg->GetTypeName() << " on state " << session->get_state();
			  return;
		  }break;
		  default:
			  break;
	  }   //  switch (session_->state())
	  session->incr_frame_packet();
	  client_message_dispatcher_->handle_message(session, msg, MessageSource::CLIENT);
  }

  void Server::SendG2SCommonMessage(MtPlayer* player, const std::string &name, const std::vector<int32>& int32values, const std::vector<int64>& int64values, const std::vector<std::string>& stringvalues)
  {
	  if (player == nullptr)
		  return;
	  auto message = boost::make_shared<G2S::G2SCommonMessage>();
	  message->set_request_name(name);
	  for (auto child : int32values) {
		  message->add_int32_params(child);
	  }
	  for (auto child : int64values) {
		  message->add_int64_params(child);
	  }
	  for (auto child : stringvalues) {
		  message->add_string_params(child);
	  }
	  player->ExecuteMessage(message);
  }

  void Server::SendS2GCommonMessage(const std::string &name, const std::vector<int32>& int32values, const std::vector<int64>& int64values, const std::vector<std::string>& stringvalues)
  {
	  //player 可能为空
	  auto message = boost::make_shared<S2G::S2GCommonMessage>();
	  message->set_request_name(name);
	  for (auto child : int32values) {
		  message->add_int32_params(child);
	  }
	  for (auto child : int64values) {
		  message->add_int64_params(child);
	  }
	  for (auto child : stringvalues) {
		  message->add_string_params(child);
	  }

	  SendMessage(message, nullptr);
  }


  void Server::BroadcastG2SCommonMessageToPlayer(const std::string &name, const std::vector<int32>& int32values, const std::vector<int64>& int64values, const std::vector<std::string>& stringvalues)
  {
	  auto message = boost::make_shared<G2S::G2SCommonMessage>();
	  message->set_request_name(name);
	  for (auto child : int32values) {
		  message->add_int32_params(child);
	  }
	  for (auto child : int64values) {
		  message->add_int64_params(child);
	  }
	  for (auto child : stringvalues) {
		  message->add_string_params(child);
	  }

	  MtSceneManager::Instance().BroadcastPlayerExcuteMessage(message);
  }

  void Server::BroadcastG2SCommonMessageToScene(const std::string &name, const std::vector<int32>& int32values, const std::vector<int64>& int64values, const std::vector<std::string>& stringvalues)
  {
	  auto message = boost::make_shared<G2S::G2SCommonMessage>();
	  message->set_request_name(name);
	  for (auto child : int32values) {
		  message->add_int32_params(child);
	  }
	  for (auto child : int64values) {
		  message->add_int64_params(child);
	  }
	  for (auto child : stringvalues) {
		  message->add_string_params(child);
	  }

	  MtSceneManager::Instance().BroadcastSceneExcuteMessage(message);
  }


  MtBattleGroundManager& Server::GetBattleManager() 
  {
	  return battle_manager_;
  }

  MtLuaCallProxy* Server::GetLuaCallProxy()
  {
	  return &lua_call_;
  }

  void Server::OnServerCommand()
  {
	  std::vector<std::string> command_list;
	  utils::enum_dir_files(SYSTEM_COMMAND_DIR, command_list);
	  for (auto& child : command_list){
		  if (child == "gm_mode" && info_ ){
			  std::string filepathname(SYSTEM_COMMAND_DIR);
			  filepathname += child;
			  ::remove(filepathname.c_str());

			  auto gm = MtConfig::Instance().gm_mode_;
			  MtConfig::Instance().gm_mode_ = !gm;
			  gm_mode = !gm;
			  LOG_INFO << "gm_mode by commandfile ";
		  }
		  else if(child == "shm_debug") {
			  std::string filepathname(SYSTEM_COMMAND_DIR);
			  filepathname += child;
			  ::remove(filepathname.c_str());

			  shm_debug_ = !shm_debug_;
			  LOG_INFO << "shm_debug by commandfile ";
		  }
		  else if (child == "pro_debug") {
			  std::string filepathname(SYSTEM_COMMAND_DIR);
			  filepathname += child;
			  ::remove(filepathname.c_str());

			  programer_debug_ = !programer_debug_;
			  LOG_INFO << "pro_debug by commandfile ";
		  }
		  else if (child == "shutdown") {
			  std::string filepathname(SYSTEM_COMMAND_DIR);
			  filepathname += child;
			  ::remove(filepathname.c_str());

			  OnShutdown();

			  LOG_INFO << "shut_down by commandfile ";
		  }
	  }
  }

  void Server::OffLine(const uint64 guid)
  {
	  boost::upgrade_lock<boost::shared_mutex> l(player_shared_mutex_);
	  LOG_INFO << "[Server Offline],player:" << guid
		  << ",empty:" << (online_player_.find(guid) == online_player_.end());
	  online_player_.erase(guid);
  }


  void Server::OnMqPayOrderCallBack(boost::shared_ptr<Packet::PayOrderInfo> payorder)
  {
	  if (payorder == nullptr)
	  {
		  ZXERO_ASSERT(false)<<"PayOrderInfo payorder null";
		  return;
	  }
	  auto player_ = FindPlayer(payorder->playerid());
	  if (player_ == nullptr)
	  {
		  ZXERO_ASSERT(false) << "PayOrderInfo Player null " << payorder->playerid() <<" orderid="<< payorder->orderid();
		  return;
	  }

	  Server::Instance().SendG2SCommonMessage(player_.get(), "G2SChargeItem", {}, {}, { payorder->orderid(),payorder->productid(),payorder->ip(),payorder->extra() });
	  
	  std::stringstream DBlog;
	  DBlog << "PayOrder," << player_->PlayerBibase() << "," << payorder->orderid()<<","<< payorder->productid()<<","<< payorder->chargemoney()<<","<< payorder->finalchargemoney();
	  Server::Instance().AddUrlTask(new db_log_require_work(DBlog.str(), __FILE__, __LINE__));

	  LOG_INFO << "PayOrderInfo OnMqPayOrderCallBack Send Message To Player playerid=" << payorder->playerid() << " orderid=" << payorder->orderid() << " serverid=" << payorder->serverid()<<" productid="<< payorder->productid();
  }

  Server::~Server()
  {
	  thread_lua.reset(nullptr);
  }

}
