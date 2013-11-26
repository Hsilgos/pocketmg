#include <boost/test/unit_test.hpp>

#include "image.h"
#include "scale.h"
#include "rotate.h"
#include "byteArray.h"

#include "testJpg_jpg.h"

#include "testBenchmark.h"

#include "report.h"

#include <iostream>


namespace
{
	class ScaleHelper
	{
		//wxImage mJpegImage;

	protected:
		img::Image mImage;

		test::Report mReport;

		const char *qualityName(img::ScaleQuality aQuality)
		{
			switch(aQuality)
			{
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

		const char *angleName(img::RotateAngle aAngle)
		{
			switch(aAngle)
			{
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
		ScaleHelper()
		{
		}

		~ScaleHelper()
		{
			std::cout << std::endl;
			mReport.printTable( std::cout );
			std::cout << std::endl;
		}

		void init(void *aData, unsigned int aSize)
		{
			BOOST_REQUIRE( mImage.load("jpg", tools::ByteArray(aData, aSize)) );

			std::stringstream tDescr;
			tDescr << "Image size: " << mImage.width() << "x" << mImage.height();
			mReport.setDescription(tDescr.str());
		}

		/*void doScale(const wxSize &aSize, img::ScaleQuality aQuality )
		{
			doScale(aSize.x, aSize.y, aQuality);
		}*/

		void doScale(int aNewWidth, int aNewHeight, img::ScaleQuality aQuality)
		{
			if( 0 ==  aNewWidth)
				aNewWidth = img::proportionalWidth(aNewHeight,  mImage.getSize() );

			if( 0 ==  aNewHeight)
				aNewHeight = img::proportionalHeight(aNewWidth,  mImage.getSize() );

			std::stringstream tRowName;
			tRowName << aNewWidth << "x" << aNewHeight;

			img::Image tCache;
			tCache.createSame(mImage);
			tCache.enableMinimumReallocations(true);

			BENCHMARK( mReport.output(qualityName(aQuality), tRowName.str()) )
				img::scale(mImage, tCache, aQuality, aNewWidth, aNewHeight);
		}

		void doGrey()
		{
			//BENCHMARK( mReport.output("GREY", tRowName.str()) )
				img::toGray(mImage, mImage);
		}

		void doRotate(img::RotateAngle aAngle)
		{
			std::stringstream tDescr;
			tDescr <<mImage.width() << "x" << mImage.height();

			img::Image tCache;
			tCache.createSame(mImage);
			tCache.enableMinimumReallocations(true);

			BENCHMARK( mReport.output(angleName(aAngle), tDescr.str()) )
				img::rotate(mImage, tCache, aAngle);
		}
	};
}


namespace test
{
	// --log_level=test_suite --run_test=TestScale
	BOOST_AUTO_TEST_SUITE( TestScale )

	BOOST_FIXTURE_TEST_CASE( TestColor, ScaleHelper )
	{
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

	BOOST_FIXTURE_TEST_CASE( TestGrey, ScaleHelper )
	{
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

	BOOST_FIXTURE_TEST_CASE( TestRotate_Color, ScaleHelper )
	{
		init(get_testJpg_jpg_buf(), get_testJpg_jpg_size());

		doRotate(img::Angle_90);
		doRotate(img::Angle_180);
		doRotate(img::Angle_270);
	}


	BOOST_FIXTURE_TEST_CASE( TestRotate, ScaleHelper )
	{
		init(get_testJpg_jpg_buf(), get_testJpg_jpg_size());

		doGrey();

		doRotate(img::Angle_90);
		doRotate(img::Angle_180);
		doRotate(img::Angle_270);
	}

	
	BOOST_AUTO_TEST_SUITE_END()
}