#include "app.h"
#include "module.h"
#include "thread.h"
#include "network.h"
#include <boost/date_time.hpp>

using namespace zxero;

typedef std::vector< std::string >	strings_t;
typedef std::vector< float > numbers_t;

network::inet_address local(911);
network::inet_address target("127.0.0.1", 911);

class test_module
{
	void run()
	{
		int ret = client_->connect(target);
		if (ret)
		{
			std::cerr << "client connect error " << errno << std::endl;
			return;
		}

		std::cout << "client connect to " << client_->peer_address().ip_port() << " from " << client_->local_address().ip_port() << std::endl;
		client_->keep_alive(true);
		client_->no_delay(true);
		std::cout << "client window s = " << client_->send_buff() << ", r = " << client_->send_buff() << std::endl;
		//client_->send_buff(20);
		//client_->recv_buff(20);
		//std::cout << "client window s = " << client_->send_buff() << ", r = " << client_->send_buff() << std::endl;
		//client_->nonblocking(true);
		char buf[64 * 1024];
		int64 totallen = 0;
		int len = 0;
		boost::posix_time::ptime start_time_point = boost::posix_time::second_clock::local_time();
		
		while ((len = client_->write(buf, 64 * 1024)) >= 0)
		{
			totallen += len;
			//len = client_->read(buf, 16 * 1024);
			//if (len <= 0)
			//	break;
			boost::posix_time::ptime now = boost::posix_time::second_clock::local_time();
			boost::posix_time::time_duration td = now - start_time_point;
			if (td.total_seconds() > 60)
			{
				std::cout << td << "s send data " << totallen << std::endl;
				break;
			}
		}
		client_->shutdown_write();
	}

	thread_ptr_t thread_;

	network::tcp_socket_ptr client_;
public:
	int onLoad()
	{
		client_ = make_shared<network::tcp_socket>(INVALID_SOCKET);

		std::cout << "local: " << local.ip_port() << std::endl;
		std::cout << "target: " << target.ip_port() << std::endl;

		std::cout << "test module load" << std::endl;
		thread_ = make_shared< thread_t >(bind(&test_module::run, this), "test_module_run");
		thread_->start();
		return 0;
	}

	void onUnload()
	{
		thread_->join();
		std::cout << "test module unload" << std::endl;
	}

	void onTestOption(const numbers_t& args)
	{
		//for (strings_t::const_iterator iter = args.cbegin(); iter != args.cend(); ++iter)
		for (auto str : args)	//	c++11的风格
		{
			std::cout << "test option : " << str << std::endl;
		}
	}

	void onCheckOption()
	{
		std::cout << "test checked" << std::endl;
	}
};

REGISTER_MODULE(test_module)
{
	//
	require("test_2_module");

	//	注册模块加载和卸载函数
	register_load_function(STAGE_LOAD, boost::bind(&test_module::onLoad, this));
	register_unload_function(STAGE_UNLOAD, boost::bind(&test_module::onUnload, this));

	//	注册选项及其处理函数
	register_option("test,t", "test option",
		boost::program_options::value< numbers_t >()->composing()->multitoken(),
		opt_delegate< numbers_t >(boost::bind(&test_module::onTestOption, this, _1)));

	register_option("check,c", "check option",
		opt_void_delegate(boost::bind(&test_module::onCheckOption, this)));
}

class test_2_module
{
	void run()
	{
		network::tcp_socket_ptr client = server_->accept();
		if (!client)
		{
			std::cerr << "server accept client failed" << std::endl;
		}
		else
		{
			std::cout << "accept connection from " << client->peer_address().ip_port() << std::endl;
			std::cout << "target window s = " << client->send_buff() << ", r = " << client->send_buff() << std::endl;

			char buf[128*1024];
			int len = 0;
			while (len = client->read(buf, 128 * 1024))
			{
				//client->write(buf, len);
			}
			std::cout << "target accept size " << len << std::endl;
		}
	}

	thread_ptr_t thread_;

	network::tcp_socket_ptr server_;
public:
	int onPreLoad()
	{
		return network::startup();
	}
	int onLoad()
	{
		server_ = make_shared<network::tcp_socket>(INVALID_SOCKET);
		server_->bind(local);
		server_->listen();

		std::cout << "test 2 module load" << std::endl;
		thread_ = make_shared< thread_t >(bind(&test_2_module::run, this), "test_2_module_run");
		thread_->start();
		return 0;
	}

	int onPostLoad()
	{
		thread_->join();
		std::cout << "test 2 module post load" << std::endl;
		return 0;
	}

	void onPostUnload()
	{
		network::shutdown();
		std::cout << "test 2 module post unload" << std::endl;
	}

	void onTestOption(const numbers_t& args)
	{
		//for (strings_t::const_iterator iter = args.cbegin(); iter != args.cend(); ++iter)
		for (auto str : args)	//	c++11的风格
		{
			std::cout << "test 2 option : " << str << std::endl;
		}
	}

	void onCheckOption()
	{
		std::cout << "test 2 checked" << std::endl;
	}
};

REGISTER_MODULE(test_2_module)
{
	register_load_function(STAGE_PRE_LOAD, boost::bind(&test_2_module::onPreLoad, this));
	register_load_function(STAGE_LOAD, boost::bind(&test_2_module::onLoad, this));
	register_load_function(STAGE_POST_LOAD, boost::bind(&test_2_module::onPostLoad, this));
	register_unload_function(STAGE_UNLOAD, boost::bind(&test_2_module::onPostUnload, this));

	//	注册选项及其处理函数
	register_option("test,t", "test option",
		boost::program_options::value< numbers_t >()->composing()->multitoken(),
		opt_delegate< numbers_t >(boost::bind(&test_2_module::onTestOption, this, _1)));

	register_option("check,c", "check option",
		opt_void_delegate(boost::bind(&test_2_module::onCheckOption, this)));
}

int main(int argc, char** argv)
{
	return zxero::app::main(argc, argv);
}
