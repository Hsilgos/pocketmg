#include "config.h"


namespace pocket
{

Config::Config(const std::string &path)
	:config_( OpenConfig(path.c_str(), 0) )
{

}

Config::~Config()
{
	CloseConfig(config_);
}

int Config::readInt(const std::string &name, int def)
{
	return ReadInt(config_, name.c_str(), def);
}
std::string Config::readString(const std::string &name, const std::string &def)
{
	return ReadString(config_, name.c_str(), def.c_str());
}

void Config::write(const std::string &name, int value)
{
	WriteInt(config_, name.c_str(), value);
}
void Config::write(const std::string &name, const std::string &value)
{
	WriteString(config_, name.c_str(), value.c_str());
}

}
