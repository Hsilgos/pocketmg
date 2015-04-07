#include "imgDecoder.h"


namespace img {
IDecoder::IDecoder() : align_(1), byte_per_pixel_(0) {
}
 
IDecoder::~IDecoder() {
}

void IDecoder::setAlignment(size_t align) {
  align_ = align;
}

void IDecoder::setDesiredBytePerPixel(unsigned int byte_per_pixel) {
  byte_per_pixel_ = byte_per_pixel;
}
size_t IDecoder::getAlignment() const {
  return align_;
}

unsigned int IDecoder::getDesiredBytePerPixel() const {
  return byte_per_pixel_;
}
}
