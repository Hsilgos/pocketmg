#pragma once

#include <string>
#include <vector>

namespace fs
{
	class FilePath
	{
		std::string mPath;
		bool mIsFile;

		typedef std::vector<std::string> StringVector;
		std::vector<std::string> mEntries;

		size_t mFirstLevel;

		void parse();
		void rebuildPath();

		bool doCompare(const FilePath &aOther, int aLevel, bool aCmpFromBack) const;

		int correctLevel(int aLevel) const;
	public:
		enum
		{
			LastLevel = -1
		};

		FilePath();
		FilePath(const std::string &aPath, bool aIsFile);

		bool equals(const FilePath &aOther, bool aCmpFromBack = false) const;

		bool operator == (const FilePath &aSecond) const;
		bool operator != (const FilePath &aSecond) const;

		bool operator < (const FilePath &aSecond) const;

		void set(const std::string &aPath, bool aIsFile);
		void clear();

		// returns maximum level
		size_t getLevel() const;
		size_t getDirLevel() const;

		// sets level which will be first fo algorythms.
		// For example for 'sorting'
		void setFirstLevel(size_t aFirstLevel);

		// returns 'true' if entry at level aLevel is directory name
		bool isDirectory(int aLevel = LastLevel) const;

		// return name of entry at level aLevel
		std::string getName(int aLevel = LastLevel) const;

		// returns whole path
		const std::string &getPath() const;

		// returns extension of file, i.e. 'txt' for readme.txt
		// returns empty string if it's directory or file doesn't have extension
		std::string getExtension() const;

		std::string getFileName() const;
		const std::string &getLastEntry() const;
		
		void addFolder(const std::string &aName);
		void removeLastEntry();
		
		// .filename
		bool isHidden() const;
		
		// ..
		bool isBack() const;

		bool empty() const;

		// 
		bool startsWith(const FilePath &aOther, bool aCmpFromBack = false) const;
		bool startsWith(const FilePath &aOther, size_t aLevel, bool aCmpFromBack = false) const;

		void setFile(const std::string &aName);

		FilePath copy(int aLevel = LastLevel) const;

		static void setGlobalSelector(const char aSelector);
	};

	bool HaveSameDirectory(const FilePath &aFirst, const FilePath &aSecond);
	FilePath CopyDirectory(const FilePath &aPath);
	FilePath FindCommonPath(const FilePath &aFirst, const FilePath &aSecond);

	std::ostream& operator<< (std::ostream& aStream, const fs::FilePath& aPath);
	//std::istream& operator>> (std::istream& aStream, ByteArray& aArray);
}

