#pragma once

#include <string>

namespace utils
{
	extern const std::string EmptyString;

#define TOKEN_JOIN(X, Y)		TOKEN_DO_JOIN(X, Y)
#define TOKEN_DO_JOIN(X, Y)		TOKEN_DO_JOIN2(X, Y)
#define TOKEN_DO_JOIN2(X, Y)	X##Y


	struct Rect
	{
		int x;
		int y;

		typedef unsigned int SizeType;
		SizeType width;
		SizeType height;

		Rect(
			int ax		= 0,
			int ay		= 0,
			SizeType aWidth		= 0,
			SizeType aHeight	= 0);
	};

	Rect restrictBy(const Rect &aSrc, const Rect &aRestricter);

	struct Size
	{
		typedef unsigned int SizeType;

		SizeType width;
		SizeType height;

		Size(
			SizeType aWidth	= 0,
			SizeType aHeight	= 0);
	};
}
