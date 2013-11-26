#include "scale.h"

#include "image.h"

#include <vector>
#include <assert.h>

namespace
{
#ifdef __ARM__
	typedef float FloatType;
#else
	typedef double FloatType;
#endif

	static const FloatType ZeroValue	= 0.0;
	static const FloatType OneValue		= 1.0;
}

namespace img
{
	inline FloatType spline_cube(FloatType value)
	{
		return value <= ZeroValue ? ZeroValue : value * value * value;
	}

	inline FloatType spline_weight(FloatType value)
	{
		return (spline_cube(value + 2) -
			4 * spline_cube(value + 1) +
			6 * spline_cube(value) -
			4 * spline_cube(value - 1)) / 6;
	}

	struct BicubicPrecalc 
	{ 
		FloatType weight[4];
		int offset[4];
	};

	inline void precalculate(std::vector< BicubicPrecalc > &aWeight, int aOrigDim)
	{
		const int tNewSize = aWeight.size();
		for ( int dstd = 0; dstd < tNewSize; dstd++ )
		{
			// We need to calculate the source pixel to interpolate from - Y-axis
			const FloatType srcpixd = static_cast<FloatType>(dstd * aOrigDim) / tNewSize;
			const FloatType dd = srcpixd - static_cast<int>(srcpixd);

			BicubicPrecalc &tPreCalc = aWeight[dstd];

			for ( int k = -1; k <= 2; k++ )
			{
				const int tOffset = static_cast<int>(srcpixd + k);
				tPreCalc.offset[k + 1] = tOffset < 0
					? 0
					: tOffset >= aOrigDim
					? aOrigDim - 1
					: tOffset;

				tPreCalc.weight[k + 1] = spline_weight(k - dd);
			}
		}
	}

	template<int Depth>
	void resampleBicubic(const Image &aIn, Image &aOut, int width, int height)
	{
		const unsigned char* src_data	= aIn.data();
		unsigned char* dst_data			= aOut.data();

		// Precalculate weights
		std::vector< BicubicPrecalc > tWeightY(height);
		std::vector< BicubicPrecalc > tWeightX(width);
		precalculate(tWeightY, aIn.height());
		precalculate(tWeightX, aIn.width());
		// ~Precalculate weights

		for ( int dsty = 0; dsty < height; dsty++ )
		{
			// We need to calculate the source pixel to interpolate from - Y-axis
			const BicubicPrecalc &tPreY = tWeightY[dsty];

			for ( int dstx = 0; dstx < width; dstx++ )
			{
				// X-axis of pixel to interpolate from
				const BicubicPrecalc &tPreX = tWeightX[dstx];

				// Sums for each color channel
				//FloatType sum_r = 0, sum_g = 0, sum_b = 0;//, sum_a = 0;
				FloatType sum_pixel[Depth] = {0};

				// Here we actually determine the RGBA values for the destination pixel
				for ( int k = -1; k <= 2; k++ )
				{
					// Y offset
					const int y_offset = tPreY.offset[k + 1];

					// Loop across the X axis
					for ( int i = -1; i <= 2; i++ )
					{
						// X offset
						const int x_offset = tPreX.offset[i + 1];

						// Calculate the exact position where the source data
						// should be pulled from based on the x_offset and y_offset
						const int src_pixel_index = y_offset*aIn.width() + x_offset;

						// Calculate the weight for the specified pixel according
						// to the bicubic b-spline kernel we're using for
						// interpolation
						FloatType pixel_weight = tPreY.weight[k + 1] * tPreX.weight[i + 1];

						// Create a sum of all values for each color channel
						// adjusted for the pixel's calculated weight

						const int tSrcPosBeg =  src_pixel_index * Depth;
						for( int cnt = 0; cnt < Depth; ++cnt )
							sum_pixel[cnt] += src_data[tSrcPosBeg + cnt] * pixel_weight;
					}
				}

				// Put the data into the destination image.  The summed values are
				// of FloatType data type and are rounded here for accuracy
				for( int cnt = 0; cnt < Depth; ++cnt )
					dst_data[cnt] = static_cast<unsigned char>(sum_pixel[cnt] + 0.5);

				dst_data += Depth;
			}
		}
	}

	//////////////////////////////////////////////////////////////////////////
	template<int Depth>
	void resampleNearest(const Image &aIn, Image &aOut, int width, int height)
	{
		const unsigned char *source_data	= aIn.data();
		unsigned char *target_data			= aOut.data();

		const long old_height = aIn.height();
		const long old_width  = aIn.width();
		const long x_delta = (old_width<<16) / width;
		const long y_delta = (old_height<<16) / height;

		unsigned char* dest_pixel = target_data;

		long y = 0;
		for ( long j = 0; j < height; j++ )
		{
			const unsigned char* src_line = &source_data[(y>>16)*old_width*Depth];

			long x = 0;
			for ( long i = 0; i < width; i++ )
			{
				const unsigned char* src_pixel = &src_line[(x>>16)*Depth];
				for( int cnt = 0; cnt < Depth; ++cnt )
					dest_pixel[cnt] = src_pixel[cnt];
				dest_pixel += Depth;

				x += x_delta;
			}

			y += y_delta;
		}
	}

	struct BilinearPrecalc 
	{ 
		int offset1;
		int offset2;
		FloatType dd;
		FloatType dd1;
	};

	void precalculate(std::vector< BilinearPrecalc > &aWeight, int aOrigDim)
	{
		const int tNewSize		= aWeight.size();
		const FloatType HFactor	= FloatType(aOrigDim) / tNewSize;
		const int srcpixmax	= aOrigDim - 1;

		//BilinearPrecalc tPrecalc;

		for ( int dsty = 0; dsty < tNewSize; dsty++ )
		{
			// We need to calculate the source pixel to interpolate from - Y-axis
			FloatType srcpix = FloatType(dsty) * HFactor;
			FloatType srcpix1 = static_cast<FloatType>(static_cast<int>(srcpix));
			FloatType srcpix2 = ( srcpix1 == srcpixmax ) ? srcpix1 : srcpix1 + OneValue;

			BilinearPrecalc &tPreCalc = aWeight[dsty];

			tPreCalc.dd = srcpix - (int)srcpix;
			tPreCalc.dd1 = OneValue - tPreCalc.dd;
			tPreCalc.offset1 = srcpix1 < ZeroValue ? 0 : srcpix1 > srcpixmax ? srcpixmax : (int)srcpix1;
			tPreCalc.offset2 = srcpix2 < ZeroValue ? 0 : srcpix2 > srcpixmax ? srcpixmax : (int)srcpix2;
		}
	}

	template<int Depth>
	void resampleBilinear(const Image &aIn, Image &aOut, int width, int height)
	{
		const unsigned char* src_data = aIn.data();
		unsigned char* dst_data = aOut.data();

		std::vector< BilinearPrecalc > tWeightY(height);
		std::vector< BilinearPrecalc > tWeightX(width);
		precalculate(tWeightY, aIn.height());
		precalculate(tWeightX, aIn.width());

		// initialize alpha values to avoid g++ warnings about possibly
		// uninitialized variables
		FloatType pixel1[Depth];
		FloatType pixel2[Depth];

		for ( int dsty = 0; dsty < height; dsty++ )
		{
			// We need to calculate the source pixel to interpolate from - Y-axis
			const BilinearPrecalc &tPteY = tWeightY[dsty];


			for ( int dstx = 0; dstx < width; dstx++ )
			{
				// X-axis of pixel to interpolate from
				const BilinearPrecalc &tPteX = tWeightX[dstx];

				int src_pixel_index00 = tPteY.offset1 * aIn.width() + tPteX.offset1;
				int src_pixel_index01 = tPteY.offset1 * aIn.width() + tPteX.offset2;
				int src_pixel_index10 = tPteY.offset2 * aIn.width() + tPteX.offset1;
				int src_pixel_index11 = tPteY.offset2 * aIn.width() + tPteX.offset2;

				// first line
				for( int cnt = 0; cnt < Depth; ++cnt )
					pixel1[cnt] = src_data[src_pixel_index00 * Depth + cnt] * tPteX.dd1 + src_data[src_pixel_index01 * Depth + cnt] * tPteX.dd;

				// second line
				for( int cnt = 0; cnt < Depth; ++cnt )
					pixel2[cnt] = src_data[src_pixel_index10 * Depth + cnt] * tPteX.dd1 + src_data[src_pixel_index11 * Depth + cnt] * tPteX.dd;

				// result lines
				for( int cnt = 0; cnt < Depth; ++cnt )
					dst_data[cnt] = static_cast<unsigned char>(pixel1[cnt] * tPteY.dd1 + pixel2[cnt] * tPteY.dd);

				dst_data += Depth;
			}
		}
	}

	//////////////////////////////////////////////////////////////////////////
	struct ScaleFuncs
	{
		typedef void (*ScaleFun)(const Image &, Image &, int, int);
		
		static const int MaxDepth = 5;
		static const int FunCount = 3;

		ScaleFun funcs[FunCount][MaxDepth] ;
	};
	

	ScaleFuncs fillFuncs()
	{
		ScaleFuncs tResult;

		// undefined depths
		tResult.funcs[FastScaling][0]		= 0;
		tResult.funcs[MiddleScaling][0]		= 0;
		tResult.funcs[HighScaling][0]		= 0;
		tResult.funcs[FastScaling][2]		= 0;
		tResult.funcs[MiddleScaling][2]		= 0;
		tResult.funcs[HighScaling][2]		= 0;

		// grey
		tResult.funcs[FastScaling][1]		= resampleNearest<1>;
		tResult.funcs[MiddleScaling][1]		= resampleBilinear<1>;
		tResult.funcs[HighScaling][1]		= resampleBicubic<1>;

		// rgb, bgr, etc...
		tResult.funcs[FastScaling][3]		= resampleNearest<3>;
		tResult.funcs[MiddleScaling][3]		= resampleBilinear<3>;
		tResult.funcs[HighScaling][3]		= resampleBicubic<3>;

		// with alpha channel
		tResult.funcs[FastScaling][4]		= resampleNearest<4>;
		tResult.funcs[MiddleScaling][4]		= resampleBilinear<4>;
		tResult.funcs[HighScaling][4]		= resampleBicubic<4>;

		return tResult;
	}

	Image scale(const Image &aIn, Image &aCached, ScaleQuality aQuality, int width, int height)
	{
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
		assert( aIn.depth() != 0 );
		assert( aIn.depth() != 2 );
		assert( aIn.depth() < ScaleFuncs::MaxDepth );
		assert( aQuality < ScaleFuncs::FunCount );

		static const ScaleFuncs tFuncs = fillFuncs();


//		Image tResult;

		if( 0 == width && 0 == height )
			return Image::emptyImage;

		if( 0 == height )
			height = proportionalHeight(width, aIn.getSize());

		if( 0 == width )
			width = proportionalWidth(height, aIn.getSize());

		ScaleFuncs::ScaleFun tFun = tFuncs.funcs[aQuality][aIn.depth()];

		assert( tFun );
		if( tFun )
		{
			Image tTemp;
			const bool tUseTemp = (&aIn == &aCached);
			Image &tDst = tUseTemp?tTemp:aCached;
			//Image tResult(width, height, aIn.depth());
			tDst.create(width, height, aIn.depth());
			tFun(aIn, tDst, width, height);

			if( tUseTemp )
				img::copy(tTemp, aCached);

			return aCached;
		}

		return Image::emptyImage;
	}

	Image scale(const Image &aIn, ScaleQuality aQuality, int width, int height)
	{
		Image tResult;
		return scale(aIn, tResult, aQuality, width, height);
	}
}
