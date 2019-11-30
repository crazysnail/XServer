
/********************************************************************
	created:	2016/9/20
	filename: 	json_obj_base.h
	author:		jingang.li
	
	purpose:	json serializer

	Copyright (c) 2016 KooGame Inc.
*********************************************************************/

#ifndef json_obj_base_h
#define json_obj_base_h

#include "obj_reflector.h"
#include "log.h"

//
namespace zxero
{
#ifdef ZXERO_OS_WINDOWS
#pragma warning(disable:4267)
#endif

	typedef void* (*GenerateJsonObjForDeSerializeFromOutSide)(const std::string& propertyName);

	class json_obj_base  
	{  
	protected:  

	public:  
		json_obj_base(void){}  
	public:  
		virtual ~json_obj_base(void){		
		}  
		std::string serialize()  
		{  
			Json::Value new_item = do_serialize();         
			Json::FastWriter writer;    
			std::string out2 = writer.write(new_item);   
			return out2;  
		}  

		bool deserialize(const char* str)  
		{  
			Json::Reader reader;    
			Json::Value root;  
			if (reader.parse(str, root))  
			{    
				return do_deserialize(root);  
			}  
			return false;  
		}  

		Json::Value do_serialize()  
		{  
			Json::Value new_item;  
			const std::vector<property*> m_propertys = m_reflector.get_properties();

			for (std::size_t i=0; i < m_propertys.size(); ++i )
			{  
				void* pAddr = m_propertys[i]->m_addr;  
				switch(m_propertys[i]->m_type)
				{  
				case asNormalArray:
					new_item[m_propertys[i]->m_name] = do_normal_array_serialize(pAddr, m_propertys[i]->m_count, m_propertys[i]->m_child_node_type);
					break;
				case asVectorArray:
				case asListArray:
					new_item[m_propertys[i]->m_name] = do_array_serialize(pAddr, m_propertys[i]->m_type, m_propertys[i]->m_child_node_type);
					break;
				case asSpecialArray:  
					new_item[m_propertys[i]->m_name] = do_special_array_serialize(m_propertys[i]->m_name); 
					break;  
				case asJsonObj:  
					new_item[m_propertys[i]->m_name] = ((json_obj_base*)pAddr)->do_serialize();  
					break;  
				case asBool:  
					new_item[m_propertys[i]->m_name] = serialize<bool>(pAddr); 
				case asInt8: 
				case asInt16: 
				case asInt32:  
					new_item[m_propertys[i]->m_name] = serialize<int32>(pAddr); 
					break;  
				case asUInt8: 
				case asUInt16: 
				case asUInt32:  
					new_item[m_propertys[i]->m_name] = serialize<uint32>(pAddr);  
					break;  
				case asInt64:  
					new_item[m_propertys[i]->m_name] = serialize<int64>(pAddr);  
					break;  
				case asUInt64:  
					new_item[m_propertys[i]->m_name] = serialize<uint64>(pAddr);  
					break;  
				case asString:  
					new_item[m_propertys[i]->m_name] = serialize<std::string>(pAddr);  
					break;
				default:  
					//暂时只支持这几种类型，需要的可以自行添加
					ZXERO_ASSERT(false);
					break;  
				}         
			}  
			return new_item;  
		} 
	protected:

		virtual void desc_properties()=0;

		bool do_deserialize(Json::Value& root)  
		{  
			const std::vector<property*> m_propertys = m_reflector.get_properties();
			for (std::size_t i=0; i < m_propertys.size(); ++i )  
			{  
				void* pAddr = m_propertys[i]->m_addr;  

				switch(m_propertys[i]->m_type)  
				{  
				case asNormalArray:
					{
						if (root.isNull() || root[m_propertys[i]->m_name].isNull())
						{
							break;
						}
						do_normal_array_deserialize(m_propertys[i]->m_name, pAddr, root[m_propertys[i]->m_name], m_propertys[i]->m_type, m_propertys[i]->m_child_node_type);
					}
					break;
				case asListArray:
				case asVectorArray:
					{
						if (root.isNull() || root[m_propertys[i]->m_name].isNull())
						{
							break;
						}
						do_array_deserialize(m_propertys[i]->m_name, pAddr, root[m_propertys[i]->m_name], m_propertys[i]->m_type, m_propertys[i]->m_child_node_type);
					}
					break;
				case asJsonObj:  
					{  
						if (!root[ m_propertys[i]->m_name ].isNull())   
							((json_obj_base*)pAddr)->do_deserialize(root[m_propertys[i]->m_name]);  
					}                 
					break;  
				case asBool:  
					(*(bool*)pAddr) = root.get(m_propertys[i]->m_name, 0).asBool();  
					break;  
				case asInt8: 
				case asInt16: 
				case asInt32:  
					(*(int32*)pAddr) = root.get(m_propertys[i]->m_name, 0).asInt();  
					break;  
				case asUInt8: 
				case asUInt16: 
				case asUInt32:  
					(*(uint32*)pAddr) = root.get(m_propertys[i]->m_name, 0).asUInt();  
					break;  
				case asInt64:  
					(*(int64*)pAddr) = root.get(m_propertys[i]->m_name, 0).asInt64();  
					break;  
				case asUInt64:  
					(*(uint64*)pAddr) = root.get(m_propertys[i]->m_name, 0).asUInt64();  
					break;  
				case asString:  
					(*(std::string*)pAddr) = root.get(m_propertys[i]->m_name, "").asString();  
				default:  
					//只支持这几种类型，需要的可以自行添加   
					ZXERO_ASSERT(false);
					break;  
				}             
			}  
			return true;  
		}  

		//有特殊对象需要序列化时，请重载此函数 
		virtual Json::Value do_special_array_serialize(const std::string& propertyName){ return "";}

		//在反序列化时，如果对象中嵌套了列表，并且列表中容纳的内容是其他特殊对象时，需要重载此函数，new出真正的对象
		virtual json_obj_base* GenerateJsonObjForDeSerialize(const std::string& propertyName){ return NULL;}

		bool do_array_deserialize(const std::string& propertyName, void* addr, Json::Value& root, reflect_type listType, reflect_type paramType)
		{
			if (listType == asVectorArray)
			{
				switch(paramType)
				{ 
				case asJsonObj:  
					{  
						return do_obj_array_deserialize<vector<json_obj_base*>>(propertyName, addr, root);
					}                 
					break;  
				case asInt8: 
				case asInt16: 
				case asInt32:  
					return do_array_deserialize<vector<int32>,int32>(addr, root); 
				case asUInt8: 
				case asUInt16: 
				case asUInt32:  
					return do_array_deserialize<vector<uint32>,uint32>(addr, root);   
				case asInt64:  						
					return do_array_deserialize<vector<int64>,int64>(addr, root);     
				case asUInt64:  					
					return do_array_deserialize<vector<uint64>,uint64>(addr, root);     
				case asString:  					
					return do_array_deserialize<vector<std::string>,std::string>(addr, root);       
				default:
					ZXERO_ASSERT(false);
					break;  
				}
			}
			else if(listType == asListArray)
			{
				switch(paramType)
				{ 
				case asJsonObj:  
					{  
						return do_obj_array_deserialize<list<json_obj_base*>>(propertyName, addr, root);
					}                 
					break;  
				case asInt8: 
				case asInt16: 
				case asInt32:  
					return do_array_deserialize<list<int32>,int32>(addr, root); 
				case asUInt8: 
				case asUInt16: 
				case asUInt32:  
					return do_array_deserialize<list<uint32>,uint32>(addr, root);   
				case asInt64:  						
					return do_array_deserialize<list<int64>,int64>(addr, root);     
				case asUInt64:  					
					return do_array_deserialize<list<uint64>,uint64>(addr, root);     
				case asString:  					
					return do_array_deserialize<list<std::string>,std::string>(addr, root);       
				default:
					ZXERO_ASSERT(false);
					break;   
				}
			}

			return true;
		}
		//
		bool do_normal_array_deserialize(const std::string& propertyName, void* addr, Json::Value& root, int count, reflect_type type )
		{
			switch(type)
			{ 
			case asJsonObj:  
				{  
					return do_obj_normal_array_deserialize<json_obj_base*>(propertyName, addr, root);
				}                 
				break;  
			case asBool:  
				return do_normal_array_deserialize<bool>(addr, root); 
			case asInt8: 
			case asInt16: 
			case asInt32:  
				return do_normal_array_deserialize<int32>(addr, root); 
			case asUInt8: 
			case asUInt16: 
			case asUInt32:  
				return do_normal_array_deserialize<uint32>(addr, root);   
			case asInt64:  
				return do_normal_array_deserialize<int64>(addr, root);     
			case asUInt64:  
				return do_normal_array_deserialize<uint64>(addr, root);     
			case asString:  
				return do_normal_array_deserialize<std::string>(addr, root);       
			default:
				ZXERO_ASSERT(false);
				break;  
			}

			return true;
		}

		//特殊对象列表的反序列化
		template<typename T1>
		bool do_obj_array_deserialize(const std::string& propertyName, void* addr, Json::Value& node)
		{
			if (!node.isArray())
			{
				return false;
			}
			T1* pList = (T1*)addr;
			int size = node.size();
			for(int i = 0; i < size; ++i)
			{
				json_obj_base* pNode = GenerateJsonObjForDeSerialize(propertyName);
				pNode->do_deserialize(node[i]);
				pList->push_back(pNode);
			}
			return true;
		}
		//
		template<typename T1>
		bool do_obj_normal_array_deserialize(const std::string& propertyName, void* addr, Json::Value& node)

		{
			if (!node.isArray())
			{
				return false;
			}
			T1* pArray = (T1*)addr;
			int size = node.size();
			for(int i = 0; i < size; ++i)
			{
				json_obj_base* pNode = GenerateJsonObjForDeSerialize(propertyName);
				pNode->do_deserialize(node[i]);
				pArray[i] = pNode;
			}
			return true;
		}
	public:
		//常见类型列表的反序列化 
		template <typename T1, typename T2>
		static bool do_array_deserialize(void* addr, Json::Value& node)
		{
			if (!node.isArray())
			{
				return false;
			}
			T1* pList = (T1*)addr;
			int size = node.size();
			for(int i = 0; i < size; ++i)
				pList->push_back( deserialize<T2>(node[i]) );
			return true;
		}

		//特殊对象列表的反序列化
		template<typename T1>
		static bool do_obj_array_deserialize(const string& propertyName, void* addr, Json::Value& node, GenerateJsonObjForDeSerializeFromOutSide funGenerate)
		{
			if (!node.isArray())
			{
				return false;
			}
			T1* pList = (T1*)addr;
			int size = node.size();
			for(int i = 0; i < size; ++i)
			{
				json_obj_base* pNode = (json_obj_base*)funGenerate(propertyName);
				pNode->do_deserialize(node[i]);
				pList->push_back(pNode);
			}
			return true;
		}

		//
		//常见类型列表的反序列化 
		template <typename T>
		static bool do_normal_array_deserialize(void* addr, Json::Value& node)
		{
			if (!node.isArray())
			{
				return false;
			}
			T* pArray = (T*)addr;
			int size = node.size();
			for(int i = 0; i < size; ++i)
			{
				pArray[i]=deserialize<T>(node[i]);
			}
			return true;
		}
		//常见类型列表的反序列化 
		template <typename T, int N>
		static bool do_normal_array_deserialize(T(* addr)[N], Json::Value& node)
		{
			if (!node.isArray())
			{
				return false;
			}
			int size = node.size();
			if(size>N)
			{
				return false;
			}
			for(int i = 0; i < size; ++i)
			{
				(*addr)[i]=deserialize<T>(node[i]);
			}
			return true;
		}

		//常见类型列表的反序列化 
		template <typename T, int N>
		static bool do_normal_obj_array_deserialize(T(* addr)[N], Json::Value& node)
		{
			if (!node.isArray())
			{
				return false;
			}
			int size = node.size();
			if(size>N)
			{
				return false;
			}
			for(int i = 0; i < size; ++i)
			{
				(*addr)[i].do_deserialize(node[i]);
			}
			return true;
		}

		//常见类型列表的反序列化 
		template <typename T, int N>
		static bool do_normal_obj_point_array_deserialize(T(* addr)[N], Json::Value& node)
		{
			if (!node.isArray())
			{
				return false;
			}
			int size = node.size();
			if(size>N)
			{
				return false;
			}
			if( *addr == NULL )
			{
				return false;
			}

			for(int i = 0; i < size; ++i)
			{
				//(*addr)[i] = new T();//
				(*addr)[i]->do_deserialize(node[i]);
			}
			return true;
		}

		//特殊对象列表的反序列化
		template<typename T>
		static bool do_obj_normal_array_deserialize(const string& propertyName, void* addr, Json::Value& node, GenerateJsonObjForDeSerializeFromOutSide funGenerate)
		{
			if (!node.isArray())
			{
				return false;
			}
			T* pArray = (T*)addr;
			int size = node.size();
			for(int i = 0; i < size; ++i)
			{
				json_obj_base* pNode = (json_obj_base*)funGenerate(propertyName);
				pNode->do_deserialize(node[i]);
				pArray[i] = pNode;
			}
			return true;
		}

	protected:
		Json::Value do_array_serialize(void* addr, reflect_type listType, reflect_type paramType)
		{
			if (listType == asVectorArray)
			{
				switch(paramType)
				{ 
				case asBool:
					return do_array_serialize((std::vector<bool>*)addr);
				case asJsonObj:
					return do_array_serialize((std::vector<json_obj_base*>*)addr);
				case asInt8: 
				case asInt16: 
				case asInt32:
					return do_array_serialize((std::vector<int32>*)addr);
				case asUInt8: 
				case asUInt16: 
				case asUInt32:  
					return do_array_serialize((std::vector<uint32>*)addr);
				case asInt64:  
					return do_array_serialize((std::vector<int64>*)addr);
				case asUInt64:  
					return do_array_serialize((std::vector<uint64>*)addr);  
				case asString:  
					return do_array_serialize((std::vector<std::string>*)addr);
				default:
					ZXERO_ASSERT(false);
				}
			}
			else 
			{
				switch(paramType)
				{ 
				case asBool:  
					return do_array_serialize((std::list<bool>*)addr);
				case asJsonObj:
					return do_array_serialize((std::list<json_obj_base*>*)addr);
				case asInt8: 
				case asInt16: 
				case asInt32:  
					return do_array_serialize((std::list<int32>*)addr);
				case asUInt8: 
				case asUInt16: 
				case asUInt32:  
					return do_array_serialize((std::list<uint32>*)addr);
				case asInt64:  
					return do_array_serialize((std::list<int64>*)addr);
				case asUInt64:  
					return do_array_serialize((std::list<uint64>*)addr);  
				case asString:  
					return do_array_serialize((std::list<std::string>*)addr);
				default:
					ZXERO_ASSERT(false);
				}
			}
			return "";
		}
		//
		Json::Value do_normal_array_serialize(void* addr, int count, reflect_type type)
		{
			switch(type)
			{ 
			case asBool:  
				return do_normal_array_serialize( (bool*)addr, count);
			case asInt8: 
			case asInt16: 
			case asInt32:  
				return do_normal_array_serialize( (int32*)addr, count);
			case asUInt8: 
			case asUInt16: 
			case asUInt32:  
				return do_normal_array_serialize( (uint32*)addr, count); 
			case asInt64:  
				return do_normal_array_serialize( (int64*)addr, count);   
			case asUInt64:  
				return do_normal_array_serialize( (uint64*)addr, count);  
			case asString:  
				return do_normal_array_serialize( (std::string*)addr, count);
			case asJsonObj:
				return do_normal_obj_array_serialize( (json_obj_base*)addr, count);
			default:
				ZXERO_ASSERT(false);
			}
			return "";
		}
	public:

		///////////////////////////////////////////////////////容器
		template <typename T>
		static Json::Value do_array_serialize(T* pList)
		{
			Json::Value arrayValue;

			for (auto it = pList->begin(); it != pList->end(); ++ it)
			{
				arrayValue.append(*it);			
			}

			return arrayValue;
		}		

		//vector<bool>有坑，需要特殊处理
		template <>
		static Json::Value do_array_serialize(std::vector<bool>* pList)
		{
			Json::Value arrayValue;

			for (auto it = pList->begin(); it != pList->end(); ++ it)
			{
				arrayValue.append((bool)(*it));			
			}

			return arrayValue;
		}

		template <>
		static Json::Value do_array_serialize(std::vector<json_obj_base*>* pList)
		{
			Json::Value arrayValue;
			for (auto it = pList->begin(); it != pList->end(); ++ it)
			{
				arrayValue.append((*it)->do_serialize());			
			}
			return arrayValue;
		}

		template <>
		static Json::Value do_array_serialize(std::list<json_obj_base*>* pList)
		{
			Json::Value arrayValue;
			for (auto it = pList->begin(); it != pList->end(); ++ it)
			{
				arrayValue.append((*it)->do_serialize());			
			}
			return arrayValue;
		}

		///////////////////////////////////////////////////////常规数组
		template <typename T,int N>
		static Json::Value do_normal_array_serialize(T (* pArray)[N] )
		{
			return do_normal_array_serialize(*pArray,N);
		}

		template <typename T>
		static Json::Value do_normal_array_serialize(T * pArray,int count )
		{
			Json::Value arrayValue;

			for (int i =0; i<count; i++)
			{
				arrayValue.append(pArray[i]);			
			}

			return arrayValue;
		}

		//template <>
		//static Json::Value DoNormalArraySerialize(json_obj_base* pArray, int count)
		//{
		//	Json::Value arrayValue;
		//	for (int i =0; i<count; i++)
		//	{
		//		arrayValue.append(pArray[i].do_serialize());		
		//	}

		//	return arrayValue;
		//}

		//obj array
		template <typename T, int N>
		static Json::Value do_normal_obj_point_array_serialize( T(* pArray)[N] )
		{
			Json::Value arrayValue;
			for (int i =0; i<N; i++)
			{
				json_obj_base * p = (json_obj_base *)(*pArray)[i];
				if(p==nullptr)
					return "";

				arrayValue.append(p->do_serialize());			
			}

			return arrayValue;
		}

		template <typename T>
		static Json::Value do_normal_obj_point_array_serialize( T* pArray, int count )
		{
			Json::Value arrayValue;
			for (int i =0; i<count; i++)
			{
				json_obj_base * p = (json_obj_base *)pArray[i];
				if(p==nullptr)
					return "";

				arrayValue.append(p->do_serialize());			
			}

			return arrayValue;
		}


		template <typename T, int N>
		static Json::Value do_normal_obj_array_serialize( T (* pArray)[N] )
		{
			Json::Value arrayValue;
			for (int i =0; i<N; i++)
			{
				T & p = (*pArray)[i];
				arrayValue.append(p.do_serialize());	
			}

			return arrayValue;

		}

		template <typename T>
		static Json::Value do_normal_obj_array_serialize( T * pArray, int count )
		{
			Json::Value arrayValue;
			for (int i =0; i<count; i++)
			{
				T & p = pArray[i];
				arrayValue.append(p.do_serialize());			
			}

			return arrayValue;
		}

		///////////////
		static std::string JsonValueToString(Json::Value& tvalue)
		{
			Json::FastWriter writer;    
			return writer.write(tvalue);
		}
	private:
		template <typename T>
		Json::Value serialize(void* addr)
		{
			return (*(T*)addr);
		}

		template <typename T>
		static T deserialize(Json::Value& root)
		{
			return GetData<T>(root);
		}

		template <typename T>
		static T GetData(Json::Value& root)
		{
			T temp;
			return temp;
		}

		template <>
		static bool GetData(Json::Value& root)
		{
			return root.asBool();
		}

		template <>
		static int32 GetData(Json::Value& root)
		{
			return root.asInt();
		}

		template <>
		static uint32 GetData(Json::Value& root)
		{
			return root.asUInt();
		}
		template <>
		static int64 GetData(Json::Value& root)
		{
			return root.asInt64();
		}
		template <>
		static uint64 GetData(Json::Value& root)
		{
			return root.asUInt64();
		}
		template <>
		static std::string GetData(Json::Value& root)
		{
			return root.asString();
		}


	protected:  

		obj_reflector m_reflector;

	}; 

}
#endif