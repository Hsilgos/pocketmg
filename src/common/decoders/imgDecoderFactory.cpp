#include "imgDecoderFactory.h"

#include <vector>

#include "common/image.h"
#include "common/decoders/imgDecoder.h"

namespace img {
DecoderFactory::DecoderFactory() : align_(1), decode_mode_(DecodeAsIs) {
}

void DecoderFactory::registerDecoder(img::IDecoder* decoder) {
  if (decoder) {
    std::vector<std::string> exts = decoder->getExts();
    std::vector<std::string>::const_iterator it = exts.begin(), itEnd = exts.end();
    for (; it != itEnd; ++it) {
      decoders_map_[*it] = decoder;
    }

    decoders_list_.push_back(decoder);
    decoder->setAlignment(align_);
    decoder->setDecodeMode(decode_mode_);
  }
}

void DecoderFactory::setAlignment(size_t align) {
  align_ = align;
  DecodersMap::const_iterator it = decoders_map_.begin(), itEnd = decoders_map_.end();
  for (; it != itEnd; ++it) {
    it->second->setAlignment(align_);
  }
}

void DecoderFactory::setDecodeMode(DecodeMode mode) {
  decode_mode_ = mode;
  DecodersMap::const_iterator it = decoders_map_.begin(), itEnd = decoders_map_.end();
  for (; it != itEnd; ++it) {
    it->second->setDecodeMode(decode_mode_);
  }
}

void DecoderFactory::unregisterDecoder(const std::string& ext) {
  decoders_map_.erase(ext);
}

bool DecoderFactory::decode(const std::string& ext, const tools::ByteArray& data, img::Image& image) const {
  img::IDecoder* found = 0;

  // search by extension first...
  if (!ext.empty()) {
    DecodersMap::const_iterator it = decoders_map_.find(ext);
    if (it != decoders_map_.end())
      found = it->second;
  }

  if (found && found->decode(data, image))
    return true;

  DecodersList::const_iterator it = decoders_list_.begin(), itEnd = decoders_list_.end();
  for (; it != itEnd; ++it) {
    // exclude decoder which we use on last step
    if (*it != found && (*it)->decode(data, image))
      return true;
  }

  return false;
}
}
