#include "mt_guid.h"
#include "module.h"


namespace Mt
{
	boost::thread_specific_ptr<MT19937> rand_gen;
	REGISTER_MODULE(MtGuid)
	{
		require("MtConfig");
		register_load_function(module_base::STAGE_LOAD, boost::bind(&MtGuid::OnLoad, boost::ref(MtGuid::Instance())));
	}

	static MtGuid* __mt_guid = nullptr;
	MtGuid& MtGuid::Instance()
	{
		return *__mt_guid;
	}

	int MtGuid::OnLoad() {
		return 0;
	}

	MtGuid::MtGuid() {
		__mt_guid = this;
	}

}

