#pragma once

#include <string>
#include <memory>
#include <vector>
#include <ostream>

#include "filepath.h"
#include "byteArray.h"
#include "filemanager.h"
#include "iArchive.h"
#include "image.h"

namespace img
{
	class Image;
}

namespace manga
{
	void FixUpFileTreeForTest(std::vector<fs::FilePath> &aFiles, const fs::FilePath &aRoot);

	struct PathToFile
	{
		// First is path to archive
		// Second is path in archive

		fs::FilePath filePath;
		fs::FilePath pathInArchive;

		bool operator == (const PathToFile &aOther) const
		{
			return 
				filePath == aOther.filePath &&
				pathInArchive == aOther.pathInArchive;
		}

		bool operator != (const PathToFile &aOther) const
		{
			return !(*this == aOther);
		}

		bool empty() const
		{
			return filePath.empty() && pathInArchive.empty();
		}

		PathToFile()
		{

		}

		PathToFile(const fs::FilePath &aFilePath)
			: filePath(aFilePath)
		{
		}

		PathToFile(const fs::FilePath &aFilePath, const fs::FilePath &aFileInArch)
			: filePath(aFilePath), pathInArchive(aFileInArch)
		{
		}

		PathToFile containingFolder() const
		{
			PathToFile result;
			return result;
		}
	};

	inline std::ostream& operator<< (std::ostream& aStream, const PathToFile& aPath)
	{
		aStream << aPath.filePath;
		if(!aPath.pathInArchive.empty())
			aStream <<" [" <<  aPath.pathInArchive << "]";
		return aStream;
	}

	class BookExplorer
	{
	public:
		BookExplorer(
			fs::IFileManager *aFileMgr,
			fs::IFileManager::EntryTypes aTypes = fs::IFileManager::FileAndDirectory);

		typedef std::vector<fs::FilePath> FileList;

		// Returns file list ascending sorted
		std::vector<PathToFile> fileList() const;

		bool setRoot(const fs::FilePath &aRoot);
		const fs::FilePath &getRoot() const;

		// Enters to some directory or archive
		bool enter(const PathToFile &aPath);
		bool back();

		bool toNextFile();
		bool toPreviousFile();

		bool toFirstFile();
		bool toLastFile();

		PathToFile getCurrentPos() const;
		tools::ByteArray readCurrentFile() const;
	private:
		BookExplorer(const BookExplorer &);
		BookExplorer& operator = (const BookExplorer &);

		bool openArchive(const fs::FilePath &aPath, bool aToBeginning);
		void closeArchive();
		bool isCurrentInArchiveFile() const;
		//bool FindFirstSuitable()
		std::auto_ptr<fs::IFileManager> mFileMgr;

		fs::IFileManager::EntryTypes mFindEntries;

		fs::FilePath mRoot;
		std::auto_ptr<archive::IArchive> mCurrentArchive;

		std::vector<fs::FilePath> mFiles;
		std::vector<fs::FilePath> mFilesInArchive;

		struct FileIndex
		{
			size_t currentFile;
			FileIndex()
				:currentFile(0)
			{
			}
		};

		FileIndex mFs;
		FileIndex mArchive;
	};

	struct Bookmark
	{
		PathToFile		currentFile;
		fs::FilePath	rootDir;
	};

	class IBookCache
	{
	public:
		virtual ~IBookCache();

		virtual IBookCache *clone() = 0;
		virtual void swap(IBookCache *aOther) = 0;
		virtual bool onLoaded(img::Image &aImage) = 0;
		//virtual Cache getCached(size_t aId) const = 0;
	};

	class Book 
	{
	public:
		Book();
		explicit Book(fs::IFileManager *aFileMgr);
		// Looks for next image
		bool incrementPosition();
		// Looks for previous image
		bool decrementPosition();

		void preload();

		// All about current image
		img::Image currentImage() const;
		PathToFile currentPath() const;
		bool hasCurrentImage() const;
		IBookCache *currentCache() const;

		bool setRoot(const fs::FilePath &aRoot);
		bool toFirstFile();
		bool toLastFile();

		void setCachePrototype(IBookCache *aCache);

		// Set/Get bookmark
		Bookmark bookmark() const;
		bool goToBookmark(const Bookmark &aBookmark);
	private:
		Book(const Book &);
		Book& operator = (const Book &);

		struct ImageData {
			img::Image image;
			std::auto_ptr<IBookCache> cache;
			Bookmark bookmark;

			void swap(ImageData &aOther);
			bool empty() const;
			void clear();
		};

		bool findAndLoadPrevious();
		bool findAndLoadNext();

		bool loadFromExplorerInto(ImageData &aData);		

		ImageData mPrevious;
		ImageData mCurrent;
		ImageData mNext;

		BookExplorer mExplorer;
	};
}


