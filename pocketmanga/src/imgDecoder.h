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
  virtual ~IDecoder() {}
  // returns preferable extensions
  virtual std::vector<std::string> getExts() const = 0;
  virtual bool decode(const tools::ByteArray &encoded, img::Image &decoded) = 0;
};


}

