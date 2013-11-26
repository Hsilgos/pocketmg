#include "filemanager.h"

#include <algorithm>
#include <fstream>

#include "filepath.h"

#include "byteArray.h"

#include "algorithms_ex.h"

namespace
{
	inline int fetchNumber(const std::string &aName, size_t &aPos)
	{
		if( !isdigit(aName[aPos]) )
			return 0;

		const std::string::size_type tNumEnd = aName.find_first_not_of("0123456789", aPos);
		const std::string tNumStr = aName.substr(aPos, tNumEnd - aPos);
		const int tNumber = atoi(tNumStr.c_str());
		aPos = (std::string::npos == tNumEnd) ? aName.size() : tNumEnd;

		return tNumber;
	}

	inline std::string getFileNameCI(const fs::FilePath &aFirst, int aLevel)
	{
		std::string tName = aFirst.getName(aLevel);
		std::transform(tName.begin(), tName.end(), tName.begin(), tolower);
		return tName;
	}
}

namespace fs
{	
	// build correct list of entries in each sublevel

	bool Comparator::operator ()(const fs::FilePath &aFirst, const fs::FilePath &aSecond) const
	{
		bool tResult = false;

		for( size_t tLevel = 0; tLevel < std::min(aFirst.getLevel(), aSecond.getLevel()); ++tLevel )
		{
			if(aFirst.isDirectory(tLevel) != aSecond.isDirectory(tLevel))
				return aFirst.isDirectory(tLevel);

			std::string tName1 = getFileNameCI(aFirst, tLevel);
			std::string tName2 = getFileNameCI(aSecond, tLevel);

			if( compareLevel(tName1, tName2, tResult) )
				return tResult;
			// words are equal, go to the next level
		}

		if( aFirst.getLevel() != aSecond.getLevel() )
			return aFirst.getLevel() < aSecond.getLevel();

		if( aFirst.isDirectory() != aSecond.isDirectory() )
			return aFirst.isDirectory();

		// both are equal
		return false;
	}

	Comparator::~Comparator()
	{
	}
	
	bool WordNumberOrder::compareLevel(const std::string &aName1, const std::string &aName2, bool &aResult) const
	{
		size_t i = 0, j = 0;
		while( i < aName1.size() && j < aName2.size() )
		{
			const bool tNumIsStarted = isdigit(aName1[i]) && isdigit(aName2[j]);
			if( tNumIsStarted || (aName1[i] != aName2[j]) )
			{
				if( tNumIsStarted )
				{
					// we met two numbers.
					// so now we have to compare numbers...
					const int tNumber1 = fetchNumber(aName1, i);
					const int tNumber2 = fetchNumber(aName2, j);

					if( tNumber1 != tNumber2 )
					{
						aResult = tNumber1 < tNumber2;
						return true;
					}

					continue;
				}
				else
				{
					aResult = aName1[i] < aName2[j];
					return true;
				}
			}

			++i; 
			++j;
		}

		if( aName1.size() != i || aName2.size() != j )
		{
			aResult = aName1.size() < aName2.size();
			return true;
		}

		return false;
	}

	
	bool NumberOrder::compareLevel(const std::string &aName1, const std::string &aName2, bool &aResult) const
	{
		static const char tNumbers[] = "1234567890";
		std::string::size_type tNum1Begin = aName1.find_first_of(tNumbers);
		std::string::size_type tNum2Begin = aName2.find_first_of(tNumbers);

		while( std::string::npos != tNum1Begin && std::string::npos != tNum2Begin )
		{
			const int tNumber1 = fetchNumber(aName1, tNum1Begin);
			const int tNumber2 = fetchNumber(aName2, tNum2Begin);

			if( tNumber1 != tNumber2 )
			{
				aResult = tNumber1 < tNumber2;
				return true;
			}

			if( std::string::npos != tNum1Begin )
				tNum1Begin = aName1.find_first_of(tNumbers, tNum1Begin);

			if( std::string::npos != tNum2Begin )
				tNum2Begin = aName2.find_first_of(tNumbers, tNum2Begin);
		}

		if( aName1 != aName2 )
		{
			aResult = aName1 < aName2;
			return true;
		}

		return false;
	}

	tools::ByteArray IFileManager::readFile(const fs::FilePath &aFilePath, size_t aMaxSize)
	{
		tools::ByteArray tData;

		std::ifstream tFile(aFilePath.getPath().c_str(), std::ios::binary);
		const size_t tSize = utils::get_stream_size(tFile);

		if( tSize < aMaxSize )
			tFile >> tData;

		return tData;
	}

	IFileManager::~IFileManager()
	{

	}
	

	//////////////////////////////////////////////////////////////////////////
}
