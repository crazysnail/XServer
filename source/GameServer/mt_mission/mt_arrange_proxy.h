#ifndef MTONLINE_GAMESERVER_MT_ARANGE_PROXY_H__
#define MTONLINE_GAMESERVER_MT_ARANGE_PROXY_H__
#include "../mt_types.h"
#include <vector>


namespace Mt
{
	class MtArrangeProxy : public boost::noncopyable
	{
	public:
		static void lua_reg(lua_State* ls);
	public:
		MtArrangeProxy(){
			ZXERO_ASSERT(false) << "warning!!!!";
		}
		MtArrangeProxy(MtPlayer& player);
		~MtArrangeProxy() {};

		bool OnAddItem(const int32 configid, const int32 count=1);
		bool OnArrangeTo();
		
		void SetMissionId(const int32 mission_id) {
			mission_id_ = mission_id;
		}
		bool OnRollArrange();
		bool ConfirmRollItem( uint64 item_guid, const uint64 player_guid);
		bool CancelArrangeItem( const uint64 player_guid);
		bool SetTo( uint64 item_guid, const uint64 player_guid);
		void RefreshData2Client();
		void UpdateArrangeInfo();
		bool IsArrangeBagFull();
		void OnArrangeBegin(const std::vector<uint64> & filter_list);
		void OnArrangeEnd();
		void OnTick( uint64 elapseTime);
		void OnClearContainer();
		boost::shared_ptr<MtContainer > GetContainer() { return container_; };

	private:
		bool ArrangeTo(const uint64 item_guid, const uint64 player_guid);
		void CheckLostArrange(const std::vector<uint64> & members);
		bool RollCheck(const uint64 player_guid);
		bool RollPoint(const uint64 item_guid, const uint64 player_guid);
		bool IsAllRollOver();
		bool RollItem(uint64 item_guid, const uint64 player_guid);

		void DoArrange(std::vector<uint64>& un_arrange_item);
		void DoLostArrange(std::vector<uint64>& un_arrange_item);
		void DoLastArrange(std::vector<uint64>& un_arrange_item);

	private:

		boost::shared_ptr<MtContainer > container_;
		const boost::weak_ptr<MtPlayer> leader_;
		int64 wait_timer_ = 0;

		//来源
		int32 mission_id_ = 0;

		std::map<uint64, std::map<uint64, int32>> roll_points_;
		std::map<uint64, uint64> arrange_list_;

		std::vector<uint64> lost_players_;	 //没分配到的
		std::vector<uint64> last_chance_;	 //参与最后分配
		std::vector<uint64> join_players_;	 //参与
		std::vector<uint64> filter_list_;	 //放弃+不能

	};

}
#endif // MTONLINE_GAMESERVER_MT_MISSION_H__
