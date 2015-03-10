#include <boost/test/unit_test.hpp>

#include "color.h"
#include "image.h"


#include "testJpg_jpg.h"
#include "testBenchmark.h"

namespace test
{
	// --log_level=test_suite --run_test=TestColor
	BOOST_AUTO_TEST_SUITE( TestColor )

	BOOST_AUTO_TEST_CASE( Smoke )
	{
		// RGB
		color::Rgb rgb_smoke;

		STATIC_ASSERT(color::Rgb::RedIndex		== 0,	ShoulBe0);
		STATIC_ASSERT(color::Rgb::GreenIndex	== 1,	ShoulBe1);
		STATIC_ASSERT(color::Rgb::BlueIndex		== 2,	ShoulBe2);

		color::Rgb rgb(1, 2, 3);
		BOOST_CHECK_EQUAL(rgb.red(),	1);
		BOOST_CHECK_EQUAL(rgb.green(), 2);
		BOOST_CHECK_EQUAL(rgb.blue(),	3);

		color::Rgb rgb_other = rgb;
		BOOST_CHECK_EQUAL(rgb_other.red(),	1);
		BOOST_CHECK_EQUAL(rgb_other.green(), 2);
		BOOST_CHECK_EQUAL(rgb_other.blue(),	3);

		rgb_smoke = rgb_other;
		BOOST_CHECK_EQUAL(rgb_smoke.red(),	1);
		BOOST_CHECK_EQUAL(rgb_smoke.green(), 2);
		BOOST_CHECK_EQUAL(rgb_smoke.blue(),	3);

		// BGR
		color::Bgr bgr_smoke;

		STATIC_ASSERT(color::Bgr::BlueIndex		== 0,	ShoulBe0);
		STATIC_ASSERT(color::Bgr::GreenIndex	== 1,	ShoulBe1);
		STATIC_ASSERT(color::Bgr::RedIndex		== 2,	ShoulBe2);

		// RGBA
		color::Rgba rgba_smoke;
		STATIC_ASSERT(color::Rgba::RedIndex		== 0,	ShoulBe0);
		STATIC_ASSERT(color::Rgba::GreenIndex	== 1,	ShoulBe1);
		STATIC_ASSERT(color::Rgba::BlueIndex	== 2,	ShoulBe2);
		STATIC_ASSERT(color::Rgba::AlphaIndex	== 3,	ShoulBe3);

		// ARGB
		color::Argb argb_smoke;
		STATIC_ASSERT(color::Argb::AlphaIndex	== 0,	ShoulBe0);
		STATIC_ASSERT(color::Argb::RedIndex		== 1,	ShoulBe1);
		STATIC_ASSERT(color::Argb::GreenIndex	== 2,	ShoulBe2);
		STATIC_ASSERT(color::Argb::BlueIndex	== 3,	ShoulBe3);
	}

	BOOST_AUTO_TEST_CASE( rgb2bgr )
	{
		color::Rgb rgb(1, 2, 3);
		color::Bgr bgr = rgb;

		BOOST_CHECK_EQUAL(rgb.red(),	1);
		BOOST_CHECK_EQUAL(rgb.green(), 2);
		BOOST_CHECK_EQUAL(rgb.blue(),	3);
		BOOST_CHECK_EQUAL(bgr.red(),	1);
		BOOST_CHECK_EQUAL(bgr.green(), 2);
		BOOST_CHECK_EQUAL(bgr.blue(),	3);

		BOOST_CHECK_EQUAL(rgb.colors[0],	1);
		BOOST_CHECK_EQUAL(rgb.colors[1],	2);
		BOOST_CHECK_EQUAL(rgb.colors[2],	3);

		BOOST_CHECK_EQUAL(bgr.colors[0],	3);
		BOOST_CHECK_EQUAL(bgr.colors[1],	2);
		BOOST_CHECK_EQUAL(bgr.colors[2],	1);

		color::Bgr bgr2(0, 0, 0);
		bgr2 = rgb;
		BOOST_CHECK_EQUAL(bgr2.red(),	1);
		BOOST_CHECK_EQUAL(bgr2.green(), 2);
		BOOST_CHECK_EQUAL(bgr2.blue(),	3);
		BOOST_CHECK_EQUAL(bgr2.colors[0],	3);
		BOOST_CHECK_EQUAL(bgr2.colors[1],	2);
		BOOST_CHECK_EQUAL(bgr2.colors[2],	1);

	}

	BOOST_AUTO_TEST_CASE( rgba2bgra )
	{
		color::Rgba rgba(1, 2, 3, 4);
		color::Bgra bgra = rgba;

		BOOST_CHECK_EQUAL(rgba.red(),		1);
		BOOST_CHECK_EQUAL(rgba.green(),	2);
		BOOST_CHECK_EQUAL(rgba.blue(),		3);
		BOOST_CHECK_EQUAL(rgba.alpha(),	4);

		BOOST_CHECK_EQUAL(bgra.red(),		1);
		BOOST_CHECK_EQUAL(bgra.green(),	2);
		BOOST_CHECK_EQUAL(bgra.blue(),		3);
		BOOST_CHECK_EQUAL(bgra.alpha(),	4);

		BOOST_CHECK_EQUAL(rgba.colors[0],	1);
		BOOST_CHECK_EQUAL(rgba.colors[1],	2);
		BOOST_CHECK_EQUAL(rgba.colors[2],	3);
		BOOST_CHECK_EQUAL(rgba.colors[3],	4);

		BOOST_CHECK_EQUAL(bgra.colors[0],	3);
		BOOST_CHECK_EQUAL(bgra.colors[1],	2);
		BOOST_CHECK_EQUAL(bgra.colors[2],	1);
		BOOST_CHECK_EQUAL(bgra.colors[3],	4);

	}

	BOOST_AUTO_TEST_CASE( rgb2rgba )
	{
		color::Rgb  rgb(1, 2, 3);
		color::Rgba	rgba = rgb;

		BOOST_CHECK_EQUAL(rgb.red(),		1);
		BOOST_CHECK_EQUAL(rgb.green(),		2);
		BOOST_CHECK_EQUAL(rgb.blue(),		3);

		BOOST_CHECK_EQUAL(rgba.red(),		1);
		BOOST_CHECK_EQUAL(rgba.green(),	2);
		BOOST_CHECK_EQUAL(rgba.blue(),		3);
		BOOST_CHECK_EQUAL(rgba.alpha(),	color::NotTransparent);
	}

	BOOST_AUTO_TEST_CASE( rgba2rgb )
	{
		color::Rgba rgba(1, 2, 3, 4);
		color::Rgb	rgb = rgba;

		BOOST_CHECK_EQUAL(rgba.red(),		1);
		BOOST_CHECK_EQUAL(rgba.green(),	2);
		BOOST_CHECK_EQUAL(rgba.blue(),		3);
		BOOST_CHECK_EQUAL(rgba.alpha(),	4);

		BOOST_CHECK_EQUAL(rgb.red(),		1);
		BOOST_CHECK_EQUAL(rgb.green(),		2);
		BOOST_CHECK_EQUAL(rgb.blue(),		3);
	}

	BOOST_AUTO_TEST_CASE( SmokeRef )
	{
		color::Channel buffer[3] = {0};

		color::RgbRef rgb(buffer, 1, 2, 3);

		STATIC_ASSERT(color::RgbRef::RedIndex		== 0,	ShoulBe0);
		STATIC_ASSERT(color::RgbRef::GreenIndex		== 1,	ShoulBe1);
		STATIC_ASSERT(color::RgbRef::BlueIndex		== 2,	ShoulBe2);

		BOOST_CHECK_EQUAL(rgb.red(),	1);
		BOOST_CHECK_EQUAL(rgb.green(), 2);
		BOOST_CHECK_EQUAL(rgb.blue(),	3);

		BOOST_CHECK_EQUAL(buffer[0],	1);
		BOOST_CHECK_EQUAL(buffer[1],	2);
		BOOST_CHECK_EQUAL(buffer[2],	3);
	}

	BOOST_AUTO_TEST_CASE( RgbRef2BgrRef )
	{
		color::Channel buffer1[3] = {0};
		color::Channel buffer2[3] = {0};
		color::RgbRef rgb(buffer1, 1, 2, 3);
		color::BgrRef bgr(buffer2);
		bgr = rgb;

		BOOST_CHECK_EQUAL(rgb.red(),	1);
		BOOST_CHECK_EQUAL(rgb.green(), 2);
		BOOST_CHECK_EQUAL(rgb.blue(),	3);
		BOOST_CHECK_EQUAL(bgr.red(),	1);
		BOOST_CHECK_EQUAL(bgr.green(), 2);
		BOOST_CHECK_EQUAL(bgr.blue(),	3);

		BOOST_CHECK_EQUAL(rgb.colors[0],	1);
		BOOST_CHECK_EQUAL(rgb.colors[1],	2);
		BOOST_CHECK_EQUAL(rgb.colors[2],	3);

		BOOST_CHECK_EQUAL(bgr.colors[0],	3);
		BOOST_CHECK_EQUAL(bgr.colors[1],	2);
		BOOST_CHECK_EQUAL(bgr.colors[2],	1);

		img::Image image;
		BOOST_REQUIRE( 
			image.load("jpg", 
				tools::ByteArray(
				get_testJpg_jpg_buf(), 
				get_testJpg_jpg_size())) );

		BENCHMARK( "Rgb->Bgr" )
			toBgr(image, image);

	}

	BOOST_AUTO_TEST_CASE( Rgb2Gray )
	{
		img::Image image;
		BOOST_REQUIRE( 
			image.load("jpg", 
				tools::ByteArray(
				get_testJpg_jpg_buf(), 
				get_testJpg_jpg_size())) );

		img::Image dst;
		toGray(image, dst);
		BENCHMARK( "Rgb->Gray" )
			toGray(image, dst);
	}

	BOOST_AUTO_TEST_SUITE_END()
}
