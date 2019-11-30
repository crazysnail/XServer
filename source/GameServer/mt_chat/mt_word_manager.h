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
		* \brief �����Ƿ�Ψһ
		* \param name
		* \retval true Ψһ
		* \retval false ������
		*/
		bool UniqueName(const std::string& name);
		/**
		* \brief ������Ч, ���������дʻ����ظ���
		* \param name
		* \retval true ��Ч
		*  \retval false ��Ч������
		*/
		bool InvalidName(const std::string& name);
		/**
		* \brief ���дʼ��
		* \param content
		* \retval true ���������д�
		* \retval false �������д�
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
