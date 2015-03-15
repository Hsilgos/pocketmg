#include "imgDecoder.h"
#include "imgDecoderFactory.h"

#include "forbid_copy_assign.h"
#include "byteArray.h"

#include "image.h"

#include "static_assert.h"

namespace {
#pragma pack(push,2)

struct BITMAPFILEHEADER {
  unsigned short  type;
  unsigned long   size;
  unsigned short  reserved1;
  unsigned short  reserved2;
  unsigned long  offBits;
};

#pragma pack(pop)


struct BITMAPINFOHEADER {
  unsigned long size;
  long   width;
  long   height;
  unsigned short planes;
  unsigned short bitCount;
  unsigned long compression;
  unsigned long sizeImage;
  long   xPelsPerMeter;
  long   yPelsPerMeter;
  unsigned long clrUsed;
  unsigned long clrImportant;
};

static const unsigned short BmpType = 0x4d42; // 'MB'

enum Compreesion {
  NoCompression = 0, // BI_RGB,
  BitFields  = 3  // BI_BITFIELDS
};

struct Palette {
  const unsigned char *data;
  const unsigned int size;

  const unsigned char *getColor(unsigned int index) const {
    unsigned int real_idx = index * 4;
    if( real_idx < size )
      return &data[real_idx];

    return 0;
  }

  Palette(const unsigned char *data, unsigned int size)
    :data(data), size(size) {
  }

private:
    FORBID_ASSIGN(Palette);
};
}


namespace img {
class BmpDecoder: public IDecoder {
public:
  virtual std::vector<std::string> getExts() const {
    std::vector<std::string> exts;

    exts.push_back("bmp");

    return exts;
  }

  unsigned int correctScanline(unsigned int scanline) {
    while( scanline % 4 != 0 )
      ++scanline;

    return scanline;
  }

  bool loadMono(
    unsigned int width,
    unsigned int height,
    const unsigned char* src_begin,
    unsigned long data_size,
    const Palette &pallete,
    img::Image &decoded) {
    decoded.create(width, height, 3);

    const unsigned int src_scanline = correctScanline(width/8 + ((width%8 != 0)?1:0));

    if( src_scanline * height > data_size )
      return false;

    unsigned char* dst = decoded.data();

    for( unsigned short y = 0; y < height; ++y) {
      const unsigned char* src_line = &src_begin[( height - y - 1 )*src_scanline];

      for( unsigned short x = 0; x < width; ++x) {
        const unsigned char* src = &src_line[x/8];
        const bool bit = ( (*src >> (7 - x % 8)) & 1 );
        const unsigned char *bgra = pallete.getColor(bit?1:0);
        if( bgra ) {
          dst[0] = bgra[2];
          dst[1] = bgra[1];
          dst[2] = bgra[0];

          dst += 3;
        } else {
          return 0;
        }
      }
    }

    return true;
  }

  bool loadColor16(
    unsigned int width,
    unsigned int height,
    const unsigned char* src_begin,
    unsigned long data_size,
    const Palette &pallete,
    img::Image &decoded) {
    decoded.create(width, height, 3);

    const unsigned int src_scanline = correctScanline(width/2 + ((width % 2) !=  0 ? 1 : 0) );

    if( src_scanline * height > data_size )
      return false;

    unsigned char* dst = decoded.data();

    for( unsigned short y = 0; y < height; ++y) {
      const unsigned char* src_line = &src_begin[( height - y - 1 )*src_scanline];

      for( unsigned short x = 0; x < width; ++x) {
        const unsigned char pix_pair = src_line[x/2];
        const unsigned char index = (x&1)?(pix_pair&0xF):(pix_pair >> 4);

        const unsigned char *bgra = pallete.getColor(index);

        if( bgra ) {
          //bgr(a) ->rgb
          dst[0] = bgra[2];
          dst[1] = bgra[1];
          dst[2] = bgra[0];

          dst += 3;
        } else {
          return false;
        }
      }
    }

    return true;
  }

  bool loadColor256(
    unsigned int width,
    unsigned int height,
    const unsigned char* src_begin,
    unsigned long data_size,
    const Palette &pallete,
    img::Image &decoded) {
    decoded.create(width, height, 3);

    const unsigned int src_scanline = correctScanline(width);

    if( src_scanline * height > data_size )
      return false;

    unsigned char* dst = decoded.data();

    for( unsigned short y = 0; y < height; ++y) {
      const unsigned char* src_line = &src_begin[( height - y - 1 )*src_scanline];

      for( unsigned short x = 0; x < width; ++x) {
        unsigned char src = src_line[x];

        const unsigned char *bgra = pallete.getColor(src);

        if( bgra ) {
          //bgr(a) ->rgb
          dst[0] = bgra[2];
          dst[1] = bgra[1];
          dst[2] = bgra[0];

          dst += 3;
        } else {
          return false;
        }
      }
    }

    return true;
  }

  bool loadColor16_XXX(
    unsigned int width,
    unsigned int height,
    const unsigned short* src_begin,
    unsigned long data_size,
    img::Image &decoded,
    unsigned short mask_red,
    unsigned short mask_green,
    unsigned short mask_blue) {
    decoded.create(width, height, 3);

    const unsigned int src_scanline = correctScanline(width * 2);

    if( src_scanline * height > data_size )
      return false;

    unsigned short r_off = 0, g_off = 0, b_off = 0;
    while( !((mask_red >> r_off) & 1) )
      ++r_off;
    while( !((mask_green >> g_off) & 1) )
      ++g_off;
    while( !((mask_blue >> b_off) & 1) )
      ++b_off;

    const unsigned int r_mult = 256 / (mask_red >> r_off);
    const unsigned int g_mult = 256 / (mask_green >> g_off);
    const unsigned int b_mult = 256 / (mask_blue >> b_off);

    unsigned char* dst = decoded.data();

    for( unsigned short y = 0; y < height; ++y) {
      const unsigned short* src_line = &src_begin[( height - y - 1 )*(src_scanline/2)];

      for( unsigned short x = 0; x < width; ++x) {
        unsigned short src = src_line[x];

        dst[0] = ((src & mask_red) >> r_off) * r_mult;
        dst[1] = ((src & mask_green) >> g_off) * g_mult;
        dst[2] = ((src & mask_blue) >> b_off) * b_mult;

        dst += 3;
      }
    }

    return true;
  }

  bool loadColor16_555(
    unsigned int width,
    unsigned int height,
    const unsigned char* src_begin,
    unsigned long data_size,
    const Palette &/*pallete*/,
    img::Image &decoded) {
    unsigned short mask_blue  = 0x1F;
    unsigned short mask_green = mask_blue << 5;
    unsigned short mask_red  = mask_blue << 10;

    return loadColor16_XXX(
             width,
             height,
             reinterpret_cast<const unsigned short *>(src_begin),
             data_size,
             decoded,
             mask_red,
             mask_green,
             mask_blue);
  }

  bool loadColor16_XXX(
    unsigned int width,
    unsigned int height,
    const unsigned char* src_begin,
    unsigned long data_size,
    const Palette &pallete,
    img::Image &decoded) {
    if( pallete.size < 3 )
      return false;

    unsigned short mask_red = *(reinterpret_cast<const unsigned short *>(pallete.getColor(0)));
    unsigned short mask_green = *(reinterpret_cast<const unsigned short *>(pallete.getColor(1)));
    unsigned short mask_blue = *(reinterpret_cast<const unsigned short *>(pallete.getColor(2)));


    return loadColor16_XXX(
             width,
             height,
             reinterpret_cast<const unsigned short *>(src_begin),
             data_size,
             decoded,
             mask_red,
             mask_green,
             mask_blue);
  }

  bool loadColorFull(
    unsigned int width,
    unsigned int height,
    const unsigned char* src_begin,
    unsigned long data_size,
    unsigned short byte_per_pixel,
    img::Image &decoded) {
    decoded.create(width, height, byte_per_pixel );

    const unsigned int src_scanline = correctScanline(width * byte_per_pixel);

    if( src_scanline * height > data_size )
      return false;

    unsigned char* dst = decoded.data();

    for( unsigned short y = 0; y < height; ++y) {
      const unsigned char* src = &src_begin[( height - y - 1 ) * src_scanline];

      for( unsigned short x = 0; x < width; ++x) {
        dst[0] = src[2];
        dst[1] = src[1];
        dst[2] = src[0];

        if( 4 == byte_per_pixel )
          dst[3] = src[3];

        dst += byte_per_pixel;
        src += byte_per_pixel;
      }
    }

    return true;
  }

  bool loadColorFull24(
    unsigned int width,
    unsigned int height,
    const unsigned char* src_begin,
    unsigned long data_size,
    img::Image &decoded) {
    return loadColorFull(width, height, src_begin, data_size, 3, decoded);
  }

  bool loadColorFull32(
    unsigned int width,
    unsigned int height,
    const unsigned char* src_begin,
    unsigned long data_size,
    img::Image &decoded) {
    return loadColorFull(width, height, src_begin, data_size, 4, decoded);
  }

  virtual bool decode(const tools::ByteArray &encoded, img::Image &decoded) {
    STATIC_ASSERT( (sizeof(BITMAPFILEHEADER) == 14), Header_size_mismatch );
    STATIC_ASSERT( (sizeof(BITMAPINFOHEADER) == 40), Info_size_mismatch );

    // TODO: check size
    BITMAPFILEHEADER header = {0};
    if( !fromByteArray(encoded, header) ||
        header.type != BmpType )
      return false;

    BITMAPINFOHEADER info = {0};
    if( !fromByteArray(encoded, sizeof(BITMAPFILEHEADER), info) )
      return false;

    if( info.compression != NoCompression &&
        !( info.bitCount == 16 && info.compression == BitFields )
      )
      return false;

    const unsigned int width   = info.width;
    const unsigned int height   = std::abs(info.height);

    const unsigned int palette_begin = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
    const unsigned int palette_size  = header.offBits - palette_begin;

    if( encoded.getSize() < palette_begin ||
        encoded.getSize() < header.offBits )
      return false;

    const Palette palette(encoded.getData() + palette_begin, palette_size);

    const unsigned char *data_begin = encoded.getData() + header.offBits;
    const unsigned int data_size = encoded.getSize() - header.offBits;

    if( data_size != info.sizeImage ||
        width > data_size ||
        height > data_size )
      return false;

    switch( info.bitCount ) {
    case 1:
      return loadMono(width, height, data_begin, info.sizeImage, palette, decoded);

    case 4:
      return loadColor16(width, height, data_begin, info.sizeImage, palette, decoded);

    case 8:
      return loadColor256(width, height, data_begin, info.sizeImage, palette, decoded);

    case 16:
      if( info.compression == NoCompression )
        return loadColor16_555(width, height, data_begin, info.sizeImage, palette, decoded);
      else
        return loadColor16_XXX(width, height, data_begin, info.sizeImage, palette, decoded);

    case 24:
      return loadColorFull24(width, height, data_begin, info.sizeImage, decoded);

    case 32:
      return loadColorFull32(width, height, data_begin, info.sizeImage, decoded);
    }

    return false;
  }
};

AUTO_REGISTER_DECODER( BmpDecoder );
}
