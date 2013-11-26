#include "testUtils.h"
//#include "strdef.h"

#include <iostream>

#ifdef WIN32
# include <Windows.h>
# include <crtdbg.h>
# include <sstream>
#endif

namespace utils
{
#ifdef WIN32
	void ideOutput(const char *aOutput, bool aEndLine)
	{
		if( isDebugging() )
		{
			OutputDebugStringA(aOutput);
			if( aEndLine )
				OutputDebugStringA("\n");
		}
	}

	void ideOutput(const wchar_t *aOutput, bool aEndLine)
	{
		if( isDebugging() )
		{
			OutputDebugStringW(aOutput);
			if( aEndLine )
				OutputDebugStringW(L"\n");
		}
	}

	void doBlockLeakDetect(bool aBlock)
	{
		int flags = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);

		if(aBlock)	// Disable allocation tracking
			flags &= ~_CRTDBG_ALLOC_MEM_DF;
		else		// Enable allocation tracking
			flags |= _CRTDBG_ALLOC_MEM_DF;

		_CrtSetDbgFlag(flags);
	}

	void setConsoleColor(unsigned int aColor)
	{
		unsigned int tColor = aColor;

		if( tColor != CCWhite )
			tColor |= FOREGROUND_INTENSITY;// Bold

		HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
		SetConsoleTextAttribute(hStdout, tColor);
	}

	void lastErrorDescription(std::string &aResult, int aErrorCode)
	{
		std::stringstream tStream;
		tStream << "(code:" << aErrorCode << ")";

		char *tBuffer = 0;
		FormatMessageA(
			FORMAT_MESSAGE_FROM_SYSTEM|FORMAT_MESSAGE_ALLOCATE_BUFFER,
			0,
			aErrorCode,
			MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US),
			(LPSTR)&tBuffer,
			0 ,
			0 );
		if( tBuffer )
		{
			tStream << " " << tBuffer;
			LocalFree(tBuffer);
		}

		aResult = tStream.str();
	}

	std::string lastErrorDescription()
	{
		std::string tResult;
		lastErrorDescription(tResult, GetLastError());
		return tResult;
	}

	std::string lastErrorDescription(int aErrorCode)
	{
		std::string tResult;
		lastErrorDescription(tResult, aErrorCode);
		return tResult;
	}

#else
	void ideOutput(const char *, bool) {}
	void ideOutput(const wchar_t *, bool){}
	void doBlockLeakDetect(bool){}

	int toColorCode(unsigned int aColor)
	{
		switch(aColor)
		{
		case CCRed:
			return 31;
		case CCGreen:
			return 32;
		case CCBlue:
			return 34;
		case CCRed|CCGreen:
			return 33;
		case CCRed|CCBlue:
			return 35;
		case CCGreen|CCBlue:
			return 36;
		case CCWhite:
			return 37;
		}

		return 0;
	}

	void setConsoleColor(unsigned int aColor)
	{
		if( CCWhite == aColor )
		{
			// Default console color, instead of white (it can be black, green, etc)
			std::cout << "\033[0m";
		}
		else
		{
			const int tColorCode = toColorCode(aColor);

			// No bold
			//std::cout <<  "\033[0;"<< tColorCode << "m";

			// Bold
			std::cout <<  "\033[1;"<< tColorCode << "m";
		}
	}

	std::string lastErrorDescription()
	{
		return utils::EmptyString;
	}

	std::string lastErrorDescription(int aErrorCode)
	{
		return utils::EmptyString;
	}
#endif
}
