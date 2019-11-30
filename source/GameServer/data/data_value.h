#ifndef ZXERO_GAMESERVER_DATA_VALUE_H__
#define ZXERO_GAMESERVER_DATA_VALUE_H__



#include "log.h"
#include "../mt_types.h"
#include "data_table.h"


namespace Mt
{
	const static int DATA_VALUE_MAX_STR_LEN = 8;

	enum e_data_type
	{
		e_dt_int = 0,
		e_dt_long,
		e_dt_float,
		e_dt_string,    
		e_dt_date,
	};	

	class data_value
	{
	public:
		data_value();
		~data_value();
		void setType(e_data_type type);    
		int getInt() const ;
		int64 getLong() const;
		float getFloat() const;
		int64 getDate() const;
		const char* getString() const;      
		char* getBuffer(); // 此接口其实是为了做buf的设置。为了减少一次内存拷贝，把buf取出来，把内容附着上去

		void setInt(int i);
		void setLong(int64 l);
		void setFloat(float f);
		void setDate(int64 l);
		void createAppendStr(int len);
		bool isString() const;
    
	private:
		union _data
		{
			int     i;
			int64   l;
			float   f;
			char    s[DATA_VALUE_MAX_STR_LEN];
		}m_data;
    
		char*	m_appendStr;
		int32	m_strLen;
		e_data_type m_type;
    
	};

	struct data_row
	{
		data_table *m_table;
		data_value* m_row;

		data_row()
		{
			m_table = nullptr;
			m_row = nullptr;
		}

		bool have_col(const char* col_name )
		{
			int columnIndex = m_table->find_col(col_name);
			if (columnIndex < 0)
			{
				return false;
			}
			return true;
		}

		int32 getInt( const char* col_name ) const 
		{
			int columnIndex = m_table->find_col(col_name);
			ZXERO_ASSERT(columnIndex>=0) << "error attribute"<< col_name;
			data_value* cell = m_row + columnIndex;
			return cell->getInt();
		}

		int64 getLong( const char* col_name ) const
		{
			int columnIndex = m_table->find_col(col_name);
			ZXERO_ASSERT(columnIndex >= 0) << "error attribute" << col_name;
			data_value* cell = m_row + columnIndex;
			return cell->getLong();
		}

		real32 getFloat( const char* col_name ) const
		{
			int columnIndex = m_table->find_col(col_name);
			ZXERO_ASSERT(columnIndex >= 0) << "error attribute" << col_name;
			data_value* cell = m_row + columnIndex;
			return cell->getFloat();
		}

		const char* getString( const char* col_name) const
		{
			int columnIndex = m_table->find_col(col_name);
			ZXERO_ASSERT(columnIndex >= 0) << "error attribute" << col_name;
			data_value* cell = m_row + columnIndex;
			return cell->getString();
		}
		bool isString(const char* col_name) const {
			ZXERO_ASSERT(col_name) << "error attribute";
			int columnIndex = m_table->find_col(col_name);
			data_value* cell = m_row + columnIndex;
			return cell->isString();
		}
	};
}

#endif // ZXERO_GAMESERVER_DATA_VALUE_H__