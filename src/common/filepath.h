#pragma once

#include <string>
#include <vector>

namespace fs {
class FilePath {
  std::string path_;
  bool is_file_;

  typedef std::vector<std::string> StringVector;
  std::vector<std::string> entries_;

  size_t first_level_;

  void parse();
  void rebuildPath();

  bool doCompare(const FilePath& other, int level, bool cmp_from_back) const;

  int correctLevel(size_t level) const;
public:
  static const size_t LastLevel;

  FilePath();
  FilePath(const std::string& path, bool is_file);

  bool equals(const FilePath& other, bool cmp_from_back = false) const;

  bool operator ==(const FilePath& second) const;
  bool operator !=(const FilePath& second) const;

  bool operator <(const FilePath& second) const;

  void set(const std::string& path, bool is_file);
  void clear();

  // returns maximum level
  size_t getLevel() const;
  size_t getDirLevel() const;

  // sets level which will be first of algorithms.
  // For example for 'sorting'
  void setFirstLevel(size_t first_level);

  // returns 'true' if entry at level level is directory name
  bool isDirectory(size_t level = LastLevel) const;

  // return name of entry at level level
  std::string getName(size_t level = LastLevel) const;

  // returns whole path
  const std::string& getPath() const;

  // returns extension of file, i.e. 'txt' for readme.txt
  // returns empty string if it's directory or file doesn't have extension
  std::string getExtension() const;

  std::string getFileName() const;
  const std::string& getLastEntry() const;

  void addFolder(const std::string& name);
  void removeLastEntry();

  // .filename
  bool isHidden() const;

  // ..
  bool isBack() const;

  bool empty() const;

  //
  bool startsWith(const FilePath& other, bool cmp_from_back = false) const;
  bool startsWith(const FilePath& other, size_t level, bool cmp_from_back = false) const;

  void setFile(const std::string& name);

  FilePath copy(size_t level = LastLevel) const;

  static void setGlobalSelector(const char selector);
};

bool HaveSameDirectory(const FilePath& first, const FilePath& second);
FilePath CopyDirectory(const FilePath& path);
FilePath FindCommonPath(const FilePath& first, const FilePath& second);

std::ostream& operator<<(std::ostream& stream, const fs::FilePath& path);
//std::istream& operator>> (std::istream& stream, ByteArray& array);
}
