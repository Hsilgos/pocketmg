#include "testImageDecoder.h"

#include "imgDecoderFactory.h"
#include "byteArray.h"
#include "image.h"

namespace {
static const std::string TestImageHeader = "POSCKETMANGATESTIMAGE:";
//template<class T, int N>
//char (&ArraySize(T (&)[N]))[N];
}


namespace test {
const std::string& TestImgHeader() {
  return TestImageHeader;
}

TestImageDecoder::TestImageDecoder() {
  img::DecoderFactory::getInstance().registerDecoder(this);
}

TestImageDecoder::~TestImageDecoder() {
  img::DecoderFactory::getInstance().unregisterDecoder(getExts()[0]);
}

std::vector<std::string> TestImageDecoder::getExts() const {
  std::vector<std::string> result;
  result.push_back("testimg");
  return result;
}

// Format:
// POSCKETMANGATESTIMAGE:some data
bool TestImageDecoder::decode(const tools::ByteArray& encoded, img::Image& decoded) {
  static const size_t header_size = TestImageHeader.size();
  if (encoded.getSize() < header_size)
    return false;

  if (!compare(encoded, 0, tools::toByteArray(TestImageHeader), 0, header_size))
    return false;

  const size_t rest_len = encoded.getSize() - header_size;
  if (rest_len == 0)
    return false;

  decoded.create(rest_len, 1, 1);
  memcpy(decoded.data(), encoded.getData() + header_size, rest_len);

  return true;
}

tools::ByteArray CreateTestImage(const std::string& data) {
  return data.empty() ?  tools::ByteArray::empty : tools::toByteArray(TestImageHeader + data);
}

std::string DataFromTestImage(const img::Image& image) {
  return std::string(image.data(), image.data() + image.width());
}
}
