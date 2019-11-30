
#include "utils.h"
#include "log.h"
#include "date_time.h"
#include <boost/algorithm/string.hpp>
#include "md5.h"
#include <regex>

namespace zxero
{
		bool utils::check_date( int32 begin, int32 end )
		{
			if( begin < 0 || end < 0 )
				return false;
						

			timestamp_t time = now();
			date_t now_date = time.date();

			int32 begin_year_ = begin/10000;
			int32 begin_month_ = begin%10000/100;
			int32 begin_day_ = begin%100;

			int32 end_year_ = end/10000;
			int32 end_month_ = end%10000/100;
			int32 end_day_ = end%100;

			date_t begin_date((uint16)begin_year_, (uint16)begin_month_, (uint16)begin_day_);
			date_t end_date((uint16)end_year_, (uint16)end_month_, (uint16)end_day_);

			//date_t begin_date = from_undelimited_string()
			if( now_date >= begin_date && now_date <= end_date)
			{
				return true;
			}
			else 
			{
				return false;
			}
		}

		bool utils::check_date_next_days( int32 source_date, int32 n )
		{
			int32 end_year_ = source_date/10000;
			int32 end_month_ = source_date%10000/100;
			int32 end_day_ = source_date%100;	
			date_t end_date((uint16)end_year_, (uint16)end_month_, (uint16)end_day_);

			date_duration leap_day(n);
			date end_date_next_day = end_date + leap_day ;
			const auto& now_date = now().date();
			if( now_date.year() == end_date_next_day.year() 
				&& now_date.month() == end_date_next_day.month()
				&& now_date.day() == end_date_next_day.day())
			{
				return true;
			}
			else
			{
				return false;
			}
		}

		int32 utils::gen_date_day_number()
		{
			const auto& now_date = now().date();

			int32 year_ = now_date.year();
			int32 month_ = now_date.month(); 
			int32 day_ = now_date.day(); 

			int32 number = year_*10000 + month_*100 + day_;
			return number;
		}

		int32 utils::convert_data_day_number( uint64 time )
		{
			date_t shutdown_date = seconds_transform_date(time);
			int32 year_ = shutdown_date.year();
			int32 month_ = shutdown_date.month(); 
			int32 day_ = shutdown_date.day(); 

			int32 number = year_*10000 + month_*100 + day_;
			return number;
		}
		
		bool utils::name_check( std::string name )
		{
			//长度检查
			if(name.length() < 6 || name.length() > 24 )
			{
				return false;
			}

			//敏感词检测
			//todo
			return true;
		}

		void utils::split_file_string(const char * src_string, const char * key, std::vector<std::string> &sub_lst )
		{
			ZXERO_ASSERT(src_string != nullptr);
			if( src_string == nullptr )
				return;

			int32 len = strlen(src_string);
			char *buffer = new char[len+1];
			char *p = nullptr;
			strncpy(buffer,src_string,len);
			buffer[len] = '\0';
			char *ptr = zstrtok(buffer, key,&p);  
			while(ptr != nullptr){  
				sub_lst.push_back(std::string(ptr));
				ptr = zstrtok(nullptr, key,&p);			
			}  

			delete []buffer;
		}

		std::string utils::bytestohexstring(char* bytes, int bytelength)
		{
			std::string str("");
			std::string str2("0123456789ABCDEF");
			for (int i = 0; i < bytelength; i++) {
				int b;
				b = 0x0f & (bytes[i] >> 4);
				//char s1 = str2.at(b);
				str.append(1, str2.at(b));
				b = 0x0f & bytes[i];
				str.append(1, str2.at(b));
				//char s2 = str2.at(b);
			}
			return str;
		}

		void utils::split_string(const char * src_string, const char * key, std::vector<std::string> &sub_lst )
		{
			ZXERO_ASSERT(src_string != nullptr);
			if( src_string == nullptr )
				return;
			std::string str(src_string);
			/*
			ZXERO_ASSERT(strlen(src_string)<MAX_LINE_SIZE);
			char buffer[MAX_LINE_SIZE] = {0};
			strncpy(buffer,src_string,strlen(src_string));
			char *p = nullptr;
			char *ptr = zstrtok(buffer, key,&p);  
			while(ptr != nullptr){  
				sub_lst.push_back(std::string(ptr));
				ptr = zstrtok(nullptr, key,&p);			
			}  */
			boost::split(sub_lst, str, boost::is_any_of(key));
		}
		void utils::split_string(const char * src_string, const char * key, std::vector<int32> &sub_lst )
		{
			ZXERO_ASSERT(src_string != nullptr);
			if( src_string == nullptr )
				return;

			ZXERO_ASSERT(strlen(src_string)<MAX_LINE_SIZE);

			char buffer[MAX_LINE_SIZE] = {0};
			strncpy(buffer,src_string,strlen(src_string));
			char *p = nullptr;
			char *ptr = zstrtok(buffer, key,&p);  
			while(ptr != nullptr){  
				sub_lst.push_back(atoi(ptr));
				ptr = zstrtok(nullptr, key, &p);			
			}  
		}


		unsigned char* utils::get_file_data(const char * filename, const char* mode, size_t *size,bool is_utf8 )
		{
			unsigned char * buffer = nullptr;
			ZXERO_ASSERT( filename != nullptr && size != nullptr && mode != nullptr) << "Invalid parameters.";
			*size = 0;
			do
			{
				FILE *fp = fopen( filename, mode );
				ZXERO_ASSERT( fp != nullptr) << "open file failed! file name:"<<filename;
				if( fp == nullptr )
					break;

				if( is_utf8 )
				{
					unsigned char flag[3] = {0};
					fread( flag,sizeof(unsigned char), 3, fp );
					if( (flag[0] == 0xEF && flag[1] == 0xBB && flag[2] == 0xBF) )// 有bom  
					{  
						fseek(fp,0,SEEK_END);
						*size = ftell(fp);
						fseek(fp,3,SEEK_SET);
					} 
					else
					{
						fseek(fp, 0, SEEK_END);
						*size = ftell(fp);
						fseek(fp, 0, SEEK_SET);
						LOG_INFO << filename << " utf8 without bom file!";
					}
				}
				else
				{
					fseek(fp,0,SEEK_END);
					*size = ftell(fp);
					fseek(fp,0,SEEK_SET);
				}

				buffer = new unsigned char[*size+1];
				*size = fread(buffer,sizeof(unsigned char), *size, fp);
				buffer[*size] = '\0';

				fclose(fp);

			} while (0);

		
			ZXERO_ASSERT( buffer != nullptr ) << "open file failed!";

			return buffer;
		}

	#ifdef ZXERO_OS_WINDOWS

		int32 utils::enum_dir_files(std::string src_path, std::vector<std::string>& file_list)
		{
			int32 dwStatus = 0;

			WIN32_FIND_DATA findFileData;
			std::string filePath = src_path;
			std::string srcNewPath = src_path;
			filePath += "/*.*";
			HANDLE hFind = FindFirstFile(filePath.c_str(), &findFileData);
			if (hFind != INVALID_HANDLE_VALUE)
			{
				do 
				{
					srcNewPath = src_path + "/" + findFileData.cFileName;				
					if (strcmp(findFileData.cFileName, ".") == 0 || strcmp(findFileData.cFileName, "..") == 0 || strcmp(findFileData.cFileName, ".svn") == 0)
					{
						continue;
					}
					else if (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
					{
						continue;
					}
					else
					{
						file_list.push_back(findFileData.cFileName);
					}
					if (dwStatus != 0)
					{
						break;
					}
				} while (FindNextFile(hFind, &findFileData));
				FindClose(hFind);
			}
			return dwStatus;
		}

	#endif

	#ifdef ZXERO_OS_LINUX
		bool utils::is_dir(const char *pszName)
		{
			struct stat S_stat;

			//取得文件状态
			if (lstat(pszName, &S_stat) < 0)
			{
				return false;
			}

			if (S_ISDIR(S_stat.st_mode))
			{
				return true;
			}
			else
			{
				return false;
			}
		}

		int32 utils::enum_dir_files(std::string src_path, std::vector<std::string>& file_list)
		{
			int32 ret = 0;
			DIR *dir_ptr;
			struct dirent *dirent_ptr;
			dir_ptr = opendir(src_path.c_str());

			if (dir_ptr == nullptr)
			{
				LOG_ERROR << "[on_load]enum_dir_files:: invalid path, the src_path= " << src_path.c_str();
				return -1;
			}
			while ((dirent_ptr = readdir(dir_ptr)) != nullptr)
			{
				if (strcmp(dirent_ptr->d_name, ".") == 0 || strcmp(dirent_ptr->d_name, "..") == 0 || strcmp(dirent_ptr->d_name, ".svn") == 0)
				{
					continue;
				}
				char temp_path[1024] = {0};
				zprintf(temp_path, sizeof(temp_path), "%s/%s", src_path.c_str(), dirent_ptr->d_name);

				//LOG_INFO << "[on_load]enum_dir_files:: find config file=" << temp_path;

				if (utils::is_dir(temp_path))
				{
					continue;
				}
				else
				{/* do something */					
					file_list.push_back(dirent_ptr->d_name);
				}
			}

			closedir(dir_ptr);

			return ret;
		}
	#endif

		/*   删除左边的空格   */
		char * utils::l_trim(char * szOutput, const char *szInput)
		{
			assert(szInput != NULL);
			assert(szOutput != NULL);
			assert(szOutput != szInput);
			for   (NULL; *szInput != '\0' && isspace(*szInput); ++szInput){
				;
			}
			return strcpy(szOutput, szInput);
		}

		/*   删除右边的空格   */
		char * utils::r_trim(char *szOutput, const char *szInput)
		{
			char *p = NULL;
			assert(szInput != NULL);
			assert(szOutput != NULL);
			assert(szOutput != szInput);
			strcpy(szOutput, szInput);
			for(p = szOutput + strlen(szOutput) - 1; p >= szOutput && isspace(*p); --p){
				;
			}
			*(++p) = '\0';
			return szOutput;
		}

		/*   删除两边的空格   */
		char * utils::a_trim(char * szOutput, const char * szInput)
		{
			char *p = NULL;
			assert(szInput != NULL);
			assert(szOutput != NULL);
			l_trim(szOutput, szInput);
			for   (p = szOutput + strlen(szOutput) - 1;p >= szOutput && isspace(*p); --p){
				;
			}
			*(++p) = '\0';
			return szOutput;
		}

		void utils::regexstring(const std::string sourcestr,std::string reg_esp, std::vector<std::string>& out_match)
		{
			try
			{
				std::regex rgx(reg_esp);
				//std::cmatch match;
				auto words_begin =
					std::sregex_iterator(sourcestr.begin(), sourcestr.end(), rgx);
				auto words_end = std::sregex_iterator();
				for (std::sregex_iterator i = words_begin; i != words_end; ++i)
				{
					std::smatch match = *i;
					std::string match_str = match.str();
					out_match.push_back(match_str);
				}
			}
			catch (const std::regex_error& e)
			{
				LOG_WARN << "regex match error:" << e.what() << " code:" << e.code();
			}
			
		}
		uint64 utils::guid_string_to_uint64(std::string guid_str)
		{
			std::stringstream strValue;
			strValue << guid_str.c_str();
			uint64  _guid = INVALID_GUID;
			strValue >> _guid;
			return _guid;
		}
		//从INI文件读取字符串类型数据
		char * utils::get_ini_key_string(const char *title,const char *key,const char *filename) 
		{ 
			FILE *fp; 
			char szLine[1024];
			static char tmpstr[1024];
			int rtnval;
			int i = 0; 
			int flag = 0; 
			char *tmp;

			if((fp = fopen(filename, "r")) == NULL) 
			{ 
				printf("have   no   such   file %s",filename);
				return nullptr; 
			}
			while(!feof(fp)) 
			{ 
				rtnval = fgetc(fp); 
				if(rtnval == EOF) 
				{ 
					break; 
				} 
				else
				{ 
					szLine[i++] = (char)rtnval; 
				} 
				if(rtnval == '\n') 
				{ 
//#ifndef WIN32
//					i--;
//#endif  
					szLine[--i] = '\0';
					i = 0; 
					tmp = strchr(szLine, '='); 

					if(( tmp != NULL )&&(flag == 1)) 
					{ 
						if(strstr(szLine,key)!=NULL) 
						{ 
							//注释行
							if ('#' == szLine[0])
							{
							}
							else if ( '/' == szLine[0] && '/' == szLine[1] )
							{

							}
							else
							{
								//找打key对应变量
								strcpy(tmpstr,tmp+1); 
								fclose(fp);
								return tmpstr; 
							}
						} 
					}
					else
					{ 
						strcpy(tmpstr,"["); 
						strcat(tmpstr,title); 
						strcat(tmpstr,"]");
						if( strncmp(tmpstr,szLine,strlen(tmpstr)) == 0 ) 
						{
							//找到title
							flag = 1; 
						}
					}
				}
			}
			fclose(fp);
			ZXERO_ASSERT(false)<<"the system param "<<key<<" is null!";
			return nullptr; 
		}

		//从INI文件读取整类型数据
		int utils::get_ini_key_int(const char *title,const char *key, const char *filename)
		{
			return atoi(utils::get_ini_key_string(title,key,filename));
		}

		//从INI文件读取长整类型数据
		long utils::get_ini_key_long(const char *title,const char *key, const char *filename)
		{
			return atol(utils::get_ini_key_string(title,key,filename));
		}

		void utils::Q_strncpyz(char *dest, const char *src, int destsize)
		{
			if (!src) {
				//AssertEx(src,"null pointer in utils::Q_strncpyz");
				return;
			}
			if (destsize < 1) {
				//AssertEx(destsize>0,"dest buffer is null in utils::Q_strncpyz");
				return;
			}
			//strncpy_s(dest, sizeof(dest) / dest[0], src, destsize);
			strncpy(dest, src, destsize);
			dest[destsize - 1] = 0;
		}

		std::string utils::md5(const std::string src)
		{
			return MD5(src).toStr();
		}

		profiler::profiler(std::string log)
		{
			m_time = now_millisecond();
			m_log = log;
		}

		profiler::~profiler()
		{
			m_time = now_millisecond() - m_time;
			LOG_INFO<<m_log.c_str()<<" profiler elapse time="<<m_time;
		}


		profiler2addr::profiler2addr(int32* addr) :addr_(addr)
		{
			time_ = now_millisecond();
		}

		profiler2addr::~profiler2addr()
		{
			if (addr_ != nullptr) {
				*addr_ = int32(now_millisecond() - time_);
			}
		}

}