#include "pictureview.h"

#include "inkview.h"

#include "singleton.h"

#include "book.h"
#include "scale.h"
#include "rotate.h"


extern const ibitmap add_folder;

namespace pocket
{

int picture_handler(int type, int par1, int par2)
{
	if( EVT_SHOW == type )
	{
		//PictureView::getInstance().draw();
	}

	if (type == EVT_KEYPRESS) {

		  switch(par1)
		  {
		  case KEY_RIGHT:
		  case KEY_NEXT:
			  pocket::PictureView::getInstance().next();
			  break;
		  case KEY_LEFT:
		  case KEY_PREV:
			  pocket::PictureView::getInstance().previous();
			  break;
		  case KEY_MENU:
		  case KEY_HOME:
			  CloseApp();
			  break;
		  }
	}

	return 0;
}

void PictureView::setBook( std::auto_ptr<manga::Book> aBook)
{
	SetEventHandler(picture_handler);

	mImage.enableMinimumReallocations(true);
	mBook = aBook;

	next();
}

bool PictureView::next()
{
	if( mBook.get() )
	{
		if( mBook->getNextImage(mImage) )
		{
			prepareImageToDraw(mImage, mDrawable);
			draw(mDrawable);

			return true;
		}
	}

	return false;
}

bool PictureView::previous()
{
	if( mBook.get() )
	{
		if( mBook->getPreviousImage(mImage) )
		{
			prepareImageToDraw(mImage, mDrawable);
			draw(mDrawable);

			return true;
		}
	}

	return false;
}


void manDrawBitmap(const img::Image& bmp, int x, int y)
{
	int i = 0;
	unsigned char* canvas_addr = NULL;
	const unsigned char* bmp_addr = NULL;
	int line_size = 0;
	int line_count = 0;
	int start_y = 0;
	int start_x = 0;
	int pixel_depth = 0;
	int copy_size = 0;

	icanvas* current_canvas = NULL;
	current_canvas = GetCanvas();

	if ((x <= current_canvas->clipx2) &&
		((y + 1) <= current_canvas->clipy2) &&
		((x + bmp.width()) >= current_canvas->clipx1) &&
		((y + 1 + bmp.height()) >= current_canvas->clipy1))
	{
		if (current_canvas->depth == 8)
		{

			pixel_depth = 1;

			line_count = (y + 1 + bmp.height()) > (current_canvas->clipy2 + 1) ? (current_canvas->clipy2 + 1 - y - 1) : bmp.height();
			line_size = (x + bmp.width()) > (current_canvas->clipx2 + 1) ? (current_canvas->clipx2 + 1 - x) : bmp.width();

			start_y = (y + 1)  < current_canvas->clipy1 ? (current_canvas->clipy1 - (y + 1)) : 0;
			start_x = x < current_canvas->clipx1 ? (current_canvas->clipx1 - x) : 0;

			line_count -= start_y;
			line_size -= start_x;

			canvas_addr = current_canvas->addr + (x + start_x) * pixel_depth + (start_y + y + 1) * current_canvas->scanline;
			//bmp_addr = bmp->data + start_y * bmp->skip + start_x * 1;
			bmp_addr = bmp.data(start_x * 1);

			copy_size = line_size * pixel_depth;

			const int tScanline = bmp.scanline();

			for (i = 0; i < line_count; i++)
			{

				memcpy(canvas_addr, bmp_addr, copy_size);
				canvas_addr += current_canvas->scanline;
				bmp_addr += tScanline;
			}


			FullUpdate();
			//calcUpdateArea(x + start_x, y + start_y + top_margin + 1, line_size, line_count, /*bmp->bw ? PBSC_PARTIALUPBW : */PBSC_PARTIALUP);
		}
	}
}


void PictureView::prepareImageToDraw(const img::Image &aIn, Drawable &aDrawable)
{
	const int tScreenWidth = ScreenWidth();
	const int tScreenHeight = ScreenHeight();

	img::toGray(aIn, aDrawable.orig);
	if( aDrawable.orig.width() < tScreenWidth && aDrawable.orig.height() < tScreenHeight )
	{
		aDrawable.representType = Drawable::Whole;
		aDrawable.orientation = Drawable::Vertical;

		img::copy(aDrawable.orig, aDrawable.image);
	}
	else if( aDrawable.orig.width() < tScreenHeight && aDrawable.orig.height() < tScreenWidth )
	{
		aDrawable.representType = Drawable::Whole;
		aDrawable.orientation = Drawable::Horizontal;

		img::rotate(aDrawable.orig, aDrawable.image, img::Angle_270);
	}
	if( aDrawable.orig.width() <= aDrawable.orig.height() )
	{
		aDrawable.representType = Drawable::Parts3;
		aDrawable.orientation = Drawable::Vertical;

		img::scale(aDrawable.orig, mDrawable.image, img::HighScaling,  tScreenHeight, 0);
		img::rotate(mDrawable.image, mDrawable.image, img::Angle_270);

		aDrawable.currectBouds.x = 0;
		aDrawable.currectBouds.y = 0;
		aDrawable.currectBouds.width 	= tScreenWidth;
		aDrawable.currectBouds.height 	= tScreenHeight;
		/*aDrawable.representType = Drawable::Parts3;
		aDrawable.orientation = Drawable::Vertical;

		const double tImgProp = static_cast<double>(mImage.width())/static_cast<double>(mImage.height());
		const double tScrProp = static_cast<double>(tScreenWidth)/static_cast<double>(tScreenHeight);

		if( tImgProp < tScrProp )
			img::scale(mImage, mDrawable.image, img::HighScaling,  0, tScreenHeight);
		else
			img::scale(mImage, mDrawable.image, img::HighScaling, tScreenWidth, 0);

		aDrawable.currectBouds.x = 0;
		aDrawable.currectBouds.y = 0;
		aDrawable.currectBouds.width 	= tScreenWidth;
		aDrawable.currectBouds.height 	= tScreenHeight;*/
	}
	else
	{
		aDrawable.representType = Drawable::Parts3;
		aDrawable.orientation = Drawable::Vertical;

		img::scale(aDrawable.orig, mDrawable.image, img::HighScaling,  0, tScreenHeight);
		//img::rotate(mDrawable.image, mDrawable.image, img::Angle_270);

		aDrawable.currectBouds.x = 0;
		aDrawable.currectBouds.y = 0;
		aDrawable.currectBouds.width 	= tScreenWidth;
		aDrawable.currectBouds.height 	= tScreenHeight;
		/*aDrawable.representType = Drawable::Parts3;
		aDrawable.orientation = Drawable::Vertical;

		const double tImgProp = static_cast<double>(mImage.height())/static_cast<double>(mImage.width());
		const double tScrProp = static_cast<double>(tScreenWidth)/static_cast<double>(tScreenHeight);

		if( tImgProp < tScrProp )
			img::scale(mImage, mDrawable.image, img::HighScaling, tScreenHeight, 0);
		else
			img::scale(mImage, mDrawable.image, img::HighScaling, 0, tScreenWidth);

		img::rotate(mDrawable.image, mDrawable.image, img::Angle_270);
		aDrawable.currectBouds.x = 0;
		aDrawable.currectBouds.y = 0;
		aDrawable.currectBouds.width 	= tScreenWidth;
		aDrawable.currectBouds.height 	= tScreenHeight;*/
	}
}

void PictureView::draw()
{
	draw(mDrawable);
}

void PictureView::draw(Drawable &aDrawable)
{
	if( mImage.empty() )
		return;

	ClearScreen();

	if( Drawable::Whole == aDrawable.representType )
	{
		const int x = (ScreenWidth() - mDrawable.image.width())/2;
		const int y = (ScreenHeight() - mDrawable.image.height())/2;
		manDrawBitmap(mDrawable.image, x, y);
	}
	else if( Drawable::Parts3 == aDrawable.representType )
	{
		manDrawBitmap(
				mDrawable.image,
				mDrawable.currectBouds.x,
				mDrawable.currectBouds.y);

		int tStep = mDrawable.image.width()/3;
		mDrawable.currectBouds.x += tStep;
	}
	//DrawBitmap(0, 0, t);
	//FullUpdate();
}

}

