#include "pb2json.h"
#include "google/protobuf/message.h"
#include "google/protobuf/descriptor.h"

namespace zxero
{
	void FormatRepeatedField(Json::Value& value, const ::google::protobuf::Message& msg, const ::google::protobuf::FieldDescriptor *field, const ::google::protobuf::Reflection *reflection)
	{
		char int64str[25];
		if (NULL == field || NULL == reflection)
		{
			FormatToJson(value, msg);
		}

		for (int i = 0; i < reflection->FieldSize(msg, field); ++i)
		{
			Json::Value tmp_value;
			switch (field->type())
			{
			case google::protobuf::FieldDescriptor::TYPE_MESSAGE:
			{
				const google::protobuf::Message& tmp_msg = reflection->GetRepeatedMessage(msg, field, i);
				if (0 != tmp_msg.ByteSize())
				{
					FormatToJson(tmp_value, tmp_msg);
				}
			}
			break;
			case google::protobuf::FieldDescriptor::TYPE_INT32:
				tmp_value = reflection->GetRepeatedInt32(msg, field, i);
				break;
			case google::protobuf::FieldDescriptor::TYPE_UINT32:
				tmp_value = reflection->GetRepeatedUInt32(msg, field, i);
				break;
				break;
			case google::protobuf::FieldDescriptor::TYPE_INT64:
			{
				memset(int64str, 0, sizeof(int64str));
				snprintf(int64str, sizeof(int64str), "%lld", (long long)reflection->GetRepeatedInt64(msg, field, i));
				tmp_value = int64str;
			}
			break;
			case google::protobuf::FieldDescriptor::TYPE_FIXED64:
			case google::protobuf::FieldDescriptor::TYPE_UINT64:
			{
				memset(int64str, 0, sizeof(int64str));
				snprintf(int64str, sizeof(int64str), "%llu", (unsigned long long)reflection->GetRepeatedUInt64(msg, field, i));
				tmp_value = int64str;
			}
			break;
			case google::protobuf::FieldDescriptor::TYPE_STRING:
			case google::protobuf::FieldDescriptor::TYPE_BYTES:
				tmp_value = reflection->GetRepeatedString(msg, field, i);
				break;
			default:
				break;
			}
			value.append(tmp_value);
		}
	}
	void FormatToJson(Json::Value& value, const ::google::protobuf::Message& msg)
	{
		char int64str[25];
		const google::protobuf::Descriptor* descriptor = msg.GetDescriptor();
		const google::protobuf::Reflection* reflection = msg.GetReflection();

		const int count = descriptor->field_count();

		for (int i = 0; i < count; ++i)
		{
			const google::protobuf::FieldDescriptor* field = descriptor->field(i);

			if (field->is_repeated())
			{
				if (reflection->FieldSize(msg, field) > 0)
				{
					FormatRepeatedField(value[field->name()], msg, field, reflection);
				}
				continue;
			}

			if (!reflection->HasField(msg, field))
			{
				continue;
			}

			switch (field->type())
			{
			case google::protobuf::FieldDescriptor::TYPE_MESSAGE:
			{
				const google::protobuf::Message& tmp_msg = reflection->GetMessage(msg, field);
				if (0 != tmp_msg.ByteSize())
				{
					FormatToJson(value[field->name()], tmp_msg);
				}
			}
			break;
			case google::protobuf::FieldDescriptor::TYPE_INT32:
				value[field->name()] = reflection->GetInt32(msg, field);
				break;
			case google::protobuf::FieldDescriptor::TYPE_UINT32:
				value[field->name()] = reflection->GetUInt32(msg, field);
				break;
			case google::protobuf::FieldDescriptor::TYPE_INT64:
			{
				memset(int64str, 0, sizeof(int64str));
				snprintf(int64str, sizeof(int64str), "%lld", (long long)reflection->GetInt64(msg, field));
				value[field->name()] = int64str;
			}
			break;
			case google::protobuf::FieldDescriptor::TYPE_FIXED64:
			case google::protobuf::FieldDescriptor::TYPE_UINT64:
			{
				memset(int64str, 0, sizeof(int64str));
				snprintf(int64str, sizeof(int64str), "%llu", (unsigned long long)reflection->GetUInt64(msg, field));
				value[field->name()] = int64str;
			}
			break;
			case google::protobuf::FieldDescriptor::TYPE_STRING:
			case google::protobuf::FieldDescriptor::TYPE_BYTES:
			{
				value[field->name()] = reflection->GetString(msg, field);
			}
			break;
			default:
				break;
			}
		}
	}

	std::string JsonToString(Json::Value& value)
	{
		Json::FastWriter writer;
		return writer.write(value);
	}
}