#include "imgDecoder.h"
#include "imgDecoderFactory.h"

#include "byteArray.h"

#include "image.h"

#include "static_assert.h"

namespace
{
#pragma pack(push,2)

	struct BITMAPFILEHEADER 
	{
		unsigned short		type;
		unsigned long 		size;
		unsigned short		reserved1;
		unsigned short		reserved2;
		unsigned long		offBits;
	};

#pragma pack(pop)


	struct BITMAPINFOHEADER
	{
		unsigned long	size;
		long			width;
		long			height;
		unsigned short	planes;
		unsigned short	bitCount;
		unsigned long	compression;
		unsigned long	sizeImage;
		long			xPelsPerMeter;
		long			yPelsPerMeter;
		unsigned long	clrUsed;
		unsigned long	clrImportant;
	};

	static const unsigned short BmpType = 0x4d42; // 'MB'

	enum Compreesion
	{
		NoCompression	= 0, // BI_RGB,
		BitFields		= 3  // BI_BITFIELDS
	};

	struct Palette
	{
		const unsigned char *data;
		const unsigned int size;

		const unsigned char *getColor(unsigned int aIndex) const
		{
			unsigned int tRealIdx = aIndex * 4;
			if( tRealIdx < size )
				return &data[tRealIdx];

			return 0;
		}

		Palette(const unsigned char *aData, unsigned int aSize)
			:data(aData), size(aSize)
		{
		}
	};
}


namespace img
{
	class BmpDecoder: public IDecoder
	{
	public:
		virtual std::vector<std::string> getExts() const
		{
			std::vector<std::string> tExts;

			tExts.push_back("bmp");

			return tExts;
		}

		unsigned int correctScanline(unsigned int aScanline)
		{
			while( aScanline % 4 != 0 )
				++aScanline;

			return aScanline;
		}

		bool loadMono(
			unsigned int aWidth, 
			unsigned int aHeight,
			const unsigned char* aSrcBegin,
			unsigned long aDataSize,
			const Palette &aPallete,
			img::Image &aDecoded)
		{
			aDecoded.create(aWidth, aHeight, 3);

			const unsigned int tSrcScanline = correctScanline(aWidth/8 + ((aWidth%8 != 0)?1:0));

			if( tSrcScanline * aHeight > aDataSize )
				return false;

			unsigned char* tDst = aDecoded.data();

			for( unsigned short y = 0; y < aHeight; ++y)
			{
				const unsigned char* tSrcLine = &aSrcBegin[( aHeight - y - 1 )*tSrcScanline];

				for( unsigned short x = 0; x < aWidth; ++x)
				{
					const unsigned char* tSrc = &tSrcLine[x/8];
					const bool tBit = ( (*tSrc >> (7 - x % 8)) & 1 );
					const unsigned char *tBgra = aPallete.getColor(tBit?1:0);
					if( tBgra )
					{
						tDst[0] = tBgra[2];
						tDst[1] = tBgra[1];
						tDst[2] = tBgra[0];

						tDst += 3;
					}
					else
					{
						return 0;
					}
				}
			}

			return true;
		}

		bool loadColor16(
			unsigned int aWidth, 
			unsigned int aHeight,
			const unsigned char* aSrcBegin,
			unsigned long aDataSize,
			const Palette &aPallete,
			img::Image &aDecoded)
		{
			aDecoded.create(aWidth, aHeight, 3);

			const unsigned int tSrcScanline = correctScanline(aWidth/2 + ((aWidth % 2) !=  0 ? 1 : 0) );

			if( tSrcScanline * aHeight > aDataSize )
				return false;

			unsigned char* tDst = aDecoded.data();

			for( unsigned short y = 0; y < aHeight; ++y)
			{
				const unsigned char* tSrcLine = &aSrcBegin[( aHeight - y - 1 )*tSrcScanline];

				for( unsigned short x = 0; x < aWidth; ++x)
				{
					const unsigned char tPixPair = tSrcLine[x/2];
					const unsigned char tIndex = (x&1)?(tPixPair&0xF):(tPixPair >> 4);

					const unsigned char *tBgra = aPallete.getColor(tIndex);

					if( tBgra )
					{
						//bgr(a) ->rgb
						tDst[0] = tBgra[2];
						tDst[1] = tBgra[1];
						tDst[2] = tBgra[0];

						tDst += 3;
					}
					else
					{
						return false;
					}
				}
			}

			return true;
		}

		bool loadColor256(
			unsigned int aWidth, 
			unsigned int aHeight,
			const unsigned char* aSrcBegin,
			unsigned long aDataSize,
			const Palette &aPallete,
			img::Image &aDecoded)
		{
			aDecoded.create(aWidth, aHeight, 3);

			const unsigned int tSrcScanline = correctScanline(aWidth);

			if( tSrcScanline * aHeight > aDataSize )
				return false;

			unsigned char* tDst = aDecoded.data();

			for( unsigned short y = 0; y < aHeight; ++y)
			{
				const unsigned char* tSrcLine = &aSrcBegin[( aHeight - y - 1 )*tSrcScanline];

				for( unsigned short x = 0; x < aWidth; ++x)
				{
					unsigned char tSrc = tSrcLine[x];

					const unsigned char *tBgra = aPallete.getColor(tSrc);

					if( tBgra )
					{
						//bgr(a) ->rgb
						tDst[0] = tBgra[2];
						tDst[1] = tBgra[1];
						tDst[2] = tBgra[0];

						tDst += 3;
					}
					else
					{
						return false;
					}
				}
			}

			return true;
		}

		bool loadColor16_XXX(
			unsigned int aWidth, 
			unsigned int aHeight,
			const unsigned short* aSrcBegin,
			unsigned long aDataSize,
			img::Image &aDecoded,
			unsigned short aMaskRed,
			unsigned short aMaskGreen,
			unsigned short aMaskBlue)
		{
			aDecoded.create(aWidth, aHeight, 3);

			const unsigned int tSrcScanline = correctScanline(aWidth * 2);

			if( tSrcScanline * aHeight > aDataSize )
				return false;

			unsigned short tROff = 0, tGOff = 0, tBOff = 0;
			while( !((aMaskRed >> tROff) & 1) )
				++tROff;
			while( !((aMaskGreen >> tGOff) & 1) )
				++tGOff;
			while( !((aMaskBlue >> tBOff) & 1) )
				++tBOff;

			const unsigned int tRMult = 256 / (aMaskRed >> tROff);
			const unsigned int tGMult = 256 / (aMaskGreen >> tGOff);
			const unsigned int tBMult = 256 / (aMaskBlue >> tBOff);

			unsigned char* tDst = aDecoded.data();

			for( unsigned short y = 0; y < aHeight; ++y)
			{
				const unsigned short* tSrcLine = &aSrcBegin[( aHeight - y - 1 )*(tSrcScanline/2)];

				for( unsigned short x = 0; x < aWidth; ++x)
				{
					unsigned short tSrc = tSrcLine[x];

					tDst[0] = ((tSrc & aMaskRed) >> tROff) * tRMult;
					tDst[1] = ((tSrc & aMaskGreen) >> tGOff) * tGMult;
					tDst[2] = ((tSrc & aMaskBlue) >> tBOff) * tBMult;

					tDst += 3;
				}
			}

			return true;
		}

		bool loadColor16_555(
			unsigned int aWidth, 
			unsigned int aHeight,
			const unsigned char* aSrcBegin,
			unsigned long aDataSize,
			const Palette &/*aPallete*/,
			img::Image &aDecoded)
		{
			unsigned short tMaskBlue		= 0x1F;
			unsigned short tMaskGreen	= tMaskBlue << 5;
			unsigned short tMaskRed		= tMaskBlue << 10;

			return loadColor16_XXX(
				aWidth, 
				aHeight, 
				reinterpret_cast<const unsigned short *>(aSrcBegin),
				aDataSize,
				aDecoded,
				tMaskRed,
				tMaskGreen,
				tMaskBlue);
		}

		bool loadColor16_XXX(
			unsigned int aWidth, 
			unsigned int aHeight,
			const unsigned char* aSrcBegin,
			unsigned long aDataSize,
			const Palette &aPallete,
			img::Image &aDecoded)
		{
			if( aPallete.size < 3 )
				return false;

			unsigned short tMaskRed = *(reinterpret_cast<const unsigned short *>(aPallete.getColor(0)));
			unsigned short tMaskGreen = *(reinterpret_cast<const unsigned short *>(aPallete.getColor(1)));
			unsigned short tMaskBlue = *(reinterpret_cast<const unsigned short *>(aPallete.getColor(2)));

			
			return loadColor16_XXX(
				aWidth, 
				aHeight, 
				reinterpret_cast<const unsigned short *>(aSrcBegin),
				aDataSize,
				aDecoded,
				tMaskRed,
				tMaskGreen,
				tMaskBlue);
		}

		bool loadColorFull(
			unsigned int aWidth, 
			unsigned int aHeight,
			const unsigned char* aSrcBegin,
			unsigned long aDataSize,
			unsigned short aBytePerPixel,
			img::Image &aDecoded)
		{
			aDecoded.create(aWidth, aHeight, aBytePerPixel );

			const unsigned int tSrcScanline = correctScanline(aWidth * aBytePerPixel);

			if( tSrcScanline * aHeight > aDataSize )
				return false;

			unsigned char* tDst = aDecoded.data();

			for( unsigned short y = 0; y < aHeight; ++y)
			{
				const unsigned char* tSrc = &aSrcBegin[( aHeight - y - 1 ) * tSrcScanline];

				for( unsigned short x = 0; x < aWidth; ++x)
				{
					tDst[0]	= tSrc[2];
					tDst[1]	= tSrc[1];
					tDst[2]	= tSrc[0];

					if( 4 == aBytePerPixel )
						tDst[3]	= tSrc[3];

					tDst += aBytePerPixel;
					tSrc += aBytePerPixel;
				}
			}

			return true;
		}

		bool loadColorFull24(
			unsigned int aWidth, 
			unsigned int aHeight,
			const unsigned char* aSrcBegin,
			unsigned long aDataSize,
			img::Image &aDecoded)
		{
			return loadColorFull(aWidth, aHeight, aSrcBegin, aDataSize, 3, aDecoded);
		}

		bool loadColorFull32(
			unsigned int aWidth, 
			unsigned int aHeight,
			const unsigned char* aSrcBegin,
			unsigned long aDataSize,
			img::Image &aDecoded)
		{
			return loadColorFull(aWidth, aHeight, aSrcBegin, aDataSize, 4, aDecoded);
		}

		virtual bool decode(const tools::ByteArray &aEncoded, img::Image &aDecoded)
		{
			STATIC_ASSERT( (sizeof(BITMAPFILEHEADER) == 14), Header_size_mismatch );
			STATIC_ASSERT( (sizeof(BITMAPINFOHEADER) == 40), Info_size_mismatch );

			// TODO: check size
			BITMAPFILEHEADER tHeader = {0};
			if( !fromByteArray(aEncoded, tHeader) ||
				tHeader.type != BmpType )
				return false;

			BITMAPINFOHEADER tInfo = {0};
			if( !fromByteArray(aEncoded, sizeof(BITMAPFILEHEADER), tInfo) )
				return false;

			if( tInfo.compression != NoCompression &&
				!( tInfo.bitCount == 16 && tInfo.compression == BitFields ) 
				)
				return false;

			const unsigned int tWidth			= tInfo.width;
			const unsigned int tHeight			= std::abs(tInfo.height);

			const unsigned int tPaletteBegin	= sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
			const unsigned int tPaletteSize		= tHeader.offBits - tPaletteBegin;

			if( aEncoded.getSize() < tPaletteBegin || 
				aEncoded.getSize() < tHeader.offBits )
				return false;

			const Palette tPalette(aEncoded.getData() + tPaletteBegin, tPaletteSize);

			const unsigned char *tDataBegin = aEncoded.getData() + tHeader.offBits;
			const unsigned int tDataSize = aEncoded.getSize() - tHeader.offBits;

			if( tDataSize != tInfo.sizeImage ||
				tWidth > tDataSize || 
				tHeight > tDataSize )
				return false;

			switch( tInfo.bitCount )
			{
			case 1:
				return loadMono(tWidth, tHeight, tDataBegin, tInfo.sizeImage, tPalette, aDecoded);

			case 4:
				return loadColor16(tWidth, tHeight, tDataBegin, tInfo.sizeImage, tPalette, aDecoded);

			case 8:
				return loadColor256(tWidth, tHeight, tDataBegin, tInfo.sizeImage, tPalette, aDecoded);

			case 16:
				if( tInfo.compression == NoCompression )
					return loadColor16_555(tWidth, tHeight, tDataBegin, tInfo.sizeImage, tPalette, aDecoded);
				else
					return loadColor16_XXX(tWidth, tHeight, tDataBegin, tInfo.sizeImage, tPalette, aDecoded);

			case 24:
				return loadColorFull24(tWidth, tHeight, tDataBegin, tInfo.sizeImage, aDecoded);

			case 32:
				return loadColorFull32(tWidth, tHeight, tDataBegin, tInfo.sizeImage, aDecoded);
			}

			return false;
		}
	};

	AUTO_REGISTER_DECODER( BmpDecoder );
}
