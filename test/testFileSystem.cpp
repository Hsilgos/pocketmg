#include "testFileSystem.h"

#include "byteArray.h"
#include "filepath.h"

#include <algorithm>


namespace test {
std::vector<fs::FilePath> TestFileSystem::getFileList(const fs::FilePath& root, EntryTypes entries, bool recursive) {
  std::vector<fs::FilePath> result;

  const fs::FilePath path = root;

  if (root_.name != path.getName(0))
    return result;

  Dir* curr_dir = &root_;

  fs::FilePath prefix;
  prefix.addFolder(curr_dir->name);

  for (size_t i = 1; i < path.getLevel(); ++i) {
    if (!hasDir(*curr_dir, path.getName(i)))
      return result;  // empty

    curr_dir = &findOrCreateDir(*curr_dir, path.getName(i));
    prefix.addFolder(curr_dir->name);
  }


  for (size_t i = 0; i < curr_dir->subdirs.size(); ++i) {
    fs::FilePath path = prefix;
    path.addFolder(curr_dir->subdirs[i].name);

    if ((entries & Directory) == Directory)
      result.push_back(path);

    if (recursive) {
      std::vector<fs::FilePath> subfiles = getFileList(path, entries, recursive);
      result.insert(result.end(), subfiles.begin(), subfiles.end());
    }
  }

  if ((entries & File) == File) {
    for (size_t i = 0; i < curr_dir->files.size(); ++i) {
      fs::FilePath path = prefix;
      path.setFile(curr_dir->files[i].name);

      result.push_back(path);
    }
  }

  std::random_shuffle(result.begin(), result.end());

  return result;
}

tools::ByteArray TestFileSystem::readFile(const fs::FilePath& file, size_t max_size) {
  Dir* dir = findDirByPath(file);
  if (!dir)
    return tools::ByteArray::empty;

  std::vector<TestFile>& files = dir->files;
  for (size_t i = 0; i < files.size(); ++i) {
    if (files[i].name == file.getName())
      return files[i].data;
  }

  return tools::ByteArray::empty;
}

struct CmpDir {
  const std::string& name_;
  CmpDir(const std::string& name)
    : name_(name) {}

  bool operator ()(const TestFileSystem::Dir& dir) const {
    return dir.name == name_;
  }
};

TestFileSystem::Dir* TestFileSystem::hasDir(Dir& parent, const std::string& name) {
  std::vector<Dir>::iterator it =
    std::find_if(parent.subdirs.begin(), parent.subdirs.end(), CmpDir(name));

  return (it != parent.subdirs.end()) ? (&(*it)) : 0;
}

TestFileSystem::Dir& TestFileSystem::findOrCreateDir(Dir& parent, const std::string& name) {
  std::vector<Dir>::iterator it =
    std::find_if(parent.subdirs.begin(), parent.subdirs.end(), CmpDir(name));

  if (it != parent.subdirs.end()) {
    return *it;
  }

  Dir new_dir;
  new_dir.name = name;
  parent.subdirs.push_back(new_dir);

  return parent.subdirs.back();
}

TestFileSystem::Dir& TestFileSystem::addEntry(Dir& parent, const fs::FilePath& path, size_t level) {
  if (level >= path.getLevel())
    return parent;

  const std::string name = path.getName(level);
  const bool is_file = !path.isDirectory(level);

  if (!is_file)
    return addEntry(findOrCreateDir(parent, name), path, level + 1);

  return parent;
}

void TestFileSystem::addFile(const std::string& path, const tools::ByteArray& data) {
  fs::FilePath file_path(path, true);

  if (0 == file_path.getLevel())
    return;

  root_.name = file_path.getName(0);
  Dir& last_parent = addEntry(root_, file_path, 1);
  last_parent.files.push_back(TestFile(file_path.getName(), data));
}

void TestFileSystem::addFolder(const std::string& path) {
  fs::FilePath file_path(path, false);

  if (0 == file_path.getLevel())
    return;

  root_.name = file_path.getName(0);
  addEntry(root_, file_path, 1);
}

void TestFileSystem::addArchive(const std::string& path, const TestArchive& acrh) {
  fs::FilePath file_path(path, false);

  if (0 == file_path.getLevel())
    return;

  root_.name = file_path.getName(0);
  Dir& last_parent = addEntry(root_, file_path, 1);
  last_parent.archives.push_back(acrh);
  last_parent.files.push_back(TestFile(acrh.getName(), tools::ByteArray::empty));
}

TestFileSystem::Dir* TestFileSystem::findDirByPath(const std::string& path, bool is_file) {
  fs::FilePath file_path(path, is_file);
  return findDirByPath(file_path);
}

TestFileSystem::Dir* TestFileSystem::findDirByPath(const fs::FilePath& path) {
  fs::FilePath file_path = path;
  if (!file_path.isDirectory())
    file_path.removeLastEntry();

  if (0 == file_path.getLevel())
    return false;

  Dir* curr_dir = &root_;

  for (size_t i = 1; i < file_path.getLevel(); ++i) {
    curr_dir = hasDir(*curr_dir, file_path.getName(i));
    if (!curr_dir)
      return 0;
  }

  return curr_dir;
}
}
