#pragma once

#include <vector>
#include <string>

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
  void setDesiredBytePerPixel(unsigned int byte_per_pixel);
  size_t getAlignment() const;
  unsigned int getDesiredBytePerPixel() const;

private:
  size_t align_;
  unsigned int byte_per_pixel_;
};
}
