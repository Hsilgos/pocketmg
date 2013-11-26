#include "testImageDecoder.h"

#include "imgDecoderFactory.h"
#include "byteArray.h"
#include "image.h"

namespace
{
	static const std::string TestImageHeader = "POSCKETMANGATESTIMAGE:";
	//template<class T, int N>
	//char (&ArraySize(T (&)[N]))[N];
}


namespace test
{
	const std::string &TestImgHeader()
	{
		return TestImageHeader;
	}

	TestImageDecoder::TestImageDecoder()
	{
		img::DecoderFactory::getInstance().registerDecoder(this);
	}

	TestImageDecoder::~TestImageDecoder()
	{
		img::DecoderFactory::getInstance().unregisterDecoder(getExts()[0]);
	}

	std::vector<std::string> TestImageDecoder::getExts() const
	{
		std::vector<std::string> tResult;
		tResult.push_back("testimg");
		return tResult;
	}

	// Format: 
	// POSCKETMANGATESTIMAGE:some data
	bool TestImageDecoder::decode(const tools::ByteArray &aEncoded, img::Image &aDecoded)
	{
		static const size_t header_size = TestImageHeader.size();
		if( aEncoded.getSize() < header_size )
			return false;

		if( !compare(aEncoded, 0, tools::toByteArray(TestImageHeader), 0, header_size) )
			return false;

		const size_t rest_len = aEncoded.getSize() - header_size;
		if( rest_len == 0 )
			return false;

		aDecoded.create(rest_len, 1, 1);
		memcpy(aDecoded.data(), aEncoded.getData() + header_size, rest_len);

		return true;
	}

	tools::ByteArray CreateTestImage(const std::string &aData)
	{
		return 
			aData.empty() ?  tools::ByteArray::empty : tools::toByteArray(TestImageHeader + aData);
	}

	std::string DataFromTestImage(const img::Image &aImage)
	{
		return std::string(aImage.data(), aImage.data() + aImage.width());
	}
}
