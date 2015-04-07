#include "mirror.h"

#include "image.h"

namespace {
void mirrorSame(img::Image& image, const img::MirrorType type) {
  const img::Image::SizeType height = image.height();
  const img::Image::SizeType width = image.width();
  const img::Image::SizeType scanline_with_align = image.scanline(true);
  const img::Image::SizeType scanline = image.scanline(false);
  const unsigned short bytes_per_pixel = image.depth();

  if (img::UpsideDown == type) {
    unsigned char* line_src = image.data();
    unsigned char* line_dst = image.data() + scanline_with_align * (height - 1);

    for (img::Image::SizeType h = 0; h < height / 2; ++h) {
      unsigned char* line_src_it = line_src;
      unsigned char* line_dst_it = line_dst;

      for (img::Image::SizeType w = 0; w < scanline; ++w)
        std::swap(*line_src_it++, *line_dst_it++);

      line_src += scanline_with_align;
      line_dst -= scanline_with_align;
    }
  } else {
    unsigned char* line = image.data();
    for (img::Image::SizeType h = 0; h < height; ++h) {
      unsigned char* line_begin = line;
      unsigned char* line_end = line + scanline - bytes_per_pixel;
      for (img::Image::SizeType w = 0; w < width / 2; ++w) {
        for (unsigned short p = 0; p < bytes_per_pixel; ++p)
          std::swap(*(line_begin + p), *(line_end + p));
        line_begin += bytes_per_pixel;
        line_end -= bytes_per_pixel;
      }
      line += scanline_with_align;
    }
  }
}

void mirrorToOther(const img::Image& src, img::Image& dst, const img::MirrorType type) {
  dst.createSame(src);

  const img::Image::SizeType height = src.height();
  const img::Image::SizeType width = src.width();
  const img::Image::SizeType scanline_with_align = src.scanline(true);
  const img::Image::SizeType scanline = src.scanline(false);
  const unsigned short bytes_per_pixel = src.depth();
  if (img::UpsideDown == type) {
    const unsigned char* line_src = src.data();
    unsigned char* line_dst = dst.data() + scanline_with_align * (height - 1);

    for (img::Image::SizeType h = 0; h < height; ++h) {
      const unsigned char* line_src_it = line_src;
      unsigned char* line_dst_it = line_dst;

      for (img::Image::SizeType w = 0; w < scanline; ++w)
        *line_dst_it++ = *line_src_it++;

      line_src += scanline_with_align;
      line_dst -= scanline_with_align;
    }
  } else {
    const unsigned char* line_src = src.data();
    unsigned char* line_dst = dst.data();
    for (img::Image::SizeType h = 0; h < height; ++h) {
      const unsigned char* line_src_begin = line_src;
      unsigned char* line_dst_end = line_dst + scanline - bytes_per_pixel;
      for (img::Image::SizeType w = 0; w < width; ++w) {
        for (unsigned short p = 0; p < bytes_per_pixel; ++p)
          *(line_dst_end + p) = *(line_src_begin + p);
        line_src_begin += bytes_per_pixel;
        line_dst_end -= bytes_per_pixel;
      }
      line_src += scanline_with_align;
      line_dst += scanline_with_align;
    }
  }
}

} // namespace

namespace img {
void mirror(const img::Image& src, img::Image& dst, MirrorType type) {
  const bool is_same_image = (&dst == &src);
  if (is_same_image)
    mirrorSame(dst, type);
  else
    mirrorToOther(src, dst, type);
}
} // namespace img
