#include "mt_data_cell.h"
#include "mt_data_container.h"
#include "module.h"
#include "../data/data_manager.h"
#include "../mt_player/mt_player.h"
#include "../mt_server/mt_server.h"
#include "../base/mt_db_save_work.h"
#include "active_model.h"
#include <DataCellPacket.pb.h>
#include <DataCellPacket.proto.fflua.h>
#include "../mt_guid/mt_guid.h"

namespace Mt
{
	static MtDataCellManager* __datacell_manager = nullptr;
	REGISTER_MODULE(MtDataCellManager)
	{
		require("data_manager");
		register_load_function(module_base::STAGE_LOAD, boost::bind(&MtDataCellManager::OnLoad, __datacell_manager));
	}

	bool MtDataCellManager::LoadDataCell(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<zxero::dbcontext>& dbctx)
	{		
		auto container_ = boost::make_shared<MtCellContainer>(*player);
		player->SetDataCellContainer(container_);

		auto all_cell = ActiveModel::Base<Packet::DataCell>(dbctx).FindAll({ { "player_guid", player->Guid()} });
		for( auto child : all_cell ){
			auto data_cell = boost::make_shared<MtDataCell>(child);
			container_->add_cell_32(data_cell);
		}

		auto all_cell_64 = ActiveModel::Base<Packet::Data64Cell>(dbctx).FindAll({ { "player_guid", player->Guid() } });
		for (auto child : all_cell_64) {
			auto data_cell = boost::make_shared<MtData64Cell>(child);
			container_->add_cell_64(data_cell);
		}

		//放到进入场景之后再做这件事情
		//player->InitDataCell();

		return true;
	}


	MtDataCellManager::MtDataCellManager()
	{
		__datacell_manager = this;
	}

	MtDataCellManager& MtDataCellManager::Instance()
	{
		return *__datacell_manager;
	}

	zxero::int32 MtDataCellManager::OnLoad()
	{
		return 0;
	}
	//////////////////////////////////////////////////////////////////////////////////////////////////////////
	MtDataCell::MtDataCell(const boost::shared_ptr<Packet::DataCell> & data_cell)
		:m_data_cell(data_cell)
	{
		init();
	}

	MtDataCell::~MtDataCell()
	{
	}

	/////////////////////////////////////////////
	void MtDataCell::lua_reg(lua_State* ls)
	{
		ff::fflua_register_t<MtDataCell, ctor()>(ls, "MtDataCell")
			.def(&MtDataCell::reset, "reset")
			.def(&MtDataCell::get_data, "get_data")
			.def(&MtDataCell::get_type, "get_type")
			.def(&MtDataCell::set_data, "set_data");
	}

	void MtDataCell::reset()
	{
		if (m_data_cell == nullptr) {
			ZXERO_ASSERT(false);
			return;
		}
		m_data_cell->set_guid(INVALID_GUID);
		m_data_cell->set_player_guid(INVALID_GUID);
		m_data_cell->set_type(Packet::CellLogicType::InvalidCellType);
		for (int32 i = 0; i < MAX_CHUNK_SIZE; i++) {
			m_data_cell->set_data(i,0);
			m_size++;
		}
	}

	void MtDataCell::init()
	{
		m_size = 0;
		if (m_data_cell == nullptr) {
			ZXERO_ASSERT(false);
			return;
		}
		for (int32 i = 0; i < MAX_CHUNK_SIZE; i++)	{
			if (m_data_cell->data(i) >= 0)	{
				m_size++;
			}
		}
	}
	void MtDataCell::add_data(int32 data)
	{
		if (m_data_cell == nullptr) {
			ZXERO_ASSERT(false);
			return;
		}

		for (int32 i = 0; i < MAX_CHUNK_SIZE; i++){
			if (m_data_cell->data(i) <= 0){
				m_data_cell->set_data(i, data);
				break;
			}
		}
	}

	bool MtDataCell::is_full()
	{	
		for (int32 i = 0; i < m_size; ++i) {
			if (m_data_cell->data(i) <= 0)//0是初始值，说明cell还有空间
				return false;
		}
		return true;
	}

	//检查data是否存在于cell里
	bool MtDataCell::check_data( int32 data )
	{
		if (m_data_cell == nullptr) {
			ZXERO_ASSERT(false);
			return false;
		}
		for(int32 i=0; i<m_size; ++i){
			if(m_data_cell->data(i) == data)
				return true;
		}

		return false;
	}

	int32 MtDataCell::get_data(int32 ud_index)
	{
		if (m_data_cell == nullptr) {
			ZXERO_ASSERT(false);
			return -1;
		}
		ZXERO_ASSERT(ud_index >=0 && ud_index < MAX_CHUNK_SIZE);
		if( ud_index <0 || ud_index >= MAX_CHUNK_SIZE )
		{
			return -1;
		}

		return m_data_cell->data(ud_index);
	}
	bool MtDataCell::set_data(int32 ud_index, int32 data)
	{
		if (m_data_cell == nullptr) {
			ZXERO_ASSERT(false);
			return false;
		}
		ZXERO_ASSERT(ud_index >=0 && ud_index < MAX_CHUNK_SIZE);
		if( ud_index <0 || ud_index >= MAX_CHUNK_SIZE )
		{
			return false;
		}

		m_data_cell->set_data(ud_index, data);
		return true;
	}

	Packet::CellLogicType MtDataCell::get_type()
	{
		if (m_data_cell == nullptr) {
			return Packet::CellLogicType::InvalidCellType;
		}
		return m_data_cell->type();
	}
	///////////////////////////////////////////////////////////////////////////////////////////
	MtData64Cell::MtData64Cell(const boost::shared_ptr<Packet::Data64Cell> & data_cell)
		:m_data_cell(data_cell)
	{
		init();
	}

	MtData64Cell::~MtData64Cell()
	{
	}

	void MtData64Cell::lua_reg(lua_State* ls)
	{
		ff::fflua_register_t<MtData64Cell, ctor()>(ls, "MtData64Cell")
			.def(&MtData64Cell::reset, "reset")
			.def(&MtData64Cell::get_data, "get_data")
			.def(&MtData64Cell::get_type, "get_type")
			.def(&MtData64Cell::set_data, "set_data");
	}

	void MtData64Cell::reset()
	{
		if (m_data_cell == nullptr) {
			ZXERO_ASSERT(false);
			return;
		}
		m_data_cell->set_guid(INVALID_GUID);
		m_data_cell->set_player_guid(INVALID_GUID);
		m_data_cell->set_type(Packet::Cell64LogicType::InvalidCell64Type);
		for (int32 i = 0; i < MAX_CHUNK_SIZE; i++) {
			m_data_cell->set_data(i, 0);
		}
		m_size = 0;
	}

	void MtData64Cell::init()
	{
		if (m_data_cell == nullptr) {
			ZXERO_ASSERT(false);
			return;
		}
		for (int32 i = 0; i < MAX_CHUNK_SIZE; i++) {
			if (m_data_cell->data(i) >= 0) {
				m_size++;
			}
		}
	}
	void MtData64Cell::add_data(int64 data)
	{
		if (m_data_cell == nullptr) {
			ZXERO_ASSERT(false);
			return;
		}

		for (int32 i = 0; i < MAX_CHUNK_SIZE; i++) {
			if (m_data_cell->data(i) <= 0) {
				m_data_cell->set_data(i, data);
				break;
			}
		}
	}

	bool MtData64Cell::is_full()
	{
		for (int32 i = 0; i < m_size; ++i) {
			if (m_data_cell->data(i) <= 0)//0是初始值，说明cell还有空间
				return false;
		}
		return true;
	}

	//检查data是否存在于cell里
	bool MtData64Cell::check_data(int64 data)
	{
		if (m_data_cell == nullptr) {
			ZXERO_ASSERT(false);
			return false;
		}
		for (int32 i = 0; i < m_size; ++i) {
			if (m_data_cell->data(i) == (uint64)data)
				return true;
		}

		return false;
	}

	int64 MtData64Cell::get_data(int32 ud_index)
	{
		if (m_data_cell == nullptr) {
			ZXERO_ASSERT(false);
			return -1;
		}
		ZXERO_ASSERT(ud_index >= 0 && ud_index < MAX_CHUNK_SIZE);
		if (ud_index < 0 || ud_index >= MAX_CHUNK_SIZE)
		{
			return -1;
		}

		return m_data_cell->data(ud_index);
	}
	bool MtData64Cell::set_data(int32 ud_index, int64 data)
	{
		if (m_data_cell == nullptr) {
			ZXERO_ASSERT(false);
			return false;
		}
		ZXERO_ASSERT(ud_index >= 0 && ud_index < MAX_CHUNK_SIZE);
		if (ud_index < 0 || ud_index >= MAX_CHUNK_SIZE)
		{
			return false;
		}

		m_data_cell->set_data(ud_index, data);
		return true;
	}

	Packet::Cell64LogicType MtData64Cell::get_type()
	{
		if (m_data_cell == nullptr) {
			return Packet::Cell64LogicType::InvalidCell64Type;
		}
		return m_data_cell->type();
	}

}	//	namespace game

