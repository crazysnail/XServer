#include "mt_data_container.h"
#include "mt_data_cell.h"
#include "module.h"
#include "../data/data_manager.h"
#include "../mt_player/mt_player.h"
#include "../mt_server/mt_server.h"
#include "../base/mt_db_save_work.h"
#include "../mt_cache/mt_shm_manager.h"
#include "active_model.h"
#include <DataCellPacket.pb.h>
#include <DataCellPacket.proto.fflua.h>
#include "../mt_guid/mt_guid.h"

namespace Mt
{
	void MtCellContainer::lua_reg(lua_State* ls)
	{
		ff::fflua_register_t<MtCellContainer, ctor()>(ls, "MtCellContainer")
			.def(&MtCellContainer::get_data_32, "get_data_32")
			.def(&MtCellContainer::get_data_64, "get_data_64")
			.def(&MtCellContainer::lua_set_data_32, "set_data_32")
			.def(&MtCellContainer::lua_set_data_64, "set_data_64")
			.def(&MtCellContainer::lua_add_data_32, "add_data_32")
			.def(&MtCellContainer::lua_add_data_64, "add_data_64")
			.def(&MtCellContainer::check_bit_data, "check_bit_data")
			.def(&MtCellContainer::set_bit_data, "set_bit_data")
			.def(&MtCellContainer::reset_bit_data, "reset_bit_data")
			.def(&MtCellContainer::lua_check_bit_data_32, "check_bit_data_32")			
			.def(&MtCellContainer::lua_set_bit_data_32, "set_bit_data_32")
			.def(&MtCellContainer::lua_reset_bit_data_32, "reset_bit_data_32")
			.def(&MtCellContainer::lua_check_bit_data_64, "check_bit_data_64")
			.def(&MtCellContainer::lua_set_bit_data_64, "set_bit_data_64")
			.def(&MtCellContainer::lua_reset_bit_data_64, "reset_bit_data_64")
			.def(&MtCellContainer::try_init_int32_data,"try_init_int32_data")
			.def(&MtCellContainer::try_init_int64_data, "try_init_int64_data")
			.def(&MtCellContainer::push_data_32, "push_data_32")
			.def(&MtCellContainer::push_data_64, "push_data_64")
			.def(&MtCellContainer::check_data_32, "check_data_32")
			.def(&MtCellContainer::check_data_64, "check_data_64")
			.def(&MtCellContainer::get_high_16_bit, "get_high_16_bit")
			.def(&MtCellContainer::get_low_16_bit, "get_low_16_bit")
			.def(&MtCellContainer::get_high_32_bit, "get_high_32_bit")
			.def(&MtCellContainer::get_low_32_bit, "get_low_32_bit")
			.def(&MtCellContainer::bind_bit_data_32, "bind_bit_data_32")
			.def(&MtCellContainer::bind_bit_data_64, "bind_bit_data_64")
			;
	}

	MtCellContainer::MtCellContainer(MtPlayer& player)
		:player_(player.weak_from_this())
	{
		for (int32 i = 0; i < Packet::CellLogicType_ARRAYSIZE; i++) {
			m_datacell_pool[i].clear();
		}

		for (int32 i = 0; i < Packet::Cell64LogicType_ARRAYSIZE; i++) {
			m_data64cell_pool[i].clear();
		}
	}

	MtCellContainer::~MtCellContainer()
	{			
	}

	bool MtCellContainer::try_init_int32_data(Packet::CellLogicType logic_type, int32 ud_index, int32 data)
	{
		int32 cell_value = get_data_32(logic_type, ud_index);
		if (cell_value == -1){//说明是之前没有数据

			int32 group = ud_index / MAX_CHUNK_SIZE;//在第几组
			int size = group - calc_cell_count_32(logic_type);
			if( size >= 0 )
			{// 如果新组号大于已有的组号说明没有可用的cell了 [12/20/2014 SEED]
				if(!align_size_32(logic_type,size+1)){
					ZXERO_ASSERT(false) << "align_size failed! cell_logic_type=" << logic_type << ";ud_index=" << ud_index << ";value=" << data;
					return false;
				}
			}
			return set_data_32(logic_type, ud_index, data);
		}
		else{
			return false;
		}
	}

	bool MtCellContainer::try_init_int64_data(Packet::Cell64LogicType logic_type, int32 ud_index, int64 data)
	{
		int64 cell_value = get_data_64(logic_type, ud_index);
		if (cell_value == -1){//说明是之前没有数据

			int32 group = ud_index / MAX_CHUNK_SIZE;//在第几组
			int size = group - calc_cell_count_64(logic_type);
			if (size >= 0)
			{// 如果新组号大于已有的组号说明没有可用的cell了 [12/20/2014 SEED]
				if (!align_size_64(logic_type, size + 1)) {
					ZXERO_ASSERT(false) << "align_size failed! cell_logic_type=" << logic_type << ";ud_index=" << ud_index << ";value=" << data;
					return false;
				}
			}

			return set_data_64(logic_type, ud_index, data);
		}
		else{
			return false;
		}
	}

	void MtCellContainer::add_cell_32(boost::shared_ptr<MtDataCell> & cell)
	{
		if (cell == nullptr){
			return;
		}
		auto type = cell->get_type();
		if (type > Packet::CellLogicType::InvalidCellType && type< Packet::CellLogicType_ARRAYSIZE){
			m_datacell_pool[type].push_back(cell);
		}			
	}

	void MtCellContainer::add_cell_64(boost::shared_ptr<MtData64Cell> & cell)
	{
		if (cell == nullptr) {
			return;
		}
		auto type = cell->get_type();
		if (type > Packet::Cell64LogicType::InvalidCell64Type && type < Packet::Cell64LogicType_ARRAYSIZE) {
			m_data64cell_pool[type].push_back(cell);
		}
	}

	bool MtCellContainer::align_size_32(Packet::CellLogicType logic_type,int32 size )
	{
		if (logic_type <= Packet::CellLogicType::InvalidCellType || logic_type >= Packet::CellLogicType_ARRAYSIZE) {
			return false;
		}
		auto player = player_.lock();
		if (player == nullptr) {
			LOG_ERROR << "[cell container] player is null";
			return false;
		}
		for( int i=0; i<size; i++ )
		{
			auto celldb = boost::make_shared<Packet::DataCell>();
			if (celldb != nullptr) {
				int32 index = m_datacell_pool[logic_type].size();
				int64 guid = gen_cell32_guid(logic_type, index);

				celldb->set_player_guid(player->Guid());
				celldb->set_type(logic_type);
				celldb->set_guid(guid);

				for (int32 k = 0; k < MAX_CHUNK_SIZE; k++) {
					celldb->add_data(0);
				}

				auto cell = boost::make_shared<MtDataCell>(celldb);
				if (cell == nullptr ){
					return false;
				}

				m_datacell_pool[logic_type].push_back(cell);
			}
			else {
				return false;
			}
		}

		DataCellListUpdate();
		
		return true;
	}

	bool MtCellContainer::align_size_64(Packet::Cell64LogicType logic_type, int32 size)
	{
		if (logic_type <= Packet::Cell64LogicType::InvalidCell64Type || logic_type >= Packet::Cell64LogicType_ARRAYSIZE) {
			return false;
		}

		auto player = player_.lock();
		if (player == nullptr) {
			LOG_ERROR << "[cell container] player is null";
			return false;
		}
		for (int i = 0; i < size; i++)
		{
			auto celldb = boost::make_shared<Packet::Data64Cell>();
			if (celldb != nullptr) {
				int32 index = m_data64cell_pool[logic_type].size();
				int64 guid = gen_cell64_guid(logic_type, index);

				celldb->set_player_guid(player->Guid());
				celldb->set_type(logic_type);
				celldb->set_guid(guid);

				for (int32 k = 0; k < MAX_CHUNK_SIZE; k++) {
					celldb->add_data(0);
				}

				auto cell = boost::make_shared<MtData64Cell>(celldb);
				if (cell == nullptr) {
					return false;
				}
				m_data64cell_pool[logic_type].push_back(cell);
			}
			else {
				return false;
			}
		}

		DataCell64ListUpdate();

		return true;
	}

	uint32 MtCellContainer::calc_cell_count_32(Packet::CellLogicType logic_type)
	{
		if (logic_type <= Packet::CellLogicType::InvalidCellType || logic_type >= Packet::CellLogicType_ARRAYSIZE) {
			return 0;
		}

		return m_datacell_pool[logic_type].size();
	}

	uint32 MtCellContainer::calc_cell_count_64(Packet::Cell64LogicType logic_type)
	{
		if (logic_type <= Packet::Cell64LogicType::InvalidCell64Type || logic_type >= Packet::Cell64LogicType_ARRAYSIZE) {
			return 0;
		}

		return m_data64cell_pool[logic_type].size();
	}

	//通过类型取得可用的cell
	boost::shared_ptr<MtDataCell> MtCellContainer::get_useable_cell_32( Packet::CellLogicType logic_type )
	{
		auto player = player_.lock();
		if (player == nullptr) {
			LOG_ERROR << "[cell container] player is null";
			return nullptr;
		}
		if( logic_type <= Packet::CellLogicType::InvalidCellType || logic_type >= Packet::CellLogicType_ARRAYSIZE){
			return nullptr;
		}
	
		for( auto& iter : m_datacell_pool[logic_type]){
			if (!iter->is_full()) {
				return iter;
			}
		}
		// 说明没有可用的cell了 [12/20/2014 SEED]
		auto celldb = boost::make_shared<Packet::DataCell>();		
		if(celldb != nullptr ){
			int32 index = m_datacell_pool[logic_type].size();
			int64 guid = gen_cell32_guid(logic_type, index);

			celldb->set_player_guid(player->Guid());
			celldb->set_type(logic_type);
			celldb->set_guid(guid);
			for (int32 k = 0; k < MAX_CHUNK_SIZE; k++) {
				celldb->add_data(0);
			}

			auto cell = boost::make_shared<MtDataCell>(celldb);
			m_datacell_pool[logic_type].push_back(cell);

			return cell;
		}
		else{
			return nullptr;
		}
	}

	boost::shared_ptr<MtData64Cell> MtCellContainer::get_useable_cell_64(Packet::Cell64LogicType logic_type)
	{
		auto player = player_.lock();
		if (player == nullptr) {
			LOG_ERROR << "[cell container] player is null";
			return nullptr;
		}
		if (logic_type <= Packet::Cell64LogicType::InvalidCell64Type || logic_type >= Packet::Cell64LogicType_ARRAYSIZE) {
			return nullptr;
		}

		for (auto& iter : m_data64cell_pool[logic_type]){
			if (!iter->is_full()) {
				return iter;
			}
		}
		// 说明没有可用的cell了 [12/20/2014 SEED]
		auto celldb = boost::make_shared<Packet::Data64Cell>();
		if (celldb != nullptr) {
			int32 index = m_data64cell_pool[logic_type].size();
			int64 guid = gen_cell64_guid(logic_type, index);

			celldb->set_player_guid(player->Guid());
			celldb->set_type(logic_type);
			celldb->set_guid(guid);
			for (int32 k = 0; k < MAX_CHUNK_SIZE; k++) {
				celldb->add_data(0);
			}

			auto cell = boost::make_shared<MtData64Cell>(celldb);
			m_data64cell_pool[logic_type].push_back(cell);

			return cell;
		}
		else {
			return nullptr;
		}
	}

	bool MtCellContainer::push_data_32(Packet::CellLogicType logic_type, int32 data, bool refresh2client)
	{
		if (check_data_32(logic_type, data)) {
			return false;
		}

		auto cell = get_useable_cell_32(logic_type);
		if (cell == nullptr) {
			return false;
		}
		cell->add_data(data);

		if (refresh2client) {
			SingleDataCellListUpdate(logic_type);
		}

		return true;

	}
	bool MtCellContainer::push_data_64(Packet::Cell64LogicType logic_type, int64 data, bool refresh2client)
	{
		if (check_data_64(logic_type, data)) {
			return false;
		}

		auto cell = get_useable_cell_64(logic_type);
		if (cell == nullptr) {
			return false;
		}
		cell->add_data(data);

		if (refresh2client) {
			SingleDataCell64ListUpdate(logic_type);
		}

		return true;
	}

	//数据检查
	bool MtCellContainer::check_data_32(Packet::CellLogicType logic_type,int32 data)
	{
		if (logic_type <= Packet::CellLogicType::InvalidCellType || logic_type >= Packet::CellLogicType_ARRAYSIZE) {
			return false;
		}
		for (auto& iter : m_datacell_pool[logic_type]){
			if (iter->check_data(data)) {
				return true;
			}
		}
		//数据不存在
		return false;
	}

	bool MtCellContainer::check_data_64(Packet::Cell64LogicType logic_type, int64 data)
	{
		if (logic_type <= Packet::Cell64LogicType::InvalidCell64Type || logic_type >= Packet::Cell64LogicType_ARRAYSIZE) {
			return false;
		}

		for (auto& iter : m_data64cell_pool[logic_type]){
			if (iter->check_data(data)){
				return true;
			}
		}
		//数据不存在
		return false;
	}
	
	//数据获取
	int32 MtCellContainer::get_data_32(Packet::CellLogicType logic_type,int32 ud_index)
	{
		if (logic_type <= Packet::CellLogicType::InvalidCellType || logic_type >= Packet::CellLogicType_ARRAYSIZE) {
			return -1;
		}
		int32 offset = ud_index%MAX_CHUNK_SIZE; //固定的偏移下标
		uint32 group = ud_index/MAX_CHUNK_SIZE;//在第几组
		if (group < 0 || group >= m_datacell_pool[logic_type].size()) {
			return -1;
		}
		auto cell = m_datacell_pool[logic_type][group];
		if (cell != nullptr) {
			return cell->get_data(offset);
		}

		//数据不存在
		return -1;
	}

	int64 MtCellContainer::get_data_64(Packet::Cell64LogicType logic_type, int32 ud_index)
	{
		if (logic_type <= Packet::Cell64LogicType::InvalidCell64Type || logic_type >= Packet::Cell64LogicType_ARRAYSIZE) {
			return false;
		}
		int32 offset = ud_index%MAX_CHUNK_SIZE; //固定的偏移下标
		uint32 group = ud_index / MAX_CHUNK_SIZE;//在第几组
		if (group < 0 || group >= m_data64cell_pool[logic_type].size()) {
			return -1;
		}
		auto cell = m_data64cell_pool[logic_type][group];
		if (cell != nullptr) {
			return cell->get_data(offset);
		}

		//数据不存在
		return -1;
	}

	void MtCellContainer::get_all_data_32(Packet::CellLogicType logic_type, std::vector<int32> &data)
	{
		if (logic_type <= Packet::CellLogicType::InvalidCellType || logic_type >= Packet::CellLogicType_ARRAYSIZE) {
			return;
		}

		for( auto& iter : m_datacell_pool[logic_type])
		{
			for (int32 i=0; i<MAX_CHUNK_SIZE; ++i)
			{
				int32 datacell = iter->get_data(i);
				if (datacell>0){
					data.push_back(datacell);
				}
			}
		}
	}

	uint32 MtCellContainer::get_count_data_32(Packet::CellLogicType logic_type)
	{
		std::vector<int32> data;
		get_all_data_32(logic_type,data);
		return data.size();
	}

	void MtCellContainer::get_all_data_64(Packet::Cell64LogicType logic_type, std::vector<int64> &data)
	{
		if (logic_type <= Packet::Cell64LogicType::InvalidCell64Type || logic_type >= Packet::Cell64LogicType_ARRAYSIZE) {
			return;
		}

		for (auto& iter : m_data64cell_pool[logic_type])
		{
			for (int32 i = 0; i < MAX_CHUNK_SIZE; ++i)
			{
				int64 datacell = iter->get_data(i);
				if (datacell > 0){
					data.push_back(datacell);
				}
			}
		}
	}

	uint32 MtCellContainer::get_count_data_64(Packet::Cell64LogicType logic_type)
	{
		std::vector<int64> data;
		get_all_data_64(logic_type, data);
		return data.size();
	}

	//数据设置
	bool MtCellContainer::set_data_32(Packet::CellLogicType logic_type,int32 ud_index, int32 data, bool refresh2client)
	{
		if (logic_type <= Packet::CellLogicType::InvalidCellType || logic_type >= Packet::CellLogicType_ARRAYSIZE) {
			return false;
		}

		int32 offset = ud_index%MAX_CHUNK_SIZE; //固定的偏移下标
		uint32 group = ud_index/MAX_CHUNK_SIZE;//在第几组
		
		if (group < 0 || group >= m_datacell_pool[logic_type].size()) {
			return false;
		}
		auto cell = m_datacell_pool[logic_type][group];
		if (cell != nullptr) {
			bool ret = cell->set_data(offset, data);
			if (!ret) {
				LOG_ERROR << "[data_cell] index out of range! offset=" << ud_index << "logic_type=" << logic_type << "e_int32_cell";
			}
			if (refresh2client) {
				DataCellValueUpdate(logic_type, ud_index, data);
			}
			return ret;
		}

		//数据不存在
		ZXERO_ASSERT(false) << "[data_cell] group out of range! offset=" << ud_index << "logic_type=" << logic_type << "e_int32_cell";
		return false;
	}

	bool MtCellContainer::set_data_64(Packet::Cell64LogicType logic_type, int32 ud_index, int64 data, bool refresh2client)
	{
		if (logic_type <= Packet::Cell64LogicType::InvalidCell64Type || logic_type >= Packet::Cell64LogicType_ARRAYSIZE) {
			return false;
		}
		int32 offset = ud_index%MAX_CHUNK_SIZE; //固定的偏移下标
		uint32 group = ud_index / MAX_CHUNK_SIZE;//在第几组

		if (group < 0 || group >= m_data64cell_pool[logic_type].size()) {
			return false;
		}
		auto cell = m_data64cell_pool[logic_type][group];
		if (cell != nullptr) {
			bool ret = cell->set_data(offset, data);
			if (!ret) {
				LOG_ERROR << "[data_cell] index out of range! offset=" << ud_index << "logic_type=" << logic_type;
			}
			if (refresh2client) {
				DataCellValueUpdate(logic_type, ud_index, data);
			}
			return ret;
		}

		//数据不存在
		ZXERO_ASSERT(false) << "[data_cell] group out of range! offset=" << ud_index << "logic_type=" << logic_type << "e_int32_cell";
		return false;
	}
	
	bool MtCellContainer::add_data_32(Packet::CellLogicType logic_type,int32 ud_index, int32 data, bool refresh2client)
	{
		int32 old_count = get_data_32(logic_type,ud_index);
		old_count+=data;
		return set_data_32(logic_type,ud_index,old_count, refresh2client);
	}

	bool MtCellContainer::add_data_64(Packet::Cell64LogicType logic_type, int32 ud_index, int64 data, bool refresh2client)
	{
		int64 old_count = get_data_64(logic_type, ud_index);
		old_count += data;
		return set_data_64(logic_type, ud_index, old_count, refresh2client);
	}
	
	void MtCellContainer::OnSave(const boost::shared_ptr<zxero::dbcontext>& dbctx)
	{
		auto player = player_.lock();
		if (player == nullptr) {
			LOG_ERROR << "[cell container] player is null";
			return;
		}
		ActiveModel::Base<Packet::DataCell>(dbctx).Delete({ { "player_guid", player->Guid() } });
		for (int32 i = 0; i < Packet::CellLogicType_ARRAYSIZE; i++) {
			for (auto child : m_datacell_pool[i]) {
				ActiveModel::Base<Packet::DataCell>(dbctx).Save(child->db_data());
			}
		}

		ActiveModel::Base<Packet::Data64Cell>(dbctx).Delete({ { "player_guid", player->Guid() } });
		for (int32 i = 0; i < Packet::Cell64LogicType_ARRAYSIZE; i++) {
			for (auto child : m_data64cell_pool[i]) {
				ActiveModel::Base<Packet::Data64Cell>(dbctx).Save(child->db_data());
			}
		}
	}

	void MtCellContainer::OnCache()
	{
		auto player = player_.lock();
		if (player == nullptr) {
			LOG_ERROR << "[cell container] player is null";
			return;
		}
		for (int32 i = 0; i < Packet::CellLogicType_ARRAYSIZE; i++) {
			for (auto child : m_datacell_pool[i]) {
				player->Cache2Save(child->db_data().get());
			}
		}
		for (int32 i = 0; i < Packet::Cell64LogicType_ARRAYSIZE; i++) {
			for (auto child : m_data64cell_pool[i]) {
				player->Cache2Save(child->db_data().get());
			}
		}
	}

	//////////////////////////////////////////////////////////////////////////
	//位标记集和
	bool MtCellContainer::check_bit_data(Packet::BitFlagCellIndex ud_index)
	{
		int32 bit_offset = ud_index % 32;//在int32的第几位
		int32 bit_group = ud_index / 32;//在第几个int32

		int32 bit_data = get_data_32(Packet::BitFlagCell, bit_group);
		if (bit_data <= -1) {
			bit_data = 0;
		}

		if (set_data_32(Packet::BitFlagCell, bit_group, bit_data)) {
			return utils::bit_check(bit_data, bit_offset);
		}
		return false;
	}

	bool MtCellContainer::set_bit_data(Packet::BitFlagCellIndex ud_index, bool refresh2client)
	{
		int32 bit_offset = ud_index % 32;//在int32的第几位
		int32 bit_group = ud_index / 32;//在第几个int32

		int32 bit_data = get_data_32(Packet::BitFlagCell, bit_group);
		if (bit_data <= -1) {
			bit_data = 0;
		}

		utils::bit_set(bit_data, bit_offset);
		return set_data_32(Packet::BitFlagCell, bit_group, bit_data, refresh2client);
	}

	bool MtCellContainer::reset_bit_data(Packet::BitFlagCellIndex ud_index, bool refresh2client)
	{
		int32 bit_offset = ud_index % 32;//在int32的第几位
		int32 bit_group = ud_index / 32;//在第几个int32

		int32 bit_data = get_data_32(Packet::BitFlagCell, bit_group);
		if (bit_data <= -1) {
			bit_data = 0;
		}

		utils::bit_reset(bit_data, bit_offset);
		return set_data_32(Packet::BitFlagCell, bit_group, bit_data, refresh2client);
	}

	//////////////////////////////////////////////////

	void MtCellContainer::SingleDataCellListUpdate(Packet::CellLogicType logic_type) 
	{
		if (logic_type <= Packet::CellLogicType::InvalidCellType || logic_type >= Packet::CellLogicType_ARRAYSIZE) {
			return;
		}
		auto player = player_.lock();
		if (player == nullptr) {
			LOG_ERROR << "[cell container] player is null";
			return;
		}
		Packet::SingleDataCell32ListReply msg;
		msg.set_type(logic_type);
		for (auto child : m_datacell_pool[logic_type]){
			auto cell = msg.add_cell_list();
			cell->CopyFrom(*(child->db_data()));
		}
		player->SendMessage(msg);
	}

	void MtCellContainer::SingleDataCell64ListUpdate(Packet::Cell64LogicType logic_type) 
	{
		if (logic_type <= Packet::Cell64LogicType::InvalidCell64Type || logic_type >= Packet::Cell64LogicType_ARRAYSIZE) {
			return;
		}
		auto player = player_.lock();
		if (player == nullptr) {
			LOG_ERROR << "[cell container] player is null";
			return;
		}
		Packet::SingleDataCell64ListReply msg;
		msg.set_type(logic_type);
		for (auto child : m_data64cell_pool[logic_type]){
			auto cell = msg.add_cell_list();
			cell->CopyFrom(*(child->db_data()));
		}
		player->SendMessage(msg);
	}

	void MtCellContainer::DataCellListUpdate() 
	{
		auto player = player_.lock();
		if (player == nullptr) {
			LOG_ERROR << "[cell container] player is null";
			return;
		}
		Packet::DataCell32ListReply msg;
		for (int32 i = 0; i < Packet::CellLogicType_ARRAYSIZE; i++) {
			for (auto child : m_datacell_pool[i]) {
				auto cell = msg.add_cell_list();
				cell->CopyFrom(*(child->db_data()));
			}
		}
		player->SendMessage(msg);
	}

	void MtCellContainer::DataCell64ListUpdate() 
	{
		auto player = player_.lock();
		if (player == nullptr) {
			LOG_ERROR << "[cell container] player is null";
			return;
		}
		Packet::DataCell64ListReply msg;
		for (int32 i = 0; i < Packet::Cell64LogicType_ARRAYSIZE; i++) {
			for (auto child : m_data64cell_pool[i])	{
				auto cell = msg.add_cell_list();
				cell->CopyFrom(*(child->db_data()));
			}
		}
		player->SendMessage(msg);
	}

	void MtCellContainer::DataCellValueUpdate(Packet::CellLogicType logic_type, int32 ud_index, int32 data) 
	{
		auto player = player_.lock();
		if (player == nullptr) {
			LOG_ERROR << "[cell container] player is null";
			return;
		}
		Packet::DataCell32ValueUpdateReply msg;
		msg.set_type(logic_type);
		msg.set_index(ud_index);
		msg.set_data(data);
		player->SendMessage(msg);
	}

	void MtCellContainer::DataCellValueUpdate(Packet::Cell64LogicType logic_type, int32 ud_index, int64 data) 
	{
		auto player = player_.lock();
		if (player == nullptr) {
			LOG_ERROR << "[cell container] player is null";
			return;
		}
		Packet::DataCell64ValueUpdateReply msg;
		msg.set_type(logic_type);
		msg.set_index(ud_index);
		msg.set_data(data);
		player->SendMessage(msg);
	}

	void MtCellContainer::DataCellUpdate(const boost::shared_ptr<MtDataCell> datacell) 
	{
		auto player = player_.lock();
		if (player == nullptr) {
			LOG_ERROR << "[cell container] player is null";
			return;
		}
		Packet::DataCellUpdateReply msg;
		auto cell = msg.mutable_cell32();
		cell->CopyFrom(*datacell->db_data());
		player->SendMessage(msg);
	}

	void MtCellContainer::DataCellUpdate(const boost::shared_ptr<MtData64Cell> datacell) 
	{
		auto player = player_.lock();
		if (player == nullptr) {
			LOG_ERROR << "[cell container] player is null";
			return;
		}
		Packet::DataCellUpdateReply msg;
		auto cell = msg.mutable_cell64();
		cell->CopyFrom(*datacell->db_data());
		player->SendMessage(msg);
	}


}

