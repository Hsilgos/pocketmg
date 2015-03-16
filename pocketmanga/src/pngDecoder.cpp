#include "imgDecoder.h"
#include "imgDecoderFactory.h"

#include <png.h>

#include <stdexcept>
#include <sstream>

#include "byteArray.h"
#include "image.h"

namespace {
static void img_my_png_error(png_structp /*png_ptr*/, png_const_charp error_string) {
  std::stringstream info;
  info << "Error while decoding PNG image: " << error_string;
  throw std::logic_error(info.str());
}

static void img_my_png_warning(png_structp /*a*/, png_const_charp /*b*/) {}

struct MemPngSrc {
  const tools::ByteArray& array;
  tools::ByteArray::SizeType seek;

  MemPngSrc(const tools::ByteArray& array)
    : array(array), seek(0) {}

private:
  MemPngSrc(const MemPngSrc&);
  MemPngSrc operator=(const MemPngSrc&);
};

void ReadDataFromMemory(png_structp png_ptr, png_bytep outBytes, png_size_t byteCountToRead) {
  png_voidp io_ptr = png_get_io_ptr(png_ptr);
  if (!io_ptr)
    throw std::logic_error("Error while decoding PNG image");   // add custom error handling here

  // using spark::InputStream
  // -> replace with your own data source interface
  const tools::ByteArray& array    = static_cast<MemPngSrc*>(io_ptr)->array;
  tools::ByteArray::SizeType& seek   = static_cast<MemPngSrc*>(io_ptr)->seek;
  tools::ByteArray::SizeType still_in_array = array.getLength() - seek;

  if (still_in_array < byteCountToRead)
    throw std::logic_error("Error while decoding PNG image");

  memcpy(outBytes, array.getData() + seek, byteCountToRead);
  seek += byteCountToRead;
}

inline bool do_use_swap() {

#ifndef REPACK_16
#ifdef C_LITTLE_ENDIAN

  return true;

#endif /* #ifdef C_LITTLE_ENDIAN */
#endif /* #ifndef REPACK_16 */

  return false;
}
}

namespace img {
class PngDecoder : public IDecoder {
  virtual std::vector<std::string> getExts() const {
    std::vector<std::string> exts;

    exts.push_back("png");
    exts.push_back("x-png");

    return exts;
  }

  struct PngStruct {
    png_structp png_ptr;
    png_infop info_ptr;
    png_bytep* row_pointers;

    PngStruct()
      : png_ptr(0), info_ptr(0), row_pointers(0) {}

    ~PngStruct() {
      if (row_pointers) {
        free(row_pointers);
        row_pointers = NULL;
      }

      png_structpp png_ptr_copy = png_ptr ? &png_ptr : NULL;
      png_infopp info_ptr_copy = info_ptr ? &info_ptr : NULL;

      png_destroy_read_struct(png_ptr_copy, info_ptr_copy, NULL);
    }

  };

  virtual bool decode(const tools::ByteArray& encoded, img::Image& decoded) {
    static const int PngSignatureLength = 8;

    if (encoded.getSize() < PngSignatureLength ||
        !png_check_sig(const_cast<png_bytep>(encoded.getData()), PngSignatureLength))
      return false;

    PngStruct png;
    png.png_ptr = png_create_read_struct(
      PNG_LIBPNG_VER_STRING,
      NULL,
      img_my_png_error,
      img_my_png_warning);

    if (!png.png_ptr)
      return false;

    png.info_ptr = png_create_info_struct(png.png_ptr);
    if (!png.info_ptr)
      return false;

    try {
      MemPngSrc mem_src(encoded);

      png_set_read_fn(png.png_ptr, &mem_src, ReadDataFromMemory);
      png_read_info(png.png_ptr, png.info_ptr);

      const unsigned int bit_depth = png_get_bit_depth(png.png_ptr, png.info_ptr);
      const unsigned int color_type = png_get_color_type(png.png_ptr, png.info_ptr);

      unsigned short bytes_per_pixel = 3;

      if (color_type == PNG_COLOR_TYPE_PALETTE)
        png_set_expand(png.png_ptr);

      if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)
        png_set_expand(png.png_ptr);

      if (png_get_valid(png.png_ptr, png.info_ptr, PNG_INFO_tRNS)) {
        png_set_expand(png.png_ptr);
        bytes_per_pixel++;
      }

      if (color_type == PNG_COLOR_TYPE_GRAY ||
          color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
        png_set_gray_to_rgb(png.png_ptr);

      if (bit_depth == 16) {
        if (do_use_swap())
          png_set_swap(png.png_ptr);

        bytes_per_pixel *= sizeof(unsigned short);
      }

      png_set_interlace_handling(png.png_ptr);
      if (color_type == PNG_COLOR_TYPE_RGB_ALPHA ||
          color_type == PNG_COLOR_TYPE_GRAY_ALPHA) {
        if (bytes_per_pixel == 3 ||
            bytes_per_pixel == 3 * sizeof(unsigned short))
          bytes_per_pixel = 4 * bytes_per_pixel / 3;
      }

      png_read_update_info(png.png_ptr, png.info_ptr);

      //////////////////////////////////////////////////////////////////////////

      const unsigned int width = png_get_image_width(png.png_ptr, png.info_ptr);
      const unsigned int height = png_get_image_height(png.png_ptr, png.info_ptr);

      decoded.create(width, height, bytes_per_pixel);

      const unsigned int scanline = decoded.scanline();

      png.row_pointers = (png_bytep*) malloc(sizeof(png_bytep) * height);
      unsigned char* data = decoded.data();
      for (unsigned int y = 0; y < height; ++y)
        png.row_pointers[y] = &(data[y * scanline]);

      png_read_image(png.png_ptr, png.row_pointers);
      png_read_end(png.png_ptr, NULL);
    } catch(std::exception&) {
      //std::string error = exc.what();
      return false;
    }

    return true;
  }
};

//////////////////////////////////////////////////////////////////////////

AUTO_REGISTER_DECODER(PngDecoder);
}
