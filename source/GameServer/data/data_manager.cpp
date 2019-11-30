#include "module.h"
#include "app.h"
#include "data_manager.h"
#include "event_loop.h"
#include "work_pool.h"
#include "work.h"
#include <thread>
#include <atomic>
#include "../mt_config/mt_config.h"
#include "utils.h"
#include <ProtoBufOption.pb.h>
#include <google/protobuf/descriptor.pb.h>
#include <google/protobuf/message.h>
#include <boost/format.hpp>
#include <boost/algorithm/string.hpp>

//必须在最后include！-------------------
#include "../zxero/mem_debugger.h"
//------------------------------------

namespace Mt
{
	using namespace zxero;
	static data_manager *sg_data_manager = nullptr;
	REGISTER_MODULE(data_manager)
	{
		require("luafile_manager");
		register_load_function(STAGE_LOAD, boost::bind(&data_manager::on_load, data_manager::instance()));
		register_unload_function(STAGE_POST_UNLOAD, boost::bind(&data_manager::on_unload, data_manager::instance()));

	}

	data_manager::data_manager()
	{
		sg_data_manager = this;
	}

	data_manager::~data_manager(void)
	{
		on_release();
	}

	data_manager* data_manager::instance()
	{
		return sg_data_manager;
	}
	int32 data_manager::on_load()
	{
		std::vector<std::string> file_list;
		std::string tablePath = MtConfig::Instance().conf_data_dir_;
		int32 ret = utils::enum_dir_files( tablePath,file_list );

		if( file_list.empty() )
		{
			LOG_ERROR << "[on_load]enum_dir_files:: can't find config data files! the config path=" << tablePath.c_str();
			return ret;
		}
		network::event_loop loop;
		work_pool data_load_work_pool(&loop, "file_load");
		struct file_load_work : public work {
			file_load_work(const std::string& file_path, 
				const std::string& file_name,
				std::map<std::string, data_table*>& result_db,
				std::atomic<uint32>& load_count)
				:file_name_(file_name), file_path_(file_path),
				data_table_(nullptr), result_db_(result_db),load_count_(load_count) {
				work_ = [&](boost::any& /*any*/) {this->load_file(); };
				done_ = [&](const boost::shared_ptr<zxero::work>& /*w*/) {this->load_done(); };
			}
			void load_file() {
				data_table_ = new data_table();
				size_t size;
				auto file = file_path_ + file_name_;
				LOG_INFO << "Load table start: " << file_name_;
				if (data_table_->load(file.c_str(), "rb", &size) == false) {
					delete data_table_;
				}
				mark_success();
			}
			void load_done() {
				if (data_table_) {
					result_db_.insert({ file_name_, data_table_ });
				}
				load_count_++;
				LOG_INFO << "Load table done: " << file_name_;
			}
			virtual const std::string& name() const override
			{
				return name_;
			}
			const std::string& file_name_;
			const std::string& file_path_;
			std::string name_ = "file_load";		
			data_table* data_table_ = nullptr;
			std::map<std::string, data_table*>& result_db_;
			std::atomic<uint32>& load_count_;
		};
		auto thread_count = std::thread::hardware_concurrency() / 2;
		thread_count = thread_count == 0 ? 1 : thread_count;
		data_load_work_pool.start(thread_count);
		std::atomic<uint32> load_count(0);
		auto final_count = file_list.size();
		for( auto& child:file_list)
		{
 			data_load_work_pool.add_work(new file_load_work(tablePath, child, m_tables, load_count));
			//on_load_file(tablePath,child);
		}
		loop.run_every(seconds(1), 
			[&]() {
			if (load_count == final_count){
				data_load_work_pool.stop();
				loop.quit();
			}
		});
		loop.loop();
		return 0;
	}

	data_table* data_manager::on_load_file(const std::string& filepath, const std::string& filename)
	{
		size_t size;
		std::string path_filenam = filepath + filename;
		data_table* pDataTable = SAFE_NEW data_table();
		if( pDataTable->load( path_filenam.c_str(), "rb", &size ))
		{
			m_tables.insert(std::make_pair(filename,pDataTable));
			return pDataTable;
		}
		else
		{
			SAFE_DELETE(pDataTable);
			return nullptr;
		}
	}

	data_table* data_manager::on_reload_file(const std::string& filename)
	{
		on_unload_file(filename+".csv");
		return on_load_file(MtConfig::Instance().conf_data_dir_, filename + ".csv");
	}

	void  data_manager::on_unload_file(const std::string& filename)
	{
		auto p_data_iter = m_tables.find(filename);
		if (p_data_iter != m_tables.end())
		{
			SAFE_DELETE(p_data_iter->second);
			m_tables.erase(p_data_iter);
		}
		else
		{
			ZXERO_ASSERT(false)<<"data_manager::on_unload_file,can not find file name="<<filename;
		}
	}
	int32 data_manager::on_unload()
	{
		on_release();
		return 0;
	}

	void data_manager::on_release()
	{
		for( auto &child:m_tables )
		{
			if(child.second)
			{
				child.second->release();
			}
			SAFE_DELETE(child.second);
		}

		m_tables.clear();
	}

	data_table* data_manager::get_table( const char *table_name )
	{
		const auto& iter = m_tables.find(table_name);
		if( iter != m_tables.end() )
			return iter->second;
		else {
			std::string table(table_name);
			const auto& csv_iter = m_tables.find(table += ".csv");
			if (csv_iter != m_tables.end()) {
				return csv_iter->second;
			}
			else {
				return nullptr;
			}
		}
	}

	const data_value* data_manager::get_value( const char *table_name, const char* attribute, int rowIndex)
	{
		const data_table* pTable = get_table(table_name);
		return pTable->get_value(attribute, rowIndex);
	}


	const data_value* data_manager::get_value( const char *table_name, int columnIndex, int rowIndex)
	{
		const data_table* pTable = get_table(table_name);
		return pTable->get_value(columnIndex, rowIndex);
	}

	void FillMessageByRow(google::protobuf::Message& message,
		data_row& row,
		std::string prefix /*= ""*/,
		const std::map<std::string, std::function<bool(int32 value)>>& validator /*= std::map<std::string, std::function<bool(int32 value)>>()*/)
	{
		auto reflection = message.GetReflection();
		auto descriptor = message.GetDescriptor();
		for (auto i = 0; i < descriptor->field_count(); ++i) {
			auto field_descriptor = descriptor->field(i);
			if (field_descriptor->label() == google::protobuf::FieldDescriptor::Label::LABEL_REQUIRED) {

				auto table_field_name_stub = prefix + field_descriptor->name();
				auto table_field_name = table_field_name_stub.c_str();

				switch (field_descriptor->cpp_type())
				{
				case google::protobuf::FieldDescriptor::CppType::CPPTYPE_INT32:

					reflection->SetInt32(&message, field_descriptor, row.getInt(table_field_name));
					break;
				case google::protobuf::FieldDescriptor::CppType::CPPTYPE_INT64:
					reflection->SetInt64(&message, field_descriptor, (zxero::int64)row.getInt(table_field_name));
					break;
				case google::protobuf::FieldDescriptor::CppType::CPPTYPE_UINT32:
					reflection->SetInt64(&message, field_descriptor, (zxero::uint32)row.getInt(table_field_name));
					break;
				case google::protobuf::FieldDescriptor::CppType::CPPTYPE_UINT64:
					reflection->SetInt64(&message, field_descriptor, (zxero::uint64)row.getInt(table_field_name));
					break;
				case google::protobuf::FieldDescriptor::CppType::CPPTYPE_DOUBLE:
					reflection->SetDouble(&message, field_descriptor, (double)row.getFloat(table_field_name));
					break;
				case google::protobuf::FieldDescriptor::CppType::CPPTYPE_FLOAT:
					reflection->SetFloat(&message, field_descriptor, (float)row.getFloat(table_field_name));
					break;
				case google::protobuf::FieldDescriptor::CppType::CPPTYPE_BOOL:
					reflection->SetBool(&message, field_descriptor, row.getInt(table_field_name) > 0);
					break;
				case google::protobuf::FieldDescriptor::CppType::CPPTYPE_ENUM:
					reflection->SetEnum(&message, field_descriptor, field_descriptor->enum_type()->FindValueByNumber(row.getInt(table_field_name)));
					break;
				case google::protobuf::FieldDescriptor::CppType::CPPTYPE_STRING:
					reflection->SetString(&message, field_descriptor, row.getString(table_field_name));
					break;
				case google::protobuf::FieldDescriptor::CppType::CPPTYPE_MESSAGE:
					FillMessageByRow(*reflection->MutableMessage(&message, field_descriptor), row, table_field_name_stub + ".");
					break;
				default:
					ZXERO_ASSERT(false) << "some field type not add";
					break;
				}
			}
			if (field_descriptor->label() == google::protobuf::FieldDescriptor::Label::LABEL_REPEATED) {
				auto table_field_name_stub = field_descriptor->name();
				auto field_count = field_descriptor->options().GetExtension(Packet::column_count);
				bool int_or_enum = false;
				if (field_descriptor->cpp_type() == google::protobuf::FieldDescriptor::CppType::CPPTYPE_ENUM ||
					field_descriptor->cpp_type() == google::protobuf::FieldDescriptor::CppType::CPPTYPE_INT32)
				{
					int_or_enum = true;
				}
				if (int_or_enum && row.isString(table_field_name_stub.c_str())) {
					std::vector<std::string> temp_strs;
					std::string string_values(row.getString(table_field_name_stub.c_str()));
					if (string_values != "") {
						boost::split(temp_strs, string_values, boost::is_any_of("|"));
						for (auto& buff_id_str : temp_strs) {
							auto value = boost::lexical_cast<int32>(buff_id_str);
							if (field_descriptor->cpp_type() == google::protobuf::FieldDescriptor::CppType::CPPTYPE_ENUM)
								reflection->AddEnum(&message, field_descriptor, field_descriptor->enum_type()->FindValueByNumber(value));
							if (field_descriptor->cpp_type() == google::protobuf::FieldDescriptor::CppType::CPPTYPE_INT32)
								reflection->AddInt32(&message, field_descriptor, value);
						}
					}
				}
				else {
					for (auto field_index = 0; field_index < field_count; ++field_index) {
						auto _table_field_name = (boost::format("%1%_%2%") % table_field_name_stub % field_index).str();
						auto table_field_name = _table_field_name.c_str();
						switch (field_descriptor->cpp_type())
						{
						case google::protobuf::FieldDescriptor::CppType::CPPTYPE_INT32:
						{
							auto value = row.getInt(table_field_name);
							if (value >= 0)
								reflection->AddInt32(&message, field_descriptor, value);
						}
							break;
						case google::protobuf::FieldDescriptor::CppType::CPPTYPE_INT64:
						{
							auto value = (zxero::int64)row.getInt(table_field_name);
							if (value >= 0)
								reflection->AddInt64(&message, field_descriptor, value);
						}
							break;
						case google::protobuf::FieldDescriptor::CppType::CPPTYPE_UINT32:
						{
							auto value = (zxero::uint32)row.getInt(table_field_name);
							if (value >= 0)
								reflection->AddUInt32(&message, field_descriptor, value);
						}
							break;
						case google::protobuf::FieldDescriptor::CppType::CPPTYPE_UINT64:
						{
							auto value = (zxero::uint64)row.getInt(table_field_name);
							if (value >= 0)
								reflection->AddUInt64(&message, field_descriptor, value);
						}
							break;
						case google::protobuf::FieldDescriptor::CppType::CPPTYPE_DOUBLE:
						{
							auto value = (real64)row.getFloat(table_field_name);
							if (value >= 0)
								reflection->AddDouble(&message, field_descriptor, value);
						}
							break;
						case google::protobuf::FieldDescriptor::CppType::CPPTYPE_FLOAT:
						{
							auto value = (real32)row.getFloat(table_field_name);
							if (value >= 0)
								reflection->AddFloat(&message, field_descriptor, value);
						}
							break;
						case google::protobuf::FieldDescriptor::CppType::CPPTYPE_BOOL:
						{
							auto value = row.getInt(table_field_name) > 0;
							reflection->AddBool(&message, field_descriptor, value);
						}
							break;
						case google::protobuf::FieldDescriptor::CppType::CPPTYPE_ENUM:
						{
							auto value = row.getInt(table_field_name);
							if (value >= 0)
								reflection->AddEnum(&message, field_descriptor, field_descriptor->enum_type()->FindValueByNumber(value));
						}
							break;
						case google::protobuf::FieldDescriptor::CppType::CPPTYPE_STRING:
						{
							std::string value = row.getString(table_field_name);
							if (value != "")
								reflection->AddString(&message, field_descriptor, value);
						}
							break;
						case google::protobuf::FieldDescriptor::CppType::CPPTYPE_MESSAGE:
						{
							auto field_message_desc = field_descriptor->message_type();
							auto first_sub_field = field_message_desc->field(0);
							if (validator.find(first_sub_field->name()) != std::end(validator)) {
								auto first_sub_field_name = _table_field_name + "." + first_sub_field->name();
								auto value = row.getInt(first_sub_field_name.c_str());
								auto checker = validator.find(first_sub_field->name())->second;
								if (checker(value)) {
									FillMessageByRow(*reflection->AddMessage(&message, field_descriptor), row, _table_field_name + ".");
								}
							} else {
								FillMessageByRow(*reflection->AddMessage(&message, field_descriptor), row, _table_field_name + ".");
							}
						}
							break;
						default:
							ZXERO_ASSERT(false) << "some field type not add";
							break;
						}
					}
				}
			}
		}
	}


}

