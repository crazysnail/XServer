#include "types.h"
#include "log.h"


#define BOOST_TEST_MAIN
#include <boost/test/unit_test.hpp>

using namespace zxero;

BOOST_AUTO_TEST_CASE(testLogStreamBooleans)
{
	log_message::log_level(LL_TRACE);
	LOG_TRACE << false;
	LOG_TRACE << true;
	LOG_TRACE << (char)1;
	LOG_TRACE << -1;
	LOG_TRACE << -1.0f;
	LOG_TRACE << -1.0;
	LOG_TRACE << string("abc");
	LOG_TRACE << std::string("abcdef");
	LOG_TRACE << (int16)65535;
	LOG_TRACE << (uint64)-1;
}
