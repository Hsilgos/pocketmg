#include <boost/test/unit_test.hpp>

#include "image.h"
#include "rotate.h"

#include "testJpg_jpg.h"

#include "testBenchmark.h"

#include "report.h"

#include <iostream>

namespace
{
	static const unsigned int Width		= 256;
	static const unsigned int Height	= 128;
}


namespace
{
	class RotateFixture
	{
	public:
		img::Image mImage;

		virtual color::Rgba pixelFor(
			unsigned int x, 
			unsigned int y, 
			unsigned int w) = 0;

		void create(int aDepth)
		{
			mImage.create(Width, Height, 3);

			unsigned int h = mImage.height();
			unsigned int w = mImage.width();
			for( unsigned int y = 0; y < h; ++y )
			{
				for( unsigned int x = 0; x < w; ++x )
				{
					mImage.setPixel(x, y, pixelFor(x, y, w));
				}
			}
		}

		void check90()
		{
			BOOST_CHECK_EQUAL(mImage.width(), Height);
			BOOST_CHECK_EQUAL(mImage.height(), Width);

			unsigned int h = mImage.height();
			unsigned int w = mImage.width();
			for( unsigned int y = 0; y < h; ++y )
			{
				for( unsigned int x = 0; x < w; ++x )
				{
					BOOST_CHECK(pixelFor(y, w - x - 1, h) == mImage.getPixel(x, y));
				}
			}
		}

		void check180()
		{
			BOOST_CHECK_EQUAL(mImage.width(), Width);
			BOOST_CHECK_EQUAL(mImage.height(), Height);

			unsigned int h = mImage.height();
			unsigned int w = mImage.width();
			for( unsigned int y = 0; y < h; ++y )
			{
				for( unsigned int x = 0; x < w; ++x )
				{
					BOOST_CHECK(pixelFor(w - x - 1, h - y - 1, w) == mImage.getPixel(x, y));
				}
			}
		}

		void check270()
		{
			BOOST_CHECK_EQUAL(mImage.width(), Height);
			BOOST_CHECK_EQUAL(mImage.height(), Width);

			unsigned int h = mImage.height();
			unsigned int w = mImage.width();
			for( unsigned int y = 0; y < h; ++y )
			{
				for( unsigned int x = 0; x < w; ++x )
				{
					BOOST_CHECK(pixelFor(h - y - 1, x, h) == mImage.getPixel(x, y));
				}
			}
		}
	};

	class RotateColorFixture: public RotateFixture
	{
		color::Rgba pixelFor(
			unsigned int x, 
			unsigned int y, 
			unsigned int w)
		{
			unsigned int tPos = y * w + x;

			color::Rgb tResult;

			tResult.setRed(tPos % 256);
			tResult.setGreen((tPos / 256) % 256);
			tResult.setBlue(tPos / (256 * 256));

			return tResult;
			
		}
	};


	class RotateGreyFixture: public RotateFixture
	{
	public:
		color::Rgba pixelFor(
			unsigned int x, 
			unsigned int y, 
			unsigned int w)
		{
			unsigned int tPos = y * w + x;

			color::Gray tResult;

			tResult.set(tPos % 256);

			return tResult;

		}
	};
}


namespace test
{
	// --log_level=test_suite --run_test=TestRotate
	BOOST_AUTO_TEST_SUITE( TestRotate )

	BOOST_FIXTURE_TEST_CASE( Test90Color, RotateColorFixture )
	{
		create(3);

		mImage = img::rotate(mImage, img::Angle_90);

		check90();
	}

	BOOST_FIXTURE_TEST_CASE( Test180Color, RotateColorFixture )
	{
		create(3);

		mImage = img::rotate(mImage, img::Angle_180);

		check180();
	}

	BOOST_FIXTURE_TEST_CASE( Test270Color, RotateColorFixture )
	{
		create(3);

		mImage = img::rotate(mImage, img::Angle_270);

		check270();
	}

	//////////////////////////////////////////////////////////////////////////

	BOOST_FIXTURE_TEST_CASE( Test90ColorSelf, RotateColorFixture )
	{
		create(3);

		img::rotate(mImage, mImage, img::Angle_90);

		check90();
	}

	BOOST_FIXTURE_TEST_CASE( Test180ColorSelf, RotateColorFixture )
	{
		create(3);

		img::rotate(mImage, mImage, img::Angle_180);

		check180();
	}

	BOOST_FIXTURE_TEST_CASE( Test270ColorSelf, RotateColorFixture )
	{
		create(3);

		img::rotate(mImage, mImage, img::Angle_270);

		check270();
	}

	//////////////////////////////////////////////////////////////////////////

	BOOST_FIXTURE_TEST_CASE( Test90Grey, RotateGreyFixture )
	{
		create(1);

		mImage = img::rotate(mImage, img::Angle_90);

		check90();
	}

	BOOST_FIXTURE_TEST_CASE( Test180Grey, RotateGreyFixture )
	{
		create(1);

		mImage = img::rotate(mImage, img::Angle_180);

		check180();
	}

	BOOST_FIXTURE_TEST_CASE( Test270Grey, RotateGreyFixture )
	{
		create(1);

		mImage = img::rotate(mImage, img::Angle_270);

		check270();
	}

	//////////////////////////////////////////////////////////////////////////

	BOOST_FIXTURE_TEST_CASE( Test90GreySelf, RotateGreyFixture )
	{
		create(1);

		img::rotate(mImage, mImage, img::Angle_90);

		check90();
	}

	BOOST_FIXTURE_TEST_CASE( Test180GreySelf, RotateGreyFixture )
	{
		create(1);

		img::rotate(mImage, mImage, img::Angle_180);

		check180();
	}

	BOOST_FIXTURE_TEST_CASE( Test270GreySelf, RotateGreyFixture )
	{
		create(1);

		img::rotate(mImage, mImage, img::Angle_270);

		check270();
	}

	BOOST_AUTO_TEST_SUITE_END()
}

