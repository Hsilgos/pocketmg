#pragma once

#include <string>

#include "byteArray.h"

#include "defines.h"

#include "color.h"

namespace tools {
class ByteArray;
}

namespace fs {
class FilePath;
}

namespace utils {
struct Rect;
}

namespace img {
/*
Class has 'copy on write' ideology, i.e.
buffer will be copied at time when source or destination will try to change buffer.
for example:

Image src;
src.create(800, 600, 3);

Image dst = src;
// src and dst points to same buffer.

dst[0] = Rgb(255, 255, 255);
// dst will copy it's buffer from src
*/

class Image {
  tools::ByteArray data_;

  utils::Size   size_;
  unsigned short  depth_;

  bool enable_min_realloc_;
public:
  typedef utils::Size::SizeType SizeType;

  Image();
  ~Image();

  Image(SizeType width, SizeType height, unsigned short depth);

  bool load(const tools::ByteArray &buffer);
  bool load(const std::string &file_ext, const tools::ByteArray &buffer);

  static Image loadFrom(const tools::ByteArray &buffer);
  static Image loadFrom(const std::string &file_ext, const tools::ByteArray &buffer);

  void create(SizeType width, SizeType height, unsigned short depth);
  void createSame(const Image &other);
  void destroy();
  bool empty() const;

  const utils::Size &getSize() const;
  SizeType width() const;
  SizeType height() const;
  unsigned short depth() const;
  SizeType scanline() const;

  void setDepth(unsigned short depth);
  void setWidth(SizeType width);
  void setHeight(SizeType height);

  void swap(Image &other);

  color::Rgba getPixel(SizeType x, SizeType y) const;
  void setPixel(SizeType x, SizeType y, const color::Rgba& rgb);
  void setPixel(SizeType x, SizeType y, const color::Gray& grey);

  //void moveTo(Image &other);
  //void copyTo(Image &other) const;

  void enableMinimumReallocations(bool enable);

  // offset in bytes
  unsigned char *data(SizeType offset = 0);
  const unsigned char *data(SizeType offset = 0) const;

  static const Image emptyImage;

//  ibitmap *native();
//  const ibitmap *native() const;
};

utils::Rect getRect(const Image &src);

bool toGray(const Image &src, Image &dst);

bool toBgr(const Image &src, Image &dst);

bool copyRect(const img::Image &src, img::Image &dst, const utils::Rect &rect_to_copy);
void copy(const img::Image &src, img::Image &dst);
Image::SizeType dataSize(const img::Image &img);
}

