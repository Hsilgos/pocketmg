#include "imgDecoderFactory.h"

#include <vector>

#include "image.h"
#include "imgDecoder.h"

namespace img {
void DecoderFactory::registerDecoder(img::IDecoder *decoder) {
  if( decoder ) {
    std::vector<std::string> exts = decoder->getExts();
    std::vector<std::string>::const_iterator it = exts.begin(), itEnd = exts.end();
    for( ; it != itEnd; ++it ) {
      decoders_map_[*it] = decoder;
    }

    decoders_list_.push_back(decoder);
  }
}

void DecoderFactory::unregisterDecoder(const std::string &ext) {
  decoders_map_.erase(ext);
}

bool DecoderFactory::decode(const std::string &ext, const tools::ByteArray &data, img::Image &image) const {
  img::IDecoder *found = 0;

  // search by extension first...
  if( !ext.empty() ) {
    DecodersMap::const_iterator it = decoders_map_.find(ext);
    if( it != decoders_map_.end() )
      found = it->second;
  }

  if( found && found->decode(data, image) )
    return true;

  DecodersList::const_iterator it = decoders_list_.begin(), itEnd = decoders_list_.end();
  for ( ; it != itEnd; ++it ) {
    // exclude decoder which we use on last step
    if( *it != found && (*it)->decode(data, image) )
      return true;
  }

  return false;
}
}
