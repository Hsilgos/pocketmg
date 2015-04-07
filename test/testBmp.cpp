#include <boost/test/unit_test.hpp>

#include "byteArray.h"
#include "image.h"
#include "mirror.h"
#include "testBenchmark.h"
#include "imgDecoderFactory.h"
#include "win32/scoped_handle.h"

#include <Windows.h>
#include <iomanip>

// 1 bit per pixel
#include "Generated/bmp/valid/1bpp_1x1_bmp.h"
#include "Generated/bmp/valid/1bpp_1x1_tmpl_bmp.h"
#include "Generated/bmp/valid/1bpp_320x240_bmp.h"
#include "Generated/bmp/valid/1bpp_320x240_tmpl_bmp.h"
#include "Generated/bmp/valid/1bpp_320x240_color_bmp.h"
#include "Generated/bmp/valid/1bpp_320x240_color_tmpl_bmp.h"
#include "Generated/bmp/valid/1bpp_320x240_overlappingcolor_bmp.h"
#include "Generated/bmp/valid/1bpp_320x240_overlappingcolor_tmpl_bmp.h"
#include "Generated/bmp/valid/1bpp_321x240_bmp.h"
#include "Generated/bmp/valid/1bpp_321x240_tmpl_bmp.h"
#include "Generated/bmp/valid/1bpp_322x240_bmp.h"
#include "Generated/bmp/valid/1bpp_322x240_tmpl_bmp.h"
#include "Generated/bmp/valid/1bpp_323x240_bmp.h"
#include "Generated/bmp/valid/1bpp_323x240_tmpl_bmp.h"
#include "Generated/bmp/valid/1bpp_324x240_bmp.h"
#include "Generated/bmp/valid/1bpp_324x240_tmpl_bmp.h"
#include "Generated/bmp/valid/1bpp_325x240_bmp.h"
#include "Generated/bmp/valid/1bpp_325x240_tmpl_bmp.h"
#include "Generated/bmp/valid/1bpp_326x240_bmp.h"
#include "Generated/bmp/valid/1bpp_326x240_tmpl_bmp.h"
#include "Generated/bmp/valid/1bpp_327x240_bmp.h"
#include "Generated/bmp/valid/1bpp_327x240_tmpl_bmp.h"
#include "Generated/bmp/valid/1bpp_328x240_bmp.h"
#include "Generated/bmp/valid/1bpp_328x240_tmpl_bmp.h"
#include "Generated/bmp/valid/1bpp_329x240_bmp.h"
#include "Generated/bmp/valid/1bpp_329x240_tmpl_bmp.h"
#include "Generated/bmp/valid/1bpp_330x240_bmp.h"
#include "Generated/bmp/valid/1bpp_330x240_tmpl_bmp.h"
#include "Generated/bmp/valid/1bpp_331x240_bmp.h"
#include "Generated/bmp/valid/1bpp_331x240_tmpl_bmp.h"
#include "Generated/bmp/valid/1bpp_332x240_bmp.h"
#include "Generated/bmp/valid/1bpp_332x240_tmpl_bmp.h"
#include "Generated/bmp/valid/1bpp_333x240_bmp.h"
#include "Generated/bmp/valid/1bpp_333x240_tmpl_bmp.h"
#include "Generated/bmp/valid/1bpp_334x240_bmp.h"
#include "Generated/bmp/valid/1bpp_334x240_tmpl_bmp.h"
#include "Generated/bmp/valid/1bpp_335x240_bmp.h"
#include "Generated/bmp/valid/1bpp_335x240_tmpl_bmp.h"
#include "Generated/bmp/valid/1bpp_topdown_320x240_bmp.h"

// 4 bit per pixel
#include "Generated/bmp/valid/4bpp_1x1_bmp.h"
#include "Generated/bmp/valid/4bpp_1x1_tmpl_bmp.h"
#include "Generated/bmp/valid/4bpp_320x240_bmp.h"
#include "Generated/bmp/valid/4bpp_320x240_tmpl_bmp.h"
#include "Generated/bmp/valid/4bpp_321x240_bmp.h"
#include "Generated/bmp/valid/4bpp_321x240_tmpl_bmp.h"
#include "Generated/bmp/valid/4bpp_322x240_bmp.h"
#include "Generated/bmp/valid/4bpp_322x240_tmpl_bmp.h"
#include "Generated/bmp/valid/4bpp_323x240_bmp.h"
#include "Generated/bmp/valid/4bpp_323x240_tmpl_bmp.h"
#include "Generated/bmp/valid/4bpp_324x240_bmp.h"
#include "Generated/bmp/valid/4bpp_324x240_tmpl_bmp.h"
#include "Generated/bmp/valid/4bpp_325x240_bmp.h"
#include "Generated/bmp/valid/4bpp_325x240_tmpl_bmp.h"
#include "Generated/bmp/valid/4bpp_326x240_bmp.h"
#include "Generated/bmp/valid/4bpp_326x240_tmpl_bmp.h"
#include "Generated/bmp/valid/4bpp_327x240_bmp.h"
#include "Generated/bmp/valid/4bpp_327x240_tmpl_bmp.h"
#include "Generated/bmp/valid/4bpp_topdown_320x240_bmp.h"

// 8 bit per pixel
#include "Generated/bmp/valid/8bpp_1x1_bmp.h"
#include "Generated/bmp/valid/8bpp_1x1_tmpl_bmp.h"
#include "Generated/bmp/valid/8bpp_1x64000_bmp.h"
#include "Generated/bmp/valid/8bpp_1x64000_tmpl_bmp.h"
#include "Generated/bmp/valid/8bpp_320x240_bmp.h"
#include "Generated/bmp/valid/8bpp_320x240_tmpl_bmp.h"
#include "Generated/bmp/valid/8bpp_321x240_bmp.h"
#include "Generated/bmp/valid/8bpp_321x240_tmpl_bmp.h"
#include "Generated/bmp/valid/8bpp_322x240_bmp.h"
#include "Generated/bmp/valid/8bpp_322x240_tmpl_bmp.h"
#include "Generated/bmp/valid/8bpp_323x240_bmp.h"
#include "Generated/bmp/valid/8bpp_323x240_tmpl_bmp.h"
#include "Generated/bmp/valid/8bpp_colorsimportant_two_bmp.h"
#include "Generated/bmp/valid/8bpp_colorsimportant_two_tmpl_bmp.h"
#include "Generated/bmp/valid/8bpp_colorsused_zero_bmp.h"
#include "Generated/bmp/valid/8bpp_colorsused_zero_tmpl_bmp.h"
#include "Generated/bmp/valid/8bpp_topdown_320x240_bmp.h"

// 16 bit per pixel
#include "Generated/bmp/valid/444_199x203_bmp.h"
#include "Generated/bmp/valid/444_199x203_tmpl_bmp.h"
#include "Generated/bmp/valid/444_199x203_topdown_bmp.h"
#include "Generated/bmp/valid/555_1x1_bmp.h"
#include "Generated/bmp/valid/555_1x1_tmpl_bmp.h"
#include "Generated/bmp/valid/555_320x240_bmp.h"
#include "Generated/bmp/valid/555_320x240_tmpl_bmp.h"
#include "Generated/bmp/valid/555_321x240_bmp.h"
#include "Generated/bmp/valid/555_321x240_tmpl_bmp.h"
#include "Generated/bmp/valid/565_1x1_bmp.h"
#include "Generated/bmp/valid/565_1x1_tmpl_bmp.h"
#include "Generated/bmp/valid/565_320x240_bmp.h"
#include "Generated/bmp/valid/565_320x240_tmpl_bmp.h"
#include "Generated/bmp/valid/565_320x240_topdown_bmp.h"
#include "Generated/bmp/valid/565_320x240_topdown_tmpl_bmp.h"
#include "Generated/bmp/valid/565_321x240_bmp.h"
#include "Generated/bmp/valid/565_321x240_tmpl_bmp.h"
#include "Generated/bmp/valid/565_321x240_topdown_bmp.h"
#include "Generated/bmp/valid/565_321x240_topdown_tmpl_bmp.h"
#include "Generated/bmp/valid/565_322x240_bmp.h"
#include "Generated/bmp/valid/565_322x240_tmpl_bmp.h"
#include "Generated/bmp/valid/565_322x240_topdown_bmp.h"
#include "Generated/bmp/valid/565_322x240_topdown_tmpl_bmp.h"
#include "Generated/bmp/valid/1555_199x203_bmp.h"
#include "Generated/bmp/valid/1555_199x203_tmpl_bmp.h"
#include "Generated/bmp/valid/1555_199x203_topdown_bmp.h"
#include "Generated/bmp/valid/4444_199x203_bmp.h"
#include "Generated/bmp/valid/4444_199x203_tmpl_bmp.h"
#include "Generated/bmp/valid/4444_199x203_topdown_bmp.h"

// rle4
#include "Generated/bmp/valid/rle4_absolute_320x240_bmp.h"
#include "Generated/bmp/valid/rle4_absolute_320x240_tmpl_bmp.h"
#include "Generated/bmp/valid/rle4_alternate_320x240_bmp.h"
#include "Generated/bmp/valid/rle4_alternate_320x240_tmpl_bmp.h"
#include "Generated/bmp/valid/rle4_delta_320x240_bmp.h"
#include "Generated/bmp/valid/rle4_delta_320x240_tmpl_bmp.h"
#include "Generated/bmp/valid/rle4_encoded_320x240_bmp.h"
#include "Generated/bmp/valid/rle4_encoded_320x240_tmpl_bmp.h"

// rle8
#include "Generated/bmp/valid/rle8_64000x1_bmp.h"
#include "Generated/bmp/valid/rle8_64000x1_tmpl_bmp.h"
#include "Generated/bmp/valid/rle8_absolute_320x240_bmp.h"
#include "Generated/bmp/valid/rle8_absolute_320x240_tmpl_bmp.h"
#include "Generated/bmp/valid/rle8_blank_160x120_bmp.h"
#include "Generated/bmp/valid/rle8_blank_160x120_tmpl_bmp.h"
#include "Generated/bmp/valid/rle8_delta_320x240_bmp.h"
#include "Generated/bmp/valid/rle8_delta_320x240_tmpl_bmp.h"
#include "Generated/bmp/valid/rle8_encoded_320x240_bmp.h"
#include "Generated/bmp/valid/rle8_encoded_320x240_tmpl_bmp.h"

// corrupted
#include "Generated/bmp/corrupt/1bpp_no_palette_bmp.h"
#include "Generated/bmp/corrupt/1bpp_pixeldata_cropped_bmp.h"
#include "Generated/bmp/corrupt/4bpp_no_palette_bmp.h"
#include "Generated/bmp/corrupt/4bpp_pixeldata_cropped_bmp.h"
#include "Generated/bmp/corrupt/8bpp_colorsimportant_large_bmp.h"
#include "Generated/bmp/corrupt/8bpp_colorsimportant_negative_bmp.h"
#include "Generated/bmp/corrupt/8bpp_colorsused_large_bmp.h"
#include "Generated/bmp/corrupt/8bpp_colorsused_negative_bmp.h"
#include "Generated/bmp/corrupt/8bpp_no_palette_bmp.h"
#include "Generated/bmp/corrupt/8bpp_pixeldata_cropped_bmp.h"
#include "Generated/bmp/corrupt/24bpp_pixeldata_cropped_bmp.h"
#include "Generated/bmp/corrupt/32bpp_pixeldata_cropped_bmp.h"
#include "Generated/bmp/corrupt/555_pixeldata_cropped_bmp.h"
#include "Generated/bmp/corrupt/bitdepth_large_bmp.h"
#include "Generated/bmp/corrupt/bitdepth_odd_bmp.h"
#include "Generated/bmp/corrupt/bitdepth_zero_bmp.h"
#include "Generated/bmp/corrupt/colormasks_cropped_bmp.h"
#include "Generated/bmp/corrupt/colormasks_missing_bmp.h"
#include "Generated/bmp/corrupt/compression_bad_rle4_for_8bpp_bmp.h"
#include "Generated/bmp/corrupt/compression_bad_rle8_for_4bpp_bmp.h"
#include "Generated/bmp/corrupt/compression_unknown_bmp.h"
#include "Generated/bmp/corrupt/fileinfoheader_cropped_bmp.h"
#include "Generated/bmp/corrupt/height_zero_bmp.h"
#include "Generated/bmp/corrupt/infoheader_cropped_bmp.h"
#include "Generated/bmp/corrupt/infoheader_missing_bmp.h"
#include "Generated/bmp/corrupt/infoheadersize_large_bmp.h"
#include "Generated/bmp/corrupt/infoheadersize_small_bmp.h"
#include "Generated/bmp/corrupt/infoheadersize_zero_bmp.h"
#include "Generated/bmp/corrupt/magicnumber_bad_bmp.h"
#include "Generated/bmp/corrupt/magicnumber_cropped_bmp.h"
#include "Generated/bmp/corrupt/offbits_large_bmp.h"
#include "Generated/bmp/corrupt/offbits_negative_bmp.h"
#include "Generated/bmp/corrupt/offbits_zero_bmp.h"
#include "Generated/bmp/corrupt/palette_cropped_bmp.h"
#include "Generated/bmp/corrupt/palette_missing_bmp.h"
#include "Generated/bmp/corrupt/palette_too_big_bmp.h"
#include "Generated/bmp/corrupt/pixeldata_missing_bmp.h"
#include "Generated/bmp/corrupt/rle4_absolute_cropped_bmp.h"
#include "Generated/bmp/corrupt/rle4_delta_cropped_bmp.h"
#include "Generated/bmp/corrupt/rle4_no_end_of_line_marker_bmp.h"
#include "Generated/bmp/corrupt/rle4_runlength_cropped_bmp.h"
#include "Generated/bmp/corrupt/rle8_absolute_cropped_bmp.h"
#include "Generated/bmp/corrupt/rle8_delta_cropped_bmp.h"
#include "Generated/bmp/corrupt/rle8_deltaleavesimage_bmp.h"
#include "Generated/bmp/corrupt/rle8_no_end_of_line_marker_bmp.h"
#include "Generated/bmp/corrupt/rle8_runlength_cropped_bmp.h"
#include "Generated/bmp/corrupt/width_negative_bmp.h"
#include "Generated/bmp/corrupt/width_times_height_overflow_bmp.h"
#include "Generated/bmp/corrupt/width_zero_bmp.h"

// Questionable
#include "Generated/bmp/questionable/8bpp_pixels_not_in_palette_bmp.h"
#include "Generated/bmp/questionable/32bpp_0x0_bmp.h"
#include "Generated/bmp/questionable/32bpp_0x240_bmp.h"
#include "Generated/bmp/questionable/32bpp_320x0_bmp.h"
#include "Generated/bmp/questionable/filesize_bad_bmp.h"
#include "Generated/bmp/questionable/filesize_zero_bmp.h"
#include "Generated/bmp/questionable/pels_per_meter_x_large_bmp.h"
#include "Generated/bmp/questionable/pels_per_meter_x_negative_bmp.h"
#include "Generated/bmp/questionable/pels_per_meter_x_zero_bmp.h"
#include "Generated/bmp/questionable/pels_per_meter_y_large_bmp.h"
#include "Generated/bmp/questionable/pels_per_meter_y_negative_bmp.h"
#include "Generated/bmp/questionable/pels_per_meter_y_zero_bmp.h"
#include "Generated/bmp/questionable/pixeldata_rle8_toomuch_bmp.h"
#include "Generated/bmp/questionable/pixeldata_toomuch_bmp.h"
#include "Generated/bmp/questionable/planes_large_bmp.h"
#include "Generated/bmp/questionable/planes_zero_bmp.h"
#include "Generated/bmp/questionable/reserved1_bad_bmp.h"
#include "Generated/bmp/questionable/reserved2_bad_bmp.h"
#include "Generated/bmp/questionable/rle4_height_negative_bmp.h"
#include "Generated/bmp/questionable/rle4_no_end_of_bitmap_marker_bmp.h"
#include "Generated/bmp/questionable/rle8_height_negative_bmp.h"
#include "Generated/bmp/questionable/rle8_no_end_of_bitmap_marker_bmp.h"

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
};

class BmpFixtureCorrupt {
public:
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
  compareBitmaps(Get_1bpp_1x1_Tmpl_Bmp_Array(),
                 Get_1bpp_1x1_Bmp_Array());
}

BOOST_AUTO_TEST_CASE(Bmp1bpp_320x240) {
  compareBitmaps(Get_1bpp_320x240_Tmpl_Bmp_Array(),
                 Get_1bpp_320x240_Bmp_Array());
}

BOOST_AUTO_TEST_CASE(Bmp1bpp_320x240_Color) {
  compareBitmaps(Get_1bpp_320x240_Color_Tmpl_Bmp_Array(),
                 Get_1bpp_320x240_Color_Bmp_Array());
}

BOOST_AUTO_TEST_CASE(Bmp1bpp_320x240_Overlappingcolor) {
  compareBitmaps(Get_1bpp_320x240_Overlappingcolor_Tmpl_Bmp_Array(),
                 Get_1bpp_320x240_Overlappingcolor_Bmp_Array());
}

BOOST_AUTO_TEST_CASE(Bmp1bpp_321x240) {
  compareBitmaps(Get_1bpp_321x240_Tmpl_Bmp_Array(),
                 Get_1bpp_321x240_Bmp_Array());
}

BOOST_AUTO_TEST_CASE(Bmp1bpp_322x240) {
  compareBitmaps(Get_1bpp_322x240_Tmpl_Bmp_Array(),
                 Get_1bpp_322x240_Bmp_Array());
}

BOOST_AUTO_TEST_CASE(Bmp1bpp_323x240) {
  compareBitmaps(Get_1bpp_323x240_Tmpl_Bmp_Array(),
                 Get_1bpp_323x240_Bmp_Array());
}

BOOST_AUTO_TEST_CASE(Bmp1bpp_324x240) {
  compareBitmaps(Get_1bpp_324x240_Tmpl_Bmp_Array(),
                 Get_1bpp_324x240_Bmp_Array());
}

BOOST_AUTO_TEST_CASE(Bmp1bpp_325x240) {
  compareBitmaps(Get_1bpp_325x240_Tmpl_Bmp_Array(),
                 Get_1bpp_325x240_Bmp_Array());
}

BOOST_AUTO_TEST_CASE(Bmp1bpp_326x240) {
  compareBitmaps(Get_1bpp_326x240_Tmpl_Bmp_Array(),
                 Get_1bpp_326x240_Bmp_Array());
}

BOOST_AUTO_TEST_CASE(Bmp1bpp_327x240) {
  compareBitmaps(Get_1bpp_327x240_Tmpl_Bmp_Array(),
                 Get_1bpp_327x240_Bmp_Array());
}

BOOST_AUTO_TEST_CASE(Bmp1bpp_328x240) {
  compareBitmaps(Get_1bpp_328x240_Tmpl_Bmp_Array(),
                 Get_1bpp_328x240_Bmp_Array());
}

BOOST_AUTO_TEST_CASE(Bmp1bpp_329x240) {
  compareBitmaps(Get_1bpp_329x240_Tmpl_Bmp_Array(),
                 Get_1bpp_329x240_Bmp_Array());
}

BOOST_AUTO_TEST_CASE(Bmp1bpp_330x240) {
  compareBitmaps(Get_1bpp_330x240_Tmpl_Bmp_Array(),
                 Get_1bpp_330x240_Bmp_Array());
}

BOOST_AUTO_TEST_CASE(Bmp1bpp_331x240) {
  compareBitmaps(Get_1bpp_331x240_Tmpl_Bmp_Array(),
                 Get_1bpp_331x240_Bmp_Array());
}

BOOST_AUTO_TEST_CASE(Bmp1bpp_332x240) {
  compareBitmaps(Get_1bpp_332x240_Tmpl_Bmp_Array(),
                 Get_1bpp_332x240_Bmp_Array());
}

BOOST_AUTO_TEST_CASE(Bmp1bpp_333x240) {
  compareBitmaps(Get_1bpp_333x240_Tmpl_Bmp_Array(),
                 Get_1bpp_333x240_Bmp_Array());
}

BOOST_AUTO_TEST_CASE(Bmp1bpp_334x240) {
  compareBitmaps(Get_1bpp_334x240_Tmpl_Bmp_Array(),
                 Get_1bpp_334x240_Bmp_Array());
}

BOOST_AUTO_TEST_CASE(Bmp1bpp_335x240) {
  compareBitmaps(Get_1bpp_335x240_Tmpl_Bmp_Array(),
                 Get_1bpp_335x240_Bmp_Array());
}

BOOST_AUTO_TEST_CASE(Bmp1bpp_Topdown_320x240) {
  compareBitmaps(Get_1bpp_320x240_Tmpl_Bmp_Array(),
                 Get_1bpp_Topdown_320x240_Bmp_Array());
}

// 4 bit per pixel
BOOST_AUTO_TEST_CASE(Bmp4bpp_1x1) {
  compareBitmaps(Get_4bpp_1x1_Tmpl_Bmp_Array(),
                 Get_4bpp_1x1_Bmp_Array());
}

BOOST_AUTO_TEST_CASE(Bmp4bpp_320x240) {
  compareBitmaps(Get_4bpp_320x240_Tmpl_Bmp_Array(),
                 Get_4bpp_320x240_Bmp_Array());
}

BOOST_AUTO_TEST_CASE(Bmp4bpp_321x240) {
  compareBitmaps(Get_4bpp_321x240_Tmpl_Bmp_Array(),
                 Get_4bpp_321x240_Bmp_Array());
}

BOOST_AUTO_TEST_CASE(Bmp4bpp_322x240) {
  compareBitmaps(Get_4bpp_322x240_Tmpl_Bmp_Array(),
                 Get_4bpp_322x240_Bmp_Array());
}

BOOST_AUTO_TEST_CASE(Bmp4bpp_323x240) {
  compareBitmaps(Get_4bpp_323x240_Tmpl_Bmp_Array(),
                 Get_4bpp_323x240_Bmp_Array());
}

BOOST_AUTO_TEST_CASE(Bmp4bpp_324x240) {
  compareBitmaps(Get_4bpp_324x240_Tmpl_Bmp_Array(),
                 Get_4bpp_324x240_Bmp_Array());
}

BOOST_AUTO_TEST_CASE(Bmp4bpp_325x240) {
  compareBitmaps(Get_4bpp_325x240_Tmpl_Bmp_Array(),
                 Get_4bpp_325x240_Bmp_Array());
}

BOOST_AUTO_TEST_CASE(Bmp4bpp_326x240) {
  compareBitmaps(Get_4bpp_326x240_Tmpl_Bmp_Array(),
                 Get_4bpp_326x240_Bmp_Array());
}

BOOST_AUTO_TEST_CASE(Bmp4bpp_327x240) {
  compareBitmaps(Get_4bpp_327x240_Tmpl_Bmp_Array(),
                 Get_4bpp_327x240_Bmp_Array());
}

BOOST_AUTO_TEST_CASE(Bmp4bpp_Topdown_320x240) {
  compareBitmaps(Get_4bpp_320x240_Tmpl_Bmp_Array(),
                 Get_4bpp_Topdown_320x240_Bmp_Array());
}

// 8 bit per pixel
BOOST_AUTO_TEST_CASE(Bmp8bpp_1x1) {
  compareBitmaps(Get_8bpp_1x1_Tmpl_Bmp_Array(),
                 Get_8bpp_1x1_Bmp_Array());
}

BOOST_AUTO_TEST_CASE(Bmp8bpp_1x64000) {
  compareBitmaps(Get_8bpp_1x64000_Tmpl_Bmp_Array(),
                 Get_8bpp_1x64000_Bmp_Array());
}

BOOST_AUTO_TEST_CASE(Bmp8bpp_320x240) {
  compareBitmaps(Get_8bpp_320x240_Tmpl_Bmp_Array(),
                 Get_8bpp_320x240_Bmp_Array());
}

BOOST_AUTO_TEST_CASE(Bmp8bpp_321x240) {
  compareBitmaps(Get_8bpp_321x240_Tmpl_Bmp_Array(),
                 Get_8bpp_321x240_Bmp_Array());
}

BOOST_AUTO_TEST_CASE(Bmp8bpp_322x240) {
  compareBitmaps(Get_8bpp_322x240_Tmpl_Bmp_Array(),
                 Get_8bpp_322x240_Bmp_Array());
}

BOOST_AUTO_TEST_CASE(Bmp8bpp_323x240) {
  compareBitmaps(Get_8bpp_323x240_Tmpl_Bmp_Array(),
                 Get_8bpp_323x240_Bmp_Array());
}

BOOST_AUTO_TEST_CASE(Bmp8bpp_Colorsimportant_320x240) {
  compareBitmaps(Get_8bpp_Colorsimportant_Two_Tmpl_Bmp_Array(),
                 Get_8bpp_Colorsimportant_Two_Bmp_Array());
}

BOOST_AUTO_TEST_CASE(Bmp8bpp_Colorused_320x240) {
  compareBitmaps(Get_8bpp_Colorsused_Zero_Tmpl_Bmp_Array(),
                 Get_8bpp_Colorsused_Zero_Bmp_Array());
}

BOOST_AUTO_TEST_CASE(Bmp8bpp_Topdown_320x240) {
  compareBitmaps(Get_8bpp_320x240_Tmpl_Bmp_Array(),
                 Get_8bpp_Topdown_320x240_Bmp_Array());
}

// 16 bit per pixel
BOOST_AUTO_TEST_CASE(Bmp16bpp_444_199x203) {
  compareBitmaps(Get_444_199x203_Tmpl_Bmp_Array(),
                 Get_444_199x203_Bmp_Array());
}

BOOST_AUTO_TEST_CASE(Bmp16bpp_444_199x203_Topdown) {
  compareBitmaps(Get_444_199x203_Tmpl_Bmp_Array(),
                 Get_444_199x203_Topdown_Bmp_Array());
}

BOOST_AUTO_TEST_CASE(Bmp16bpp_555_1x1) {
  compareBitmaps(Get_555_1x1_Tmpl_Bmp_Array(),
                 Get_555_1x1_Bmp_Array());
}

BOOST_AUTO_TEST_CASE(Bmp16bpp_555_320x240) {
  compareBitmaps(Get_555_320x240_Tmpl_Bmp_Array(),
                 Get_555_320x240_Bmp_Array());
}

BOOST_AUTO_TEST_CASE(Bmp16bpp_555_321x240) {
  compareBitmaps(Get_555_321x240_Tmpl_Bmp_Array(),
                 Get_555_321x240_Bmp_Array());
}

BOOST_AUTO_TEST_CASE(Bmp16bpp_565_1x1) {
  compareBitmaps(Get_565_1x1_Tmpl_Bmp_Array(),
                 Get_565_1x1_Bmp_Array());
}

BOOST_AUTO_TEST_CASE(Bmp16bpp_565_320x240) {
  compareBitmaps(Get_565_320x240_Tmpl_Bmp_Array(),
                 Get_565_320x240_Bmp_Array());
}

BOOST_AUTO_TEST_CASE(Bmp16bpp_565_320x240_Topdown) {
  compareBitmaps(Get_565_320x240_Tmpl_Bmp_Array(),
                 Get_565_320x240_Topdown_Bmp_Array());
}

BOOST_AUTO_TEST_CASE(Bmp16bpp_565_321x240) {
  compareBitmaps(Get_565_321x240_Tmpl_Bmp_Array(),
                 Get_565_321x240_Bmp_Array());
}

BOOST_AUTO_TEST_CASE(Bmp16bpp_565_321x240_Topdown) {
  compareBitmaps(Get_565_321x240_Tmpl_Bmp_Array(),
                 Get_565_321x240_Topdown_Bmp_Array());
}

BOOST_AUTO_TEST_CASE(Bmp16bpp_565_322x240) {
  compareBitmaps(Get_565_322x240_Tmpl_Bmp_Array(),
                 Get_565_322x240_Bmp_Array());
}

BOOST_AUTO_TEST_CASE(Bmp16bpp_565_322x240_Topdown) {
  compareBitmaps(Get_565_322x240_Tmpl_Bmp_Array(),
                 Get_565_322x240_Topdown_Bmp_Array());
}

BOOST_AUTO_TEST_CASE(Bmp16bpp_1555_199x203) {
  compareBitmaps(Get_1555_199x203_Tmpl_Bmp_Array(),
                 Get_1555_199x203_Bmp_Array());
}

BOOST_AUTO_TEST_CASE(Bmp16bpp_1555_199x203_Topdown) {
  compareBitmaps(Get_1555_199x203_Tmpl_Bmp_Array(),
                 Get_1555_199x203_Topdown_Bmp_Array());
}

BOOST_AUTO_TEST_CASE(Bmp16bpp_4444_199x203) {
  compareBitmaps(Get_4444_199x203_Tmpl_Bmp_Array(),
                 Get_4444_199x203_Bmp_Array());
}

BOOST_AUTO_TEST_CASE(Bmp16bpp_4444_199x203_Topdown) {
  compareBitmaps(Get_4444_199x203_Tmpl_Bmp_Array(),
                 Get_4444_199x203_Topdown_Bmp_Array());
}

// rle4
BOOST_AUTO_TEST_CASE(BmpRle4_Absolute_320x240) {
  compareBitmaps(Get_Rle4_Absolute_320x240_Tmpl_Bmp_Array(),
                 Get_Rle4_Absolute_320x240_Bmp_Array());
}

BOOST_AUTO_TEST_CASE(BmpRle4_Alternate_320x240) {
  compareBitmaps(Get_Rle4_Alternate_320x240_Tmpl_Bmp_Array(),
                 Get_Rle4_Alternate_320x240_Bmp_Array());
}

BOOST_AUTO_TEST_CASE(BmpRle4_Delta_320x240) {
  compareBitmaps(Get_Rle4_Delta_320x240_Tmpl_Bmp_Array(),
                 Get_Rle4_Delta_320x240_Bmp_Array());
}

BOOST_AUTO_TEST_CASE(BmpRle4_Encoded_320x240) {
  compareBitmaps(Get_Rle4_Encoded_320x240_Tmpl_Bmp_Array(),
                 Get_Rle4_Encoded_320x240_Bmp_Array());
}

// rle8
BOOST_AUTO_TEST_CASE(BmpRle8_64000x1) {
  img::Image original_image = img::Image::loadFrom("bmp", Get_Rle8_64000x1_Tmpl_Bmp_Array());
  img::Image loaded_image = img::Image::loadFrom("bmp", Get_Rle8_64000x1_Bmp_Array());

  img::rgba2rgb(original_image, original_image);
  compareBitmaps(original_image, loaded_image);
}

BOOST_AUTO_TEST_CASE(BmpRle8_Absolute_320x240) {
  compareBitmaps(Get_Rle8_Absolute_320x240_Tmpl_Bmp_Array(),
                 Get_Rle8_Absolute_320x240_Bmp_Array());
}

BOOST_AUTO_TEST_CASE(BmpRle8_Blank_160x120) {
  compareBitmaps(Get_Rle8_Blank_160x120_Tmpl_Bmp_Array(),
                 Get_Rle8_Blank_160x120_Bmp_Array());
}

BOOST_AUTO_TEST_CASE(BmpRle8_Delta_320x240) {
  compareBitmaps(Get_Rle8_Delta_320x240_Tmpl_Bmp_Array(),
                 Get_Rle8_Delta_320x240_Bmp_Array());
}

BOOST_AUTO_TEST_CASE(BmpRle8_Encoded_320x240) {
  compareBitmaps(Get_Rle8_Encoded_320x240_Tmpl_Bmp_Array(),
                 Get_Rle8_Encoded_320x240_Bmp_Array());
}

BOOST_AUTO_TEST_SUITE_END()
} // namespace test

namespace test {
// --log_level=test_suite --run_test=TestBmpValid
BOOST_FIXTURE_TEST_SUITE(TestBmpCorrupt, BmpFixtureCorrupt)

BOOST_AUTO_TEST_CASE(Bmp1bpp_No_Palette) {
  checkNotLoaded(Get_1bpp_No_Palette_Bmp_Array());
}

BOOST_AUTO_TEST_CASE(Bmp1bpp_Pixeldata_Cropped) {
  checkNotLoaded(Get_1bpp_Pixeldata_Cropped_Bmp_Array());
}

BOOST_AUTO_TEST_CASE(Bmp4bpp_No_Palette) {
  checkNotLoaded(Get_4bpp_No_Palette_Bmp_Array());
}

BOOST_AUTO_TEST_CASE(Bmp4bpp_Pixeldata_Cropped) {
  checkNotLoaded(Get_4bpp_Pixeldata_Cropped_Bmp_Array());
}

BOOST_AUTO_TEST_CASE(Bmp8bpp_Colorsimportant_Large) {
  checkNotLoaded(Get_8bpp_Colorsimportant_Large_Bmp_Array());
}

BOOST_AUTO_TEST_CASE(Bmp8bpp_Colorsimportant_Negative) {
  checkNotLoaded(Get_8bpp_Colorsimportant_Negative_Bmp_Array());
}

BOOST_AUTO_TEST_CASE(Bmp8bpp_Colorsused_Large) {
  checkNotLoaded(Get_8bpp_Colorsused_Large_Bmp_Array());
}

BOOST_AUTO_TEST_CASE(Bmp8bpp_Colorsused_Negative) {
  checkNotLoaded(Get_8bpp_Colorsused_Negative_Bmp_Array());
}

BOOST_AUTO_TEST_CASE(Bmp8bpp_No_Palette) {
  checkNotLoaded(Get_8bpp_No_Palette_Bmp_Array());
}

BOOST_AUTO_TEST_CASE(Bmp8bpp_Pixeldata_Cropped) {
  checkNotLoaded(Get_8bpp_Pixeldata_Cropped_Bmp_Array());
}

BOOST_AUTO_TEST_CASE(Bmp24bpp_Pixeldata_Cropped) {
  checkNotLoaded(Get_24bpp_Pixeldata_Cropped_Bmp_Array());
}

BOOST_AUTO_TEST_CASE(Bmp32bpp_Pixeldata_Cropped) {
  checkNotLoaded(Get_32bpp_Pixeldata_Cropped_Bmp_Array());
}

BOOST_AUTO_TEST_CASE(Bmp555_Pixeldata_Cropped) {
  checkNotLoaded(Get_555_Pixeldata_Cropped_Bmp_Array());
}

BOOST_AUTO_TEST_CASE(BmpBitdepth_Large) {
  checkNotLoaded(Get_Bitdepth_Large_Bmp_Array());
}

BOOST_AUTO_TEST_CASE(BmpBitdepth_Odd) {
  checkNotLoaded(Get_Bitdepth_Odd_Bmp_Array());
}

BOOST_AUTO_TEST_CASE(BmpBitdepth_Zero) {
  checkNotLoaded(Get_Bitdepth_Zero_Bmp_Array());
}

BOOST_AUTO_TEST_CASE(BmpColormasks_Cropped) {
  checkNotLoaded(Get_Colormasks_Cropped_Bmp_Array());
}

BOOST_AUTO_TEST_CASE(BmpColormasks_Missing) {
  checkNotLoaded(Get_Colormasks_Missing_Bmp_Array());
}

BOOST_AUTO_TEST_CASE(BmpCompression_Bad_Rle4_For_8bpp) {
  checkNotLoaded(Get_Compression_Bad_Rle4_For_8bpp_Bmp_Array());
}

BOOST_AUTO_TEST_CASE(BmpCompression_Bad_Rle8_For_4bpp) {
  checkNotLoaded(Get_Compression_Bad_Rle8_For_4bpp_Bmp_Array());
}

BOOST_AUTO_TEST_CASE(BmpCompression_Unknown) {
  checkNotLoaded(Get_Compression_Unknown_Bmp_Array());
}

BOOST_AUTO_TEST_CASE(BmpFileinfoheader_Cropped) {
  checkNotLoaded(Get_Fileinfoheader_Cropped_Bmp_Array());
}

BOOST_AUTO_TEST_CASE(BmpHeight_Zero) {
  checkNotLoaded(Get_Height_Zero_Bmp_Array());
}

BOOST_AUTO_TEST_CASE(BmpInfoheader_Cropped) {
  checkNotLoaded(Get_Infoheader_Cropped_Bmp_Array());
}

BOOST_AUTO_TEST_CASE(BmpInfoheader_Missing) {
  checkNotLoaded(Get_Infoheader_Missing_Bmp_Array());
}

BOOST_AUTO_TEST_CASE(BmpInfoheadersize_Large) {
  checkNotLoaded(Get_Infoheadersize_Large_Bmp_Array());
}

BOOST_AUTO_TEST_CASE(BmpInfoheadersize_Small) {
  checkNotLoaded(Get_Infoheadersize_Small_Bmp_Array());
}

BOOST_AUTO_TEST_CASE(BmpInfoheadersize_Zero) {
  checkNotLoaded(Get_Infoheadersize_Zero_Bmp_Array());
}

BOOST_AUTO_TEST_CASE(BmpMagicnumber_Bad) {
  checkNotLoaded(Get_Magicnumber_Bad_Bmp_Array());
}

BOOST_AUTO_TEST_CASE(BmpMagicnumber_Cropped) {
  checkNotLoaded(Get_Magicnumber_Cropped_Bmp_Array());
}

BOOST_AUTO_TEST_CASE(BmpOffbits_Large) {
  checkNotLoaded(Get_Offbits_Large_Bmp_Array());
}

BOOST_AUTO_TEST_CASE(BmpOffbits_Negative) {
  checkNotLoaded(Get_Offbits_Negative_Bmp_Array());
}

BOOST_AUTO_TEST_CASE(BmpOffbits_Zero) {
  checkNotLoaded(Get_Offbits_Zero_Bmp_Array());
}

BOOST_AUTO_TEST_CASE(BmpPalette_Cropped) {
  checkNotLoaded(Get_Palette_Cropped_Bmp_Array());
}

BOOST_AUTO_TEST_CASE(BmpPalette_Missing) {
  checkNotLoaded(Get_Palette_Missing_Bmp_Array());
}

BOOST_AUTO_TEST_CASE(BmpPalette_Too_Big) {
  checkNotLoaded(Get_Palette_Too_Big_Bmp_Array());
}

BOOST_AUTO_TEST_CASE(BmpPixeldata_Missing) {
  checkNotLoaded(Get_Pixeldata_Missing_Bmp_Array());
}

BOOST_AUTO_TEST_CASE(BmpRle4_Absolute_Cropped) {
  checkNotLoaded(Get_Rle4_Absolute_Cropped_Bmp_Array());
}

BOOST_AUTO_TEST_CASE(BmpRle4_Delta_Cropped) {
  checkNotLoaded(Get_Rle4_Delta_Cropped_Bmp_Array());
}

BOOST_AUTO_TEST_CASE(BmpRle4_No_End_Of_Line_Marker) {
  checkNotLoaded(Get_Rle4_No_End_Of_Line_Marker_Bmp_Array());
}

BOOST_AUTO_TEST_CASE(BmpRle4_Runlength_Cropped) {
  checkNotLoaded(Get_Rle4_Runlength_Cropped_Bmp_Array());
}

BOOST_AUTO_TEST_CASE(BmpRle8_Absolute_Cropped) {
  checkNotLoaded(Get_Rle8_Absolute_Cropped_Bmp_Array());
}

BOOST_AUTO_TEST_CASE(BmpRle8_Delta_Cropped) {
  checkNotLoaded(Get_Rle8_Delta_Cropped_Bmp_Array());
}

BOOST_AUTO_TEST_CASE(BmpRle8_Deltaleavesimage) {
  checkNotLoaded(Get_Rle8_Deltaleavesimage_Bmp_Array());
}

BOOST_AUTO_TEST_CASE(BmpRle8_No_End_Of_Line_Marker) {
  checkNotLoaded(Get_Rle8_No_End_Of_Line_Marker_Bmp_Array());
}

BOOST_AUTO_TEST_CASE(BmpRle8_Runlength_Cropped) {
  checkNotLoaded(Get_Rle8_Runlength_Cropped_Bmp_Array());
}

BOOST_AUTO_TEST_CASE(BmpWidth_Negative) {
  checkNotLoaded(Get_Width_Negative_Bmp_Array());
}

BOOST_AUTO_TEST_CASE(BmpWidth_Times_Height_Overflow) {
  checkNotLoaded(Get_Width_Times_Height_Overflow_Bmp_Array());
}

BOOST_AUTO_TEST_CASE(BmpWidth_Zero) {
  checkNotLoaded(Get_Width_Zero_Bmp_Array());
}

BOOST_AUTO_TEST_SUITE_END()
} // namespace test

namespace test {
// --log_level=test_suite --run_test=TestBmpQuestionable
BOOST_FIXTURE_TEST_SUITE(TestBmpQuestionable, BmpFixture)

BOOST_FIXTURE_TEST_CASE(Bmp8bpp_pixels_not_in_palette, BmpFixtureCorrupt) {
  checkNotLoaded(Get_8bpp_Pixels_Not_In_Palette_Bmp_Array());
}

BOOST_FIXTURE_TEST_CASE(Bmp32bpp_0x0, BmpFixtureCorrupt) {
  checkNotLoaded(Get_32bpp_0x0_Bmp_Array());
}

BOOST_FIXTURE_TEST_CASE(Bmp32bpp_0x240, BmpFixtureCorrupt) {
  checkNotLoaded(Get_32bpp_0x240_Bmp_Array());
}

BOOST_FIXTURE_TEST_CASE(Bmp32bpp_320x0, BmpFixtureCorrupt) {
  checkNotLoaded(Get_32bpp_320x0_Bmp_Array());
}

BOOST_AUTO_TEST_CASE(Bmp_Filesize_Bad) {
  compareBitmaps(Get_Filesize_Bad_Bmp_Array(),
                 Get_1bpp_320x240_Bmp_Array());
}

BOOST_AUTO_TEST_CASE(Bmp_Filesize_Zero) {
  compareBitmaps(Get_Filesize_Zero_Bmp_Array(),
                 Get_1bpp_320x240_Bmp_Array());
}

BOOST_AUTO_TEST_CASE(BmpPelsPerMeterXLarge) {
  compareBitmaps(Get_Pels_Per_Meter_X_Large_Bmp_Array(),
                 Get_8bpp_320x240_Tmpl_Bmp_Array());
}

BOOST_AUTO_TEST_CASE(BmpPelsPerMeterXNegative) {
  compareBitmaps(Get_Pels_Per_Meter_X_Negative_Bmp_Array(),
                 Get_8bpp_320x240_Tmpl_Bmp_Array());
}

BOOST_AUTO_TEST_CASE(BmpPelsPerMeterXZero) {
  compareBitmaps(Get_Pels_Per_Meter_X_Zero_Bmp_Array(),
                 Get_8bpp_320x240_Tmpl_Bmp_Array());
}

BOOST_AUTO_TEST_CASE(BmpPelsPerMeterYLarge) {
  compareBitmaps(Get_Pels_Per_Meter_Y_Large_Bmp_Array(),
                 Get_8bpp_320x240_Tmpl_Bmp_Array());
}

BOOST_AUTO_TEST_CASE(BmpPelsPerMeterYNegative) {
  compareBitmaps(Get_Pels_Per_Meter_Y_Negative_Bmp_Array(),
                 Get_8bpp_320x240_Tmpl_Bmp_Array());
}

BOOST_AUTO_TEST_CASE(BmpPelsPerMeterYZero) {
  compareBitmaps(Get_Pels_Per_Meter_Y_Zero_Bmp_Array(),
                 Get_8bpp_320x240_Tmpl_Bmp_Array());
}

BOOST_AUTO_TEST_CASE(BmpPixeldataRle8Toomuch) {
  compareBitmaps(Get_Pixeldata_Rle8_Toomuch_Bmp_Array(),
                 Get_8bpp_320x240_Tmpl_Bmp_Array());
}

BOOST_AUTO_TEST_CASE(BmpPixeldataToomuch) {
  compareBitmaps(Get_Pixeldata_Toomuch_Bmp_Array(),
                 Get_1bpp_320x240_Tmpl_Bmp_Array());
}

BOOST_AUTO_TEST_CASE(BmpPlanesLarge) {
  compareBitmaps(Get_Planes_Large_Bmp_Array(),
                 Get_1bpp_320x240_Tmpl_Bmp_Array());
}

BOOST_AUTO_TEST_CASE(BmpPlanesZero) {
  compareBitmaps(Get_Planes_Zero_Bmp_Array(),
                 Get_1bpp_320x240_Tmpl_Bmp_Array());
}

BOOST_AUTO_TEST_CASE(BmpReversed1Bad) {
  compareBitmaps(Get_Reserved1_Bad_Bmp_Array(),
                 Get_1bpp_320x240_Tmpl_Bmp_Array());
}

BOOST_AUTO_TEST_CASE(BmpReversed2Bad) {
  compareBitmaps(Get_Reserved2_Bad_Bmp_Array(),
                 Get_1bpp_320x240_Tmpl_Bmp_Array());
}

BOOST_AUTO_TEST_CASE(BmpRle4HeightNegative) {
  compareBitmaps(Get_Rle4_Height_Negative_Bmp_Array(),
                 Get_8bpp_320x240_Tmpl_Bmp_Array());
}

BOOST_AUTO_TEST_CASE(BmpRle4NoEndOfBitmap) {
  compareBitmaps(Get_Rle4_No_End_Of_Bitmap_Marker_Bmp_Array(),
                 Get_8bpp_320x240_Tmpl_Bmp_Array());
}

BOOST_AUTO_TEST_CASE(BmpRle8HeightNegative) {
  compareBitmaps(Get_Rle8_Height_Negative_Bmp_Array(),
                 Get_8bpp_320x240_Tmpl_Bmp_Array());
}

BOOST_AUTO_TEST_CASE(BmpRle8NoEndOfBitmap) {
  compareBitmaps(Get_Rle8_No_End_Of_Bitmap_Marker_Bmp_Array(),
                 Get_8bpp_320x240_Tmpl_Bmp_Array());
}


BOOST_AUTO_TEST_SUITE_END()
} // namespace test