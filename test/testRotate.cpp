#include <boost/test/unit_test.hpp>

#include "image.h"
#include "rotate.h"

#include "testJpg_jpg.h"

#include "testBenchmark.h"

#include "report.h"

#include <iostream>

namespace {
static const unsigned int Width		= 256;
static const unsigned int Height	= 128;
}


namespace {
class RotateFixture {
public:
  img::Image image_;

  virtual color::Rgba pixelFor(
    unsigned int x,
    unsigned int y,
    unsigned int w) = 0;

  void create(int depth) {
    image_.create(Width, Height, 3);

    unsigned int h = image_.height();
    unsigned int w = image_.width();
    for( unsigned int y = 0; y < h; ++y ) {
      for( unsigned int x = 0; x < w; ++x ) {
        image_.setPixel(x, y, pixelFor(x, y, w));
      }
    }
  }

  void check90() {
    BOOST_CHECK_EQUAL(image_.width(), Height);
    BOOST_CHECK_EQUAL(image_.height(), Width);

    unsigned int h = image_.height();
    unsigned int w = image_.width();
    for( unsigned int y = 0; y < h; ++y ) {
      for( unsigned int x = 0; x < w; ++x ) {
        BOOST_CHECK(pixelFor(y, w - x - 1, h) == image_.getPixel(x, y));
      }
    }
  }

  void check180() {
    BOOST_CHECK_EQUAL(image_.width(), Width);
    BOOST_CHECK_EQUAL(image_.height(), Height);

    unsigned int h = image_.height();
    unsigned int w = image_.width();
    for( unsigned int y = 0; y < h; ++y ) {
      for( unsigned int x = 0; x < w; ++x ) {
        BOOST_CHECK(pixelFor(w - x - 1, h - y - 1, w) == image_.getPixel(x, y));
      }
    }
  }

  void check270() {
    BOOST_CHECK_EQUAL(image_.width(), Height);
    BOOST_CHECK_EQUAL(image_.height(), Width);

    unsigned int h = image_.height();
    unsigned int w = image_.width();
    for( unsigned int y = 0; y < h; ++y ) {
      for( unsigned int x = 0; x < w; ++x ) {
        BOOST_CHECK(pixelFor(h - y - 1, x, h) == image_.getPixel(x, y));
      }
    }
  }
};

class RotateColorFixture: public RotateFixture {
  color::Rgba pixelFor(
    unsigned int x,
    unsigned int y,
    unsigned int w) {
    unsigned int pos = y * w + x;

    color::Rgb result;

    result.setRed(pos % 256);
    result.setGreen((pos / 256) % 256);
    result.setBlue(pos / (256 * 256));

    return result;

  }
};


class RotateGreyFixture: public RotateFixture {
public:
  color::Rgba pixelFor(
    unsigned int x,
    unsigned int y,
    unsigned int w) {
    unsigned int pos = y * w + x;

    color::Gray result;

    result.set(pos % 256);

    return result;

  }
};
}


namespace test {
// --log_level=test_suite --run_test=TestRotate
BOOST_AUTO_TEST_SUITE( TestRotate )

BOOST_FIXTURE_TEST_CASE( Test90Color, RotateColorFixture ) {
  create(3);

  image_ = img::rotate(image_, img::Angle_90);

  check90();
}

BOOST_FIXTURE_TEST_CASE( Test180Color, RotateColorFixture ) {
  create(3);

  image_ = img::rotate(image_, img::Angle_180);

  check180();
}

BOOST_FIXTURE_TEST_CASE( Test270Color, RotateColorFixture ) {
  create(3);

  image_ = img::rotate(image_, img::Angle_270);

  check270();
}

//////////////////////////////////////////////////////////////////////////

BOOST_FIXTURE_TEST_CASE( Test90ColorSelf, RotateColorFixture ) {
  create(3);

  img::rotate(image_, image_, img::Angle_90);

  check90();
}

BOOST_FIXTURE_TEST_CASE( Test180ColorSelf, RotateColorFixture ) {
  create(3);

  img::rotate(image_, image_, img::Angle_180);

  check180();
}

BOOST_FIXTURE_TEST_CASE( Test270ColorSelf, RotateColorFixture ) {
  create(3);

  img::rotate(image_, image_, img::Angle_270);

  check270();
}

//////////////////////////////////////////////////////////////////////////

BOOST_FIXTURE_TEST_CASE( Test90Grey, RotateGreyFixture ) {
  create(1);

  image_ = img::rotate(image_, img::Angle_90);

  check90();
}

BOOST_FIXTURE_TEST_CASE( Test180Grey, RotateGreyFixture ) {
  create(1);

  image_ = img::rotate(image_, img::Angle_180);

  check180();
}

BOOST_FIXTURE_TEST_CASE( Test270Grey, RotateGreyFixture ) {
  create(1);

  image_ = img::rotate(image_, img::Angle_270);

  check270();
}

//////////////////////////////////////////////////////////////////////////

BOOST_FIXTURE_TEST_CASE( Test90GreySelf, RotateGreyFixture ) {
  create(1);

  img::rotate(image_, image_, img::Angle_90);

  check90();
}

BOOST_FIXTURE_TEST_CASE( Test180GreySelf, RotateGreyFixture ) {
  create(1);

  img::rotate(image_, image_, img::Angle_180);

  check180();
}

BOOST_FIXTURE_TEST_CASE( Test270GreySelf, RotateGreyFixture ) {
  create(1);

  img::rotate(image_, image_, img::Angle_270);

  check270();
}

BOOST_AUTO_TEST_SUITE_END()
}

