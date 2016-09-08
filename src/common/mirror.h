#pragma once

namespace img {
class Image;

enum MirrorType {
  UpsideDown,
  LeftToRight
};

void mirror(const img::Image& src, img::Image& dst, MirrorType type);
} // namespace img
