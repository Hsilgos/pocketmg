#include "imgDecoder.h"
#include "imgDecoderFactory.h"

#include "forbid_copy_assign.h"
#include "byteArray.h"

#include "debugUtils.h"
#include "image.h"

#include "static_assert.h"

#include <cassert>
#include <cmath>
#include <exception>
#include <iostream>

#if 1
namespace std {
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef int int32_t;
} //namespace std
#endif

namespace {
#pragma pack(push,2)

struct BitmapFileHeader {
  std::uint16_t type;
  std::uint32_t size;
  std::uint16_t reserved1;
  std::uint16_t reserved2;
  std::uint32_t offBits;
};

#pragma pack(pop)


struct BitmapInfoHeader {
  std::uint32_t size;
  std::int32_t width;
  std::int32_t height;
  std::uint16_t planes;
  std::uint16_t bitCount;
  std::uint32_t compression;
  std::uint32_t sizeImage;
  std::int32_t xPelsPerMeter;
  std::int32_t yPelsPerMeter;
  std::uint32_t clrUsed;
  std::uint32_t clrImportant;
};

struct BitmapInfoHeaderV3Unique {
  std::uint32_t mask_red;
  std::uint32_t mask_green;
  std::uint32_t mask_blue;
  std::uint32_t mask_alpha;
};

struct BitmapInfoHeaderV3 {
  BitmapInfoHeader v1;
  BitmapInfoHeaderV3Unique v3;
};

struct CIEXYZ {
  std::int32_t ciexyz_x;
  std::int32_t ciexyz_y;
  std::int32_t ciexyz_z;
};

struct CIEXYZTRIPLE {
  CIEXYZ ciexyz_red;
  CIEXYZ ciexyz_green;
  CIEXYZ ciexyz_blue;
};

struct BitmapInfoHeaderV4Unique {
  std::uint32_t type;
  CIEXYZTRIPLE endpoints;
  std::uint32_t gamma_red;
  std::uint32_t gamma_green;
  std::uint32_t gamma_blue;
};

struct BitmapInfoHeaderV4 {
  BitmapInfoHeader v1;
  BitmapInfoHeaderV3Unique v3;
  BitmapInfoHeaderV4Unique v4;
};

struct BitmapInfoHeaderV5Unique {
  std::uint32_t intent;
  std::uint32_t profile_data;
  std::uint32_t profile_size;
  std::uint32_t reserved;
};

struct BitmapInfoHeaderV5 {
  BitmapInfoHeader v1;
  BitmapInfoHeaderV3Unique v3;
  BitmapInfoHeaderV4Unique v4;
  BitmapInfoHeaderV5Unique v5;
};

static const unsigned short BmpType = 0x4d42; // 'MB'

class PalleteError : public std::exception {
public:
  PalleteError() : std::exception() {}
};

struct Palette {
  const unsigned char* data;
  const unsigned int size;

  const unsigned char* getColor(unsigned int index) const {
    unsigned int real_idx = index * 4;
    if (real_idx < size)
      return &data[real_idx];

    throw PalleteError();
  }

  Palette(const unsigned char* data, unsigned int size)
    : data(data), size(size) {}

private:
  FORBID_ASSIGN(Palette)
};

enum Compreesion {
  NoCompression = 0, // BI_RGB,
  Rle8 = 1,
  Rle4 = 2,
  BitFields = 3  // BI_BITFIELDS
};

class HeaderParser {
public:
  enum HeaderVersion {
    Undefined,
    V1 = sizeof(BitmapInfoHeader),
    V3 = sizeof(BitmapInfoHeaderV3),
    V4 = sizeof(BitmapInfoHeaderV4),
    V5 = sizeof(BitmapInfoHeaderV5)
  };

  HeaderParser(const tools::ByteArray& encoded)
    : encoded_(encoded), parsed_successfully_(false), file_header_(NULL), header_version_(Undefined), file_info_basic_header_(NULL), palette_begin_(0), palette_size_(0) {
    STATIC_ASSERT((sizeof(BitmapFileHeader) == 14), Header_size_mismatch);
    STATIC_ASSERT((sizeof(BitmapInfoHeader) == 40), Info_size_mismatch);
    STATIC_ASSERT((sizeof(BitmapInfoHeaderV3) == 56), Info_size_mismatch_version_3);
    STATIC_ASSERT((sizeof(BitmapInfoHeaderV4) == 108), Info_size_mismatch_version_4);
    STATIC_ASSERT((sizeof(BitmapInfoHeaderV5) == 124), Info_size_mismatch_version_5);

    parsed_successfully_ = parse(encoded);
  }

  unsigned int paletteMaxSizeForBits() const {
    switch (file_info_basic_header_->v1.bitCount) {
    case 1:
      return 2;
    case 4:
      return 16;
    case 8:
      return 256;
    case 16:
      return 3;
    }
    return 0;
  }

  bool is_valid(size_t align) const {
    if (!parsed_successfully_)
      return false;

    const short bits = file_info_basic_header_->v1.bitCount;
    const short bpp = bits == 32 ? 4 : 3; // only 3 and 4 depth are currently supported

    if (file_info_basic_header_->v1.width <= 0)
      return false;

    const bool copression_is_valid =
        file_info_basic_header_->v1.compression == NoCompression ||
        (8 == bits && file_info_basic_header_->v1.compression == Rle8) ||
        (4 == bits && file_info_basic_header_->v1.compression == Rle4) ||
        file_info_basic_header_->v1.compression == BitFields;

    const bool dimentions_are_small = img::areValidDimentions(
        file_info_basic_header_->v1.width,
        std::abs(file_info_basic_header_->v1.height), bpp, align);

    const bool palette_is_suite =
        file_info_basic_header_->v1.clrUsed <= paletteMaxSizeForBits();
    
    const bool important_are_less_used =
        file_info_basic_header_->v1.clrImportant <=
        file_info_basic_header_->v1.clrUsed;

    return copression_is_valid &&
           important_are_less_used &&
           dimentions_are_small &&
           palette_is_suite;
  }

  bool is_reversed() const {
    return file_info_basic_header_->v1.height < 0;
  }

  utils::Size image_size() const {
    return utils::Size(file_info_basic_header_->v1.width, std::abs(file_info_basic_header_->v1.height));
  }

  Palette palette() const {
    return Palette(encoded_.getData() + palette_begin_, palette_size_);
  }

  const unsigned char* data_begin() const {
    return encoded_.getData() + file_header_->offBits;
  }

  unsigned int data_size() const {
    return encoded_.getSize() - file_header_->offBits;
  }

  unsigned int image_size_in_byte() const {
    if (0 != file_info_basic_header_->v1.sizeImage)
      return file_info_basic_header_->v1.sizeImage;

    if (NoCompression == compression()) {
      return file_header_->size > file_header_->offBits
          ? file_header_->size - file_header_->offBits : 0;
    }

    return 0;
  }

  unsigned short bit_count() const {
    return file_info_basic_header_->v1.bitCount;
  }

  std::uint32_t compression() const {
    return file_info_basic_header_->v1.compression;
  }

  HeaderVersion header_version() const {
    return header_version_;
  }

  unsigned short mask_impl(std::uint32_t BitmapInfoHeaderV3Unique::*field) const {
    return header_version() >= V3
        ? static_cast<unsigned short>(file_info_basic_header_->v3.*field)
        : 0;
  }

  unsigned short mask_red() const {
    return mask_impl(&BitmapInfoHeaderV3Unique::mask_red);
  }

  unsigned short mask_green() const {
    return mask_impl(&BitmapInfoHeaderV3Unique::mask_green);
  }

  unsigned short mask_blue() const {
    return mask_impl(&BitmapInfoHeaderV3Unique::mask_blue);
  }

  unsigned short mask_alpha() const {
    return mask_impl(&BitmapInfoHeaderV3Unique::mask_alpha);
  }

private:
  HeaderParser(const HeaderParser&);
  HeaderParser& operator =(const HeaderParser&);

  HeaderVersion parse_version(const tools::ByteArray& encoded) {
    std::uint32_t header_size = 0;
    if (!fromByteArray(encoded, sizeof(BitmapFileHeader), header_size))
      return Undefined;

    if (V1 == header_size || V3 == header_size || V4 == header_size || V5 == header_size)
      return static_cast<HeaderVersion>(header_size);

    return Undefined;
  }

  unsigned int header_info_size(HeaderVersion version) const {
    return version == Undefined ? 0 : version;
  }

  bool parse(const tools::ByteArray& encoded) {
    if (encoded.getSize() < sizeof(BitmapFileHeader))
      return false;

    file_header_ = static_cast<const BitmapFileHeader*>(encoded.getBuffer());
    if (file_header_->type != BmpType)
      return false;

    header_version_ = parse_version(encoded);
    if (Undefined == header_version_)
      return false;

    const unsigned int header_info_pos = sizeof(BitmapFileHeader);
    if (encoded.getSize() < header_info_pos + header_info_size(header_version_))
      return false;

    file_info_basic_header_ = reinterpret_cast<const BitmapInfoHeaderV3*>(encoded.getData() + header_info_pos);

    palette_begin_ = sizeof(BitmapFileHeader) + sizeof(BitmapInfoHeader);
    palette_size_ = file_header_->offBits - palette_begin_;
    if (encoded.getSize() < palette_begin_ || encoded.getSize() < file_header_->offBits)
      return false;

    return true;
  }

  const tools::ByteArray& encoded_;
  bool parsed_successfully_;
  const BitmapFileHeader* file_header_;
  HeaderVersion header_version_;
  const BitmapInfoHeaderV3* file_info_basic_header_;

  unsigned int palette_begin_;
  unsigned int palette_size_;
};
}

class DestinationIsFull : public std::exception {
public:
  DestinationIsFull() : std::exception() {}
};

namespace {
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

enum DecryptAlgorythm {
  DAMono = 0,
  DA16Colors,
  DA16ColorsRLE4,
  DA256Colors,
  DA256ColorsRLE8,
  DA16BitsNoAlpha,
  DA16BitsWithAlpha,
  DA24Bits,
  DA32Bits,
  DA32BitsSkipFirst,
  DAUnknown
};

struct LoadImageData {
  LoadImageData(const utils::Size& size_in,
                const unsigned char* src_begin_in,
                std::uint32_t data_size_in,
                const Palette& pallete_in,
                const bool is_reversed_in,
                img::DecodeMode decode_mode,
                size_t align_in)
      : decrypt_algorythm(DAUnknown),
        size(size_in),
        src_begin(src_begin_in),
        data_size(data_size_in),
        pallete(pallete_in),
        decode_mode_(decode_mode),
        align(align_in),
        is_reversed(is_reversed_in),
        mask_red(0),
        mask_green(0),
        mask_blue(0),
        mask_alpha(0) {
  }

  DecryptAlgorythm decrypt_algorythm;
  utils::Size size;
  const unsigned char* src_begin;
  std::uint32_t data_size;
  Palette pallete;
  img::DecodeMode decode_mode_;
  const size_t align;
  const bool is_reversed;

  unsigned short mask_red;
  unsigned short mask_green;
  unsigned short mask_blue;
  unsigned short mask_alpha;
};

inline unsigned int bmpSrcScanline(unsigned int scanline) {
  return img::correctScanline(scanline, 4);
}

// MONO
struct ExpandedByte {
  ExpandedByte(unsigned char byte) {
    unsigned char mask = 1;
    for (unsigned int i = 8; i > 0; --i) {
      expaned_byte[i - 1] = ((byte & mask) == mask);
      mask = mask << 1;
    }
  }

  bool expaned_byte[8];
};

template<int DstBytePerPixel>
inline void CopyPixelDataForSrc3(unsigned char* dst, const unsigned char* src) {
  typedef ColorScheme<3, DstBytePerPixel> ColorScheme;
  typedef typename ColorScheme::DstReference DstReference;
  typedef typename ColorScheme::SrcConstReference SrcConstReference;
  DstReference(dst, SrcConstReference(src));
}

template<int DstBytePerPixel, bool IsReversed>
struct Mono {
  static bool load(img::Image& decoded, const LoadImageData& data) {
    decoded.create(data.size.width, data.size.height, DstBytePerPixel, data.align);
    DestinationIterator<DstBytePerPixel, IsReversed> dst_it(decoded);

    const unsigned int src_scanline = bmpSrcScanline(data.size.width / 8 + ((data.size.width % 8 != 0) ? 1 : 0));

    if (src_scanline * data.size.height > data.data_size)
      return false;

    SourceIterator<1> src_it(data.src_begin, data.data_size, src_scanline);

    for (unsigned short y = 0; y < data.size.height; ++y) {
      if (!src_it.isValid())
        return false;
      unsigned short step_x = 0;
      for (unsigned short x = 0; x < data.size.width; x += step_x) {
        ExpandedByte byte(*src_it++);
        step_x = std::min(8u, data.size.width - x);
        for (unsigned int i = 0; i < step_x; ++i) {
          const unsigned char* bgr = data.pallete.getColor(byte.expaned_byte[i] ? 1 : 0);
          CopyPixelDataForSrc3<DstBytePerPixel>(dst_it++, bgr);
        }
      }

      dst_it.nextLine();
      src_it.nextLine();
    }

    return true;
  }
};

template<int SrcBytePerPixel, int DstBytePerPixel, bool IsReversed, bool SkipFirstByte>
struct ColorFullBase {
  static bool load(img::Image& decoded, const LoadImageData& data) {
    decoded.create(data.size.width, data.size.height, DstBytePerPixel, data.align);
    DestinationIterator<DstBytePerPixel, IsReversed> dst_it(decoded);

    const unsigned int src_scanline = bmpSrcScanline(data.size.width * SrcBytePerPixel);

    if (src_scanline * data.size.height > data.data_size)
      return false;

    SourceIterator<SrcBytePerPixel> src_it(data.src_begin, data.data_size, src_scanline);

    for (unsigned short y = 0; y < data.size.height; ++y) {
      if (SkipFirstByte)
        src_it.skip(1);

      if (!src_it.isValid())
        return false;

      for (unsigned short x = 0; x < data.size.width; ++x) {
        typedef ColorScheme<(SrcBytePerPixel - (SkipFirstByte ? 1 : 0)), DstBytePerPixel> ColorScheme;
        typedef typename ColorScheme::DstReference DstReference;
        typedef typename ColorScheme::SrcConstReference SrcConstReference;

        DstReference(dst_it++, SrcConstReference(src_it++));
      }

      dst_it.nextLine();
      src_it.nextLine();
    }

    return true;
  }
};

template<int DstBytePerPixel, bool IsReversed>
struct ColorFull24 : public ColorFullBase<3, DstBytePerPixel, IsReversed, false> {
};

template<int DstBytePerPixel, bool IsReversed>
struct ColorFull32 : public ColorFullBase<4, DstBytePerPixel, IsReversed, false> {
};

template<int DstBytePerPixel, bool IsReversed>
struct ColorFull32SkipFirst : public ColorFullBase<4, DstBytePerPixel, IsReversed, true> {
};

template<int DstBytePerPixel, bool IsReversed>
struct Color256 {
  static bool load(img::Image& decoded, const LoadImageData& data) {
    decoded.create(data.size.width, data.size.height, DstBytePerPixel, data.align);
    DestinationIterator<DstBytePerPixel, IsReversed> dst_it(decoded);

    const unsigned int src_scanline = bmpSrcScanline(data.size.width);

    if (src_scanline * data.size.height > data.data_size)
      return false;

    SourceIterator<1> src_it(data.src_begin, data.data_size, src_scanline);

    for (unsigned short y = 0; y < data.size.height; ++y) {
      if (!src_it.isValid())
        return false;

      for (unsigned short x = 0; x < data.size.width; ++x) {
        const unsigned char* bgr = data.pallete.getColor(*src_it++);
        CopyPixelDataForSrc3<DstBytePerPixel>(dst_it++, bgr);
      }

      dst_it.nextLine();
      src_it.nextLine();
    }

    return true;
  }
};

template<int DstBytePerPixel, bool IsReversed>
struct Color16 {
  static void loadFromPallete(DestinationIterator<DstBytePerPixel, IsReversed>& dst_it,
                              const Palette& pallete,
                              unsigned char index) {
    const unsigned char* bgr = pallete.getColor(index);
    CopyPixelDataForSrc3<DstBytePerPixel>(dst_it++, bgr);
  }

  static bool load(img::Image& decoded, const LoadImageData& data) {
    decoded.create(data.size.width, data.size.height, DstBytePerPixel, data.align);
    DestinationIterator<DstBytePerPixel, IsReversed> dst_it(decoded);

    const unsigned int src_scanline = bmpSrcScanline(data.size.width / 2 + ((data.size.width % 2) != 0 ? 1 : 0));

    if (src_scanline * data.size.height > data.data_size)
      return false;

    SourceIterator<1> src_it(data.src_begin, data.data_size, src_scanline);

    for (unsigned short y = 0; y < data.size.height; ++y) {
      if (!src_it.isValid())
        return false;

      for (unsigned short x = 0; x < data.size.width - 1; x += 2) {
        const unsigned char pix_pair = *src_it++;
        loadFromPallete(dst_it, data.pallete, pix_pair >> 4);
        loadFromPallete(dst_it, data.pallete, pix_pair & 0xF);
      }

      if (data.size.width & 1) {
        const unsigned char pix_pair = *src_it++;
        loadFromPallete(dst_it, data.pallete, pix_pair >> 4);
      }

      dst_it.nextLine();
      src_it.nextLine();
    }

    return true;
  }
};

template<int DstBytePerPixel, bool IsReversed, bool WithAlpha>
struct Color16XXXBase {
  static bool load(img::Image& decoded, const LoadImageData& data) {
    decoded.create(data.size.width, data.size.height, DstBytePerPixel, data.align);
    DestinationIterator<DstBytePerPixel, IsReversed> dst_it(decoded);

    const unsigned int src_scanline = bmpSrcScanline(data.size.width * 2);

    if (src_scanline * data.size.height > data.data_size ||
        0 == data.mask_red || 0 == data.mask_green || 0 == data.mask_blue)
      return false;

    const bool has_alpha = WithAlpha && data.mask_alpha != 0;

    unsigned short r_off = 0, g_off = 0, b_off = 0, a_off = 0;
    while (!((data.mask_red >> r_off) & 1))
      ++r_off;
    while (!((data.mask_green >> g_off) & 1))
      ++g_off;
    while (!((data.mask_blue >> b_off) & 1))
      ++b_off;

    if (has_alpha) {
      while (!((data.mask_alpha >> a_off) & 1))
        ++a_off;
    }

    const double r_mult = 255.0 / (data.mask_red >> r_off);
    const double g_mult = 255.0 / (data.mask_green >> g_off);
    const double b_mult = 255.0 / (data.mask_blue >> b_off);
    const double a_mult = has_alpha ? 255.0 / (data.mask_alpha >> a_off) : 0;

    unsigned char bgr_buff[4];

    for (unsigned short y = 0; y < data.size.height; ++y) {
      const unsigned short* src_line = reinterpret_cast<const unsigned short*>(&data.src_begin[y * src_scanline]);

      for (unsigned short x = 0; x < data.size.width; ++x) {
        unsigned short src = src_line[x];

        bgr_buff[0] = static_cast<unsigned char>(std::floor(((src & data.mask_blue) >> b_off) * b_mult + 0.5));
        bgr_buff[1] = static_cast<unsigned char>(std::floor(((src & data.mask_green) >> g_off) * g_mult + 0.5));
        bgr_buff[2] = static_cast<unsigned char>(std::floor(((src & data.mask_red) >> r_off) * r_mult + 0.5));

        if (WithAlpha)
          bgr_buff[3] = static_cast<unsigned char>(std::floor(((src & data.mask_alpha) >> a_off) * a_mult + 0.5));

        typedef ColorScheme<(WithAlpha ? 4 : 3), DstBytePerPixel> ColorScheme;
        typedef typename ColorScheme::DstReference DstReference;
        typedef typename ColorScheme::SrcConstReference SrcConstReference;
        DstReference(dst_it++, SrcConstReference(bgr_buff));
      }

      dst_it.nextLine();
    }

    return true;
  }
};

template<int DstBytePerPixel, bool IsReversed>
struct Color16XXXAlpha : public Color16XXXBase<DstBytePerPixel, IsReversed, true> {
};

template<int DstBytePerPixel, bool IsReversed>
struct Color16XXXNoAlpha : public Color16XXXBase<DstBytePerPixel, IsReversed, false> {
};

template<int DstBytePerPixel, bool IsReversed>
struct Color16_555 {
  static bool load(img::Image& decoded, const LoadImageData& data) {
    LoadImageData new_data = data;
    new_data.mask_blue = 0x1F;
    new_data.mask_green = new_data.mask_blue << 5;
    new_data.mask_red = new_data.mask_blue << 10;
    return Color16XXXNoAlpha<DstBytePerPixel, IsReversed>::load(decoded, new_data);
  }
};

template<int DstBytePerPixel, bool IsReversed>
struct Color16XXX {
  static bool load(img::Image& decoded, const LoadImageData& data) {
    if (data.pallete.size < 3)
      return false;

    LoadImageData new_data = data;

    new_data.mask_red = *(reinterpret_cast<const unsigned short*>(new_data.pallete.getColor(0)));
    new_data.mask_green = *(reinterpret_cast<const unsigned short*>(new_data.pallete.getColor(1)));
    new_data.mask_blue = *(reinterpret_cast<const unsigned short*>(new_data.pallete.getColor(2)));
    new_data.mask_alpha = static_cast<short>(~0) ^ static_cast<short>(new_data.mask_red | new_data.mask_green | new_data.mask_blue);
    if (0 != new_data.mask_alpha)
      return Color16XXXAlpha<DstBytePerPixel, IsReversed>::load(decoded, new_data);
    else
      return Color16XXXNoAlpha<DstBytePerPixel, IsReversed>::load(decoded, new_data);
  }
};

template<template<int DstBytePerPixel, bool IsReversed> class RleLoader, int DstBytePerPixel, bool IsReversed>
struct RLEBase {
  static bool load(img::Image& decoded, const LoadImageData& data) {
    decoded.create(data.size.width, data.size.height, DstBytePerPixel, data.align);
    DestinationIterator<DstBytePerPixel, IsReversed> dst_it(decoded);

    enum RleControlBytes {
      EndOfLine = 0,
      EndOfBitmap = 1,
      Delta = 2
    };

    SourceIterator<1> src_it(data.src_begin, data.data_size, 0);

    typedef RleLoader<DstBytePerPixel, IsReversed> RleLoaderType;

    while (!src_it.isEnd()) {
      const unsigned char current_byte = *src_it++;
      if (src_it.isEnd())
        return false;

      if (current_byte > 0) {
        if (!RleLoaderType::Encoded(current_byte, src_it, dst_it, data.pallete))
          return false;
      } else {
        unsigned char command_byte = *src_it++;
        switch (command_byte) {
        case EndOfLine:
          dst_it.nextLine();
          break;
        case EndOfBitmap:
          dst_it.finish();
          return src_it.isEnd(true);
        case Delta: {
            if (!src_it.isPixelInside(2))
              return false;

            unsigned char offset_x = *src_it++;
            unsigned char offset_y = *src_it++;

            dst_it.skip(offset_x, offset_y);
          }
          break;
        default:
          if (!RleLoaderType::Absolute(command_byte, src_it, dst_it, data.pallete))
            return false;
          break;
        }
      }
    }

    // EndOfBitmap was not met. It's not so critical error
    return true;/*false*/;
  }
};

template<int DstBytePerPixel, bool IsReversed>
struct Rle8Loader {
  static bool Absolute(unsigned int count_of_bytes,
                       SourceIterator<1>& src_it,
                       DestinationIterator<DstBytePerPixel, IsReversed>& dst_it,
                       const Palette& pallete) {
    if (!dst_it.isPixelInsideLine(count_of_bytes) || !src_it.isPixelInside(count_of_bytes))
      return false;
    for (unsigned int i = 0; i < count_of_bytes; ++i) {
      const unsigned char *bgr = pallete.getColor(*src_it++);
      if (!bgr)
        return false;

      CopyPixelDataForSrc3<DstBytePerPixel>(dst_it++, bgr);
    }
    if (count_of_bytes & 1)
      ++src_it;

    return true;
  }

  static bool Encoded(unsigned int count_of_pixels,
                      SourceIterator<1>& src_it,
                      DestinationIterator<DstBytePerPixel, IsReversed>& dst_it,
                      const Palette& pallete) {
    const unsigned char *color_to_fill = pallete.getColor(*src_it++);
    if (!dst_it.isPixelInsideLine(count_of_pixels))
      return false;

    for (unsigned int i = 0; i < count_of_pixels; ++i)
      CopyPixelDataForSrc3<DstBytePerPixel>(dst_it++, color_to_fill);

    return true;
  }
};

template<int DstBytePerPixel, bool IsReversed>
struct RLE8 : public RLEBase<Rle8Loader, DstBytePerPixel, IsReversed> {
};

template<int DstBytePerPixel, bool IsReversed>
struct Rle4Loader {
  static bool DoLoad(DestinationIterator<DstBytePerPixel, IsReversed>& dst_it,
                    const Palette& pallete,
                    unsigned char index) {
    const unsigned char *bgr = pallete.getColor(index);
    CopyPixelDataForSrc3<DstBytePerPixel>(dst_it++, bgr);
    return true;
  }

  static bool Absolute(unsigned int count_of_bytes,
                       SourceIterator<1>& src_it,
                       DestinationIterator<DstBytePerPixel, IsReversed>& dst_it,
                       const Palette& pallete) {
    unsigned char count_of_bytes_src = count_of_bytes / 2 + (count_of_bytes & 1);
    if (!dst_it.isPixelInsideLine(count_of_bytes) || !src_it.isPixelInside(count_of_bytes_src))
      return false;
    unsigned char two_colors = 0;
    for (unsigned int i = 0; i < count_of_bytes - 1; i += 2) {
      two_colors = *src_it++;
      if (!DoLoad(dst_it, pallete, two_colors >> 4) ||
          !DoLoad(dst_it, pallete, two_colors & 0xF))
          return false;
    }

    // Last byte
    if (!DoLoad(dst_it, pallete, *src_it++ >> 4))
      return false;

    if (count_of_bytes_src & 1)
      ++src_it;

    return true;
  }

  static bool Encoded(unsigned int count_of_pixels,
                      SourceIterator<1>& src_it,
                      DestinationIterator<DstBytePerPixel, IsReversed>& dst_it,
                      const Palette& pallete) {
    const unsigned char two_colors = *src_it++;
    if (!dst_it.isPixelInsideLine(count_of_pixels))
      return false;

    for (unsigned int i = 0; i < count_of_pixels; ++i) {
      if (!DoLoad(dst_it, pallete, (i & 1) ? (two_colors & 0xF) : (two_colors >> 4)))
        return false;
    }

    return true;
  }
};

template<int DstBytePerPixel, bool IsReversed>
struct RLE4 : public RLEBase<Rle4Loader, DstBytePerPixel, IsReversed> {
};

typedef bool (*LoadFunction)(img::Image& decoded, const LoadImageData& data);
const unsigned int MaxDestinationDecodeMode = img::DecodeAsIs;
const unsigned int MaxSourceBytePerPixel = 5;
const unsigned int BoolValuesCount = 2;

struct AlgorithmTable {
  AlgorithmTable() {
    for (int i = 0; i < MaxDestinationDecodeMode; ++i)
      for (int j = 0; j < BoolValuesCount; ++j)
        table[i][j] = 0;
  }
  LoadFunction table[MaxDestinationDecodeMode][BoolValuesCount];
};

template<template<int DstBytePerPixel, bool IsReversed> class Algorithm>
AlgorithmTable FillAlgorithmTable() {
  AlgorithmTable result;

  result.table[img::DecodeIntoGray][0] = &Algorithm<1, false>::load;
  result.table[img::DecodeIntoGray][1] = &Algorithm<1, true>::load;
  result.table[img::DecodeIntoRgb][0] = &Algorithm<3, false>::load;
  result.table[img::DecodeIntoRgb][1] = &Algorithm<3, true>::load;
  result.table[img::DecodeIntoRgba][0] = &Algorithm<4, false>::load;
  result.table[img::DecodeIntoRgba][1] = &Algorithm<4, true>::load;

  return result;
}

struct AllAlgorythms {
  AllAlgorythms() {
    algorythms[DAMono] = FillAlgorithmTable<Mono>();
    algorythms[DA16Colors] = FillAlgorithmTable<Color16>();
    algorythms[DA16ColorsRLE4] = FillAlgorithmTable<RLE4>();
    algorythms[DA256Colors] = FillAlgorithmTable<Color256>();
    algorythms[DA256ColorsRLE8] = FillAlgorithmTable<RLE8>();
    algorythms[DA16BitsNoAlpha] = FillAlgorithmTable<Color16XXXNoAlpha>();
    algorythms[DA16BitsWithAlpha] = FillAlgorithmTable<Color16XXXAlpha>();
    algorythms[DA24Bits] = FillAlgorithmTable<ColorFull24>();
    algorythms[DA32Bits] = FillAlgorithmTable<ColorFull32>();
    algorythms[DA32BitsSkipFirst] = FillAlgorithmTable<ColorFull32SkipFirst>();
  }

  AlgorithmTable algorythms[DAUnknown];
};

LoadFunction GetLoadFunctionFor(const LoadImageData& data) {
  static const AllAlgorythms all_algorythms;
  if (data.decrypt_algorythm >= DAMono && data.decrypt_algorythm < DAUnknown)
    return all_algorythms.algorythms[data.decrypt_algorythm]
        .table[data.decode_mode_][data.is_reversed ? 1 : 0];

  return NULL;
}

} // namespace

namespace img {
class BmpDecoder : public IDecoder {
public:
  virtual std::vector<std::string> getExts() const {
    std::vector<std::string> exts;

    exts.push_back("bmp");
    exts.push_back("dib");

    return exts;
  }

  LoadImageData FillDataFromHeader(const HeaderParser& header) {
    const bool is_reversed = header.is_reversed();
    const utils::Size image_size = header.image_size();
    const Palette palette = header.palette();

    const unsigned char* data_begin = header.data_begin();
    const unsigned int data_size = header.data_size();
    const unsigned int image_size_in_byte = header.image_size_in_byte();
    LoadImageData data(image_size, data_begin, image_size_in_byte, palette, is_reversed, getDecodeMode(), getAlignment());
    if (data_size != image_size_in_byte)
      return data;

    img::DecodeMode decode_mode_best = img::DecodeIntoRgb;
    switch (header.bit_count()) {
    case 1:
      data.decrypt_algorythm = DAMono;
      break;
    case 4:
      if (header.compression() == NoCompression) {
        data.decrypt_algorythm = DA16Colors;
      } else {
        data.decrypt_algorythm = DA16ColorsRLE4;
      }

      break;
    case 8:
      if (header.compression() == NoCompression) {
        data.decrypt_algorythm = DA256Colors;
      } else {
        data.decrypt_algorythm = DA256ColorsRLE8;
      }

      break;
    case 16: {
      if (header.compression() == NoCompression) {
        data.mask_blue = 0x1F;
        data.mask_green = data.mask_blue << 5;
        data.mask_red = data.mask_blue << 10;
      } else {
        if (header.header_version() >= HeaderParser::V3) {
            data.mask_red = header.mask_red();
            data.mask_green = header.mask_green();
            data.mask_blue = header.mask_blue();
            data.mask_alpha = header.mask_alpha();
        } else {
          if (data.pallete.size < 3)
            return data;

          data.mask_red = *(reinterpret_cast<const unsigned short*>(data.pallete.getColor(0)));
          data.mask_green = *(reinterpret_cast<const unsigned short*>(data.pallete.getColor(1)));
          data.mask_blue = *(reinterpret_cast<const unsigned short*>(data.pallete.getColor(2)));
          data.mask_alpha = static_cast<short>(~0) ^ static_cast<short>(data.mask_red | data.mask_green | data.mask_blue);
        }
      }

      if (0 != data.mask_alpha) {
        decode_mode_best = img::DecodeIntoRgba;
        data.decrypt_algorythm = DA16BitsWithAlpha;
      } else {
        data.decrypt_algorythm = DA16BitsNoAlpha;
      }

      break;
    }

    case 24:
      data.decrypt_algorythm = DA24Bits;
      break;

    case 32:
      if (header.compression() == BitFields) {
        data.decrypt_algorythm = DA32BitsSkipFirst;
      } else {
        data.decrypt_algorythm = DA32Bits;
      }
      decode_mode_best = img::DecodeIntoRgba;
      break;

    default:
      return data;
    }

    if (img::DecodeAsIs == data.decode_mode_)
      data.decode_mode_ = decode_mode_best;

    return data;
  }

  virtual bool decode(const tools::ByteArray& encoded, img::Image& decoded) {
    const HeaderParser header(encoded);
    if (!header.is_valid(getAlignment()))
      return false;

    const LoadImageData data = FillDataFromHeader(header);
    if (DAUnknown == data.decrypt_algorythm)
      return false;

    LoadFunction load_function = GetLoadFunctionFor(data);

    try {
      return load_function && load_function(decoded, data);
    } catch (DestinationIsFull&) {
      // Destionation is full, it's no such critical error
      return true/*false*/;
    } catch (std::exception&) {
      return false;
    }
  }
};

AUTO_REGISTER_DECODER(BmpDecoder);
}
