#ifndef ZXERO_CHATMESSAGE_H__
#define ZXERO_CHATMESSAGE_H__


#include "../mt_types.h"

namespace Mt
{
	using namespace zxero;
	void chatmessage_lua_reg(lua_State* ls);
	void scene_chat_message(const boost::shared_ptr<MtScene> scene,
		boost::shared_ptr<MtPlayer> player,
		const boost::shared_ptr<Packet::ChatMessage>& message);
	int32 send_message_byplayer(Packet::ChatChannel channel, const std::string &chatmessage, int32 chat_time, const boost::shared_ptr<MtPlayer>& player);
	boost::shared_ptr<Packet::ChatMessage> make_player_msg(Packet::ChatChannel channel, const std::string &chatmessage, int32 chat_time, const boost::shared_ptr<MtPlayer>& player);
	boost::shared_ptr<Packet::ChatMessage> make_normal_msg(Packet::ChatChannel channel, const std::string &chatmessage);
	void fill_message_hyper(const std::string &chatmessage, boost::shared_ptr<Packet::ChatMessage>& message, const boost::shared_ptr<MtPlayer>& player);
	void send_system_message_toworld(const std::string &chatmessage);
	void send_system_message_toplayer(const std::string &chatmessage, const boost::shared_ptr<MtPlayer> player);
	void send_team_system_message(const std::string &chatmessage,uint64 teamid);
	void send_guild_system_message(const std::string &chatmessage, uint64 guildid);
	void send_run_massage(const std::string &chatmessage);
	void gm_command(const boost::shared_ptr<MtScene> scene,
		const boost::shared_ptr<MtPlayer> player, 
		std::string msg);
}
#endif // ZXERO_CHATMESSAGE_H__