#pragma once

#include "imgDecoder.h"

namespace test {
class TestImageDecoder: public img::IDecoder {
public:
  TestImageDecoder();
  virtual ~TestImageDecoder();

  // img::IDecoder
  virtual std::vector<std::string> getExts() const;
  virtual bool decode(const tools::ByteArray &encoded, img::Image &decoded);
};

tools::ByteArray CreateTestImage(const std::string &data);
std::string DataFromTestImage(const img::Image &image);
const std::string &TestImgHeader();
}
