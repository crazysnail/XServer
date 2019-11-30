#ifndef url_codeing_h
#define url_codeing_h

#include "types.h"
namespace zxero
{
	namespace network
	{
		class strCoding
		{
		public:
			strCoding(void);
			~strCoding(void);

			static void UTF_8ToGB2312(std::string &pOut, const char *pText, int pLen);//utf_8תΪgb2312
			static void GB2312ToUTF_8(std::string& pOut, const char *pText, int pLen); //gb2312 תutf_8
			static std::string UrlGB2312(char * str);                           //urlgb2312����
			static std::string UrlUTF8(char * str);                             //urlutf8 ����
			static std::string UrlUTF8Decode(std::string str);                  //urlutf8����
			static std::string UrlUTF8DecodeToGB2312(std::string str);          //urlgb2312����

		private:
			static void Gb2312ToUnicode(wchar_t* pOut,const char *gbBuffer);
			static void UTF_8ToUnicode(wchar_t* pOut,const char *pText);
			static void UnicodeToUTF_8(char* pOut, wchar_t* pText);
			static void UnicodeToGB2312(char* pOut, wchar_t uData);
			static char CharToInt(char ch);
			static char StrToBin(char *str);

		};
	}
}
#endif