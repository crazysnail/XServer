#include "mt_migrate.h"
#include "../mt_config/mt_config.h"
#include "../mt_guid/mt_guid.h"
#include "module.h"
#include <Base.pb.h>
#include "LoginPacket.pb.h"
#include "ActorBasicInfo.pb.h"
#include "GuildCapturePoint.pb.h"
#include "FriendMail.pb.h"
#include <SceneStage.pb.h>
#include <ItemAndEquip.pb.h>
#include <MissionPacket.pb.h>
#include <DataCellPacket.pb.h>
#include <TrialFieldDB.pb.h>
#include <ArenaDb.pb.h>
#include <TrialFieldDB.pb.h>
#include <active_model.h>
#include <Guild.pb.h>
#include <WorldBoss.pb.h>
#include <SimpleImpact.pb.h>


namespace Mt
{
	static MtMigrate* __mt_migrate = nullptr;
	REGISTER_MODULE(MtMigrate)
	{
		require("MtConfig");
		require("MtGuid");
		register_load_function(module_base::STAGE_LOAD, boost::bind(&MtMigrate::OnLoad, boost::ref(MtMigrate::Instance())));
	}
	int MtMigrate::OnLoad()
	{
		boost::shared_ptr<dbcontext> ctx = MtConfig::Instance().CreateDbContext(false);
		auto migrate = ActiveModel::Migration<sql::Connection>(ctx->get_connection(), MtConfig::Instance().db_name_);
		migrate.RegistTable(ActiveModel::Base<Packet::AccountModel>(ctx));
		migrate.RegistTable(ActiveModel::Base<Packet::PlayerDBInfo>(ctx));
		migrate.RegistTable(ActiveModel::Base<Packet::ActorBasicInfo>(ctx));
		migrate.RegistTable(ActiveModel::Base<Packet::ActorSkill>(ctx));
		migrate.RegistTable(ActiveModel::Base<Packet::Container>(ctx));
		migrate.RegistTable(ActiveModel::Base<Packet::NormalItem>(ctx));
		migrate.RegistTable(ActiveModel::Base<Packet::EquipItem>(ctx));
		migrate.RegistTable(ActiveModel::Base<Packet::GemItem>(ctx));
		migrate.RegistTable(ActiveModel::Base<Packet::Market>(ctx));
		migrate.RegistTable(ActiveModel::Base<Packet::ActorFormation>(ctx));
		migrate.RegistTable(ActiveModel::Base<Packet::Mission>(ctx));
		migrate.RegistTable(ActiveModel::Base<Packet::MessageListPlayerInfo>(ctx));
		migrate.RegistTable(ActiveModel::Base<Packet::FriendPlayer>(ctx));
		migrate.RegistTable(ActiveModel::Base<Packet::ZoneInfo>(ctx));
		migrate.RegistTable(ActiveModel::Base<Packet::MessageBoard>(ctx));
		migrate.RegistTable(ActiveModel::Base<Packet::MessageInfo>(ctx));
		migrate.RegistTable(ActiveModel::Base<Packet::FriendMessageInfo>(ctx));
		migrate.RegistTable(ActiveModel::Base<Packet::SceneStageInfo>(ctx));
		migrate.RegistTable(ActiveModel::Base<Packet::DataCell>(ctx));
		migrate.RegistTable(ActiveModel::Base<Packet::Data64Cell>(ctx));
		migrate.RegistTable(ActiveModel::Base<Packet::GuildUser>(ctx));
		migrate.RegistTable(ActiveModel::Base<Packet::GuildDBInfo>(ctx));
		migrate.RegistTable(ActiveModel::Base<Packet::GuildBossCompetitionDbInfo>(ctx));
		migrate.RegistTable(ActiveModel::Base<Packet::WorldBossInfo>(ctx));
		migrate.RegistTable(ActiveModel::Base<Packet::GuildWetCopyInfo>(ctx));
		migrate.RegistTable(ActiveModel::Base<Packet::GuildWarehouse>(ctx));
		migrate.RegistTable(ActiveModel::Base<Packet::GuildWarehouseRecord>(ctx));
		migrate.RegistTable(ActiveModel::Base<Packet::GuildSpoilItem>(ctx));
		migrate.RegistTable(ActiveModel::Base<Packet::GuildRecord>(ctx));
		migrate.RegistTable(ActiveModel::Base<Packet::GuildUserDamage>(ctx));
		migrate.RegistTable(ActiveModel::Base<Packet::GuildBattleInfo>(ctx));
		migrate.RegistTable(ActiveModel::Base<Packet::PracticeInfo>(ctx));
		migrate.RegistTable(ActiveModel::Base<DB::PlayerTrialInfo>(ctx));
		migrate.RegistTable(ActiveModel::Base<DB::ArenaUser>(ctx));
		migrate.RegistTable(ActiveModel::Base<Packet::SimpleImpactData>(ctx));
		migrate.RegistTable(ActiveModel::Base<Packet::ServerInfo>(ctx));
		migrate.RegistTable(ActiveModel::Base<Packet::PlayerStageDB>(ctx));
		migrate.RegistTable(ActiveModel::Base<Packet::PlayerStage>(ctx));
		migrate.RegistTable(ActiveModel::Base<Packet::PlayerSetting>(ctx));
		migrate.RegistTable(ActiveModel::Base<Packet::CapturePointDB>(ctx));
		migrate.RegistTable(ActiveModel::Base<Packet::GuildCapturePointDB>(ctx));
		migrate.RegistTable(ActiveModel::Base<Packet::GuildCaptureBattleLogDB>(ctx));
		migrate.RegistTable(ActiveModel::Base<Packet::BattleReply>(ctx));
		migrate.RegistTable(ActiveModel::Base<Packet::PayOrderInfo>(ctx));
		migrate.RegistTable(ActiveModel::Base<DB::PlayerTowerInfo>(ctx));
		migrate.Migrate();

		all_tables_ = migrate.GetTables();	
		int32 count = 1;
		for (auto child : all_tables_) {
			key_tables_.insert({ child.first, count++ });
		}
		return 0;
	}

	UniqueValue MtMigrate::GetMsgUniqueValue(const google::protobuf::Message& msg)
	{
		auto descriptor = msg.GetDescriptor();
		auto reflection = msg.GetReflection();

		auto name = descriptor->name();
		auto iter = all_tables_.find(name);
		if (iter == all_tables_.end()) {
			if (name == "ShmHead") {//shmhead 特殊处理一下
				return{ name, 99999999999 };
			}
			else {
				ZXERO_ASSERT(false) << "invalid db message! name=" << name;
				return{ "",INVALID_GUID };
			}
		}

		std::vector<std::string> key_names;
		for (auto child : iter->second) {
			if (child.primary_key_) {
				key_names.push_back(child.name_);
			}
		}
		if (key_names.empty() || key_names.size() > 2) {
			ZXERO_ASSERT(false) << "invalid primary_key count! name=" << name;
			return{ "",INVALID_GUID };
		}

		std::vector<uint64> values;
		for (auto child : key_names) {
			auto field_count = descriptor->field_count();
			for (auto i = 0; i < field_count; ++i) {
				auto field_desc = descriptor->field(i);
				if (field_desc->is_optional()) {
					continue;
				}
				if (field_desc->type() == google::protobuf::FieldDescriptor::Type::TYPE_MESSAGE) {
					continue;
				}

				if (field_desc->name() == child) {
					switch (field_desc->cpp_type()){
					case google::protobuf::FieldDescriptor::CppType::CPPTYPE_INT32:
						values.push_back((zxero::uint64)reflection->GetInt32(msg, field_desc));
						break;
					case google::protobuf::FieldDescriptor::CppType::CPPTYPE_INT64:
						values.push_back((zxero::uint64)reflection->GetInt64(msg, field_desc));
						break;
					case google::protobuf::FieldDescriptor::CppType::CPPTYPE_UINT32:
						values.push_back((zxero::uint64)reflection->GetUInt32(msg, field_desc));
						break;
					case google::protobuf::FieldDescriptor::CppType::CPPTYPE_UINT64:
						values.push_back((zxero::uint64)reflection->GetUInt64(msg, field_desc));
						break;
					case google::protobuf::FieldDescriptor::CppType::CPPTYPE_ENUM:
						values.push_back((zxero::uint64)reflection->GetEnum(msg, field_desc));
						break;
					default:
						ZXERO_ASSERT(false)<<"invalid primary_key type! name=" << name;
					}

					
				}
			}
		}

		if (key_names.size() != values.size()) {
			ZXERO_ASSERT(false)<< "invalid primary_key count! name=" << name;
			return{ "",INVALID_GUID };
		}
		if (key_tables_.find(name) == key_tables_.end()) {
			ZXERO_ASSERT(false) << "invalid msg key! name=" << name;
			return{ "",INVALID_GUID };
		}

		uint64 key = key_tables_[name];
		//8 | 24 | 32
		if (values.size() == 1) {
			return{ name, (key << 56) | (values[0] << 40) | 1 };
		}
		else if (values.size() == 2) {//取低32拼接  
		 	return{ name, (key << 56) | (values[0] << 40) | (values[1] & 0x00000000ffffffff) };
		}
		else {
			ZXERO_ASSERT(false) << "invalid primary_key count! name=" << name;
			return{ "",INVALID_GUID };
		}
	}

	int MtMigrate::OnPostLoad()
	{
		return 0;
	}

	MtMigrate& MtMigrate::Instance()
	{
		return *__mt_migrate;
	}

	MtMigrate::MtMigrate()
	{
		__mt_migrate = this;
	}

}