#pragma once


#include "defines.h"

/**
 * Usage:
 *
 * <code>STATIC_ASSERT(expression, message)</code>
 *
 * When the static assertion test fails, a compiler error message that somehow
 * contains the "STATIC_ASSERTION_FAILED_AT_LINE_xxx_message" is generated.
 *
 * /!\ message has to be a valid C++ identifier, that is to say it must not
 * contain space characters, cannot start with a digit, etc.
 *
 * STATIC_ASSERT(true, this_message_will_never_be_displayed);
 */

#define STATIC_ASSERT(expression, message)\
  struct TOKEN_JOIN(__static_assertion_at_line_, __LINE__)\
  {\
    implementation::StaticAssertion<static_cast<bool>((expression))> TOKEN_JOIN(TOKEN_JOIN(TOKEN_JOIN(STATIC_ASSERTION_FAILED_AT_LINE_, __LINE__), _), message);\
  };\
  typedef implementation::StaticAssertionTest<sizeof(TOKEN_JOIN(__static_assertion_at_line_, __LINE__))> TOKEN_JOIN(__static_assertion_test_at_line_, __LINE__)

  // note that we wrap the non existing type inside a struct to avoid warning
  // messages about unused variables when static assertions are used at function
  // scope
  // the use of sizeof makes sure the assertion error is not ignored by SFINAE

namespace implementation {

  template <bool>
  struct StaticAssertion;

  template <>
  struct StaticAssertion<true>
  {
  }; // StaticAssertion<true>

  template<int i>
  struct StaticAssertionTest
  {
  }; // StaticAssertionTest<int>

} // namespace implementation

