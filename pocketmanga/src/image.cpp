#include "image.h"

#include "byteArray.h"
#include "filepath.h"
#include "imgDecoderFactory.h"
#include "defines.h"

#include <stdexcept>

namespace {
inline size_t dataSize(
  img::Image::SizeType width,
  img::Image::SizeType height,
  unsigned short depth) {
  return width * height * depth;
}
}

namespace img {
bool Image::load(const tools::ByteArray &buffer) {
  return load(utils::EmptyString, buffer);
}

bool Image::load(const std::string &file_ext, const tools::ByteArray &buffer) {
  return DecoderFactory::getInstance().decode(file_ext, buffer, *this);
}

Image Image::loadFrom(const tools::ByteArray &buffer) {
  Image result;
  if( result.load(buffer) )
    return result;

  return Image::emptyImage;
}

Image Image::loadFrom(const std::string &file_ext, const tools::ByteArray &buffer) {
  Image result;
  if( result.load(file_ext, buffer) )
    return result;

  return Image::emptyImage;
}

Image::Image()
  :enable_min_realloc_(false),
   depth_(0) {
}

Image::Image(SizeType width, SizeType height, unsigned short depth)
  :enable_min_realloc_(false) {
  create(width, height, depth);
}

Image::~Image() {
  destroy();
}

void Image::create(SizeType width, SizeType height, unsigned short depth) {
  const size_t new_size = ::dataSize(width, height, depth);
  if( 0 == new_size )
    return;

  if( !enable_min_realloc_ || new_size > data_.getSize() )
    data_.resize(new_size);

  size_ =		utils::Size(width, height);
  depth_		= depth;
}

void Image::createSame(const Image &other) {
  create(other.width(), other.height(), other.depth());
}

const utils::Size &Image::getSize() const {
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

void Image::setDepth(unsigned short depth) {
  depth_ = depth;
}

void Image::setWidth(SizeType width) {
  size_.width = width;
}

void Image::setHeight(SizeType height) {
  size_.height = height;
}

Image::SizeType Image::scanline() const {
  return size_.width * depth_;
}

void Image::destroy() {
  data_.reset();
}

unsigned char *Image::data(SizeType offset) {
  return empty()?0:(data_.askBuffer(data_.getLength()) + offset);
}

const unsigned char *Image::data(SizeType offset) const {
  return empty()?0:(data_.getData() + offset);
}

void Image::swap(Image &other) {
  std::swap(*this, other);
}

void Image::enableMinimumReallocations(bool enable) {
  enable_min_realloc_ = enable;
}

color::Rgba Image::getPixel(SizeType x, SizeType y) const {
  SizeType dest_pixel = y * size_.width + x;
  SizeType dest_pos	= dest_pixel * depth_;

  switch( depth_ ) {
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
  SizeType dest_pos	= dest_pixel * depth_;

  switch( depth_ ) {
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
  SizeType dest_pos	= dest_pixel * depth_;

  switch( depth_ ) {
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
utils::Rect getRect(const Image &src) {
  return utils::Rect(0, 0, src.width(), src.height());
}

bool toGray(const Image &src, Image &dst) {
  if( src.empty() )
    return false;

  if( &src != &dst )
    dst.create( src.width(), src.height(), 1 );

  const unsigned short bytes_per_pixel = src.depth();
  if( 3 != bytes_per_pixel && 4 != bytes_per_pixel )
    return false;

  const unsigned char *line_src = src.data();
  unsigned char *line_dst = dst.data();

  const Image::SizeType  total_iters = src.height() * src.width();

  for( Image::SizeType i = 0; i < total_iters; ++i, line_src += bytes_per_pixel, ++line_dst ) {
    color::RgbConstRef rgb(line_src);
    color::GrayRef(line_dst, rgb);
  }

  if( &src == &dst )
    dst.setDepth(1);

  return true;
}

bool toBgr(const Image &src, Image &dst) {
  if( src.empty() )
    return false;

  const unsigned int bytes_per_pixel = src.depth();

  if( bytes_per_pixel != 3 && bytes_per_pixel != 4 )
    return false;

  dst = src;

  const Image::SizeType  total_iters = src.height() * src.width();
  unsigned char *line_src = dst.data();
  for( Image::SizeType i = 0; i < total_iters; ++i, line_src += bytes_per_pixel )
    color::RgbRef(line_src).swap(color::RgbRef::RedIndex, color::RgbRef::BlueIndex);

  return true;
}

bool copyRect(const img::Image &src, img::Image &dst, const utils::Rect &rect_to_copy) {
  utils::Rect rect = restrictBy(rect_to_copy, getRect(src));

  if( rect.width <= 0|| rect.height <= 0)
    return false;

  const unsigned short bytes_per_pixel = src.depth();

  if( &src != &dst )
    dst.create(rect.width, rect.height, bytes_per_pixel);

  const int scan_line = rect.width * bytes_per_pixel;

  for( int i = rect.y; i < rect.y + rect.height; ++i) {
    const int dst_position = (i - rect.y)* scan_line;
    unsigned char *line_dst = dst.data( dst_position );

    const int src_position = (i * src.width() + rect.x) * bytes_per_pixel;
    const unsigned char *line_src = src.data( src_position );

    memcpy(line_dst, line_src, scan_line);
  }

  if( &src == &dst ) {
    dst.setWidth(rect.width);
    dst.setHeight(rect.height);
  }

  return true;
}

void copy(const img::Image &src, img::Image &dst) {
  dst.createSame(src);
  memcpy(dst.data(), src.data(), dataSize(src));
}

Image::SizeType dataSize(const img::Image &img) {
  return ::dataSize(img.width(), img.height(), img.depth());
}
}

