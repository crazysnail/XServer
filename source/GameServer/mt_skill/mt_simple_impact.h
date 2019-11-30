#ifndef MTONLINE_GAMESERVER_MT_SIMPLE_IMPACT_H__
#define MTONLINE_GAMESERVER_MT_SIMPLE_IMPACT_H__

#include "../mt_types.h"
#include <SimpleImpact.pb.h>
#include "object_pool.h"

namespace Mt
{
	class MtSimpleImpactSet : public boost::noncopyable
	{
	public:
		static void lua_reg(lua_State* ls);
	public:
		MtSimpleImpactSet();
		virtual ~MtSimpleImpactSet() { impact_set_.release(); };

		virtual bool ApplySimpleImpact( Packet::SimpleImpactData* impact) { impact; ZXERO_ASSERT(false); return false; }
		virtual bool UnApplySimpleImpact( Packet::SimpleImpactData* impact) { impact; ZXERO_ASSERT(false); return false; }
		virtual bool RefreshImpact(const boost::shared_ptr<Packet::SimpleImpactConfig> config) { config; ZXERO_ASSERT(false); return false; }
		virtual void OnCache() { ZXERO_ASSERT(false); }
		virtual void OnLoad(const boost::shared_ptr<zxero::dbcontext>& dbctx) { dbctx; ZXERO_ASSERT(false); }
		virtual void OnSave(const boost::shared_ptr<zxero::dbcontext>& dbctx) { dbctx; ZXERO_ASSERT(false); }

		const  Packet::SimpleImpactData* GetImpact(const int32 impact_type);
		bool OnCheckImpact(const int32 impact_type);
		bool AddSimpleImpact(const int32 impact_type, const int64 left_time, const int32 param1, const int32 param2, const bool dbsave);
		bool DecSimpleImpact(const int32 impact_type);
		void SimpleImpactUpdate(uint64 elapseTime);
		void foreach(const std::function<void(const Packet::SimpleImpactData*)>& func){
			return impact_set_.foreach(func);
		}
		void OnClearImpact(Packet::SimpleImpactEffectType effect_type);
	
	protected:		
		object_pool_ex<Packet::SimpleImpactData> impact_set_{ 20 };
	};

	class MtPlayerSimpleImpactSet : public MtSimpleImpactSet
	{
	public:
		static void lua_reg(lua_State* ls);
	public:
		MtPlayerSimpleImpactSet() {};
		virtual ~MtPlayerSimpleImpactSet() {};
		void Init(MtPlayer * owner) { owner_ = owner; }
		virtual bool ApplySimpleImpact( Packet::SimpleImpactData* impact);
		virtual bool UnApplySimpleImpact( Packet::SimpleImpactData* impact);
		virtual bool RefreshImpact(const boost::shared_ptr<Packet::SimpleImpactConfig> config);
		virtual void OnCache();
		virtual void OnLoad(const boost::shared_ptr<zxero::dbcontext>& dbctx);
		virtual void OnSave(const boost::shared_ptr<zxero::dbcontext>& dbctx);
	private:
		MtPlayer * owner_;
	};
	class MtActorSimpleImpactSet : public MtSimpleImpactSet
	{
	public:
		static void lua_reg(lua_State* ls);
	public:
		MtActorSimpleImpactSet() {};
		virtual ~MtActorSimpleImpactSet() {};
		void Init(MtActor * owner) { owner_ = owner; }
		virtual bool ApplySimpleImpact( Packet::SimpleImpactData* impact);
		virtual bool UnApplySimpleImpact( Packet::SimpleImpactData* impact);
		virtual bool RefreshImpact(const boost::shared_ptr<Packet::SimpleImpactConfig> config);
		virtual void OnCache();
		virtual void OnLoad(const boost::shared_ptr<zxero::dbcontext>& dbctx);
		virtual void OnSave(const boost::shared_ptr<zxero::dbcontext>& dbctx);
	private:
		MtActor * owner_;
	};
	

}
#endif // MTONLINE_GAMESERVER_MT_SIMPLE_IMPACT_H__
