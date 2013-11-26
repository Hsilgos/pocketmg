#include "pictureview.h"

#include "inkview.h"

#include <assert.h>

#include "singleton.h"

#include "book.h"
#include "scale.h"
#include "rotate.h"
#include "cacheScaler.h"


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

	//mImage.enableMinimumReallocations(true);
	mBook = aBook;
	mScaler = new manga::CacheScaler(ScreenWidth(), ScreenHeight());
	mBook->setCachePrototype(mScaler);
	if( mBook->toFirstFile() )
	{
		draw(mScaler);
		mBook->preload();
	}

	//next();
}

bool PictureView::next()
{
	if( mBook.get() )
	{
		assert(mScaler);
		if(mScaler->scaledGrey().nextBounds())
		{
			draw(mScaler);
			return true;
		}

		if( mBook->incrementPosition() )
		{
			//prepareImageToDraw(mImage, mDrawable);
			draw(mScaler);

			mBook->preload();

			return true;
		}
	}

	return false;
}

bool PictureView::previous()
{
	if( mBook.get() )
	{
		assert(mScaler);
		if(mScaler->scaledGrey().previousBounds())
		{
			draw(mScaler);
			return true;
		}

		if( mBook->decrementPosition() )
		{
			//prepareImageToDraw(mImage, mDrawable);
			draw(mScaler);

			mBook->preload();

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

void PictureView::draw()
{
	draw(mScaler);
}

void PictureView::draw(manga::CacheScaler *aScaler)
{
	if( !aScaler )
		return;

	ClearScreen();

	if( manga::CacheScaler::Whole == aScaler->scaledGrey().representation )
	{
		const int x = (ScreenWidth() - aScaler->scaledGrey().image.width())/2;
		const int y = (ScreenHeight() - aScaler->scaledGrey().image.height())/2;
		manDrawBitmap(aScaler->scaledGrey().image, -x, y);
	}
	else if( manga::CacheScaler::Parts3 == aScaler->scaledGrey().representation )
	{
		manDrawBitmap(
				aScaler->scaledGrey().image,
				-aScaler->scaledGrey().bounds.x,
				aScaler->scaledGrey().bounds.y);
	}
	//DrawBitmap(0, 0, t);
	//FullUpdate();
}

}

