#pragma once

#include "common/forbid_copy_assign.h"
#include "common/image.h"

#include <exception>

namespace img {

class DestinationIsFull : public std::exception {
public:
  DestinationIsFull() : std::exception() {}
};

template<size_t BytesPerPixel, bool IsReversed>
class DestinationIterator {
public:
  DestinationIterator(img::Image& decoded)
      : destination_begin_(decoded.data()),
        scanline_with_align_(decoded.scanline(true)),
        scanline_no_align_(decoded.scanline(false)),
        image_height_(decoded.height()),
        total_dst_size_(scanline_with_align_ * image_height_),
        destination_end_(destination_begin_ + total_dst_size_),
        destination_line_begin_(IsReversed ? destination_end_ - scanline_with_align_ : destination_begin_),
        destination_line_end_(destination_line_begin_ + scanline_with_align_),
        destination_iterator_(destination_line_begin_) {
  }

  void completeLine() {
    for (; destination_iterator_ < destination_line_end_; ++destination_iterator_) {
      CheckIterator(destination_iterator_);
      *destination_iterator_ = 0;
    }
  }

  void finish() {
    while(destination_iterator_ >= destination_begin_ &&
          destination_iterator_ < destination_end_) {
      completeLine();
      iterateLine();
    }
  }

  void nextLine() {
    completeLine();
    iterateLine();
  }

  void iterateLine() {
    if (IsReversed) {
      destination_line_begin_ -= scanline_with_align_;
    } else {
      destination_line_begin_ += scanline_with_align_;
    }

    destination_line_end_ = destination_line_begin_ + scanline_with_align_;
    destination_iterator_ = destination_line_begin_;
  }

  void skip(unsigned char offset_x, unsigned char offset_y) {
    const unsigned int delta_offset_lines = offset_y * scanline_with_align_;
    const unsigned int delta_offset_total = delta_offset_lines + offset_x * BytesPerPixel;
    if (IsReversed) {
      destination_iterator_ -= delta_offset_total;
      destination_line_begin_ -= delta_offset_lines;
    } else {
      destination_iterator_ += delta_offset_total;
      destination_line_begin_ += delta_offset_lines;
    }

    destination_line_end_ = destination_line_begin_ + scanline_with_align_;
  }

  bool isPixelInsideLine(int pixel_offset) const {
    const unsigned char* const byte_offset = destination_iterator_ +  BytesPerPixel * pixel_offset;
    return byte_offset >= destination_line_begin_ && byte_offset <= destination_line_end_;
  }

  unsigned char* operator++(int) {
    unsigned char* result = destination_iterator_;
    destination_iterator_ += BytesPerPixel;
    CheckIterator(result);
    return result;
  }

private:
  void CheckIterator(unsigned char* it) const {
    if (it < destination_line_begin_ || it >= destination_end_)
      throw DestinationIsFull();
  }

  DestinationIterator (const DestinationIterator&);
  DestinationIterator& operator = (const DestinationIterator&);

  unsigned char* const destination_begin_;
  const img::Image::SizeType scanline_with_align_;
  const img::Image::SizeType scanline_no_align_;
  const img::Image::SizeType image_height_;
  const img::Image::SizeType total_dst_size_;
  unsigned char* const destination_end_;

  unsigned char* destination_line_begin_;
  unsigned char* destination_line_end_;
  unsigned char* destination_iterator_;
};

template<size_t SrcBytePerPixel>
class SourceIterator {
public:
  SourceIterator(const unsigned char* src_begin, img::Image::SizeType src_size, img::Image::SizeType src_scanline)
    : src_begin_(src_begin), src_line_(src_begin), src_iterator_(src_line_), src_end_(src_begin + src_size), src_scanline_(src_scanline) {
  }

  bool isValid() const {
    return src_line_ < src_end_;
  }

  void nextLine() {
    src_line_ += src_scanline_;
    src_iterator_ = src_line_;
  }

  const unsigned char* operator++(int) {
    const unsigned char* result = src_iterator_;
    src_iterator_ += SrcBytePerPixel;
    return result;
  }

  const unsigned char* operator++() {
    src_iterator_ += SrcBytePerPixel;
    return src_iterator_;
  }

  void skip(int count_to_skip) {
    src_iterator_ += count_to_skip;
  }

  const unsigned char* at(size_t pos) {
    return &src_line_[pos];
  }

  bool isEnd() const {
    return src_iterator_ >= src_end_;
  }

  bool isEnd(bool strict) const {
    return strict ? src_iterator_ == src_end_ : isEnd();
  }

  bool isPixelInside(int pixel_offset) const {
    const unsigned char* const byte_offset = src_iterator_ + SrcBytePerPixel * pixel_offset;
    return byte_offset >= src_begin_ && byte_offset < src_end_;
  }

private:
  SourceIterator(const SourceIterator&);
  SourceIterator& operator = (const SourceIterator&);

  const unsigned char* src_begin_;
  const unsigned char* src_line_;
  const unsigned char* src_iterator_;
  const unsigned char* src_end_;
  const img::Image::SizeType src_scanline_;
};

template<int SrcBytePerPixel, int DstBytePerPixel>
struct ColorScheme {};

template<>
struct ColorScheme<1, 1> {
  typedef color::GrayConstRef SrcConstReference;
  typedef color::GrayRef DstReference;
};

template<>
struct ColorScheme<1, 3> {
  typedef color::GrayConstRef SrcConstReference;
  typedef color::RgbRef DstReference;
};

template<>
struct ColorScheme<1, 4> {
  typedef color::GrayConstRef SrcConstReference;
  typedef color::RgbaRef DstReference;
};

template<>
struct ColorScheme<3, 1> {
  typedef color::BgrConstRef SrcConstReference;
  typedef color::GrayRef DstReference;
};

template<>
struct ColorScheme<4, 1> {
  typedef color::BgraConstRef SrcConstReference;
  typedef color::GrayRef DstReference;
};

template<>
struct ColorScheme<3, 3> {
  typedef color::BgrConstRef SrcConstReference;
  typedef color::RgbRef DstReference;
};

template<>
struct ColorScheme<3, 4> {
  typedef color::BgrConstRef SrcConstReference;
  typedef color::RgbaRef DstReference;
};

template<>
struct ColorScheme<4, 3> {
  typedef color::BgraConstRef SrcConstReference;
  typedef color::RgbRef DstReference;
};

template<>
struct ColorScheme<4, 4> {
  typedef color::BgraConstRef SrcConstReference;
  typedef color::RgbaRef DstReference;
};

template<int DstBytePerPixel>
inline void CopyPixelDataForSrc3(unsigned char* dst, const unsigned char* src) {
  typedef ColorScheme<3, DstBytePerPixel> ColorScheme;
  typedef typename ColorScheme::DstReference DstReference;
  typedef typename ColorScheme::SrcConstReference SrcConstReference;
  DstReference(dst, SrcConstReference(src));
}
}
