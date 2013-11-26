#include "rotate.h"


#include "image.h"

#include <assert.h>


/*
0, 0  -----  10, 0       0, 0 ---------------100, 0
 |            |           |                    |
 |            |           |                    |
 |            |---------> |                    |
 |            |           |                    |
 |            |           |                    |
 |            |         0, 10 ---------------100, 10
 |            |
 |            |
0, 100-----  10, 100
*/

namespace
{
	typedef img::Image::SizeType SizeType;
}

namespace img
{
	void rotate90_1(const Image &aSrc, Image &aDst)
	{
		const SizeType tOrigHeight	= aSrc.height();
		const SizeType tOrigWidth	= aSrc.width();

		const SizeType tNewHeight	= aSrc.width();
		const SizeType tNewWidth		= aSrc.height();

		aDst.create(tNewWidth, tNewHeight, aSrc.depth());

		const unsigned char* src_data	= aSrc.data();
		unsigned char* dst_data			= aDst.data();

		for( SizeType y = 0; y < tOrigHeight; ++y )
		{
			for( SizeType x = 0; x < tOrigWidth; ++x )
			{
				SizeType tPixelSrcInd = y * tOrigWidth + x;//y * tOrigWidth	+ x;
				SizeType tPixelDstInd = x * tNewWidth + tNewWidth - y - 1;//x * tNewWidth	+ tNewWidth - y - 1;

				assert( tPixelSrcInd < tOrigHeight * tOrigWidth);
				assert( tPixelDstInd < tNewHeight * tNewWidth);

				dst_data[tPixelDstInd] = src_data[tPixelSrcInd];
				//RotatePixelFun(src_data, dst_data, tPixelSrcInd, tPixelDstInd);
			}
		}
	}

	void rotate90_3(const Image &aSrc, Image &aDst)
	{
		const SizeType tOrigHeight	= aSrc.height();
		const SizeType tOrigWidth	= aSrc.width();

		const SizeType tNewHeight	= aSrc.width();
		const SizeType tNewWidth		= aSrc.height();

		aDst.create(tNewWidth, tNewHeight, aSrc.depth());

		const unsigned char* src_data	= aSrc.data();
		unsigned char* dst_data			= aDst.data();

		for( SizeType y = 0; y < tOrigHeight; ++y )
		{
			for( SizeType x = 0; x < tOrigWidth; ++x )
			{
				SizeType tPixelSrcInd = y * tOrigWidth + x;//y * tOrigWidth	+ x;
				SizeType tPixelDstInd = x * tNewWidth + tNewWidth - y - 1;//x * tNewWidth	+ tNewWidth - y - 1;

				assert( tPixelSrcInd < tOrigHeight * tOrigWidth);
				assert( tPixelDstInd < tNewHeight * tNewWidth);

				dst_data[tPixelDstInd * 3 + 0] = src_data[tPixelSrcInd * 3 + 0];
				dst_data[tPixelDstInd * 3 + 1] = src_data[tPixelSrcInd * 3 + 1];
				dst_data[tPixelDstInd * 3 + 2] = src_data[tPixelSrcInd * 3 + 2];
			}
		}
	}


	void rotate180_1(const Image &aSrc, Image &aDst)
	{
		const SizeType tOrigHeight	= aSrc.height();
		const SizeType tOrigWidth	= aSrc.width();

		const SizeType tNewHeight	= aSrc.height();
		const SizeType tNewWidth		= aSrc.width();

		aDst.create(tNewWidth, tNewHeight, aSrc.depth());

		const unsigned char* src_data	= aSrc.data();
		unsigned char* dst_data			= aDst.data();

		for( SizeType y = 0; y < tOrigHeight; ++y )
		{
			const SizeType destinationY = (tNewHeight - 1 - y) * tNewWidth;
			
			for( SizeType x = 0; x < tOrigWidth; ++x )
			{
				const SizeType destinationX = tNewWidth - 1 - x;

				SizeType tPixelSrcInd = y * tOrigWidth + x;
				SizeType tPixelDstInd = destinationX + destinationY;

				assert( tPixelSrcInd < tOrigHeight * tOrigWidth);
				assert( tPixelDstInd < tNewHeight * tNewWidth);

				dst_data[tPixelDstInd] = src_data[tPixelSrcInd];
			}
		}
	}

	void rotate180_3(const Image &aSrc, Image &aDst)
	{
		const SizeType tOrigHeight	= aSrc.height();
		const SizeType tOrigWidth	= aSrc.width();

		const SizeType tNewHeight	= aSrc.height();
		const SizeType tNewWidth		= aSrc.width();

		aDst.create(tNewWidth, tNewHeight, aSrc.depth());

		const unsigned char* src_data	= aSrc.data();
		unsigned char* dst_data			= aDst.data();

		for( SizeType y = 0; y < tOrigHeight; ++y )
		{
			const SizeType destinationY = (tNewHeight - 1 - y) * tNewWidth;

			for( SizeType x = 0; x < tOrigWidth; ++x )
			{
				const SizeType destinationX = tNewWidth - 1 - x;

				SizeType tPixelSrcInd = y * tOrigWidth + x;
				SizeType tPixelDstInd = destinationX + destinationY;

				assert( tPixelSrcInd < tOrigHeight * tOrigWidth);
				assert( tPixelDstInd < tNewHeight * tNewWidth);

				dst_data[tPixelDstInd * 3 + 0] = src_data[tPixelSrcInd * 3 + 0];
				dst_data[tPixelDstInd * 3 + 1] = src_data[tPixelSrcInd * 3 + 1];
				dst_data[tPixelDstInd * 3 + 2] = src_data[tPixelSrcInd * 3 + 2];
			}
		}
	}

	void rotate270_1(const Image &aSrc, Image &aDst)
	{
		const SizeType tOrigHeight	= aSrc.height();
		const SizeType tOrigWidth	= aSrc.width();

		const SizeType tNewHeight	= aSrc.width();
		const SizeType tNewWidth	= aSrc.height();

		aDst.create(tNewWidth, tNewHeight, aSrc.depth());

		const unsigned char* src_data	= aSrc.data();
		unsigned char* dst_data			= aDst.data();

		for( SizeType y = 0; y < tOrigHeight; ++y )
		{
			const SizeType destinationX = y;

			for( SizeType x = 0; x < tOrigWidth; ++x )
			{
				const SizeType destinationY = tNewHeight - 1 - x;

				SizeType tPixelSrcInd = y * tOrigWidth + x;
				SizeType tPixelDstInd = destinationX + destinationY*tNewWidth;

				assert( tPixelSrcInd < tOrigHeight * tOrigWidth);
				assert( tPixelDstInd < tNewHeight * tNewWidth);

				dst_data[tPixelDstInd] = src_data[tPixelSrcInd];
			}
		}
	}

	void rotate270_3(const Image &aSrc, Image &aDst)
	{
		const SizeType tOrigHeight	= aSrc.height();
		const SizeType tOrigWidth	= aSrc.width();

		const SizeType tNewHeight	= aSrc.width();
		const SizeType tNewWidth		= aSrc.height();

		aDst.create(tNewWidth, tNewHeight, aSrc.depth());

		const unsigned char* src_data	= aSrc.data();
		unsigned char* dst_data			= aDst.data();

		for( SizeType y = 0; y < tOrigHeight; ++y )
		{
			const SizeType destinationX = y;

			for( SizeType x = 0; x < tOrigWidth; ++x )
			{
				const SizeType destinationY = tNewHeight - 1 - x;

				SizeType tPixelSrcInd = y * tOrigWidth + x;
				SizeType tPixelDstInd = destinationX + destinationY*tNewWidth;

				assert( tPixelSrcInd < tOrigHeight * tOrigWidth);
				assert( tPixelDstInd < tNewHeight * tNewWidth);

				dst_data[tPixelDstInd * 3 + 0] = src_data[tPixelSrcInd * 3 + 0];
				dst_data[tPixelDstInd * 3 + 1] = src_data[tPixelSrcInd * 3 + 1];
				dst_data[tPixelDstInd * 3 + 2] = src_data[tPixelSrcInd * 3 + 2];
			}
		}
	}

	Image rotate(const Image &aSrc, Image &aCached, RotateAngle aAngle)
	{
		Image tTemp;
		const bool tUseTemp = (&aSrc == &aCached);
		Image &tDst = tUseTemp?tTemp:aCached;
		//Image &tDst = aCached;

		switch(aAngle)
		{
		case Angle_90:
			if( 1 == aSrc.depth() )
				rotate90_1(aSrc, tDst);
			else
				rotate90_3(aSrc, tDst);
			break;
		case Angle_180:
			if( 1 == aSrc.depth() ) 
				rotate180_1(aSrc, tDst);
			else
				rotate180_3(aSrc, tDst);
			break;
		case Angle_270:
			if( 1 == aSrc.depth() )
				rotate270_1(aSrc, tDst);
			else
				rotate270_3(aSrc, tDst);
			break;
		}

		if( tUseTemp )
			img::copy(tTemp, aCached);

		return aCached;
	}

	Image rotate(const Image &aSrc, RotateAngle aAngle)
	{
		Image tResult;
		return rotate(aSrc, tResult, aAngle);
	}


}

