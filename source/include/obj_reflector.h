
/********************************************************************
	created:	2016/9/20
	filename: 	obj_reflector.h
	author:		jingang.li
	
	purpose:	simple reflector

	Copyright (c) 2016 KooGame Inc.
*********************************************************************/

#ifndef obj_reflect_h
#define obj_reflect_h

#include "types.h"
#include <string>  
#include <vector>  
#include <list>
#include "json/json.h"  
using std::string;  
using std::vector;  
using std::list;

//
namespace zxero
{

	enum reflect_type  
	{
		asInvalid = -1,
		asBool,  
		asInt8,
		asUInt8,
		asInt16,
		asUInt16,
		asInt32,  
		asUInt32,  
		asString,  
		asInt64,  
		asUInt64,  
		asJsonObj,
		asNormalArray,
		asSpecialArray,
		asVectorArray,
		asListArray,
	};  

	struct property
	{
		void * m_addr;
		int m_count;
		std::string m_name;
		reflect_type m_child_node_type;
		reflect_type m_type;
	};

	class json_obj_base;
	class obj_reflector
	{
	public: 
		obj_reflector(){} 
		virtual ~obj_reflector(void)
		{
			for (auto child: m_properties)
			{
				delete child;
			}
			m_properties.clear();
		}  
		//
		template< typename T>
		void desc_property(const std::string& name,T * p, int count=1 )
		{
			if( obj_reflector::TypeOfT<T>() == asVectorArray )
			{
				desc_property_internal(p, name, obj_reflector::TypeOfT<T>(), count, obj_reflector::TypeOfTChild<T>());
			}
			else if( obj_reflector::TypeOfT<T>() == asListArray )
			{
				desc_property_internal(p, name, obj_reflector::TypeOfT<T>(), count, obj_reflector::TypeOfTChild<T>());
			}
			else
			{
				desc_property_internal(p, name, obj_reflector::TypeOfT<T>(), count);
			}		
		}
		template< typename T, int N >
		void desc_property(const std::string& name,T (* p)[N])
		{
			desc_property(name, *p, N); 
		}
		//
		const std::vector<property*> &get_properties()
		{
			return m_properties;
		}

	public:
		/////////////////////////////////////////////////////////////////////////////////
		template< typename T >
		reflect_type static TypeOfT(){	return asInvalid;}

		template< typename T >
		reflect_type static TypeOfT( T ){return TypeOfT<T>();}

		template<> static reflect_type TypeOfT<bool>(){	return asBool;}
		template<> static reflect_type TypeOfT<int8>() { return asInt8; }
		template<> static reflect_type TypeOfT<uint8>() { return asInt8; }
		template<> static reflect_type TypeOfT<int16>() { return asInt16; }
		template<> static reflect_type TypeOfT<uint16>() { return asInt16; }
		template<> static reflect_type TypeOfT<int32>(){	return asInt32;}
		template<> static reflect_type TypeOfT<uint32>(){	return asUInt32;}
		template<> static reflect_type TypeOfT<std::string>(){	return asString;}
		template<> static reflect_type TypeOfT<int64>(){	return asInt64;}
		template<> static reflect_type TypeOfT<uint64>(){	return asUInt64;}
		template<> static reflect_type TypeOfT<json_obj_base>(){	return asJsonObj;}


		//template<> CEnumJsonTypeMap TypeOfT<int>(){	return asNormalArray;}
		//template<> CEnumJsonTypeMap TypeOfT<int>(){	return asSpecialArray;}
		//
		template<> static reflect_type TypeOfT<std::vector<bool>>(){ return asVectorArray;}
		template<> static reflect_type TypeOfT<std::vector<int8>>(){	return asVectorArray;}
		template<> static reflect_type TypeOfT<std::vector<uint8>>(){	return asVectorArray;}
		template<> static reflect_type TypeOfT<std::vector<int16>>(){	return asVectorArray;}
		template<> static reflect_type TypeOfT<std::vector<uint16>>(){	return asVectorArray;}
		template<> static reflect_type TypeOfT<std::vector<int32>>(){ return asVectorArray;}
		template<> static reflect_type TypeOfT<std::vector<uint32>>(){ return asVectorArray;}
		template<> static reflect_type TypeOfT<std::vector<std::string>>(){ return asVectorArray;}
		template<> static reflect_type TypeOfT<std::vector<int64>>(){ return asVectorArray;}
		template<> static reflect_type TypeOfT<std::vector<uint64>>(){ return asVectorArray;}
		template<> static reflect_type TypeOfT<std::vector<json_obj_base>>(){ return asVectorArray;}
		template<> static reflect_type TypeOfT<std::vector<json_obj_base*>>(){ return asVectorArray;}
		//		   static 
		template<> static reflect_type TypeOfT<std::list<bool>>(){ return asListArray;}
		template<> static reflect_type TypeOfT<std::list<int8>>(){	return asListArray;}
		template<> static reflect_type TypeOfT<std::list<uint8>>(){	return asListArray;}
		template<> static reflect_type TypeOfT<std::list<int16>>(){	return asListArray;}
		template<> static reflect_type TypeOfT<std::list<uint16>>(){	return asListArray;}
		template<> static reflect_type TypeOfT<std::list<int32>>(){ return asListArray;}
		template<> static reflect_type TypeOfT<std::list<uint32>>(){ return asListArray;}
		template<> static reflect_type TypeOfT<std::list<std::string>>(){ return asListArray;}
		template<> static reflect_type TypeOfT<std::list<int64>>(){ return asListArray;}
		template<> static reflect_type TypeOfT<std::list<uint64>>(){ return asListArray;}
		//template<> static reflect_type TypeOfT<std::list<json_obj_base>>(){ return asListArray;}//暂不支持
		template<> static reflect_type TypeOfT<std::list<json_obj_base*>>(){ return asListArray;}

		template< typename T >
		reflect_type static TypeOfTChild(){return asInvalid; }
		template<> static reflect_type TypeOfTChild<std::vector<bool>>(){ return asBool;}
		template<> static reflect_type TypeOfTChild<std::vector<int8>>(){	return asInt8;}
		template<> static reflect_type TypeOfTChild<std::vector<uint8>>(){	return asUInt8;}
		template<> static reflect_type TypeOfTChild<std::vector<int16>>(){	return asInt16;}
		template<> static reflect_type TypeOfTChild<std::vector<uint16>>(){return asUInt16;}
		template<> static reflect_type TypeOfTChild<std::vector<int32>>(){ return asInt32;}
		template<> static reflect_type TypeOfTChild<std::vector<uint32>>(){ return asUInt32;}
		template<> static reflect_type TypeOfTChild<std::vector<std::string>>(){ return asString;}
		template<> static reflect_type TypeOfTChild<std::vector<int64>>(){ return asInt64;}
		template<> static reflect_type TypeOfTChild<std::vector<uint64>>(){ return asUInt64;}
		template<> static reflect_type TypeOfTChild<std::vector<json_obj_base>>(){ return asJsonObj;}
		template<> static reflect_type TypeOfTChild<std::vector<json_obj_base*>>(){ return asJsonObj;}

		template<> static reflect_type TypeOfTChild<std::list<bool>>(){ return asBool;}
		template<> static reflect_type TypeOfTChild<std::list<int8>>(){	return asInt8;}
		template<> static reflect_type TypeOfTChild<std::list<uint8>>(){	return asUInt8;}
		template<> static reflect_type TypeOfTChild<std::list<int16>>(){	return asInt16;}
		template<> static reflect_type TypeOfTChild<std::list<uint16>>(){return asUInt16;}
		template<> static reflect_type TypeOfTChild<std::list<int32>>(){ return asInt32;}
		template<> static reflect_type TypeOfTChild<std::list<uint32>>(){ return asUInt32;}
		template<> static reflect_type TypeOfTChild<std::list<std::string>>(){ return asString;}
		template<> static reflect_type TypeOfTChild<std::list<int64>>(){ return asInt64;}
		template<> static reflect_type TypeOfTChild<std::list<uint64>>(){ return asUInt64;}
		//template<> static reflect_type TypeOfTChild<std::list<json_obj_base>>(){ return asJsonObj;}//暂不支持
		template<> static reflect_type TypeOfTChild<std::list<json_obj_base*>>(){ return asJsonObj;}

	protected:
		//////////////////////////////////
		void desc_property_internal(void* addr, std::string name, reflect_type type, int count=1,  reflect_type child_node_type = asInvalid)  
		{  
			if (count == 1)
			{
				auto db_prop = new property();
				db_prop->m_addr = addr;
				db_prop->m_count = count;
				db_prop->m_name = name;
				db_prop->m_type = type;
				db_prop->m_child_node_type = child_node_type;

				m_properties.push_back(db_prop);
			}
			else
			{
				auto db_prop = new property();
				db_prop->m_addr = addr;
				db_prop->m_count = count;
				db_prop->m_name = name;
				db_prop->m_type = asNormalArray;
				db_prop->m_child_node_type = type;

				m_properties.push_back(db_prop);

				////	自动展开，方便后续遍历处理
				//for (UINT i = 0; i < count; ++i)
				//{
				//	char buf[128];
				//	sprintf_s(buf, sizeof(buf), "%s%u", name.c_str(), i);

				//	auto db_prop = new desc_property();
				//	db_prop->m_addr = addr;
				//	db_prop->m_count = count;
				//	db_prop->m_name = buf;
				//	db_prop->m_type = type;
				//	db_prop->m_child_node_type = child_node_type;

				//	m_propertys.push_back(db_prop);
				//}
			}
		}  

	private:
		std::vector<property*> m_properties;
	};
}
#endif