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

namespace {
void do_not_exit(j_common_ptr /*cinfo*/) {
  throw std::logic_error("Error while decoding JPEG image");
}

class JpgMemSrc: public jpeg_source_mgr {
  const tools::ByteArray &buffer_;

  static JpgMemSrc *getThis(j_decompress_ptr cinfo) {
    return static_cast<JpgMemSrc *>(cinfo->src);
  }

  static void do_init_source(j_decompress_ptr /*cinfo*/) {
  }

  static boolean do_fill_input_buffer(j_decompress_ptr /*cinfo*/) {
    return FALSE; /* We utilize I/O suspension (or emulsion? ;-) ) */
  }

  static void do_skip_input_data(j_decompress_ptr cinfo,long num_bytes) {
    getThis(cinfo)->skip_input_data_impl(num_bytes);
  }

  static void do_term_source(j_decompress_ptr /*cinfo*/) {
  }

  //////////////////////////////////////////////////////////////////////////

  void skip_input_data_impl(long num_bytes) {
      if (num_bytes > 0 && static_cast<long>(num_bytes) > bytes_in_buffer) {
      next_input_byte += bytes_in_buffer;
      bytes_in_buffer = 0;
    } else {
      next_input_byte += num_bytes;
      bytes_in_buffer -= num_bytes;
    }
  }

  JpgMemSrc(const JpgMemSrc&);
  JpgMemSrc operator=(const JpgMemSrc&);
public:
  JpgMemSrc(const tools::ByteArray &buffer)
    :buffer_(buffer) {
    init_source   = do_init_source;
    fill_input_buffer = do_fill_input_buffer;
    skip_input_data  = do_skip_input_data;
    term_source   = do_term_source;
    resync_to_restart = jpeg_resync_to_restart;

    next_input_byte  = buffer.getData();
    bytes_in_buffer  = buffer.getLength();
  }
};
}

namespace img {
class JpegDecoder: public IDecoder {
  jpeg_decompress_struct desc_;
  jpeg_error_mgr error_;

  virtual std::vector<std::string> getExts() const {
    std::vector<std::string> exts;

    exts.push_back("jpeg");
    exts.push_back("jpg");
    exts.push_back("jpe");
    exts.push_back("pjpe");
    exts.push_back("pjpeg");
    exts.push_back("pjpg");

    return exts;
  }

  virtual bool decode(const tools::ByteArray &encoded, img::Image &decoded) {
    if( encoded.isEmpty() )
      return false;

    volatile bool decompress_started = false;

    JpgMemSrc mem_src(encoded);
    desc_.src = &mem_src;

    try {
      int rc = jpeg_read_header(&desc_, TRUE);
      if (rc != JPEG_HEADER_OK)
        return false;

      decompress_started = jpeg_start_decompress(&desc_) != 0;
      if( !decompress_started ) {
        throw std::logic_error("Failed to start decompression");
      }

      decoded.create(
        desc_.output_width,
        desc_.output_height,
        static_cast<unsigned short>(desc_.num_components));

      unsigned char* dest = decoded.data();

      JSAMPARRAY pJpegBuffer
        = (*desc_.mem->alloc_sarray)((j_common_ptr) &desc_, JPOOL_IMAGE, desc_.output_width * desc_.output_components, 1);

      //const bool color = desc_.output_components > 1;

      int height = desc_.output_height;
      const int width = desc_.output_width;
      const int step  = width * desc_.num_components;

      for( ; height--; dest += step ) {
        jpeg_read_scanlines( &desc_, pJpegBuffer, 1 );
        memcpy( dest, pJpegBuffer[0], step );
      }

      jpeg_finish_decompress(&desc_);

      return true;

    } catch(...) {
      if( decompress_started ) {
        try {
          if( !jpeg_finish_decompress(&desc_) )
            throw std::logic_error("Failed to stop decompression");
        } catch(...) {
        }
      }

      return false;
    }
  }

public:
  JpegDecoder() {
    jpeg_create_decompress(&desc_);

    desc_.err = jpeg_std_error(&error_);
    desc_.err->error_exit = do_not_exit;
  }

  ~JpegDecoder() {
    jpeg_destroy_decompress(&desc_);
  }
};

//////////////////////////////////////////////////////////////////////////

AUTO_REGISTER_DECODER( JpegDecoder );
}



