#include "mt_types.h"

void SetMessageDefaultValue(google::protobuf::Message& msg)
{
	auto reflection = msg.GetReflection();
	auto descriptor = msg.GetDescriptor();
	for (auto i = 0; i < descriptor->field_count(); ++i) {
		auto field_descriptor = descriptor->field(i);
		if (field_descriptor->is_repeated() || field_descriptor->is_optional())
			continue;
		switch (field_descriptor->cpp_type())
		{
		case google::protobuf::FieldDescriptor::CppType::CPPTYPE_BOOL:
			reflection->SetBool(&msg, field_descriptor, false);
			break;
		case google::protobuf::FieldDescriptor::CppType::CPPTYPE_ENUM:
			reflection->SetEnum(&msg, field_descriptor, field_descriptor->enum_type()->value(0));
			break;
		case google::protobuf::FieldDescriptor::CppType::CPPTYPE_INT32:
			reflection->SetInt32(&msg, field_descriptor, 0);
			break;
		case google::protobuf::FieldDescriptor::CppType::CPPTYPE_INT64:
			reflection->SetInt64(&msg, field_descriptor, 0);
			break;
		case google::protobuf::FieldDescriptor::CppType::CPPTYPE_UINT32:
			reflection->SetUInt32(&msg, field_descriptor, 0);
			break;
		case google::protobuf::FieldDescriptor::CppType::CPPTYPE_UINT64:
			reflection->SetUInt64(&msg, field_descriptor, 0);
			break;
		case google::protobuf::FieldDescriptor::CppType::CPPTYPE_DOUBLE:
			reflection->SetDouble(&msg, field_descriptor, 0);
			break;
		case google::protobuf::FieldDescriptor::CppType::CPPTYPE_FLOAT:
			reflection->SetFloat(&msg, field_descriptor, 0);
			break;
		case google::protobuf::FieldDescriptor::CppType::CPPTYPE_STRING:
			reflection->SetString(&msg, field_descriptor, "");
			break;
		case google::protobuf::FieldDescriptor::CppType::CPPTYPE_MESSAGE:
			SetMessageDefaultValue(*(reflection->MutableMessage(&msg, field_descriptor)));
			break;
		default:
			ZXERO_ASSERT(field_descriptor->cpp_type() == google::protobuf::FieldDescriptor::CppType::MAX_CPPTYPE);
			break;
		}
	}
}

void MessageMulti(google::protobuf::Message& lhs, zxero::real32 factor)
{
  auto reflection = lhs.GetReflection();
  auto descriptor = lhs.GetDescriptor();
  for (auto i = 0; i < descriptor->field_count(); ++i) {
    auto field_descriptor = descriptor->field(i);
    switch (field_descriptor->cpp_type())
    {
    case google::protobuf::FieldDescriptor::CppType::CPPTYPE_INT32:
    {
      if (reflection->HasField(lhs, field_descriptor)) {
        auto old_value = reflection->GetInt32(lhs, field_descriptor);
        auto new_value = (zxero::int32)(old_value * factor);
        reflection->SetInt32(&lhs, field_descriptor, new_value);
      }
    }
    break;
    case google::protobuf::FieldDescriptor::CppType::CPPTYPE_INT64:
    {
      if (reflection->HasField(lhs, field_descriptor)) {
        auto old_value = reflection->GetInt64(lhs, field_descriptor);
        auto new_value = (zxero::int64)(old_value * factor);
        reflection->SetInt64(&lhs, field_descriptor, new_value);
      }
    }
    break;
    case google::protobuf::FieldDescriptor::CppType::CPPTYPE_UINT32:
    {
      if (reflection->HasField(lhs, field_descriptor)) {
        auto old_value = reflection->GetUInt32(lhs, field_descriptor);
        auto new_value = (zxero::uint32)(old_value * factor);
        reflection->SetUInt32(&lhs, field_descriptor, new_value);
      }
    }
    break;
    case google::protobuf::FieldDescriptor::CppType::CPPTYPE_UINT64:
    {
      if (reflection->HasField(lhs, field_descriptor)) {
        auto old_value = reflection->GetUInt64(lhs, field_descriptor);
        auto new_value = (zxero::uint64)(old_value * factor);
        reflection->SetUInt64(&lhs, field_descriptor, new_value);
      }
    }
    break;
    case google::protobuf::FieldDescriptor::CppType::CPPTYPE_DOUBLE:
    {
      if (reflection->HasField(lhs, field_descriptor)) {
        auto old_value = reflection->GetDouble(lhs, field_descriptor);
        auto new_value = (zxero::real64)(old_value * factor);
        reflection->SetDouble(&lhs, field_descriptor, new_value);
      }
    }
    break;
    case google::protobuf::FieldDescriptor::CppType::CPPTYPE_FLOAT:
    {
      if (reflection->HasField(lhs, field_descriptor)) {
        auto old_value = reflection->GetFloat(lhs, field_descriptor);
        auto new_value = (zxero::real32)(old_value * factor);
        reflection->SetFloat(&lhs, field_descriptor, new_value);
      }
    }
    break;
    case google::protobuf::FieldDescriptor::CppType::CPPTYPE_MESSAGE:
    {
      if (reflection->HasField(lhs, field_descriptor)) {
        MessageMulti(*(reflection->MutableMessage(&lhs, field_descriptor)), factor);
      }
      break;
    }
    default:
      ZXERO_ASSERT(false) << "unsupport field type" << field_descriptor->cpp_type_name();
      break;
    }
  }
}

void MessageSub(google::protobuf::Message& lhs, const google::protobuf::Message& rhs)
{
  ZXERO_ASSERT(lhs.GetTypeName() == rhs.GetTypeName());
  auto reflection = lhs.GetReflection();
  auto descriptor = lhs.GetDescriptor();
  for (auto i = 0; i < descriptor->field_count(); ++i) {
    auto field_descriptor = descriptor->field(i);
    switch (field_descriptor->cpp_type())
    {
    case google::protobuf::FieldDescriptor::CppType::CPPTYPE_INT32:
    {
      if (reflection->HasField(lhs, field_descriptor)) {
        auto old_value = reflection->GetInt32(lhs, field_descriptor);
        auto new_value = old_value - reflection->GetInt32(rhs, field_descriptor);
        reflection->SetInt32(&lhs, field_descriptor, new_value);
      }
    }
    break;
    case google::protobuf::FieldDescriptor::CppType::CPPTYPE_INT64:
    {
      if (reflection->HasField(lhs, field_descriptor)) {
        auto old_value = reflection->GetInt64(lhs, field_descriptor);
        auto new_value = old_value - reflection->GetInt64(rhs, field_descriptor);
        reflection->SetInt64(&lhs, field_descriptor, new_value);
      }
    }
    break;
    case google::protobuf::FieldDescriptor::CppType::CPPTYPE_UINT32:
    {
      if (reflection->HasField(lhs, field_descriptor)) {
        auto old_value = reflection->GetUInt32(lhs, field_descriptor);
        auto new_value = old_value - reflection->GetUInt32(rhs, field_descriptor);
        reflection->SetUInt32(&lhs, field_descriptor, new_value);
      }
    }
    break;
    case google::protobuf::FieldDescriptor::CppType::CPPTYPE_UINT64:
    {
      if (reflection->HasField(lhs, field_descriptor)) {
        auto old_value = reflection->GetUInt64(lhs, field_descriptor);
        auto new_value = old_value - reflection->GetUInt64(rhs, field_descriptor);
        reflection->SetUInt64(&lhs, field_descriptor, new_value);
      }
    }
    break;
    case google::protobuf::FieldDescriptor::CppType::CPPTYPE_DOUBLE:
    {
      if (reflection->HasField(lhs, field_descriptor)) {
        auto old_value = reflection->GetDouble(lhs, field_descriptor);
        auto new_value = old_value - reflection->GetDouble(rhs, field_descriptor);
        reflection->SetDouble(&lhs, field_descriptor, new_value);
      }
    }
    break;
    case google::protobuf::FieldDescriptor::CppType::CPPTYPE_FLOAT:
    {
      if (reflection->HasField(lhs, field_descriptor)) {
        auto old_value = reflection->GetFloat(lhs, field_descriptor);
        auto new_value = old_value - reflection->GetFloat(rhs, field_descriptor);
        reflection->SetFloat(&lhs, field_descriptor, new_value);
      }
    }
    break;
    case google::protobuf::FieldDescriptor::CppType::CPPTYPE_MESSAGE:
    {
      if (reflection->HasField(lhs, field_descriptor)) {
        MessageSub(*(reflection->MutableMessage(&lhs, field_descriptor)), reflection->GetMessage(rhs, field_descriptor));
      }
      break;
    }
    default:
      ZXERO_ASSERT(false) << "unsupport field type" << field_descriptor->cpp_type_name();
      break;
    }
  }
}

void MessageAdd(google::protobuf::Message& lhs, const google::protobuf::Message& rhs)
{
  ZXERO_ASSERT(lhs.GetTypeName() == rhs.GetTypeName());
  auto reflection = lhs.GetReflection();
  auto descriptor = lhs.GetDescriptor();
  for (auto i = 0; i < descriptor->field_count(); ++i) {
    auto field_descriptor = descriptor->field(i);
    switch (field_descriptor->cpp_type())
    {
    case google::protobuf::FieldDescriptor::CppType::CPPTYPE_INT32:
    {
      if (reflection->HasField(lhs, field_descriptor)) {
        auto old_value = reflection->GetInt32(lhs, field_descriptor);
        auto new_value = old_value + reflection->GetInt32(rhs, field_descriptor);
        reflection->SetInt32(&lhs, field_descriptor, new_value);
      }
    }
    break;
    case google::protobuf::FieldDescriptor::CppType::CPPTYPE_INT64:
    {
      if (reflection->HasField(lhs, field_descriptor)) {
        auto old_value = reflection->GetInt64(lhs, field_descriptor);
        auto new_value = old_value + reflection->GetInt64(rhs, field_descriptor);
        reflection->SetInt64(&lhs, field_descriptor, new_value);
      }
    }
    break;
    case google::protobuf::FieldDescriptor::CppType::CPPTYPE_UINT32:
    {
      if (reflection->HasField(lhs, field_descriptor)) {
        auto old_value = reflection->GetUInt32(lhs, field_descriptor);
        auto new_value = old_value + reflection->GetUInt32(rhs, field_descriptor);
        reflection->SetUInt32(&lhs, field_descriptor, new_value);
      }
    }
    break;
    case google::protobuf::FieldDescriptor::CppType::CPPTYPE_UINT64:
    {
      if (reflection->HasField(lhs, field_descriptor)) {
        auto old_value = reflection->GetUInt64(lhs, field_descriptor);
        auto new_value = old_value + reflection->GetUInt64(rhs, field_descriptor);
        reflection->SetUInt64(&lhs, field_descriptor, new_value);
      }
    }
    break;
    case google::protobuf::FieldDescriptor::CppType::CPPTYPE_DOUBLE:
    {
      if (reflection->HasField(lhs, field_descriptor)) {
        auto old_value = reflection->GetDouble(lhs, field_descriptor);
        auto new_value = old_value + reflection->GetDouble(rhs, field_descriptor);
        reflection->SetDouble(&lhs, field_descriptor, new_value);
      }
    }
    break;
    case google::protobuf::FieldDescriptor::CppType::CPPTYPE_FLOAT:
    {
      if (reflection->HasField(lhs, field_descriptor)) {
        auto old_value = reflection->GetFloat(lhs, field_descriptor);
        auto new_value = old_value + reflection->GetFloat(rhs, field_descriptor);
        reflection->SetFloat(&lhs, field_descriptor, new_value);
      }
    }
    break;
    case google::protobuf::FieldDescriptor::CppType::CPPTYPE_MESSAGE:
    {
      if (reflection->HasField(lhs, field_descriptor)) {
        MessageAdd(*(reflection->MutableMessage(&lhs, field_descriptor)), reflection->GetMessage(rhs, field_descriptor));
      }
      break;
    }
    default:
      ZXERO_ASSERT(false) << "unsupport field type" << field_descriptor->cpp_type_name();
      break;
    }
  }
}

void MessageAdd(google::protobuf::Message& lhs, zxero::real32 factor)
{
  auto reflection = lhs.GetReflection();
  auto descriptor = lhs.GetDescriptor();
  for (auto i = 0; i < descriptor->field_count(); ++i) {
    auto field_descriptor = descriptor->field(i);
    switch (field_descriptor->cpp_type())
    {
    case google::protobuf::FieldDescriptor::CppType::CPPTYPE_INT32:
    {
      if (reflection->HasField(lhs, field_descriptor)) {
        auto old_value = reflection->GetInt32(lhs, field_descriptor);
        auto new_value = (zxero::int32)(old_value + factor);
        reflection->SetInt32(&lhs, field_descriptor, new_value);
      }
    }
    break;
    case google::protobuf::FieldDescriptor::CppType::CPPTYPE_INT64:
    {
      if (reflection->HasField(lhs, field_descriptor)) {
        auto old_value = reflection->GetInt64(lhs, field_descriptor);
        auto new_value = (zxero::int64)(old_value + factor);
        reflection->SetInt64(&lhs, field_descriptor, new_value);
      }
    }
    break;
    case google::protobuf::FieldDescriptor::CppType::CPPTYPE_UINT32:
    {
      if (reflection->HasField(lhs, field_descriptor)) {
        auto old_value = reflection->GetUInt32(lhs, field_descriptor);
        auto new_value = (zxero::uint32)(old_value + factor);
        reflection->SetUInt32(&lhs, field_descriptor, new_value);
      }
    }
    break;
    case google::protobuf::FieldDescriptor::CppType::CPPTYPE_UINT64:
    {
      if (reflection->HasField(lhs, field_descriptor)) {
        auto old_value = reflection->GetUInt64(lhs, field_descriptor);
        auto new_value = (zxero::uint64)(old_value + factor);
        reflection->SetUInt64(&lhs, field_descriptor, new_value);
      }
    }
    break;
    case google::protobuf::FieldDescriptor::CppType::CPPTYPE_DOUBLE:
    {
      if (reflection->HasField(lhs, field_descriptor)) {
        auto old_value = reflection->GetDouble(lhs, field_descriptor);
        auto new_value = (zxero::real64)(old_value + factor);
        reflection->SetDouble(&lhs, field_descriptor, new_value);
      }
    }
    break;
    case google::protobuf::FieldDescriptor::CppType::CPPTYPE_FLOAT:
    {
      if (reflection->HasField(lhs, field_descriptor)) {
        auto old_value = reflection->GetFloat(lhs, field_descriptor);
        auto new_value = (zxero::real32)(old_value + factor);
        reflection->SetFloat(&lhs, field_descriptor, new_value);
      }
    }
    break;
    case google::protobuf::FieldDescriptor::CppType::CPPTYPE_MESSAGE:
    {
      if (reflection->HasField(lhs, field_descriptor)) {
        MessageAdd(*(reflection->MutableMessage(&lhs, field_descriptor)), factor);
      }
      break;
    }
    default:
      ZXERO_ASSERT(false) << "unsupport field type" << field_descriptor->cpp_type_name();
      break;
    }
  }
}

