#include "dbhelper.h"

namespace zxero
{
	prepared_statement_builder::prepared_statement_builder(const std::string& table_name)
		:table_name_(table_name)
	{

	}
	
	prepared_statement_builder::~prepared_statement_builder()
	{
		for( auto &child : properties_ )
		{
			delete child;
		}
		properties_.clear();
	}

	void prepared_statement_builder::add_column_internal(uint32 offset, uint32 size, uint32 count, const std::string& name, db_setter setter, db_getter getter)
	{
		if (count == 1)
		{
			auto db_prop = new db_property;
			db_prop->offset = offset;
			db_prop->count = count;
			db_prop->name = name;
			db_prop->setter = setter;
			db_prop->getter = getter;

			properties_.push_back(db_prop);
		}
		else
		{
			//	自动展开，方便后续遍历处理
			for (uint32 i = 0; i < count; ++i)
			{
				char buf[128];
				zprintf(buf, sizeof(buf), "%s%u", name.c_str(), i);

				auto db_prop = new db_property;
				db_prop->offset = offset + size * i;
				db_prop->count = count;
				db_prop->name = buf;
				db_prop->setter = setter;
				db_prop->getter = getter;

				properties_.push_back(db_prop);
			}
		}

	}

	/////
	//	INSERT INTO table (p1,p2,p3,...) VALUES (?,?,?,...) ON DUPLICATE KEY UPDATE p2 = VALUES(p2), p3 = VALUES(p3), ...
	std::string prepared_statement_builder::build_update_statement()
	{
		std::string stat = "INSERT INTO " + table_name_ + " (";

		int32 idx = 0;
		for (auto& property : properties_)
		{
			if (idx) stat += ",";
			++idx;

			stat += property->name;
		}

		stat += ") VALUES (";
		idx = 0;

		for (auto& property : properties_)
		{
			(void)property;
			if (idx) stat += ",";
			++idx;

			stat += "?";
		}

		stat += ") ON DUPLICATE KEY UPDATE ";
		idx = -1;
		
		for (auto& property : properties_)
		{
			if (idx > 0) stat += ",";
			if (idx >= 0)
			{
				stat += property->name + "=VALUES(" + property->name + ")";
			}

			++idx;
		}

		return stat;
	}

	std::string prepared_statement_builder::build_load_statement(const std::string& key)
	{
		std::string stat = "SELECT ";

		int32 idx = 0;

		for (auto& property : properties_)
		{
			if (idx) stat += ",";
			++idx;

			stat += property->name;
		}

		stat += " FROM " + table_name_ + " WHERE " + key + " = ?";

		return stat;
	}

	std::string prepared_statement_builder::build_delete_statement(const std::string& key)
	{
		std::string stat = "DELETE FROM " + table_name_ + " WHERE " + key + " = ?";
		return stat;
	}

	//  构建存储过程语句
	//call pro_save_XXX(?,?,?,...)
	std::string prepared_statement_builder::build_call_proc_save_statement()
	{
		std::string stat = "CALL pro_save_" + table_name_ + "(";
		int32 idx = 0;
		for (auto& property : properties_)
		{
			(void)property;
			if (idx) stat += ",";
			++idx;

			stat += "?";
		}
		stat += ")";

		return stat;
	}

	/////
	template<> void db_setter_function<int8>(sql::PreparedStatement* ps, uint32 idx, void* ptr)	{ps->setInt(idx, *(int8*)ptr);}
	template<> void db_setter_function<int16>(sql::PreparedStatement* ps, uint32 idx, void* ptr) { ps->setInt(idx, *(int16*)ptr); }
	template<> void db_setter_function<int32>(sql::PreparedStatement* ps, uint32 idx, void* ptr) { ps->setInt(idx, *(int32*)ptr); }
	template<> void db_setter_function<int64>(sql::PreparedStatement* ps, uint32 idx, void* ptr) { ps->setUInt64(idx, *(int64*)ptr); }
	template<> void db_setter_function<uint8>(sql::PreparedStatement* ps, uint32 idx, void* ptr) { ps->setUInt(idx, *(uint8*)ptr); }
	template<> void db_setter_function<uint16>(sql::PreparedStatement* ps, uint32 idx, void* ptr) { ps->setUInt(idx, *(uint16*)ptr); }
	template<> void db_setter_function<uint32>(sql::PreparedStatement* ps, uint32 idx, void* ptr) { ps->setUInt(idx, *(uint32*)ptr); }
	template<> void db_setter_function<uint64>(sql::PreparedStatement* ps, uint32 idx, void* ptr) { ps->setUInt64(idx, *(uint64*)ptr); }
	template<> void db_setter_function<std::string>(sql::PreparedStatement* ps, uint32 idx, void* ptr) { ps->setString(idx, ((std::string*)ptr)->c_str()); }
	
	template<> void db_getter_function<int8>(sql::ResultSet* rs, uint32 idx, void* ptr) { *(int8*)ptr = (int8)rs->getInt(idx); }
	template<> void db_getter_function<int16>(sql::ResultSet* rs, uint32 idx, void* ptr) { *(int16*)ptr = (int16)rs->getInt(idx); }
	template<> void db_getter_function<int32>(sql::ResultSet* rs, uint32 idx, void* ptr) { *(int32*)ptr = (int32)rs->getInt(idx); }
	template<> void db_getter_function<int64>(sql::ResultSet* rs, uint32 idx, void* ptr) { *(int64*)ptr = (int64)rs->getInt64(idx); }
	template<> void db_getter_function<uint8>(sql::ResultSet* rs, uint32 idx, void* ptr) { *(uint8*)ptr = (uint8)rs->getUInt(idx); }
	template<> void db_getter_function<uint16>(sql::ResultSet* rs, uint32 idx, void* ptr) { *(uint16*)ptr = (uint16)rs->getUInt(idx); }
	template<> void db_getter_function<uint32>(sql::ResultSet* rs, uint32 idx, void* ptr) { *(uint32*)ptr = (uint32)rs->getUInt(idx); }
	template<> void db_getter_function<uint64>(sql::ResultSet* rs, uint32 idx, void* ptr) { *(uint64*)ptr = (uint64)rs->getUInt64(idx); }
	template<> void db_getter_function<std::string>(sql::ResultSet* rs, uint32 idx, void* ptr) { *(std::string*)ptr = rs->getString(idx).c_str(); }

}

