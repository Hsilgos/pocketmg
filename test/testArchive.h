#pragma once

#include <map>
#include <string>

#include "byteArray.h"
#include "iArchive.h"
//#include "testFileSystem.h"

namespace fs
{
	class FilePath;
}

namespace test
{
	class TestArchive
	{
	public:
		TestArchive(const std::string &name = utils::EmptyString);

		typedef std::map<fs::FilePath, tools::ByteArray> Content;

		void addFile(
			const std::string &file_in_arch, 
			const tools::ByteArray &data = tools::ByteArray::empty);

		void addFolder(const std::string &path_in_arch);

		Content &getContent();
		const std::string &getName() const;
		bool empty() const;
	private:
		Content content_;
		std::string name_;
	};

	//////////////////////////////////////////////////////////////////////////

	class TestFileSystem;

	class TestArchiver: public archive::IArchive
	{
		TestFileSystem *file_system_;
		TestArchive *current_archive_;

		static TestArchiver *instance;
		static archive::IArchive *createProxy();
	public:
		TestArchiver(TestFileSystem *afs);
		~TestArchiver();

		static const char *ArchExt;

		bool open(const std::string &file);
		void close();
		std::vector<fs::FilePath> getFileList(bool files_only);
		tools::ByteArray getFile(const fs::FilePath &file_in_archive, size_t max_size);
	};
}

