#include <fstream>

#include <boost/test/unit_test.hpp>

#include "byteArray.h"
#include "image.h"
#include "mirror.h"
#include "testBenchmark.h"
#include "common/decoders/imgDecoderFactory.h"

#ifdef WIN32
#include "win32/scoped_handle.h"
#include <Windows.h>
#endif
#include <iomanip>

namespace {
class BmpFixture {
protected:
  BmpFixture() {
    img::DecoderFactory::getInstance().setAlignment(4);
  }

  std::string printColor(const color::Rgba& color1) {
    std::stringstream stream;
    stream << "#"
      << std::setw(2) << std::setfill('0') << std::hex << static_cast<int>(color1.alpha())
      << std::setw(2) << std::setfill('0') << std::hex << static_cast<int>(color1.red())
      << std::setw(2) << std::setfill('0') << std::hex << static_cast<int>(color1.green())
      << std::setw(2) << std::setfill('0') << std::hex << static_cast<int>(color1.blue());
    return stream.str();
  }

  void compareBitmaps(const img::Image& original_image, const img::Image& loaded_image) {
    BOOST_REQUIRE(!original_image.empty());
    BOOST_REQUIRE(!loaded_image.empty());

    BOOST_REQUIRE_EQUAL(original_image.alignment(), loaded_image.alignment());

    for (img::Image::SizeType h = 0; h < loaded_image.height(); ++h) {
      for (img::Image::SizeType w = 0; w < loaded_image.width(); ++w) {
        color::Rgba color1 = original_image.getPixel(w, h);
        color::Rgba color2 = loaded_image.getPixel(w, h);
        if (!(color1 == color2)) {
          BOOST_CHECK_MESSAGE(color1 == color2,
            printColor(color1) << "!=" << printColor(color2) << " (" << w << "," << h << ")");
        }
      }
    }
  }

  void compareBitmaps(const tools::ByteArray& original_array, const tools::ByteArray& byte_array) {
    const img::Image original_image = img::Image::loadFrom("bmp", original_array);
    const img::Image loaded_image = img::Image::loadFrom("bmp", byte_array);
    compareBitmaps(original_image, loaded_image);
  }

  void compareBitmaps(const std::string& original_file_path, const std::string& compare_with_path) {
      std::ifstream original_file("test_data/bmp/" + original_file_path, std::ios::binary);
      BOOST_REQUIRE(original_file);
      tools::ByteArray original_array;
      original_file >> original_array;

      std::ifstream compare_file("test_data/bmp/" + compare_with_path, std::ios::binary);
      BOOST_REQUIRE(compare_file);
      tools::ByteArray compare_array;
      compare_file >> compare_array;
      compareBitmaps(original_array, compare_array);
  }
};

class BmpFixtureCorrupt {
public:
  void checkNotLoaded(const std::string& file_path) {
      std::ifstream file("test_data/bmp/" + file_path, std::ios::binary);
      BOOST_REQUIRE(file);
      tools::ByteArray array;
      file >> array;
      checkNotLoaded(array);
  }
  void checkNotLoaded(const tools::ByteArray& byte_array) {
    const img::Image loaded_image = img::Image::loadFrom("bmp", byte_array);
    BOOST_CHECK(loaded_image.empty());
  }
};
} // namespace

namespace test {
// --log_level=test_suite --run_test=TestBmpValid
BOOST_FIXTURE_TEST_SUITE(TestBmpValid, BmpFixture)

BOOST_AUTO_TEST_CASE(Bmp1bpp_1x1) {
  compareBitmaps("valid/1bpp-1x1.bmp", "valid/1bpp-1x1-tmpl.bmp");
}

BOOST_AUTO_TEST_CASE(Bmp1bpp_320x240) {
  compareBitmaps("valid/1bpp-320x240-tmpl.bmp", "valid/1bpp-320x240.bmp");
}

BOOST_AUTO_TEST_CASE(Bmp1bpp_320x240_Color) {
  compareBitmaps("valid/1bpp-320x240-color-tmpl.bmp", "valid/1bpp-320x240-color.bmp");
}

BOOST_AUTO_TEST_CASE(Bmp1bpp_320x240_Overlappingcolor) {
  compareBitmaps("valid/1bpp-320x240-overlappingcolor-tmpl.bmp", "valid/1bpp-320x240-overlappingcolor.bmp");
}

BOOST_AUTO_TEST_CASE(Bmp1bpp_321x240) {
  compareBitmaps("valid/1bpp-321x240-tmpl.bmp", "valid/1bpp-321x240.bmp");
}

BOOST_AUTO_TEST_CASE(Bmp1bpp_322x240) {
  compareBitmaps("valid/1bpp-322x240-tmpl.bmp", "valid/1bpp-322x240.bmp");
}

BOOST_AUTO_TEST_CASE(Bmp1bpp_323x240) {
  compareBitmaps("valid/1bpp-323x240-tmpl.bmp", "valid/1bpp-323x240.bmp");
}

BOOST_AUTO_TEST_CASE(Bmp1bpp_324x240) {
  compareBitmaps("valid/1bpp-324x240-tmpl.bmp", "valid/1bpp-324x240.bmp");
}

BOOST_AUTO_TEST_CASE(Bmp1bpp_325x240) {
  compareBitmaps("valid/1bpp-325x240-tmpl.bmp", "valid/1bpp-325x240.bmp");
}

BOOST_AUTO_TEST_CASE(Bmp1bpp_326x240) {
  compareBitmaps("valid/1bpp-326x240-tmpl.bmp", "valid/1bpp-326x240.bmp");
}

BOOST_AUTO_TEST_CASE(Bmp1bpp_327x240) {
  compareBitmaps("valid/1bpp-327x240-tmpl.bmp", "valid/1bpp-327x240.bmp");
}

BOOST_AUTO_TEST_CASE(Bmp1bpp_328x240) {
  compareBitmaps("valid/1bpp-328x240-tmpl.bmp", "valid/1bpp-328x240.bmp");
}

BOOST_AUTO_TEST_CASE(Bmp1bpp_329x240) {
  compareBitmaps("valid/1bpp-329x240-tmpl.bmp", "valid/1bpp-329x240.bmp");
}

BOOST_AUTO_TEST_CASE(Bmp1bpp_330x240) {
  compareBitmaps("valid/1bpp-330x240-tmpl.bmp", "valid/1bpp-330x240.bmp");
}

BOOST_AUTO_TEST_CASE(Bmp1bpp_331x240) {
  compareBitmaps("valid/1bpp-331x240-tmpl.bmp", "valid/1bpp-331x240.bmp");
}

BOOST_AUTO_TEST_CASE(Bmp1bpp_332x240) {
  compareBitmaps("valid/1bpp-332x240-tmpl.bmp", "valid/1bpp-332x240.bmp");
}

BOOST_AUTO_TEST_CASE(Bmp1bpp_333x240) {
  compareBitmaps("valid/1bpp-333x240-tmpl.bmp", "valid/1bpp-333x240.bmp");
}

BOOST_AUTO_TEST_CASE(Bmp1bpp_334x240) {
  compareBitmaps("valid/1bpp-334x240-tmpl.bmp", "valid/1bpp-334x240.bmp");
}

BOOST_AUTO_TEST_CASE(Bmp1bpp_335x240) {
  compareBitmaps("valid/1bpp-335x240-tmpl.bmp", "valid/1bpp-335x240.bmp");
}

BOOST_AUTO_TEST_CASE(Bmp1bpp_Topdown_320x240) {
  compareBitmaps("valid/1bpp-320x240-tmpl.bmp", "valid/1bpp-topdown-320x240.bmp");
}

// 4 bit per pixel
BOOST_AUTO_TEST_CASE(Bmp4bpp_1x1) {
  compareBitmaps("valid/4bpp-1x1-tmpl.bmp", "valid/4bpp-1x1.bmp");
}

BOOST_AUTO_TEST_CASE(Bmp4bpp_320x240) {
  compareBitmaps("valid/4bpp-320x240-tmpl.bmp", "valid/4bpp-320x240.bmp");
}

BOOST_AUTO_TEST_CASE(Bmp4bpp_321x240) {
  compareBitmaps("valid/4bpp-321x240-tmpl.bmp", "valid/4bpp-321x240.bmp");
}

BOOST_AUTO_TEST_CASE(Bmp4bpp_322x240) {
  compareBitmaps("valid/4bpp-322x240-tmpl.bmp", "valid/4bpp-322x240.bmp");
}

BOOST_AUTO_TEST_CASE(Bmp4bpp_323x240) {
    compareBitmaps("valid/4bpp-323x240-tmpl.bmp", "valid/4bpp-323x240.bmp");
}

BOOST_AUTO_TEST_CASE(Bmp4bpp_324x240) {
  compareBitmaps("valid/4bpp-324x240-tmpl.bmp", "valid/4bpp-324x240.bmp");
}

BOOST_AUTO_TEST_CASE(Bmp4bpp_325x240) {
  compareBitmaps("valid/4bpp-325x240-tmpl.bmp", "valid/4bpp-325x240.bmp");
}

BOOST_AUTO_TEST_CASE(Bmp4bpp_326x240) {
  compareBitmaps("valid/4bpp-326x240-tmpl.bmp", "valid/4bpp-326x240.bmp");
}

BOOST_AUTO_TEST_CASE(Bmp4bpp_327x240) {
  compareBitmaps("valid/4bpp-327x240-tmpl.bmp", "valid/4bpp-327x240.bmp");
}

BOOST_AUTO_TEST_CASE(Bmp4bpp_Topdown_320x240) {
  compareBitmaps("valid/4bpp-320x240-tmpl.bmp", "valid/4bpp-topdown-320x240.bmp");
}

// 8 bit per pixel
BOOST_AUTO_TEST_CASE(Bmp8bpp_1x1) {
  compareBitmaps("valid/8bpp-1x1-tmpl.bmp", "valid/8bpp-1x1.bmp");
}

BOOST_AUTO_TEST_CASE(Bmp8bpp_1x64000) {
  compareBitmaps("valid/8bpp-1x64000-tmpl.bmp", "valid/8bpp-1x64000.bmp");
}

BOOST_AUTO_TEST_CASE(Bmp8bpp_320x240) {
  compareBitmaps("valid/8bpp-320x240-tmpl.bmp", "valid/8bpp-320x240.bmp");
}

BOOST_AUTO_TEST_CASE(Bmp8bpp_321x240) {
  compareBitmaps("valid/8bpp-321x240-tmpl.bmp", "valid/8bpp-321x240.bmp");
}

BOOST_AUTO_TEST_CASE(Bmp8bpp_322x240) {
  compareBitmaps("valid/8bpp-322x240-tmpl.bmp", "valid/8bpp-322x240.bmp");
}

BOOST_AUTO_TEST_CASE(Bmp8bpp_323x240) {
  compareBitmaps("valid/8bpp-323x240-tmpl.bmp", "valid/8bpp-323x240.bmp");
}

BOOST_AUTO_TEST_CASE(Bmp8bpp_Colorsimportant_320x240) {
  compareBitmaps("valid/8bpp-colorsimportant-two-tmpl.bmp", "valid/8bpp-colorsimportant-two.bmp");
}

BOOST_AUTO_TEST_CASE(Bmp8bpp_Colorused_320x240) {
  compareBitmaps("valid/8bpp-colorsused-zero-tmpl.bmp", "valid/8bpp-colorsused-zero.bmp");
}

BOOST_AUTO_TEST_CASE(Bmp8bpp_Topdown_320x240) {
  compareBitmaps("valid/8bpp-320x240-tmpl.bmp", "valid/8bpp-topdown-320x240.bmp");
}

// 16 bit per pixel
BOOST_AUTO_TEST_CASE(Bmp16bpp_444_199x203) {
  compareBitmaps("valid/444-199x203-tmpl.bmp", "valid/444-199x203.bmp");
}

BOOST_AUTO_TEST_CASE(Bmp16bpp_444_199x203_Topdown) {
  compareBitmaps("valid/444-199x203-tmpl.bmp", "valid/444-199x203-topdown.bmp");
}

BOOST_AUTO_TEST_CASE(Bmp16bpp_555_1x1) {
  compareBitmaps("valid/555-1x1-tmpl.bmp", "valid/555-1x1.bmp");
}

BOOST_AUTO_TEST_CASE(Bmp16bpp_555_320x240) {
  compareBitmaps("valid/555-320x240-tmpl.bmp", "valid/555-320x240.bmp");
}

BOOST_AUTO_TEST_CASE(Bmp16bpp_555_321x240) {
  compareBitmaps("valid/555-321x240-tmpl.bmp", "valid/555-321x240.bmp");
}

BOOST_AUTO_TEST_CASE(Bmp16bpp_565_1x1) {
  compareBitmaps("valid/565-1x1-tmpl.bmp", "valid/565-1x1.bmp");
}

BOOST_AUTO_TEST_CASE(Bmp16bpp_565_320x240) {
  compareBitmaps("valid/565-320x240-tmpl.bmp", "valid/565-320x240.bmp");
}

BOOST_AUTO_TEST_CASE(Bmp16bpp_565_320x240_Topdown) {
  compareBitmaps("valid/565-320x240-tmpl.bmp", "valid/565-320x240-topdown.bmp");
}

BOOST_AUTO_TEST_CASE(Bmp16bpp_565_321x240) {
  compareBitmaps("valid/565-321x240-tmpl.bmp", "valid/565-321x240.bmp");
}

BOOST_AUTO_TEST_CASE(Bmp16bpp_565_321x240_Topdown) {
  compareBitmaps("valid/565-321x240-tmpl.bmp", "valid/565-321x240-topdown.bmp");
}

BOOST_AUTO_TEST_CASE(Bmp16bpp_565_322x240) {
  compareBitmaps("valid/565-322x240-tmpl.bmp", "valid/565-322x240.bmp");
}

BOOST_AUTO_TEST_CASE(Bmp16bpp_565_322x240_Topdown) {
  compareBitmaps("valid/565-322x240-tmpl.bmp", "valid/565-322x240-topdown.bmp");
}

BOOST_AUTO_TEST_CASE(Bmp16bpp_1555_199x203) {
  compareBitmaps("valid/1555-199x203-tmpl.bmp", "valid/1555-199x203.bmp");
}

BOOST_AUTO_TEST_CASE(Bmp16bpp_1555_199x203_Topdown) {
  compareBitmaps("valid/1555-199x203-tmpl.bmp", "valid/1555-199x203-topdown.bmp");
}

BOOST_AUTO_TEST_CASE(Bmp16bpp_4444_199x203) {
  compareBitmaps("valid/4444-199x203-tmpl.bmp", "valid/4444-199x203.bmp");
}

BOOST_AUTO_TEST_CASE(Bmp16bpp_4444_199x203_Topdown) {
  compareBitmaps("valid/4444-199x203-tmpl.bmp", "valid/4444-199x203-topdown.bmp");
}

// rle4
BOOST_AUTO_TEST_CASE(BmpRle4_Absolute_320x240) {
  compareBitmaps("valid/rle4-absolute-320x240-tmpl.bmp", "valid/rle4-absolute-320x240.bmp");
}

BOOST_AUTO_TEST_CASE(BmpRle4_Alternate_320x240) {
  compareBitmaps("valid/rle4-alternate-320x240-tmpl.bmp", "valid/rle4-alternate-320x240.bmp");
}

BOOST_AUTO_TEST_CASE(BmpRle4_Delta_320x240) {
  compareBitmaps("valid/rle4-delta-320x240-tmpl.bmp", "valid/rle4-delta-320x240.bmp");
}

BOOST_AUTO_TEST_CASE(BmpRle4_Encoded_320x240) {
  compareBitmaps("valid/rle4-encoded-320x240-tmpl.bmp", "valid/rle4-encoded-320x240.bmp");
}

// rle8
/*BOOST_AUTO_TEST_CASE(BmpRle8_64000x1) {
  img::Image original_image = img::Image::loadFrom("bmp", Get_Rle8_64000x1_Tmpl_Bmp_Array());
  img::Image loaded_image = img::Image::loadFrom("bmp", Get_Rle8_64000x1_Bmp_Array());

  img::rgba2rgb(original_image, original_image);
  compareBitmaps(original_image, loaded_image);
}*/

BOOST_AUTO_TEST_CASE(BmpRle8_Absolute_320x240) {
  compareBitmaps("valid/rle8-absolute-320x240-tmpl.bmp", "valid/rle8-absolute-320x240.bmp");
}

BOOST_AUTO_TEST_CASE(BmpRle8_Blank_160x120) {
  compareBitmaps("valid/rle8-blank-160x120-tmpl.bmp", "valid/rle8-blank-160x120.bmp");
}

BOOST_AUTO_TEST_CASE(BmpRle8_Delta_320x240) {
  compareBitmaps("valid/rle8-delta-320x240-tmpl.bmp", "valid/rle8-delta-320x240.bmp");
}

BOOST_AUTO_TEST_CASE(BmpRle8_Encoded_320x240) {
  compareBitmaps("valid/rle8-encoded-320x240-tmpl.bmp", "valid/rle8-encoded-320x240.bmp");
}

BOOST_AUTO_TEST_SUITE_END()
} // namespace test

namespace test {

// --log_level=test_suite --run_test=TestBmpCorrupt
BOOST_FIXTURE_TEST_SUITE(TestBmpCorrupt, BmpFixtureCorrupt)

BOOST_AUTO_TEST_CASE(Bmp1bpp_No_Palette) {
  checkNotLoaded("corrupt/1bpp-no-palette.bmp");
}

BOOST_AUTO_TEST_CASE(Bmp1bpp_Pixeldata_Cropped) {
  checkNotLoaded("corrupt/1bpp-pixeldata-cropped.bmp");
}

BOOST_AUTO_TEST_CASE(Bmp4bpp_No_Palette) {
  checkNotLoaded("corrupt/4bpp-no-palette.bmp");
}

BOOST_AUTO_TEST_CASE(Bmp4bpp_Pixeldata_Cropped) {
  checkNotLoaded("corrupt/4bpp-pixeldata-cropped.bmp");
}

BOOST_AUTO_TEST_CASE(Bmp8bpp_Colorsimportant_Large) {
  checkNotLoaded("corrupt/8bpp-colorsimportant-large.bmp");
}

BOOST_AUTO_TEST_CASE(Bmp8bpp_Colorsimportant_Negative) {
  checkNotLoaded("corrupt/8bpp-colorsimportant-negative.bmp");
}

BOOST_AUTO_TEST_CASE(Bmp8bpp_Colorsused_Large) {
  checkNotLoaded("corrupt/8bpp-colorsused-large.bmp");
}

BOOST_AUTO_TEST_CASE(Bmp8bpp_Colorsused_Negative) {
  checkNotLoaded("corrupt/8bpp-colorsused-negative.bmp");
}

BOOST_AUTO_TEST_CASE(Bmp8bpp_No_Palette) {
  checkNotLoaded("corrupt/8bpp-no-palette.bmp");
}

BOOST_AUTO_TEST_CASE(Bmp8bpp_Pixeldata_Cropped) {
  checkNotLoaded("corrupt/8bpp-pixeldata-cropped.bmp");
}

BOOST_AUTO_TEST_CASE(Bmp24bpp_Pixeldata_Cropped) {
  checkNotLoaded("corrupt/24bpp-pixeldata-cropped.bmp");
}

BOOST_AUTO_TEST_CASE(Bmp32bpp_Pixeldata_Cropped) {
  checkNotLoaded("corrupt/32bpp-pixeldata-cropped.bmp");
}

BOOST_AUTO_TEST_CASE(Bmp555_Pixeldata_Cropped) {
  checkNotLoaded("corrupt/555-pixeldata-cropped.bmp");
}

BOOST_AUTO_TEST_CASE(BmpBitdepth_Large) {
  checkNotLoaded("corrupt/bitdepth-large.bmp");
}

BOOST_AUTO_TEST_CASE(BmpBitdepth_Odd) {
  checkNotLoaded("corrupt/bitdepth-odd.bmp");
}

BOOST_AUTO_TEST_CASE(BmpBitdepth_Zero) {
  checkNotLoaded("corrupt/bitdepth-zero.bmp");
}

BOOST_AUTO_TEST_CASE(BmpColormasks_Cropped) {
  checkNotLoaded("corrupt/colormasks-cropped.bmp");
}

BOOST_AUTO_TEST_CASE(BmpColormasks_Missing) {
  checkNotLoaded("corrupt/colormasks-missing.bmp");
}

BOOST_AUTO_TEST_CASE(BmpCompression_Bad_Rle4_For_8bpp) {
  checkNotLoaded("corrupt/compression-bad-rle4-for-8bpp.bmp");
}

BOOST_AUTO_TEST_CASE(BmpCompression_Bad_Rle8_For_4bpp) {
  checkNotLoaded("corrupt/compression-bad-rle8-for-4bpp.bmp");
}

BOOST_AUTO_TEST_CASE(BmpCompression_Unknown) {
  checkNotLoaded("corrupt/compression-unknown.bmp");
}

BOOST_AUTO_TEST_CASE(BmpFileinfoheader_Cropped) {
  checkNotLoaded("corrupt/fileinfoheader-cropped.bmp");
}

BOOST_AUTO_TEST_CASE(BmpHeight_Zero) {
  checkNotLoaded("corrupt/height-zero.bmp");
}

BOOST_AUTO_TEST_CASE(BmpInfoheader_Cropped) {
  checkNotLoaded("corrupt/infoheader-cropped.bmp");
}

BOOST_AUTO_TEST_CASE(BmpInfoheader_Missing) {
  checkNotLoaded("corrupt/infoheader-missing.bmp");
}

BOOST_AUTO_TEST_CASE(BmpInfoheadersize_Large) {
  checkNotLoaded("corrupt/infoheadersize-large.bmp");
}

BOOST_AUTO_TEST_CASE(BmpInfoheadersize_Small) {
  checkNotLoaded("corrupt/infoheadersize-small.bmp");
}

BOOST_AUTO_TEST_CASE(BmpInfoheadersize_Zero) {
  checkNotLoaded("corrupt/infoheadersize-zero.bmp");
}

BOOST_AUTO_TEST_CASE(BmpMagicnumber_Bad) {
  checkNotLoaded("corrupt/magicnumber-bad.bmp");
}

BOOST_AUTO_TEST_CASE(BmpMagicnumber_Cropped) {
  checkNotLoaded("corrupt/magicnumber-cropped.bmp");
}

BOOST_AUTO_TEST_CASE(BmpOffbits_Large) {
  checkNotLoaded("corrupt/offbits-large.bmp");
}

BOOST_AUTO_TEST_CASE(BmpOffbits_Negative) {
  checkNotLoaded("corrupt/offbits-negative.bmp");
}

BOOST_AUTO_TEST_CASE(BmpOffbits_Zero) {
  checkNotLoaded("corrupt/offbits-zero.bmp");
}

BOOST_AUTO_TEST_CASE(BmpPalette_Cropped) {
  checkNotLoaded("corrupt/palette-cropped.bmp");
}

BOOST_AUTO_TEST_CASE(BmpPalette_Missing) {
  checkNotLoaded("corrupt/palette-missing.bmp");
}

BOOST_AUTO_TEST_CASE(BmpPalette_Too_Big) {
  checkNotLoaded("corrupt/palette-too-big.bmp");
}

BOOST_AUTO_TEST_CASE(BmpPixeldata_Missing) {
  checkNotLoaded("corrupt/pixeldata-missing.bmp");
}

BOOST_AUTO_TEST_CASE(BmpRle4_Absolute_Cropped) {
  checkNotLoaded("corrupt/rle4-absolute-cropped.bmp");
}

BOOST_AUTO_TEST_CASE(BmpRle4_Delta_Cropped) {
  checkNotLoaded("corrupt/rle4-delta-cropped.bmp");
}

BOOST_AUTO_TEST_CASE(BmpRle4_No_End_Of_Line_Marker) {
  checkNotLoaded("corrupt/rle4-no-end-of-line-marker.bmp");
}

BOOST_AUTO_TEST_CASE(BmpRle4_Runlength_Cropped) {
  checkNotLoaded("corrupt/rle4-runlength-cropped.bmp");
}

BOOST_AUTO_TEST_CASE(BmpRle8_Absolute_Cropped) {
  checkNotLoaded("corrupt/rle8-absolute-cropped.bmp");
}

BOOST_AUTO_TEST_CASE(BmpRle8_Delta_Cropped) {
  checkNotLoaded("corrupt/rle8-delta-cropped.bmp");
}

BOOST_AUTO_TEST_CASE(BmpRle8_Deltaleavesimage) {
  checkNotLoaded("corrupt/rle8-deltaleavesimage.bmp");
}

BOOST_AUTO_TEST_CASE(BmpRle8_No_End_Of_Line_Marker) {
  checkNotLoaded("corrupt/rle8-no-end-of-line-marker.bmp");
}

BOOST_AUTO_TEST_CASE(BmpRle8_Runlength_Cropped) {
  checkNotLoaded("corrupt/rle8-runlength-cropped.bmp");
}

BOOST_AUTO_TEST_CASE(BmpWidth_Negative) {
  checkNotLoaded("corrupt/width-negative.bmp");
}

BOOST_AUTO_TEST_CASE(BmpWidth_Times_Height_Overflow) {
  checkNotLoaded("corrupt/width-times-height-overflow.bmp");
}

BOOST_AUTO_TEST_CASE(BmpWidth_Zero) {
  checkNotLoaded("corrupt/width-zero.bmp");
}

BOOST_AUTO_TEST_SUITE_END()
} // namespace test

namespace test {
// --log_level=test_suite --run_test=TestBmpQuestionable
BOOST_FIXTURE_TEST_SUITE(TestBmpQuestionable, BmpFixture)

BOOST_FIXTURE_TEST_CASE(Bmp8bpp_pixels_not_in_palette, BmpFixtureCorrupt) {
  checkNotLoaded("questionable/8bpp-pixels-not-in-palette.bmp");
}

BOOST_FIXTURE_TEST_CASE(Bmp32bpp_0x0, BmpFixtureCorrupt) {
  checkNotLoaded("questionable/32bpp-0x0.bmp");
}

BOOST_FIXTURE_TEST_CASE(Bmp32bpp_0x240, BmpFixtureCorrupt) {
  checkNotLoaded("questionable/32bpp-0x240.bmp");
}

BOOST_FIXTURE_TEST_CASE(Bmp32bpp_320x0, BmpFixtureCorrupt) {
  checkNotLoaded("questionable/32bpp-320x0.bmp");
}

BOOST_AUTO_TEST_CASE(Bmp_Filesize_Bad) {
  compareBitmaps("valid/1bpp-320x240.bmp", "questionable/filesize-bad.bmp");
}

BOOST_AUTO_TEST_CASE(Bmp_Filesize_Zero) {
  compareBitmaps("valid/1bpp-320x240.bmp", "questionable/filesize-zero.bmp");
}

BOOST_AUTO_TEST_CASE(BmpPelsPerMeterXLarge) {
  compareBitmaps("valid/8bpp-320x240-tmpl.bmp", "questionable/pels-per-meter-x-large.bmp");
}

BOOST_AUTO_TEST_CASE(BmpPelsPerMeterXNegative) {
  compareBitmaps("valid/8bpp-320x240-tmpl.bmp", "questionable/pels-per-meter-x-negative.bmp");
}

BOOST_AUTO_TEST_CASE(BmpPelsPerMeterXZero) {
  compareBitmaps("valid/8bpp-320x240-tmpl.bmp", "questionable/pels-per-meter-x-zero.bmp");
}

BOOST_AUTO_TEST_CASE(BmpPelsPerMeterYLarge) {
  compareBitmaps("valid/8bpp-320x240-tmpl.bmp", "questionable/pels-per-meter-y-large.bmp");
}

BOOST_AUTO_TEST_CASE(BmpPelsPerMeterYNegative) {
  compareBitmaps("valid/8bpp-320x240-tmpl.bmp", "questionable/pels-per-meter-y-negative.bmp");
}

BOOST_AUTO_TEST_CASE(BmpPelsPerMeterYZero) {
  compareBitmaps("valid/8bpp-320x240-tmpl.bmp", "questionable/pels-per-meter-y-zero.bmp");
}

BOOST_AUTO_TEST_CASE(BmpPixeldataRle8Toomuch) {
  compareBitmaps("valid/8bpp-320x240-tmpl.bmp", "questionable/pixeldata-rle8-toomuch.bmp");
}

BOOST_AUTO_TEST_CASE(BmpPixeldataToomuch) {
  compareBitmaps("valid/1bpp-320x240-tmpl.bmp", "questionable/pixeldata-toomuch.bmp");
}

BOOST_AUTO_TEST_CASE(BmpPlanesLarge) {
  compareBitmaps("valid/1bpp-320x240-tmpl.bmp", "questionable/planes-large.bmp");
}

BOOST_AUTO_TEST_CASE(BmpPlanesZero) {
  compareBitmaps("valid/1bpp-320x240-tmpl.bmp", "questionable/planes-zero.bmp");
}

BOOST_AUTO_TEST_CASE(BmpReversed1Bad) {
  compareBitmaps("valid/1bpp-320x240-tmpl.bmp", "questionable/reserved1-bad.bmp");
}

BOOST_AUTO_TEST_CASE(BmpReversed2Bad) {
  compareBitmaps("valid/1bpp-320x240-tmpl.bmp", "questionable/reserved2-bad.bmp");
}

BOOST_AUTO_TEST_CASE(BmpRle4HeightNegative) {
  compareBitmaps("valid/8bpp-320x240-tmpl.bmp", "questionable/rle4-height-negative.bmp");
}

BOOST_AUTO_TEST_CASE(BmpRle4NoEndOfBitmap) {
  compareBitmaps("valid/8bpp-320x240-tmpl.bmp", "questionable/rle4-no-end-of-bitmap-marker.bmp");
}

BOOST_AUTO_TEST_CASE(BmpRle8HeightNegative) {
  compareBitmaps("valid/8bpp-320x240-tmpl.bmp", "questionable/rle8-height-negative.bmp");
}

BOOST_AUTO_TEST_CASE(BmpRle8NoEndOfBitmap) {
  compareBitmaps("valid/8bpp-320x240-tmpl.bmp", "questionable/rle8-no-end-of-bitmap-marker.bmp");
}

BOOST_AUTO_TEST_SUITE_END()
} // namespace test
