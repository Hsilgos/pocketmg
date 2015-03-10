#include "rotate.h"


#include "image.h"

#include <assert.h>


/*
0, 0  -----  10, 0       0, 0 ---------------100, 0
 |            |           |                    |
 |            |           |                    |
 |            |---------> |                    |
 |            |           |                    |
 |            |           |                    |
 |            |         0, 10 ---------------100, 10
 |            |
 |            |
0, 100-----  10, 100
*/

namespace
{
	typedef img::Image::SizeType SizeType;
}

namespace img
{
	void rotate90_1(const Image &src, Image &dst)
	{
		const SizeType orig_height	= src.height();
		const SizeType orig_width	= src.width();

		const SizeType new_height	= src.width();
		const SizeType new_width		= src.height();

		dst.create(new_width, new_height, src.depth());

		const unsigned char* src_data	= src.data();
		unsigned char* dst_data			= dst.data();

		for( SizeType y = 0; y < orig_height; ++y )
		{
			for( SizeType x = 0; x < orig_width; ++x )
			{
				SizeType pixel_src_ind = y * orig_width + x;//y * orig_width	+ x;
				SizeType pixel_dst_ind = x * new_width + new_width - y - 1;//x * new_width	+ new_width - y - 1;

				assert( pixel_src_ind < orig_height * orig_width);
				assert( pixel_dst_ind < new_height * new_width);

				dst_data[pixel_dst_ind] = src_data[pixel_src_ind];
				//RotatePixelFun(src_data, dst_data, pixel_src_ind, pixel_dst_ind);
			}
		}
	}

	void rotate90_3(const Image &src, Image &dst)
	{
		const SizeType orig_height	= src.height();
		const SizeType orig_width	= src.width();

		const SizeType new_height	= src.width();
		const SizeType new_width		= src.height();

		dst.create(new_width, new_height, src.depth());

		const unsigned char* src_data	= src.data();
		unsigned char* dst_data			= dst.data();

		for( SizeType y = 0; y < orig_height; ++y )
		{
			for( SizeType x = 0; x < orig_width; ++x )
			{
				SizeType pixel_src_ind = y * orig_width + x;//y * orig_width	+ x;
				SizeType pixel_dst_ind = x * new_width + new_width - y - 1;//x * new_width	+ new_width - y - 1;

				assert( pixel_src_ind < orig_height * orig_width);
				assert( pixel_dst_ind < new_height * new_width);

				dst_data[pixel_dst_ind * 3 + 0] = src_data[pixel_src_ind * 3 + 0];
				dst_data[pixel_dst_ind * 3 + 1] = src_data[pixel_src_ind * 3 + 1];
				dst_data[pixel_dst_ind * 3 + 2] = src_data[pixel_src_ind * 3 + 2];
			}
		}
	}


	void rotate180_1(const Image &src, Image &dst)
	{
		const SizeType orig_height	= src.height();
		const SizeType orig_width	= src.width();

		const SizeType new_height	= src.height();
		const SizeType new_width		= src.width();

		dst.create(new_width, new_height, src.depth());

		const unsigned char* src_data	= src.data();
		unsigned char* dst_data			= dst.data();

		for( SizeType y = 0; y < orig_height; ++y )
		{
			const SizeType destinationY = (new_height - 1 - y) * new_width;
			
			for( SizeType x = 0; x < orig_width; ++x )
			{
				const SizeType destinationX = new_width - 1 - x;

				SizeType pixel_src_ind = y * orig_width + x;
				SizeType pixel_dst_ind = destinationX + destinationY;

				assert( pixel_src_ind < orig_height * orig_width);
				assert( pixel_dst_ind < new_height * new_width);

				dst_data[pixel_dst_ind] = src_data[pixel_src_ind];
			}
		}
	}

	void rotate180_3(const Image &src, Image &dst)
	{
		const SizeType orig_height	= src.height();
		const SizeType orig_width	= src.width();

		const SizeType new_height	= src.height();
		const SizeType new_width		= src.width();

		dst.create(new_width, new_height, src.depth());

		const unsigned char* src_data	= src.data();
		unsigned char* dst_data			= dst.data();

		for( SizeType y = 0; y < orig_height; ++y )
		{
			const SizeType destinationY = (new_height - 1 - y) * new_width;

			for( SizeType x = 0; x < orig_width; ++x )
			{
				const SizeType destinationX = new_width - 1 - x;

				SizeType pixel_src_ind = y * orig_width + x;
				SizeType pixel_dst_ind = destinationX + destinationY;

				assert( pixel_src_ind < orig_height * orig_width);
				assert( pixel_dst_ind < new_height * new_width);

				dst_data[pixel_dst_ind * 3 + 0] = src_data[pixel_src_ind * 3 + 0];
				dst_data[pixel_dst_ind * 3 + 1] = src_data[pixel_src_ind * 3 + 1];
				dst_data[pixel_dst_ind * 3 + 2] = src_data[pixel_src_ind * 3 + 2];
			}
		}
	}

	void rotate270_1(const Image &src, Image &dst)
	{
		const SizeType orig_height	= src.height();
		const SizeType orig_width	= src.width();

		const SizeType new_height	= src.width();
		const SizeType new_width	= src.height();

		dst.create(new_width, new_height, src.depth());

		const unsigned char* src_data	= src.data();
		unsigned char* dst_data			= dst.data();

		for( SizeType y = 0; y < orig_height; ++y )
		{
			const SizeType destinationX = y;

			for( SizeType x = 0; x < orig_width; ++x )
			{
				const SizeType destinationY = new_height - 1 - x;

				SizeType pixel_src_ind = y * orig_width + x;
				SizeType pixel_dst_ind = destinationX + destinationY*new_width;

				assert( pixel_src_ind < orig_height * orig_width);
				assert( pixel_dst_ind < new_height * new_width);

				dst_data[pixel_dst_ind] = src_data[pixel_src_ind];
			}
		}
	}

	void rotate270_3(const Image &src, Image &dst)
	{
		const SizeType orig_height	= src.height();
		const SizeType orig_width	= src.width();

		const SizeType new_height	= src.width();
		const SizeType new_width		= src.height();

		dst.create(new_width, new_height, src.depth());

		const unsigned char* src_data	= src.data();
		unsigned char* dst_data			= dst.data();

		for( SizeType y = 0; y < orig_height; ++y )
		{
			const SizeType destinationX = y;

			for( SizeType x = 0; x < orig_width; ++x )
			{
				const SizeType destinationY = new_height - 1 - x;

				SizeType pixel_src_ind = y * orig_width + x;
				SizeType pixel_dst_ind = destinationX + destinationY*new_width;

				assert( pixel_src_ind < orig_height * orig_width);
				assert( pixel_dst_ind < new_height * new_width);

				dst_data[pixel_dst_ind * 3 + 0] = src_data[pixel_src_ind * 3 + 0];
				dst_data[pixel_dst_ind * 3 + 1] = src_data[pixel_src_ind * 3 + 1];
				dst_data[pixel_dst_ind * 3 + 2] = src_data[pixel_src_ind * 3 + 2];
			}
		}
	}

	Image rotate(const Image &src, Image &cached, RotateAngle angle)
	{
		Image temp;
		const bool use_temp = (&src == &cached);
		Image &dst = use_temp?temp:cached;
		//Image &dst = cached;

		switch(angle)
		{
		case Angle_90:
			if( 1 == src.depth() )
				rotate90_1(src, dst);
			else
				rotate90_3(src, dst);
			break;
		case Angle_180:
			if( 1 == src.depth() ) 
				rotate180_1(src, dst);
			else
				rotate180_3(src, dst);
			break;
		case Angle_270:
			if( 1 == src.depth() )
				rotate270_1(src, dst);
			else
				rotate270_3(src, dst);
			break;
		}

		if( use_temp )
			img::copy(temp, cached);

		return cached;
	}

	Image rotate(const Image &src, RotateAngle angle)
	{
		Image result;
		return rotate(src, result, angle);
	}


}

