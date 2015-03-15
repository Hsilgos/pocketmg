#pragma once

#include <vector>
#include <numeric>
#include <memory.h>
#include <limits>
#include <string>

#ifdef max
#undef max
#endif

/*
 ByteArray has semantic of smart pointer. Data will not be copied in operator = and
 in copy constructor. Reference will be incremented by one. When object is destroyed
 reference is decremented by one. When references is 0 then buffer will be deleted.
*/

namespace tools {
class AtomicInt {
  volatile int value_;
public:
  AtomicInt(int init_value = 0);

  AtomicInt(const AtomicInt &other);
  AtomicInt &operator = (const AtomicInt &other);

  int getValue() const;

  void inc(int step = 1);
  void dec(int step = 1);

  operator int () const;
  AtomicInt &operator ++();
  AtomicInt operator ++(int);

  AtomicInt &operator --();
  AtomicInt operator --(int);
};

class ByteArray {
public:
  typedef unsigned char    ByteType;
  typedef std::vector<ByteType>  Buffer;
  typedef Buffer::size_type   SizeType;
  typedef Buffer::iterator   iterator;
  typedef Buffer::const_iterator  const_iterator;

  static const SizeType npos;
private:
  struct SharedData {
    AtomicInt   ref;
    Buffer    buffer;
  };

  SharedData *shared_data_;

  void changeTo(SharedData *other);
  void implByteArray(const void *data, SizeType len);

  // get data for edit.
  SharedData *acquire();
public:
  ByteArray();
  explicit ByteArray(SizeType len);

  // CopyBuffer and ReadOnlyBuffer
  explicit ByteArray(const void *data, SizeType len);

  ByteArray(const ByteArray &other);
  ByteArray &operator = (const ByteArray &other);

  iterator begin();
  iterator end();
  const_iterator begin() const;
  const_iterator end() const;

  void reset(const ByteArray &other = ByteArray() );

  SizeType getLength() const;
  SizeType getSize() const {
    return getLength();
  }
  SizeType getReservedSize() const;
  const void *getBuffer() const;
  const ByteType *getData() const;
  // return pointer to data for update.
  // use it carefully.
  ByteType *askBuffer(SizeType new_len);

  bool isNull() const;
  bool isEmpty() const;

  bool setChar(SizeType index, char character);

  bool insert(SizeType index, const void *ptr, SizeType length);
  bool remove(SizeType index, SizeType count = std::numeric_limits<SizeType>::max());

  void resize(SizeType new_len);
  void reserve(SizeType new_len);

  ByteArray copyPart(SizeType index, SizeType count = std::numeric_limits<SizeType>::max()) const;

  const ByteType &operator [](SizeType index) const;
  ByteType &operator [](SizeType index);

  virtual ~ByteArray();

  static const ByteArray empty;
};

bool insert(ByteArray &array, ByteArray::SizeType index, const void *data, ByteArray::SizeType length);
bool replace(ByteArray &array, ByteArray::SizeType index, const void *data, ByteArray::SizeType length);
bool replace(ByteArray &array, ByteArray::SizeType index, const ByteArray &src);

bool append(ByteArray &array, const void *data, ByteArray::SizeType length);
bool append(ByteArray &array, const ByteArray &other);

bool prepend(ByteArray &array, const void *data, ByteArray::SizeType length);
bool prepend(ByteArray &array, const ByteArray &other);

void reset(ByteArray &array, const char *data, ByteArray::SizeType len);

template <class T>
void reset(ByteArray &array, const T &value) {
  char buffer[sizeof(T)];
  memcpy(&buffer[0], &value, buffer.size());
  reset(array, &buffer[0], buffer.size());
}

inline bool fromByteArray(const ByteArray &array, ByteArray::SizeType index, void *ptr, ByteArray::SizeType count) {
  if( !ptr || index + count > array.getLength() )
    return false;

  memcpy(ptr, &array[index], count);
  return true;
}

template <class T>
inline bool fromByteArray(const ByteArray &array, ByteArray::SizeType index, T &value) {
  return fromByteArray(array, index, &value, sizeof(T));
}

template <class T>
inline bool fromByteArray(const ByteArray &array, T &value) {
  return fromByteArray(array, 0, value);
}

template <class T>
bool insert(ByteArray &array, ByteArray::SizeType index, const T &value) {
  char buffer[sizeof(T)];
  memcpy(&buffer[0], &value, buffer.size());
  return insert(array, index, &buffer[0], buffer.size());
}

template <class T>
bool append(ByteArray &array, const T &value) {
  return insert(array, array.getLength(), value);
}

template <class T>
bool prepend(ByteArray &array, const T &value) {
  return insert(array, 0, value);
}

template <class T>
bool replace(ByteArray &array, ByteArray::SizeType index, const T &value) {
  return replace(array, index, &value, sizeof(T));
}

bool append(ByteArray &array, const std::string &str);
bool append(ByteArray &array, const char *str);

bool prepend(ByteArray &array, const std::string &str);
bool prepend(ByteArray &array, const char *str);

bool compare(
  const ByteArray &array1,
  ByteArray::SizeType from1,
  const ByteArray &array2,
  ByteArray::SizeType from2,
  ByteArray::SizeType count);

bool compare(const ByteArray &array1, const ByteArray &array2);

ByteArray::SizeType find(
  const ByteArray &array,
  ByteArray::ByteType a_ch,
  ByteArray::SizeType a_from = 0,
  ByteArray::SizeType a_maxCount = std::numeric_limits<ByteArray::SizeType>::max());

ByteArray toByteArray(const std::string &str);
std::string byteArray2string(
  const ByteArray &array,
  ByteArray::SizeType index,
  ByteArray::SizeType count,
  bool upto0 = false);
inline std::string byteArray2string(const ByteArray &array, bool upto0 = false) {
  return byteArray2string(array, 0, array.getSize(), upto0);
}

std::ostream& operator<< (std::ostream& stream, const ByteArray& array);
std::istream& operator>> (std::istream& stream, ByteArray& array);
}

//std::ostream& operator<< (std::ostream& stream, const tools::ByteArray& array);
