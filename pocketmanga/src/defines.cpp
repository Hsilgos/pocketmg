#include "defines.h"

#include <algorithm>

namespace utils {
const std::string EmptyString;

Rect::Rect(int ax, int ay, SizeType width, SizeType height)
  :x(ax), y(ay), width(width), height(height) {
}

Rect restrictBy(const Rect &src, const Rect &restricter) {
  int src_x1 = src.x;
  int src_y1 = src.y;
  int src_x2 = src.x + src.width;
  int src_y2 = src.y + src.height;

  int restr_x1 = restricter.x;
  int restr_y1 = restricter.y;
  int restr_x2 = restricter.x + restricter.width;
  int restr_y2 = restricter.y + restricter.height;

  Rect result;
  result.x  = std::max(src_x1, restr_x1);
  result.y  = std::max(src_y1, restr_y1);
  result.width = std::max<int>(0, std::min(src_x2, restr_x2) - result.x);
  result.height = std::max<int>(0, std::min(src_y2, restr_y2) - result.y);

  return result;
}

Size::Size(SizeType width, SizeType height)
  :width(width), height(height) {
}
}
