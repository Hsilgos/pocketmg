#pragma once

#include "filemanager.h"
#include "testArchive.h"

namespace test {
class TestFileSystem: public fs::IFileManager {
  // fs::IFileManager
  virtual std::vector<fs::FilePath> getFileList(const fs::FilePath &root, EntryTypes entries, bool recursive);
public:
  // fs::IFileManager
  virtual tools::ByteArray readFile(const fs::FilePath &file, size_t max_size);

  struct TestFile {
    std::string name;
    tools::ByteArray data;
    TestFile(const std::string& name, const tools::ByteArray& data)
      :name(name), data(data) {
    }
  };

  struct Dir {
    std::string name;

    std::vector<Dir> subdirs;
    std::vector<TestFile> files;
    std::vector<TestArchive> archives;
  };

  void addFile(const std::string &path, const tools::ByteArray &data = tools::ByteArray::empty);
  void addFolder(const std::string &path);
  //void addPath(const std::string &path, bool is_file);
  void addArchive(const std::string &path, const TestArchive &acrh);

  Dir *hasDir(Dir &parent, const std::string &name);

  Dir* findDirByPath(const std::string &path, bool is_file);
private:

  TestFileSystem::Dir* findDirByPath(const fs::FilePath &path);
  //Dir *findDirByPath(const std::string &path) const;

  Dir root_;

  Dir &findOrCreateDir(Dir &parent, const std::string &name);

  Dir &addEntry(Dir &parent, const fs::FilePath &path, size_t level);
};
}

