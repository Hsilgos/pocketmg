#include "cacheScaler.h"

#include <assert.h>

#include "rotate.h"
#include "scale.h"
#include "debugUtils.h"

namespace manga
{
	IBookCache *CacheScaler::clone()
	{
		return new CacheScaler(mScreenWidth, mScreenHeight);
	}

	void CacheScaler::swap(IBookCache *aOther)
	{
		CacheScaler *tOther = 
			utils::isDebugging() ? dynamic_cast<CacheScaler *>(aOther) : static_cast<CacheScaler *>(aOther);

		assert(tOther);
		mOrig.swap(tOther->mOrig);
		mScaled.swap(tOther->mScaled);
	}

	void CacheScaler::Cache::swap(Cache &aOther)
	{
		std::swap(orientation, aOther.orientation);
		std::swap(representation, aOther.representation);
		std::swap(bounds, aOther.bounds);
		std::swap(currentShowing, aOther.currentShowing);
		image.swap(aOther.image);
	}

	bool CacheScaler::onLoaded(img::Image &aImage)
	{
		img::toBgr(aImage, mOrig.image);
		img::toGray(mOrig.image, mOrig.image);
		if( mOrig.image.width() < mScreenWidth && mOrig.image.height() < mScreenHeight )
		{
			mScaled.representation = Whole;
			mScaled.orientation = Vertical;

			img::copy(mOrig.image, mScaled.image);
		}
		else if( mOrig.image.width() < mScreenHeight && mOrig.image.height() < mScreenWidth )
		{
			mScaled.representation = Whole;
			mScaled.orientation = Horizontal;

			img::rotate(mOrig.image, mScaled.image, img::Angle_270);
		}
		if( mOrig.image.width() <= mOrig.image.height() )
		{
			mScaled.representation = Parts3;
			mScaled.orientation = Vertical;

			img::scale(mOrig.image, mScaled.image, img::HighScaling,  mScreenHeight, 0);
			img::rotate(mScaled.image, mScaled.image, img::Angle_270);

			mScaled.bounds.x = 0;
			mScaled.bounds.y = 0;
			mScaled.bounds.width 	= mScreenWidth;
			mScaled.bounds.height 	= mScreenHeight;
			mScaled.currentShowing	= 0;
			/*aDrawable.representType = Parts3;
			aDrawable.orientation = Vertical;

			const double tImgProp = static_cast<double>(mImage.width())/static_cast<double>(mImage.height());
			const double tScrProp = static_cast<double>(tScreenWidth)/static_cast<double>(mScreenHeight);

			if( tImgProp < tScrProp )
				img::scale(mImage, mDrawable.image, img::HighScaling,  0, mScreenHeight);
			else
				img::scale(mImage, mDrawable.image, img::HighScaling, tScreenWidth, 0);

			aDrawable.currectBouds.x = 0;
			aDrawable.currectBouds.y = 0;
			aDrawable.currectBouds.width 	= tScreenWidth;
			aDrawable.currectBouds.height 	= mScreenHeight;*/
		}
		else
		{
			mScaled.representation = Parts3;
			mScaled.orientation = Vertical;

			img::scale(mOrig.image, mScaled.image, img::HighScaling,  0, mScreenHeight);
			//img::rotate(mDrawable.image, mDrawable.image, img::Angle_270);

			mScaled.bounds.x = 0;
			mScaled.bounds.y = 0;
			mScaled.bounds.width 	= mScreenWidth;
			mScaled.bounds.height 	= mScreenHeight;
			mScaled.currentShowing	= 0;
			/*aDrawable.representType = Parts3;
			aDrawable.orientation = Vertical;

			const double tImgProp = static_cast<double>(mImage.height())/static_cast<double>(mImage.width());
			const double tScrProp = static_cast<double>(tScreenWidth)/static_cast<double>(mScreenHeight);

			if( tImgProp < tScrProp )
				img::scale(mImage, mDrawable.image, img::HighScaling, mScreenHeight, 0);
			else
				img::scale(mImage, mDrawable.image, img::HighScaling, 0, tScreenWidth);

			img::rotate(mDrawable.image, mDrawable.image, img::Angle_270);
			aDrawable.currectBouds.x = 0;
			aDrawable.currectBouds.y = 0;
			aDrawable.currectBouds.width 	= tScreenWidth;
			aDrawable.currectBouds.height 	= mScreenHeight;*/
		}

		return true;
	}

	CacheScaler::Cache &CacheScaler::scaledGrey()
	{
		return mScaled;
	}

	CacheScaler::CacheScaler(const size_t aScreenWidth, const size_t aScreenHeight)
		:mScreenWidth(aScreenWidth), mScreenHeight(aScreenHeight)
	{
	}
}

