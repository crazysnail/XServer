#include "app.h"
#include "module.h"
#include "network.h"
#include "acceptor.h"


using namespace zxero;

class test_module
{
private:
	network::event_loop main_loop_;
	shared_ptr<network::acceptor> acceptor;

public:
	int onLoad()
	{
		network::inet_address listen_addr(911);
		acceptor = make_shared<network::acceptor>(&main_loop_, listen_addr);
		acceptor->listen();

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


