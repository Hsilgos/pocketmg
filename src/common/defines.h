#pragma once

#include <string>

namespace utils {
extern const std::string EmptyString;

#define TOKEN_JOIN(X, Y)  TOKEN_DO_JOIN(X, Y)
#define TOKEN_DO_JOIN(X, Y)  TOKEN_DO_JOIN2(X, Y)
#define TOKEN_DO_JOIN2(X, Y) X ## Y


struct Rect {
  int x;
  int y;

  typedef unsigned int SizeType;
  SizeType width;
  SizeType height;

  Rect(int ax  = 0,
       int ay  = 0,
       SizeType width  = 0,
       SizeType height = 0);
};

Rect restrictBy(const Rect& src, const Rect& restricter);

struct Size {
  typedef unsigned int SizeType;

  SizeType width;
  SizeType height;

  Size(SizeType width = 0,
       SizeType height = 0);
};
}
