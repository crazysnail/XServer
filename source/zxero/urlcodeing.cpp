#include "urlcodeing.h"
//这是个类strCoding (strCoding.cpp文件)
namespace zxero
{
	namespace network
	{
		strCoding::strCoding(void)
		{
		}

		strCoding::~strCoding(void)
		{
		}
		void strCoding::Gb2312ToUnicode(wchar_t* pOut, const char *gbBuffer)
		{
			//::MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, gbBuffer, 2, pOut, 1);
			mbstowcs(pOut, gbBuffer,4);
			return;
		}
		void strCoding::UTF_8ToUnicode(wchar_t* pOut,const char *pText)
		{
			char* uchar = (char *)pOut;

			uchar[1] = ((pText[0] & 0x0F) << 4) + ((pText[1] >> 2) & 0x0F);
			uchar[0] = ((pText[1] & 0x03) << 6) + (pText[2] & 0x3F);

			return;
		}

		void strCoding::UnicodeToUTF_8(char* pOut, wchar_t* pText)
		{
			// 注意 wchar_t高低字的顺序,低字节在前，高字节在后
			char* pchar = (char *)pText;

			pOut[0] = (0xE0 | ((pchar[1] & 0xF0) >> 4));
			pOut[1] = (0x80 | ((pchar[1] & 0x0F) << 2)) + ((pchar[0] & 0xC0) >> 6);
			pOut[2] = (0x80 | (pchar[0] & 0x3F));

			return;
		}
		void strCoding::UnicodeToGB2312(char* pOut, wchar_t uData)
		{
			//WideCharToMultiByte(CP_ACP, NULL, &uData, 1, pOut, sizeof(wchar_t), NULL, NULL);
			wcstombs(pOut, &uData, 4);
			return;
		}

		//做为解Url使用
		char strCoding::CharToInt(char ch) {
			if (ch >= '0' && ch <= '9')return (char)(ch - '0');
			if (ch >= 'a' && ch <= 'f')return (char)(ch - 'a' + 10);
			if (ch >= 'A' && ch <= 'F')return (char)(ch - 'A' + 10);
			return -1;
		}
		char strCoding::StrToBin(char *str) {
			char tempWord[2];
			char chn;

			tempWord[0] = CharToInt(str[0]);                         //make the B to 11 -- 00001011
			tempWord[1] = CharToInt(str[1]);                         //make the 0 to 0 -- 00000000

			chn = (tempWord[0] << 4) | tempWord[1];                //to change the BO to 10110000

			return chn;
		}


		//UTF_8 转gb2312
		void strCoding::UTF_8ToGB2312(std::string &pOut,const char *pText, int pLen)
		{
			char buf[4];
			char* rst = new char[pLen + (pLen >> 2) + 2];
			memset(buf, 0, 4);
			memset(rst, 0, pLen + (pLen >> 2) + 2);

			int i = 0;
			int j = 0;

			while (i < pLen)
			{
				if (*(pText + i) >= 0)
				{

					rst[j++] = pText[i++];
				}
				else
				{
					wchar_t Wtemp;


					UTF_8ToUnicode(&Wtemp, pText + i);

					UnicodeToGB2312(buf, Wtemp);

					unsigned short int tmp = 0;
					tmp = rst[j] = buf[0];
					tmp = rst[j + 1] = buf[1];
					tmp = rst[j + 2] = buf[2];

					//newBuf[j] = Ctemp[0];
					//newBuf[j + 1] = Ctemp[1];

					i += 3;
					j += 2;
				}

			}
			rst[j] = '\0';
			pOut = rst;
			delete[]rst;
		}

		//GB2312 转为 UTF-8
		void strCoding::GB2312ToUTF_8(std::string& pOut, const char *pText, int pLen)
		{
			char buf[4];
			memset(buf, 0, 4);

			pOut.clear();

			int i = 0;
			while (i < pLen)
			{
				//如果是英文直接复制就可以
				if (pText[i] >= 0)
				{
					char asciistr[2] = { 0 };
					asciistr[0] = (pText[i++]);
					pOut.append(asciistr);
				}
				else
				{
					wchar_t pbuffer;
					Gb2312ToUnicode(&pbuffer, pText + i);

					UnicodeToUTF_8(buf, &pbuffer);

					pOut.append(buf);

					i += 2;
				}
			}

			return;
		}
		//把str编码为网页中的 GB2312 url encode ,英文不变，汉字双字节 如%3D%AE%88
		std::string strCoding::UrlGB2312(char * str)
		{
			std::string dd;
			size_t len = strlen(str);
			for (size_t i = 0; i < len; i++)
			{
				if (isalnum((uint8)str[i]))
				{
					char tempbuff[2];
					sprintf(tempbuff, "%c", str[i]);
					dd.append(tempbuff);
				}
				else if (isspace((uint8)str[i]))
				{
					dd.append("+");
				}
				else
				{
					char tempbuff[4];
					sprintf(tempbuff, "%%%X%X", ((uint8*)str)[i] >> 4, ((uint8*)str)[i] % 16);
					dd.append(tempbuff);
				}

			}
			return dd;
		}

		//把str编码为网页中的 UTF-8 url encode ,英文不变，汉字三字节 如%3D%AE%88

		std::string strCoding::UrlUTF8(char * str)
		{
			std::string tt;
			std::string dd;
			GB2312ToUTF_8(tt, str, (int)strlen(str));

			size_t len = tt.length();
			for (size_t i = 0; i < len; i++)
			{
				if (isalnum((uint8)tt.at(i)))
				{
					char tempbuff[2] = { 0 };
					sprintf(tempbuff, "%c", (uint8)tt.at(i));
					dd.append(tempbuff);
				}
				else if (isspace((uint8)tt.at(i)))
				{
					dd.append("+");
				}
				else
				{
					char tempbuff[4];
					sprintf(tempbuff, "%%%X%X", ((uint8)tt.at(i)) >> 4, ((uint8)tt.at(i)) % 16);
					dd.append(tempbuff);
				}

			}
			return dd;
		}
		//把url GB2312解码
		std::string strCoding::UrlUTF8Decode(std::string str)
		{
			std::string output = "";
			char tmp[2];
			int i = 0, len = str.length();//idx = 0, ndx, 

			while (i < len) {
				if (str[i] == '%') {
					tmp[0] = str[i + 1];
					tmp[1] = str[i + 2];
					output += StrToBin(tmp);
					i = i + 3;
				}
				else if (str[i] == '+') {
					output += ' ';
					i++;
				}
				else {
					output += str[i];
					i++;
				}
			}

			return output;
		}
		//把url utf8解码
		std::string strCoding::UrlUTF8DecodeToGB2312(std::string str)
		{
			std::string output = "";

			std::string temp = UrlUTF8Decode(str);//

			UTF_8ToGB2312(output, (char *)temp.data(), strlen(temp.data()));

			return output;
		}
	}
}