#include "app.h"
#include "module.h"
#include "network.h"


using namespace zxero;

class test_module
{
private:
	shared_ptr<network::event_loop_thread_pool> iopool_;
	network::event_loop main_loop_;

public:
	int onLoad()
	{
		iopool_ = make_shared<network::event_loop_thread_pool>(main_loop_);
		iopool_->start(60);

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
