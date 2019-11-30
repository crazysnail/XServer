#include "event_loop.h"
#include "network.h"

using namespace zxero;
using namespace network;

void on_connection(const network::tcp_connection_ptr& conn)
{
	std::cout << "on_connection get new connection ["
		<< conn->name() << "] from "
		<< conn->peer_address().ip_port()
		<< std::endl;
}

void on_message(const network::tcp_connection_ptr& conn, buffer* buffer, timestamp_t overtime)
{
	std::cout << "on_message received " << buffer->readable_bytes() << " bytes from connection [" << conn->name() << "]" << std::endl;
	conn->send(buffer);
}

int main(void)
{
	startup();

	event_loop loop;
	inet_address s("127.0.0.1", 9981);
	tcp_client c(&loop, s, "test");
	c.connection_callback(on_connection);
	c.message_callback(on_message);
	c.enable_retry();
	c.connect();

	loop.loop();

	return 0;
}
