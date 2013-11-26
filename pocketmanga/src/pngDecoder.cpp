#include "imgDecoder.h"
#include "imgDecoderFactory.h"

#include <png.h>

//#ifdef WIN32
//#include <pngstruct.h>
//#include <pnginfo.h>
//# endif

#include <stdexcept>
#include <sstream>

#include "byteArray.h"
#include "image.h"

namespace
{
	static void img_my_png_error(png_structp /*png_ptr*/, png_const_charp error_string)
	{
		std::stringstream tInfo;
		tInfo << "Error while decoding PNG image: " << error_string;
		throw std::logic_error(tInfo.str());
	}

	static void img_my_png_warning(png_structp /*a*/, png_const_charp /*b*/)
	{
	}

	struct MemPngSrc
	{
		const tools::ByteArray& array;
		tools::ByteArray::SizeType seek;

		MemPngSrc(const tools::ByteArray& aArray)
			:array(aArray),
			seek(0)
		{
		}

	private:
		MemPngSrc(const MemPngSrc&);
		MemPngSrc operator=(const MemPngSrc&);
	};

	void ReadDataFromMemory(png_structp png_ptr, png_bytep outBytes, png_size_t byteCountToRead)
	{
		png_voidp tIoPtr = png_get_io_ptr(png_ptr);
		if( !tIoPtr )
			throw std::logic_error("Error while decoding PNG image");   // add custom error handling here

		// using spark::InputStream
		// -> replace with your own data source interface
		const tools::ByteArray& tArray				= static_cast<MemPngSrc*>(tIoPtr)->array;
		tools::ByteArray::SizeType &tSeek			= static_cast<MemPngSrc*>(tIoPtr)->seek;
		tools::ByteArray::SizeType tStillInArray	= tArray.getLength() - tSeek;

		if( tStillInArray < byteCountToRead )
			throw std::logic_error("Error while decoding PNG image");

		memcpy(outBytes, tArray.getData() + tSeek, byteCountToRead);
		tSeek += byteCountToRead;
	}

	inline bool do_use_swap()
	{

#ifndef REPACK_16
#ifdef C_LITTLE_ENDIAN

		return true;

#endif /* #ifdef C_LITTLE_ENDIAN */
#endif /* #ifndef REPACK_16 */

		return false;
	}
}

namespace img
{
	class PngDecoder: public IDecoder
	{
		virtual std::vector<std::string> getExts() const
		{
			std::vector<std::string> tExts;

			tExts.push_back("png");
			tExts.push_back("x-png");

			return tExts;
		}

		struct PngStruct
		{
			png_structp png_ptr;
			png_infop info_ptr;
			png_bytep *row_pointers;

			PngStruct()
				:png_ptr(0),
				info_ptr(0),
				row_pointers(0)
			{

			}

			~PngStruct()
			{
				if( row_pointers )
				{
					free(row_pointers);
					row_pointers = NULL;
				}

				png_structpp tPngPtr	= png_ptr?&png_ptr:NULL;
				png_infopp tInfoPtr		= info_ptr?&info_ptr:NULL;

				png_destroy_read_struct(tPngPtr, tInfoPtr, NULL);
			}

		};

		virtual bool decode(const tools::ByteArray &aEncoded, img::Image &aDecoded)
		{
			static const int PngSignatureLength = 8;

			if( aEncoded.getSize() < PngSignatureLength ||
				!png_check_sig(const_cast<png_bytep>(aEncoded.getData()), PngSignatureLength))
				return false;

			PngStruct tPng;
			tPng.png_ptr = png_create_read_struct(
				PNG_LIBPNG_VER_STRING,
				NULL, 
				img_my_png_error, 
				img_my_png_warning);

			if( !tPng.png_ptr )
				return false;

			tPng.info_ptr = png_create_info_struct(tPng.png_ptr);
			if(!tPng.info_ptr)
				return false;

			try
			{
				MemPngSrc tMemSrc(aEncoded);

				png_set_read_fn(tPng.png_ptr, &tMemSrc, ReadDataFromMemory);
				png_read_info(tPng.png_ptr, tPng.info_ptr);

				const unsigned int tBitDepth	= png_get_bit_depth(tPng.png_ptr, tPng.info_ptr);
				const unsigned int tColorType	= png_get_color_type(tPng.png_ptr, tPng.info_ptr);

				unsigned short tBytesPerPixel = 3;

				if (tColorType == PNG_COLOR_TYPE_PALETTE)
					png_set_expand(tPng.png_ptr);

				if (tColorType == PNG_COLOR_TYPE_GRAY && tBitDepth < 8) 
					png_set_expand(tPng.png_ptr);

				if ( png_get_valid(tPng.png_ptr, tPng.info_ptr, PNG_INFO_tRNS) )
				{
					png_set_expand(tPng.png_ptr);
					tBytesPerPixel++;
				}

				if (tColorType == PNG_COLOR_TYPE_GRAY ||
					tColorType == PNG_COLOR_TYPE_GRAY_ALPHA)
					png_set_gray_to_rgb(tPng.png_ptr);

				if ( tBitDepth == 16 )
				{
					if( do_use_swap() )
						png_set_swap(tPng.png_ptr);

					tBytesPerPixel *= sizeof(unsigned short);
				}

				png_set_interlace_handling(tPng.png_ptr);
				if (tColorType==PNG_COLOR_TYPE_RGB_ALPHA ||
					tColorType==PNG_COLOR_TYPE_GRAY_ALPHA)
				{
						if( tBytesPerPixel == 3 || 
							tBytesPerPixel == 3*sizeof(unsigned short))
							tBytesPerPixel=4*tBytesPerPixel/3;
				}

				png_read_update_info(tPng.png_ptr, tPng.info_ptr);

				//////////////////////////////////////////////////////////////////////////

				const unsigned int tWidth	= png_get_image_width(tPng.png_ptr, tPng.info_ptr);
				const unsigned int tHeight	= png_get_image_height(tPng.png_ptr, tPng.info_ptr);

				aDecoded.create(tWidth, tHeight, tBytesPerPixel);

				const unsigned int scanline = aDecoded.scanline();

				tPng.row_pointers = (png_bytep*) malloc(sizeof(png_bytep) * tHeight);
				unsigned char *tData = aDecoded.data();
				for (unsigned int y = 0; y < tHeight; ++y)
					tPng.row_pointers[y] = &(tData[y * scanline]);

				png_read_image(tPng.png_ptr, tPng.row_pointers);
				png_read_end(tPng.png_ptr, NULL);
			}
			catch( std::exception &)
			{
				//std::string tError = aExc.what();
				return false;
			}

			return true;
		}
	};

	//////////////////////////////////////////////////////////////////////////	

	AUTO_REGISTER_DECODER( PngDecoder );
}
