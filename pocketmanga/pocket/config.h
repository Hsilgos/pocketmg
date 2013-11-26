#pragma once


#include <string>

#include "defines.h"
#include "inkview.h"


namespace pocket
{
	class Config
	{
		iconfig *mConfig;
	public:
		Config(const std::string &aPath);

		~Config();

		int readInt(const std::string &aName, int aDef = 0);
		std::string readString(const std::string &aName, const std::string &aDef = utils::EmptyString);

		void write(const std::string &aName, int aValue);
		void write(const std::string &aName, const std::string &aValue);
	};
}

