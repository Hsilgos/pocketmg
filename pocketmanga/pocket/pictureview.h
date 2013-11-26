#pragma once

#include <memory>

#include "inkview.h"

#include "singleton.h"
#include "image.h"

namespace manga
{
	class Book;
}

struct Drawable
{
	img::Image orig;
	img::Image image;

	enum Orientation
	{
		Horizontal,
		Vertical
	} orientation;

	enum RepresentType
	{
		Whole,
		Parts3,
		Parts6
	} representType;

	utils::Rect currectBouds;

	Drawable()
	{
		orig.enableMinimumReallocations(true);
		image.enableMinimumReallocations(true);
	}
};

namespace pocket
{
	class PictureView: public utils::SingletonStatic<PictureView>
	{
		std::auto_ptr<manga::Book> mBook;
		img::Image mImage;

		Drawable mPrevDrawable;
		Drawable mDrawable;
		Drawable mNextDrawable;

		void prepareImageToDraw(const img::Image &aIn, Drawable &aDrawable);
	public:
		void setBook( std::auto_ptr<manga::Book> aBook);
		bool next();
		bool previous();



		void draw();
		void draw(Drawable &aDrawable);
	};
}
