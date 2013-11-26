#include "imgDecoderFactory.h"

#include <vector>

#include "image.h"
#include "imgDecoder.h"

namespace img
{
	void DecoderFactory::registerDecoder(img::IDecoder *aDecoder)
	{
		if( aDecoder )
		{
			std::vector<std::string> tExts = aDecoder->getExts();
			std::vector<std::string>::const_iterator it = tExts.begin(), itEnd = tExts.end();
			for( ; it != itEnd; ++it )
			{
				mDecodersMap[*it] = aDecoder;
			}

			mDecodersList.push_back(aDecoder);
		}
	}

	void DecoderFactory::unregisterDecoder(const std::string &aExt)
	{
		mDecodersMap.erase(aExt);
	}

	bool DecoderFactory::decode(const std::string &aExt, const tools::ByteArray &aData, img::Image &aImage) const
	{
		img::IDecoder *tFound = 0;

		// search by extension first...
		if( !aExt.empty() )
		{
			DecodersMap::const_iterator it = mDecodersMap.find(aExt);
			if( it != mDecodersMap.end() )
				tFound = it->second;
		}

		if( tFound && tFound->decode(aData, aImage) )
			return true;

		DecodersList::const_iterator it = mDecodersList.begin(), itEnd = mDecodersList.end();
		for ( ; it != itEnd; ++it )
		{
			// exclude decoder which we use on last step
			if( *it != tFound && (*it)->decode(aData, aImage) )
				return true;
		}

		return false;
	}
}
