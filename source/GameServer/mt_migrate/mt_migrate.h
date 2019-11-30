#ifndef ZXERO_GAMESERVER_MT_MIGRATE_H__
#define ZXERO_GAMESERVER_MT_MIGRATE_H__
#include "../mt_types.h"

namespace Mt
{	


	class MtMigrate : public boost::noncopyable
	{
	public:
		int OnLoad();;
		int OnPostLoad();
		static MtMigrate& Instance();
		MtMigrate();
		UniqueValue GetMsgUniqueValue(const google::protobuf::Message& msg);

	private:
		std::map<std::string, std::list<ActiveModel::FieldSqlProperty>> all_tables_;
		std::map<std::string, uint64> key_tables_;
	};
}


#endif // ZXERO_GAMESERVER_MT_MIGRATE_H__
