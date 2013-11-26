#pragma once

#include <boost/assert.hpp>
#include <string>

#include "debugUtils.h"

namespace utils
{
	enum TargetMachnie
	{
		x86,
		x64
	};

	inline TargetMachnie getTargetMachine()
	{
		return sizeof(void*) == 8?x64:x86;
	}

	void ideOutput(const char *, bool aEndLine = true);
	void ideOutput(const wchar_t *, bool aEndLine = true);

	enum ConsoleColor
	{
		CCBlue		= 1,
		CCGreen		= 2,
		CCRed		= 4,
		CCYellow	= CCRed|CCGreen,
		CCPink		= CCRed|CCBlue,
		CCLightGreen= CCGreen|CCBlue,
		CCWhite		= CCBlue|CCGreen|CCRed
	};

	void setConsoleColor(unsigned int aColor);

	void doBlockLeakDetect(bool aBlock);
	inline void enableLeakDetecting()
	{
		if( isDebugging() )
			doBlockLeakDetect(true);
	}
	inline void disableLeakDetecting()
	{
		if( isDebugging() )
			doBlockLeakDetect(true);
	}
	class BlockLeakDetect
	{
		void doBlock(bool aBlock);
	public:
		BlockLeakDetect()
		{
			enableLeakDetecting();
		}

		~BlockLeakDetect()
		{
			disableLeakDetecting();
		}
	};

	std::string lastErrorDescription();
	std::string lastErrorDescription(int aErrorCode);
}

#define ASSERT_RET1( cond ) \
	if( !(cond) )\
	{\
		BOOST_ASSERT(false);\
		return;\
	}

#define ASSERT_RET2( cond, ret ) \
	if( !(cond) )\
	{\
		BOOST_ASSERT(false);\
		return (ret);\
	}

#define ASSERT_CONTINUE( cond ) \
	if( !(cond) )\
	{\
		BOOST_ASSERT(false);\
		continue;\
	}