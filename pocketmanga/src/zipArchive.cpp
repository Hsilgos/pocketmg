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
		unzFile zip_file_;

		bool open(const std::string &file )
		{
			close();

			zip_file_ = unzOpen(file.c_str());

			return zip_file_ != NULL;
		}

		void close()
		{
			if( zip_file_ )
			{
				unzClose(zip_file_);
				zip_file_ = 0;
			}
		}

		bool isDirectory(const std::string &path) const
		{
			return !path.empty() && path[path.size() - 1] == '/';
		}

		fs::FilePath createFilepath(const std::string &path)
		{
			if( path.empty() )
				return fs::FilePath();

			fs::FilePath tfpsth(path, !isDirectory(path) );

			return tfpsth;
		}

		std::vector<fs::FilePath> getFileList(bool files_only)
		{
			std::vector<fs::FilePath> list;
			int result = unzGoToFirstFile(zip_file_);

			const int MaxFileName = 1024;
			char buffer[MaxFileName];
			while( UNZ_OK == result )
			{
				unz_file_info file_info = {0};
				unzGetCurrentFileInfo(zip_file_, &file_info, buffer, MaxFileName, NULL, 0, NULL, 0);

				fs::FilePath path = createFilepath(buffer);

				if( !path.empty() && ( !path.isDirectory() || !files_only ) )
					list.push_back(path);

				result = unzGoToNextFile(zip_file_);
			}

			return list;
		}

		tools::ByteArray getFile(const fs::FilePath &file_in_archive, size_t max_size)
		{
			tools::ByteArray data;
			if( UNZ_OK != unzLocateFile(zip_file_, file_in_archive.getPath().c_str(), 1) )
				return data;

			if( UNZ_OK != unzOpenCurrentFile(zip_file_) )
				return data;

			// read length
			unz_file_info file_info = {0};
			if( UNZ_OK != unzGetCurrentFileInfo(zip_file_, &file_info, NULL, 0, NULL, 0, NULL, 0) )
			{
				unzCloseCurrentFile(zip_file_);
				return data;
			}

			if( file_info.uncompressed_size > max_size )
			{
				unzCloseCurrentFile(zip_file_);
				return data;
			}

			void *buff_ptr = data.askBuffer(file_info.uncompressed_size);
			const int total_read = unzReadCurrentFile(zip_file_, buff_ptr, data.getLength());
			if( total_read != file_info.uncompressed_size )
				data.reset();

			unzCloseCurrentFile(zip_file_);
			return data;
		}

		~ZipArchive()
		{
			close();
		}
	public:
		ZipArchive()
			:zip_file_(0)
		{
		}
	};

	AUTO_REGISTER_ARCHIVER("zip", ZipArchive);
}
