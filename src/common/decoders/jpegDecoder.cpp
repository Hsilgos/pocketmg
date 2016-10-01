#include "imgDecoder.h"

#include "common/decoders/decoderCommon.h"
#include "common/byteArray.h"
#include "common/image.h"

extern "C"
{
#include <jpeglib.h>
}
#include <stdexcept>

#include "imgDecoderFactory.h"

namespace {
void do_not_exit(j_common_ptr /*cinfo*/) {
  throw std::logic_error("Error while decoding JPEG image");
}

class JpgMemSrc : public jpeg_source_mgr {
  const tools::ByteArray& buffer_;

  static JpgMemSrc* getThis(j_decompress_ptr cinfo) {
    return static_cast<JpgMemSrc*>(cinfo->src);
  }

  static void do_init_source(j_decompress_ptr /*cinfo*/) {}

  static boolean do_fill_input_buffer(j_decompress_ptr /*cinfo*/) {
    return FALSE; /* We utilize I/O suspension (or emulsion? ;-) ) */
  }

  static void do_skip_input_data(j_decompress_ptr cinfo, long num_bytes) {
    getThis(cinfo)->skip_input_data_impl(num_bytes);
  }

  static void do_term_source(j_decompress_ptr /*cinfo*/) {}

  //////////////////////////////////////////////////////////////////////////

  void skip_input_data_impl(long num_bytes) {
    if (num_bytes > 0 && static_cast<size_t>(num_bytes) > bytes_in_buffer) {
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
  JpgMemSrc(const tools::ByteArray& buffer)
    : buffer_(buffer) {
    init_source   = do_init_source;
    fill_input_buffer = do_fill_input_buffer;
    skip_input_data  = do_skip_input_data;
    term_source   = do_term_source;
    resync_to_restart = jpeg_resync_to_restart;

    next_input_byte  = buffer.getData();
    bytes_in_buffer  = buffer.getLength();
  }
};

template<int SrcBytePerPixel, int DstBytePerPixel>
class LineReader {
  public:
    static void load(img::Image& decoded, jpeg_decompress_struct& description, JSAMPARRAY& jpeg_buffer, int dst_alignment) {
        decoded.create(
                description.output_width,
                description.output_height,
                DstBytePerPixel,
                dst_alignment);

        img::DestinationIterator<DstBytePerPixel, true> dst_it(decoded);
        const int src_scanline = SrcBytePerPixel * description.output_height;

        for (unsigned short y = 0; y < description.output_height; ++y) {
          jpeg_read_scanlines(&description, jpeg_buffer, 1);
          img::SourceIterator<SrcBytePerPixel> src_it(jpeg_buffer[0], src_scanline, src_scanline);

          for (unsigned short x = 0; x < description.output_width; ++x) {
            typedef img::ColorScheme<SrcBytePerPixel, DstBytePerPixel> ColorScheme;
            typedef typename ColorScheme::DstReference DstReference;
            typedef typename ColorScheme::SrcConstReference SrcConstReference;

            DstReference(dst_it++, SrcConstReference(src_it++));
          }

          dst_it.nextLine();
        }
    }
};

typedef void (*LoadFunction)(img::Image& decoded, jpeg_decompress_struct& description, JSAMPARRAY& jpeg_buffer, int dst_alignment);
const unsigned int MaxDestinationDecodeMode = img::DecodeAsIs;
const unsigned int MaxSourceBytePerPixel = 4;

struct LoadFunctions {
  LoadFunctions() {
    for (int i = 0; i < MaxDestinationDecodeMode; ++i)
        for (int j = 0; j < MaxSourceBytePerPixel + 1; ++j)
            table[i][j] = 0;
  }
  LoadFunction table[MaxSourceBytePerPixel + 1][MaxDestinationDecodeMode + 1];
};

template<template<int SrcBytePerPixel, int DstBytePerPixel> class Algorithm>
LoadFunctions FillAlgorithmTable() {
  LoadFunctions result;

  result.table[1][img::DecodeAsIs] = &Algorithm<1, 1>::load;
  result.table[3][img::DecodeAsIs] = &Algorithm<3, 3>::load;
  result.table[4][img::DecodeAsIs] = &Algorithm<4, 4>::load;
  result.table[1][img::DecodeIntoGray] = &Algorithm<1, 1>::load;
  result.table[3][img::DecodeIntoGray] = &Algorithm<3, 1>::load;
  result.table[4][img::DecodeIntoGray] = &Algorithm<4, 1>::load;
  result.table[1][img::DecodeIntoRgb] = &Algorithm<1, 3>::load;
  result.table[3][img::DecodeIntoRgb] = &Algorithm<3, 3>::load;
  result.table[4][img::DecodeIntoRgb] = &Algorithm<4, 3>::load;
  result.table[1][img::DecodeIntoRgba] = &Algorithm<1, 4>::load;
  result.table[3][img::DecodeIntoRgba] = &Algorithm<3, 4>::load;
  result.table[4][img::DecodeIntoRgba] = &Algorithm<4, 4>::load;

  return result;
}
}

namespace img {
class JpegDecoder : public IDecoder {
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

  virtual bool decode(const tools::ByteArray& encoded, img::Image& decoded) {
    if (encoded.isEmpty())
      return false;

    volatile bool decompress_started = false;

    JpgMemSrc mem_src(encoded);
    desc_.src = &mem_src;

    try {
      int rc = jpeg_read_header(&desc_, TRUE);
      if (rc != JPEG_HEADER_OK)
        return false;

      decompress_started = jpeg_start_decompress(&desc_) != 0;
      if (!decompress_started) {
        throw std::logic_error("Failed to start decompression");
      }

      // TODO: memoryleak?
      JSAMPARRAY pJpegBuffer =
        (*desc_.mem->alloc_sarray)((j_common_ptr) & desc_, JPOOL_IMAGE, desc_.output_width * desc_.output_components, 1);

      int height = desc_.output_height;
      if (height <= 0)
        return false;

      // load(img::Image& decoded, jpeg_decompress_struct& description, JSAMPARRAY& jpeg_buffer)
      static LoadFunctions algorythms = FillAlgorithmTable<LineReader>();
      LoadFunction load_function = algorythms.table[desc_.output_components][getDecodeMode()];
      if (!load_function)
        return false;
      load_function(decoded, desc_, pJpegBuffer, getAlignment());

      jpeg_finish_decompress(&desc_);

      return true;

    } catch(...) {
      if (decompress_started) {
        try {
          if (!jpeg_finish_decompress(&desc_))
            throw std::logic_error("Failed to stop decompression");
        } catch(...) {}
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

AUTO_REGISTER_DECODER(JpegDecoder);
}
