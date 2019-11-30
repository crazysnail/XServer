#ifndef _MT_WORD_MANAGER_H_
#define _MT_WORD_MANAGER_H_

#include "../mt_types.h"
#include "AllPacketEnum.pb.h"
#include "AllConfigEnum.pb.h"

namespace Mt
{
	class MtWordManager
	{
	public:
		static void lua_reg(lua_State* ls);
	public:
		static MtWordManager* Instance();
		MtWordManager();
		std::string RandomName(Packet::Gender gender);
		~MtWordManager();

		int32	OnLoad();
		int32	OnUnLoad();

		bool RegName(const std::string& name);
		
		/**
		* \brief 名字是否唯一
		* \param name
		* \retval true 唯一
		* \retval false 有重名
		*/
		bool UniqueName(const std::string& name);
		/**
		* \brief 名字有效, 不包含敏感词或者重复了
		* \param name
		* \retval true 有效
		*  \retval false 无效的名字
		*/
		bool InvalidName(const std::string& name);
		/**
		* \brief 铭感词检查
		* \param content
		* \retval true 不包含敏感词
		* \retval false 包含敏感词
		*/
		bool NoSensitiveWord(const std::string& content);	
		bool FilterWord(std::string &content);
	
	public:
		std::vector<std::string> m_sensitive_words;
		std::set<std::string> m_used_name;
		std::vector<std::string> prefix_;
		std::vector<std::string> man_;
		std::vector<std::string> women_;
		std::vector<std::string> suffix_;
	};
}

#endif
