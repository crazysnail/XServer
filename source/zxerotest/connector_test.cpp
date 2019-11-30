#include "connector.h"
#include "event_loop.h"
#include "network.h"

using namespace zxero;
using namespace network;

void on_connect(tcp_socket_ptr&)
{
	std::cout << "connected" << std::endl;
}

int main(void)
{
	startup();

	event_loop loop;
	inet_address s("127.0.0.1", 9981);
	shared_ptr<connector> c(new connector(&loop, s));
	c->new_connection_callback(on_connect);
	c->start();

	loop.loop();

	return 0;
}
