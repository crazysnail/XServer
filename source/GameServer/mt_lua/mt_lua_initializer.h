#include "../mt_types.h"
#include "lua/fflua.h"

namespace Mt
{
	class MtLuaInitializer : boost::noncopyable
	{
	public:
		static MtLuaInitializer& Instance();
		void SceneDataInit(ff::fflua_t& ff_lua) const;
		void ServerDataInit(ff::fflua_t& ff_lua) const;
	private:
		void GlobalDataInit(ff::fflua_t& ff_lua) const;
	};
}