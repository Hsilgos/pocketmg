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
	Image rotate(const Image &aSrc, Image &aCached, RotateAngle aAngle);
	Image rotate(const Image &aSrc, RotateAngle aAngle);
}

