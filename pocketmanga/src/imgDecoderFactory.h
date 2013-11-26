#pragma once

#include "defines.h"
#include "singleton.h"

#include <map>
#include <list>
#include <string>

namespace tools
{
	class ByteArray;
}

namespace img
{
	class Image;
}

namespace img
{
	class IDecoder;

	class DecoderFactory: public utils::SingletonStatic<DecoderFactory>
	{
		typedef std::map<std::string, IDecoder *>	DecodersMap;
		typedef std::list<IDecoder *>				DecodersList;

		DecodersMap		mDecodersMap;
		DecodersList	mDecodersList;
	public:
		void registerDecoder(img::IDecoder *aDecoder);
		void unregisterDecoder(const std::string &aExt);
		bool decode(const std::string &aExt, const tools::ByteArray &aData, img::Image &aImage) const;
	};


#define AUTO_REGISTER_DECODER( decoder_type )																	\
	namespace																									\
	{																											\
		img::IDecoder*  TOKEN_JOIN(create_decoder, __LINE__)()													\
		{																										\
				return new decoder_type;																		\
		}																										\
																												\
		struct TOKEN_JOIN(FactoryRegistrar, __LINE__)															\
		{																										\
			TOKEN_JOIN(FactoryRegistrar, __LINE__)()															\
			{																									\
				img::DecoderFactory::getInstance().registerDecoder( TOKEN_JOIN(create_decoder, __LINE__)() );	\
			}																									\
		};																										\
		static TOKEN_JOIN(FactoryRegistrar, __LINE__) TOKEN_JOIN(__global_factory_registrar__, __LINE__ );		\
	}

}

