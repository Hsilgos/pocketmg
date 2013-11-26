#pragma once

#include "defines.h"

namespace img
{
	class Image;

	enum ScaleQuality
	{
		FastScaling,
		MiddleScaling,
		HighScaling
	};

	
	inline int proportionalHeight(int aNewWidth, int aOldWidth, int aOldHeight)
	{
		return aNewWidth*aOldHeight/aOldWidth;
	}

	inline int proportionalWidth(int aNewHeight, int aOldWidth, int aOldHeight)
	{
		return aNewHeight*aOldWidth/aOldHeight;
	}

	inline int proportionalHeight(int aNewWidth, const utils::Size &aSize)
	{
		return proportionalHeight(aNewWidth, aSize.width, aSize.height);
	}

	inline int proportionalWidth(int aNewHeight, const utils::Size &aSize)
	{
		return proportionalWidth(aNewHeight, aSize.width, aSize.height);
	}


	Image scale(const Image &aIn, Image &aCached, ScaleQuality aQuality, int width, int height);
	Image scale(const Image &aIn, ScaleQuality aQuality, int width, int height);
}

