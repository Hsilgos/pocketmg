#include "testFileSystem.h"

#include "byteArray.h"
#include "filepath.h"

#include <algorithm>


namespace test
{
	std::vector<fs::FilePath> TestFileSystem::getFileList(const fs::FilePath &aRoot, EntryTypes aEntries, bool aRecursive)
	{
		std::vector<fs::FilePath> tResult;

		const fs::FilePath tPath = aRoot;

		if( mRoot.name != tPath.getName(0) )
			return tResult;

		Dir *tCurrDir = &mRoot;

		fs::FilePath tPrefix;
		tPrefix.addFolder(tCurrDir->name);

		for( size_t i = 1; i < tPath.getLevel(); ++i )
		{
			if( !hasDir(*tCurrDir, tPath.getName(i)) )
				return tResult;// empty

			tCurrDir = &findOrCreateDir(*tCurrDir, tPath.getName(i));
			tPrefix.addFolder(tCurrDir->name);
		}

		
		for( size_t i = 0; i < tCurrDir->subdirs.size(); ++i )
		{
			fs::FilePath tPath = tPrefix;
			tPath.addFolder(tCurrDir->subdirs[i].name);
			
			if( (aEntries & Directory) == Directory )
				tResult.push_back(tPath);

			if(aRecursive)
			{
				std::vector<fs::FilePath> tSubfiles = getFileList(tPath, aEntries, aRecursive);
				tResult.insert(tResult.end(), tSubfiles.begin(), tSubfiles.end());
			}
		}

		if( (aEntries & File) == File )
		{
			for( size_t i = 0; i < tCurrDir->files.size(); ++i )
			{
				fs::FilePath tPath = tPrefix;
				tPath.setFile(tCurrDir->files[i].name);

				tResult.push_back(tPath);
			}
		}

		std::random_shuffle(tResult.begin(), tResult.end());

		return tResult;
	}

	tools::ByteArray TestFileSystem::readFile(const fs::FilePath &aFile, size_t aMaxSize)
	{
		Dir* tDir = findDirByPath(aFile);
		if( !tDir )
			return tools::ByteArray::empty;

		std::vector<TestFile> &tFiles = tDir->files;
		for( size_t i = 0; i < tFiles.size(); ++i )
		{
			if( tFiles[i].name == aFile.getName() )
				return tFiles[i].data;
		}

		return tools::ByteArray::empty;
	}

	struct CmpDir
	{
		const std::string &mName;
		CmpDir(const std::string &aName)
			:mName(aName)
		{
		}

		bool operator ()(const TestFileSystem::Dir& aDir) const
		{
			return aDir.name == mName;
		}
	};

	TestFileSystem::Dir* TestFileSystem::hasDir(Dir &aParent, const std::string &aName)
	{
		std::vector<Dir>::iterator it =
			std::find_if(aParent.subdirs.begin(), aParent.subdirs.end(), CmpDir(aName));

		return (it != aParent.subdirs.end())?(&(*it)):0;
	}

	TestFileSystem::Dir &TestFileSystem::findOrCreateDir(Dir &aParent, const std::string &aName)
	{
		std::vector<Dir>::iterator it =
			std::find_if(aParent.subdirs.begin(), aParent.subdirs.end(), CmpDir(aName));

		if( it != aParent.subdirs.end() )
		{
			return *it;
		}

		Dir tNewDir;
		tNewDir.name = aName;
		aParent.subdirs.push_back(tNewDir);

		return aParent.subdirs.back();
	}

	TestFileSystem::Dir &TestFileSystem::addEntry(Dir &aParent, const fs::FilePath &aPath, size_t aLevel)
	{
		if( aLevel >= aPath.getLevel() )
			return aParent;

		const std::string tName = aPath.getName(aLevel);
		const bool tIsFile = !aPath.isDirectory(aLevel);

		if( !tIsFile )
			return addEntry(findOrCreateDir(aParent, tName), aPath, aLevel + 1);

		return aParent;
	}

	void TestFileSystem::addFile(const std::string &aPath, const tools::ByteArray &aData)
	{
		fs::FilePath tPath(aPath, true);

		if( 0 == tPath.getLevel() )
			return;

		mRoot.name = tPath.getName(0);
		Dir &tLastParent = addEntry(mRoot, tPath, 1);
		tLastParent.files.push_back(TestFile(tPath.getName(), aData));
	}

	void TestFileSystem::addFolder(const std::string &aPath)
	{
		fs::FilePath tPath(aPath, false);

		if( 0 == tPath.getLevel() )
			return;

		mRoot.name = tPath.getName(0);
		addEntry(mRoot, tPath, 1);
	}

	void TestFileSystem::addArchive(const std::string &aPath, const TestArchive &aAcrh)
	{
		fs::FilePath tPath(aPath, false);

		if( 0 == tPath.getLevel() )
			return;

		mRoot.name = tPath.getName(0);
		Dir &tLastParent = addEntry(mRoot, tPath, 1);
		tLastParent.archives.push_back(aAcrh);
		tLastParent.files.push_back(TestFile(aAcrh.getName(), tools::ByteArray::empty));
	}

	TestFileSystem::Dir* TestFileSystem::findDirByPath(const std::string &aPath, bool aIsFile)
	{
		fs::FilePath tPath(aPath, aIsFile);
		return findDirByPath(tPath);
	}

	TestFileSystem::Dir* TestFileSystem::findDirByPath(const fs::FilePath &aPath)
	{
		fs::FilePath tPath = aPath;
		if( !tPath.isDirectory() )
			tPath.removeLastEntry();

		if( 0 == tPath.getLevel() )
			return false;

		Dir *tCurrDir = &mRoot;

		for( size_t i = 1; i < tPath.getLevel(); ++i )
		{
			tCurrDir = hasDir(*tCurrDir, tPath.getName(i));
			if( !tCurrDir )
				return 0;
		}

		return tCurrDir;
	}
}

