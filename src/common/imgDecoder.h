#pragma once

#include <vector>
#include <string>

#include "decodeMode.h"

namespace tools {
class ByteArray;
}

namespace img {
class Image;

class IDecoder {
public:
  IDecoder();
  virtual ~IDecoder();
  // returns preferable extensions
  virtual std::vector<std::string> getExts() const = 0;
  virtual bool decode(const tools::ByteArray& encoded, img::Image& decoded) = 0;

  void setAlignment(size_t align);
  size_t getAlignment() const;

  void setDecodeMode(DecodeMode mode);
  DecodeMode getDecodeMode() const;

private:
  size_t align_;
  DecodeMode decode_mode_;
};
}
