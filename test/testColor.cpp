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
		color::Rgb tRgbSmoke;

		STATIC_ASSERT(color::Rgb::RedIndex		== 0,	ShoulBe0);
		STATIC_ASSERT(color::Rgb::GreenIndex	== 1,	ShoulBe1);
		STATIC_ASSERT(color::Rgb::BlueIndex		== 2,	ShoulBe2);

		color::Rgb tRgb(1, 2, 3);
		BOOST_CHECK_EQUAL(tRgb.red(),	1);
		BOOST_CHECK_EQUAL(tRgb.green(), 2);
		BOOST_CHECK_EQUAL(tRgb.blue(),	3);

		color::Rgb tRgbOther = tRgb;
		BOOST_CHECK_EQUAL(tRgbOther.red(),	1);
		BOOST_CHECK_EQUAL(tRgbOther.green(), 2);
		BOOST_CHECK_EQUAL(tRgbOther.blue(),	3);

		tRgbSmoke = tRgbOther;
		BOOST_CHECK_EQUAL(tRgbSmoke.red(),	1);
		BOOST_CHECK_EQUAL(tRgbSmoke.green(), 2);
		BOOST_CHECK_EQUAL(tRgbSmoke.blue(),	3);

		// BGR
		color::Bgr tBgrSmoke;

		STATIC_ASSERT(color::Bgr::BlueIndex		== 0,	ShoulBe0);
		STATIC_ASSERT(color::Bgr::GreenIndex	== 1,	ShoulBe1);
		STATIC_ASSERT(color::Bgr::RedIndex		== 2,	ShoulBe2);

		// RGBA
		color::Rgba tRgbaSmoke;
		STATIC_ASSERT(color::Rgba::RedIndex		== 0,	ShoulBe0);
		STATIC_ASSERT(color::Rgba::GreenIndex	== 1,	ShoulBe1);
		STATIC_ASSERT(color::Rgba::BlueIndex	== 2,	ShoulBe2);
		STATIC_ASSERT(color::Rgba::AlphaIndex	== 3,	ShoulBe3);

		// ARGB
		color::Argb tArgbSmoke;
		STATIC_ASSERT(color::Argb::AlphaIndex	== 0,	ShoulBe0);
		STATIC_ASSERT(color::Argb::RedIndex		== 1,	ShoulBe1);
		STATIC_ASSERT(color::Argb::GreenIndex	== 2,	ShoulBe2);
		STATIC_ASSERT(color::Argb::BlueIndex	== 3,	ShoulBe3);
	}

	BOOST_AUTO_TEST_CASE( rgb2bgr )
	{
		color::Rgb tRgb(1, 2, 3);
		color::Bgr tBgr = tRgb;

		BOOST_CHECK_EQUAL(tRgb.red(),	1);
		BOOST_CHECK_EQUAL(tRgb.green(), 2);
		BOOST_CHECK_EQUAL(tRgb.blue(),	3);
		BOOST_CHECK_EQUAL(tBgr.red(),	1);
		BOOST_CHECK_EQUAL(tBgr.green(), 2);
		BOOST_CHECK_EQUAL(tBgr.blue(),	3);

		BOOST_CHECK_EQUAL(tRgb.colors[0],	1);
		BOOST_CHECK_EQUAL(tRgb.colors[1],	2);
		BOOST_CHECK_EQUAL(tRgb.colors[2],	3);

		BOOST_CHECK_EQUAL(tBgr.colors[0],	3);
		BOOST_CHECK_EQUAL(tBgr.colors[1],	2);
		BOOST_CHECK_EQUAL(tBgr.colors[2],	1);

		color::Bgr tBgr2(0, 0, 0);
		tBgr2 = tRgb;
		BOOST_CHECK_EQUAL(tBgr2.red(),	1);
		BOOST_CHECK_EQUAL(tBgr2.green(), 2);
		BOOST_CHECK_EQUAL(tBgr2.blue(),	3);
		BOOST_CHECK_EQUAL(tBgr2.colors[0],	3);
		BOOST_CHECK_EQUAL(tBgr2.colors[1],	2);
		BOOST_CHECK_EQUAL(tBgr2.colors[2],	1);

	}

	BOOST_AUTO_TEST_CASE( rgba2bgra )
	{
		color::Rgba tRgba(1, 2, 3, 4);
		color::Bgra tBgra = tRgba;

		BOOST_CHECK_EQUAL(tRgba.red(),		1);
		BOOST_CHECK_EQUAL(tRgba.green(),	2);
		BOOST_CHECK_EQUAL(tRgba.blue(),		3);
		BOOST_CHECK_EQUAL(tRgba.alpha(),	4);

		BOOST_CHECK_EQUAL(tBgra.red(),		1);
		BOOST_CHECK_EQUAL(tBgra.green(),	2);
		BOOST_CHECK_EQUAL(tBgra.blue(),		3);
		BOOST_CHECK_EQUAL(tBgra.alpha(),	4);

		BOOST_CHECK_EQUAL(tRgba.colors[0],	1);
		BOOST_CHECK_EQUAL(tRgba.colors[1],	2);
		BOOST_CHECK_EQUAL(tRgba.colors[2],	3);
		BOOST_CHECK_EQUAL(tRgba.colors[3],	4);

		BOOST_CHECK_EQUAL(tBgra.colors[0],	3);
		BOOST_CHECK_EQUAL(tBgra.colors[1],	2);
		BOOST_CHECK_EQUAL(tBgra.colors[2],	1);
		BOOST_CHECK_EQUAL(tBgra.colors[3],	4);

	}

	BOOST_AUTO_TEST_CASE( rgb2rgba )
	{
		color::Rgb  tRgb(1, 2, 3);
		color::Rgba	tRgba = tRgb;

		BOOST_CHECK_EQUAL(tRgb.red(),		1);
		BOOST_CHECK_EQUAL(tRgb.green(),		2);
		BOOST_CHECK_EQUAL(tRgb.blue(),		3);

		BOOST_CHECK_EQUAL(tRgba.red(),		1);
		BOOST_CHECK_EQUAL(tRgba.green(),	2);
		BOOST_CHECK_EQUAL(tRgba.blue(),		3);
		BOOST_CHECK_EQUAL(tRgba.alpha(),	color::NotTransparent);
	}

	BOOST_AUTO_TEST_CASE( rgba2rgb )
	{
		color::Rgba tRgba(1, 2, 3, 4);
		color::Rgb	tRgb = tRgba;

		BOOST_CHECK_EQUAL(tRgba.red(),		1);
		BOOST_CHECK_EQUAL(tRgba.green(),	2);
		BOOST_CHECK_EQUAL(tRgba.blue(),		3);
		BOOST_CHECK_EQUAL(tRgba.alpha(),	4);

		BOOST_CHECK_EQUAL(tRgb.red(),		1);
		BOOST_CHECK_EQUAL(tRgb.green(),		2);
		BOOST_CHECK_EQUAL(tRgb.blue(),		3);
	}

	BOOST_AUTO_TEST_CASE( SmokeRef )
	{
		color::Channel buffer[3] = {0};

		color::RgbRef tRgb(buffer, 1, 2, 3);

		STATIC_ASSERT(color::RgbRef::RedIndex		== 0,	ShoulBe0);
		STATIC_ASSERT(color::RgbRef::GreenIndex		== 1,	ShoulBe1);
		STATIC_ASSERT(color::RgbRef::BlueIndex		== 2,	ShoulBe2);

		BOOST_CHECK_EQUAL(tRgb.red(),	1);
		BOOST_CHECK_EQUAL(tRgb.green(), 2);
		BOOST_CHECK_EQUAL(tRgb.blue(),	3);

		BOOST_CHECK_EQUAL(buffer[0],	1);
		BOOST_CHECK_EQUAL(buffer[1],	2);
		BOOST_CHECK_EQUAL(buffer[2],	3);
	}

	BOOST_AUTO_TEST_CASE( RgbRef2BgrRef )
	{
		color::Channel buffer1[3] = {0};
		color::Channel buffer2[3] = {0};
		color::RgbRef tRgb(buffer1, 1, 2, 3);
		color::BgrRef tBgr(buffer2);
		tBgr = tRgb;

		BOOST_CHECK_EQUAL(tRgb.red(),	1);
		BOOST_CHECK_EQUAL(tRgb.green(), 2);
		BOOST_CHECK_EQUAL(tRgb.blue(),	3);
		BOOST_CHECK_EQUAL(tBgr.red(),	1);
		BOOST_CHECK_EQUAL(tBgr.green(), 2);
		BOOST_CHECK_EQUAL(tBgr.blue(),	3);

		BOOST_CHECK_EQUAL(tRgb.colors[0],	1);
		BOOST_CHECK_EQUAL(tRgb.colors[1],	2);
		BOOST_CHECK_EQUAL(tRgb.colors[2],	3);

		BOOST_CHECK_EQUAL(tBgr.colors[0],	3);
		BOOST_CHECK_EQUAL(tBgr.colors[1],	2);
		BOOST_CHECK_EQUAL(tBgr.colors[2],	1);

		img::Image tImage;
		BOOST_REQUIRE( 
			tImage.load("jpg", 
				tools::ByteArray(
				get_testJpg_jpg_buf(), 
				get_testJpg_jpg_size())) );

		BENCHMARK( "Rgb->Bgr" )
			toBgr(tImage, tImage);

	}

	BOOST_AUTO_TEST_CASE( Rgb2Gray )
	{
		img::Image tImage;
		BOOST_REQUIRE( 
			tImage.load("jpg", 
				tools::ByteArray(
				get_testJpg_jpg_buf(), 
				get_testJpg_jpg_size())) );

		img::Image tDst;
		toGray(tImage, tDst);
		BENCHMARK( "Rgb->Gray" )
			toGray(tImage, tDst);
	}

	BOOST_AUTO_TEST_SUITE_END()
}
