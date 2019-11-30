#include "network.h"

using namespace zxero;
using namespace zxero::network;

void onConnection(const tcp_connection_ptr& conn)
{
	if (conn->connected())
	{
		conn->no_delay(true);
	}
}

void onMessage(const tcp_connection_ptr& conn, buffer* buf, timestamp_t)
{
	conn->send(buf);
}

int main()
{
	startup();

	event_loop loop;
	tcp_server server(&loop, inet_address(9981), "pingpong");

	server.connection_callback(onConnection);
	server.message_callback(onMessage);

	server.start(1);

	loop.loop();

	return 0;
}
