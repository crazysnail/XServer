#include "data_table.h"
#include "data_value.h"
#include "utils.h"

//必须在最后include！-------------------
#include "../zxero/mem_debugger.h"
//------------------------------------

namespace Mt
{
		data_table::data_table()
		{
			m_rows = 0;
			m_cols = 0;
			m_dataMatrix = nullptr;
		}

		data_table::~data_table()
		{
			release();
		}

		void data_table::release()
		{

			m_attributes.clear();

			if( m_dataMatrix != nullptr )
			{
				for(int i = 0; i < m_rows; i++)
				{
					SAFE_DELETE_ARRAY(m_dataMatrix[i]);
				}

				m_rows = 0;
				m_cols = 0;

				SAFE_DELETE_ARRAY(m_dataMatrix);
			}

		}

		bool data_table::load(const char * filename, const char* mode, size_t *size, bool is_utf8 )
		{
			ZXERO_ASSERT( filename != nullptr && size != nullptr && mode != nullptr) << "Invalid parameters.";
			


			unsigned char* content = utils::get_file_data(filename, mode, size, is_utf8);
			ZXERO_ASSERT( content != nullptr ) << "get file data failed,tabel_name="<<filename;
			if( content == nullptr )
			{
				return false;
			}

			std::vector<std::vector<std::string>> table;
			std::vector<std::string> lines;
			std::vector<std::string> cells;
			utils::split_file_string((char *)content,"\n",lines);

			SAFE_DELETE_ARRAY(content);

			ZXERO_ASSERT( !(lines.empty()||lines.size() <= 2 )) << "invalid table!";
			if( lines.empty() || lines.size() <= 2 )
			{
				return false;
			}

			//m_cols = type_table.size();
			m_rows = lines.size()-3;
			std::vector<e_data_type> type_table;

			for(uint32 i=0; i<lines.size(); i++)
			{
				utils::trim_string(lines[i]);
				utils::split_string(lines[i].c_str(),",",cells);
				if( cells.empty() )
				{
					continue;
				}

				if(i == 0 )
				{//名字行
					m_cols = cells.size();
					int index  = 0;
					for( auto& child:cells )
					{
						m_attributes.insert(make_pair(child,index));
						index++;
					}
				}

				ZXERO_ASSERT( (uint32)m_cols == cells.size() ) << "the table cols count do not match! tbname="<<filename;
				if( (uint32)m_cols != cells.size() )
				{//列数检查
					return false;
				}


				if(i == 2 )
				{//类型行
					for( auto& child:cells )
					{
						if( child == "int" )
						{
							type_table.push_back(e_dt_int);
						}
						else if( child == "float" )
						{
							type_table.push_back(e_dt_float);
						}
						else if( child == "string" )
						{
							type_table.push_back(e_dt_string);
						}
						else if( child == "long" )
						{
							type_table.push_back(e_dt_long);
						}
						else
						{
							ZXERO_ASSERT( false ) << "invalid data type!";
							return false;
						}
					}
				}

				if( i >= 3 )
				{
					table.push_back(cells);
				}
				
				cells.clear();
			}

			//数据映射处理
			m_dataMatrix = new data_value*[m_rows];
			for( int32 i=0; i<m_rows; ++i )
			{
				m_dataMatrix[i] = new data_value[m_cols];
				for( int32 j=0; j<m_cols; ++j )
				{
					e_data_type type = type_table[j];
					data_value* pDataValue = m_dataMatrix[i]+j;
					std::string temp = table[i][j];
					pDataValue->setType(type);
					switch(type){
					case e_dt_int:
						{
							int32 value = atoi(temp.c_str());
							pDataValue->setInt(value);
							break;
						}
					case e_dt_long:
						{
							int64 value = utils::atoi64(temp.c_str());
							pDataValue->setLong(value);
							break;
						}
					case e_dt_float:
						{
							float value = (float)atof(temp.c_str());
							pDataValue->setFloat(value);
							break;
						}
					case e_dt_string:
						{
							int strLen = temp.length();
							if(strLen >= DATA_VALUE_MAX_STR_LEN){
								pDataValue->createAppendStr(strLen);
							}
							char* buf = pDataValue->getBuffer();
							strncpy(buf,temp.c_str(),strLen);
							buf[strLen] = '\0';
							break;
						}
					default:
						ZXERO_ASSERT( false ) << "invalid data type!";
						break;
					}
				}
			}

			return true;

		}

		int32  data_table::find_col( const char* attribute )
		{
			//const auto& iter = m_attributes.find(attribute);
			//ZXERO_ASSERT(iter != m_attributes.end()) << "DataTable::getValue: error attribute : "<<attribute;
			//return iter->second;

			if (m_attributes.find(attribute) != m_attributes.end())
			{
				return m_attributes[attribute];
			}

			return -1;
		}

		
		data_row*  data_table::find_row( int rowIndex )
		{			
			ZXERO_ASSERT(rowIndex < m_rows ) << "rowIndex is too big";
			static data_row row_find;
			row_find.m_row = m_dataMatrix[rowIndex];
			row_find.m_table = this;

			return &row_find;
		}

		const data_value* data_table::get_value(const char *attribute, int rowIndex) const
		{
			ZXERO_ASSERT(attribute) << "error attribute";
			ZXERO_ASSERT(rowIndex < m_rows ) << "rowIndex is too big";
   
			const auto& iter = m_attributes.find(attribute);
			ZXERO_ASSERT(iter != m_attributes.end()) << "DataTable::getValue: error attribute";
			int columnIndex = iter->second;
			return m_dataMatrix[rowIndex] + columnIndex;
  
		}


		const data_value* data_table::get_value(int colomnIndex, int rowIndex) const
		{
			ZXERO_ASSERT(rowIndex < m_rows ) << "rowIndex is too big";
			return m_dataMatrix[rowIndex] + colomnIndex;
		}

}