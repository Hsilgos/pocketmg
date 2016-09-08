#pragma once

#include <string>
#include <vector>

#include "byteArray.h"
#include "filepath.h"
#include "defines.h"

namespace archive {
class IArchive {
public:
  virtual ~IArchive() {};

  virtual bool open(const std::string& file) = 0;
  virtual void close() = 0;
  virtual std::vector<fs::FilePath> getFileList(bool files_only) = 0;
  virtual tools::ByteArray getFile(const fs::FilePath& file_in_archive, size_t max_size) = 0;

  typedef IArchive* (*FactoryMethod)();
  static void registerArchiver(const std::string& pref_ext, FactoryMethod method);
  static void unregisterArchiver(const std::string& pref_ext);
};

#define AUTO_REGISTER_ARCHIVER(pref_ext, archive_class)                  \
  namespace                         \
  {                           \
  archive::IArchive* TOKEN_JOIN(create_archiver, __LINE__) ()            \
  {                          \
    return new archive_class;                  \
  }                          \
                            \
  struct TOKEN_JOIN (FactoryRegistrar, __LINE__)               \
  {                          \
    TOKEN_JOIN(FactoryRegistrar, __LINE__) ()               \
    {                         \
      archive::IArchive::registerArchiver(pref_ext, TOKEN_JOIN(create_archiver, __LINE__));   \
    }                         \
  };                          \
  static TOKEN_JOIN(FactoryRegistrar, __LINE__) TOKEN_JOIN(__global_factory_registrar__, __LINE__);  \
  }

IArchive* recognize(const fs::FilePath& path);
}
