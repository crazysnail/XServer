#include "module.h"
#include "utils.h"
#include "mt_word_manager.h"
#include "random_generator.h"
#include "../data/data_manager.h"
#include <boost/algorithm/string.hpp>
namespace Mt
{
	static MtWordManager* sg_mt_word_manager = nullptr;
	MtWordManager* MtWordManager::Instance()
	{
		return sg_mt_word_manager;
	}

	REGISTER_MODULE(MtWordManager)
	{
		require("data_manager");

		register_load_function(STAGE_LOAD, bind(&MtWordManager::OnLoad, MtWordManager::Instance()));
		register_unload_function(STAGE_POST_UNLOAD, bind(&MtWordManager::OnUnLoad, MtWordManager::Instance()));

	}

	void MtWordManager::lua_reg(lua_State* ls)
	{
		ff::fflua_register_t<MtWordManager, ctor()>(ls, "MtWordManager")
			.def(&MtWordManager::NoSensitiveWord, "NoSensitiveWord")
			;
		ff::fflua_register_t<>(ls)
			.def(&MtWordManager::Instance, "LuaMtWordManager");
	}
	MtWordManager::MtWordManager()
	{
		sg_mt_word_manager = this;
	}
	
	std::string MtWordManager::RandomName(Packet::Gender gender)
	{
		std::string result = prefix_[rand_gen->intgen(0, prefix_.size() - 1)];
		if (gender == Packet::Gender::MALE) {
			result += man_[rand_gen->intgen(0, man_.size() - 1)];
		} else {
			result += women_[rand_gen->intgen(0, women_.size() - 1)];
		}
		result += suffix_[rand_gen->intgen(0, suffix_.size() - 1)];
		return result;
	}

	int32 MtWordManager::OnLoad()
	{
		data_table *sensitive_word_table = data_manager::instance()->get_table("sensitive_word");
		data_row *data_in_row = nullptr;
		for (int i = 0; i < sensitive_word_table->get_rows(); ++i){
			data_in_row = sensitive_word_table->find_row(i);
			m_sensitive_words.push_back(data_in_row->getString("name"));
		}

		data_table* name_table = data_manager::instance()->get_table("t_random_name");
		if (name_table != nullptr) {
			for (int i = 0; i < name_table->get_rows(); ++i) {
				data_row* row = name_table->find_row(i);
				std::string prefix = row->getString("prefix");
				std::string man = row->getString("boy_name");
				std::string women = row->getString("girl_name");
				std::string suffix = row->getString("suffix");
				if (prefix != "-1") prefix_.push_back(prefix);
				if (man != "-1") man_.push_back(man);
				if (women != "-1") women_.push_back(women);
				if (suffix != "-1") suffix_.push_back(suffix);
			}
		}
		return 0;
	}

	int32 MtWordManager::OnUnLoad()
	{
		return 0;
	}

	MtWordManager::~MtWordManager()
	{

	}

	bool MtWordManager::RegName(const std::string& name)
	{
		if (m_used_name.find(name) != m_used_name.end()) {
			return false;
		}

		m_used_name.insert(name);
		return true;
	}
	bool MtWordManager::UniqueName(const std::string& name)
	{
		return m_used_name.find(name) == m_used_name.end();
	}

	bool MtWordManager::InvalidName(const std::string& name)
	{
		//³¤¶È¼ì²é
		if (name.length() < 6 || name.length() > 24){
			return false;
		}

		if ( !NoSensitiveWord(name)) {
			return false;
		}

		if (m_used_name.find(name) != m_used_name.end()) {
			return false;
		}

		return true;
	}

	bool MtWordManager::NoSensitiveWord(const std::string& content)
	{
		zxero::profiler prof("sensitive check");
		for (auto child : m_sensitive_words) {
			if (content.find(child) != std::string::npos){
				return false;
			}
		}
		return true;
	}

	bool MtWordManager::FilterWord(std::string &content)
	{
		bool has = false;
		for (auto child : m_sensitive_words) {
			if (content.find(child) != std::string::npos){
				boost::algorithm::replace_all(content, child, "*");
				has = true;
			}
		}
		return has;
	}
}
