#pragma once

#include "imgDecoder.h"

namespace test
{
	class TestImageDecoder: public img::IDecoder
	{
	public:
		TestImageDecoder();
		virtual ~TestImageDecoder();

		// img::IDecoder
		virtual std::vector<std::string> getExts() const;
		virtual bool decode(const tools::ByteArray &aEncoded, img::Image &aDecoded);
	};

	tools::ByteArray CreateTestImage(const std::string &aData);
	std::string DataFromTestImage(const img::Image &aImage);
	const std::string &TestImgHeader();
}
