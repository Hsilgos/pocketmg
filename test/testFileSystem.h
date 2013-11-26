#pragma once

#include "filemanager.h"
#include "testArchive.h"

namespace test
{
	class TestFileSystem: public fs::IFileManager
	{
		// fs::IFileManager
		virtual std::vector<fs::FilePath> getFileList(const fs::FilePath &aRoot, EntryTypes aEntries, bool aRecursive);
	public:
		// fs::IFileManager
		virtual tools::ByteArray readFile(const fs::FilePath &aFile, size_t aMaxSize);

		struct TestFile
		{
			std::string name;
			tools::ByteArray data;
			TestFile(const std::string& aName, const tools::ByteArray& aData)
				:name(aName), data(aData)
			{
			}
		};

		struct Dir
		{
			std::string name;

			std::vector<Dir> subdirs;
			std::vector<TestFile> files;
			std::vector<TestArchive> archives;
		};

		void addFile(const std::string &aPath, const tools::ByteArray &aData = tools::ByteArray::empty);
		void addFolder(const std::string &aPath);
		//void addPath(const std::string &aPath, bool aIsFile);
		void addArchive(const std::string &aPath, const TestArchive &aAcrh);

		Dir *hasDir(Dir &aParent, const std::string &aName);

		Dir* findDirByPath(const std::string &aPath, bool aIsFile);
	private:

		TestFileSystem::Dir* findDirByPath(const fs::FilePath &aPath);
		//Dir *findDirByPath(const std::string &aPath) const;

		Dir root_;

		Dir &findOrCreateDir(Dir &aParent, const std::string &aName);

		Dir &addEntry(Dir &aParent, const fs::FilePath &aPath, size_t aLevel);
	};
}

