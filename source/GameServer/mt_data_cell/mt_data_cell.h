#ifndef _data_cell_h
#define _data_cell_h
#include "../mt_types.h"
#include "AllPacketEnum.pb.h"
#include "AllConfigEnum.pb.h"
namespace Mt
{
	class MtDataCellManager : public boost::noncopyable
	{
	public:
		static MtDataCellManager& Instance();
		MtDataCellManager();
		zxero::int32 OnLoad();
		bool LoadDataCell(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<zxero::dbcontext>& dbctx);
		
	};

	const int32 MAX_CHUNK_SIZE = 64;
	class MtDataCell : public boost::noncopyable, public boost::enable_shared_from_this<MtDataCell>
	{
	public:
		static void lua_reg(lua_State* ls);
	public:
		MtDataCell() {};
		MtDataCell( const boost::shared_ptr<Packet::DataCell> & data_cell );
		virtual ~MtDataCell();

	public:
		// init函数不能用来重置数据！有需求用reset
		void init();			
		//重置
		void reset();

		//添加一个暑假
		void add_data(int32 data);
		//检查一个cell是否已经满了
		bool is_full();
		//检查data是否存在于cell里
		bool check_data( int32 data );
		//数据获取
		int32 get_data(int32 ud_index);
		//数据设置
		bool set_data(int32 ud_index, int32 data);
		//取得cell当前数据个数
		int32 get_size() const{
			return m_size;
		}
		Packet::CellLogicType get_type();

		boost::shared_ptr<Packet::DataCell> db_data() const  {
			return m_data_cell;
		}
	public:
		//size是给不定长的datacell用的，通过addXXX接口发生变化，
		//setXXX接口是不会影响该值的（这里可能会存在隐患，
		//就是当一个setXXX的datacell变成了不定长的就会导致数据混乱）
		int32 m_size;
		mutable boost::shared_ptr<Packet::DataCell> m_data_cell;
	};

	class MtData64Cell : public boost::noncopyable, public boost::enable_shared_from_this<MtData64Cell>
	{
	public:
		static void lua_reg(lua_State* ls);
	public:
		MtData64Cell() {};
		MtData64Cell(const boost::shared_ptr<Packet::Data64Cell> & data_cell);
		virtual ~MtData64Cell();

	public:
		// init函数不能用来重置数据！有需求用reset
		void init();
		//重置
		void reset();

		//添加一个暑假
		void add_data(int64 data);
		//检查一个cell是否已经满了
		bool is_full();
		//检查data是否存在于cell里
		bool check_data(int64 data);
		//数据获取
		int64 get_data(int32 ud_index);
		//数据设置
		bool set_data(int32 ud_index, int64 data);
		//取得cell当前数据个数
		int32 get_size() const {
			return m_size;
		}

		Packet::Cell64LogicType get_type();

		boost::shared_ptr<Packet::Data64Cell> db_data() const {
			return m_data_cell;
		}
	public:
		//size是给不定长的datacell用的，通过addXXX接口发生变化，
		//setXXX接口是不会影响该值的（这里可能会存在隐患，
		//就是当一个setXXX的datacell变成了不定长的就会导致数据混乱）
		int32 m_size;
		mutable boost::shared_ptr<Packet::Data64Cell> m_data_cell;
	};

}	//	namespace game
#endif	//	#ifndef _data_cell_h
