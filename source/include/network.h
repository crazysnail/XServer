//	网络库
//	要封装eventloop和thread么？
#ifndef _zxero_network_h_
#define _zxero_network_h_


namespace zxero
{
	namespace network
	{
		//	启动网络功能
		int startup();

		//	关闭网络功能
		void shutdown();
	}
}

#endif	//	#ifndef _zxero_network_h_
