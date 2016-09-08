#include "imgDecoder.h"


namespace img {
IDecoder::IDecoder() : align_(1), decode_mode_(DecodeAsIs) {
}
 
IDecoder::~IDecoder() {
}

void IDecoder::setAlignment(size_t align) {
  align_ = align;
}

size_t IDecoder::getAlignment() const {
  return align_;
}

void IDecoder::setDecodeMode(DecodeMode mode) {
  decode_mode_ = mode;
}

DecodeMode IDecoder::getDecodeMode() const {
  return decode_mode_;
}

}
