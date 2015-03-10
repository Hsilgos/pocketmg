#pragma once

#include <string>

#include <vector>

namespace tools {
class ByteArray;
}

namespace fs {
class FilePath;

class Comparator {
  virtual bool compareLevel(const std::string &name1, const std::string &name2, bool &result) const = 0;
public:
  bool operator ()(const fs::FilePath &first, const fs::FilePath &second) const;
  virtual ~Comparator();
};

class WordNumberOrder: public Comparator {
  virtual bool compareLevel(const std::string &name1, const std::string &name2, bool &result) const;
};

class NumberOrder: public Comparator {
  virtual bool compareLevel(const std::string &name1, const std::string &name2, bool &result) const;
};

class IFileManager {
public:
  enum EntryTypes {
    File			= 1,
    Directory		= 2,
    FileAndDirectory	= File|Directory
  };

  //virtual std::vector<fs::FilePath> getFileList(const std::string &root, EntryTypes entries, bool recursive) = 0;
  virtual std::vector<fs::FilePath> getFileList(const fs::FilePath &root, EntryTypes entries, bool recursive) = 0;
  virtual tools::ByteArray readFile(const fs::FilePath &file, size_t max_size);

  virtual ~IFileManager();

  static IFileManager *create();
};
}
