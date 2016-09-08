#include "filemanager.h"

#ifdef WIN32

#include "filepath.h"
#include <Windows.h>

namespace fs {
namespace w32 {
class FileManager32 : public IFileManager {
  void appendFileName(std::string& parent, const std::string& file) {
    if (parent.empty())
      parent = file;

    if (parent[parent.size() - 1] != '\\' && parent[parent.size() - 1] != '/')
      parent.append("\\");

    parent.append(file);
  }

  void doGetFileList(std::vector<fs::FilePath>& list, const std::string& root, EntryTypes entries, bool recursive) {
    HANDLE hFind = INVALID_HANDLE_VALUE;

    WIN32_FIND_DATAA ffd = {0};

    std::string find_mask = root;
    appendFileName(find_mask, "*");
    hFind = FindFirstFileA(find_mask.c_str(), &ffd);
    if (INVALID_HANDLE_VALUE == hFind)
      return;

    do {
      const bool is_dir = (FILE_ATTRIBUTE_DIRECTORY == (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY));

      std::string file_name = ffd.cFileName;
      if (file_name == "." || file_name == "..")
        continue;

      std::string full_path = root;
      appendFileName(full_path, ffd.cFileName);
      if (is_dir)
        full_path += "\\";

      fs::FilePath path(full_path, !is_dir);

      if ((entries & IFileManager::Directory) && is_dir)
        list.push_back(path);

      if ((entries & IFileManager::File) && !is_dir)
        list.push_back(path);

      if (recursive && is_dir)
        doGetFileList(list, path.getPath(), entries, recursive);
    } while (FindNextFileA(hFind, &ffd) != 0);
  }

  virtual std::vector<fs::FilePath> getFileList(const fs::FilePath& root, EntryTypes entries, bool recursive) {
    std::vector<fs::FilePath> result;

    doGetFileList(result, root.getPath(), entries, recursive);

    return result;
  }
};
}


IFileManager* IFileManager::create() {
  return new w32::FileManager32;
}
}

#endif
