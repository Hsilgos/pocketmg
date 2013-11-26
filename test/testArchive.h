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
		TestArchive(const std::string &aName = utils::EmptyString);

		typedef std::map<fs::FilePath, tools::ByteArray> Content;

		void addFile(
			const std::string &aFileInArch, 
			const tools::ByteArray &aData = tools::ByteArray::empty);

		void addFolder(const std::string &aPathInArch);

		Content &getContent();
		const std::string &getName() const;
		bool empty() const;
	private:
		Content mContent;
		std::string mName;
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

		bool open(const std::string &aFile);
		void close();
		std::vector<fs::FilePath> getFileList(bool aFilesOnly);
		tools::ByteArray getFile(const fs::FilePath &aFileInArchive, size_t aMaxSize);
	};
}

