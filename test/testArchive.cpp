#include "testArchive.h"
#include "testFileSystem.h"

#include "algorithms_ex.h"

namespace test {
TestArchive::TestArchive(const std::string& name)
  : name_(name) {}

void TestArchive::addFile(const std::string& file_in_arch, const tools::ByteArray& data)                         {
  fs::FilePath path(file_in_arch, true);
  content_[path] = data;
}

void TestArchive::addFolder(const std::string& path_in_arch) {
  fs::FilePath path(path_in_arch, false);
  content_[path] = tools::ByteArray::empty;
}

TestArchive::Content& TestArchive::getContent() {
  return content_;
}

const std::string& TestArchive::getName() const {
  return name_;
}

bool TestArchive::empty() const {
  return name_.empty() && content_.empty();
}

//////////////////////////////////////////////////////////////////////////
TestArchiver::TestArchiver(TestFileSystem* afs)
  : file_system_(afs), current_archive_(0) {
  instance = this;
  archive::IArchive::registerArchiver(ArchExt, &TestArchiver::createProxy);
}

TestArchiver::~TestArchiver() {
  archive::IArchive::unregisterArchiver(ArchExt);
  instance = NULL;
}

const char* TestArchiver::ArchExt = "testarch";

class ProxyArch : public archive::IArchive {
public:
  ProxyArch(archive::IArchive* other)
    : other_(other) {}
private:
  virtual bool open(const std::string& file) {
    return other_->open(file);
  }

  virtual void close() {
    return other_->close();
  }
  virtual std::vector<fs::FilePath> getFileList(bool files_only) {
    return other_->getFileList(files_only);
  }
  virtual tools::ByteArray getFile(const fs::FilePath& file_in_archive, size_t max_size) {
    return other_->getFile(file_in_archive, max_size);
  }

  archive::IArchive* other_;
};

archive::IArchive* TestArchiver::createProxy() {
  return new ProxyArch(instance);
}

TestArchiver* TestArchiver::instance = 0;

bool TestArchiver::open(const std::string& file) {
  fs::FilePath path(file, true);
  if (path.getExtension() != ArchExt)
    return false;

  TestFileSystem::Dir* dir = file_system_->findDirByPath(file, true);
  if (!dir)
    return false;

  if (dir->archives.empty())
    return false;

  std::vector<TestArchive>::iterator
    it = dir->archives.begin(),
    itEnd = dir->archives.end();

  for (; it != itEnd; ++it) {
    if (it->getName() == path.getFileName()) {
      current_archive_ = &(*it);
      return true;
    }
  }

  return false;
}

void TestArchiver::close() {
  current_archive_ = 0;
}

std::vector<fs::FilePath> TestArchiver::getFileList(bool files_only) {
  std::vector<fs::FilePath> result;
  if (current_archive_)
    utils::collectKeys(current_archive_->getContent(), std::back_inserter(result));
  return result;
}

tools::ByteArray TestArchiver::getFile(const fs::FilePath& file_in_archive, size_t max_size) {
  tools::ByteArray result;

  if (current_archive_) {
    TestArchive::Content::iterator it = current_archive_->getContent().find(file_in_archive);
    if (it != current_archive_->getContent().end())
      result = it->second;
  }

  return result;
}
}
