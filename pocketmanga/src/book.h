#pragma once

#include <string>
#include <memory>
#include <vector>
#include <ostream>

#include "filepath.h"
#include "byteArray.h"
#include "filemanager.h"
#include "iArchive.h"
#include "image.h"

namespace img {
class Image;
}

namespace manga {
void FixUpFileTreeForTest(std::vector<fs::FilePath>& files, const fs::FilePath& root);

struct PathToFile {
  // First is path to archive
  // Second is path in archive

  fs::FilePath filePath;
  fs::FilePath pathInArchive;

  bool operator ==(const PathToFile& other) const {
    return filePath == other.filePath &&
           pathInArchive == other.pathInArchive;
  }

  bool operator !=(const PathToFile& other) const {
    return !(*this == other);
  }

  bool empty() const {
    return filePath.empty() && pathInArchive.empty();
  }

  PathToFile() {}

  PathToFile(const fs::FilePath& file_path)
    : filePath(file_path) {}

  PathToFile(const fs::FilePath& file_path, const fs::FilePath& file_in_arch)
    : filePath(file_path), pathInArchive(file_in_arch) {}

  PathToFile containingFolder() const {
    PathToFile result;
    return result;
  }
};

inline std::ostream& operator<<(std::ostream& stream, const PathToFile& path) {
  stream << path.filePath;
  if (!path.pathInArchive.empty())
    stream << " [" <<  path.pathInArchive << "]";
  return stream;
}

class BookExplorer {
public:
  BookExplorer(fs::IFileManager* file_mgr,
               fs::IFileManager::EntryTypes types = fs::IFileManager::FileAndDirectory);

  typedef std::vector<fs::FilePath> FileList;

  // Returns file list ascending sorted
  std::vector<PathToFile> fileList() const;

  bool setRoot(const fs::FilePath& root);
  const fs::FilePath& getRoot() const;

  // Enters to some directory or archive
  bool enter(const PathToFile& path);
  bool back();

  bool toNextFile();
  bool toPreviousFile();

  bool toFirstFile();
  bool toLastFile();

  PathToFile getCurrentPos() const;
  tools::ByteArray readCurrentFile() const;

private:
  BookExplorer(const BookExplorer&);
  BookExplorer& operator =(const BookExplorer&);

  bool openArchive(const fs::FilePath& path, bool to_beginning);
  void closeArchive();
  bool isCurrentInArchiveFile() const;
  //bool FindFirstSuitable()
  std::auto_ptr<fs::IFileManager> file_mgr_;

  fs::IFileManager::EntryTypes find_entries_;

  fs::FilePath root_;
  std::auto_ptr<archive::IArchive> current_archive_;

  std::vector<fs::FilePath> files_;
  std::vector<fs::FilePath> files_in_archive_;

  struct FileIndex {
    size_t currentFile;
    FileIndex()
      : currentFile(0) {}
  };

  FileIndex fs_;
  FileIndex archive_;
};

struct Bookmark {
  PathToFile currentFile;
  fs::FilePath rootDir;
};

class IBookCache {
public:
  virtual ~IBookCache();

  virtual IBookCache* clone() = 0;
  virtual void swap(IBookCache* other) = 0;
  virtual bool onLoaded(img::Image& image) = 0;
  //virtual Cache getCached(size_t id) const = 0;
};

class Book {
public:
  Book();
  explicit Book(fs::IFileManager* file_mgr);
  // Looks for next image
  bool incrementPosition();
  // Looks for previous image
  bool decrementPosition();

  void preload();

  // All about current image
  img::Image currentImage() const;
  PathToFile currentPath() const;
  bool hasCurrentImage() const;
  IBookCache* currentCache() const;

  bool setRoot(const fs::FilePath& root);

  bool toFirstFile();
  bool toLastFile();

  void setCachePrototype(IBookCache* cache);

  // Set/Get bookmark
  Bookmark bookmark() const;
  bool goToBookmark(const Bookmark& bookmark);
private:
  Book(const Book&);
  Book& operator =(const Book&);

  struct ImageData {
    img::Image image;
    std::auto_ptr<IBookCache> cache;
    Bookmark bookmark;

    void swap(ImageData& other);
    bool empty() const;
    void clear();
  };

  bool findAndLoadPrevious();
  bool findAndLoadNext();

  bool loadFromExplorerInto(ImageData& data);

  ImageData previous_;
  ImageData current_;
  ImageData next_;

  BookExplorer explorer_;
};
}
