#pragma once

#include <string>

#ifdef _WIN32
# define UNIT_TEST_EXTERN __declspec(dllexport)
#else
# define UNIT_TEST_EXTERN
#endif

namespace boost
{
	namespace unit_test
	{
		class unit_test_log_formatter;
	}
};

typedef boost::unit_test::unit_test_log_formatter LogFormatter;

int UNIT_TEST_EXTERN ExternalRunStr(const std::string &aArgs, LogFormatter *aForm = 0);
int UNIT_TEST_EXTERN ExternalRunMain(int aArgc, char* aArgv[], LogFormatter *aForm = 0);

