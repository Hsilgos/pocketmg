#pragma once

#include <string>

namespace boost {
namespace unit_test {
class unit_test_log_formatter;
class test_suite;
}
};

namespace test {
typedef boost::unit_test::unit_test_log_formatter LogFormatter;
typedef boost::unit_test::test_suite* (*InitTestFunc)( int, char* [] );

int DoExternalRunStr(InitTestFunc, const std::string &args, LogFormatter *form);
int DoExternalRunMain(InitTestFunc, int argc, char* argv[], LogFormatter *form);
}

struct TestGlobalFixture {
  TestGlobalFixture();
  ~TestGlobalFixture();
};

