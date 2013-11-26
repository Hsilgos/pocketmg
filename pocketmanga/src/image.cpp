#include "image.h"

#include "byteArray.h"
#include "filepath.h"
#include "imgDecoderFactory.h"
#include "defines.h"

#include <stdexcept>

namespace
{
	inline size_t dataSize(
		img::Image::SizeType aWidth, 
		img::Image::SizeType aHeight, 
		unsigned short aDepth)
	{
		return aWidth * aHeight * aDepth;
	}
}

namespace img
{
	bool Image::load(const tools::ByteArray &aBuffer)
	{
		return load(utils::EmptyString, aBuffer);
	}

	bool Image::load(const std::string &aFileExt, const tools::ByteArray &aBuffer)
	{
		return DecoderFactory::getInstance().decode(aFileExt, aBuffer, *this);
	}

	Image Image::loadFrom(const tools::ByteArray &aBuffer)
	{
		Image tResult;
		if( tResult.load(aBuffer) )
			return tResult;

		return Image::emptyImage;
	}

	Image Image::loadFrom(const std::string &aFileExt, const tools::ByteArray &aBuffer)
	{
		Image tResult;
		if( tResult.load(aFileExt, aBuffer) )
			return tResult;

		return Image::emptyImage;
	}

	Image::Image()
		:mEnableMinRealloc(false),
		mDepth(0)
	{
	}

	Image::Image(SizeType aWidth, SizeType aHeight, unsigned short aDepth)
		:mEnableMinRealloc(false)
	{
		create(aWidth, aHeight, aDepth);
	}

	Image::~Image()
	{
		destroy();
	}

	void Image::create(SizeType aWidth, SizeType aHeight, unsigned short aDepth)
	{
		const size_t tNewSize = ::dataSize(aWidth, aHeight, aDepth);
		if( 0 == tNewSize )
			return;

		if( !mEnableMinRealloc || tNewSize > mData.getSize() )
			mData.resize(tNewSize);

		mSize =		utils::Size(aWidth, aHeight);
		mDepth		= aDepth;
	}

	void Image::createSame(const Image &aOther)
	{
		create(aOther.width(), aOther.height(), aOther.depth());
	}

	const utils::Size &Image::getSize() const
	{
		return mSize;
	}

	Image::SizeType Image::width() const
	{
		return mSize.width;
	}

	Image::SizeType Image::height() const
	{
		return mSize.height;
	}

	unsigned short Image::depth() const
	{
		return mDepth;
	}

	void Image::setDepth(unsigned short aDepth)
	{
		mDepth = aDepth;
	}

	void Image::setWidth(SizeType aWidth)
	{
		mSize.width = aWidth;
	}

	void Image::setHeight(SizeType aHeight)
	{
		mSize.height = aHeight;
	}

	Image::SizeType Image::scanline() const
	{
		return mSize.width * mDepth;
	}

	void Image::destroy()
	{
		mData.reset();
	}

	unsigned char *Image::data(SizeType aOffset)
	{
		return empty()?0:(mData.askBuffer(mData.getLength()) + aOffset);
	}

	const unsigned char *Image::data(SizeType aOffset) const
	{
		return empty()?0:(mData.getData() + aOffset);
	}

	void Image::swap(Image &aOther)
	{
		std::swap(*this, aOther);
	}

	void Image::enableMinimumReallocations(bool aEnable)
	{
		mEnableMinRealloc = aEnable;
	}

	color::Rgba Image::getPixel(SizeType x, SizeType y) const
	{
		SizeType tDestPixel = y * mSize.width + x;
		SizeType tDestPos	= tDestPixel * mDepth;

		switch( mDepth )
		{
		case 1:
			return color::GrayConstRef(&mData[tDestPos]);
		case 3:
			return color::RgbConstRef(&mData[tDestPos]);
		case 4:
			return color::RgbaConstRef(&mData[tDestPos]);
		default:
			throw std::logic_error("You can't get pixel with such depth");
		}
	}

	void Image::setPixel(SizeType x, SizeType y, const color::Rgba& aRgb)
	{
		SizeType tDestPixel = y * mSize.width + x;
		SizeType tDestPos	= tDestPixel * mDepth;

		switch( mDepth )
		{
		case 1:
			color::GrayRef(&mData[tDestPos], aRgb);
			break;
		case 4:
			color::RgbaRef(&mData[tDestPos], aRgb);
			break;
		case 3:
			color::RgbRef(&mData[tDestPos], aRgb);
			break;
		default:
			throw std::logic_error("You can't set pixel with such depth");
		}
	}

	void Image::setPixel(SizeType x, SizeType y, const color::Gray& aGrey)
	{
		SizeType tDestPixel = y * mSize.width + x;
		SizeType tDestPos	= tDestPixel * mDepth;

		switch( mDepth )
		{
		case 1:
			color::GrayRef(&mData[tDestPos], aGrey);
			break;
		case 4:
			color::RgbaRef(&mData[tDestPos], aGrey);
			break;
		case 3:
			color::RgbRef(&mData[tDestPos], aGrey);
			break;
		default:
			throw std::logic_error("You can't set pixel with such depth");
		}
	}

	const Image Image::emptyImage;

	bool Image::empty() const
	{
		//return mBitmap == 0;
		return mData.isEmpty();
	}

	//////////////////////////////////////////////////////////////////////////
	utils::Rect getRect(const Image &aSrc)
	{
		return utils::Rect(0, 0, aSrc.width(), aSrc.height());
	}

	bool toGray(const Image &aSrc, Image &aDst)
	{
		if( aSrc.empty() )
			return false;

		if( &aSrc != &aDst )
			aDst.create( aSrc.width(), aSrc.height(), 1 );

		const unsigned short tBytesPerPixel = aSrc.depth();
		if( 3 != tBytesPerPixel && 4 != tBytesPerPixel )
			return false;

		const unsigned char *tLineSrc = aSrc.data();
		unsigned char *tLineDst = aDst.data();

		const Image::SizeType  tTotalIters = aSrc.height() * aSrc.width();

		for( Image::SizeType i = 0; i < tTotalIters; ++i, tLineSrc += tBytesPerPixel, ++tLineDst )
		{
			color::RgbConstRef tRgb(tLineSrc);
			color::GrayRef(tLineDst, tRgb);
		}

		if( &aSrc == &aDst )
			aDst.setDepth(1);

		return true;
	}

	bool toBgr(const Image &aSrc, Image &aDst)
	{
		if( aSrc.empty() )
			return false;

		const unsigned int tBytesPerPixel = aSrc.depth();

		if( tBytesPerPixel != 3 && tBytesPerPixel != 4 )
			return false;

		aDst = aSrc;

		const Image::SizeType  tTotalIters = aSrc.height() * aSrc.width();
		unsigned char *tLineSrc = aDst.data();
		for( Image::SizeType i = 0; i < tTotalIters; ++i, tLineSrc += tBytesPerPixel )
			color::RgbRef(tLineSrc).swap(color::RgbRef::RedIndex, color::RgbRef::BlueIndex);

		return true;
	}

	bool copyRect(const img::Image &aSrc, img::Image &aDst, const utils::Rect &aRect)
	{
		utils::Rect tRect = restrictBy(aRect,  getRect(aSrc) );

		if( tRect.width <= 0|| tRect.height <= 0)
			return false;

		const unsigned short tBytesPerPixel = aSrc.depth();

		if( &aSrc != &aDst )
			aDst.create(tRect.width, tRect.height, tBytesPerPixel);

		const int tScanLine = tRect.width * tBytesPerPixel;

		for( int i = tRect.y; i < tRect.y + tRect.height; ++i)
		{
			const int tDstPosition = (i - tRect.y)* tScanLine;
			unsigned char *tLineDst = aDst.data( tDstPosition );

			const int tSrcPosition = (i * aSrc.width() + tRect.x) * tBytesPerPixel;
			const unsigned char *tLineSrc = aSrc.data( tSrcPosition );

			memcpy(tLineDst, tLineSrc, tScanLine);
		}

		if( &aSrc == &aDst )
		{
			aDst.setWidth(tRect.width);
			aDst.setHeight(tRect.height);
		}

		return true;
	}

	void copy(const img::Image &aSrc, img::Image &aDst)
	{
		aDst.createSame(aSrc);
		memcpy(aDst.data(), aSrc.data(), dataSize(aSrc));
	}

	Image::SizeType dataSize(const img::Image &aImg)
	{
		return ::dataSize(aImg.width(), aImg.height(), aImg.depth());
	}
}

