#include "app.h"
#include "module.h"
#include "network.h"
#include "buffer.h"


using namespace zxero;

class test_module
{
private:
	network::event_loop main_loop_;
	boost::shared_ptr<network::tcp_server> server_;

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

public:
	int onLoad()
	{
		network::inet_address listen_addr(9981);
		server_ = make_shared<network::tcp_server>(&main_loop_, listen_addr, "client server");
		server_->connection_callback(boost::bind(&test_module::on_connection, this, _1));
		server_->message_callback(boost::bind(&test_module::on_message, this, _1, _2, _3));
		server_->start(50);

		return 0;
	}

	void main()
	{
		main_loop_.loop();
	}
};
REGISTER_MODULE(test_module)
{
	network::startup();

	register_load_function(STAGE_LOAD, bind(&test_module::onLoad, this));

	app::set_main_work(bind(&test_module::main, this));
}


int main(int argc, char** argv)
{
	return zxero::app::main(argc, argv);
}


