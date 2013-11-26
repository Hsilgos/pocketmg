#pragma once

#include <string>

#include "byteArray.h"

#include "defines.h"

#include "color.h"

namespace tools
{
	class ByteArray;
}

namespace fs
{
	class FilePath;
}

namespace utils
{
	struct Rect;
}

namespace img
{
	/*
	Class has 'copy on write' ideology, i.e.
	buffer will be copied at time when source or destination will try to change buffer.
	for example:

	Image src;
	src.create(800, 600, 3);

	Image dst = src;
	// src and dst points to same buffer.

	dst[0] = Rgb(255, 255, 255);
	// dst will copy it's buffer from src
	*/

	class Image
	{
		tools::ByteArray	mData;

		utils::Size			mSize;
		unsigned short		mDepth;

		bool mEnableMinRealloc;
	public:
		typedef utils::Size::SizeType SizeType;

		Image();
		~Image();

		Image(SizeType aWidth, SizeType aHeight, unsigned short aDepth);

		bool load(const tools::ByteArray &aBuffer);
		bool load(const std::string &aFileExt, const tools::ByteArray &aBuffer);

		static Image loadFrom(const tools::ByteArray &aBuffer);
		static Image loadFrom(const std::string &aFileExt, const tools::ByteArray &aBuffer);

		void create(SizeType aWidth, SizeType aHeight, unsigned short aDepth);
		void createSame(const Image &aOther);
		void destroy();
		bool empty() const;

		const utils::Size &getSize() const;
		SizeType width() const;
		SizeType height() const;
		unsigned short depth() const;
		SizeType scanline() const;

		void setDepth(unsigned short aDepth);
		void setWidth(SizeType aWidth);
		void setHeight(SizeType aHeight);

		void swap(Image &aOther);

		color::Rgba getPixel(SizeType x, SizeType y) const;
		void setPixel(SizeType x, SizeType y, const color::Rgba& aRgb);
		void setPixel(SizeType x, SizeType y, const color::Gray& aGrey);

		//void moveTo(Image &aOther);
		//void copyTo(Image &aOther) const;

		void enableMinimumReallocations(bool aEnable);

		// aOffset in bytes
		unsigned char *data(SizeType aOffset = 0);
		const unsigned char *data(SizeType aOffset = 0) const;

		static const Image emptyImage;

//		ibitmap *native();
//		const ibitmap *native() const;
	};

	utils::Rect getRect(const Image &aSrc);

	bool toGray(const Image &aSrc, Image &aDst);

	bool toBgr(const Image &aSrc, Image &aDst);

	bool copyRect(const img::Image &aSrc, img::Image &aDst, const utils::Rect &aRect);
	void copy(const img::Image &aSrc, img::Image &aDst);
	Image::SizeType dataSize(const img::Image &aImg);
}

