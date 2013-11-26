#pragma once

#include <memory>

#include "inkview.h"

#include "singleton.h"
#include "image.h"

namespace manga
{
	class Book;
	class CacheScaler;
}

namespace pocket
{
	class PictureView: public utils::SingletonStatic<PictureView>
	{
		std::auto_ptr<manga::Book> mBook;

		manga::CacheScaler *mScaler;
	public:
		void setBook( std::auto_ptr<manga::Book> aBook);
		bool next();
		bool previous();



		void draw();
		void draw(manga::CacheScaler *aScaler);
	};
}
