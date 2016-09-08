#pragma once

#ifdef UNIT_TEST_SUPPORT
#define MOCKED_METHOD virtual
#else
#define MOCKED_METHOD
#endif
