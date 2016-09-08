#include "scale.h"

#include "image.h"

#include <vector>
#include <assert.h>

namespace {
#ifdef __ARM__
typedef float FloatType;
#else
typedef double FloatType;
#endif

static const FloatType ZeroValue = 0.0;
static const FloatType OneValue  = 1.0;
}

namespace img {
inline FloatType spline_cube(FloatType value) {
  return value <= ZeroValue ? ZeroValue : value * value * value;
}

inline FloatType spline_weight(FloatType value) {
  return (spline_cube(value + 2) -
          4 * spline_cube(value + 1) +
          6 * spline_cube(value) -
          4 * spline_cube(value - 1)) / 6;
}

struct BicubicPrecalc {
  FloatType weight[4];
  int offset[4];
};

inline void precalculate(std::vector<BicubicPrecalc>& weight, int orig_dim) {
  const int new_size = weight.size();
  for (int dstd = 0; dstd < new_size; dstd++) {
    // We need to calculate the source pixel to interpolate from - Y-axis
    const FloatType srcpixd = static_cast<FloatType>(dstd * orig_dim) / new_size;
    const FloatType dd = srcpixd - static_cast<int>(srcpixd);

    BicubicPrecalc& pre_calc = weight[dstd];

    for (int k = -1; k <= 2; k++) {
      const int offset = static_cast<int>(srcpixd + k);
      pre_calc.offset[k + 1] = offset < 0
                               ? 0
                               : offset >= orig_dim
                               ? orig_dim - 1
                               : offset;

      pre_calc.weight[k + 1] = spline_weight(k - dd);
    }
  }
}

template<int Depth>
void resampleBicubic(const Image& in, Image& out, int width, int height) {
  const unsigned char* src_data = in.data();
  unsigned char* dst_data   = out.data();

  const img::Image::SizeType src_image_width = in.width();
  const img::Image::SizeType src_image_height = in.height();
  // Precalculate weights
  std::vector<BicubicPrecalc> weight_y(height);
  std::vector<BicubicPrecalc> weight_x(width);
  precalculate(weight_y, src_image_height);
  precalculate(weight_x, src_image_width);
  // ~Precalculate weights

  for (int dsty = 0; dsty < height; dsty++) {
    // We need to calculate the source pixel to interpolate from - Y-axis
    const BicubicPrecalc& pre_y = weight_y[dsty];

    for (int dstx = 0; dstx < width; dstx++) {
      // X-axis of pixel to interpolate from
      const BicubicPrecalc& pre_x = weight_x[dstx];

      // Sums for each color channel
      //FloatType sum_r = 0, sum_g = 0, sum_b = 0;//, sum_a = 0;
      FloatType sum_pixel[Depth] = {0};

      // Here we actually determine the RGBA values for the destination pixel
      for (int k = -1; k <= 2; k++) {
        // Y offset
        const int y_offset = pre_y.offset[k + 1];

        const int src_pixel_middle = y_offset * src_image_width;
        // Loop across the X axis
        for (int i = -1; i <= 2; i++) {
          // X offset
          const int x_offset = pre_x.offset[i + 1];

          // Calculate the exact position where the source data
          // should be pulled from based on the x_offset and y_offset
          const int src_pixel_index = src_pixel_middle + x_offset;

          // Calculate the weight for the specified pixel according
          // to the bicubic b-spline kernel we're using for
          // interpolation
          FloatType pixel_weight = pre_y.weight[k + 1] * pre_x.weight[i + 1];

          // Create a sum of all values for each color channel
          // adjusted for the pixel's calculated weight

          const int src_pos_beg =  src_pixel_index * Depth;
          for (int cnt = 0; cnt < Depth; ++cnt)
            sum_pixel[cnt] += src_data[src_pos_beg + cnt] * pixel_weight;
        }
      }

      // Put the data into the destination image.  The summed values are
      // of FloatType data type and are rounded here for accuracy
      for (int cnt = 0; cnt < Depth; ++cnt)
        dst_data[cnt] = static_cast<unsigned char>(sum_pixel[cnt] + 0.5);

      dst_data += Depth;
    }
  }
}

//////////////////////////////////////////////////////////////////////////
template<int Depth>
void resampleNearest(const Image& in, Image& out, int width, int height) {
  const unsigned char* source_data = in.data();
  unsigned char* target_data   = out.data();

  const long old_height = in.height();
  const long old_width  = in.width();
  const long x_delta = (old_width << 16) / width;
  const long y_delta = (old_height << 16) / height;

  unsigned char* dest_pixel = target_data;

  long y = 0;
  for (long j = 0; j < height; j++) {
    const unsigned char* src_line = &source_data[(y >> 16) * old_width * Depth];

    long x = 0;
    for (long i = 0; i < width; i++) {
      const unsigned char* src_pixel = &src_line[(x >> 16) * Depth];
      for (int cnt = 0; cnt < Depth; ++cnt)
        dest_pixel[cnt] = src_pixel[cnt];
      dest_pixel += Depth;

      x += x_delta;
    }

    y += y_delta;
  }
}

struct BilinearPrecalc {
  int offset1;
  int offset2;
  FloatType dd;
  FloatType dd1;
};

void precalculate(std::vector<BilinearPrecalc>& weight, int orig_dim) {
  const int new_size  = weight.size();
  const FloatType HFactor = FloatType(orig_dim) / new_size;
  const int srcpixmax = orig_dim - 1;

  //BilinearPrecalc precalc;

  for (int dsty = 0; dsty < new_size; dsty++) {
    // We need to calculate the source pixel to interpolate from - Y-axis
    FloatType srcpix = FloatType(dsty) * HFactor;
    FloatType srcpix1 = static_cast<FloatType>(static_cast<int>(srcpix));
    FloatType srcpix2 = (srcpix1 == srcpixmax) ? srcpix1 : srcpix1 + OneValue;

    BilinearPrecalc& pre_calc = weight[dsty];

    pre_calc.dd = srcpix - (int)srcpix;
    pre_calc.dd1 = OneValue - pre_calc.dd;
    pre_calc.offset1 = srcpix1<ZeroValue ? 0 : srcpix1> srcpixmax ? srcpixmax : (int)srcpix1;
    pre_calc.offset2 = srcpix2<ZeroValue ? 0 : srcpix2> srcpixmax ? srcpixmax : (int)srcpix2;
  }
}

template<int Depth>
void resampleBilinear(const Image& in, Image& out, int width, int height) {
  const unsigned char* src_data = in.data();
  unsigned char* dst_data = out.data();

  std::vector<BilinearPrecalc> weight_y(height);
  std::vector<BilinearPrecalc> weight_x(width);
  precalculate(weight_y, in.height());
  precalculate(weight_x, in.width());

  // initialize alpha values to avoid g++ warnings about possibly
  // uninitialized variables
  FloatType pixel1[Depth];
  FloatType pixel2[Depth];

  for (int dsty = 0; dsty < height; dsty++) {
    // We need to calculate the source pixel to interpolate from - Y-axis
    const BilinearPrecalc& pte_y = weight_y[dsty];


    for (int dstx = 0; dstx < width; dstx++) {
      // X-axis of pixel to interpolate from
      const BilinearPrecalc& pte_x = weight_x[dstx];

      int src_pixel_index00 = pte_y.offset1 * in.width() + pte_x.offset1;
      int src_pixel_index01 = pte_y.offset1 * in.width() + pte_x.offset2;
      int src_pixel_index10 = pte_y.offset2 * in.width() + pte_x.offset1;
      int src_pixel_index11 = pte_y.offset2 * in.width() + pte_x.offset2;

      // first line
      for (int cnt = 0; cnt < Depth; ++cnt)
        pixel1[cnt] = src_data[src_pixel_index00 * Depth + cnt] * pte_x.dd1 + src_data[src_pixel_index01 * Depth + cnt] * pte_x.dd;

      // second line
      for (int cnt = 0; cnt < Depth; ++cnt)
        pixel2[cnt] = src_data[src_pixel_index10 * Depth + cnt] * pte_x.dd1 + src_data[src_pixel_index11 * Depth + cnt] * pte_x.dd;

      // result lines
      for (int cnt = 0; cnt < Depth; ++cnt)
        dst_data[cnt] = static_cast<unsigned char>(pixel1[cnt] * pte_y.dd1 + pixel2[cnt] * pte_y.dd);

      dst_data += Depth;
    }
  }
}

//////////////////////////////////////////////////////////////////////////
struct ScaleFuncs {
  typedef void (*ScaleFun)(const Image&, Image&, int, int);

  static const int MaxDepth = 5;
  static const int FunCount = 3;

  ScaleFun funcs[FunCount][MaxDepth];
};


ScaleFuncs fillFuncs() {
  ScaleFuncs result;

  // undefined depths
  result.funcs[FastScaling][0]  = 0;
  result.funcs[MiddleScaling][0]  = 0;
  result.funcs[HighScaling][0]  = 0;
  result.funcs[FastScaling][2]  = 0;
  result.funcs[MiddleScaling][2]  = 0;
  result.funcs[HighScaling][2]  = 0;

  // grey
  result.funcs[FastScaling][1]  = resampleNearest<1>;
  result.funcs[MiddleScaling][1]  = resampleBilinear<1>;
  result.funcs[HighScaling][1]  = resampleBicubic<1>;

  // rgb, bgr, etc...
  result.funcs[FastScaling][3]  = resampleNearest<3>;
  result.funcs[MiddleScaling][3]  = resampleBilinear<3>;
  result.funcs[HighScaling][3]  = resampleBicubic<3>;

  // with alpha channel
  result.funcs[FastScaling][4]  = resampleNearest<4>;
  result.funcs[MiddleScaling][4]  = resampleBilinear<4>;
  result.funcs[HighScaling][4]  = resampleBicubic<4>;

  return result;
}

Image scale(const Image& in, Image& cached, ScaleQuality quality, int width, int height) {
  // This function implements a Bicubic B-Spline algorithm for resampling.
  // This method is certainly a little slower than wxImage's default pixel
  // replication method, however for most reasonably sized images not being
  // upsampled too much on a fairly average CPU this difference is hardly
  // noticeable and the results are far more pleasing to look at.
  //
  // This particular bicubic algorithm does pixel weighting according to a
  // B-Spline that basically implements a Gaussian bell-like weighting
  // kernel. Because of this method the results may appear a bit blurry when
  // upsampling by large factors.  This is basically because a slight
  // gaussian blur is being performed to get the smooth look of the upsampled
  // image.

  // Edge pixels: 3-4 possible solutions
  // - (Wrap/tile) Wrap the image, take the color value from the opposite
  // side of the image.
  // - (Mirror)    Duplicate edge pixels, so that pixel at coordinate (2, n),
  // where n is nonpositive, will have the value of (2, 1).
  // - (Ignore)    Simply ignore the edge pixels and apply the kernel only to
  // pixels which do have all neighbours.
  // - (Clamp)     Choose the nearest pixel along the border. This takes the
  // border pixels and extends them out to infinity.
  //
  // NOTE: below the y_offset and x_offset variables are being set for edge
  // pixels using the "Mirror" method mentioned above
  assert(in.depth() != 0);
  assert(in.depth() != 2);
  assert(in.depth() < ScaleFuncs::MaxDepth);
  assert(quality < ScaleFuncs::FunCount);

  static const ScaleFuncs funcs = fillFuncs();


//  Image result;

  if (0 == width && 0 == height)
    return Image::emptyImage;

  if (0 == height)
    height = proportionalHeight(width, in.getSize());

  if (0 == width)
    width = proportionalWidth(height, in.getSize());

  ScaleFuncs::ScaleFun fun = funcs.funcs[quality][in.depth()];

  assert(fun);
  if (fun) {
    Image temp;
    const bool use_temp = (&in == &cached);
    Image& dst = use_temp ? temp : cached;
    //Image result(width, height, in.depth());
    dst.create(width, height, in.depth());
    fun(in, dst, width, height);

    if (use_temp)
      img::copy(temp, cached);

    return cached;
  }

  return Image::emptyImage;
}

Image scale(const Image& in, ScaleQuality quality, int width, int height) {
  Image result;
  return scale(in, result, quality, width, height);
}
}
