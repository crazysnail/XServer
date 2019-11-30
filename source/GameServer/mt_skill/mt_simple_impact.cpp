#include "mt_simple_impact.h"
#include "active_model.h"
#include "../mt_actor/mt_actor.h"
#include "../mt_player/mt_player.h"
#include "../mt_item/mt_item.h"
#include <AllPacketEnum.proto.fflua.h>
#include <SimpleImpact.pb.h>


namespace Mt
{
	void MtSimpleImpactSet::lua_reg(lua_State* ls)
	{
		ff::fflua_register_t<MtSimpleImpactSet, ctor()>(ls, "MtSimpleImpactSet")
			.def(&MtSimpleImpactSet::OnCheckImpact, "OnCheckImpact")
			.def(&MtSimpleImpactSet::AddSimpleImpact, "AddSimpleImpact")
			.def(&MtSimpleImpactSet::DecSimpleImpact, "DecSimpleImpact");
	}

	MtSimpleImpactSet::MtSimpleImpactSet()
	{
	}

	void MtSimpleImpactSet::SimpleImpactUpdate(uint64 elapseTime)
	{
		impact_set_.foreach([&](auto ptr) {
			if (ptr != nullptr) {
				ptr->set_left_time((int64)(ptr->left_time()) - (int64)elapseTime);
			}
		});

		std::set<int32> sets;
		impact_set_.remove_if([&](auto ptr) {
			if (ptr != nullptr && (int64)(ptr->left_time())<=0) {
				sets.insert(ptr->impact_type());
				this->UnApplySimpleImpact(ptr);
				return true;
			}
			return false;
		});

		for (auto child : sets) {
			auto config = MtItemManager::Instance().ImpactType2Config(child);
			if (config != nullptr) {
				RefreshImpact(config);
			}
		}
	}

	const Packet::SimpleImpactData* MtSimpleImpactSet::GetImpact(const int32 impact_type)
	{
		auto ptr = impact_set_.find_if([&](auto ptr) {
			if (ptr->impact_type() == impact_type) {
				return true;
			}
			return false;
		});
		return ptr;
	}
	bool MtSimpleImpactSet::OnCheckImpact(const int32 impact_type) {
		auto impact = impact_set_.find_if([&](auto ptr) {
			if (ptr != nullptr && ptr->impact_type() == impact_type) {
				return true;
			}
			return false;
		});
		return impact == nullptr ? false : true;
	}
	
	void MtSimpleImpactSet::OnClearImpact(Packet::SimpleImpactEffectType effect_type)
	{
		impact_set_.remove_if([&](auto ptr) {
			if (ptr != nullptr) {
				auto config = MtItemManager::Instance().ImpactType2Config(ptr->impact_type());
				if (config && config->effect() == effect_type) {
					this->UnApplySimpleImpact(ptr);
					return true;
				}
			}
			return false;
		});
	}
	bool MtSimpleImpactSet::DecSimpleImpact(const int32 impact_type)
	{
		auto des_config = MtItemManager::Instance().ImpactType2Config(impact_type);
		if (des_config == nullptr) {
			return false;
		}
		Packet::SimpleImpactData* temp = nullptr;
		impact_set_.remove_if([&](auto ptr) {			
			if (ptr != nullptr) {
				auto src_config = MtItemManager::Instance().ImpactType2Config(ptr->impact_type());
				if (src_config == nullptr) {
					return false;
				}
				if (src_config->group() == des_config->group()) {
					this->UnApplySimpleImpact(ptr);
					temp = ptr;
					return true;
				}				
			}
			return false;
		});

		RefreshImpact(des_config);
		return false;
	}
	
	bool MtSimpleImpactSet::AddSimpleImpact(const int32 impact_type, const int64 left_time, const int32 param1, const int32 param2, const bool dbsave)
	{
		auto final_time = left_time * 1000;
		auto des_config = MtItemManager::Instance().ImpactType2Config(impact_type);
		if (des_config == nullptr) {
			return false;
		}
		auto ptr = impact_set_.find_if([&](auto ptr) {
			auto src_config = MtItemManager::Instance().ImpactType2Config(ptr->impact_type());
			if (src_config == nullptr) {
				return false;
			}
			if (src_config->group() == des_config->group()) {
				return true;
			}
			return false;
		});
		if (ptr != nullptr) {
			//ÒÆ³ý¾ÉÐ§¹û
			UnApplySimpleImpact(ptr);
		}
		else {
			ptr = impact_set_.create();
			if (ptr == nullptr) {
				ZXERO_ASSERT(false) << "new new_impact error! ";
				return false;
			}
		}

		ptr->set_impact_type(impact_type);
		ptr->set_left_time(final_time);
		ptr->set_param1(param1);
		ptr->set_param2(param2);
		ptr->set_guid(INVALID_GUID);
		ptr->set_dbsave(dbsave);

		ApplySimpleImpact(ptr);

		RefreshImpact(des_config);

		return true;
		
	}
	
	//--------------------------------------------------------------------------------
	void MtPlayerSimpleImpactSet::lua_reg(lua_State* ls)
	{
		ff::fflua_register_t<MtPlayerSimpleImpactSet, ctor()>(ls, "MtPlayerSimpleImpactSet");
	}

	bool MtPlayerSimpleImpactSet::ApplySimpleImpact( Packet::SimpleImpactData* impact)
	{
		if (owner_ == nullptr || impact == nullptr) {
			return false;
		}

		impact->set_guid(owner_->Guid());

		Packet::SimpleImpactAdd msg;
		msg.set_actor_guid(INVALID_GUID);
		auto impact_msg = msg.add_simple_impact();
		impact_msg->CopyFrom(*impact);
		owner_->SendMessage(msg);
		return true;
	}

	bool MtPlayerSimpleImpactSet::UnApplySimpleImpact(Packet::SimpleImpactData* impact)
	{
		if (owner_ == nullptr || impact == nullptr) {
			return false;
		}

		impact->set_guid(owner_->Guid());

		Packet::SimpleImpactDel msg;
		msg.set_actor_guid(INVALID_GUID);
		auto impact_msg = msg.add_simple_impact();
		impact_msg->CopyFrom(*impact);
		owner_->SendMessage(msg);

		return true;
	}

	bool MtPlayerSimpleImpactSet::RefreshImpact(const boost::shared_ptr<Packet::SimpleImpactConfig> config)
	{
		if (config == nullptr) {
			return false;
		}

		if (config->effect() == Packet::SimpleImpactEffectType::EF_PlayerSpeed) {
			owner_->ResetSpeed();
		}
		return true;
	}

	void MtPlayerSimpleImpactSet::OnCache() {
		if (owner_ == nullptr) {
			return;
		}

		impact_set_.foreach([&](auto ptr) {
			if (ptr->dbsave()) {
				owner_->Cache2Save(ptr);
			}
		});
	}

	void MtPlayerSimpleImpactSet::OnLoad(const boost::shared_ptr<zxero::dbcontext>& dbctx) {
		if (owner_ == nullptr) {
			return;
		}
		auto impacts = ActiveModel::Base<Packet::SimpleImpactData>(dbctx).FindAll({ { "guid", owner_->Guid() } });
		for (auto child : impacts) {
			AddSimpleImpact(child->impact_type(), child->left_time()/1000, child->param1(), child->param2(), child->dbsave());
		}
	}
	void MtPlayerSimpleImpactSet::OnSave(const boost::shared_ptr<zxero::dbcontext>& /*dbctx*/) {
	}

	//----------------------------------------------------------------
	void MtActorSimpleImpactSet::lua_reg(lua_State* ls)
	{
		ff::fflua_register_t<MtActorSimpleImpactSet, ctor()>(ls, "MtActorSimpleImpactSet");
	}
	

	bool MtActorSimpleImpactSet::ApplySimpleImpact( Packet::SimpleImpactData* impact)
	{
		if (owner_ == nullptr || impact == nullptr) {
			return false;
		}

		impact->set_guid(owner_->Guid());

		Packet::SimpleImpactAdd msg;
		msg.set_actor_guid(owner_->Guid());
		auto impact_msg = msg.add_simple_impact();
		impact_msg->CopyFrom(*impact);
		owner_->SendMessage(msg);
		return true;
	}

	bool MtActorSimpleImpactSet::UnApplySimpleImpact( Packet::SimpleImpactData* impact)
	{
		if (owner_ == nullptr || impact == nullptr ) {
			return false;
		}

		impact->set_guid(owner_->Guid());

		Packet::SimpleImpactDel msg;
		msg.set_actor_guid(owner_->Guid());
		auto impact_msg = msg.add_simple_impact();
		impact_msg->CopyFrom(*impact);
		owner_->SendMessage(msg);
		return true;
	}

	bool MtActorSimpleImpactSet::RefreshImpact(const boost::shared_ptr<Packet::SimpleImpactConfig> config)
	{
		if (config && config->effect() == Packet::SimpleImpactEffectType::EF_ActorAttribute && owner_ ) {
			owner_->OnBattleInfoChanged();
		}
		return true;
	}

	void MtActorSimpleImpactSet::OnCache() {
		if (owner_ == nullptr) {
			return;
		}
		auto p = owner_->Player().lock();
		if (p == nullptr) {
			ZXERO_ASSERT(false);
			return;
		}			

		impact_set_.foreach([&](auto ptr) {
			if (ptr->dbsave()) {
				p->Cache2Save(ptr);
			}
		});
	}
	void MtActorSimpleImpactSet::OnLoad(const boost::shared_ptr<zxero::dbcontext>& dbctx) {
		if (owner_ == nullptr) {
			return;
		}

		auto impacts = ActiveModel::Base<Packet::SimpleImpactData>(dbctx).FindAll({ { "guid", owner_->Guid() } });
		for (auto child : impacts) {
			AddSimpleImpact(child->impact_type(), child->left_time()/1000, child->param1(), child->param2(), child->dbsave());
		}
	}
	void MtActorSimpleImpactSet::OnSave(const boost::shared_ptr<zxero::dbcontext>& /*dbctx*/) {
	}
}