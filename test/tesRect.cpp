#include <boost/test/unit_test.hpp>

#include "defines.h"

namespace {
void doTest(
  int x, int y, int w, int h,
  int xrest, int yrest, int wrest, int hrest,
  int xresult, int yresult, int wresult, int hresult) {
  utils::Rect rect(x, y, w, h);
  utils::Rect restrict(xrest, yrest, wrest, hrest);

  utils::Rect result = restrictBy(rect, restrict);
  BOOST_CHECK_EQUAL( result.x,		xresult);
  BOOST_CHECK_EQUAL( result.y,		yresult);
  BOOST_CHECK_EQUAL( result.width,	wresult);
  BOOST_CHECK_EQUAL( result.height,	hresult);
}
}


namespace test {
// --log_level=test_suite --run_test=TestRect
BOOST_AUTO_TEST_SUITE( TestRect )

BOOST_AUTO_TEST_CASE( Restrict ) {
  doTest(
    0, 0, 100, 100,
    0, 0, 100, 100,
    0, 0, 100, 100);

  doTest(
    0, 0, 100, 100,
    0, 0, 90, 90,
    0, 0, 90, 90);

  doTest(
    -10, -10, 110, 110,
    0, 0, 100, 100,
    0, 0, 100, 100);

  doTest(
    -100, -100, 100, 100,
    0, 0, 100, 100,
    0, 0, 0, 0);

  doTest(
    0, 0, 100, 100,
    50, 50, 150, 150,
    50, 50, 50, 50);
}

BOOST_AUTO_TEST_SUITE_END()
}
