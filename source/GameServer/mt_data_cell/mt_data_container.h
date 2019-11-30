#ifndef _game_sotcc_data_container_h_
#define _game_sotcc_data_container_h_

#include "../mt_types.h"
#include "utils.h"
#include "AllPacketEnum.pb.h"
#include "AllConfigEnum.pb.h"
namespace Mt
{		
	class MtCellContainer : public boost::noncopyable
	{
	public:
		static void lua_reg(lua_State* ls);
		
	public:
		MtCellContainer() {};
		MtCellContainer(MtPlayer& player);
		virtual ~MtCellContainer();

	public:

		//��������,ud_index���ⲿ������±�ֵ
		bool lua_set_data_32(Packet::CellLogicType logic_type, int32 ud_index, int32 data, bool refresh2client) {
			return set_data_32(logic_type, ud_index, data, refresh2client);
		}
		bool lua_set_data_64(Packet::Cell64LogicType logic_type, int32 ud_index, int64 data, bool refresh2client){
			return set_data_64(logic_type, ud_index, data, refresh2client);
		}
		bool lua_add_data_32(Packet::CellLogicType type, int32 index, int32 data, bool refresh2client)
		{
			return add_data_32(type, index, data, refresh2client);
		}
		bool lua_add_data_64(Packet::Cell64LogicType type, int32 index, int64 data, bool refresh2client)
		{
			return add_data_64(type, index, data, refresh2client);
		}

		bool  lua_check_bit_data_32(int32 source, int32 bit) {
			return utils::bit_check(source, bit);
		}
		int32 lua_set_bit_data_32(int32 source, int32 bit) {
			utils::bit_set(source, bit);
			return source;
		}
		int32 lua_reset_bit_data_32(int32 source, int32 bit) {
			utils::bit_reset(source, bit);
			return source;
		}
		bool  lua_check_bit_data_64(int64 source, int32 bit) {
			return utils::bit_check(source, bit);
		}
		int64 lua_set_bit_data_64(int64 source, int32 bit) {
			utils::bit_set(source, bit);
			return source;
		}
		int64 lua_reset_bit_data_64(int64 source, int32 bit) {
			utils::bit_reset(source, bit);
			return source;
		}
		int32 get_high_16_bit(int32 source) {
			return (source & 0xffff0000)>>16;
		}
		int32 get_low_16_bit(int32 source) {
			return source & 0x0000ffff;
		}
		int64 get_high_32_bit(int64 source) {
			return (source & 0xffffffff00000000)>>32;
		}
		int64 get_low_32_bit(int64 source) {
			return source & 0x00000000ffffffff;
		}
		int32 bind_bit_data_32(int32 high, int32 low) {
			return ((high << 16) & 0xffff0000) | (low & 0x0000ffff);
		}
		int64 bind_bit_data_64(int64 high, int64 low) {
			return ((high << 32) & 0xffffffff00000000) | (low & 0x00000000ffffffff);
		}

		int64 gen_cell32_guid(Packet::CellLogicType logic_type, int32 sub_index) {
			//int64 guid = bind_bit_data_64(logic_type, index);
			//int64 guid = (int64)bind_bit_data_32(logic_type, index);
			//ע�⣡���ﲻ����bind_bit_data_64��bind_bit_data_32����Ϊcache��ʱ��MtMigrate::GetMsgUniqueValue����˫��msg��
			//������guid�ĸ�32+8λ����
			//����logictype���ݽ׶Σ�index��Ϊ��32�����Ա�ʶ��ͬlogictype���͵�datacell�����յ���datacell���ݴ洢����
			//���ֵ���ܳ���2^24=16777216
			return (logic_type * 10000 + sub_index);
		}
		int64 gen_cell64_guid(Packet::Cell64LogicType logic_type, int32 sub_index) {
			//int64 guid = bind_bit_data_64(logic_type, index);
			//int64 guid = (int64)bind_bit_data_32(logic_type, index);
			//ע�⣡���ﲻ����bind_bit_data_64��bind_bit_data_32����Ϊcache��ʱ��MtMigrate::GetMsgUniqueValue����˫��msg��
			//������guid�ĸ�32+8λ����
			//����logictype���ݽ׶Σ�index��Ϊ��32�����Ա�ʶ��ͬlogictype���͵�datacell�����յ���datacell���ݴ洢����
			//���ֵ���ܳ���2^24=16777216
			return (logic_type * 10000 + sub_index);
		}

		////////////////////////////
		bool try_init_int32_data(Packet::CellLogicType logic_type, int32 ud_index, int32 data);
		bool try_init_int64_data(Packet::Cell64LogicType logic_type, int32 ud_index, int64 data);
		// 
		void add_cell_32(boost::shared_ptr<MtDataCell> & cell);
		void add_cell_64(boost::shared_ptr<MtData64Cell> & cell);


		//���벻�ظ�������
		bool push_data_32(Packet::CellLogicType logic_type, int32 data, bool refresh2client);
		bool push_data_64(Packet::Cell64LogicType logic_type, int64 data, bool refresh2client);
		//���ݼ��
		bool check_data_32(Packet::CellLogicType logic_type, int32 data);
		bool check_data_64(Packet::Cell64LogicType logic_type, int64 data);

		//���ݻ�ȡ,ud_index���ⲿ������±�ֵ
		int32 get_data_32(Packet::CellLogicType logic_type, int32 ud_index);
		int64 get_data_64(Packet::Cell64LogicType logic_type, int32 ud_index);

		//////////////
		bool set_data_32(Packet::CellLogicType logic_type, int32 ud_index, int32 data, bool refresh2client=false);
		bool set_data_64(Packet::Cell64LogicType logic_type, int32 ud_index, int64 data, bool refresh2client = false);

		//���ݻ�ȡ,�õ�����ָ�����͵�����ֵ
		void get_all_data_32(Packet::CellLogicType logic_type, std::vector<int32> &data);
		void get_all_data_64(Packet::Cell64LogicType logic_type, std::vector<int64> &data);
		uint32 get_count_data_32(Packet::CellLogicType logic_type);
		uint32 get_count_data_64(Packet::Cell64LogicType logic_type);

		//�����������,ud_index���ⲿ������±�ֵ
		bool add_data_32(Packet::CellLogicType logic_type,int32 ud_index, int32 data, bool refresh2client = false);
		bool add_data_64(Packet::Cell64LogicType logic_type, int32 ud_index, int64 data, bool refresh2client = false);

		//�洢�ӿ�
		void OnSave(const boost::shared_ptr<zxero::dbcontext>& dbctx);
		void OnCache();


		//λ��Ǽ���
		bool check_bit_data(Packet::BitFlagCellIndex ud_index);
		bool set_bit_data(Packet::BitFlagCellIndex ud_index, bool refresh2client = false);
		bool reset_bit_data(Packet::BitFlagCellIndex ud_index, bool refresh2client = false);

		//����ͬ��
		void DataCellListUpdate() ;
		void DataCell64ListUpdate() ;
		void DataCellValueUpdate(Packet::CellLogicType logic_type, int32 ud_index, int32 data) ;
		void DataCellValueUpdate(Packet::Cell64LogicType logic_type, int32 ud_index, int64 data) ;
		void DataCellUpdate(const boost::shared_ptr<MtDataCell> datacell) ;
		void DataCellUpdate(const boost::shared_ptr<MtData64Cell> datacell) ;
		void SingleDataCellListUpdate(Packet::CellLogicType logic_type) ;
		void SingleDataCell64ListUpdate(Packet::Cell64LogicType logic_type) ;

	private:
		bool align_size_32(Packet::CellLogicType logic_type,int32 size );
		bool align_size_64(Packet::Cell64LogicType logic_type, int32 size);
		uint32 calc_cell_count_32(Packet::CellLogicType logic_type);
		uint32 calc_cell_count_64(Packet::Cell64LogicType logic_type);

		//ͨ������ȡ�ÿ��õ�cell����SAFE_NEW
		boost::shared_ptr<MtDataCell> get_useable_cell_32(Packet::CellLogicType logic_type);
		boost::shared_ptr<MtData64Cell> get_useable_cell_64(Packet::Cell64LogicType logic_type);

	private:
		std::vector<boost::shared_ptr<MtDataCell>> m_datacell_pool[Packet::CellLogicType_ARRAYSIZE];
		std::vector<boost::shared_ptr<MtData64Cell>> m_data64cell_pool[Packet::Cell64LogicType_ARRAYSIZE];

		const boost::weak_ptr<MtPlayer> player_;
	};
}
#endif //_game_sotcc_data_container_h_
