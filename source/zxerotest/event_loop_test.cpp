#include "app.h"
#include "module.h"
#include "thread.h"
#include "network.h"
#include "channel.h"

using namespace zxero;

class test_event_loop
{
	thread_ptr_t	one_loop_;
	thread_ptr_t	two_loop_;

	network::event_loop*		loop_;

	void timeout()
	{
		std::cout << "timeout" << std::endl;
		loop_->quit();
	}

	void loopFunction()
	{
		std::cout << "loop function : pid = " << boost::this_thread::get_id() << std::endl;

		network::event_loop loop;
		loop_ = &loop;

		static int port = 911;

		network::inet_address a(port++);
		network::tcp_socket t(INVALID_SOCKET);
		t.bind(a);
		t.nonblocking(true);
		t.listen();

		network::channel c(&loop, t.fd());
		c.read_callback(bind(&test_event_loop::timeout, this));
		c.enable_reading();
		c.enable_writing();
		c.disable_writing();

		loop.loop();
		//loop_->loop();
	}

public:
	int onLoad()
	{
		network::startup();

		one_loop_ = boost::make_shared< thread_t >(boost::bind(&test_event_loop::loopFunction, this), "one loop");
		//two_loop_ = boost::make_shared< thread_t >(boost::bind(&test_event_loop::loopFunction, this), "two loop");

		one_loop_->start();
		//two_loop_->start();

		return 0;
	}

	void onUnload()
	{
		one_loop_->join();
		//two_loop_->join();

		network::shutdown();
	}
};
REGISTER_MODULE(test_event_loop)
{
	register_load_function(STAGE_LOAD, boost::bind(&test_event_loop::onLoad, this));
	register_unload_function(STAGE_UNLOAD, boost::bind(&test_event_loop::onUnload, this));
}

int main(int argc, char** argv)
{
	return zxero::app::main(argc, argv);
}

