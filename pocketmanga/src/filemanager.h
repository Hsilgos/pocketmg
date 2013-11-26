#pragma once

#include <string>

#include <vector>

namespace tools
{
	class ByteArray;
}

namespace fs
{
	class FilePath;

	class Comparator
	{
		virtual bool compareLevel(const std::string &aName1, const std::string &aName2, bool &aResult) const = 0;
	public:
		bool operator ()(const fs::FilePath &aFirst, const fs::FilePath &aSecond) const;
		virtual ~Comparator();
	};

	class WordNumberOrder: public Comparator
	{
		virtual bool compareLevel(const std::string &aName1, const std::string &aName2, bool &aResult) const;
	};

	class NumberOrder: public Comparator
	{
		virtual bool compareLevel(const std::string &aName1, const std::string &aName2, bool &aResult) const;
	};

	class IFileManager
	{
	public:
		enum EntryTypes
		{
		  File			= 1,
		  Directory		= 2,
		  FileAndDirectory	= File|Directory
		};
		
		//virtual std::vector<fs::FilePath> getFileList(const std::string &aRoot, EntryTypes aEntries, bool aRecursive) = 0;
		virtual std::vector<fs::FilePath> getFileList(const fs::FilePath &aRoot, EntryTypes aEntries, bool aRecursive) = 0;
		virtual tools::ByteArray readFile(const fs::FilePath &aFile, size_t aMaxSize);

		virtual ~IFileManager();

		static IFileManager *create();
	};
}
