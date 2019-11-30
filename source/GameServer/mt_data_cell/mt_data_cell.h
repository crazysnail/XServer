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
		// init�������������������ݣ���������reset
		void init();			
		//����
		void reset();

		//���һ�����
		void add_data(int32 data);
		//���һ��cell�Ƿ��Ѿ�����
		bool is_full();
		//���data�Ƿ������cell��
		bool check_data( int32 data );
		//���ݻ�ȡ
		int32 get_data(int32 ud_index);
		//��������
		bool set_data(int32 ud_index, int32 data);
		//ȡ��cell��ǰ���ݸ���
		int32 get_size() const{
			return m_size;
		}
		Packet::CellLogicType get_type();

		boost::shared_ptr<Packet::DataCell> db_data() const  {
			return m_data_cell;
		}
	public:
		//size�Ǹ���������datacell�õģ�ͨ��addXXX�ӿڷ����仯��
		//setXXX�ӿ��ǲ���Ӱ���ֵ�ģ�������ܻ����������
		//���ǵ�һ��setXXX��datacell����˲������ľͻᵼ�����ݻ��ң�
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
		// init�������������������ݣ���������reset
		void init();
		//����
		void reset();

		//���һ�����
		void add_data(int64 data);
		//���һ��cell�Ƿ��Ѿ�����
		bool is_full();
		//���data�Ƿ������cell��
		bool check_data(int64 data);
		//���ݻ�ȡ
		int64 get_data(int32 ud_index);
		//��������
		bool set_data(int32 ud_index, int64 data);
		//ȡ��cell��ǰ���ݸ���
		int32 get_size() const {
			return m_size;
		}

		Packet::Cell64LogicType get_type();

		boost::shared_ptr<Packet::Data64Cell> db_data() const {
			return m_data_cell;
		}
	public:
		//size�Ǹ���������datacell�õģ�ͨ��addXXX�ӿڷ����仯��
		//setXXX�ӿ��ǲ���Ӱ���ֵ�ģ�������ܻ����������
		//���ǵ�һ��setXXX��datacell����˲������ľͻᵼ�����ݻ��ң�
		int32 m_size;
		mutable boost::shared_ptr<Packet::Data64Cell> m_data_cell;
	};

}	//	namespace game
#endif	//	#ifndef _data_cell_h
