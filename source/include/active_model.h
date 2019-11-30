#ifndef ZXERO_GAMECOMMON_ACTIVE_MODEL_H__
#define ZXERO_GAMECOMMON_ACTIVE_MODEL_H__


#include "types.h"
#include "dbcontext.h"
#include <boost/shared_ptr.hpp>
#include <boost/variant.hpp>
#include <boost/algorithm/string.hpp>
#ifdef ZXERO_OS_WINDOWS
#pragma warning(disable:4251)
#pragma warning(disable:4100)
#endif
#include <cppconn/sqlstring.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/connection.h>
#include <cppconn/statement.h>
#include <cppconn/resultset.h>
#include <mysql_driver.h>
#ifdef ZXERO_OS_WINDOWS
#pragma warning(default:4100)
#pragma warning(default:4251)
#endif
#include <google/protobuf/message.h>
#include <boost/variant.hpp>
#include <boost/format.hpp>
#include <boost/algorithm/string/join.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/stream_buffer.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/ptr_container/ptr_vector.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/lexical_cast.hpp>
#include <string>
#include <map>
#include <vector>
#include <ProtoBufOption.pb.h>
#include <google/protobuf/descriptor.pb.h>

namespace sql
{
	class Connection;
	class Driver;
}

#ifdef GetMessage
#undef GetMessage
#endif // GetMessage


namespace ActiveModel
{
	struct FieldSqlProperty
	{
		std::string name_;
		std::string type_;
		bool primary_key_;
		std::string non_unique_key_name_; //非唯一索引的索引名
		std::string unique_key_name_; //唯一索引的索引名
	};

	typedef boost::variant<const boost::shared_ptr<std::stringstream> , bool, const char*, zxero::int16, zxero::int32, zxero::uint32, zxero::int64, zxero::uint64, zxero::real32, zxero::real64, std::string> sql_value_type;
	struct sql_value_type_string_visitor
	{
		typedef std::string result_type;
		std::string operator()(const std::string& str) const { return std::string('"' + str + '"'); }
		std::string operator()(const char* str) const { return std::string('"' + std::string(str) + '"'); }

		std::string operator()(const boost::shared_ptr<std::stringstream>& /*stream*/) const {
			return "?";
		}
		template <typename NonStrType>
		std::string operator()(const NonStrType& value) const { return boost::lexical_cast<std::string>(value); }
	};
/*
	struct sql_value_type_blob_visitor
	{
		typedef  const google::protobuf::Message* result_type;
		const google::protobuf::Message* operator()(google::protobuf::Message* msg) const {
			return msg;
		}
		template <typename Other>
		const google::protobuf::Message* operator()(const Other& / *value* /) const { return nullptr; }
	};*/

	struct sql_value_type_lambda_setter_visitor : public boost::static_visitor<std::function<void(const boost::shared_ptr<sql::PreparedStatement>& stat, zxero::int32)>>
	{
		typedef const boost::shared_ptr<sql::PreparedStatement>& stat_type;
		result_type operator()(const google::protobuf::Message* msg) const {
			return [=](stat_type stat, zxero::int32 index) { 
				std::stringstream stream;
				msg->SerializePartialToOstream(&stream);
				stat->setBlob(index, &stream); 
			};
		}
		result_type operator()(const boost::shared_ptr<std::stringstream>& stream) const {
			return [=](stat_type stat, zxero::int32 index) {
				stat->setBlob(index, stream.get());
			};
		}
		result_type operator()(const bool value) const {
			return [=](stat_type stat, zxero::int32 index) { stat->setBoolean(index, value); };
		}
		result_type operator()(const char* value) const {
			return [=](stat_type stat, zxero::int32 index) { std::string str(value);stat->setString(index, str); };
		}
		result_type operator()(const zxero::int16 value) const {
			return [=](stat_type stat, zxero::int32 index) { stat->setInt(index, value); };
		}
		result_type operator()(const zxero::int32 value) const {
			return [=](stat_type stat, zxero::int32 index) { stat->setInt(index, value); };
		}
		result_type operator()(const zxero::uint32 value) const {
			return [=](stat_type stat, zxero::int32 index) { stat->setUInt(index, value); };
		}
		result_type operator()(const zxero::int64 value) const {
			return [=](stat_type stat, zxero::int32 index) { stat->setInt64(index, value); };
		}
		result_type operator()(const zxero::uint64 value) const {
			return [=](stat_type stat, zxero::int32 index) { stat->setUInt64(index, value); };
		}
		result_type operator()(const zxero::real32 value) const {
			return [=](stat_type stat, zxero::int32 index) { stat->setDouble(index, value); };
		}
		result_type operator()(const zxero::real64 value) const {
			return [=](stat_type stat, zxero::int32 index) { stat->setDouble(index, value); };
		}
		result_type operator()(const std::string value) const {
			return [=](stat_type stat, zxero::int32 index) { stat->setString(index, value); };
		}

	};
	//TODO: 使用protobuf 的option来增加字段描述, 索引定义等
	//MessageType 只能是google::protobuf::Message的子类
	template <typename MessageType>
	class Base
	{
	public:
		Base(const boost::shared_ptr<zxero::dbcontext>& ctx)
			:conn_(ctx->get_connection())
		{
			auto descriptor = message_.GetDescriptor();
			table_name_ = descriptor->name();
			RecursionMessage(message_.GetDescriptor());
			if (!have_primary_key_) {
				auto it = std::find_if(std::begin(column_and_types_), std::end(column_and_types_), boost::bind(&FieldSqlProperty::name_, _1) == "guid");
				if (it != std::end(column_and_types_)) {
					it->primary_key_ = true;
					have_primary_key_ = true;
				}
			}
			ZXERO_ASSERT(have_primary_key_) << "not primary key found";
		}

		boost::shared_ptr<MessageType> FindOrCreateOne(const std::map<std::string, sql_value_type>& find_hash) {
			if (CheckExist(find_hash)) {
				return FindOne(find_hash);
			}
			else {
				return Create(find_hash);
			}
		}

		boost::shared_ptr<MessageType> CreateAndLoad(const std::map<std::string, sql_value_type>& create_hash,
			const std::map<std::string, sql_value_type>& load_hash)
		{
			Create(create_hash, false);
			return FindOne(load_hash);
		}

		//根据find_hash创建一条字段
		boost::shared_ptr<MessageType> Create(const std::map<std::string, sql_value_type>& create_hash, bool load = true) {
			std::vector<std::string> insert_names;
			std::vector<std::string> insert_values;
			for (auto field : column_and_types_) {
				if (field.primary_key_) {
					ZXERO_ASSERT(create_hash.find(field.name_) != std::end(create_hash)) << "should set primary key value";
				}
			}
			std::remove_const<std::remove_reference<decltype(create_hash)>::type>::type load_hash;
			for (auto& it : create_hash) {
				insert_names.push_back("`" + it.first + "`");
				insert_values.push_back(" ? ");
				auto field_type = *std::find_if(std::begin(column_and_types_), std::end(column_and_types_), boost::bind(&FieldSqlProperty::name_, _1) == it.first);
				if (field_type.primary_key_) {
					load_hash.insert({ it.first, it.second });
				}
			}
			std::string sql_str = (boost::format("INSERT INTO `%1%` (%2%) VALUES (%3%);") % table_name_ %
				boost::algorithm::join(insert_names, ",") %
				boost::algorithm::join(insert_values, ",")).str();
			boost::shared_ptr<sql::PreparedStatement> prepare_stat(conn_->prepareStatement(sql_str));
			auto index = 1;
			for (auto it = std::begin(create_hash); it != std::end(create_hash); ++it, ++index) {
				boost::apply_visitor(sql_value_type_lambda_setter_visitor(), it->second)(prepare_stat, index);
			}
			prepare_stat->execute();
			if (load) {
				return FindOne(load_hash);
			}
			else {
				return boost::shared_ptr<MessageType>(nullptr);
			}

		}

		void DeleteAll()
		{
			std::string sql_str = (boost::format("DELETE FROM `%1%`") % table_name_).str();
			boost::scoped_ptr<sql::Statement> delete_stat(conn_->createStatement());
			delete_stat->execute(sql_str);
		}

		void Delete(const std::map<std::string, sql_value_type>& delete_hash)
		{
			std::string sql_str = (boost::format("DELETE FROM `%1%` WHERE ") % table_name_).str();
			std::vector<std::string> delete_conditons;
			for (auto& it : delete_hash) {
				delete_conditons.push_back(
					(boost::format("( `%1%` = %2% )") % it.first % boost::apply_visitor(sql_value_type_string_visitor(), it.second)).str());
			}
			sql_str += boost::algorithm::join(delete_conditons, " and ");
			boost::scoped_ptr<sql::Statement> delete_stat(conn_->createStatement());
			delete_stat->execute(sql_str);
		}

		void Save(const boost::shared_ptr<MessageType>& message) 
		{
			return Save(*message);
		}

		void Save(const google::protobuf::Message& message) 
		{
			std::map<std::string, sql_value_type> check_values;
			for (auto field : column_and_types_) {
				if (field.primary_key_) {
					if (field.type_ == "uint64")
						check_values.insert({ field.name_, (zxero::uint64)message.GetReflection()->GetUInt64(message, message.GetDescriptor()->FindFieldByName(field.name_)) });
					else if (field.type_ == "int32")
						check_values.insert({ field.name_, (zxero::int32)message.GetReflection()->GetInt32(message, message.GetDescriptor()->FindFieldByName(field.name_)) });
					else if (field.type_ == "enum")
						check_values.insert({ field.name_, message.GetReflection()->GetEnum(message, message.GetDescriptor()->FindFieldByName(field.name_))->number() });
					else
						ZXERO_ASSERT(false);
				}
			}
			if (CheckExist(check_values) == false) {
				std::map<std::string, sql_value_type> create_hash;
				__PushMessage(create_hash, message);
				CreateAndLoad(create_hash, check_values);
			}
			Update(message);
		}
		void __PushMessage(std::map<std::string, sql_value_type>& update_hash, const google::protobuf::Message& message, std::string prefix = "") {
			auto reflection = message.GetReflection();
			auto descriptor = message.GetDescriptor();

			for (auto i = 0; i < descriptor->field_count(); ++i) {
				auto field_descriptor = descriptor->field(i);
				auto sql_type = field_descriptor->options().GetExtension(Packet::sql_type);
				boost::to_upper(sql_type);
				if (sql_type == "TINYBLOB" || sql_type == "MEDIUMBLOB" || sql_type == "BLOB" || sql_type == "LONGBLOB") {
					auto stream = boost::make_shared<std::stringstream>();
					reflection->GetMessage(message, field_descriptor).SerializePartialToOstream(stream.get());
					update_hash.insert({ prefix + field_descriptor->name(), stream});
				} else if (field_descriptor->is_required()) {
					switch (field_descriptor->cpp_type())
					{
					case google::protobuf::FieldDescriptor::CppType::CPPTYPE_INT32:
						update_hash.insert({ prefix + field_descriptor->name(), (zxero::int32)reflection->GetInt32(message, field_descriptor) });
						break;
					case google::protobuf::FieldDescriptor::CppType::CPPTYPE_INT64:
						update_hash.insert({ prefix + field_descriptor->name(), (zxero::int64)reflection->GetInt64(message, field_descriptor) });
						break;
					case google::protobuf::FieldDescriptor::CppType::CPPTYPE_UINT32:
						update_hash.insert({ prefix + field_descriptor->name(), (zxero::uint32)reflection->GetUInt32(message, field_descriptor) });
						break;
					case google::protobuf::FieldDescriptor::CppType::CPPTYPE_UINT64:
						update_hash.insert({ prefix + field_descriptor->name(), (zxero::uint64)reflection->GetUInt64(message, field_descriptor) });
						break;
					case google::protobuf::FieldDescriptor::CppType::CPPTYPE_FLOAT:
						update_hash.insert({ prefix + field_descriptor->name(), (zxero::real32)reflection->GetFloat(message, field_descriptor) });
						break;
					case google::protobuf::FieldDescriptor::CppType::CPPTYPE_DOUBLE:
						update_hash.insert({ prefix + field_descriptor->name(), (zxero::real64)reflection->GetDouble(message, field_descriptor) });
						break;
					case google::protobuf::FieldDescriptor::CppType::CPPTYPE_BOOL:
						update_hash.insert({ prefix + field_descriptor->name(), reflection->GetBool(message, field_descriptor) });
						break;
					case google::protobuf::FieldDescriptor::CppType::CPPTYPE_ENUM:
						update_hash.insert({ prefix + field_descriptor->name(), reflection->GetEnum(message, field_descriptor)->number() });
						break;
					case google::protobuf::FieldDescriptor::CppType::CPPTYPE_STRING:
						update_hash.insert({ prefix + field_descriptor->name(), reflection->GetString(message, field_descriptor) });
						break;
					case google::protobuf::FieldDescriptor::CppType::CPPTYPE_MESSAGE:
						__PushMessage(update_hash, reflection->GetMessage(message, field_descriptor), prefix + field_descriptor->name() + '.');
						break;
					default:
						ZXERO_ASSERT(false) << "some field type not add";
						break;
					}
				} else if (field_descriptor->is_repeated()) {
					auto field_count = field_descriptor->options().GetExtension(Packet::column_count);
					for (auto field_index = 0; field_index < field_count; ++field_index) {
						auto field_name = (boost::format("%1%%2%_%3%") % prefix % field_descriptor->name() % field_index).str();
						switch (field_descriptor->cpp_type())
						{
						case google::protobuf::FieldDescriptor::CppType::CPPTYPE_INT32:
							update_hash.insert({ field_name, (zxero::int32)reflection->GetRepeatedInt32(message, field_descriptor, field_index) });
							break;
						case google::protobuf::FieldDescriptor::CppType::CPPTYPE_INT64:
							update_hash.insert({ field_name, (zxero::int64)reflection->GetRepeatedInt64(message, field_descriptor, field_index) });
							break;
						case google::protobuf::FieldDescriptor::CppType::CPPTYPE_UINT32:
							update_hash.insert({ field_name, (zxero::uint32)reflection->GetRepeatedUInt32(message, field_descriptor, field_index) });
							break;
						case google::protobuf::FieldDescriptor::CppType::CPPTYPE_UINT64:
							update_hash.insert({ field_name, (zxero::uint64)reflection->GetRepeatedUInt64(message, field_descriptor, field_index) });
							break;
						case google::protobuf::FieldDescriptor::CppType::CPPTYPE_FLOAT:
							update_hash.insert({ field_name, (zxero::real32)reflection->GetRepeatedFloat(message, field_descriptor, field_index) });
							break;
						case google::protobuf::FieldDescriptor::CppType::CPPTYPE_DOUBLE:
							update_hash.insert({ field_name, (zxero::real64)reflection->GetRepeatedDouble(message, field_descriptor, field_index) });
							break;
						case google::protobuf::FieldDescriptor::CppType::CPPTYPE_BOOL:
							update_hash.insert({ field_name, reflection->GetRepeatedBool(message, field_descriptor, field_index) });
							break;
						case google::protobuf::FieldDescriptor::CppType::CPPTYPE_ENUM:
							update_hash.insert({ field_name, reflection->GetRepeatedEnum(message, field_descriptor, field_index)->number() });
							break;
						case google::protobuf::FieldDescriptor::CppType::CPPTYPE_STRING:
							update_hash.insert({ field_name, reflection->GetRepeatedString(message, field_descriptor, field_index) });
							break;
						case google::protobuf::FieldDescriptor::CppType::CPPTYPE_MESSAGE:
							__PushMessage(update_hash, reflection->GetRepeatedMessage(message, field_descriptor, field_index), field_name + '.');
							break;
						default:
							ZXERO_ASSERT(false) << "some field type not add";
							break;
						}
					}
				}
				
			}
		}

		void Update(const google::protobuf::Message& message) {
			std::map<std::string, sql_value_type> update_hash;
			__PushMessage(update_hash, message);
			std::vector<std::string> primary_key_values;
			for (auto& field : column_and_types_) {
				if (field.primary_key_) {
					if (field.type_ == "uint64")
						primary_key_values.push_back((boost::format("(`%1%` = %2%)") % field.name_ %  message.GetReflection()->GetUInt64(message, message.GetDescriptor()->FindFieldByName(field.name_))).str());
					else if (field.type_ == "int32")
						primary_key_values.push_back((boost::format("(`%1%` = %2%)") % field.name_ %  message.GetReflection()->GetInt32(message, message.GetDescriptor()->FindFieldByName(field.name_))).str());
					else if (field.type_ == "enum")
						primary_key_values.push_back((boost::format("(`%1%` = %2%)") % field.name_ %  message.GetReflection()->GetEnum(message, message.GetDescriptor()->FindFieldByName(field.name_))->number()).str());
					else
						ZXERO_ASSERT(false);
				}
			}
			std::vector<std::string> set_value;
			std::transform(std::begin(update_hash), std::end(update_hash), std::back_inserter(set_value), [](auto& origin) {
				return (boost::format(" `%1%` = ? ") % origin.first).str();
			});
			std::string where_sql = (boost::format("%1%") % boost::algorithm::join(primary_key_values, " AND ")).str();
			std::string prepared_update_sql = (boost::format("UPDATE `%1%` SET %2% WHERE %3%") %
				table_name_ % boost::algorithm::join(set_value, ",") % where_sql).str();
			boost::shared_ptr<sql::PreparedStatement> prepared_stat(conn_->prepareStatement(prepared_update_sql));
			auto index = 1;
			for (auto it = std::begin(update_hash); it != std::end(update_hash); ++it, ++index) {
				boost::apply_visitor(sql_value_type_lambda_setter_visitor(), it->second)(prepared_stat, index);
			}
			prepared_stat->execute();
		}

		//现在不会自己填充primary key的内容了, 得自己做
		void Create(MessageType& message)
		{
			std::map<std::string, sql_value_type> create_hash;
			__PushMessage(create_hash, message);
			auto new_message = Create(create_hash, true);
			message.CopyFrom(*new_message);
		}

		std::vector<boost::shared_ptr<MessageType>> FindAll(const std::map<std::string, sql_value_type>& find_hash) 
    {
			std::vector<boost::shared_ptr<MessageType>> ret;
			std::vector<std::string> conditions;
			for (auto& it : find_hash) {
				conditions.push_back((boost::format(" (`%1%` = %2%) ") % it.first %
					boost::apply_visitor(sql_value_type_string_visitor(), it.second)).str());
			}
			std::string load_sql = (boost::format("SELECT * FROM `%1%`;") % table_name_).str();
			if (conditions.size() > 0)
				load_sql = (boost::format("SELECT * FROM `%1%` WHERE %2%;") % table_name_ % boost::algorithm::join(conditions, " AND ")).str();
			boost::scoped_ptr<sql::Statement> statment(conn_->createStatement());
			boost::scoped_ptr<sql::ResultSet> result(statment->executeQuery(load_sql));
			while (result->next()) {
				auto message = boost::make_shared<MessageType>();
				__PullMessage(*result.get(), *message.get());
				ret.push_back(message);
			} //end while
			ZXERO_ASSERT(ret.size() < 2000) << table_name_ << "load data too many in once time."
				<< ret.size() << ". sql:" << load_sql;
			return ret;
		}

		bool CheckExist(const std::map<std::string, sql_value_type>& find_hash) {
			std::vector<std::string> conditions;
			for (auto& it : find_hash) {
				conditions.push_back((boost::format(" (`%1%` = %2%) ") % it.first %
					boost::apply_visitor(sql_value_type_string_visitor(), it.second)).str());
			}
			std::string check_sql_str = (boost::format("SELECT 1 FROM `%1%` WHERE %2% LIMIT 1;") % table_name_ % boost::algorithm::join(conditions, " AND ")).str();
			boost::scoped_ptr<sql::Statement> statment(conn_->createStatement());
			boost::scoped_ptr<sql::ResultSet> result(statment->executeQuery(check_sql_str));
			if (result->next())
				return true;
			else
				return false;
		}

		zxero::int32 Count(const std::map<std::string, sql_value_type>& find_hash = {})
		{
			std::string check_sql_str = (boost::format("SELECT COUNT(*) FROM `%1%`;") % table_name_).str();
			if (find_hash.size() != 0) {
				std::vector<std::string> conditions;
				for (auto& it : find_hash) {
					conditions.push_back((boost::format(" (`%1%` = %2%) ") % it.first %
						boost::apply_visitor(sql_value_type_string_visitor(), it.second)).str());
				}
				check_sql_str = (boost::format("SELECT COUNT(*) FROM `%1%` WHERE %2% LIMIT 1;") % table_name_ % boost::algorithm::join(conditions, " AND ")).str();		
			}
			boost::scoped_ptr<sql::Statement> statment(conn_->createStatement());
			boost::scoped_ptr<sql::ResultSet> result(statment->executeQuery(check_sql_str));
			ZXERO_ASSERT(result->next());
			return 	result->getInt(1);
		}

		void Delete(const google::protobuf::Message& msg)
		{
			std::map<std::string, sql_value_type> values;
			__PushMessage(values, msg);
			std::vector<std::string> conditions;
			for (auto field : column_and_types_) {
				if (field.primary_key_) {
					ZXERO_ASSERT(values.find(field.name_) != std::end(values)) << "should set primary key value";
					conditions.push_back((boost::format(" (`%1%` = %2%) ") % field.name_ %
						boost::apply_visitor(sql_value_type_string_visitor(), values[field.name_])).str());
				}
			}
			std::string sql_str = (boost::format("DELETE FROM `%1%` WHERE %2%") % table_name_ % boost::algorithm::join(conditions, " AND ")).str();
			boost::scoped_ptr<sql::Statement> statment(conn_->createStatement());
			statment->execute(sql_str);
			return;
		}
  
		void __PullMessage(const sql::ResultSet& result, google::protobuf::Message& message, std::string prefix = "") 
    {
			auto reflection = message.GetReflection();
			auto descriptor = message.GetDescriptor();
			for (auto i = 0; i < descriptor->field_count(); ++i) {
				auto field_descriptor = descriptor->field(i);
				auto sql_type = field_descriptor->options().GetExtension(Packet::sql_type);
				boost::to_upper(sql_type);
				if (sql_type == "TINYBLOB" || sql_type == "MEDIUMBLOB" || sql_type == "BLOB" || sql_type == "LONGBLOB") {
					auto sub_message = reflection->MutableMessage(&message, field_descriptor);
					boost::scoped_ptr<std::istream> stream(result.getBlob(prefix + field_descriptor->name()));
					auto parse_ret = sub_message->ParseFromIstream(stream.get());
					if (!parse_ret) {
						sub_message->SetInitialized();
						sub_message->SetDirty();
						sub_message->IsInitialized();
					}
				} else if (field_descriptor->is_required()) {
					switch (field_descriptor->cpp_type())
					{
					case google::protobuf::FieldDescriptor::CppType::CPPTYPE_INT32:
						reflection->SetInt32(&message, field_descriptor, result.getInt(prefix + field_descriptor->name()));
						break;
					case google::protobuf::FieldDescriptor::CppType::CPPTYPE_INT64:
						reflection->SetInt64(&message, field_descriptor, result.getInt64(prefix + field_descriptor->name()));
						break;
					case google::protobuf::FieldDescriptor::CppType::CPPTYPE_UINT32:
						reflection->SetUInt32(&message, field_descriptor, result.getUInt(prefix + field_descriptor->name()));
						break;
					case google::protobuf::FieldDescriptor::CppType::CPPTYPE_UINT64:
						reflection->SetUInt64(&message, field_descriptor, result.getUInt64(prefix + field_descriptor->name()));
						break;
					case google::protobuf::FieldDescriptor::CppType::CPPTYPE_DOUBLE:
						reflection->SetDouble(&message, field_descriptor, (double)result.getDouble(prefix + field_descriptor->name()));
						break;
					case google::protobuf::FieldDescriptor::CppType::CPPTYPE_FLOAT:
						reflection->SetFloat(&message, field_descriptor, (float)result.getDouble(prefix + field_descriptor->name()));
						break;
					case google::protobuf::FieldDescriptor::CppType::CPPTYPE_BOOL:
						reflection->SetBool(&message, field_descriptor, result.getBoolean(prefix + field_descriptor->name()));
						break;
					case google::protobuf::FieldDescriptor::CppType::CPPTYPE_ENUM:
						reflection->SetEnum(&message, field_descriptor, field_descriptor->enum_type()->FindValueByNumber(result.getInt(prefix + field_descriptor->name())));
						break;
					case google::protobuf::FieldDescriptor::CppType::CPPTYPE_STRING:
						reflection->SetString(&message, field_descriptor, result.getString(prefix + field_descriptor->name()).asStdString());
						break;
					case google::protobuf::FieldDescriptor::CppType::CPPTYPE_MESSAGE:
						__PullMessage(result, *reflection->MutableMessage(&message, field_descriptor), prefix + field_descriptor->name() + '.');
						break;
					default:
						ZXERO_ASSERT(false) << "some field type not add";
						break;
					}
				} else if (field_descriptor->is_repeated()) {
					auto field_count = field_descriptor->options().GetExtension(Packet::column_count);
					for (auto field_index = 0; field_index < field_count; ++field_index) {
						auto field_name = (boost::format("%1%%2%_%3%") % prefix % field_descriptor->name() % field_index).str();
						switch (field_descriptor->cpp_type())
						{
						case google::protobuf::FieldDescriptor::CppType::CPPTYPE_INT32:
							reflection->AddInt32(&message, field_descriptor,  result.getInt(field_name));
							break;
						case google::protobuf::FieldDescriptor::CppType::CPPTYPE_INT64:
							reflection->AddInt64(&message, field_descriptor,  result.getInt64(field_name));
							break;
						case google::protobuf::FieldDescriptor::CppType::CPPTYPE_UINT32:
							reflection->AddUInt32(&message, field_descriptor,  result.getUInt(field_name));
							break;
						case google::protobuf::FieldDescriptor::CppType::CPPTYPE_UINT64:
							reflection->AddUInt64(&message, field_descriptor,  result.getUInt64(field_name));
							break;
						case google::protobuf::FieldDescriptor::CppType::CPPTYPE_DOUBLE:
							reflection->AddDouble(&message, field_descriptor,  (double)result.getDouble(field_name));
							break;
						case google::protobuf::FieldDescriptor::CppType::CPPTYPE_FLOAT:
							reflection->AddFloat(&message, field_descriptor,  (float)result.getDouble(field_name));
							break;
						case google::protobuf::FieldDescriptor::CppType::CPPTYPE_BOOL:
							reflection->AddBool(&message, field_descriptor,  result.getBoolean(field_name));
							break;
						case google::protobuf::FieldDescriptor::CppType::CPPTYPE_ENUM:
							reflection->AddEnum(&message, field_descriptor,  field_descriptor->enum_type()->FindValueByNumber(result.getInt(field_name)));
							break;
						case google::protobuf::FieldDescriptor::CppType::CPPTYPE_STRING:
							reflection->AddString(&message, field_descriptor,  result.getString(field_name).asStdString());
							break;
						case google::protobuf::FieldDescriptor::CppType::CPPTYPE_MESSAGE:
							__PullMessage(result, *reflection->AddMessage(&message, field_descriptor), field_name + '.');
							break;
						default:
							ZXERO_ASSERT(false) << "some field type not add";
							break;
						}
					}
				}
				
			}
		}
		/**
		* \brief 直接根据sql条件查找内容
		* \param by
		* \return std::vector<boost::shared_ptr<MessageType>>
		*/
		std::vector<boost::shared_ptr<MessageType>> FindBy(const std::string& by, zxero::int32 count = 1000)
		{
			std::vector<boost::shared_ptr<MessageType>> ret;
			std::string load_sql_str = (boost::format("SELECT * FROM `%1%` WHERE %2% LIMIT %3%;") % table_name_ % by % count).str();
			boost::scoped_ptr<sql::Statement> load_stat(conn_->createStatement());
			boost::scoped_ptr<sql::ResultSet> result(load_stat->executeQuery(load_sql_str));
			while (result->next()) {
				auto msg = boost::make_shared<MessageType>();
				__PullMessage(*result, *msg);
				ret.push_back(msg);
			}
			return ret;
		}
		//先CheckExist在FineOne, 或者自己调用MessageType.has_set_guid()判断有没有内容
		boost::shared_ptr<MessageType> FindOne(const std::map<std::string, sql_value_type>& find_hash) {
			boost::shared_ptr<MessageType> message = boost::make_shared<MessageType>();
			std::vector<std::string> conditions;
			for (auto& it : find_hash) {
				conditions.push_back((boost::format(" (`%1%` = %2%) ") % it.first %
					boost::apply_visitor(sql_value_type_string_visitor(), it.second)).str());
			}
			std::string load_sql_str = (boost::format("SELECT * FROM `%1%` WHERE %2% LIMIT 1;") % table_name_ % boost::algorithm::join(conditions, " AND ")).str();
			boost::scoped_ptr<sql::Statement> load_stat(conn_->createStatement());
			boost::scoped_ptr<sql::ResultSet> result(load_stat->executeQuery(load_sql_str));
			if (result->next()) {
				__PullMessage(*result, *message.get());
				return message;
			} else {
				ZXERO_ASSERT(result->next()) << "should check data exist" << load_sql_str;
				return nullptr;
			}
		}
		//bool update(const std::map<std::string, sql_value_type>& find_hash);
		//bool save();
		std::string TableName() const;
		const std::list<FieldSqlProperty>& ColumnAndTypes() const;
	
	private:
		void RecursionMessage(const google::protobuf::Descriptor* descriptor, const std::string& prefix = "");
		const MessageType message_;
		const boost::shared_ptr<sql::Connection> conn_;
		std::string table_name_;
		bool have_primary_key_ = false;
		std::list<FieldSqlProperty> column_and_types_;
	};

	template < typename MessageType>
	std::string Base<MessageType>::TableName() const
	{
		return table_name_;
	}

	template < typename MessageType>
	const std::list<FieldSqlProperty>& Base<MessageType>::ColumnAndTypes() const
	{
		return column_and_types_;
	}

	template < typename MessageType>
	void Base<MessageType>::RecursionMessage(const google::protobuf::Descriptor* descriptor, const std::string& prefix /*= ""*/)
	{

		auto field_count = descriptor->field_count();
		for (auto i = 0; i < field_count; ++i) {
			auto field_desc = descriptor->field(i);
			if (field_desc->is_optional()) {
				//LOG_WARN << "optional field not support yet";
				continue;
			}
			auto sql_type = field_desc->options().GetExtension(Packet::sql_type);
			boost::to_upper(sql_type);
			if (sql_type == "TINYBLOB" || sql_type == "MEDIUMBLOB" || sql_type == "BLOB" || sql_type == "LONGBLOB") {
				
				auto field_name = prefix + field_desc->name();
				auto field_type = field_desc->options().GetExtension(Packet::sql_type);
				auto primary_key = field_desc->options().GetExtension(Packet::sql_primary_key);
				auto non_unique_key_name = field_desc->options().GetExtension(Packet::sql_non_unique_key_name);
				auto unique_key_name = field_desc->options().GetExtension(Packet::sql_unique_key_name);
				if (primary_key) {
					have_primary_key_ = true;
				}
				column_and_types_.push_back({ field_name, field_type, primary_key, non_unique_key_name, unique_key_name });
			} else if (field_desc->is_repeated()) {
				auto sub_field_count = field_desc->options().GetExtension(Packet::column_count);
				auto non_unique_key_name = field_desc->options().GetExtension(Packet::sql_non_unique_key_name);
				auto unique_key_name = field_desc->options().GetExtension(Packet::sql_unique_key_name);
				for (auto field_index = 0; field_index < sub_field_count; ++field_index) {
					if (field_desc->type() == google::protobuf::FieldDescriptor::Type::TYPE_MESSAGE) {
						RecursionMessage(field_desc->message_type(), (boost::format("%1%%2%_%3%.") % prefix % field_desc->name() % field_index).str());
					} else {
						auto field_name = (boost::format("%1%%2%_%3%") % prefix % field_desc->name() % field_index).str();
						auto field_type = field_desc->cpp_type_name();
						column_and_types_.push_back({ field_name, field_type, false, non_unique_key_name, unique_key_name});
					}
				}
				continue;
			} else if (field_desc->type() == google::protobuf::FieldDescriptor::Type::TYPE_MESSAGE) {
				RecursionMessage(field_desc->message_type(), prefix + field_desc->name() + ".");
			} else {
				auto field_name = prefix + field_desc->name();
				auto field_type = field_desc->options().GetExtension(Packet::sql_type);
				if (field_type.empty()) {
					field_type = field_desc->cpp_type_name();
				}
				auto primary_key = field_desc->options().GetExtension(Packet::sql_primary_key);
				auto non_unique_key_name = field_desc->options().GetExtension(Packet::sql_non_unique_key_name);
				auto unique_key_name = field_desc->options().GetExtension(Packet::sql_unique_key_name);
				if (primary_key) {
					have_primary_key_ = true;
				}
				column_and_types_.push_back({field_name, field_type, primary_key, non_unique_key_name, unique_key_name});
			}
		}
	}


	template <typename DbProvide>
	class Migration
	{
	public:
		Migration(const boost::shared_ptr<DbProvide>& provide, const std::string& db_name, bool delete_column = false)
			:provide_(provide),db_name_(db_name), delete_column_(delete_column) {}
		void Migrate(){
			//check create database
			ZXERO_ASSERT(TryCreateDb()) << " create db " << db_name_ << " failed";
			//check alter table
			for (auto table : all_tables_)
			{
				if (TryCreateTable(table.first, table.second)) {
					LOG_INFO << "create table " << table.first << " ok";
				}
				else {
					ZXERO_ASSERT(TryAlterTable(table.first, table.second)) << " alter table " << table.first << "fails";
				}
			}
		}
		template < typename MessageType>
		bool RegistTable(const Base<MessageType>& base){
			auto table_name = base.TableName();
			auto need_columns = base.ColumnAndTypes();
			all_tables_.insert({ table_name, need_columns });
			return true;
		}
		const boost::shared_ptr<DbProvide>& Provider() {
			return provide_;
		}

		const std::map<std::string, std::list<FieldSqlProperty>>& GetTables() {
			return all_tables_;
		}

	private:
		bool TryCreateTable(const std::string& table_name, const std::list<FieldSqlProperty>& table_define){
			boost::shared_ptr<sql::Statement> statment(provide_->createStatement());
			std::string sql_str = (boost::format("SHOW TABLES LIKE '%1%';") % table_name).str();
			try
			{
				boost::shared_ptr<sql::ResultSet> result(statment->executeQuery(sql_str));
				if (result->next()) { // got table, no need to create
					LOG_INFO << "table " << result->getString(1) << " exists";
					return false;
				}
				else {
					std::string table_scheme = MakeTableSchema(table_name, table_define);
					LOG_INFO << "execute: " << table_scheme;
					statment->execute(table_scheme);
					LOG_INFO << "execute: " << table_scheme << " done.";
					return true;
				}
			}
			catch (const sql::SQLException& e)
			{
				LOG_FATAL << "create table: " << table_name << "` failed: " << e.what();
			}
			return false;
		}
		bool TryAlterTable(const std::string& table_name, const std::list<FieldSqlProperty>& table_define){
			try
			{
				std::map<std::string, std::string> current_scheme;
				std::vector<std::string> primary_keys;
				boost::scoped_ptr<sql::Statement> statment(provide_->createStatement());
				boost::scoped_ptr<sql::ResultSet> result(statment->executeQuery("DESCRIBE `" + table_name + "`;"));
				while (result->next()) {
					std::string field_name = result->getString("Field");
					current_scheme.insert({ field_name, result->getString("Type").asStdString() });
					result->getString("Key");
				}
				std::map<std::string, std::vector<std::string>> old_non_unique_keys;
				std::map<std::string, std::vector<std::string>> old_unique_keys;
				std::map<std::string, std::vector<std::string>> new_non_unique_keys;
				std::map<std::string, std::vector<std::string>> new_unique_keys;
				for (auto& field : table_define) {
					if (!field.non_unique_key_name_.empty()) {
						new_non_unique_keys[field.non_unique_key_name_].push_back("`" + field.name_ + "`");
					}
					if (!field.unique_key_name_.empty()) {
						new_unique_keys[field.unique_key_name_].push_back("`" + field.name_ + "`");
					}
					auto target_type_it = mysql_column_map_.find(field.type_);
					auto target_type = target_type_it == mysql_column_map_.end() ? field.type_ : target_type_it->second;
					if (current_scheme.find(field.name_) == current_scheme.end()) { //no column, need create
						std::string default_value(" default 0");
						if (target_type == "varchar(100)") default_value = " default \"\"";
						if (target_type == "blob") default_value = "";
						std::string create_str = (boost::format("ALTER TABLE `%1%` ADD `%2%` %3% %4%;") % table_name % field.name_ % target_type % default_value).str();
						LOG_INFO << "excute: " << create_str;
						statment->execute(create_str);
						LOG_INFO << "excute: " << create_str << " done";
					}
					else {
						std::string current_type = current_scheme.find(field.name_)->second;
						if (current_type == target_type) {//nothing to do

						}
						else { //ALTER TABLE
							std::string alter_str = (boost::format("ALTER TABLE `%1%` MODIFY `%2%` %3%;") %
								table_name % field.name_ % target_type).str();
							LOG_INFO << "execute: " << alter_str;
							statment->execute(alter_str);
							LOG_INFO << "execute: " << alter_str << " done";
						}
					}
				}
				//增删索引
				boost::scoped_ptr<sql::ResultSet> index_result(statment->executeQuery("SHOW INDEX FROM `" + table_name + "`where Key_name <> \"PRIMARY\""));
				while (index_result->next()) {
					std::string index_name = index_result->getString("Key_name").asStdString();
					std::string column_name = index_result->getString("Column_name").asStdString();
					if (index_result->getBoolean("Non_unique")) {
						old_non_unique_keys[index_name].push_back(column_name);
					} else {
						old_unique_keys[index_name].push_back(column_name);
					}
				}
				for (auto it : new_non_unique_keys) {
					auto old_it = old_non_unique_keys.find(it.first);
					if (old_it != std::end(old_non_unique_keys) &&
						std::is_permutation(std::begin(it.second), std::end(it.second), std::begin(old_it->second), std::end(old_it->second)) == false) {
						std::string drop_str = (boost::format("DROP INDEX `%1%` ON `%2%`") % it.first % table_name).str();
						LOG_INFO << "execute: " << drop_str;
						statment->execute(drop_str);
						LOG_INFO << "execute: " << drop_str << " done";
					}
					std::string index_col_names = boost::algorithm::join(it.second, ",");
					std::string create_str = (boost::format("CREATE INDEX `%1%` ON `%2%`(%3%)") % it.first % table_name %  index_col_names).str();
						LOG_INFO << "execute: " << create_str;
						statment->execute(create_str);
						LOG_INFO << "execute: " << create_str << " done";
				}
			}
			catch (const sql::SQLException& e)
			{
				LOG_FATAL << "alter table `" << table_name << "`; failed: " << e.what();
			}
			return true;
		}

		bool TryCreateDb(){
			boost::format formater("CREATE DATABASE IF NOT EXISTS `%1%`  DEFAULT CHARACTER SET utf8 DEFAULT COLLATE utf8_general_ci; ");
			formater % db_name_;
			try
			{
				boost::scoped_ptr<sql::Statement> statement(provide_->createStatement());
				LOG_INFO << "execute: " << formater.str();
				statement->execute(formater.str());
				LOG_INFO << "execute: " << formater.str() << " done";
				provide_->setSchema(db_name_);
				ZXERO_ASSERT(provide_->getSchema() == db_name_) << "use database " << db_name_ << " fails";
			}
			catch (sql::SQLException& e)
			{
				LOG_FATAL << "create db " << db_name_ << " failed: " << e.what();
			}
			return true;
		}
		std::string MakeTableSchema(const std::string& table_name, const std::list<FieldSqlProperty>& table_define){
			std::vector<std::string> columns;
			std::string primary_key;
			std::vector<std::string> primary_keys;
			std::vector<std::string> non_unique_keys;
			std::vector<std::string> unique_keys;
			std::map<std::string, std::vector<std::string>> non_unique_key_meta;
			std::map<std::string, std::vector<std::string>> unique_key_meta;
			for (auto& field : table_define) {
				if (mysql_column_map_.find(field.type_) == std::end(mysql_column_map_)) {
					columns.push_back((boost::format("`%1%` %2%") % field.name_ % field.type_).str());
				}
				else {
		
					columns.push_back((boost::format("`%1%` %2%") % field.name_ % mysql_column_map_.find(field.type_)->second).str());
				}
				if (field.primary_key_) {
					primary_keys.push_back((boost::format("`%1%`") % field.name_).str());
				}
				if (!field.non_unique_key_name_.empty()) {
					auto& keys = non_unique_key_meta[field.non_unique_key_name_];
					keys.push_back((boost::format("`%1%`") % field.name_).str());
				}
				if (!field.unique_key_name_.empty()) {
					auto& keys = unique_key_meta[field.unique_key_name_];
					keys.push_back((boost::format("`%1%`") % field.name_).str());
				}
			}
			for (auto& it : non_unique_key_meta) {
				auto inner_keys = boost::algorithm::join(it.second, ",");
				non_unique_keys.push_back((boost::format("KEY `%1%` (%2%)") % it.first % inner_keys).str());
			}
			for (auto& it : unique_key_meta) {
				auto inner_keys = boost::algorithm::join(it.second, ",");
				unique_keys.push_back((boost::format("KEY `%1%` (%2%)") % it.first % inner_keys).str());
			}
			primary_key = (boost::format("PRIMARY KEY(%1%)") % boost::algorithm::join(primary_keys, ",")).str();
			std::vector<std::string> all;
			all.reserve(columns.size() + 1 + non_unique_keys.size() + unique_keys.size());
			all.insert(std::end(all), std::begin(columns), std::end(columns));
			all.insert(std::end(all), std::begin(non_unique_keys), std::end(non_unique_keys));
			all.insert(std::end(all), std::begin(unique_keys), std::end(unique_keys));
			all.push_back(primary_key);
			boost::format formater("CREATE TABLE IF NOT EXISTS `%1%` (\n%2%);");
			formater % table_name % boost::algorithm::join(all, ",");
			return formater.str();
		}
		std::map<std::string, std::list<FieldSqlProperty>> all_tables_;
		const std::map<std::string, std::string> mysql_column_map_ = { 
			{"int32" , "int(10)"},
			{"int64" , "bigint(20)"},
			{"uint32", "int(10) unsigned"},
			{"uint64", "bigint(20) unsigned"},
			{"double", "double"},
			{"float",  "float"},
			{"bool", "tinyint(1)"},
			{"enum", "int(8)"},
			{"string", "varchar(100)"},
		};
		bool delete_column_ = false;
		const boost::shared_ptr<DbProvide> provide_;
		std::string db_name_;
	};

}
#endif // ZXERO_GAMECOMMON_ACTIVE_MODEL_H__
