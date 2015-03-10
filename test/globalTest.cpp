#include "testName.h"

#include "globalTest.h"

#include <boost/test/included/unit_test.hpp>

#include "testFormatter.h"


#if defined( __i386__ ) && defined ( __ANDROID__ )

#include <setjmp.h>

void siglongjmp(sigjmp_buf env, int val) {
  longjmp(env, val);
}

int sigsetjmp(sigjmp_buf env, int savemask) {
  return setjmp(env);
}

#endif

namespace test {
BOOST_GLOBAL_FIXTURE( TestGlobalFixture );
}

int ExternalRunStr(const std::string &args, LogFormatter *form) {
  return test::DoExternalRunStr(&init_unit_test_suite, args, form);
}

int ExternalRunMain(int argc, char* argv[], LogFormatter *form) {
  return test::DoExternalRunMain(&init_unit_test_suite, argc, argv, form);
}

//#endif
