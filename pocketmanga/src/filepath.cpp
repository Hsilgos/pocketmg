#include "filepath.h"

#include "defines.h"

#include <ostream>

namespace
{
#ifdef WIN32
	static char DirSelector = '\\';
#else
	static char DirSelector = '/';
#endif
}

namespace fs
{
	void FilePath::setGlobalSelector(const char aSelector)
	{
		DirSelector = aSelector;
	}

	FilePath::FilePath()
		:mIsFile(false), mFirstLevel(0)
	{
	}

	FilePath::FilePath(const std::string &aPath, bool aIsFile)
		:mIsFile(false), mFirstLevel(0)
	{
		set(aPath, aIsFile);
	}

	void FilePath::set(const std::string &aPath, bool aIsFile)
	{
		mPath = aPath;
		mIsFile = aIsFile;

		parse();
	}

	void FilePath::clear()
	{
		mPath.clear();
		mIsFile = false;
		mEntries.clear();
		mFirstLevel = 0;
	}

	// returns maximum level
	size_t FilePath::getLevel() const
	{
		return mEntries.size() - mFirstLevel;
	}

	size_t FilePath::getDirLevel() const
	{
		return getLevel() - (isDirectory() ? 0 : 1);
	}

	void FilePath::setFirstLevel(size_t aFirstLevel)
	{
		mFirstLevel = aFirstLevel;
	}

	int FilePath::correctLevel(int aLevel) const
	{
		return mFirstLevel + aLevel;
	}

	// returns 'true' if entry at level aLevel is directory name
	bool FilePath::isDirectory(int aLevel) const
	{
		return 
			(LastLevel == correctLevel(aLevel) || ( getLevel() - 1) == correctLevel(aLevel) )?(!mIsFile):true;
	}

	// return name of entry at level aLevel
	std::string FilePath::getName(int aLevel) const
	{
		if( LastLevel == aLevel )
			return mEntries.empty() ? utils::EmptyString : mEntries.back();

		const int tCorrected = correctLevel(aLevel);
		const bool good_level = tCorrected < mEntries.size() && tCorrected >= 0;

		return good_level ? mEntries[tCorrected] : utils::EmptyString;
	}

	// returns whole path
	const std::string &FilePath::getPath() const
	{
		return mPath;
	}

	bool FilePath::empty() const
	{
		return mEntries.empty();
	}

	void FilePath::parse()
	{
		//std::string::size_type tPos = mPath.find_first_of("/\\");
		std::string::size_type tPos = 0;
		std::string::size_type tPrevPos = mPath.find_first_not_of("/\\");
		if( std::string::npos == tPrevPos )
			return;

		while( tPos != std::string::npos )
		{
			tPos = mPath.find_first_of("/\\", tPos + 1);

			std::string tEntry = mPath.substr(tPrevPos, tPos - tPrevPos);
			if ( !tEntry.empty() && tEntry != "." )
			{
				mEntries.push_back(tEntry);
			}

			tPrevPos = tPos + 1;
		}
	}

	std::string FilePath::getExtension() const
	{
		if( isDirectory() )
			return utils::EmptyString;

		std::string tFileName = getFileName();
		const std::string::size_type tPos = tFileName.find_last_of('.');
		if( std::string::npos == tPos )
			return utils::EmptyString;

		return tFileName.substr(tPos + 1);
	}

	std::string FilePath::getFileName() const
	{
		if( isDirectory() || mEntries.empty() )
			return utils::EmptyString;

		return mEntries.back();
	}
	
	const std::string &FilePath::getLastEntry() const
	{
		return mEntries.empty()?utils::EmptyString:mEntries.back();
	}
	
	void FilePath::rebuildPath()
	{
		std::vector<std::string>::const_iterator it = mEntries.begin(), itEnd = mEntries.end();
		
		mPath.clear();
		
		for(  ;it != itEnd; ++it)
		{
			mPath += DirSelector;
			mPath += *it;
		}
		
		if( mPath.empty() )
			mPath = DirSelector;
	}
	
	void FilePath::addFolder(const std::string &aName)
	{
		if( !aName.empty() )
		{
			mEntries.push_back(aName);
		
			if( mPath.empty() || mPath[mPath.size() - 1] != DirSelector )
				mPath += DirSelector;
		
			rebuildPath();//mPath += aName;
		}
	}
	
	void FilePath::removeLastEntry()
	{
		if( !mEntries.empty() )
			mEntries.erase(mEntries.end() - 1);

		mIsFile = false;
		
		rebuildPath();
	}
	
	bool FilePath::isHidden() const
	{
		const std::string &tEntry = getLastEntry();
		
		return !tEntry.empty() && tEntry[0] == '.';
	}	
	
	bool FilePath::isBack() const
	{	
		const std::string &tEntry = getLastEntry();
		
		return  isDirectory() && tEntry.size() == 2 && tEntry[0] == '.' && tEntry[1] == '.';
	}

	bool FilePath::startsWith(const FilePath &aOther, bool aCmpFromBack) const
	{
		if( mEntries.size() < aOther.mEntries.size() )
			return false;

		return doCompare(aOther, aOther.mEntries.size(), aCmpFromBack);
	}

	bool FilePath::startsWith(const FilePath &aOther, size_t aLevel, bool aCmpFromBack) const
	{
		if( mEntries.size() < aLevel || aOther.mEntries.size() < aLevel )
			return false;

		return doCompare(aOther, aLevel, aCmpFromBack);
	}

	bool FilePath::equals(const FilePath &aOther, bool aCmpFromBack) const
	{
		if( mEntries.size() != aOther.mEntries.size() )
			return false;

		return doCompare(aOther, mEntries.size(), aCmpFromBack);
	}

	bool FilePath::operator == (const FilePath &aOther) const
	{
		return equals(aOther, false);
	}

	bool FilePath::operator != (const FilePath &aSecond) const
	{
		return !(aSecond == *this);
	}

	bool FilePath::operator < (const FilePath &aOther) const
	{
		for( size_t i = 0 ; i < std::min(mEntries.size(), aOther.mEntries.size()) ; ++i )
		{
			if( mEntries[i] != aOther.mEntries[i] )
				return mEntries[i] < aOther.mEntries[i];
		}

		return mEntries.size() < aOther.mEntries.size();
	}

	bool FilePath::doCompare(const FilePath &aOther, int aLevel, bool aCmpFromBack) const
	{
		if( aCmpFromBack )
		{
			const size_t tRBegin1 = mEntries.size() - aLevel;
			const size_t tRBegin2 = aOther.mEntries.size() - aLevel;

			return std::equal(
						mEntries.rbegin() + tRBegin1,
						mEntries.rbegin() + tRBegin1 + aLevel,
						aOther.mEntries.rbegin() + tRBegin2);
		}
		
		return std::equal(
						mEntries.begin(),
						mEntries.begin() + aLevel,
						aOther.mEntries.begin());
	}

	void FilePath::setFile(const std::string &aName)
	{
		addFolder(aName);
		mIsFile = true;
	}

	FilePath FilePath::copy(int aLevel) const
	{
		FilePath tResult;
		if( LastLevel == aLevel || aLevel >= getLevel() )
		{
			tResult = *this;
		}
		else
		{
			tResult.mEntries.insert(
				tResult.mEntries.begin(),
				mEntries.begin(),
				mEntries.begin() + aLevel + 1);

			tResult.rebuildPath();
		}

		return tResult;
	}

	bool HaveSameDirectory(const FilePath &aFirst, const FilePath &aSecond)
	{
		const size_t tFirstDirLevel = aFirst.getDirLevel();
		const size_t tSecondDirLevel = aSecond.getDirLevel();
		if( tFirstDirLevel != tSecondDirLevel )
			return false;

		return aFirst.startsWith(aSecond, tFirstDirLevel);
	}

	std::ostream& operator << (std::ostream& aStream, const fs::FilePath& aPath)
	{
		aStream << (aPath.isDirectory() ? "[D]" : "[F]") << aPath.getPath();
		return aStream;
	}

	FilePath CopyDirectory(const FilePath &aPath)
	{
		return aPath.copy(aPath.getDirLevel());
	}

	FilePath FindCommonPath(const FilePath &aFirst, const FilePath &aSecond)
	{
		const size_t tMinLevel = std::min(aFirst.getLevel(), aSecond.getLevel());
		for(size_t i = 0; i < tMinLevel; ++i)
		{
			if( aFirst.getName(i) != aSecond.getName(i) )
				return i > 0 ? aFirst.copy(i - 1) : FilePath();
		}

		return aFirst.copy(tMinLevel - 1);
	}
}

