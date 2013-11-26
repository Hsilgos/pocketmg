#include "imgDecoder.h"

#include "byteArray.h"
#include "image.h"

extern "C"
{
  #include <jpeglib.h>
}
#include <stdexcept>

#include "imgDecoderFactory.h"

//#include <Windows.h>

namespace 
{
	void do_not_exit(j_common_ptr /*cinfo*/)
	{
		throw std::logic_error("Error while decoding JPEG image");
	}

	class JpgMemSrc: public jpeg_source_mgr
	{
		const tools::ByteArray &mBuffer;

		static JpgMemSrc *getThis(j_decompress_ptr cinfo)
		{
			return static_cast<JpgMemSrc *>(cinfo->src);
		}

		static void do_init_source(j_decompress_ptr /*cinfo*/)
		{
		}

		static boolean do_fill_input_buffer(j_decompress_ptr /*cinfo*/)
		{
			return FALSE; /* We utilize I/O suspension (or emulsion? ;-) ) */
		}

		static void do_skip_input_data(j_decompress_ptr cinfo,long num_bytes)
		{
			getThis(cinfo)->skip_input_data_impl(num_bytes);
		}

		static void do_term_source(j_decompress_ptr /*cinfo*/)
		{
		}

		//////////////////////////////////////////////////////////////////////////

		void skip_input_data_impl(long num_bytes)
		{
			if( num_bytes > bytes_in_buffer )
			{
				next_input_byte += bytes_in_buffer;
				bytes_in_buffer	= 0;
			}
			else
			{
				next_input_byte += num_bytes;
				bytes_in_buffer -= num_bytes;
			}
		}

		JpgMemSrc(const JpgMemSrc&);
		JpgMemSrc operator=(const JpgMemSrc&);
	public:
		JpgMemSrc(const tools::ByteArray &aBuffer)
			:mBuffer(aBuffer)
		{
			init_source			= do_init_source;
			fill_input_buffer	= do_fill_input_buffer;
			skip_input_data		= do_skip_input_data;
			term_source			= do_term_source;
			resync_to_restart	= jpeg_resync_to_restart;

			next_input_byte		= aBuffer.getData();
			bytes_in_buffer		= aBuffer.getLength();
		}
	};
}

namespace img
{
	class JpegDecoder: public IDecoder
	{
		jpeg_decompress_struct mDesc;
		jpeg_error_mgr mError;

		virtual std::vector<std::string> getExts() const
		{
			std::vector<std::string> tExts;

			tExts.push_back("jpeg");
			tExts.push_back("jpg");
			tExts.push_back("jpe");
			tExts.push_back("pjpe");
			tExts.push_back("pjpeg");
			tExts.push_back("pjpg");

			return tExts;
		}

		virtual bool decode(const tools::ByteArray &aEncoded, img::Image &aDecoded)
		{
			if( aEncoded.isEmpty() )
				return false;

			volatile bool tDecompressStarted = false;

			JpgMemSrc tMemSrc(aEncoded);
			mDesc.src = &tMemSrc;

			try
			{
				int rc = jpeg_read_header(&mDesc, TRUE);
				if (rc != JPEG_HEADER_OK) 
					return false;

				tDecompressStarted = jpeg_start_decompress(&mDesc) != 0;
				if( !tDecompressStarted )
				{
					throw std::logic_error("Failed to start decompression");
				}

				aDecoded.create(
					mDesc.output_width,
					mDesc.output_height,
					mDesc.num_components);

				unsigned char* tDest = aDecoded.data();

				JSAMPARRAY pJpegBuffer 
					= (*mDesc.mem->alloc_sarray)((j_common_ptr) &mDesc, JPOOL_IMAGE, mDesc.output_width * mDesc.output_components, 1);

				//const bool tColor = mDesc.output_components > 1;

				int tHeight = mDesc.output_height;
				const int tWidth	= mDesc.output_width;
				const int tStep		= tWidth * mDesc.num_components;

				for( ; tHeight--; tDest += tStep )
				{
					jpeg_read_scanlines( &mDesc, pJpegBuffer, 1 );
					memcpy( tDest, pJpegBuffer[0], tStep );
				}

				jpeg_finish_decompress(&mDesc);

				return true;

			}
			catch(...)
			{
				if( tDecompressStarted )
				{
					try
					{
						if( !jpeg_finish_decompress(&mDesc) )
							throw std::logic_error("Failed to stop decompression");
					}
					catch(...)
					{
					}
				}

				return false;
			}						
		}

	public:
		JpegDecoder()
		{
			jpeg_create_decompress(&mDesc);

			mDesc.err = jpeg_std_error(&mError);
			mDesc.err->error_exit = do_not_exit;
		}

		~JpegDecoder()
		{
			jpeg_destroy_decompress(&mDesc);
		}
	};

	//////////////////////////////////////////////////////////////////////////	

	AUTO_REGISTER_DECODER( JpegDecoder );
}



