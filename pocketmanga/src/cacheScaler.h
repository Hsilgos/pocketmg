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
			void swap(Cache &other);

			bool nextBounds();
			bool previousBounds();
		};

		CacheScaler(const size_t screen_width, const size_t screen_height);

		Cache &scaledGrey();

	private:
		CacheScaler &operator = (const CacheScaler &);

		Cache orig_;
		Cache scaled_;
		const size_t screen_width_;
		const size_t screen_height_;

		virtual IBookCache *clone();
		virtual void swap(IBookCache *other);
		virtual bool onLoaded(img::Image &image);
	};
}
