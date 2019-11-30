#ifndef __friend_container_h__
#define __friend_container_h__

#include "../mt_types.h"
#include "FriendMail.pb.h"
namespace Mt
{
	using namespace zxero;
	enum friend_db_type : int32
	{
		FRIEND = 0,
		BACK = 1,
		APPLY = 2,
		REQUEST = 3,
	};

	class FriendContainer : public boost::noncopyable, public boost::enable_shared_from_this<FriendContainer>
	{
	public:
		FriendContainer() { LOG_ERROR << "should not create"; }
		FriendContainer(MtPlayer& player);
		~FriendContainer();
		static void lua_reg(lua_State* ls);
		void reset();
	public: 
		void fill_protocol(Packet::FriendInfotReply &reply);
		void fill_friend_protocol(Packet::FriendListUpdate &friends);
		void ask_recommend_req(Packet::RecommendsUpdate &update);
	public:
		void OnLoad(const boost::shared_ptr<zxero::dbcontext>& dbctx);
		void OnCache();
		void OnNewDay();
	public:
		//
		bool isFriend(uint64 playerguid);
		bool isBlack(uint64 playerguid);
		//������Ӻ���[�ѶԷ���ӵ��Լ��ĺ��Ѷ���,����һ��message���Է�]
		Packet::ResultCode apply_add_friend(uint32 player_max_friend_count, uint64 playerguid, const Packet::PlayerBasicInfo* info);
		//ɾ������
		Packet::ResultCode del_friend_request(uint64 friendguid);
		//�Ѻ������������
		Packet::ResultCode add_black_friend(uint64 playerguid);
		//�Ӻ������Ƴ�
		//Packet::FriendOptReply::Result move_back(uint64 playerguid);

		//void update_gifttime(uint64 firendguid,uint64 gifttime);
		//�µ�һ��
		void refresh_newday();
		const std::vector<boost::shared_ptr<Packet::FriendPlayer>> FriendList()
		{
			return friendlist_;
		}
	private:
		std::vector<boost::shared_ptr<Packet::FriendPlayer>> friendlist_;	//���Ѷ���
		std::vector<uint64> m_backlist;	//������
		const boost::weak_ptr<MtPlayer> player_;
		boost::shared_ptr<Packet::ZoneInfo> zone_data_;
		std::vector<boost::shared_ptr<Packet::MessageBoard>> messageboardlist_;
	};
}

#endif