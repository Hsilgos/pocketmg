#include "filepath.h"

#include "defines.h"

#include <algorithm>
#include <ostream>
#include <limits>

namespace {
#ifdef WIN32
static char DirSelector = '\\';
#else
static char DirSelector = '/';
#endif
}

namespace fs {
void FilePath::setGlobalSelector(const char selector) {
  DirSelector = selector;
}

FilePath::FilePath()
  : is_file_(false), first_level_(0) {}

FilePath::FilePath(const std::string& path, bool is_file)
  : is_file_(false), first_level_(0) {
  set(path, is_file);
}

void FilePath::set(const std::string& path, bool is_file) {
  path_ = path;
  is_file_ = is_file;

  parse();
}

void FilePath::clear() {
  path_.clear();
  is_file_ = false;
  entries_.clear();
  first_level_ = 0;
}

// returns maximum level
size_t FilePath::getLevel() const {
  return entries_.size() - first_level_;
}

size_t FilePath::getDirLevel() const {
  return getLevel() - (isDirectory() ? 0 : 1);
}

void FilePath::setFirstLevel(size_t first_level) {
  first_level_ = first_level;
}

int FilePath::correctLevel(size_t level) const {
  return first_level_ + level;
}

// returns 'true' if entry at level level is directory name
bool FilePath::isDirectory(size_t level) const {
  return (LastLevel == correctLevel(level) || (getLevel() - 1) == correctLevel(level)) ? (!is_file_) : true;
}

// return name of entry at level level
std::string FilePath::getName(size_t level) const {
  if (LastLevel == level)
    return entries_.empty() ? utils::EmptyString : entries_.back();

  const int corrected = correctLevel(level);
  const bool good_level = corrected >= 0 && static_cast<size_t>(corrected) < entries_.size();

  return good_level ? entries_[corrected] : utils::EmptyString;
}

// returns whole path
const std::string& FilePath::getPath() const {
  return path_;
}

bool FilePath::empty() const {
  return entries_.empty();
}

void FilePath::parse() {
  //std::string::size_type pos = path_.find_first_of("/\\");
  std::string::size_type pos = 0;
  std::string::size_type prev_pos = path_.find_first_not_of("/\\");
  if (std::string::npos == prev_pos)
    return;

  while (pos != std::string::npos) {
    pos = path_.find_first_of("/\\", pos + 1);

    std::string entry = path_.substr(prev_pos, pos - prev_pos);
    if (!entry.empty() && entry != ".") {
      entries_.push_back(entry);
    }

    prev_pos = pos + 1;
  }
}

std::string FilePath::getExtension() const {
  if (isDirectory())
    return utils::EmptyString;

  std::string file_name = getFileName();
  const std::string::size_type pos = file_name.find_last_of('.');
  if (std::string::npos == pos)
    return utils::EmptyString;

  return file_name.substr(pos + 1);
}

std::string FilePath::getFileName() const {
  if (isDirectory() || entries_.empty())
    return utils::EmptyString;

  return entries_.back();
}

const std::string& FilePath::getLastEntry() const {
  return entries_.empty() ? utils::EmptyString : entries_.back();
}

void FilePath::rebuildPath() {
  std::vector<std::string>::const_iterator it = entries_.begin(), itEnd = entries_.end();

  path_.clear();

  for (; it != itEnd; ++it) {
    path_ += DirSelector;
    path_ += *it;
  }

  if (path_.empty())
    path_ = DirSelector;
}

void FilePath::addFolder(const std::string& name) {
  if (!name.empty()) {
    entries_.push_back(name);

    if (path_.empty() || path_[path_.size() - 1] != DirSelector)
      path_ += DirSelector;

    rebuildPath(); //path_ += name;
  }
}

void FilePath::removeLastEntry() {
  if (!entries_.empty())
    entries_.erase(entries_.end() - 1);

  is_file_ = false;

  rebuildPath();
}

bool FilePath::isHidden() const {
  const std::string& entry = getLastEntry();

  return !entry.empty() && entry[0] == '.';
}

bool FilePath::isBack() const {
  const std::string& entry = getLastEntry();

  return isDirectory() && entry.size() == 2 && entry[0] == '.' && entry[1] == '.';
}

bool FilePath::startsWith(const FilePath& other, bool cmp_from_back) const {
  if (entries_.size() < other.entries_.size())
    return false;

  return doCompare(other, other.entries_.size(), cmp_from_back);
}

bool FilePath::startsWith(const FilePath& other, size_t level, bool cmp_from_back) const {
  if (entries_.size() < level || other.entries_.size() < level)
    return false;

  return doCompare(other, level, cmp_from_back);
}

bool FilePath::equals(const FilePath& other, bool cmp_from_back) const {
  if (entries_.size() != other.entries_.size())
    return false;

  return doCompare(other, entries_.size(), cmp_from_back);
}

bool FilePath::operator ==(const FilePath& other) const {
  return equals(other, false);
}

bool FilePath::operator !=(const FilePath& second) const {
  return !(second == *this);
}

bool FilePath::operator <(const FilePath& other) const {
  for (size_t i = 0; i < std::min(entries_.size(), other.entries_.size()); ++i) {
    if (entries_[i] != other.entries_[i])
      return entries_[i] < other.entries_[i];
  }

  return entries_.size() < other.entries_.size();
}

bool FilePath::doCompare(const FilePath& other, int level, bool cmp_from_back) const {
  if (cmp_from_back) {
    const size_t r_begin1 = entries_.size() - level;
    const size_t r_begin2 = other.entries_.size() - level;

    return std::equal(
             entries_.rbegin() + r_begin1,
             entries_.rbegin() + r_begin1 + level,
             other.entries_.rbegin() + r_begin2);
  }

  return std::equal(
           entries_.begin(),
           entries_.begin() + level,
           other.entries_.begin());
}

void FilePath::setFile(const std::string& name) {
  addFolder(name);
  is_file_ = true;
}

const size_t FilePath::LastLevel = std::numeric_limits<size_t>::max();

FilePath FilePath::copy(size_t level) const {
  FilePath result;
  if (level >= getLevel()) {
    result = *this;
  } else {
    result.entries_.insert(
      result.entries_.begin(),
      entries_.begin(),
      entries_.begin() + level + 1);

    result.rebuildPath();
  }

  return result;
}

bool HaveSameDirectory(const FilePath& first, const FilePath& second) {
  const size_t first_dir_level = first.getDirLevel();
  const size_t second_dir_level = second.getDirLevel();
  if (first_dir_level != second_dir_level)
    return false;

  return first.startsWith(second, first_dir_level);
}

std::ostream& operator <<(std::ostream& stream, const fs::FilePath& path) {
  stream << (path.isDirectory() ? "[D]" : "[F]") << path.getPath();
  return stream;
}

FilePath CopyDirectory(const FilePath& path) {
  return path.copy(path.getDirLevel());
}

FilePath FindCommonPath(const FilePath& first, const FilePath& second) {
  const size_t min_level = std::min(first.getLevel(), second.getLevel());
  for (size_t i = 0; i < min_level; ++i) {
    if (first.getName(i) != second.getName(i))
      return i > 0 ? first.copy(i - 1) : FilePath();
  }

  return first.copy(min_level - 1);
}
}
