#pragma once

#include <string>

namespace boost
{
	namespace unit_test
	{
		class unit_test_log_formatter;
		class test_suite;
	}
};

namespace test
{
	typedef boost::unit_test::unit_test_log_formatter LogFormatter;
	typedef boost::unit_test::test_suite* (*InitTestFunc)( int, char* [] );

	int DoExternalRunStr(InitTestFunc, const std::string &aArgs, LogFormatter *aForm);
	int DoExternalRunMain(InitTestFunc, int aArgc, char* aArgv[], LogFormatter *aForm);
}

struct TestGlobalFixture
{
	TestGlobalFixture();
	~TestGlobalFixture();
};

