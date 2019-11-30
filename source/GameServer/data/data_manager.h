//加载各类表
#ifndef ZXERO_GAMESERVER_DATA_MANAGER_H__
#define ZXERO_GAMESERVER_DATA_MANAGER_H__


#include "../mt_types.h"
#include "data_table.h"
#include "data_value.h"
#include <LoginPacket.pb.h>
#include <boost/ptr_container/ptr_map.hpp>
#include <tuple>

namespace Mt
{
	const static int eTable_Count = 32;

	typedef std::map<std::string, data_table*> data_db;
	class data_manager
	{
	public:
		data_manager();
		~data_manager();
		int32 on_load();
		int32 on_unload();

		data_table* on_reload_file(const std::string& filename);
		data_table* on_load_file(const std::string& filepath, const std::string& fileName);
		void on_unload_file(const std::string& filename);
		void on_release();
		static data_manager* instance();

	public:
		data_table* get_table(const char *table_name);
		const data_value* get_value(const char *table_name, const char* attribute, int rowIndex);
		const data_value* get_value(const char *table_name, int columnIndex, int rowIndex);

	private:
		data_db m_tables;
	};

	//当前忽略optional字段
	//validator只检查repeated的message
	void FillMessageByRow(google::protobuf::Message& message,
		data_row& row, 
		std::string prefix = "",
		const std::map<std::string, std::function<bool(int32 value)>>& validator = std::map<std::string, std::function<bool(int32 value)>>());
	//void FillMessageByRowInternal(const google::protobuf::FieldDescriptor* field_descriptor, google::protobuf::Message& message, const char *col_name, data_row& row, bool isrepeat = false);
	//void FillRepeatedMessage(std::vector<google::protobuf::Message*> messages, data_row& row);

	//template <class ConfigMessage, typename KeyType>
	/*class TableManager {
	public:
		typedef google::protobuf::Message ConfigMessage;
		typedef std::tuple<int32,int32> KeyType;
		ConfigMessage* GetConfig(const KeyType& key) {
			if (configs_.find(key) == std::end(configs_)) {
				return nullptr;
			}
			else {
				return &configs_[key];
			}
		}
	protected:
		virtual std::map<std::string, int32> RepeatedFieldInfo() const { return std::map<std::string, int32>(); }
	private:
		bool __LoadData() {
			auto temp_message = Packet::AccountNameVerify();
			auto temp_key = KeyType();
			auto message_name = temp_message.GetTypeName();
			auto table = data_manager::instance()->get_table(message_name.c_str());
			if (table == nullptr)
				return false;
			for (auto i = 0; i < table->get_rows(); ++i) {
				auto row = table->find_row(i);
				auto message = temp_message.New();
				FillMessageByRow(*message, *row);
				auto repeated_fields = RepeatedFieldInfo();
				for (auto& repeated_field : repeated_fields) {
					auto reflection = message->GetReflection();
					auto descriptor = message->GetDescriptor();
					auto field_name = repeated_field.first;
					auto field_count = repeated_field.second;
					auto field_desc = descriptor->FindFieldByName(field_name);
					std::vector<google::protobuf::Message*> repeated_messages;
					repeated_messages.reserve(field_count);
					for (auto j = 0; j < field_count; ++j) {
						repeated_messages.push_back(reflection->MutableRepeatedMessage(message, field_desc, j));
						FillRepeatedMessage(repeated_messages, *row);
					}
				}
			}
			return true;
		}
		template <typename KeySize>
		KeyType MakeKey(const ConfigMessage& message) {
			BOOST_STATIC_ASSERT_MSG(KeySize >= 4, "should less than 4");
			
		}
		boost::ptr_map<KeyType, ConfigMessage> configs_;
	};*/
	
}
#endif // ZXERO_GAMESERVER_DATA_MANAGER_H__