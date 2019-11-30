//	数据库对象辅助器，用于保存，更新，加载，删除对象
#ifndef _db_helper_h_
#define _db_helper_h_

#include "types.h"
#include "log.h"
#include "dbwork.h"
#include "dbcontext.h"
#include <vector>
#include <boost/shared_ptr.hpp>
#include "cppconn/prepared_statement.h"
#include "cppconn/resultset.h"
#include "cppconn/exception.h"


namespace zxero
{
	class dbcontext;
	typedef boost::scoped_ptr<sql::ResultSet> result_set_ptr;
	//	设置语句属性和从结果读取属性
	typedef void (*db_setter)(sql::PreparedStatement* ps, uint32 idx, void* data);
	typedef void (*db_getter)(sql::ResultSet* rs, uint32 idx, void* data);

	//	单条属性记录
	struct db_property
	{
		//	属性偏移值
		uint32 offset;
		//	属性数量
		uint32 count;
		//	属性访问器
		db_setter setter;
		db_getter getter;
		//	属性名称
		std::string name;
	};

	//	属性集合
	//typedef boost::ptr_vector<db_property> db_properties;
	typedef std::vector<db_property*> db_properties;

	//	默认属性访问器
	template< typename T >
	void db_setter_function(sql::PreparedStatement* , uint32 , void* )
	{
		ZXERO_ASSERT(false) << "unsupported setter type " << typeid(T).name();
	}

	template< typename T >
	void db_getter_function(sql::ResultSet* , uint32 , void* )
	{
		ZXERO_ASSERT(false) << "unsupported getter type " << typeid(T).name();
	}

	//	对象属性偏移值获取器
	template< typename C, typename T >
	uint32 object_offset(T C::* p)
	{
		return (uint32)((char*)&(((C*)16)->*p) - (char*)16);
	}

	template< typename C, typename T, int N >
	uint32 object_offset(T(C::* p)[N])
	{
		return (uint32)((char*)&(((C*)16)->*p) - (char*)16);
	}

	//	语句构建器
	class prepared_statement_builder : boost::noncopyable
	{
	public:
		prepared_statement_builder(const std::string& table_name);
		virtual ~prepared_statement_builder();


	public:
		//	注意，通常需要第一个属性是uint64的主键
		void add_column_internal(uint32 offset, uint32 size, uint32 count, const std::string& name, db_setter setter, db_getter getter);

	public:
		template< typename C, typename T >
		void add_column(T C::* p, const std::string& name)
		{
			add_column_internal(object_offset(p), sizeof(T), 1, name, db_setter_function<T>, db_getter_function<T>);
		}

		template< typename C, typename T, int N >
		void add_column(T(C::* p)[N], const std::string& name)
		{
			add_column_internal(object_offset(p), sizeof(T), N, name, db_setter_function<T>, db_getter_function<T>);
		}

	public:
		//	构建更新语句
		std::string build_update_statement();

		//	构建加载语句
		std::string build_load_statement(const std::string& key);

		//	构建删除语句
		std::string build_delete_statement(const std::string& key);

		//  构建存储过程语句
		std::string build_call_proc_save_statement();

	protected:
		//	表名称
		std::string table_name_;
		//	要操作的属性列表
		db_properties properties_;
	};

	//	对象操作器
	template < typename T >
	class dbhelper : public prepared_statement_builder
	{
	public:
		typedef T*	obj_ptr;
		typedef std::vector< obj_ptr >	obj_ptrs;

	public:
		dbhelper(const std::string& dbname,const std::string& table_name)
			:prepared_statement_builder(table_name)
			, dbname_(dbname)
			, load_index_(-1)
			, update_index_(-1)
			, del_index_(-1)
			, proc_save_index_(-1)
		{

		}

		virtual ~dbhelper()
		{
			delete m_p[0];
			delete m_p[1];
			delete m_p[2];
			delete m_p[3];
		}

	public:
		//	加载数据
		void load(boost::shared_ptr<dbcontext>& dbctx, uint64 key_id, obj_ptrs& objs )
		{
			try
			{
				//	获取加载语句
				auto stat = dbctx->get_prepared_statement(load_index_);

				//	设置查询key
				stat->setUInt64(1, key_id);

				//	执行查询
				//boost::shared_ptr<sql::ResultSet> rs(stat->executeQuery());

				result_set_ptr rs(stat->executeQuery());

				//	遍历所有查询结果，生成对象，并填充对应属性, 并将结果填充在objs中
				while (rs->next())
				{
					uint32 idx = 1;
					uint64 uid = rs->getUInt64(1);

					if( uid == INVALID_GUID )
						continue;

					T* obj = new T();

					for (auto& property : properties_)
					{
						property->getter(rs.get(), idx, (char*)obj + property->offset);
						++idx;
					}

					objs.push_back(obj_ptr(obj));
				}

				//return objs.size();
			}
			catch (sql::SQLException& e)
			{
				LOG_ERROR<<"dbctx load error"<<e.what();
				throw e;
			}
		}



		//	更新数据，主键必须为有效数据
		void update(boost::shared_ptr<dbcontext>& dbctx, T* obj)
		{
			try
			{
				//	获取加载语句
				auto stat = dbctx->get_prepared_statement(update_index_);

				//	填充各值
				uint32 idx = 1;
				for (auto& property : properties_)
				{
					property->setter(stat, idx, (char*)obj + property->offset);
					++idx;
				}

				//	执行查询
				stat->executeUpdate();
			}
			catch (sql::SQLException& e)
			{
				LOG_ERROR<<"dbctx update error"<<e.what();
				throw e;
			}
		}

		//	删除数据
		void del(boost::shared_ptr<dbcontext>& dbctx, uint64 id)
		{
			try
			{
				//	获取删除语句
				auto stat = dbctx->get_prepared_statement(del_index_);

				//	填充key
				stat->setUInt64(1, id);

				//	执行查询
				stat->executeUpdate();
			}
			catch (sql::SQLException& e)
			{
				LOG_ERROR<<"dbctx del error"<<e.what();
				throw e;
			}			
		}

		//	save存储过程调用
		void call_save_proc(boost::shared_ptr<dbcontext>& dbctx, T* obj, uint64 guid, int32 indexkey )
		{
			try
			{				
				if( indexkey == -1 )
				{
					throw 57;
				}
				//	获取语句
				auto stat = dbctx->get_prepared_statement(proc_save_index_);

				//	填充各值
				uint32 idx = 1;
				for (auto& property : properties_)
				{
					property->setter(stat, idx, (char*)obj + property->offset);
					++idx;
				}

				//	填充位置值
				stat->setInt(2,indexkey);
				stat->setUInt64(3,guid);

				//	执行查询

				result_set_ptr result(stat->executeQuery());
				if (!result)
				{
					LOG_ERROR<<"call_save_proc error--guid:"<<guid<<"indexkey:"<<indexkey;
					throw 57;
				}				
			}
			catch (sql::SQLException& e)
			{
				LOG_ERROR<<"call_save_proc error"<<e.what()<<"--guid:"<<guid<<"indexkey:"<<indexkey;
				throw e;
			}
		}

	public:

		//	添加完列后，要执行这个语句来构建各个操作语句
		void build_all(const std::string& load_key, const std::string& del_key)
		{
			//	注意，这就是摆明了要泄露的数据
			m_p[0] = new db_prepared_statement_register(dbname_,load_index_, build_load_statement(load_key).c_str());
			m_p[1] = new db_prepared_statement_register(dbname_,update_index_, build_update_statement().c_str());
			m_p[2] = new db_prepared_statement_register(dbname_,del_index_, build_delete_statement(del_key).c_str());
			m_p[3] = new db_prepared_statement_register(dbname_,proc_save_index_, build_call_proc_save_statement().c_str());
		}

	protected:
		std::string  dbname_;
		//
		int32	load_index_;
		int32	update_index_;
		int32	del_index_;
		int32	proc_save_index_;

		db_prepared_statement_register * m_p[4];
	};

	template<> void db_setter_function<int8>(sql::PreparedStatement* ps, uint32 idx, void* ptr);
	template<> void db_setter_function<int16>(sql::PreparedStatement* ps, uint32 idx, void* ptr);
	template<> void db_setter_function<int32>(sql::PreparedStatement* ps, uint32 idx, void* ptr);
	template<> void db_setter_function<int64>(sql::PreparedStatement* ps, uint32 idx, void* ptr);
	template<> void db_setter_function<uint8>(sql::PreparedStatement* ps, uint32 idx, void* ptr);
	template<> void db_setter_function<uint16>(sql::PreparedStatement* ps, uint32 idx, void* ptr);
	template<> void db_setter_function<uint32>(sql::PreparedStatement* ps, uint32 idx, void* ptr);
	template<> void db_setter_function<uint64>(sql::PreparedStatement* ps, uint32 idx, void* ptr);
	template<> void db_setter_function<std::string>(sql::PreparedStatement* ps, uint32 idx, void* ptr);
	template<> void db_setter_function<std::string>(sql::PreparedStatement* ps, uint32 idx, void* ptr);

	template<> void db_getter_function<int8>(sql::ResultSet* rs, uint32 idx, void* ptr);
	template<> void db_getter_function<int16>(sql::ResultSet* rs, uint32 idx, void* ptr);
	template<> void db_getter_function<int32>(sql::ResultSet* rs, uint32 idx, void* ptr);
	template<> void db_getter_function<int64>(sql::ResultSet* rs, uint32 idx, void* ptr);
	template<> void db_getter_function<uint8>(sql::ResultSet* rs, uint32 idx, void* ptr);
	template<> void db_getter_function<uint16>(sql::ResultSet* rs, uint32 idx, void* ptr);
	template<> void db_getter_function<uint32>(sql::ResultSet* rs, uint32 idx, void* ptr);
	template<> void db_getter_function<uint64>(sql::ResultSet* rs, uint32 idx, void* ptr);
	template<> void db_getter_function<std::string>(sql::ResultSet* rs, uint32 idx, void* ptr);
	template<> void db_getter_function<std::string>(sql::ResultSet* rs, uint32 idx, void* ptr);

}	//	namespace zxero

#endif	//	#define _db_helper_h_

