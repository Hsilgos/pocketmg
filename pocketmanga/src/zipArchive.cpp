#include "iArchive.h"


#include "unzip.h"
#include "filepath.h"

#include <memory>

namespace
{
	// 20 megobytes?
	//const int MaxUncomppressedFilesize = 1024 * 1024 * 20;
}

namespace archive
{
	class ZipArchive: public IArchive
	{
		unzFile mZipFile;

		bool open(const std::string &aFile )
		{
			close();

			mZipFile = unzOpen(aFile.c_str());

			return mZipFile != NULL;
		}

		void close()
		{
			if( mZipFile )
			{
				unzClose(mZipFile);
				mZipFile = 0;
			}
		}

		bool isDirectory(const std::string &aPath) const
		{
			return !aPath.empty() && aPath[aPath.size() - 1] == '/';
		}

		fs::FilePath createFilepath(const std::string &aPath)
		{
			if( aPath.empty() )
				return fs::FilePath();

			fs::FilePath tfpsth(aPath, !isDirectory(aPath) );

			return tfpsth;
		}

		std::vector<fs::FilePath> getFileList(bool aFilesOnly)
		{
			std::vector<fs::FilePath> tList;
			int tResult = unzGoToFirstFile(mZipFile);

			const int MaxFileName = 1024;
			char tBuffer[MaxFileName];
			while( UNZ_OK == tResult )
			{
				unz_file_info tFileInfo = {0};
				unzGetCurrentFileInfo(mZipFile, &tFileInfo, tBuffer, MaxFileName, NULL, 0, NULL, 0);

				fs::FilePath tPath = createFilepath(tBuffer);

				if( !tPath.empty() && ( !tPath.isDirectory() || !aFilesOnly ) )
					tList.push_back(tPath);

				tResult = unzGoToNextFile(mZipFile);
			}

			return tList;
		}

		tools::ByteArray getFile(const fs::FilePath &aFileInArchive, size_t aMaxSize)
		{
			tools::ByteArray tData;
			if( UNZ_OK != unzLocateFile(mZipFile, aFileInArchive.getPath().c_str(), 1) )
				return tData;

			if( UNZ_OK != unzOpenCurrentFile(mZipFile) )
				return tData;

			// read length
			unz_file_info tFileInfo = {0};
			if( UNZ_OK != unzGetCurrentFileInfo(mZipFile, &tFileInfo, NULL, 0, NULL, 0, NULL, 0) )
			{
				unzCloseCurrentFile(mZipFile);
				return tData;
			}

			if( tFileInfo.uncompressed_size > aMaxSize )
			{
				unzCloseCurrentFile(mZipFile);
				return tData;
			}

			void *tBuffPtr = tData.askBuffer(tFileInfo.uncompressed_size);
			const int tTotalRead = unzReadCurrentFile(mZipFile, tBuffPtr, tData.getLength());
			if( tTotalRead != tFileInfo.uncompressed_size )
				tData.reset();

			unzCloseCurrentFile(mZipFile);
			return tData;
		}

		~ZipArchive()
		{
			close();
		}
	public:
		ZipArchive()
			:mZipFile(0)
		{
		}
	};

	AUTO_REGISTER_ARCHIVER("zip", ZipArchive);
}
