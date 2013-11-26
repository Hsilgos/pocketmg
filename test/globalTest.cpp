#include "testName.h"

#include "globalTest.h"

#include <boost/test/included/unit_test.hpp>

#include "testFormatter.h"


#if defined( __i386__ ) && defined ( __ANDROID__ )

#include <setjmp.h> 

void siglongjmp(sigjmp_buf env, int val)
{
	longjmp(env, val);
}

int sigsetjmp(sigjmp_buf env, int savemask)
{
	return setjmp(env);
}

#endif

namespace test
{
	BOOST_GLOBAL_FIXTURE( TestGlobalFixture );
}

int ExternalRunStr(const std::string &aArgs, LogFormatter *aForm)
{
	return test::DoExternalRunStr(&init_unit_test_suite, aArgs, aForm);
}

int ExternalRunMain(int aArgc, char* aArgv[], LogFormatter *aForm)
{
	return test::DoExternalRunMain(&init_unit_test_suite, aArgc, aArgv, aForm);
}

//#endif
