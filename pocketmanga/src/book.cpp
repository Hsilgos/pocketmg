#include "book.h"

#include "filemanager.h"

#include "iArchive.h"

#include "defines.h"

#include <algorithm>
#include <assert.h>

// Recursively find all subdirectories and folders
// find appropriate archiver, open archive, get filelist from archive
// build correct filelist from archive
// decompress file and return decompressed data.
// go to next file.

// random access 

namespace
{
	const int MaxFilesize			= 1024 * 1024 * 20;
	const size_t ArchiveJustOpened	= std::numeric_limits<size_t>::max();
	const size_t FileNotSpecified	= std::numeric_limits<size_t>::max();

	struct CmpForLevel
	{
		const fs::FilePath &entry;
		const size_t level;
		CmpForLevel(const fs::FilePath &aEntry, size_t aLevel)
			:entry(aEntry), level(aLevel)
		{
		}

		CmpForLevel(const CmpForLevel &aOther)
			:entry(aOther.entry), level(aOther.level)
		{

		}

		bool operator ()(const fs::FilePath &aFirst)
		{
			return aFirst.startsWith(entry, level, true);
		}
	private:
		
		CmpForLevel &operator = (const CmpForLevel &);
	};

	bool FindEntryInList(
		const std::vector<fs::FilePath> &aFiles,
		const fs::FilePath &aEntry,
		//size_t &aFirstFileInDir,
		size_t &aFilePosition) 
	{
		const size_t tMaxLevel = aEntry.getDirLevel();
		std::vector<fs::FilePath>::const_iterator itFirst =
			std::find_if(aFiles.begin(), aFiles.end(), CmpForLevel(aEntry, tMaxLevel));

		if( aFiles.end() == itFirst )
			return false;

		if(aEntry.isDirectory())
		{
			aFilePosition = itFirst - aFiles.begin();
		}
		else
		{
			std::vector<fs::FilePath>::const_iterator itCurrent =
				std::find_if(
					itFirst,
					aFiles.end(),
					CmpForLevel(aEntry, aEntry.getLevel()));

			if(aFiles.end() == itCurrent)
				return false;

			aFilePosition = itCurrent - aFiles.begin();
		}

		//aFirstFileInDir = itFirst - aFiles.begin();
		return true;
	}

	size_t FindFirstFileFrom(const std::vector<fs::FilePath> &aFiles, size_t aCurrentFile)
	{
		assert(aCurrentFile < aFiles.size());
		const fs::FilePath &tCurrent = aFiles[aCurrentFile];
		for( size_t i = aCurrentFile; i > 0; --i )
		{
			if( !HaveSameDirectory(aFiles[i], tCurrent) )
				return i + 1;
		}

		return HaveSameDirectory(aFiles[0], tCurrent) ? 0 : 1;
	}

	/*
	/folder1/file1
	/folder1/folder2/file1

	/folder1/
	/folder1/folder2/file1

	/folder3/file1
	/folder2/file2

	/folder3
	/folder2/file1

	/folder1/folder2/folder3
	/folder1/folder4
	*/

	template<class Cmp>
	void FixUpFileTree(std::vector<fs::FilePath> &aFiles, const fs::FilePath &aRoot, Cmp aSortWay, bool aFilesOnly)
	{
		if( aFiles.empty() )
			return;

		std::sort(aFiles.begin(), aFiles.end(), aSortWay);

		if(aFilesOnly)
		{

		}
		else
		{
			fs::FilePath tPrevDir = CopyDirectory(aRoot);
			std::vector<fs::FilePath> tExtras;

			if(aFiles[0] != aRoot)
				tExtras.push_back(aRoot);

			for( size_t i = 0; i < aFiles.size(); ++i )
			{
				fs::FilePath tCurrentDir = CopyDirectory(aFiles[i]);
				if( tPrevDir.equals(tCurrentDir, true) )
					continue;

				if( tCurrentDir.getLevel() < tPrevDir.getLevel() &&
					tPrevDir.startsWith(tCurrentDir, true))
				{
					// /folder1/folder2/folder3/folder4
					// /folder1/folder2
					tPrevDir = tCurrentDir;
					continue;
				}

				if( (tCurrentDir.getLevel() == tPrevDir.getLevel() + 1) &&
					tCurrentDir.startsWith(tPrevDir, true) && 
					aFiles[i].isDirectory())
				{
					// /folder1/
					// /folder1/folder2
					tPrevDir = tCurrentDir;
					continue;
				}

				fs::FilePath tCommon = FindCommonPath(tPrevDir, tCurrentDir);
				const size_t tCommonLevel = tCommon.getLevel();
				const size_t tCopyTill = tCurrentDir.getLevel() -  1;
				for( size_t i = tCommonLevel; i < tCopyTill; ++i )
				{
					tExtras.push_back(tCurrentDir.copy(i));
				}
				tPrevDir = tCurrentDir;
			}

			if( !tExtras.empty() )
			{
				std::vector<fs::FilePath> tResult;
				tResult.reserve(aFiles.size() + tExtras.size());
				std::merge(
					aFiles.begin(), aFiles.end(),
					tExtras.begin(), tExtras.end(),
					std::back_inserter(tResult),
					aSortWay);

				aFiles.swap(tResult);
			}
		}
	}
}

namespace manga
{
	void FixUpFileTreeForTest(std::vector<fs::FilePath> &aFiles, const fs::FilePath &aRoot)
	{
		FixUpFileTree(aFiles, aRoot, fs::WordNumberOrder(), false);
	}

	//////////////////////////////////////////////////////////////////////////

	BookExplorer::BookExplorer(fs::IFileManager *aFileMgr, fs::IFileManager::EntryTypes aTypes)
		:mFileMgr(aFileMgr), mFindEntries(aTypes)
	{
	}

	// Returns filelist ascending sorted
	std::vector<PathToFile> BookExplorer::fileList() const
	{
		std::vector<PathToFile> tResult;
		PathToFile tCurrPos = getCurrentPos();
		if( tCurrPos.empty() )
			return tResult;

		if( mCurrentArchive.get() )
		{
			const size_t tDirLevel = tCurrPos.pathInArchive.getDirLevel();

			const size_t tFirst = FindFirstFileFrom(mFilesInArchive, mArchive.currentFile);

			for( size_t i = tFirst; i < mFilesInArchive.size(); ++i )
			{
				if( !mFilesInArchive[i].startsWith(tCurrPos.pathInArchive, tDirLevel, true) )
					break;

				if( mFilesInArchive[i].getLevel() > tDirLevel )
				{
					if( tResult.empty() || !mFilesInArchive[i].startsWith(tResult.back().pathInArchive, true) )
					{
						tResult.push_back(PathToFile(mFiles[mFs.currentFile], mFilesInArchive[i].copy(tDirLevel)));
					}
				}
			}
		}
		else
		{
			const size_t tDirLevel = tCurrPos.filePath.getDirLevel();

			const size_t tFirst = FindFirstFileFrom(mFiles, mFs.currentFile);

			for( size_t i = tFirst; i < mFiles.size(); ++i )
			{
				if( !mFiles[i].startsWith(tCurrPos.filePath, tDirLevel, true) )
					break;

				if( mFiles[i].getLevel() > tDirLevel )
				{
					if( tResult.empty() || !mFiles[i].startsWith(tResult.back().filePath, true) )
					{
						tResult.push_back(PathToFile(mFiles[i].copy(tDirLevel)));
					}
				}
			}

		}
		return tResult;
	}

	bool BookExplorer::setRoot(const fs::FilePath &aRoot)
	{
		mRoot = aRoot;
		mFiles = mFileMgr->getFileList(aRoot, mFindEntries, true);

		const bool tFilesOnly = (fs::IFileManager::Directory != (mFindEntries & fs::IFileManager::Directory));
		FixUpFileTree(mFiles, aRoot, fs::WordNumberOrder(), tFilesOnly);
		//if( std::find(mFiles.begin(), mFiles.end(), aRoot) == mFiles.end() )
		//	mFiles.push_back(aRoot);

		
		return enter(PathToFile(aRoot));
	}

	const fs::FilePath &BookExplorer::getRoot() const
	{
		return mRoot;
	}

	bool BookExplorer::openArchive(const fs::FilePath &aPath, bool aToBeginning)
	{
		mCurrentArchive.reset(archive::recognize(aPath));

		if (!mCurrentArchive.get())
			return false;

		const bool tFilesOnly = (fs::IFileManager::Directory != (mFindEntries & fs::IFileManager::Directory));
		mFilesInArchive = mCurrentArchive->getFileList(tFilesOnly);

		FixUpFileTree(mFilesInArchive, fs::FilePath(), fs::WordNumberOrder(), tFilesOnly);

		mArchive.currentFile = aToBeginning ? 0 : mFilesInArchive.size() - 1;

		return true;
	}

	bool BookExplorer::isCurrentInArchiveFile() const
	{
		return 
			mArchive.currentFile < mFilesInArchive.size() &&
			!mFilesInArchive[mArchive.currentFile].isDirectory();
	}

	void BookExplorer::closeArchive()
	{
		mCurrentArchive.reset();
		mFilesInArchive.clear();
		mArchive.currentFile = 0;
	}

	// Enters to some directory or archive
	bool BookExplorer::enter(const PathToFile &aPath)
	{
		if( mRoot.empty() || aPath.empty() )
			return false;

		// Already on the place?
		PathToFile tCurrentPos = getCurrentPos();
		if( tCurrentPos == aPath )
			return true;

		if( aPath.filePath.isDirectory() )
		{
			// it's directory...
			if( tCurrentPos.filePath != aPath.filePath )
			{
				if( !FindEntryInList(
						mFiles,
						aPath.filePath,
						//mFs.firstInCurrDir,
						mFs.currentFile) )
				{
					return false;
				}

				// close previous opened archive
				closeArchive();
				return true;
			}
		}
		else
		{
			// probably archive?
			if( tCurrentPos.filePath != aPath.filePath || !mCurrentArchive.get() )
			{
				if( !FindEntryInList(
						mFiles,
						aPath.filePath,
						mFs.currentFile) )
				{
					return false;
				}

				if( !openArchive(aPath.filePath, true) )
				{
					// Not an archive
					if( !aPath.pathInArchive.empty() )
						return false;

					// close previous archive
					closeArchive();
					return true;
				}
			}
			else if( aPath.pathInArchive.empty() )
			{
				if( mCurrentArchive.get() )
				{
					if(!FindEntryInList(
						mFilesInArchive,
						aPath.pathInArchive,
						mArchive.currentFile))
					{
						return false;
					}
				}
				return true;
			}
			else if ( !mCurrentArchive.get() )
			{
				return false;
			}

			if(!FindEntryInList(
					mFilesInArchive,
					aPath.pathInArchive,
					mArchive.currentFile))
			{
				return false;
			}

			return true;
		}

		return false;
	}

	bool BookExplorer::back()
	{
		PathToFile tNewPath = getCurrentPos();
		if(!tNewPath.pathInArchive.empty())
			tNewPath.pathInArchive.removeLastEntry();
		else 
			tNewPath.filePath.removeLastEntry();

		return enter(tNewPath);
		//return false;
	}

	bool BookExplorer::toNextFile()
	{
		for(;;)
		{
			if( mCurrentArchive.get() ) 
			{
				if(mFilesInArchive.empty())
				{
					closeArchive();
					continue;
				}
				
				//assert(mArchive.firstInCurrDir < mFilesInArchive.size());
				if( ++mArchive.currentFile >= mFilesInArchive.size() )
				{
					closeArchive();
					continue;
				}

				const fs::FilePath &tCurrEntry = mFilesInArchive[mArchive.currentFile];

				if( tCurrEntry.isDirectory() )
					continue;

				return true;
			}
			else
			{
				if( mFiles.empty() )
					return false;

				if( mFs.currentFile + 1 >= mFiles.size() )
					return false;
				
				++mFs.currentFile;
				const fs::FilePath &tCurrEntry = mFiles[mFs.currentFile];

				if( tCurrEntry.isDirectory() )
					continue;

				// archive?
				if( openArchive(tCurrEntry, true) )
				{
					if( isCurrentInArchiveFile() )
						return true;

					continue;
				}

				return true;
			}
		}
	}

	bool BookExplorer::toPreviousFile()
	{
		if( mFiles.empty() )
			return false;

		// store position

		for(;;)
		{
			if( mCurrentArchive.get() ) 
			{
				if( mFilesInArchive.empty() )
				{
					closeArchive();
					continue;
				}

				//assert(mArchive.firstInCurrDir < mFilesInArchive.size());
				if( 0 == mArchive.currentFile )
				{
					closeArchive();
					continue;
				}

				--mArchive.currentFile;

				const fs::FilePath &tCurrEntry = mFilesInArchive[mArchive.currentFile];

				if( tCurrEntry.isDirectory() )
					continue;

				return true;
			}
			else
			{
				if( 0 == mFs.currentFile )
				{
					//restore position
					return false;
				}

				--mFs.currentFile;
				const fs::FilePath &tCurrEntry = mFiles[mFs.currentFile];

				if( tCurrEntry.isDirectory() )
					continue;

				// archive?
				if( openArchive(tCurrEntry, false) )
				{
					if( isCurrentInArchiveFile() )
						return true;

					continue;
				}

				return true;
			}
		}
	}

	bool BookExplorer::toFirstFile()
	{
		closeArchive();
		mFs.currentFile = 0;
		//mFs.firstInCurrDir = 0;
		if( mFiles.empty() )
			return false;

		const fs::FilePath &tCurrEntry = mFiles[mFs.currentFile];

		if( tCurrEntry.isDirectory() )
			return toNextFile();

		if( openArchive(tCurrEntry, true)  && 
			!isCurrentInArchiveFile() )
			return toNextFile();

		return true;
	}

	bool BookExplorer::toLastFile()
	{
		closeArchive();
		if( mFiles.empty() )
			return false;

		mFs.currentFile = mFiles.size() - 1;

		const fs::FilePath &tCurrEntry = mFiles[mFs.currentFile];
		if( tCurrEntry.isDirectory() )
			return toPreviousFile();

		if( openArchive(tCurrEntry, false)  && 
			!isCurrentInArchiveFile() )
			return toPreviousFile();

		return true;
	}

	PathToFile BookExplorer::getCurrentPos() const
	{
		PathToFile tResult;

		if( !mFiles.empty() )
		{
			assert(mFs.currentFile < mFiles.size());
			tResult.filePath = mFiles[mFs.currentFile];
		}

		if( mCurrentArchive.get() && !mFilesInArchive.empty() )
		{
			assert(mArchive.currentFile < mFilesInArchive.size());
			tResult.pathInArchive = mFilesInArchive[mArchive.currentFile];
		}

		return tResult;
	}

	tools::ByteArray BookExplorer::readCurrentFile() const
	{
		if( mCurrentArchive.get() )
		{
			if( !mFilesInArchive.empty() &&
				mArchive.currentFile < mFilesInArchive.size() &&
				!mFilesInArchive[mArchive.currentFile].isDirectory() )
			{
				return mCurrentArchive->getFile(mFilesInArchive[mArchive.currentFile], MaxFilesize);
			}

			return tools::ByteArray::empty;
		}

		if( !mFiles.empty() &&
			mFs.currentFile < mFiles.size() &&
			!mFiles[mFs.currentFile].isDirectory() )
		{
			return mFileMgr->readFile(mFiles[mFs.currentFile], MaxFilesize);
		}

		return tools::ByteArray::empty;
	}
	
	//////////////////////////////////////////////////////////////////////////
	IBookCache::~IBookCache()
	{
	}

	Book::Book()
		:mExplorer(fs::IFileManager::create(), fs::IFileManager::File)
	{
	}

	Book::Book(fs::IFileManager *aFileMgr)
		:mExplorer(aFileMgr, fs::IFileManager::File)
	{
	}

	bool Book::setRoot(const fs::FilePath &aRoot)
	{
		return mExplorer.setRoot(aRoot);
	}

	bool Book::toFirstFile()
	{
		if( !mExplorer.toFirstFile() )
			return false;

		if( loadFromExplorerInto(mCurrent) )
			return true;

		return findAndLoadNext();
	}

	bool Book::toLastFile()
	{
		if( !mExplorer.toLastFile() )
			return false;

		if( loadFromExplorerInto(mPrevious) )
			return true;

		return findAndLoadPrevious();
	}

	bool Book::goToBookmark(const Bookmark &aBookmark)
	{
		return
			mExplorer.setRoot(aBookmark.rootDir) &&
			mExplorer.enter(aBookmark.currentFile);
	}

	Bookmark Book::bookmark() const
	{
		Bookmark tResult;

		tResult.rootDir = mExplorer.getRoot();
		tResult.currentFile = mExplorer.getCurrentPos();

		return tResult;
	}

	void Book::ImageData::swap(ImageData &aOther)
	{
		image.swap(aOther.image);
		std::swap(bookmark.currentFile, aOther.bookmark.currentFile);
		std::swap(bookmark.rootDir, aOther.bookmark.rootDir);

		if( cache.get() && aOther.cache.get() )
		{
			cache->swap(aOther.cache.get());
		}
	}

	bool Book::ImageData::empty() const
	{
		return image.empty();
	}

	void Book::ImageData::clear()
	{
		image.destroy();
		bookmark.currentFile = PathToFile();
		bookmark.rootDir.clear();
	}

	bool Book::incrementPosition()
	{
		if( mCurrent.empty() )
			return toFirstFile();

		if( mNext.empty() )
		{
			if( !mExplorer.enter(mCurrent.bookmark.currentFile) ||
				!findAndLoadNext() )
				return false;
		}

		if( !mNext.empty() )
		{
			mPrevious.swap(mCurrent);
			mCurrent.swap(mNext);

			mNext.clear();
			return true;
		}

		return false;
	}

	bool Book::decrementPosition()
	{
		if( mCurrent.empty() )
			return toLastFile();

		if( mPrevious.empty() )
		{
			if( !mExplorer.enter(mCurrent.bookmark.currentFile) ||
				!findAndLoadPrevious())
				return false;
		}

		if( !mPrevious.empty() )
		{
			mNext.swap(mCurrent);
			mCurrent.swap(mPrevious);

			mPrevious.clear();
			return true;
		}

		return false;
	}

	bool Book::findAndLoadPrevious()
	{
		while( mExplorer.toPreviousFile() )
		{
			if(loadFromExplorerInto(mPrevious))
				return true;
		}

		return false;
	}

	bool Book::findAndLoadNext()
	{
		// find first suitable file
		while( mExplorer.toNextFile() )
		{
			if(loadFromExplorerInto(mNext))
				return true;
		}

		return false;
	}

	bool Book::loadFromExplorerInto(ImageData &aData)
	{
		PathToFile tPath = mExplorer.getCurrentPos();

		const fs::FilePath &tFile =
			tPath.pathInArchive.empty() ? tPath.filePath : tPath.pathInArchive;

		tools::ByteArray tData = mExplorer.readCurrentFile();
		if( tData.isEmpty() )
			return false;

		if( aData.image.load(tFile.getExtension(), tData) )
		{
			aData.bookmark.currentFile = mExplorer.getCurrentPos();
			if( aData.cache.get() )
				aData.cache->onLoaded(aData.image);

			return true;
		}

		return false;
	}

	void Book::preload()
	{
		if( mCurrent.empty() )
			toFirstFile();

		if( !mNext.empty() && !mPrevious.empty() )
			return;

		if( !mCurrent.empty() )
		{
			Bookmark tCurrBM = mCurrent.bookmark;
			
			if( mPrevious.empty() )
			{
				if( mExplorer.enter(tCurrBM.currentFile) )
					findAndLoadPrevious();
			}

			if( mNext.empty() )
			{
				if( mExplorer.enter(tCurrBM.currentFile) )
					findAndLoadNext();
			}
		}
	}

	img::Image Book::currentImage() const
	{
		return mCurrent.image;
	}

	IBookCache *Book::currentCache() const
	{
		return mCurrent.cache.get();
	}

	void Book::setCachePrototype(IBookCache *aCache)
	{
		if( aCache )
		{
			mPrevious.cache.reset(aCache->clone());
			mCurrent.cache.reset(aCache);
			mNext.cache.reset(aCache->clone());
		}
		else
		{
			mPrevious.cache.reset();
			mCurrent.cache.reset();
			mNext.cache.reset();
		}
	}
}
