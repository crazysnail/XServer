#include "mt_skill.h"
#include "skill_context.h"
#include "module.h"
#include "../mt_actor/mt_actor.h"
#include "../data/data_manager.h"
#include "../mt_battle/mt_battle_ground.h"
#include "../mt_skill/mt_buff.h"
#include "../mt_battle/mt_trap.h"
#include "../mt_actor/mt_battle_state.h"
#include "../mt_battle/battle_event.h"
#include "../mt_monster/mt_monster_manager.h"
#include "../mt_config/mt_config.h"
#include "json/json.h"
#include "utils.h"
#include "random_generator.h"
#include "mt_skill_logic.h"
#include "mt_target_select.h"
#include <fstream>
#include <SkillConfig.pb.h>
#include <BattleExpression.pb.h>
#include <ActorBasicInfo.pb.h>
#include <TrapConfig.pb.h>
#include <boost/format.hpp>
namespace Mt
{
	using BuffImpl::BuffFactory;
	static MtSkillManager* __mt_skill_manager = nullptr;
	REGISTER_MODULE(MtSkillManager)
	{
		require("data_manager");
		register_load_function(module_base::STAGE_LOAD, boost::bind(&MtSkillManager::OnLoad, boost::ref(MtSkillManager::Instance())));
		register_option("validate_skill", "validate skill json file at the very beginning, default is false",
			boost::program_options::value<bool>()->default_value(false),
			opt_delegate<bool>(boost::bind(&MtSkillManager::OnValidateSkill, __mt_skill_manager, _1))
		);
	}

	void MtSkillManager::lua_reg(lua_State* ls)
	{
		ff::fflua_register_t<MtSkillManager, ctor()>(ls, "MtSKillManager")
			.def(&MtSkillManager::InitLuaContext, "InitLuaContext")
			;

		ff::fflua_register_t<>(ls)
			.def(&MtSkillManager::Instance, "LuaMtSkillManager");
	}

	MtSkillManager& MtSkillManager::Instance()
	{
		return *__mt_skill_manager;
	}

	void MtSkillManager::OnValidateSkill(bool validate)
	{
		if (validate) {
			ff::fflua_t* lua = nullptr;
			if (thread_lua.get() == nullptr) {
				lua = new ff::fflua_t();
				thread_lua.reset(lua);
			}
			InitLuaContext();
		/*	delete lua;
			thread_lua.reset(nullptr);*/
		}
	}

	MtSkillManager::MtSkillManager()
	{
		__mt_skill_manager = this;
	}

	void MtSkillManager::InitLuaContext()
	{
		SkillContext::InitLua();
		std::vector<std::string> files;
		utils::enum_dir_files(MtConfig::Instance().conf_json_dir_ + "skills/", files);
		for (auto& file : files){
				__InitLuaContext(MtConfig::Instance().conf_json_dir_ + "skills/" + file);
		}
		files.clear();
		utils::enum_dir_files(MtConfig::Instance().conf_json_dir_ + "skills/suits/", files);
		for (auto& file : files) {
			__InitLuaContext(MtConfig::Instance().conf_json_dir_ + "skills/suits/" + file);
		}
	}

	void MtSkillManager::__InitLuaContext(const std::string& json_file)
	{
		std::ifstream skill_file(json_file);
		Json::Value root;
		try
		{
			skill_file >> root;	
		}
		catch (std::exception& exp)
		{
			LOG_FATAL << json_file <<  exp.what();
		}
		skill_file.close();
		for (auto& skill : root)
		{
			int32 skill_id = skill["id"].asInt();
			ZXERO_ASSERT(skill.isObject() == true) << json_file << " error format";
			if (skill.isMember("cd") == false)
				skill["cd"] = 0;
			if (skill.isMember("repeat"))
				ZXERO_ASSERT(skill.isMember("repeat_interval"));
			ParseSkill(skill);
			json_skill_map_[skill_id] = boost::make_shared<Json::Value>(skill);
		}
	}

	void MtSkillManager::ParseSkill(Json::Value& skill_define)
	{
		ZXERO_ASSERT(skill_define.isMember("id")) << "should have id";
		int32 skill_id = skill_define["id"].asInt();
		//技能目标选择逻辑
		if (skill_define.isMember("tar_logic") == false) {
			skill_define["tar_logic"] = Json::Value(Json::ValueType::objectValue);
			skill_define["tar_logic"]["type"] = "self";
		}
		if (skill_define.isMember("passive")) {
			auto keys = skill_define.getMemberNames();
			auto it = boost::find_if(keys, [](auto& key) {return key.rfind("stub") != std::string::npos; });
			ZXERO_ASSERT(it != keys.end()) << "passive skill must has some stub";
		}
		TarLogicCheck(skill_define);
		for (auto& key : skill_define.getMemberNames()) {
			if (key.rfind("value") != std::string::npos || key.rfind("stub") != std::string::npos) {
				std::string func_name = (boost::format("skill_%1%_%2%") % skill_id % key).str();
				std::string func_content;
				if (skill_define[key].isString())
					func_content = skill_define[key].asString();
				else if (skill_define[key].isArray()){
					auto& multi_line = skill_define[key];
					for (auto& line : multi_line)
						func_content += line.asString() + "\n";
				} else {
					ZXERO_ASSERT(false) << "must be single line or multi line string";
				}
				ZXERO_ASSERT(thread_lua->addreturn(func_content));
				std::string func_define = (boost::format("function %1%(ctx) %2% end") % func_name % func_content).str();
				ZXERO_ASSERT(thread_lua->execute_str(func_define, "=skill"));
				skill_define[key + "_func"] = func_name;
			}
		}
		//部分被动技能包含的主动技能
		if (skill_define.isMember("active_skill")) {
			auto& active_skill = skill_define["active_skill"];
			active_skill["id"] = skill_id;
			ParseSkill(active_skill);
		}
		//部分主动技能包含有被动技能效果
		if (skill_define.isMember("passive_skill")) {
			auto& passive_skill = skill_define["passive_skill"];
			passive_skill["id"] = skill_id;
			ParseSkill(passive_skill);
		}
		if (skill_define.isMember("extra_buff")) {
			ExtraBuffParse(skill_id, skill_define["extra_buff"]);
		}
		//buffs
		if (skill_define.isMember("self_buff") ) {
			auto& buff_define = skill_define["self_buff"];
			for (uint32 i = 0; i < buff_define.size(); ++i) {
				if (buff_define[i].isMember("extra_buff")) {
					ExtraBuffParse(skill_id, buff_define[i]["extra_buff"]);
				}
				auto keys = buff_define[i].getMemberNames();
				for (auto& key : keys) {
					if (key.rfind("value") != std::string::npos || key.rfind("stub") != std::string::npos) {
						std::string func_name = (boost::format("skill_%1%_self_buff_%2%_%3%") % skill_id % i % key).str();
						std::string func_content;
						if (buff_define[i][key].isString())
							func_content = buff_define[i][key].asString();
						else if (buff_define[i][key].isArray()) {
							auto& multi_line = buff_define[i][key];
							for (auto& line : multi_line)
								func_content += line.asString() + "\n";
						} else {
							ZXERO_ASSERT(false) << "must be single line or multi line string";
						}
						ZXERO_ASSERT(thread_lua->addreturn(func_content));
						std::string func_define = (boost::format("function %1%(ctx) %2% end") % func_name % func_content).str();
						ZXERO_ASSERT(thread_lua->execute_str(func_define, "=skill"));
						buff_define[i][key + "_func"] = func_name;
					}
				}
			}
		}
		if (skill_define.isMember("target_buff")) {
			auto& buff_define = skill_define["target_buff"];
			for (uint32 i = 0; i < buff_define.size(); ++i) {
				if (buff_define[i].isMember("extra_buff")) {
					ExtraBuffParse(skill_id, buff_define[i]["extra_buff"]);
				}
				auto keys = buff_define[i].getMemberNames();
				for (auto& key : keys) {
					if (key.rfind("value") != std::string::npos || key.rfind("stub") != std::string::npos) {
						std::string func_name = (boost::format("skill_%1%_target_buff_%2%_%3%") % skill_id % i % key).str();
						std::string func_content;
						if (buff_define[i][key].isString())
							func_content = buff_define[i][key].asString();
						else if (buff_define[i][key].isArray()) {
							auto& multi_line = buff_define[i][key];
							for (auto& line : multi_line)
								func_content += line.asString() + "\n";
						} else {
							ZXERO_ASSERT(false) << "must be single line or multi line string";
						}
						ZXERO_ASSERT(thread_lua->addreturn(func_content));
						std::string func_define = (boost::format("function %1%(ctx) %2% end") % func_name % func_content).str();
						ZXERO_ASSERT(thread_lua->execute_str(func_define, "=skill"));
						buff_define[i][key + "_func"] = func_name;
					}
				}
			}
		}
		if (skill_define.isMember("random_traps")) {
			auto& trap_define = skill_define["random_traps"];
			for (uint32 i = 0; i < trap_define.size(); ++i) {
				if (trap_define[i].isMember("extra_buff")) {
					ExtraBuffParse(skill_id, trap_define[i]["extra_buff"]);
				}
				auto keys = trap_define[i].getMemberNames();
				for (auto& key : keys) {
					if (key.rfind("value") != std::string::npos || key.rfind("stub") != std::string::npos) {
						std::string func_name = (boost::format("skill_%1%_random_trap_%2%_%3%") % skill_id % i % key).str();
						std::string func_content;
						if (trap_define[i][key].isString())
							func_content = trap_define[i][key].asString();
						else if (trap_define[i][key].isArray()) {
							auto& multi_line = trap_define[i][key];
							for (auto& line : multi_line)
								func_content += line.asString() + "\n";
						} else {
							ZXERO_ASSERT(false) << "must be single line or multi line string";
						}
						ZXERO_ASSERT(thread_lua->addreturn(func_content));
						std::string func_define = (boost::format("function %1%(ctx) %2% end") % func_name % func_content).str();
						ZXERO_ASSERT(thread_lua->execute_str(func_define, "=skill"));
						trap_define[i][key + "_func"] = func_name;
					}
				}
			}
		}
		if (skill_define.isMember("traps")) {
			auto& trap_define = skill_define["traps"];
			for (uint32 i = 0; i < trap_define.size(); ++i) {
				if (trap_define[i].isMember("extra_buff")) {
					ExtraBuffParse(skill_id, trap_define[i]["extra_buff"]);
				}
				auto keys = trap_define[i].getMemberNames();
				for (auto& key : keys) {
					if (key.rfind("value") != std::string::npos || key.rfind("stub") != std::string::npos) {
						std::string func_name = (boost::format("skill_%1%_random_trap_%2%_%3%") % skill_id % i % key).str();
						std::string func_content;
						if (trap_define[i][key].isString())
							func_content = trap_define[i][key].asString();
						else if (trap_define[i][key].isArray()) {
							auto& multi_line = trap_define[i][key];
							for (auto& line : multi_line)
								func_content += line.asString() + "\n";
						} else {
							ZXERO_ASSERT(false) << "must be single line or multi line string";
						}
						ZXERO_ASSERT(thread_lua->addreturn(func_content));
						std::string func_define = (boost::format("function %1%(ctx) %2% end") % func_name % func_content).str();
						ZXERO_ASSERT(thread_lua->execute_str(func_define, "=skill"));
						trap_define[i][key + "_func"] = func_name;
					}
				}
			}
		}
		auto& create_actor_define = skill_define["create_actor"];
		for (uint32 i = 0; i < create_actor_define.size(); ++i) {
			auto keys = create_actor_define[i].getMemberNames();
			for (auto& key : keys) {
				if (key.rfind("value") != std::string::npos || key.rfind("stub") != std::string::npos) {
					std::string func_name = (boost::format("skill_%1%_create_actor_%2%_%3%") % skill_id % i % key).str();
					std::string func_content;
					if (create_actor_define[i][key].isString())
						func_content = create_actor_define[i][key].asString();
					else if (create_actor_define[i][key].isArray()) {
						auto& multi_line = create_actor_define[i][key];
						for (auto& line : multi_line)
							func_content += line.asString() + "\n";
					} else {
						ZXERO_ASSERT(false) << "must be single line or multi line string";
					}
					ZXERO_ASSERT(thread_lua->addreturn(func_content));
					std::string func_define = (boost::format("function %1%(ctx) %2% end") % func_name % func_content).str();
					ZXERO_ASSERT(thread_lua->execute_str(func_define, "=skill"));
					create_actor_define[i][key + "_func"] = func_name;
				}
			}
		}
	}

	void MtSkillManager::ExtraBuffParse(int32 skill_id, Json::Value& buff_define)
	{
			for (uint32 i = 0; i < buff_define.size(); ++i) {
				auto keys = buff_define[i].getMemberNames();
				for (auto& key : keys) {
					if (key.rfind("value") != std::string::npos || key.rfind("stub") != std::string::npos) {
						std::string func_name = (boost::format("skill_%1%_extra_buff_%2%_%3%") % skill_id % i % key).str();
						std::string func_content;
						if (buff_define[i][key].isString())
							func_content = buff_define[i][key].asString();
						else if (buff_define[i][key].isArray()) {
							auto& multi_line = buff_define[i][key];
							for (auto& line : multi_line)
								func_content += line.asString() + "\n";
						} else {
							ZXERO_ASSERT(false) << "must be single line or multi line string";
						}
						ZXERO_ASSERT(thread_lua->addreturn(func_content));
						std::string func_define = (boost::format("function %1%(ctx) %2% end") % func_name % func_content).str();
						ZXERO_ASSERT(thread_lua->execute_str(func_define, "=skill"));
						buff_define[i][key + "_func"] = func_name;
					}
				}
			}
	}
	
	void MtSkillManager::TarLogicCheck(Json::Value& skill_define)
	{
		int32 skill_id = skill_define["id"].asInt();
		auto& tar_logic = skill_define["tar_logic"];
		if (tar_logic["type"] == "self") return; //目标是自己的话, 没有后面的内容了
		ZXERO_ASSERT(tar_logic["count"].size() == 2);
		ZXERO_ASSERT(tar_logic.isMember("type"));
		if (tar_logic["type"] == "circle" || tar_logic["type"] == "sector") {
			ZXERO_ASSERT(tar_logic.isMember("range"));
		}
		if (tar_logic["type"] == "sector") {
			ZXERO_ASSERT(tar_logic.isMember("radian"));
		}
		if (tar_logic.isMember("tar_extra")) {
			TarExtraCheck(tar_logic["tar_extra"]);
		}
		for (auto& key : tar_logic.getMemberNames()) {
			if (key.rfind("value") != std::string::npos) {
				std::string func_name = (boost::format("skill_tar_logic_%1%_%2%") % skill_id % key).str();
				std::string func_content;
				if (tar_logic[key].isString())
					func_content = tar_logic[key].asString();
				else if (tar_logic[key].isArray()) {
					auto& multi_line = tar_logic[key];
					for (auto& line : multi_line)
						func_content += line.asString() + "\n";
				} else {
					ZXERO_ASSERT(false) << "must be single line or multi line string";
				}
				ZXERO_ASSERT(thread_lua->addreturn(func_content));
				std::string func_define = (boost::format("function %1%(ctx) %2% end") % func_name % func_content).str();
				ZXERO_ASSERT(thread_lua->execute_str(func_define, "=skill"));
				tar_logic[key + "_func"] = func_name;
			}
		}
	}

	void MtSkillManager::TarExtraCheck(const Json::Value& tar_logic)
	{
		ZXERO_ASSERT(tar_logic["count"].size() == 2);
		ZXERO_ASSERT(tar_logic.isMember("type"));
		if (tar_logic["type"] == "circle" || tar_logic["type"] == "sector") {
			ZXERO_ASSERT(tar_logic.isMember("range"));
		}
		if (tar_logic["type"] == "sector") {
			ZXERO_ASSERT(tar_logic.isMember("radian"));
		}
	}

	zxero::int32 MtSkillManager::OnLoad()
	{
		auto table = data_manager::instance()->get_table("skill_build");
		for (auto i = 0; i < table->get_rows(); ++i) {
			auto row = table->find_row(i);
			auto config = boost::make_shared<Config::SkillBuildConfig>();
			FillMessageByRow(*config, *row);
			for (auto j = 1; j < 15; ++j) {
				auto skill_id_key = (boost::format("skill_%1%") % j).str();
				auto skill_id = row->getInt(skill_id_key.c_str());
				if (skill_id > 0) {
					config->add_skill_ids(skill_id);
				}
			}
			skill_build_map_[{config->actor_id(), config->color()}] = config;
		}
		table = data_manager::instance()->get_table("buff_define");
		for (auto i = 0; i < table->get_rows(); ++i) {
			auto row = table->find_row(i);
			auto config = boost::make_shared<Config::BuffDefine>();
			Config::BuffType type;
			Config::BuffType_Parse(row->getString("buff_name"), &type);
			config->set_type(type);
			config->set_is_debuff(row->getInt("is_buff")== -1);
			config->set_can_despell(row->getInt("can_despell")== 1);
			buff_define_map_[config->type()] = config;
		}
		table = data_manager::instance()->get_table("skill_factor");
		for (auto i = 0; i < table->get_rows(); ++i) {
			auto row = table->find_row(i);
			auto level = row->getInt("level");
			skill_factor_[level-1] = row->getFloat("factor");
		}
		return 0;
	}

	bool MtSkillManager::IsDebuff(Config::BuffType type) const
	{
		auto it = buff_define_map_.find(type);
		if (it == buff_define_map_.end()) {
			return true;
		} else {
			return it->second->is_debuff();
		}
	}

	bool MtSkillManager::CanDespell(Config::BuffType type) const
	{
		auto it = buff_define_map_.find(type);
		if (it == buff_define_map_.end()) {
			return true;
		} else {
			return it->second->can_despell();
		}
	}

	const Config::SkillBuildConfig* MtSkillManager::GetSkillBuild(int32 actor_config_id, Packet::ActorColor color) const
	{
		auto it = skill_build_map_.find({ actor_config_id, color });
		if (it == std::end(skill_build_map_)) {
			return nullptr;
		}
		else {
			return skill_build_map_.at({ actor_config_id, color }).get();
		}
	}

	float MtSkillManager::SkillFactor(int32 level) const
	{
		if (level > 0 && level <= (int32)skill_factor_.size())
			return skill_factor_[level - 1];
		return 0.f;
	}

	Json::Value* MtSkillManager::SkillConfig(int32 skill_id)
	{
		if (json_skill_map_.find(skill_id) != json_skill_map_.end())
			return json_skill_map_[skill_id].get();
		else
			return nullptr;
	}

	void MtActorSkill::lua_reg(lua_State* ls)
	{
		ff::fflua_register_t<MtActorSkill, ctor()>(ls, "MtActorSkill")
			.def(&MtActorSkill::DbInfo, "DbInfo")
			;
	}

	bool MtActorSkill::CreateActor(const Json::Value& data, std::set<uint64>& r_set)
	{
		/**	{
				"id" : int npc_and_monster 里面的id
					"life_time" : 30000, 持续时间, 一直持续填 - 1
					"location" : <locaiton>  初始位置
			}
			1  按站位点
			{ "pos_index" : int  0 - 15 }

				2 按释放着相对位置
			{ "distance" :  3000, "dir" : float 弧度(0 - 3.14) }
		*/
		int32 id = data["id"].asInt();
		int32 physical_attack = 0;
		int32 magic_attack = 0;
		int32 physical_armor = 0;
		int32 magic_armor = 0;
		int32 life_time = -1;
		auto config = MtMonsterManager::Instance().FindMonster(id);
		if (config == nullptr) 
			return false;
		int32 actor_count = 1;
		if (data.isMember("count")) {
			actor_count = data["count"].asInt();
		}
		actor_count = actor_count <= 0 ? 1 : actor_count;

		if (data.isMember("life_time")) {
			life_time = data["life_time"].asInt();
		}
		if (data.isMember("physical_attack_value")) {
			physical_attack = thread_lua->call_fun<int32>(data["physical_attack_value_func"].asString(), &Ctx());
		}
		if (data.isMember("magic_attack_value")) {
			magic_attack = thread_lua->call_fun<int32>(data["magic_attack_value_func"].asString(), &Ctx());
		}
		if (data.isMember("physical_armor_value")) {
			physical_armor = thread_lua->call_fun<int32>(data["physical_armor_value_func"].asString(), &Ctx());
		}
		if (data.isMember("magic_armor_value")) {
			magic_armor = thread_lua->call_fun<int32>(data["magic_armor_value_func"].asString(), &Ctx());

		}
		const auto& location = data["location"];
		auto battle_groud = owner_->BattleGround();
		Packet::Position pos = owner_->Position();
		pos.set_y(pos.y() + 500);
		if (location.isMember("pos_index")) {
			int32 pos_index = location["pos_index"].asInt();
			pos = battle_groud->RelativePosition(owner_->Guid(), pos_index);
		} else {
			int32 distance = location["distance"].asInt();
			real32 dir = location["dir"].asFloat();
			pos = zxero::move_point(pos, distance, owner_->Direction() + dir);
		}
		for (auto i = 0; i < actor_count; ++i) {
			auto actor = boost::make_shared<SummonMonsterActor>(config,
				owner_->Level(), owner_, data["cab_be_select"].asBool());
			if (physical_attack > 0)
				actor->RunTimeBattleInfo()->set_physical_attack(physical_attack);
			if (magic_attack > 0)
				actor->RunTimeBattleInfo()->set_magic_attack(magic_attack);
			if (physical_armor)
				actor->RunTimeBattleInfo()->set_physical_armor(physical_armor);
			if (magic_armor)
				actor->RunTimeBattleInfo()->set_magic_armor(magic_armor);
			actor->LifeTime(life_time);
			actor->InitBattleInfo(config);
			actor->Position(pos);
			battle_groud->AddActor(owner_->Guid(), actor);
			r_set.insert(actor->Guid());
		}
		return true;
	}

	MtActorSkill::MtActorSkill( const Packet::ActorSkill& db_info, MtActor* owner)
		:db_info_(db_info), owner_(owner)
	{
		skill_json_config_ = MtSkillManager::Instance().SkillConfig(db_info.skill_id());
		if (skill_json_config_ == nullptr) {
			skill_json_config_ = MtSkillManager::Instance().SkillConfig(200010);
		}
		InitPassiveBits();
	}

	MtActorSkill::MtActorSkill( MtActorSkill& rhs)
		:db_info_(rhs.db_info_), skill_json_config_(rhs.skill_json_config_),
		owner_(rhs.owner_)
	{
		InitPassiveBits();
	}

	Packet::ActorSkill* MtActorSkill::DbInfo() 
	{
		return &db_info_;
	}

	void MtActorSkill::SetCoolDownTime()
	{
		cool_down_time_ = JsonConfig()["cd"].asInt();
		if (cool_down_time_ == 0)
			cool_down_time_ = ActiveConfig()["cd"].asInt();
	}

	void MtActorSkill::EnterCoolDown()
	{
		ZXERO_ASSERT(status_ == SkillStatus::IDEL);
		SetCoolDownTime();
		status_ = SkillStatus::COOLDOWN;
	}

	void MtActorSkill::Start()
	{
		//LOG_INFO << "MtActorSKill" << this << "start ";
		ZXERO_ASSERT(status_ == SkillStatus::IDEL);
		status_ = SkillStatus::START;
		SetCoolDownTime();
		prepare_time_ = ActiveConfig()["prepare_time"].asInt64();
		post_time_ = ActiveConfig()["post_time"].asInt64();
		float bullet_speed = 0;
		if (ActiveConfig().isMember("start_time_buff")) {
			for (auto& config : ActiveConfig()["start_time_buff"]) {
				if (owner_->IsDead() == false) {
					auto buff = BuffFactory::get_mutable_instance().CreateBuff(
						config, Ctx(), owner_->BattleGround(), owner_->Guid());
					if (buff) owner_->AddBuff(buff);
				}
			}
		}
		if (ActiveConfig().isMember("bullet_speed"))
			bullet_speed = ActiveConfig()["bullet_speed"].asFloat();
		auto& skill_target = targets_;
		auto target = owner_->BattleGround()->FindActor(skill_target.FirstTargetGuid());
		if (target != nullptr) {
			if (bullet_speed > 0) {
				auto distance = zxero::distance2d(owner_->Position(), target->Position()) - owner_->ObjectRadius() - target->ObjectRadius();
				prepare_time_ += uint64((distance / bullet_speed) * 1000);
			}
		}
		if (ActiveConfig().isMember("move_logic") ) {//需要表现冲锋
				std::string move_logic = ActiveConfig()["move_logic"].asString();
				position_effect_ = SkillLogicFactory::get_mutable_instance().GetPositionLogic(move_logic);
				position_effect_->StartLogic(this);
		}
		else {
			Packet::ActorUseSkill use_skill_message;
			use_skill_message.set_source_guid(owner_->Guid());
			std::for_each(skill_target.targets_.begin(), skill_target.targets_.end(),
				[&](auto actor_guid) {use_skill_message.add_target_guid(actor_guid); });
			auto skill_info = use_skill_message.mutable_skill();
			skill_info->set_skill_id(DbInfo()->skill_id());
			skill_info->set_skill_level(DbInfo()->skill_level());
			skill_info->set_bullet_speed(float(bullet_speed));
			use_skill_message.set_time_stamp(owner_->BattleGround()->TimeStamp());
			owner_->BattleGround()->BroadCastMessage(use_skill_message);
		}
	}

	void MtActorSkill::BeBreak()
	{
		if (Started())
			status_ = SkillStatus::COOLDOWN;
	}

	void MtActorSkill::OnTick(zxero::uint64 elapseTime)
	{
		if (status_ != SkillStatus::IDEL && cool_down_time_ > 0) {
			cool_down_time_ -= int64(elapseTime);
		}
		auto& skill_target = targets_;
		//这里的status_状态判断,没有办法用switch简化. 应为技能要求实时性高, 这个tick里如果
		//状态改变了, 需要再这个tick再次处理, 而不能像其他逻辑一般, 直接break掉再在下个tick处理
		if (status_ == SkillStatus::IDEL)
			return;
		if (status_ == SkillStatus::START){
			status_ = SkillStatus::PREPARE;
		}
		if (status_ == SkillStatus::PREPARE) {
			if (prepare_time_ - int64(elapseTime) >= 0) {
				prepare_time_ -= int64(elapseTime);
			} else {
				status_ = SkillStatus::POSITION_LOGIC;
			}
		}
		if (status_ == SkillStatus::POSITION_LOGIC){
			if (position_effect_) {//相位移动, 移动完成后取消移动逻辑
				if (position_effect_->Execute(this, elapseTime)) {
					position_effect_ = nullptr;
					status_ = SkillStatus::EXECUTING;
				}
			} else {
				status_ = SkillStatus::EXECUTING;
			}
		}
		if (status_ == SkillStatus::EXECUTING) {
			if (!skill_target.targets_.empty() &&
				boost::size(skill_target.targets_ | boost::adaptors::filtered(
				boost::bind(&MtActorSkill::ValidateToActor, this, _1))) == 0) {
				status_ = SkillStatus::DONE;
			} else {
				int32 repeat_time = 1;
				if (JsonConfig().isMember("repeat")) {
					repeat_time = JsonConfig()["repeat"].asInt();
				}
				TakeSelfEffect();
				if (++repeat_times_ < repeat_time) {
					prepare_time_ = JsonConfig()["repeat_interval"].asInt64();
					status_ = SkillStatus::PREPARE;
				} else {
					if (!HasPassiveConfig())	owner_->AfterUseActiveSkillStub(Ctx());
					status_ = SkillStatus::POST;
				}

			}
		}
		if (status_ == SkillStatus::POST) {
			if (post_time_ - int64(elapseTime) > 0) {
				post_time_ -= int64(elapseTime);
			} else {
				status_ = SkillStatus::COOLDOWN;
			}
		}
		if (status_ == SkillStatus::COOLDOWN)
		{
			if (cool_down_time_ <= 0) {
				cool_down_time_ = 0;
				status_ = SkillStatus::DONE;
			}
		}
		if (status_ == SkillStatus::DONE) {
			SetReuse();
		}

	}

	void MtActorSkill::CancelSkillMsg()
	{
		Packet::ActorCancelSkill msg;
		msg.set_source_guid(owner_->Guid());
		msg.set_skill_id(SkillId());
		msg.set_time_stamp(owner_->BattleGround()->TimeStamp());
		owner_->BattleGround()->BroadCastMessage(msg);
	}

	void MtActorSkill::BuffEffect()
	{
		if (repeat_times_ == 0 || (repeat_times_ > 0
			&& ActiveConfig()["repeat_calc_buff"].asBool())) {
			//self buff
			for (auto& config : ActiveConfig()["self_buff"]) {
				if (owner_->IsDead() == false) {
					auto buff = BuffFactory::get_mutable_instance().CreateBuff(
						config, Ctx(), owner_->BattleGround(), owner_->Guid());
					if (buff) owner_->AddBuff(buff);
				}
			}
			//target buff
			auto& skill_target = targets_;
			auto battle_ground = owner_->BattleGround();
			for (auto& config : ActiveConfig()["target_buff"]) {
				for (auto& target_guid : skill_target.targets_) {
					auto target = battle_ground->FindActor(target_guid);
					if (target == nullptr) continue;;
					Ctx().target_.actor_ = target;
					auto buff = BuffFactory::get_mutable_instance().CreateBuff(
						config, Ctx(), owner_->BattleGround(), owner_->Guid());
					if (buff && target->IsDead() == false) target->AddBuff(buff);
				}
				for (auto& target_guid : skill_target.extra_target_) {
					auto target = battle_ground->FindActor(target_guid);
					if (target == nullptr) continue;;
					Ctx().target_.actor_ = target;
					auto buff = BuffFactory::get_mutable_instance().CreateBuff(
						config, Ctx(), owner_->BattleGround(), owner_->Guid());
					if (buff && target->IsDead() == false) target->AddBuff(buff);
				}
			}
		}
	}

	void MtActorSkill::TrapEffect()
	{
		auto target = owner_->BattleGround()->FindActor(targets_.FirstTargetGuid());
		if (target == nullptr) return;
		if (repeat_times_ == 0) {
			Packet::Position pos = target->Position();
			if (JsonConfig().isMember("random_traps")) {
				uint32 size = JsonConfig()["random_traps"].size();
				uint32 idx = rand_gen->uintgen(0, size - 1);
				auto& trap_config = JsonConfig()["random_traps"][idx];
				auto trap = TrapImpl::CreateTrap(trap_config, pos, Ctx(), owner_);
				if (trap != nullptr) {
					trap->AfterCreate();
					owner_->BattleGround()->AddTrap(trap);
				}
			}
			for (auto& trap_config : JsonConfig()["traps"]) {
				auto trap = TrapImpl::CreateTrap(trap_config, pos, Ctx(), owner_);
				if (trap != nullptr) {
					trap->AfterCreate();
					owner_->BattleGround()->AddTrap(trap);
				}
			}
		}
	}

	void MtActorSkill::ReliveTarget()
	{
		auto& config = ActiveConfig();
		auto& skill_target = targets_;
		auto battle_ground = owner_->BattleGround();
		if (config["relive_target"].asBool()) {
			for (auto& target_guid : skill_target.targets_) {
				auto target = battle_ground->FindActor(target_guid);
				if (target  != nullptr && target->IsDead()) {
					auto& ctx = Ctx();
					ctx.source_.actor_ = owner_;
					ctx.target_.actor_ = target;
					int32 relive_hp = thread_lua->call_fun<int32>(config["relive_hp_value_func"].asString(), &ctx);
					relive_hp = relive_hp > 0 ? relive_hp : 1;
					target->OnRelive();
					target->SetHp(relive_hp);
					Packet::ActorRelive msg;
					msg.set_source_guid(owner_->Guid());
					msg.set_target_guid(target->Guid());
					msg.set_hp(relive_hp);
					msg.mutable_skill()->set_skill_id(SkillId());
					msg.mutable_skill()->set_skill_level(SKillLevel());
					msg.set_time_stamp(owner_->BattleGround()->TimeStamp());
					owner_->BattleGround()->BroadCastMessage(msg);
				}
			}
		}
	}

	void MtActorSkill::Despelling()
	{
		auto& config = ActiveConfig()["despelling"];
		if (config.empty())
			return;
		//all, [dot, debuff], layer_count
		int32 layercount = config["layer_count"].asInt();
		layercount = layercount > 0 ? layercount : 0;//没填表示全部驱散
		std::function<void(const boost::shared_ptr<BuffImpl::Buff>&)> cleaner;
		if (config["all"].asBool()) {//清除所有buff
			cleaner = [=](const boost::shared_ptr<BuffImpl::Buff>& buff) {
				if (buff->CanDespell()) {
					if (layercount > 0) {
						buff->SetLayer(buff->GetLayer() - layercount);
					} else {
						buff->SetLayer(0);
					}
				}
			};
		}
		if (config["debuff"].asBool() == false) {//不清除debuff
			cleaner = [=](const boost::shared_ptr<BuffImpl::Buff>& buff) {
				if (buff->IsDebuff() == false && buff->CanDespell())
					buff->SetLayer(buff->GetLayer() - layercount);
			};
		}
		auto& skill_target = targets_;
		auto battle_ground = owner_->BattleGround();
		for (auto& target_guid : skill_target.targets_) {
			auto target = battle_ground->FindActor(target_guid);
			if (target) boost::for_each(target->Buffs(), cleaner);
		}
		for (auto& target_guid : skill_target.extra_target_) {
			auto target = battle_ground->FindActor(target_guid);
			if (target) boost::for_each(target->Buffs(), cleaner);
		}
	}

	void MtActorSkill::SummonEffect()
	{
		std::set<uint64> created_actors;
		for (auto& create_config : ActiveConfig()["create_actor"]) {
			CreateActor(create_config, created_actors);
		}
		auto old = owner_->SkillCreatedActor(SkillId());
		if (!old.empty()) {
			auto delete_event = boost::make_shared<DeleteActorEvent>();
			delete_event->guids_ = old;
			owner_->BattleGround()->PushEvent(delete_event);
		}
		owner_->UpdateSkillCreatedActor(SkillId(), std::move(created_actors));
	}

	void MtActorSkill::HitOffEffect()
	{
		if (!ActiveConfig()["hit_off"].empty()) {
			auto& skill_target = targets_;
			auto battle_ground = owner_->BattleGround();
			for (auto& target_guid : skill_target.targets_) {
				auto target = battle_ground->FindActor(target_guid);
				if (target != nullptr && target->CurrSkill()) {
					target->CurrSkill()->BeBreak();
				}
				target->BattleState(boost::make_shared<BattleState::HitOff>(target, owner_, this));
			}
		}
	}

	void MtActorSkill::TakeSelfEffect()
	{
		try
		{
			std::string logic_type = "direct_damage";
			if (ActiveConfig()["cure"].asBool() == true) logic_type = "direct_cure";
			if (ActiveConfig()["chain_target"].asBool() == true) logic_type = "flash_chain";
			if (ActiveConfig()["chain_target"].asBool() == true
				&& ActiveConfig()["cure"].asBool())
				logic_type = "cure_chain";
			auto logic = SkillLogicFactory::get_mutable_instance().GetLogic(logic_type);
			logic->Execute(this);
			Despelling();//驱散效果,先执行,避免驱散该技能加的buff
			BuffEffect();//buffs
			TrapEffect();//traps
			SummonEffect();//召唤技能
			ReliveTarget();//复活
			HitOffEffect(); //击飞
			//广播自己使用吟唱技能事件
			if (IsLongTimeSkill()) {
				owner_->BattleGround()->EnumAllActor([&](const boost::shared_ptr<MtActor>& actor) {
					if (actor->IsDead()) return;
					actor->AfterActorUseLongTimeSkill(Ctx());
				});
			}
		}
		catch (ff::lua_exception& e)
		{
			LOG_WARN << "skill logic execute with lua error:" << e.what();
		}
	
	}

	void MtActorSkill::OnSkillHitTarget(const boost::shared_ptr<MtActor>& /*target*/, const Packet::ActorOnDamage& /*damage_info*/) const
	{
	}

	const int32 MtActorSkill::SkillId() const
	{
		return db_info_.skill_id();
	}

	const int32 MtActorSkill::SKillLevel() const
	{
		if (temp_level_ == 0) {
			int32 level = db_info_.skill_level();
			for (auto& buff : owner_->Buffs())
				level = buff->SkillLevel(level);
			return  level;
		}
		else
			return temp_level_;
	}

	zxero::uint64 MtActorSkill::Guid() const
	{
		return db_info_.guid();
	}

	uint64 MtActorSkill::SelectMoveTarget()
	{
		auto bg = owner_->BattleGround();
		auto& tar_logic = ActiveConfig()["tar_logic"];
		Config::TargetCamp camp = Config::TargetCamp::ENEMY;
		if (tar_logic["teammates"] == true)
			camp = Config::TargetCamp::TEAM;
		if (tar_logic["both"] == true)
			camp = Config::TargetCamp::BOTH;
		bool select_pet = tar_logic["pets"].asBool();
		auto camp_result = MtSkillSelectTar::CampSelect(bg->TeamMates(*owner_), bg->Enemies(*owner_), camp);
		camp_result.erase(
			std::remove_if(camp_result.begin(),
			camp_result.end(),
			boost::bind(&MtActor::CanBeSelect, _1, owner_) == false
			),
			camp_result.end());

		if (select_pet == false) {
			camp_result.erase(std::remove_if(camp_result.begin(),
				camp_result.end(), boost::bind(&MtActor::ActorType, _1) == Packet::ActorType::SUMMON_NO_SELECT),
				camp_result.end());
		}
		if (camp_result.empty()) {
			return INVALID_GUID;
		}
		if (tar_logic["type"].asString() == "self") {
			return owner_->Guid();
		}
		if (tar_logic["type"].asString() == "circle") {
			std::sort(camp_result.begin(), camp_result.end(), [&](auto& lhs, auto& rhs) {
				auto lhs_distance = zxero::distance2d(lhs->Position(), owner_->Position());
				auto rhs_distance = zxero::distance2d(rhs->Position(), owner_->Position());
				return lhs_distance < rhs_distance;
			});
			return camp_result.front()->Guid();
		}
		if (tar_logic["type"].asString() == "sector") {
			std::sort(camp_result.begin(), camp_result.end(), [&](auto& lhs, auto& rhs) {
				auto lhs_distance = zxero::distance2d(lhs->Position(), owner_->Position());
				auto rhs_distance = zxero::distance2d(rhs->Position(), owner_->Position());
				return lhs_distance < rhs_distance;
			});
			//TODO 计算朝向
			return camp_result.front()->Guid();
		}
		if (tar_logic["type"].asString() == "farest") {
			std::sort(camp_result.begin(), camp_result.end(), [&](auto& lhs, auto& rhs) {
				auto lhs_distance = zxero::distance2d(lhs->Position(), owner_->Position());
				auto rhs_distance = zxero::distance2d(rhs->Position(), owner_->Position());
				return lhs_distance > rhs_distance;
			});
			return camp_result.front()->Guid();
		}
		if (tar_logic["type"].asString() == "min_hp_percent") {
			std::sort(camp_result.begin(), camp_result.end(), [&](auto& lhs, auto& rhs) {
				return lhs->HpPercent() < rhs->HpPercent();
			});
			return camp_result.front()->Guid();
		}
		if (tar_logic["type"].asString() == "max_hp") {
			std::sort(camp_result.begin(), camp_result.end(), [&](auto& lhs, auto& rhs) {
				return lhs->Hp() > rhs->Hp();
			});
			return camp_result.front()->Guid();
		}
		ZXERO_ASSERT(false);
		return false;
	}

	bool MtActorSkill::TargetDistanceAvailable(MtActor* target) const
	{
		auto distance = zxero::distance2d(owner_->Position(), target->Position());
		if (distance <= owner_->AttackRadius() + target->ObjectRadius())
			return true;
		else
			return false;
	}
	bool MtActorSkill::SelectSkillTargetInner(const Json::Value& tar_logic,
		SkillTarget& target_result)
	{
		auto bg = owner_->BattleGround();
		Config::TargetCamp camp = Config::TargetCamp::ENEMY;
		if (tar_logic["teammates"] == true)
			camp = Config::TargetCamp::TEAM;
		if (tar_logic["both"] == true)
			camp = Config::TargetCamp::BOTH;
		bool select_pet = tar_logic["pets"].asBool();
		auto candidates = MtSkillSelectTar::CampSelect(bg->TeamMates(*owner_), bg->Enemies(*owner_), camp);
		candidates.erase(std::remove_if(candidates.begin(),
			candidates.end(), boost::bind(&MtActor::CanBeSelect, _1, owner_) == false),
			candidates.end());
		if (select_pet == false) {
			candidates.erase(std::remove_if(candidates.begin(),
				candidates.end(), boost::bind(&MtActor::ActorType, _1) == Packet::ActorType::SUMMON_NO_SELECT),
				candidates.end());
		}
		int32 max = tar_logic["count"][1].asInt();
		if (tar_logic["type"] == "self") {
			target_result.AddTarget(owner_->Guid());
			return true;
		}
		if (candidates.empty()) {
			LOG_INFO << "[skill] id:" << SkillId() << " cannot select skill target";
			return false;
		}
		if (tar_logic.isMember("range")) {//限定范围
			decltype(candidates) range_result;
			std::copy_if(candidates.begin(), candidates.end(),
				std::back_inserter(range_result),
				[&](auto& ele) {
				return this->TargetDistanceAvailable(ele);
			});
			candidates.swap(range_result);
		}
		if (tar_logic.isMember("dir")) {//限定朝向
			decltype(candidates) dir_result;
			real32 skill_dir = tar_logic["dir"].asFloat();
			std::copy_if(candidates.begin(), candidates.end(),
				std::back_inserter(dir_result),
				[&](auto& ele) {
				auto dir = zxero::get_direction(owner_->Position(), ele->Position());
				return dir <= skill_dir;
			});
			candidates.swap(dir_result);
		}
		//排序
		if (tar_logic["type"].asString() == "circle" || tar_logic["type"].asString() == "sector") {
			std::sort(candidates.begin(), candidates.end(), [&](auto& lhs, auto& rhs) {
				auto lhs_distance = zxero::distance2d(lhs->Position(), owner_->Position());
				auto rhs_distance = zxero::distance2d(rhs->Position(), owner_->Position());
				return lhs_distance < rhs_distance;
			});
		}
		if (tar_logic["type"].asString() == "farest") {
			std::sort(candidates.begin(), candidates.end(), [&](auto& lhs, auto& rhs) {
				auto lhs_distance = zxero::distance2d(lhs->Position(), owner_->Position());
				auto rhs_distance = zxero::distance2d(rhs->Position(), owner_->Position());
				return lhs_distance > rhs_distance;
			});
		}
		if (tar_logic["type"].asString() == "min_hp_percent") {
			std::sort(candidates.begin(), candidates.end(), [&](auto& lhs, auto& rhs) {
				return lhs->HpPercent() < rhs->HpPercent();
			});
		}
		if (tar_logic["type"].asString() == "max_hp") {
			std::sort(candidates.begin(), candidates.end(), [&](auto& lhs, auto& rhs) {
				return lhs->Hp() > rhs->Hp();
			});
		}
		if (tar_logic.isMember("random") && tar_logic["random"].asBool()) {
			std::random_shuffle(candidates.begin(), candidates.end());
		}

		if (candidates.empty())
			return false;
		//根据最小目标数量和最大目标数量确定最后结果
		if (candidates.size() >= tar_logic["count"][0].asUInt()) {
			max = tar_logic["count"][1].asInt() + owner_->SkillTargetCount();
			max = max > int32(candidates.size()) ? candidates.size() : max;
			decltype(candidates) result;
			result.assign(candidates.begin(), candidates.begin() + max);
			candidates.swap(result);
		} else {
			decltype(candidates) result;
			for (int32 i = 0; i < tar_logic["count"][0].asInt(); ++i) {
				result.push_back(candidates[i % candidates.size()]);
			}
			candidates.swap(result);
		}
		//目标选择子逻辑
		if (tar_logic.isMember("sub_logic")) {
			for (auto& sub_logic : tar_logic["sub_logic"]) {
				SelectSkillTargetInner(sub_logic, target_result);
			}
		}
		//确定extra目标
		if (tar_logic.isMember("tar_extra") || owner_->TarExtra().size() > 0) {
			if (!target_result.extra_target_.empty())
				return true;
			auto extra = tar_logic["tar_extra"];
			if (owner_->TarExtra().size() > 0)
				extra = owner_->TarExtra();
			ZXERO_ASSERT(extra["type"] == std::string("circle"));
			if (!candidates.empty()) {
				auto& first_target = candidates.front();
				auto target_team = bg->TeamMates(*first_target);
				std::size_t extra_max = extra["count"][1].asUInt();
				for (auto& t : target_team) {
					bool not_same = std::find_if(candidates.begin(), candidates.end(),
						boost::bind(&MtActor::Guid, _1) == t->Guid()) == candidates.end();
					if (not_same) {
						bool distance_match = std::any_of(candidates.begin(), candidates.end(), [&](auto& a) {
							return zxero::distance2d(t->Position(), a->Position()) <= extra["range"].asFloat();
						});
						if (distance_match && t->IsDead() == false) {
							target_result.extra_target_.push_back(t->Guid());
							if (target_result.extra_target_.size() >= extra_max) break;
						}
					}
				}
			}
		}
		boost::transform(candidates, 
			std::back_inserter(target_result.targets_),
			boost::bind(&MtActor::Guid, _1));
		return true;
	}
	bool MtActorSkill::SelectSkillTarget(SkillTarget* result)
	{
		return SelectSkillTargetInner(ActiveConfig()["tar_logic"], *result);
	}

	int32 MtActorSkill::SkillRange() const
	{
		if (ActiveConfig().isMember("skill_range"))
			return ActiveConfig()["skill_range"].asInt();
		else
			return ActiveConfig()["tar_logic"]["range"].asInt();
	}

	int32 MtActorSkill::ChargeStopRange() const
	{
		return ActiveConfig()["tar_logic"]["stop_range"].asInt();
	}

	SkillContext& MtActorSkill::Ctx()
	{
		if (ctx_ == nullptr) {
			ctx_ = boost::make_shared<SkillContext>();
			ctx_->skill_.skill_ = this;
		}
		return *ctx_;
	}

	bool MtActorSkill::MainPassive() const
	{
		return JsonConfig()["passive"].asBool();
	}

	bool MtActorSkill::CanTrigger() const
	{
		if (PassiveConfig()["trigger_once"].asBool()
			&& trigger_count_ > 0) {
			return false;
		}
		if (MainPassive()) {//此类技能需要考虑cd
			if (status_ == SkillStatus::IDEL && cool_down_time_ == 0) {
				return true;
			} else {
				return false;
			}
		} else {
			return true;
		}
	}

	const Json::Value& MtActorSkill::PassiveConfig() const
	{
		if (JsonConfig().isMember("passive_skill")) {
			return JsonConfig()["passive_skill"];
		} else {
			return JsonConfig();
		}
	}

	const Json::Value& MtActorSkill::ActiveConfig() const
	{
		if (JsonConfig()["active_skill"].empty()) {
			return JsonConfig();
		} else {
			return JsonConfig()["active_skill"];
		}
	}

	void MtActorSkill::TempLevel(int32 level)
	{
		temp_level_ = level;
	}

	void MtActorSkill::Refresh()
	{
		started_ = false;
		finished_ = false;
		cool_down_time_ = 0;
		prepare_time_ = 0;
		post_time_ = 0;
		sub_skill_index_ = 0;
		trigger_count_ = 0;
	}

	void MtActorSkill::SetReuse()
	{
		prepare_time_ = 0;
		post_time_ = 0;
		sub_skill_index_ = 0;
		repeat_times_ = 0;
		status_ = SkillStatus::IDEL;
		targets_.Reset();
		temp_level_ = 0;
	}

	bool MtActorSkill::IsIdle() const
	{
		return status_ == SkillStatus::IDEL;
	}

	bool MtActorSkill::IsCoolDown() const
	{
		return status_ == SkillStatus::COOLDOWN;
	}

	MtActorSkill::~MtActorSkill()
	{

	}

	void MtActorSkill::Clear()
	{
		if (ctx_ == nullptr) {
			return;
		}
		ctx_->Clear();
		ctx_ = nullptr;
	}

	bool MtActorSkill::ValidateToActor(uint64 guid) const
	{
		auto battle_ground = owner_->BattleGround();
		auto target = battle_ground->FindActor(guid);
		if (target == nullptr) return false;
		if (SelectSelf()) {
			return target->Guid() == owner_->Guid();
		}
		if (ReliveSkill()) {//复活技能
			return target->IsDead() && target->BattleActorType() != Packet::BattleActorType::BAT_SUMMON;
		} else {//非复活
			return !target->IsDead();
		}
	}
	
	bool MtActorSkill::ReliveSkill() const 
	{//复活技能
		return ActiveConfig()["relive_target"].asBool();
	}

	void MtActorSkill::OnDestroy()
	{
		ctx_ = nullptr;
	}

	bool MtActorSkill::DirectUsable() const //技能是否直接可用
	{
		return !JsonConfig()["passive"].asBool();
	}

	bool MtActorSkill::NeedTeammate() const
	{
		auto& tar_logic = ActiveConfig()["tar_logic"];
		if (tar_logic["teammates"].asBool() == true) {
			return true;
		}
		return false;
	}

	bool MtActorSkill::SelectSelf() const
	{
		auto& tar_logic = ActiveConfig()["tar_logic"];
		return tar_logic["type"].asString() == "self";
	}

	bool MtActorSkill::IsLongTimeSkill() const
	{
		return ActiveConfig()["long_time_skill"].asBool();
	}

	bool MtActorSkill::IsFirstStage() const
	{
		return repeat_times_ == 0;
	}

	void MtActorSkill::SetSkillTarget(const SkillTarget& target)
	{
		targets_ = target;
	}

	bool MtActorSkill::HasPassiveConfig() const
	{
		return has_passive_config_;
	}

	const SkillTarget& MtActorSkill::GetSkillTarget() const
	{
		return targets_;
	}

	void MtActorSkill::InitPassiveBits()
	{
		has_passive_config_ = JsonConfig()["passive"].asBool()
			|| JsonConfig().isMember("passive_skill");
		for (int i = 0; i < Packet::SkillBuffStubType::max_skill_buff_stub_type; ++i) {
			std::string name = Packet::SkillBuffStubType_Name(Packet::SkillBuffStubType(i));
			stub_bits_[i] = PassiveConfig().isMember(name);
		}
	}

	bool MtActorSkill::HasPassiveStub(Packet::SkillBuffStubType type)
	{
		return stub_bits_[int32(type)];
	}

	bool MtActorSkill::BaseSkill() const
	{
		return (SkillId() % 10) == 0;
	}

	bool operator<(const std::tuple<int32, Packet::ActorColor>& lhs, const std::tuple<int32, Packet::ActorColor>& rhs)
	{
		return std::get<0>(lhs) < std::get<0>(rhs) || (std::get<0>(lhs) == std::get<0>(rhs) && std::get<1>(lhs) < std::get<1>(rhs));
	}


	void SkillTarget::AddTarget(uint64 acotr_guid)
	{
		targets_.push_back(acotr_guid);
	}


	void SkillTarget::AddExtraTarget(uint64 acotr_guid)
	{
		extra_target_.push_back(acotr_guid);
	}

	zxero::uint64 SkillTarget::FirstTargetGuid() const
	{
		if (targets_.empty() == false) {
			return targets_.front();
		}
		return INVALID_GUID;
	}

}