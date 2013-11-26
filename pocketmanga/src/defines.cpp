#include "defines.h"

namespace utils
{
	const std::string EmptyString;

	Rect::Rect(int ax, int ay, SizeType aWidth, SizeType aHeight)
		:x(ax), y(ay), width(aWidth), height(aHeight)
	{
	}

	Rect restrictBy(const Rect &aSrc, const Rect &aRestricter)
	{
		int tSrcX1 = aSrc.x;
		int tSrcY1 = aSrc.y;
		int tSrcX2 = aSrc.x + aSrc.width;
		int tSrcY2 = aSrc.y + aSrc.height;

		int tRestrX1 = aRestricter.x;
		int tRestrY1 = aRestricter.y;
		int tRestrX2 = aRestricter.x + aRestricter.width;
		int tRestrY2 = aRestricter.y + aRestricter.height;

		Rect tResult;
		tResult.x		= std::max(tSrcX1, tRestrX1);
		tResult.y		= std::max(tSrcY1, tRestrY1);
		tResult.width	= std::max<int>(0, std::min(tSrcX2, tRestrX2) - tResult.x);
		tResult.height	= std::max<int>(0, std::min(tSrcY2, tRestrY2) - tResult.y);

		return tResult;
	}

	Size::Size(SizeType aWidth, SizeType aHeight)
		:width(aWidth), height(aHeight)
	{
	}
}
