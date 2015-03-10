#pragma once


namespace img
{
	class Image;

	enum RotateAngle
	{
		Angle_90,
		Angle_180,
		Angle_270
	};
	// rotates clockwise
	Image rotate(const Image &src, Image &cached, RotateAngle angle);
	Image rotate(const Image &src, RotateAngle angle);
}

