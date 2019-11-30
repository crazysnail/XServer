#ifndef ZXERO_GAMESERVER_MT_DB_SAVE_WORK_H__
#define ZXERO_GAMESERVER_MT_DB_SAVE_WORK_H__

#include "../mt_types.h"
#include "ItemConfig.pb.h"
#include "dbwork.h"

namespace Mt
{
	using namespace zxero;
	struct shm_object;

	void DBSaveHelper(boost::shared_ptr<zxero::dbcontext>& dbctx, const shm_object* shm_obj);

	class MessageSaveWork : public dbwork
	{
		DECLARE_DBWORK();
	public:
		MessageSaveWork(const shm_object* shm_obj);
		virtual void done() override;
	private:
		const shm_object* shm_obj_;
	};

	class MessageSetSaveWork : public dbwork
	{
		DECLARE_DBWORK();
	public:
		MessageSetSaveWork(const std::list<shm_object*>& shm_obj_set);
		virtual void done() override;
	private:
		const std::list<shm_object*> shm_obj_set_;
	};

	class MessageSaveAllWork : public dbwork
	{
		DECLARE_DBWORK();
	public:
		MessageSaveAllWork();
		virtual void done() override;
	private:
	};
		
	class ArenaUserCheckInit : public dbwork
	{
		DECLARE_DBWORK();
	};

	class SaveOfflineMail : public dbwork
	{
		DECLARE_DBWORK();
	public:
		SaveOfflineMail(const std::vector<boost::shared_ptr<Packet::MessageInfo>>& all_db_mail);
	private: 
		std::vector<boost::shared_ptr<Packet::MessageInfo>> all_db_mail_;
	};


	class ShareMemoryCrashRecover : public dbwork
	{
		DECLARE_DBWORK();
		virtual void done() override;
	};

	class SaveMqPayOrder : public dbwork
	{
		DECLARE_DBWORK();
	public:
		SaveMqPayOrder(boost::shared_ptr<Packet::PayOrderInfo> payorder);
		virtual void done() override;
	private:
		boost::shared_ptr<Packet::PayOrderInfo> payorder_;
		bool ret = false;
	};
}
#endif // ZXERO_GAMESERVER_MT_DB_SAVE_WORK_H__
