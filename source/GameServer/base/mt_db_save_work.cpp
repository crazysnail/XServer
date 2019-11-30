#include "mt_db_save_work.h"
#include "../mt_guid/mt_guid.h"
#include "../mt_player/mt_player.h"
#include "../mt_actor/mt_actor.h"
#include "../mt_skill/mt_skill.h"
#include "../mt_stage/mt_scene_stage.h"
#include "../mt_guild/mt_guild_manager.h"
#include "../mt_battle/mt_worldboss_manager.h"
#include "../mt_arena/mt_arena.h"
#include "../mt_cache/mt_shm_manager.h"

#include <LoginPacket.pb.h>
#include <ActorBasicInfo.pb.h>
#include <FriendMail.pb.h>
#include <SceneStage.pb.h>
#include <ItemAndEquip.pb.h>
#include <MissionPacket.pb.h>
#include <DataCellPacket.pb.h>
#include <TrialFieldDB.pb.h>
#include <ArenaDb.pb.h>
#include <active_model.h>
#include <Guild.pb.h>
#include <WorldBoss.pb.h>
#include <GuildCapturePoint.pb.h>

#include "../mt_server/mt_server.h"
#include <active_model.h>
#include <algorithm>


namespace Mt
{

	using namespace Packet;

	void DBSaveHelper(boost::shared_ptr<zxero::dbcontext>& dbctx, const shm_object* shm_obj)
	{
		if (shm_obj == nullptr) {
			return;
		}
		boost::scoped_ptr<google::protobuf::Message> msg_(MtShmManager::GetMessageByName(shm_obj->name));
		if (msg_ == nullptr) {
			return;
		}

		try
		{
			msg_->ParseFromArray(shm_obj->data, shm_obj->length);
		}
		catch (std::exception& e) {
			LOG_ERROR << "MessageSaveWork: proto buffer unseirielize failed! msg name =" << shm_obj->name << " exception :" << e.what();
			return;
		}

		std::string name = msg_->GetTypeName();
		if (shm_obj->Deleted())
		{
			if (name == "Packet.AccountModel")
				ActiveModel::Base<Packet::AccountModel>(dbctx).Delete(*msg_);
			else if (name == "Packet.PlayerDBInfo")
				ActiveModel::Base<Packet::PlayerDBInfo>(dbctx).Delete(*msg_);
			else if (name == "Packet.ActorBasicInfo")
				ActiveModel::Base<Packet::ActorBasicInfo>(dbctx).Delete(*msg_);
			else if (name == "Packet.ActorSkill")
				ActiveModel::Base<Packet::ActorSkill>(dbctx).Delete(*msg_);
			else if (name == "Packet.SceneStageInfo")
				ActiveModel::Base<Packet::SceneStageInfo>(dbctx).Delete(*msg_);
			else if (name == "Packet.PlayerStageDB")
				ActiveModel::Base<Packet::PlayerStageDB>(dbctx).Delete(*msg_);
			else if (name == "Packet.PlayerStage")
				ActiveModel::Base<Packet::PlayerStage>(dbctx).Delete(*msg_);
			else if (name == "Packet.Container")
				ActiveModel::Base<Packet::Container>(dbctx).Delete(*msg_);
			else if (name == "Packet.NormalItem")
				ActiveModel::Base<Packet::NormalItem>(dbctx).Delete(*msg_);
			else if (name == "Packet.EquipItem")
				ActiveModel::Base<Packet::EquipItem>(dbctx).Delete(*msg_);
			else if (name == "Packet.GemItem")
				ActiveModel::Base<Packet::GemItem>(dbctx).Delete(*msg_);
			else if (name == "Packet.Market")
				ActiveModel::Base<Packet::Market>(dbctx).Delete(*msg_);
			else if (name == "Packet.ActorFormation")
				ActiveModel::Base<Packet::ActorFormation>(dbctx).Delete(*msg_);
			else if (name == "Packet.Mission")
				ActiveModel::Base<Packet::Mission>(dbctx).Delete(*msg_);
			else if (name == "Packet.MessageListPlayerInfo")
				ActiveModel::Base<Packet::MessageListPlayerInfo>(dbctx).Delete(*msg_);
			else if (name == "Packet.FriendPlayer")
				ActiveModel::Base<Packet::FriendPlayer>(dbctx).Delete(*msg_);
			else if (name == "Packet.ZoneInfo")
				ActiveModel::Base<Packet::ZoneInfo>(dbctx).Delete(*msg_);
			else if (name == "Packet.MessageBoard")
				ActiveModel::Base<Packet::MessageBoard>(dbctx).Delete(*msg_);
			else if (name == "Packet.MessageInfo")
				ActiveModel::Base<Packet::MessageInfo>(dbctx).Delete(*msg_);
			else if (name == "Packet.FriendMessageInfo")
				ActiveModel::Base<Packet::FriendMessageInfo>(dbctx).Delete(*msg_);
			else if (name == "Packet.DataCell")
				ActiveModel::Base<Packet::DataCell>(dbctx).Delete(*msg_);
			else if (name == "Packet.Data64Cell")
				ActiveModel::Base<Packet::Data64Cell>(dbctx).Delete(*msg_);
			else if (name == "Packet.GuildUser")
				ActiveModel::Base<Packet::GuildUser>(dbctx).Delete(*msg_);
			else if (name == "Packet.GuildDBInfo")
				ActiveModel::Base<Packet::GuildDBInfo>(dbctx).Delete(*msg_);
			else if (name == "Packet.GuildBossCompetitionDbInfo")
				ActiveModel::Base<Packet::GuildBossCompetitionDbInfo>(dbctx).Delete(*msg_);
			else if (name == "Packet.GuildBattleInfo")
				ActiveModel::Base<Packet::GuildBattleInfo>(dbctx).Delete(*msg_);
			else if (name == "Packet.WorldBossInfo")
				ActiveModel::Base<Packet::WorldBossInfo>(dbctx).Delete(*msg_);
			else if (name == "Packet.GuildWetCopyInfo")
				ActiveModel::Base<Packet::GuildWetCopyInfo>(dbctx).Delete(*msg_);
			else if (name == "Packet.GuildUserDamage")
				ActiveModel::Base<Packet::GuildUserDamage>(dbctx).Delete(*msg_);
			else if (name == "Packet.GuildWarehouse")
				ActiveModel::Base<Packet::GuildWarehouse>(dbctx).Delete(*msg_);
			else if (name == "Packet.GuildWarehouseRecord")
				ActiveModel::Base<Packet::GuildWarehouseRecord>(dbctx).Delete(*msg_);
			else if (name == "Packet.GuildSpoilItem")
				ActiveModel::Base<Packet::GuildSpoilItem>(dbctx).Delete(*msg_);
			else if (name == "Packet.GuildRecord")
				ActiveModel::Base<Packet::GuildRecord>(dbctx).Delete(*msg_);
			else if (name == "DB.PlayerTrialInfo")
				ActiveModel::Base<DB::PlayerTrialInfo>(dbctx).Delete(*msg_);
			else if (name == "DB.ArenaUser")
				ActiveModel::Base<DB::ArenaUser>(dbctx).Delete(*msg_);
			else if (name == "Packet.SimpleImpactData")
				ActiveModel::Base<Packet::SimpleImpactData>(dbctx).Delete(*msg_);
			else if (name == "Packet.ServerInfo")
				ActiveModel::Base<Packet::ServerInfo>(dbctx).Delete(*msg_);
			else if (name == "Packet.PracticeInfo")
				ActiveModel::Base<Packet::PracticeInfo>(dbctx).Delete(*msg_);
			else if (name == "Packet.BattleReply")
				ActiveModel::Base<Packet::BattleReply>(dbctx).Delete(*msg_);
			else if (name == "DB.PlayerTowerInfo")
				ActiveModel::Base<DB::PlayerTowerInfo>(dbctx).Delete(*msg_);
			else if (name == "Packet.ShmHead") {
				//nothing todo
			}
			else
				ZXERO_ASSERT(false) << "invalid db message! name = " << name;

			//
			LOG_INFO << "#SAVE_WORK# delete done!" << msg_->GetDescriptor()->name() << " key=" << shm_obj->key;
		}
		else if (shm_obj->Dirty()) {
			if (name == "Packet.AccountModel")
				ActiveModel::Base<Packet::AccountModel>(dbctx).Save(*msg_);
			else if (name == "Packet.PlayerDBInfo")
				ActiveModel::Base<Packet::PlayerDBInfo>(dbctx).Save(*msg_);
			else if (name == "Packet.ActorBasicInfo")
				ActiveModel::Base<Packet::ActorBasicInfo>(dbctx).Save(*msg_);
			else if (name == "Packet.PlayerSetting")
				ActiveModel::Base<Packet::PlayerSetting>(dbctx).Save(*msg_);
			else if (name == "Packet.ActorSkill")
				ActiveModel::Base<Packet::ActorSkill>(dbctx).Save(*msg_);
			else if (name == "Packet.PlayerStage")
				ActiveModel::Base<Packet::PlayerStage>(dbctx).Save(*msg_);
			else if (name == "Packet.PlayerStageDB")
				ActiveModel::Base<Packet::PlayerStageDB>(dbctx).Save(*msg_);
			else if (name == "Packet.SceneStageInfo")
				ActiveModel::Base<Packet::SceneStageInfo>(dbctx).Save(*msg_);
			else if (name == "Packet.Container")
				ActiveModel::Base<Packet::Container>(dbctx).Save(*msg_);
			else if (name == "Packet.NormalItem")
				ActiveModel::Base<Packet::NormalItem>(dbctx).Save(*msg_);
			else if (name == "Packet.EquipItem")
				ActiveModel::Base<Packet::EquipItem>(dbctx).Save(*msg_);
			else if (name == "Packet.GemItem")
				ActiveModel::Base<Packet::GemItem>(dbctx).Save(*msg_);
			else if (name == "Packet.Market")
				ActiveModel::Base<Packet::Market>(dbctx).Save(*msg_);
			else if (name == "Packet.ActorFormation")
				ActiveModel::Base<Packet::ActorFormation>(dbctx).Save(*msg_);
			else if (name == "Packet.Mission")
				ActiveModel::Base<Packet::Mission>(dbctx).Save(*msg_);
			else if (name == "Packet.MessageListPlayerInfo")
				ActiveModel::Base<Packet::MessageListPlayerInfo>(dbctx).Save(*msg_);
			else if (name == "Packet.FriendPlayer")
				ActiveModel::Base<Packet::FriendPlayer>(dbctx).Save(*msg_);
			else if (name == "Packet.ZoneInfo")
				ActiveModel::Base<Packet::ZoneInfo>(dbctx).Save(*msg_);
			else if (name == "Packet.MessageBoard")
				ActiveModel::Base<Packet::MessageBoard>(dbctx).Save(*msg_);
			else if (name == "Packet.MessageInfo")
				ActiveModel::Base<Packet::MessageInfo>(dbctx).Save(*msg_);
			else if (name == "Packet.FriendMessageInfo")
				ActiveModel::Base<Packet::FriendMessageInfo>(dbctx).Save(*msg_);
			else if (name == "Packet.DataCell")
				ActiveModel::Base<Packet::DataCell>(dbctx).Save(*msg_);
			else if (name == "Packet.Data64Cell")
				ActiveModel::Base<Packet::Data64Cell>(dbctx).Save(*msg_);
			else if (name == "Packet.GuildUser")
				ActiveModel::Base<Packet::GuildUser>(dbctx).Save(*msg_);
			else if (name == "Packet.GuildDBInfo")
				ActiveModel::Base<Packet::GuildDBInfo>(dbctx).Save(*msg_);
			else if (name == "Packet.GuildBossCompetitionDbInfo")
				ActiveModel::Base<Packet::GuildBossCompetitionDbInfo>(dbctx).Save(*msg_);
			else if (name == "Packet.GuildBattleInfo")
				ActiveModel::Base<Packet::GuildBattleInfo>(dbctx).Save(*msg_);
			else if (name == "Packet.WorldBossInfo")
				ActiveModel::Base<Packet::WorldBossInfo>(dbctx).Save(*msg_);
			else if (name == "Packet.GuildWetCopyInfo")
				ActiveModel::Base<Packet::GuildWetCopyInfo>(dbctx).Save(*msg_);
			else if (name == "Packet.GuildUserDamage")
				ActiveModel::Base<Packet::GuildUserDamage>(dbctx).Save(*msg_);
			else if (name == "Packet.GuildWarehouse")
				ActiveModel::Base<Packet::GuildWarehouse>(dbctx).Save(*msg_);
			else if (name == "Packet.GuildWarehouseRecord")
				ActiveModel::Base<Packet::GuildWarehouseRecord>(dbctx).Save(*msg_);
			else if (name == "Packet.GuildSpoilItem")
				ActiveModel::Base<Packet::GuildSpoilItem>(dbctx).Save(*msg_);
			else if (name == "Packet.GuildRecord")
				ActiveModel::Base<Packet::GuildRecord>(dbctx).Save(*msg_);
			else if (name == "DB.PlayerTrialInfo")
				ActiveModel::Base<DB::PlayerTrialInfo>(dbctx).Save(*msg_);
			else if (name == "DB.ArenaUser")
				ActiveModel::Base<DB::ArenaUser>(dbctx).Save(*msg_);
			else if (name == "Packet.SimpleImpactData")
				ActiveModel::Base<Packet::SimpleImpactData>(dbctx).Save(*msg_);
			else if (name == "Packet.ServerInfo")
				ActiveModel::Base<Packet::ServerInfo>(dbctx).Save(*msg_);
			else if (name == "Packet.CapturePointDB")
				ActiveModel::Base<Packet::CapturePointDB>(dbctx).Save(*msg_);
			else if (name == "Packet.GuildCaptureBattleLogDB")
				ActiveModel::Base<Packet::GuildCaptureBattleLogDB>(dbctx).Save(*msg_);
			else if (name == "Packet.GuildCapturePointDB")
				ActiveModel::Base<Packet::GuildCapturePointDB>(dbctx).Save(*msg_);
			else if (name == "Packet.PracticeInfo")
				ActiveModel::Base<Packet::PracticeInfo>(dbctx).Save(*msg_);
			else if (name == "Packet.BattleReply")
				ActiveModel::Base<Packet::BattleReply>(dbctx).Save(*msg_);
			else if (name == "DB.PlayerTowerInfo")
				ActiveModel::Base<DB::PlayerTowerInfo>(dbctx).Save(*msg_);
			else if (name == "Packet.ShmHead") {
				//nothing todo
			}
			else
				ZXERO_ASSERT(false) << "invalid db message! name = " << name;
			//
			LOG_INFO << "#SAVE_WORK# update done!" << msg_->GetDescriptor()->name() << " key=" << shm_obj->key;
		}
	}

	///////////////////////////////////////////////////////////////////
	IMPLEMENT_DBWORK(MessageSaveWork)
	{
		if (shm_obj_ == nullptr)
			return;

		DBSaveHelper(dbctx,shm_obj_);
	}

	MessageSaveWork::MessageSaveWork(const shm_object* shm_obj)
		:shm_obj_(shm_obj)
	{
		if (shm_obj_ == nullptr) {
			ZXERO_ASSERT(false) << "msg is null!";
			return;
		}			
	}
	void MessageSaveWork::done()
	{
		dbwork::done();
		MtShmManager::Instance().MarkMsgSaved();
	}

	///////////////////////////////////////////////////////////////////////////////
	IMPLEMENT_DBWORK(MessageSetSaveWork)
	{
		if (shm_obj_set_.empty())
			return;

		for (auto shm_obj : shm_obj_set_) {
			try {
				DBSaveHelper(dbctx, shm_obj);
			}
			catch (sql::SQLException& e) {
				//语法异常直接跳过，断线异常抛出重连! 重连成功后会重新执行当前work!
				sql_error_ = e.getErrorCode();
				if (sql_error_ == 2003 || sql_error_ == 2013 || sql_error_ == 2006 ) {
					throw e;
				}
				else{
					ZXERO_ASSERT(false) << "msg name=" << shm_obj->name << " save work " << name() << " sql failed , error code="<<sql_error_<<"error exception: " << e.what();
				}
			}
		}
	}

	MessageSetSaveWork::MessageSetSaveWork(const std::list<shm_object*>& shm_obj_set)
		:shm_obj_set_(shm_obj_set)
	{
	}
	void MessageSetSaveWork::done()
	{
		dbwork::done();
		MtShmManager::Instance().MarkMsgSaved();
	}

	////////////////////////////////////////////////////////////////////////////////
	IMPLEMENT_DBWORK(MessageSaveAllWork)
	{
		dbctx;
		MtShmManager::Instance().Shm2Save();
	}

	MessageSaveAllWork::MessageSaveAllWork()
	{
	}
	void MessageSaveAllWork::done()
	{		
		dbwork::done();
		Server::Instance().SaveDone();
	}

	IMPLEMENT_DBWORK(ArenaUserCheckInit)
	{
		MtArenaManager::Instance().OnCheckInit(dbctx);
	}

  IMPLEMENT_DBWORK(ShareMemoryCrashRecover)
  {
    dbctx;	
    //do recover
	MtShmManager::Instance().ShmRecoverSave();
  }

  void ShareMemoryCrashRecover::done()
  {
    Server::Instance().OnStartUpSaveShareMemDataDone();
  }

  IMPLEMENT_DBWORK(SaveOfflineMail)
  {
	  for (auto mail : all_db_mail_)
	  {
		  ActiveModel::Base<Packet::MessageInfo>(dbctx).Save(*mail.get());
		  LOG_INFO << "SaveOfflineMail : "<< mail->guid() << ","<< mail->owner_id();
	  }
  }
  SaveOfflineMail::SaveOfflineMail(const std::vector<boost::shared_ptr<Packet::MessageInfo>>& all_db_mail)
  {
	  std::copy(all_db_mail.begin(), all_db_mail.end(), std::back_inserter(all_db_mail_));
  }

  IMPLEMENT_DBWORK(SaveMqPayOrder)
  {
	  if (payorder_)
	  {
		  if (ActiveModel::Base<Packet::PayOrderInfo>(dbctx).CheckExist({ { "orderid", payorder_->orderid() } })) {
			  ZXERO_ASSERT(false) << "have same order pay " << payorder_->orderid() << " playerid=" << payorder_->playerid();
		  }
		  else
		  {
			  ActiveModel::Base<Packet::PayOrderInfo>(dbctx).Save(*payorder_.get());
			  ret = true;
		  }
	  }
  }

  void SaveMqPayOrder::done()
  {
	  if (ret)
	  {
		  Server::Instance().OnMqPayOrderCallBack(payorder_);
	  }
	  else
	  {
		  if (payorder_)
		  {
			  ZXERO_ASSERT(false) << "save error order pay " << payorder_->orderid() << " playerid=" << payorder_->playerid();
		  }
		  else
		  {
			  ZXERO_ASSERT(false) << "save error order pay payorder_ null";
		  }
		  
	  }	  
  }
  SaveMqPayOrder::SaveMqPayOrder(boost::shared_ptr<Packet::PayOrderInfo> payorder):payorder_(payorder)
  {

  }
}