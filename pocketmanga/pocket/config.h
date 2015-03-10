#pragma once


#include <string>

#include "defines.h"
#include "inkview.h"


namespace pocket
{
	class Config
	{
		iconfig *config_;
	public:
		Config(const std::string &path);

		~Config();

		int readInt(const std::string &name, int def = 0);
		std::string readString(const std::string &name, const std::string &def = utils::EmptyString);

		void write(const std::string &name, int value);
		void write(const std::string &name, const std::string &value);
	};
}

