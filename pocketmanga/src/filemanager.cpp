#include "filemanager.h"

#include <algorithm>
#include <fstream>

#include "filepath.h"

#include "byteArray.h"

#include "algorithms_ex.h"

namespace {
inline int fetchNumber(const std::string &name, size_t &pos) {
  if( !isdigit(name[pos]) )
    return 0;

  const std::string::size_type num_end = name.find_first_not_of("0123456789", pos);
  const std::string num_str = name.substr(pos, num_end - pos);
  const int number = atoi(num_str.c_str());
  pos = (std::string::npos == num_end) ? name.size() : num_end;

  return number;
}

inline std::string getFileNameCI(const fs::FilePath &first, int level) {
  std::string name = first.getName(level);
  std::transform(name.begin(), name.end(), name.begin(), tolower);
  return name;
}
}

namespace fs {
// build correct list of entries in each sublevel

bool Comparator::operator ()(const fs::FilePath &first, const fs::FilePath &second) const {
  bool result = false;

  for( size_t level = 0; level < std::min(first.getLevel(), second.getLevel()); ++level ) {
    if(first.isDirectory(level) != second.isDirectory(level))
      return first.isDirectory(level);

    std::string name1 = getFileNameCI(first, level);
    std::string name2 = getFileNameCI(second, level);

    if( compareLevel(name1, name2, result) )
      return result;
    // words are equal, go to the next level
  }

  if( first.getLevel() != second.getLevel() )
    return first.getLevel() < second.getLevel();

  if( first.isDirectory() != second.isDirectory() )
    return first.isDirectory();

  // both are equal
  return false;
}

Comparator::~Comparator() {
}

bool WordNumberOrder::compareLevel(const std::string &name1, const std::string &name2, bool &result) const {
  size_t i = 0, j = 0;
  while( i < name1.size() && j < name2.size() ) {
    const bool num_is_started = isdigit(name1[i]) && isdigit(name2[j]);
    if( num_is_started || (name1[i] != name2[j]) ) {
      if( num_is_started ) {
        // we met two numbers.
        // so now we have to compare numbers...
        const int number1 = fetchNumber(name1, i);
        const int number2 = fetchNumber(name2, j);

        if( number1 != number2 ) {
          result = number1 < number2;
          return true;
        }

        continue;
      } else {
        result = name1[i] < name2[j];
        return true;
      }
    }

    ++i;
    ++j;
  }

  if( name1.size() != i || name2.size() != j ) {
    result = name1.size() < name2.size();
    return true;
  }

  return false;
}


bool NumberOrder::compareLevel(const std::string &name1, const std::string &name2, bool &result) const {
  static const char numbers[] = "1234567890";
  std::string::size_type num1_begin = name1.find_first_of(numbers);
  std::string::size_type num2_begin = name2.find_first_of(numbers);

  while( std::string::npos != num1_begin && std::string::npos != num2_begin ) {
    const int number1 = fetchNumber(name1, num1_begin);
    const int number2 = fetchNumber(name2, num2_begin);

    if( number1 != number2 ) {
      result = number1 < number2;
      return true;
    }

    if( std::string::npos != num1_begin )
      num1_begin = name1.find_first_of(numbers, num1_begin);

    if( std::string::npos != num2_begin )
      num2_begin = name2.find_first_of(numbers, num2_begin);
  }

  if( name1 != name2 ) {
    result = name1 < name2;
    return true;
  }

  return false;
}

tools::ByteArray IFileManager::readFile(const fs::FilePath &file_path, size_t max_size) {
  tools::ByteArray data;

  std::ifstream file(file_path.getPath().c_str(), std::ios::binary);
  const size_t size = utils::get_stream_size(file);

  if( size < max_size )
    file >> data;

  return data;
}

IFileManager::~IFileManager() {

}


//////////////////////////////////////////////////////////////////////////
}
