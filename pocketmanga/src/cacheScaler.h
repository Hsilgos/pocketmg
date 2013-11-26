#pragma once

#include "book.h"

namespace manga
{
	class CacheScaler: public IBookCache
	{
	public:
		enum Orientation
		{
			Vertical,
			Horizontal
		};

		enum RepresentType
		{
			Whole,
			Parts3,
			Parts6
		};

		struct Cache
		{
			Orientation orientation;
			RepresentType representation;
			utils::Rect bounds;
			img::Image image;

			int currentShowing;

			Cache();
			void swap(Cache &aOther);

			bool nextBounds();
			bool previousBounds();
		};

		CacheScaler(const size_t aScreenWidth, const size_t aScreenHeight);

		Cache &scaledGrey();

	private:
		CacheScaler &operator = (const CacheScaler &);

		Cache mOrig;
		Cache mScaled;
		const size_t mScreenWidth;
		const size_t mScreenHeight;

		virtual IBookCache *clone();
		virtual void swap(IBookCache *aOther);
		virtual bool onLoaded(img::Image &aImage);
	};
}
