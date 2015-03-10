#pragma once

#include "defines.h"

namespace img {
class Image;

enum ScaleQuality {
  FastScaling,
  MiddleScaling,
  HighScaling
};


inline int proportionalHeight(int new_width, int old_width, int old_height) {
  return new_width*old_height / old_width;
}

inline int proportionalWidth(int new_height, int old_width, int old_height) {
  return new_height*old_width / old_height;
}

inline int proportionalHeight(int new_width, const utils::Size &size) {
  return proportionalHeight(new_width, size.width, size.height);
}

inline int proportionalWidth(int new_height, const utils::Size &size) {
  return proportionalWidth(new_height, size.width, size.height);
}


Image scale(const Image &in, Image &cached, ScaleQuality quality, int width, int height);
Image scale(const Image &in, ScaleQuality quality, int width, int height);
}

