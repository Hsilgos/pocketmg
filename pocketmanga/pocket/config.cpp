#include "config.h"


namespace pocket
{

Config::Config(const std::string &aPath)
	:mConfig( OpenConfig(aPath.c_str(), 0) )
{

}

Config::~Config()
{
	CloseConfig(mConfig);
}

int Config::readInt(const std::string &aName, int aDef)
{
	return ReadInt(mConfig, aName.c_str(), aDef);
}
std::string Config::readString(const std::string &aName, const std::string &aDef)
{
	return ReadString(mConfig, aName.c_str(), aDef.c_str());
}

void Config::write(const std::string &aName, int aValue)
{
	WriteInt(mConfig, aName.c_str(), aValue);
}
void Config::write(const std::string &aName, const std::string &aValue)
{
	WriteString(mConfig, aName.c_str(), aValue.c_str());
}

}
