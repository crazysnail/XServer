#include "network.h"

using namespace zxero;
using namespace zxero::network;

string g_message;

void onConnection(const tcp_connection_ptr& conn)
{
	if (conn->connected())
	{
		conn->send(g_message);
	}
}

void onMessage(const tcp_connection_ptr& conn, buffer* buf, timestamp_t)
{
	conn->send(buf);
}

int main()
{
	startup();

	for (int i = 0; i < 16 * 1024; ++i)
	{
		g_message.push_back(i % 128);
	}

	event_loop loop;
	event_loop_thread_pool pool(&loop);
	inet_address serverIp("127.0.0.1", 9981);

	pool.start(1);

	vector< shared_ptr<tcp_client> > tcp_client_list;
	for (int i = 0; i < 1000; ++i)
	{
		char buf[32];
		sprintf_s(buf, sizeof buf, "C%05d", i);
		shared_ptr<tcp_client> c(new tcp_client(pool.next_loop(), serverIp, buf));
		c->connection_callback(onConnection);
		c->message_callback(onMessage);
		c->connect();
		tcp_client_list.push_back(c);
	}

	loop.loop();
	return 0;
}

