#include "network.h"
#include "protobuf_codec.h"
#include "protobuf_dispatcher2.h"
#include "Scott.pb.h"

using namespace zxero;
using namespace zxero::network;

void onMessage(const tcp_connection_ptr& conn, const message_ptr& msg, timestamp_t t);
void on_default_message_callback(const tcp_connection_ptr& conn, const message_ptr& msg, timestamp_t t);

static protobuf_codec g_codec(true, onMessage);
static event_loop g_loop;
static protobuf_dispatcher2 g_dispatcher(&g_loop, on_default_message_callback);

void onConnection(const tcp_connection_ptr& conn)
{
	if (conn && conn->connected())
	{
		//auto req = protobuf_codec::create_message<Scott::LoginRequest>();
		//req->set_account("test");
		//req->set_password("zddflkedslk");
		//req->set_version("1.0.0");

		//g_codec.send(conn, *req);
	}
}

void onMessage(const tcp_connection_ptr& conn, const message_ptr& msg, timestamp_t t)
{
	//std::cout << "get msg " << msg->GetTypeName() << std::endl;
	//g_codec.send(conn, *msg);

	//	分发出去
	g_dispatcher.on_message(conn, msg, t);
}

void on_default_message_callback(const tcp_connection_ptr& conn, const message_ptr& msg, timestamp_t t)
{
	//	nothing to be done
}

///
void on_LoginRequest(const tcp_connection_ptr& conn, const shared_ptr<Scott::LoginRequest>& msg, timestamp_t t)
{
	std::cout << "user " << msg->account() << " from " << conn->peer_address().ip_port() << " requrie login" << std::endl;
	auto m = protobuf_codec::create_message<Scott::LoginReply>();
	m->set_result(0);
	g_codec.send(conn, *m);
}

///

int main()
{
	startup();

	g_dispatcher.register_handler<Scott::LoginRequest>(on_LoginRequest);

	tcp_client tc(&g_loop, inet_address("127.0.0.1", 9981), "protobuf_test_client");
	tc.connection_callback(onConnection);
	tc.message_callback(bind(&protobuf_codec::on_message, &g_codec, _1, _2, _3));
	tc.enable_retry();

	tc.connect();

	g_loop.loop();

	return 0;
}
