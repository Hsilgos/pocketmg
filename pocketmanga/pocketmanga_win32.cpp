// pocketmanga.cpp : Defines the entry point for the console application.
//

#include <iostream>

#include "src/filepath.h"
#include "src/filemanager.h"
#include "src/image.h"
#include "src/book.h"
#include "src/scale.h"
#include "src/rotate.h"
#include "src/defines.h"
#include "src/cacheScaler.h"

#include <opencv2/opencv.hpp>

cv::Mat to_cv(const img::Image &aImg)
{
	img::Image tRotated = img::rotate(aImg, img::Angle_90);
	
	cv::Mat tMat1;
	tMat1.create(tRotated.height(), tRotated.width(), CV_MAKETYPE(CV_8U, tRotated.depth()));
	memcpy(tMat1.data, tRotated.data(), tRotated.height() * tRotated.width() * tRotated.depth());

	return tMat1;
}

cv::Mat to_cv(manga::CacheScaler::Cache &aCache)
{
	if( aCache.representation == manga::CacheScaler::Parts3 )
	{
		img::Image tDst;
		utils::Rect bounds = aCache.bounds;

		if(1 == aCache.currentShowing)
			bounds.x = (aCache.image.width() - bounds.width) / 2;
		else if(2 == aCache.currentShowing)
			bounds.x = aCache.image.width() - bounds.width;
		
		copyRect(aCache.image, tDst, bounds);

		aCache.currentShowing++;

		return to_cv(tDst);
	}
	else
	{
		return to_cv(aCache.image);
	}
}
/*
INTER_NEAREST=CV_INTER_NN, //!< nearest neighbor interpolation
INTER_LINEAR=CV_INTER_LINEAR, //!< bilinear interpolation
INTER_CUBIC=CV_INTER_CUBIC, //!< bicubic interpolation
INTER_AREA=CV_INTER_AREA, //!< area-based (or super) interpolation
INTER_LANCZOS4=CV_INTER_LANCZOS4, //!< Lanczos interpolation over 8x8 neighborhood
INTER_MAX=7,
WARP_INVERSE_MAP=CV_WARP_INVERSE_MAP
*/
int main()
{
	manga::Book tBook;

	tBook.setRoot(fs::FilePath("i:\\tmp", false));
	manga::CacheScaler *tScaler = new manga::CacheScaler(600, 800);
	tBook.setCachePrototype( tScaler );

	img::Image tImg;
	
	img::Image tImg2;
	tImg2.enableMinimumReallocations(true);

	img::Image tImg3;
	tImg3.enableMinimumReallocations(true);

	while( tBook.incrementPosition() )
	{
		//tImg = tBook.currentImage();
		//toBgr(tImg, tImg);
		//tImg.copyTo(tImg2);
		//cv::Mat tCv = to_cv(tImg);
		//toGray(tImg, tImg);

		//img::copyRect(tImg, tImg, utils::Rect(0, 0, 400, 400));

		//tImg = img::scale(tImg, img::HighScaling, 600, 0);

		//tImg = img::rotate(tImg, img::Angle_90);

		//cv::imshow("ololo1", to_cv(tImg));
		cv::imshow("ololo1", to_cv(tScaler->scaledGrey()));

		tBook.preload();

		if( tScaler->scaledGrey().representation == manga::CacheScaler::Parts3)
		{
			cv::waitKey(2000);
			cv::imshow("ololo1", to_cv(tScaler->scaledGrey()));
			cv::waitKey(3000);
			cv::imshow("ololo1", to_cv(tScaler->scaledGrey()));
		}
		//cv::Mat to_cv(tImg2)
		//cv::imshow("ololo2", tMat2);
		//cv::imshow("ololo2", tMat2);
		//cv::imshow("ololo2", tMat2);
		
		cv::waitKey(3000);

		
	}
	

	return 0;


/*	fs::IFileManager *tMgr = fs::IFileManager::create();
	std::vector<fs::FilePath> tList = tMgr->getFileList("i:\\books\\Prison School\\Том 01", fs::IFileManager::Directory, true);

	fs::sort(tList, fs::FirstWordThenNumbers);
	//fs::sort(tList, fs::JustNumbers);

	std::vector<fs::FilePath>::iterator it = tList.begin(), itEnd = tList.end();
	for ( ; it != itEnd; ++it )
	{
		std::cout << it->getPath() << std::endl;
	}

	return 0;*/
}
