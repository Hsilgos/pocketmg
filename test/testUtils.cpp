#include "testUtils.h"

#include <iostream>

#ifdef WIN32
# include <Windows.h>
# include <crtdbg.h>
# include <sstream>
#endif

#include "defines.h"

namespace utils {
#ifdef WIN32
void ideOutput(const char* output, bool end_line) {
  if (isDebugging()) {
    OutputDebugStringA(output);
    if (end_line)
      OutputDebugStringA("\n");
  }
}

void ideOutput(const wchar_t* output, bool end_line) {
  if (isDebugging()) {
    OutputDebugStringW(output);
    if (end_line)
      OutputDebugStringW(L"\n");
  }
}

void doBlockLeakDetect(bool block) {
  int flags = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);

  if (block) // Disable allocation tracking
    flags &= ~_CRTDBG_ALLOC_MEM_DF;
  else  // Enable allocation tracking
    flags |= _CRTDBG_ALLOC_MEM_DF;

  _CrtSetDbgFlag(flags);
}

void setConsoleColor(unsigned int console_color) {
  unsigned int color = console_color;

  if (color != CCWhite)
    color |= FOREGROUND_INTENSITY;  // Bold

  HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
  SetConsoleTextAttribute(hStdout, color);
}

void lastErrorDescription(std::string& result, int error_code) {
  std::stringstream stream;
  stream << "(code:" << error_code << ")";

  char* buffer = 0;
  FormatMessageA(
    FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER,
    0,
    error_code,
    MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US),
    (LPSTR)&buffer,
    0,
    0);
  if (buffer) {
    stream << " " << buffer;
    LocalFree(buffer);
  }

  result = stream.str();
}

std::string lastErrorDescription() {
  std::string result;
  lastErrorDescription(result, GetLastError());
  return result;
}

std::string lastErrorDescription(int error_code) {
  std::string result;
  lastErrorDescription(result, error_code);
  return result;
}

#else
void ideOutput(const char*, bool) {}
void ideOutput(const wchar_t*, bool) {}
void doBlockLeakDetect(bool) {}

int toColorCode(unsigned int color) {
  switch (color) {
  case CCRed:
    return 31;
  case CCGreen:
    return 32;
  case CCBlue:
    return 34;
  case CCRed | CCGreen:
    return 33;
  case CCRed | CCBlue:
    return 35;
  case CCGreen | CCBlue:
    return 36;
  case CCWhite:
    return 37;
  }

  return 0;
}

void setConsoleColor(unsigned int color) {
  if (CCWhite == color) {
    // Default console color, instead of white (it can be black, green, etc)
    std::cout << "\033[0m";
  } else {
    const int color_code = toColorCode(color);

    // No bold
    //std::cout <<  "\033[0;"<< color_code << "m";

    // Bold
    std::cout <<  "\033[1;" << color_code << "m";
  }
}

std::string lastErrorDescription() {
  return utils::EmptyString;
}

std::string lastErrorDescription(int error_code) {
  return utils::EmptyString;
}
#endif
}
