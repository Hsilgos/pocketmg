#include "testArchive.h"
#include "testFileSystem.h"

#include "algorithms_ex.h"

namespace test
{
	TestArchive::TestArchive(const std::string &aName)
		:mName(aName)
	{

	}

	void TestArchive::addFile(
		const std::string &aFileInArch, const tools::ByteArray &aData)
	{
		fs::FilePath tPath(aFileInArch, true);
		mContent[tPath] = aData;
	}

	void TestArchive::addFolder(const std::string &aPathInArch)
	{
		fs::FilePath tPath(aPathInArch, false);
		mContent[tPath] = tools::ByteArray::empty;
	}

	TestArchive::Content &TestArchive::getContent()
	{
		return mContent;
	}

	const std::string &TestArchive::getName() const
	{
		return mName;
	}

	bool TestArchive::empty() const 
	{
		return mName.empty() && mContent.empty();
	}

	//////////////////////////////////////////////////////////////////////////
	TestArchiver::TestArchiver(TestFileSystem *afs)
		:mFileSystem(afs),
		mCurrentArchive(0)
	{
		instance = this;
		archive::IArchive::registerArchiver(ArchExt, &TestArchiver::createProxy);
	}

	TestArchiver::~TestArchiver()
	{
		archive::IArchive::unregisterArchiver(ArchExt);
		instance = NULL;
	}

	const char *TestArchiver::ArchExt = "testarch";

	class ProxyArch: public archive::IArchive
	{
	public:
		ProxyArch(archive::IArchive *aOther)
			:mOther(aOther)
		{
		}
	private:
		virtual bool open(const std::string &aFile )
		{
			return mOther->open(aFile);
		}

		virtual void close()
		{
			return mOther->close();
		}
		virtual std::vector<fs::FilePath> getFileList(bool aFilesOnly)
		{
			return mOther->getFileList(aFilesOnly);
		}
		virtual tools::ByteArray getFile(const fs::FilePath &aFileInArchive, size_t aMaxSize)
		{
			return mOther->getFile(aFileInArchive, aMaxSize);
		}

		archive::IArchive *mOther;
	};

	archive::IArchive *TestArchiver::createProxy()
	{
		return new ProxyArch(instance);
	}

	TestArchiver *TestArchiver::instance = 0;
	
	bool TestArchiver::open(const std::string &aFile)
	{
		fs::FilePath tPath(aFile, true);
		if( tPath.getExtension() != ArchExt )
			return false;

		TestFileSystem::Dir *tDir = mFileSystem->findDirByPath(aFile, true);
		if( !tDir )
			return false;

		if( tDir->archives.empty() )
			return false;

		std::vector<TestArchive>::iterator
			it = tDir->archives.begin(),
			itEnd = tDir->archives.end();

		for( ;it != itEnd; ++it)
		{
			if( it->getName() == tPath.getFileName() )
			{
				mCurrentArchive = &(*it);
				return true;
			}
		}

		return false;
	}

	void TestArchiver::close()
	{
		mCurrentArchive = 0;
	}

	std::vector<fs::FilePath> TestArchiver::getFileList(bool aFilesOnly)
	{
		std::vector<fs::FilePath> tResult;
		if(mCurrentArchive)
			utils::collectKeys(mCurrentArchive->getContent(), std::back_inserter(tResult));
		return tResult;
	}

	tools::ByteArray TestArchiver::getFile(const fs::FilePath &aFileInArchive, size_t aMaxSize)
	{
		tools::ByteArray tResult;

		if( mCurrentArchive )
		{
			TestArchive::Content::iterator it = mCurrentArchive->getContent().find(aFileInArchive);
			if( it != mCurrentArchive->getContent().end() )
				tResult = it->second;
		}

		return tResult;
	}
}
