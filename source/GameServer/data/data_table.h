
#ifndef _data_table_h_
#define _data_table_h_

#include "../mt_types.h"
#include <map>

namespace Mt
{
	using namespace zxero;
	typedef std::map<std::string, int32> name_map;

	struct data_row;
	class data_value;

	class data_table
	{
	public:
		data_table();
		virtual ~data_table();
		void release();
    
		int32  find_col( const char* attribute );
		data_row*  find_row( int rowIndex );
		const data_value* get_value(const char* attribute, int rowIndex) const;
		const data_value* get_value(int colomnIndex, int rowIndex) const;
		bool load(const char * filename, const char* mode, size_t *size, bool is_utf8 = true );
		int get_rows() const{ return m_rows; }
		int get_cols() const { return m_cols; }
	private:
		int			        m_rows;						// 多少行
		int					m_cols;						// 多少列
		name_map		    m_attributes;   // 存放字段名-->记录的映射
		data_value**        m_dataMatrix;   // 存放具体存放数组用
    
	};
}

#endif // _data_table_h_