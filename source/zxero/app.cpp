#include "app.h"
#include "module.h"
#include "log.h"
#include "signal.h"
#include "../zxero/mem_debugger.h"

#include "google/protobuf/stubs/common.h"

#ifdef ZXERO_OS_WINDOWS
//#include <vld.h> 
#endif

namespace zxero
{
	//
	namespace app
	{
		//	主线程工作
		static main_work_t sg_main_work;
		
		static std::function<void()> sg_exit_callback;

		static bool sg_exiting = false;

		//	系统退出代码
		static int sg_exit_code = 0;

		//	系统工作标识
		static bool sg_app_running = true;

		void sigint_handler(int /*sig*/) 
		{
			signal(SIGINT, sigint_handler);
			printf("control-c got , exiting.\n");
			
			if (sg_exiting == false) {
				sg_exiting = true;
				if (sg_exit_callback) {
					sg_exit_callback();
				} else {
					sg_app_running = false;
				}
			} else {
				printf("signal int got. already in exit progress");
			}
		}
		void abort_handler(int sig)
		{
#ifdef ZXERO_OS_LINUX
			if(sig == SIGPIPE)
			{
				LOG_INFO<<"system catch signal"<<sig<<"will be ignore";
				return;
			}
#endif

			LOG_INFO<<"system crash with signal:"<<sig<<".system will closed!";
			

			//crash todo !
			ZXERO_ASSERT(false);
			

			signal(sig,SIG_DFL);
		}
		//
		int main(int argc, char** argv)
		{

			{
				signal(SIGABRT,abort_handler);
				signal(SIGFPE,abort_handler);
				signal(SIGILL,abort_handler);
				signal(SIGINT, sigint_handler);
				signal(SIGSEGV,abort_handler);
				signal(SIGTERM, sigint_handler);
#if ZXERO_OS_LINUX
				signal(SIGPIPE,abort_handler);
#endif
			}
			LOG_INFO << "main start";
			//	执行模块初始化操作
			int ret = module_base::init(argc, argv);
			if (ret != 0)
			{
				LOG_ERROR << "module init failed, error code " << ret;
				if (ret == -1)
					ret = 0;

				return ret;
			}

			//	预初始化操作
			//	初始化操作
			//	初始化后操作
			LOG_INFO << "module load start";
			ret = module_base::load();
			if (ret != 0)
			{
				LOG_ERROR << "module load failed, error code " << ret;
				return ret;
			}
			LOG_INFO << "module load finish";

			//	运行之
			if (is_running() && sg_main_work)
			{
				LOG_INFO << "main work start";
				sg_main_work();
				LOG_INFO << "main work done";
			}
			//	预退出操作
			//	退出操作
			//	退出后操作
			LOG_INFO << "module unload start";
			module_base::unload();
			LOG_INFO << "module unload finish";

			google::protobuf::ShutdownProtobufLibrary();

			mem_debugger::instance()->dump_unfreed("debug_memory.txt");
			//_CrtDumpMemoryLeaks();
			LOG_INFO << "app exit with code " << sg_exit_code;
			return sg_exit_code;
		}

		//
		bool is_running()
		{
			return sg_app_running;
		}

		//
		void exit(int exit_code)
		{
			sg_exit_code = exit_code;
			sg_app_running = false;
		}

		//
		void set_main_work(const main_work_t& main_work)
		{
			sg_main_work = main_work;
		}

		void set_exit(const std::function<void()>& exit_callback)
		{
			sg_exit_callback = exit_callback;
		}

	}	//	namespace app

}	//	namespace zxero
