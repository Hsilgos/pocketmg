#include "book.h"

#include "filemanager.h"

#include "iArchive.h"

#include "defines.h"

#include <algorithm>
#include <assert.h>

// Recursively find all subdirectories and folders
// find appropriate archiver, open archive, get filelist from archive
// build correct filelist from archive
// decompress file and return decompressed data.
// go to next file.

// random access

namespace {
const int MaxFilesize   = 1024 * 1024 * 20;
const size_t ArchiveJustOpened = std::numeric_limits<size_t>::max();
const size_t FileNotSpecified = std::numeric_limits<size_t>::max();

struct CmpForLevel {
  const fs::FilePath &entry;
  const size_t level;
  CmpForLevel(const fs::FilePath &entry, size_t level)
    :entry(entry), level(level) {
  }

  CmpForLevel(const CmpForLevel &other)
    :entry(other.entry), level(other.level) {

  }

  bool operator ()(const fs::FilePath &first) {
    return first.startsWith(entry, level, true);
  }
private:

  CmpForLevel &operator = (const CmpForLevel &);
};

bool FindEntryInList(
  const std::vector<fs::FilePath> &files,
  const fs::FilePath &entry,
  //size_t &first_file_inDir,
  size_t &file_position) {
  const size_t max_level = entry.getDirLevel();
  std::vector<fs::FilePath>::const_iterator itFirst =
    std::find_if(files.begin(), files.end(), CmpForLevel(entry, max_level));

  if( files.end() == itFirst )
    return false;

  if(entry.isDirectory()) {
    file_position = itFirst - files.begin();
  } else {
    std::vector<fs::FilePath>::const_iterator itCurrent =
      std::find_if(
        itFirst,
        files.end(),
        CmpForLevel(entry, entry.getLevel()));

    if(files.end() == itCurrent)
      return false;

    file_position = itCurrent - files.begin();
  }

  //first_file_inDir = itFirst - files.begin();
  return true;
}

size_t FindFirstFileFrom(const std::vector<fs::FilePath> &files, size_t current_file) {
  assert(current_file < files.size());
  const fs::FilePath &current = files[current_file];
  for( size_t i = current_file; i > 0; --i ) {
    if( !HaveSameDirectory(files[i], current) )
      return i + 1;
  }

  return HaveSameDirectory(files[0], current) ? 0 : 1;
}

/*
/folder1/file1
/folder1/folder2/file1

/folder1/
/folder1/folder2/file1

/folder3/file1
/folder2/file2

/folder3
/folder2/file1

/folder1/folder2/folder3
/folder1/folder4
*/

template<class Cmp>
void FixUpFileTree(std::vector<fs::FilePath> &files, const fs::FilePath &root, Cmp sort_way, bool files_only) {
  if( files.empty() )
    return;

  std::sort(files.begin(), files.end(), sort_way);

  if(files_only) {

  } else {
    fs::FilePath prev_dir = CopyDirectory(root);
    std::vector<fs::FilePath> extras;

    if(files[0] != root)
      extras.push_back(root);

    for( size_t i = 0; i < files.size(); ++i ) {
      fs::FilePath current_dir = CopyDirectory(files[i]);
      if( prev_dir.equals(current_dir, true) )
        continue;

      if( current_dir.getLevel() < prev_dir.getLevel() &&
          prev_dir.startsWith(current_dir, true)) {
        // /folder1/folder2/folder3/folder4
        // /folder1/folder2
        prev_dir = current_dir;
        continue;
      }

      if( (current_dir.getLevel() == prev_dir.getLevel() + 1) &&
          current_dir.startsWith(prev_dir, true) &&
          files[i].isDirectory()) {
        // /folder1/
        // /folder1/folder2
        prev_dir = current_dir;
        continue;
      }

      fs::FilePath common = FindCommonPath(prev_dir, current_dir);
      const size_t common_level = common.getLevel();
      const size_t copy_till = current_dir.getLevel() -  1;
      for( size_t i = common_level; i < copy_till; ++i ) {
        extras.push_back(current_dir.copy(i));
      }
      prev_dir = current_dir;
    }

    if( !extras.empty() ) {
      std::vector<fs::FilePath> result;
      result.reserve(files.size() + extras.size());
      std::merge(
        files.begin(), files.end(),
        extras.begin(), extras.end(),
        std::back_inserter(result),
        sort_way);

      files.swap(result);
    }
  }
}
}

namespace manga {
void FixUpFileTreeForTest(std::vector<fs::FilePath> &files, const fs::FilePath &root) {
  FixUpFileTree(files, root, fs::WordNumberOrder(), false);
}

//////////////////////////////////////////////////////////////////////////

BookExplorer::BookExplorer(fs::IFileManager *file_mgr, fs::IFileManager::EntryTypes types)
  :file_mgr_(file_mgr), find_entries_(types) {
}

// Returns filelist ascending sorted
std::vector<PathToFile> BookExplorer::fileList() const {
  std::vector<PathToFile> result;
  PathToFile curr_pos = getCurrentPos();
  if( curr_pos.empty() )
    return result;

  if( current_archive_.get() ) {
    const size_t dir_level = curr_pos.pathInArchive.getDirLevel();

    const size_t first = FindFirstFileFrom(files_in_archive_, archive_.currentFile);

    for( size_t i = first; i < files_in_archive_.size(); ++i ) {
      if( !files_in_archive_[i].startsWith(curr_pos.pathInArchive, dir_level, true) )
        break;

      if( files_in_archive_[i].getLevel() > dir_level ) {
        if( result.empty() || !files_in_archive_[i].startsWith(result.back().pathInArchive, true) ) {
          result.push_back(PathToFile(files_[fs_.currentFile], files_in_archive_[i].copy(dir_level)));
        }
      }
    }
  } else {
    const size_t dir_level = curr_pos.filePath.getDirLevel();

    const size_t first = FindFirstFileFrom(files_, fs_.currentFile);

    for( size_t i = first; i < files_.size(); ++i ) {
      if( !files_[i].startsWith(curr_pos.filePath, dir_level, true) )
        break;

      if( files_[i].getLevel() > dir_level ) {
        if( result.empty() || !files_[i].startsWith(result.back().filePath, true) ) {
          result.push_back(PathToFile(files_[i].copy(dir_level)));
        }
      }
    }

  }
  return result;
}

bool BookExplorer::setRoot(const fs::FilePath &root) {
  root_ = root;
  files_ = file_mgr_->getFileList(root, find_entries_, true);

  const bool files_only = (fs::IFileManager::Directory != (find_entries_ & fs::IFileManager::Directory));
  FixUpFileTree(files_, root, fs::WordNumberOrder(), files_only);
  //if( std::find(files_.begin(), files_.end(), root) == files_.end() )
  // files_.push_back(root);


  return enter(PathToFile(root));
}

const fs::FilePath &BookExplorer::getRoot() const {
  return root_;
}

bool BookExplorer::openArchive(const fs::FilePath &path, bool to_beginning) {
  current_archive_.reset(archive::recognize(path));

  if (!current_archive_.get())
    return false;

  const bool files_only = (fs::IFileManager::Directory != (find_entries_ & fs::IFileManager::Directory));
  files_in_archive_ = current_archive_->getFileList(files_only);

  FixUpFileTree(files_in_archive_, fs::FilePath(), fs::WordNumberOrder(), files_only);

  archive_.currentFile = to_beginning ? 0 : files_in_archive_.size() - 1;

  return true;
}

bool BookExplorer::isCurrentInArchiveFile() const {
  return
    archive_.currentFile < files_in_archive_.size() &&
    !files_in_archive_[archive_.currentFile].isDirectory();
}

void BookExplorer::closeArchive() {
  current_archive_.reset();
  files_in_archive_.clear();
  archive_.currentFile = 0;
}

// Enters to some directory or archive
bool BookExplorer::enter(const PathToFile &path) {
  if( root_.empty() || path.empty() )
    return false;

  // Already on the place?
  PathToFile current_pos = getCurrentPos();
  if( current_pos == path )
    return true;

  if( path.filePath.isDirectory() ) {
    // it's directory...
    if( current_pos.filePath != path.filePath ) {
      if( !FindEntryInList(
            files_,
            path.filePath,
            //fs_.firstInCurrDir,
            fs_.currentFile) ) {
        return false;
      }

      // close previous opened archive
      closeArchive();
      return true;
    }
  } else {
    // probably archive?
    if( current_pos.filePath != path.filePath || !current_archive_.get() ) {
      if( !FindEntryInList(
            files_,
            path.filePath,
            fs_.currentFile) ) {
        return false;
      }

      if( !openArchive(path.filePath, true) ) {
        // Not an archive
        if( !path.pathInArchive.empty() )
          return false;

        // close previous archive
        closeArchive();
        return true;
      }
    } else if( path.pathInArchive.empty() ) {
      if( current_archive_.get() ) {
        if(!FindEntryInList(
              files_in_archive_,
              path.pathInArchive,
              archive_.currentFile)) {
          return false;
        }
      }
      return true;
    } else if ( !current_archive_.get() ) {
      return false;
    }

    if(!FindEntryInList(
          files_in_archive_,
          path.pathInArchive,
          archive_.currentFile)) {
      return false;
    }

    return true;
  }

  return false;
}

bool BookExplorer::back() {
  PathToFile new_path = getCurrentPos();
  if(!new_path.pathInArchive.empty())
    new_path.pathInArchive.removeLastEntry();
  else
    new_path.filePath.removeLastEntry();

  return enter(new_path);
  //return false;
}

bool BookExplorer::toNextFile() {
  for(;;) {
    if( current_archive_.get() ) {
      if(files_in_archive_.empty()) {
        closeArchive();
        continue;
      }

      //assert(archive_.firstInCurrDir < files_in_archive_.size());
      if( ++archive_.currentFile >= files_in_archive_.size() ) {
        closeArchive();
        continue;
      }

      const fs::FilePath &curr_entry = files_in_archive_[archive_.currentFile];

      if( curr_entry.isDirectory() )
        continue;

      return true;
    } else {
      if( files_.empty() )
        return false;

      if( fs_.currentFile + 1 >= files_.size() )
        return false;

      ++fs_.currentFile;
      const fs::FilePath &curr_entry = files_[fs_.currentFile];

      if( curr_entry.isDirectory() )
        continue;

      // archive?
      if( openArchive(curr_entry, true) ) {
        if( isCurrentInArchiveFile() )
          return true;

        continue;
      }

      return true;
    }
  }
}

bool BookExplorer::toPreviousFile() {
  if( files_.empty() )
    return false;

  // store position

  for(;;) {
    if( current_archive_.get() ) {
      if( files_in_archive_.empty() ) {
        closeArchive();
        continue;
      }

      //assert(archive_.firstInCurrDir < files_in_archive_.size());
      if( 0 == archive_.currentFile ) {
        closeArchive();
        continue;
      }

      --archive_.currentFile;

      const fs::FilePath &curr_entry = files_in_archive_[archive_.currentFile];

      if( curr_entry.isDirectory() )
        continue;

      return true;
    } else {
      if( 0 == fs_.currentFile ) {
        //restore position
        return false;
      }

      --fs_.currentFile;
      const fs::FilePath &curr_entry = files_[fs_.currentFile];

      if( curr_entry.isDirectory() )
        continue;

      // archive?
      if( openArchive(curr_entry, false) ) {
        if( isCurrentInArchiveFile() )
          return true;

        continue;
      }

      return true;
    }
  }
}

bool BookExplorer::toFirstFile() {
  closeArchive();
  fs_.currentFile = 0;
  //fs_.firstInCurrDir = 0;
  if( files_.empty() )
    return false;

  const fs::FilePath &curr_entry = files_[fs_.currentFile];

  if( curr_entry.isDirectory() )
    return toNextFile();

  if( openArchive(curr_entry, true)  &&
      !isCurrentInArchiveFile() )
    return toNextFile();

  return true;
}

bool BookExplorer::toLastFile() {
  closeArchive();
  if( files_.empty() )
    return false;

  fs_.currentFile = files_.size() - 1;

  const fs::FilePath &curr_entry = files_[fs_.currentFile];
  if( curr_entry.isDirectory() )
    return toPreviousFile();

  if( openArchive(curr_entry, false)  &&
      !isCurrentInArchiveFile() )
    return toPreviousFile();

  return true;
}

PathToFile BookExplorer::getCurrentPos() const {
  PathToFile result;

  if( !files_.empty() ) {
    assert(fs_.currentFile < files_.size());
    result.filePath = files_[fs_.currentFile];
  }

  if( current_archive_.get() && !files_in_archive_.empty() ) {
    assert(archive_.currentFile < files_in_archive_.size());
    result.pathInArchive = files_in_archive_[archive_.currentFile];
  }

  return result;
}

tools::ByteArray BookExplorer::readCurrentFile() const {
  if( current_archive_.get() ) {
    if( !files_in_archive_.empty() &&
        archive_.currentFile < files_in_archive_.size() &&
        !files_in_archive_[archive_.currentFile].isDirectory() ) {
      return current_archive_->getFile(files_in_archive_[archive_.currentFile], MaxFilesize);
    }

    return tools::ByteArray::empty;
  }

  if( !files_.empty() &&
      fs_.currentFile < files_.size() &&
      !files_[fs_.currentFile].isDirectory() ) {
    return file_mgr_->readFile(files_[fs_.currentFile], MaxFilesize);
  }

  return tools::ByteArray::empty;
}

//////////////////////////////////////////////////////////////////////////
IBookCache::~IBookCache() {
}

Book::Book()
  :explorer_(fs::IFileManager::create(), fs::IFileManager::File) {
}

Book::Book(fs::IFileManager *file_mgr)
  :explorer_(file_mgr, fs::IFileManager::File) {
}

bool Book::setRoot(const fs::FilePath &root) {
  return explorer_.setRoot(root);
}

bool Book::toFirstFile() {
  if( !explorer_.toFirstFile() )
    return false;

  if( loadFromExplorerInto(current_) )
    return true;

  return findAndLoadNext();
}

bool Book::toLastFile() {
  if( !explorer_.toLastFile() )
    return false;

  if( loadFromExplorerInto(previous_) )
    return true;

  return findAndLoadPrevious();
}

bool Book::goToBookmark(const Bookmark &bookmark) {
  return
    explorer_.setRoot(bookmark.rootDir) &&
    explorer_.enter(bookmark.currentFile);
}

Bookmark Book::bookmark() const {
  Bookmark result;

  result.rootDir = explorer_.getRoot();
  result.currentFile = explorer_.getCurrentPos();

  return result;
}

void Book::ImageData::swap(ImageData &other) {
  image.swap(other.image);
  std::swap(bookmark.currentFile, other.bookmark.currentFile);
  std::swap(bookmark.rootDir, other.bookmark.rootDir);

  if( cache.get() && other.cache.get() ) {
    cache->swap(other.cache.get());
  }
}

bool Book::ImageData::empty() const {
  return image.empty();
}

void Book::ImageData::clear() {
  image.destroy();
  bookmark.currentFile = PathToFile();
  bookmark.rootDir.clear();
}

bool Book::incrementPosition() {
  if( current_.empty() )
    return toFirstFile();

  if( next_.empty() ) {
    if( !explorer_.enter(current_.bookmark.currentFile) ||
        !findAndLoadNext() )
      return false;
  }

  if( !next_.empty() ) {
    previous_.swap(current_);
    current_.swap(next_);

    next_.clear();
    return true;
  }

  return false;
}

bool Book::decrementPosition() {
  if( current_.empty() )
    return toLastFile();

  if( previous_.empty() ) {
    if( !explorer_.enter(current_.bookmark.currentFile) ||
        !findAndLoadPrevious())
      return false;
  }

  if( !previous_.empty() ) {
    next_.swap(current_);
    current_.swap(previous_);

    previous_.clear();
    return true;
  }

  return false;
}

bool Book::findAndLoadPrevious() {
  while( explorer_.toPreviousFile() ) {
    if(loadFromExplorerInto(previous_))
      return true;
  }

  return false;
}

bool Book::findAndLoadNext() {
  // find first suitable file
  while( explorer_.toNextFile() ) {
    if(loadFromExplorerInto(next_))
      return true;
  }

  return false;
}

bool Book::loadFromExplorerInto(ImageData &image_data) {
  PathToFile path = explorer_.getCurrentPos();

  const fs::FilePath &file =
    path.pathInArchive.empty() ? path.filePath : path.pathInArchive;

  tools::ByteArray data = explorer_.readCurrentFile();
  if( data.isEmpty() )
    return false;

  if (image_data.image.load(file.getExtension(), data)) {
    image_data.bookmark.currentFile = explorer_.getCurrentPos();
    if (image_data.cache.get())
      image_data.cache->onLoaded(image_data.image);

    return true;
  }

  return false;
}

void Book::preload() {
  if( current_.empty() )
    toFirstFile();

  if( !next_.empty() && !previous_.empty() )
    return;

  if( !current_.empty() ) {
    Bookmark curr_b_m = current_.bookmark;

    if( previous_.empty() ) {
      if( explorer_.enter(curr_b_m.currentFile) )
        findAndLoadPrevious();
    }

    if( next_.empty() ) {
      if( explorer_.enter(curr_b_m.currentFile) )
        findAndLoadNext();
    }
  }
}

img::Image Book::currentImage() const {
  return current_.image;
}

IBookCache *Book::currentCache() const {
  return current_.cache.get();
}

void Book::setCachePrototype(IBookCache *cache) {
  if( cache ) {
    previous_.cache.reset(cache->clone());
    current_.cache.reset(cache);
    next_.cache.reset(cache->clone());
  } else {
    previous_.cache.reset();
    current_.cache.reset();
    next_.cache.reset();
  }
}
}
