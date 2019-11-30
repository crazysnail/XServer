#ifndef ZXERO_GAMESERVER_MT_URL_WORK_H__
#define ZXERO_GAMESERVER_MT_URL_WORK_H__

#include "../mt_types.h"
#include "LoginPacket.pb.h"
#include "G2SMessage.pb.h"
#include "urlwork.h"

namespace Mt
{
	using namespace zxero;
	class ActiveServerWork : public urlwork
	{
		DECLARE_URLWORK();
	public:
		ActiveServerWork();

		virtual void done();
	private:
		std::string result_;
	};

	class serverstatus_require_work : public urlwork
	{
		DECLARE_URLWORK();
	public:
		serverstatus_require_work(std::string sql);

		virtual void done();
	private:
		std::string sql_;
	};

	class login_log_require_work : public urlwork
	{
		DECLARE_URLWORK();
	public:
		login_log_require_work(const std::string& log, const log_message::source_file& file, const int32 line);

		virtual void done();
	private:

		std::string log_;
		log_message::source_file file_;
		int32 line_;
	};

	class db_log_require_work : public urlwork
	{
		DECLARE_URLWORK();
	public:
		db_log_require_work(const std::string& log, const log_message::source_file& file, const int32 line);

		virtual void done();
	private:

		std::string log_;
		log_message::source_file file_;
		int32 line_;
	};

	class CheckCDKCODEWork : public urlwork
	{
		DECLARE_URLWORK();
	public:
		CheckCDKCODEWork(uint64 playerid, int32 partnerid,std::string code);

		virtual void done();
	private:
		uint64 playerid_;
		int32 partnerid_;
		std::string code_;
		std::map<int32, int32> item_map;
		int32 ret_;
	};

	class AccountLoginWork : public urlwork
	{
		DECLARE_URLWORK();
	public:
		AccountLoginWork(const boost::shared_ptr<client_session>& session,
			const boost::shared_ptr<Packet::LoginRequest>& message);
		virtual void done() override;
	private:
		const boost::shared_ptr<client_session> session_;
		const boost::shared_ptr<Packet::LoginRequest> message_;
		bool urlret = false;
		
		boost::shared_ptr<Packet::AccountModel> account_info_;
	};

	class GenerateOrderIDWork : public urlwork
	{
		DECLARE_URLWORK();
	public:
		GenerateOrderIDWork(const boost::shared_ptr<MtPlayer>& player, std::string ProductId, std::string Extra);
		virtual void done() override;
	private:
		const boost::shared_ptr<MtPlayer> player_;
		std::string ProductId_;
		std::string Extra_;
		std::string orderid_;
	};
}

#endif // ZXERO_GAMESERVER_MT_URL_WORK_H__