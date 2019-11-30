//	Ӧ�ó�����
#ifndef _zxero_application_h_
#define _zxero_application_h_

#include "types.h"
#include <boost/function.hpp>

namespace zxero
{
	//	���õ�������������ռ䣬���Ƕ�
	namespace app
	{
		//	Ĭ��������, �ṩĬ�ϵĺ���ִ�з���
		//	��Ӧ��main����ֱ��дΪ
		//	int main( int argc, char** argv ) {
		//		return zXero::Application::main( argc, argv );
		//	}
		int main(int argc, char** argv);

		//	�ж��������Ƿ����еĺ���
		bool is_running();

		//	�˳�������
		void exit(int exit_code);

		//	������������ԭ��
		typedef boost::function< void() > main_work_t;

		//	����������������
		void set_main_work(const main_work_t& main_work);

		void set_exit(const std::function<void()>& exit_callback);
	}	//	namespace app;

}	//	namespace zxero


#endif //	#ifndef _zxero_application_h_
