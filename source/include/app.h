//	应用程序辅助
#ifndef _zxero_application_h_
#define _zxero_application_h_

#include "types.h"
#include <boost/function.hpp>

namespace zxero
{
	//	不用单例类而用命名空间，算是对
	namespace app
	{
		//	默认主函数, 提供默认的函数执行方法
		//	相应的main可以直接写为
		//	int main( int argc, char** argv ) {
		//		return zXero::Application::main( argc, argv );
		//	}
		int main(int argc, char** argv);

		//	判断主程序是否运行的函数
		bool is_running();

		//	退出主程序
		void exit(int exit_code);

		//	主程序工作函数原型
		typedef boost::function< void() > main_work_t;

		//	设置主程序工作函数
		void set_main_work(const main_work_t& main_work);

		void set_exit(const std::function<void()>& exit_callback);
	}	//	namespace app;

}	//	namespace zxero


#endif //	#ifndef _zxero_application_h_
