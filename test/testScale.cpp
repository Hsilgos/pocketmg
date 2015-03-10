#include <boost/test/unit_test.hpp>

#include "image.h"
#include "scale.h"
#include "rotate.h"
#include "byteArray.h"

#include "testJpg_jpg.h"

#include "testBenchmark.h"

#include "report.h"

#include <iostream>


namespace {
class ScaleHelper {
  //wxImage jpeg_image_;

protected:
  img::Image image_;

  test::Report report_;

  const char *qualityName(img::ScaleQuality quality) {
    switch(quality) {
    case img::HighScaling:
      return "BICUBIC";
    case img::FastScaling:
      return "NEAREST";
    case img::MiddleScaling:
      return "BILINEAR";
    default:
      throw std::logic_error("Unknown scale quality");
    }

    return "Unknow";
  }

  const char *angleName(img::RotateAngle angle) {
    switch(angle) {
    case img::Angle_90:
      return "90";
    case img::Angle_180:
      return "180";
    case img::Angle_270:
      return "270";
    default:
      throw std::logic_error("Unknown angle");
    }

    return "Unknow";
  }
public:
  ScaleHelper() {
  }

  ~ScaleHelper() {
    std::cout << std::endl;
    report_.printTable( std::cout );
    std::cout << std::endl;
  }

  void init(void *data, unsigned int size) {
    BOOST_REQUIRE( image_.load("jpg", tools::ByteArray(data, size)) );

    std::stringstream descr;
    descr << "Image size: " << image_.width() << "x" << image_.height();
    report_.setDescription(descr.str());
  }

  /*void doScale(const wxSize &size, img::ScaleQuality quality )
  {
  	doScale(size.x, size.y, quality);
  }*/

  void doScale(int new_width, int new_height, img::ScaleQuality quality) {
    if( 0 ==  new_width)
      new_width = img::proportionalWidth(new_height,  image_.getSize() );

    if( 0 ==  new_height)
      new_height = img::proportionalHeight(new_width,  image_.getSize() );

    std::stringstream row_name;
    row_name << new_width << "x" << new_height;

    img::Image cache;
    cache.createSame(image_);
    cache.enableMinimumReallocations(true);

    BENCHMARK( report_.output(qualityName(quality), row_name.str()) )
    img::scale(image_, cache, quality, new_width, new_height);
  }

  void doGrey() {
    //BENCHMARK( report_.output("GREY", row_name.str()) )
    img::toGray(image_, image_);
  }

  void doRotate(img::RotateAngle angle) {
    std::stringstream descr;
    descr <<image_.width() << "x" << image_.height();

    img::Image cache;
    cache.createSame(image_);
    cache.enableMinimumReallocations(true);

    BENCHMARK( report_.output(angleName(angle), descr.str()) )
    img::rotate(image_, cache, angle);
  }
};
}


namespace test {
// --log_level=test_suite --run_test=TestScale
BOOST_AUTO_TEST_SUITE( TestScale )

BOOST_FIXTURE_TEST_CASE( TestColor, ScaleHelper ) {
  init(get_testJpg_jpg_buf(), get_testJpg_jpg_size());

  doScale(2000, 0, img::HighScaling);
  doScale(1280, 0, img::HighScaling);
  doScale(690, 0, img::HighScaling);

  doScale(2000, 0, img::MiddleScaling);
  doScale(1280, 0, img::MiddleScaling);
  doScale(690, 0, img::MiddleScaling);

  doScale(2000, 0, img::FastScaling);
  doScale(1280, 0, img::FastScaling);
  doScale(690, 0, img::FastScaling);
}

BOOST_FIXTURE_TEST_CASE( TestGrey, ScaleHelper ) {
  init(get_testJpg_jpg_buf(), get_testJpg_jpg_size());

  doGrey();

  doScale(2000, 0, img::HighScaling);
  doScale(1280, 0, img::HighScaling);
  doScale(690, 0, img::HighScaling);

  doScale(2000, 0, img::MiddleScaling);
  doScale(1280, 0, img::MiddleScaling);
  doScale(690, 0, img::MiddleScaling);

  doScale(2000, 0, img::FastScaling);
  doScale(1280, 0, img::FastScaling);
  doScale(690, 0, img::FastScaling);
}

BOOST_FIXTURE_TEST_CASE( TestRotate_Color, ScaleHelper ) {
  init(get_testJpg_jpg_buf(), get_testJpg_jpg_size());

  doRotate(img::Angle_90);
  doRotate(img::Angle_180);
  doRotate(img::Angle_270);
}


BOOST_FIXTURE_TEST_CASE( TestRotate, ScaleHelper ) {
  init(get_testJpg_jpg_buf(), get_testJpg_jpg_size());

  doGrey();

  doRotate(img::Angle_90);
  doRotate(img::Angle_180);
  doRotate(img::Angle_270);
}


BOOST_AUTO_TEST_SUITE_END()
}