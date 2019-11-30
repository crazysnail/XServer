//	zXero基础类型头文件
#ifndef _zxero_types_h_
#define _zxero_types_h_
//#define _CRTDBG_MAP_ALLOC 
//#include <crtdbg.h>
//	开启assert处理
#define BOOST_ENABLE_ASSERT_HANDLER 1
#include <boost/assert.hpp>
// 包含一些常用的boost头文件
#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <boost/noncopyable.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/make_shared.hpp>
#include <boost/any.hpp>
#include <boost/variant.hpp>

#define __STDC_FORMAT_MACROS
#include <inttypes.h>

//	系统识别
#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
	#define ZXERO_OS_WINDOWS 1

	#if defined(WIN64) || defined(_WIN64)
		#define ZXERO_ARCH_x86_64	1
	#else
		#define ZXERO_ARCH_x86	1
	#endif
#elif defined(__linux__)
//	目前就两个系统，一个linux，一个windows
//	linux 默认64位
#define ZXERO_OS_LINUX 1
#define ZXERO_ARCH_x86_64 1
#elif defined(__MAXOSX__)
//  mac系统
#define ZXERO_OS_MACOSX 1
#define ZXERO_ARCH_x86_64 1
#endif

//	系统区别的类型定义
#ifdef ZXERO_OS_WINDOWS
//	for windows

#define NOMINMAX 1

//#ifdef _WIN32_FDAPI_H
//#define INCL_WINSOCK_API_PROTOTYPES 0 
//#include <WinSock2.h>
//#include <Windows.h>
//#else
#ifdef INCL_WINSOCK_API_PROTOTYPES
#undef INCL_WINSOCK_API_PROTOTYPES
#endif

#include <WinSock2.h>
#include <Windows.h>
#pragma comment(lib,"ws2_32.lib")
//#endif

typedef SOCKET socket_t;
typedef int socklen_t;

#else
//	for linux
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>

#ifdef ZXERO_OS_LINUX
#include <sys/epoll.h>
#else
#ifndef POLLRDHUP
#define POLLRDHUP 0x2000
#endif
#endif

typedef int socket_t;

#define INVALID_SOCKET ((socket_t)-1)

#endif


#ifdef ZXERO_OS_WINDOWS
#define zprintf sprintf_s
#else
#define zprintf snprintf
#endif

#ifdef ZXERO_OS_WINDOWS
#define zstrtok strtok_s
#else
#define zstrtok strtok_r
#endif

namespace boost
{
	template <typename T>
	class function;
	template <typename T>
	class shared_ptr;
	namespace uuids
	{
		struct uuid;
	}
}

namespace google
{
	namespace protobuf
	{
		class Message;
		class Descriptor;
	}
}
namespace Mt
{
	enum MessageSource
	{
		INVALID = -1,
		CLIENT = 0, //客户端
		SCENE = 1, //场景
		GAME_EXECUTE = 2, //全局(主线程) 发往线程, 场景执行消息
		GAME_BROADCAST = 3, //全局(主线程)发往场景线程, 场景广播消息
	};
}

namespace zxero
{

	//	基本类型重定义
#ifdef  _MSC_VER	
	typedef  char int8;
	typedef  __int16 int16;
	typedef  __int32 int32;
	typedef  __int64 int64;	
	typedef  unsigned char uint8;
	typedef  unsigned __int16 uint16;
	typedef  unsigned __int32 uint32;
	typedef  unsigned __int64 uint64;
#else
	typedef int8_t  int8;
	typedef int16_t int16;
	typedef int32_t int32;
	typedef int64_t int64;

	typedef uint8_t  uint8;
	typedef uint16_t uint16;
	typedef uint32_t uint32;
	typedef uint64_t uint64;

#endif

	typedef float				real32;
	typedef double				real64;
	typedef int32				sm_obj_id; //sm_obj_id是共享内存对象索引

#ifdef ZXERO_ARCH_x86
	typedef _W64 int32			intptr;
	typedef _W64 uint32			uintptr;
#else
	typedef int64				intptr;
	typedef uint64				uintptr;
#endif
    
    //  工作预定义
    struct work;
    class work_thread;
    class work_pool;
	class buffer;
	class thread_t;
	class dbwork;
	class dbcontext;
    
	
	namespace network
	{
		class event_loop;
		class event_loop_thread;
		class event_loop_thread_pool;
		class channel;
		class poller;
		class tcp_socket;
		class tcp_connection;
		class tcp_client;
		class tcp_server;
        class tcp_client_pool;
		class select_poller;
		class inet_address;
		class acceptor;
		class connector;
		class timer;
		class timer_id;
		class timer_queue;
		class protobuf_codec;
		class http_codec;
	}

}	//	namespace zxero


#define INVALID_GUID  0x0
#define MAX_GUID 0x0fffffffffffffff
#define INVALID_UINT32   0xffffffff
#define INVALID_INT32  -1
#define MAX_INT32  0x7fffffff
#define INVALID_INT64  -1
#define INVALID_UINT64 -1

#include "timer_id.h"


#endif	//	#ifndef _zxero_types_h_
