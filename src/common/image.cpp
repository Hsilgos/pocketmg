#include "image.h"

#include "byteArray.h"
#include "filepath.h"
#include "imgDecoderFactory.h"
#include "defines.h"

#include <stdexcept>

namespace {
inline img::Image::SizeType scanlineToAlign(img::Image::SizeType scanline, size_t align) {
  return scanline % align ? ((scanline / align + 1) * align) : scanline;
}

inline size_t calcScanline(img::Image::SizeType width,
                           unsigned short depth,
                           size_t align,
                           bool with_alignment) {
  const img::Image::SizeType scanline = width * depth;
  return with_alignment ? scanlineToAlign(scanline, align) : scanline;
}

inline size_t dataSize(img::Image::SizeType width,
                       img::Image::SizeType height,
                       unsigned short depth,
                       size_t align) {
  return static_cast<size_t>(height) * calcScanline(width, depth, align, true);
}

const size_t MaximumImageSize = 1024 * 1024 * 1024; // 1 gigabyte
}

namespace img {
bool Image::load(const tools::ByteArray& buffer) {
  return load(utils::EmptyString, buffer);
}

bool Image::load(const std::string& file_ext, const tools::ByteArray& buffer) {
  return DecoderFactory::getInstance().decode(file_ext, buffer, *this);
}

Image Image::loadFrom(const tools::ByteArray& buffer) {
  Image result;
  if (result.load(buffer))
    return result;

  return Image::emptyImage;
}

Image Image::loadFrom(const std::string& file_ext, const tools::ByteArray& buffer) {
  Image result;
  if (result.load(file_ext, buffer))
    return result;

  return Image::emptyImage;
}

Image::Image()
  : enable_min_realloc_(false), depth_(0) {}

Image::Image(SizeType width, SizeType height, unsigned short depth, size_t align)
  : enable_min_realloc_(false) {
  create(width, height, depth, align);
}

Image::~Image() {
  destroy();
}

void Image::create(SizeType width, SizeType height, unsigned short depth, size_t align) {
  if (depth != 1 && depth != 3 && depth != 4)
      throw std::invalid_argument("Depth must be 1, 3 or 4");

  if (height > MaximumImageSize / depth || width > MaximumImageSize / depth || height * depth > MaximumImageSize / width)
      throw std::bad_alloc();

  const size_t new_size = ::dataSize(width, height, depth, align);
  if (0 == new_size)
    return;

  if (!enable_min_realloc_ || new_size > data_.getSize())
    data_.resize(new_size);

  size_ = utils::Size(width, height);
  depth_ = depth;
  align_ = align;
}

void Image::createSame(const Image& other) {
  create(other.width(), other.height(), other.depth(), other.alignment());
}

const utils::Size& Image::getSize() const {
  return size_;
}

Image::SizeType Image::width() const {
  return size_.width;
}

Image::SizeType Image::height() const {
  return size_.height;
}

unsigned short Image::depth() const {
  return depth_;
}

size_t Image::alignment() const {
  return align_;
}

void Image::setDepth(unsigned short depth) {
  depth_ = depth;
}

void Image::setWidth(SizeType width) {
  size_.width = width;
}

void Image::setHeight(SizeType height) {
  size_.height = height;
}

Image::SizeType Image::scanline(bool with_alignment) const {
  return calcScanline(size_.width, depth_, align_, with_alignment);
}

void Image::destroy() {
  data_.reset();
}

unsigned char* Image::data(SizeType offset) {
  return empty() ? 0 : (data_.askBuffer(data_.getLength()) + offset);
}

const unsigned char* Image::data(SizeType offset) const {
  return empty() ? 0 : (data_.getData() + offset);
}

void Image::swap(Image& other) {
  std::swap(*this, other);
}

void Image::enableMinimumReallocations(bool enable) {
  enable_min_realloc_ = enable;
}

color::Rgba Image::getPixel(SizeType x, SizeType y) const {
  SizeType const dest_pos = y * scanline(true) + x * depth_;
  //SizeType dest_pixel = y * size_.width + x;
  //SizeType dest_pos = dest_pixel * depth_;

  switch (depth_) {
  case 1:
    return color::GrayConstRef(&data_[dest_pos]);
  case 3:
    return color::RgbConstRef(&data_[dest_pos]);
  case 4:
    return color::RgbaConstRef(&data_[dest_pos]);
  default:
    throw std::logic_error("You can't get pixel with such depth");
  }
}

void Image::setPixel(SizeType x, SizeType y, const color::Rgba& rgb) {
  SizeType dest_pixel = y * size_.width + x;
  SizeType dest_pos = dest_pixel * depth_;

  switch (depth_) {
  case 1:
    color::GrayRef(&data_[dest_pos], rgb);
    break;
  case 4:
    color::RgbaRef(&data_[dest_pos], rgb);
    break;
  case 3:
    color::RgbRef(&data_[dest_pos], rgb);
    break;
  default:
    throw std::logic_error("You can't set pixel with such depth");
  }
}

void Image::setPixel(SizeType x, SizeType y, const color::Gray& grey) {
  SizeType dest_pixel = y * size_.width + x;
  SizeType dest_pos = dest_pixel * depth_;

  switch (depth_) {
  case 1:
    color::GrayRef(&data_[dest_pos], grey);
    break;
  case 4:
    color::RgbaRef(&data_[dest_pos], grey);
    break;
  case 3:
    color::RgbRef(&data_[dest_pos], grey);
    break;
  default:
    throw std::logic_error("You can't set pixel with such depth");
  }
}

const Image Image::emptyImage;

bool Image::empty() const {
  //return bitmap_ == 0;
  return data_.isEmpty();
}

//////////////////////////////////////////////////////////////////////////
utils::Rect getRect(const Image& src) {
  return utils::Rect(0, 0, src.width(), src.height());
}

bool toGray(const Image& src, Image& dst) {
  if (src.empty())
    return false;

  if (&src != &dst)
    dst.create(src.width(), src.height(), 1);

  const unsigned short bytes_per_pixel = src.depth();
  if (3 != bytes_per_pixel && 4 != bytes_per_pixel)
    return false;

  const unsigned char* line_src = src.data();
  unsigned char* line_dst = dst.data();

  const Image::SizeType total_iters = src.height() * src.width();

  for (Image::SizeType i = 0; i < total_iters; ++i, line_src += bytes_per_pixel, ++line_dst)
    color::GrayRef(line_dst, color::RgbConstRef(line_src));

  if (&src == &dst)
    dst.setDepth(1);

  return true;
}

bool toBgr(const Image& src, Image& dst) {
  if (src.empty())
    return false;

  const unsigned int bytes_per_pixel = src.depth();

  if (bytes_per_pixel != 3 && bytes_per_pixel != 4)
    return false;

  dst = src;

  const Image::SizeType scanline_with_align = src.scanline(true);
  const Image::SizeType scanline = src.scanline(false);
  const Image::SizeType height = src.height();

  unsigned char* src_begin = dst.data();
  for (Image::SizeType h = 0; h < height; ++h, src_begin += scanline_with_align) {
    unsigned char* line_src = src_begin;
    for (Image::SizeType s = 0; s < scanline; s += bytes_per_pixel, line_src += bytes_per_pixel) {
      color::RgbRef(line_src).swap(color::RgbRef::RedIndex, color::RgbRef::BlueIndex);
    }
  }

  return true;
}

bool rgba2rgb(const Image& src, Image& dst) {
  if (src.empty())
    return false;

  const unsigned int bpp_src = src.depth();
  const unsigned int bpp_dst = 3;
  if (bpp_src != 4)
    return false;

  const Image::SizeType scanline_with_align = src.scanline(true);
  const Image::SizeType scanline = src.scanline(false);
  const Image::SizeType height = src.height();
  const unsigned char* src_begin = src.data();

  dst.create(src.width(), src.height(), bpp_dst, src.alignment());

  const Image::SizeType dst_line_with_align = dst.scanline(true);
  unsigned char* dst_begin = dst.data();

  for (Image::SizeType h = 0; h < height; ++h, src_begin += scanline_with_align, dst_begin += dst_line_with_align) {
    const unsigned char* line_src = src_begin;
    unsigned char* line_dst = dst_begin;
    for (Image::SizeType s = 0; s < scanline; s += bpp_src, line_src += bpp_src, line_dst += bpp_dst)
      color::RgbRef rgb(line_dst, color::RgbaConstRef(line_src));
  }

  return true;
}

bool rgb2rgba(const Image& src, Image& dst) {
    if (src.empty())
      return false;

    const unsigned int bpp_src = src.depth();
    const unsigned int bpp_dst = 4;
    if (bpp_src != 3)
      return false;

    const Image::SizeType scanline_with_align = src.scanline(true);
    const Image::SizeType scanline = src.scanline(false);
    const Image::SizeType height = src.height();
    const unsigned char* src_begin = src.data();

    dst.create(src.width(), src.height(), bpp_dst, src.alignment());

    const Image::SizeType dst_line_with_align = dst.scanline(true);
    unsigned char* dst_begin = dst.data();

    for (Image::SizeType h = 0; h < height; ++h, src_begin += scanline_with_align, dst_begin += dst_line_with_align) {
      const unsigned char* line_src = src_begin;
      unsigned char* line_dst = dst_begin;
      for (Image::SizeType s = 0; s < scanline; s += bpp_src, line_src += bpp_src, line_dst += bpp_dst)
        color::RgbaRef rgba(line_dst, color::RgbConstRef(line_src));
    }

    return true;
}

bool grey2rgba(const Image& src, Image& dst) {
    if (src.empty())
      return false;

    const unsigned int bpp_src = src.depth();
    const unsigned int bpp_dst = 4;
    if (bpp_src != 1)
      return false;

    const Image::SizeType scanline_with_align = src.scanline(true);
    const Image::SizeType scanline = src.scanline(false);
    const Image::SizeType height = src.height();
    const unsigned char* src_begin = src.data();

    dst.create(src.width(), src.height(), bpp_dst, src.alignment());

    const Image::SizeType dst_line_with_align = dst.scanline(true);
    unsigned char* dst_begin = dst.data();

    for (Image::SizeType h = 0; h < height; ++h, src_begin += scanline_with_align, dst_begin += dst_line_with_align) {
      const unsigned char* line_src = src_begin;
      unsigned char* line_dst = dst_begin;
      for (Image::SizeType s = 0; s < scanline; s += bpp_src, line_src += bpp_src, line_dst += bpp_dst)
        color::RgbaRef rgba(line_dst, color::GrayConstRef(line_src));
    }

    return true;
}

bool copyRect(const img::Image& src, img::Image& dst, const utils::Rect& rect_to_copy) {
  utils::Rect rect = restrictBy(rect_to_copy, getRect(src));

  if (rect.width <= 0 || rect.height <= 0)
    return false;

  const unsigned short bytes_per_pixel = src.depth();

  if (&src != &dst)
    dst.create(rect.width, rect.height, bytes_per_pixel);

  const int scan_line = rect.width * bytes_per_pixel;

  for (int i = rect.y; i < rect.y + rect.height; ++i) {
    const int dst_position = (i - rect.y) * scan_line;
    unsigned char* line_dst = dst.data(dst_position);

    const int src_position = (i * src.width() + rect.x) * bytes_per_pixel;
    const unsigned char* line_src = src.data(src_position);

    memcpy(line_dst, line_src, scan_line);
  }

  if (&src == &dst) {
    dst.setWidth(rect.width);
    dst.setHeight(rect.height);
  }

  return true;
}

void copy(const img::Image& src, img::Image& dst) {
  dst.createSame(src);
  memcpy(dst.data(), src.data(), dataSize(src));
}

Image::SizeType dataSize(const img::Image& img) {
  return ::dataSize(img.width(), img.height(), img.depth(), img.alignment());
}

bool areValidDimentions(Image::SizeType width, Image::SizeType height, unsigned short depth, size_t align) {
  if (width == 0 || height == 0 || depth < 1 || depth > 4)
    return false;

  if (std::numeric_limits<size_t>::max() / width / depth < height)
    return false;

  const size_t scanline = calcScanline(width, depth, align, true);
  return std::numeric_limits<size_t>::max() / scanline > height;
}

Image::SizeType correctScanline(Image::SizeType scanline, size_t align) {
  return scanlineToAlign(scanline, align);
}
} // namespace img
