#include "byteArray.h"

#include <algorithm>

#include <ostream>
#include <istream>

namespace tools {
AtomicInt::AtomicInt(int init_value)
  : value_(init_value) {}

AtomicInt::AtomicInt(const AtomicInt& other)
  : value_(other.value_) {}

AtomicInt& AtomicInt::operator =(const AtomicInt& other) {
  value_ = other.value_;
  return *this;
}

int AtomicInt::getValue() const {
  return value_;
}

void AtomicInt::inc(int step) {
  value_ += step;
}

void AtomicInt::dec(int step) {
  value_ -= step;
}

AtomicInt::operator int() const {
  return getValue();
}

AtomicInt& AtomicInt::operator ++() {
  inc(1);
  return *this;
}

AtomicInt AtomicInt::operator ++(int) {
  AtomicInt t_ret = *this;
  inc(1);
  return t_ret;
}

AtomicInt& AtomicInt::operator --() {
  dec(1);
  return *this;
}

AtomicInt AtomicInt::operator --(int) {
  AtomicInt t_ret = *this;
  dec(1);
  return t_ret;
}

//////////////////////////////////////////////////////////////////////////

ByteArray::ByteArray()
  : shared_data_(0) {}

ByteArray::ByteArray(ByteArray::SizeType len)
  : shared_data_(0) {
  resize(len);
}

ByteArray::ByteArray(const void* data, SizeType len)
  : shared_data_(0) {
  implByteArray(data, len);
}

const ByteArray::SizeType ByteArray::npos = std::numeric_limits<ByteArray::SizeType>::max();

void ByteArray::implByteArray(const void* raw_data, SizeType len) {
  if (raw_data && len > 0) {
    shared_data_ = new SharedData;

    const ByteType* data = static_cast<const ByteType*>(raw_data);
    shared_data_->buffer.assign(data, data + len);
    shared_data_->ref.inc();
  }
}

ByteArray::ByteArray(const ByteArray& other)
  : shared_data_(0) {
  changeTo(other.shared_data_);
}

ByteArray& ByteArray::operator =(const ByteArray& other) {
  changeTo(other.shared_data_);
  return *this;
}

ByteArray::iterator ByteArray::begin() {
  if (shared_data_) {
    return shared_data_->buffer.begin();
  }
  return iterator();
}

ByteArray::iterator ByteArray::end() {
  if (shared_data_) {
    return shared_data_->buffer.end();
  }
  return iterator();
}

ByteArray::const_iterator ByteArray::begin() const {
  if (shared_data_) {
    return shared_data_->buffer.begin();
  }
  return iterator();
}

ByteArray::const_iterator ByteArray::end() const {
  if (shared_data_) {
    return shared_data_->buffer.end();
  }
  return iterator();
}

ByteArray::~ByteArray() {
  changeTo(0);
}

void ByteArray::changeTo(ByteArray::SharedData* other) {
  if (shared_data_ == other)
    return;

  if (shared_data_) {
    if (1 == shared_data_->ref.getValue()) {
      delete shared_data_;
    } else {
      shared_data_->ref.dec();
    }

    shared_data_ = 0;
  }

  if (other) {
    shared_data_ = other;
    shared_data_->ref.inc();
  }
}

ByteArray::SharedData* ByteArray::acquire() {
  if (shared_data_) {
    if (shared_data_->ref.getValue() > 1) {
      SharedData* other  = shared_data_;

      shared_data_    = new SharedData;
      shared_data_->buffer  = other->buffer;

      shared_data_->ref.inc();
      other->ref.dec();
    }
  }
  return shared_data_;
}

ByteArray::SizeType ByteArray::getLength() const {
  if (shared_data_) {
    return shared_data_->buffer.size();
  }
  return 0;
}

ByteArray::SizeType ByteArray::getReservedSize() const {
  if (shared_data_) {
    return shared_data_->buffer.capacity();
  }
  return 0;
}

const ByteArray::ByteType* ByteArray::getData() const {
  if (shared_data_) {
    if (!shared_data_->buffer.empty())
      return &shared_data_->buffer[0];
  }

  return 0;
}

ByteArray::ByteType* ByteArray::askBuffer(SizeType new_len) {
  resize(new_len);

  return &shared_data_->buffer[0];
}

const void* ByteArray::getBuffer() const {
  return getData();
}

bool ByteArray::isNull() const {
  return !shared_data_;
}

bool ByteArray::isEmpty() const {
  return isNull() || shared_data_->buffer.empty();
}

bool ByteArray::setChar(ByteArray::SizeType index, char character) {
  if (shared_data_) {

    if (index < shared_data_->buffer.size()) {
      SharedData* t_myData = acquire();
      if (t_myData) {
        t_myData->buffer[index] = character;
        return true;
      }
    }
  }

  return false;
}

bool ByteArray::insert(SizeType index, const void* ptr, SizeType length) {
  // we already added data of length 0 :-)
  if (0 == length)
    return true;

  if (0 == ptr)
    return false;

  if (isNull()) {
    // Buffer may be not initialized. So, if we add data at the beginning, we can
    // init buffer
    reserve(length);
  }

  if (shared_data_) {
    if (index <= shared_data_->buffer.size()) {
      SharedData* my_data = acquire();

      if (my_data) {
        const char* data = static_cast<const char*>(ptr);
        shared_data_->buffer.insert(my_data->buffer.begin() + index, data, data + length);

        return true;
      }
    }
  }

  return false;
}

bool ByteArray::remove(SizeType index, SizeType count) {
  if (shared_data_ && index <= shared_data_->buffer.size()) {
    SizeType my_len = getLength();
    // sum can me more than size_type.
    if ((count > my_len) || (index > (my_len - count))) {
      resize(index);
      return true;
    }

    SharedData* my_data = acquire();
    if (my_data) {
      Buffer::iterator it1 = my_data->buffer.begin() + index;
      Buffer::iterator it2 = it1 + count;

      my_data->buffer.erase(it1, it2);
      return true;
    }
  }
  return false;
}

void ByteArray::resize(ByteArray::SizeType new_len) {

  if (!shared_data_) {
    if (0 == new_len)
      return;

    shared_data_ = new SharedData;
    shared_data_->buffer.resize(new_len, 0);
    shared_data_->ref.inc();
  } else {
    SharedData* my_data = acquire();
    if (my_data)
      my_data->buffer.resize(new_len, 0);
  }
}

void ByteArray::reserve(SizeType new_len) {

  if (!shared_data_) {
    if (0 == new_len)
      return;

    shared_data_ = new SharedData;
    shared_data_->buffer.reserve(new_len);
    shared_data_->ref.inc();
  } else {
    SharedData* my_data = acquire();
    if (my_data)
      my_data->buffer.reserve(new_len);
  }
}

ByteArray ByteArray::copyPart(SizeType index, SizeType copy_count) const {

  if (shared_data_) {

    SizeType my_len = getSize();
    if (index >= shared_data_->buffer.size())
      return ByteArray();

    SizeType count = copy_count;
    if ((count > my_len) || (index > (my_len - count)))
      count = my_len - index;

    if (count > 0)
      return ByteArray(&shared_data_->buffer[index], count);
  }

  return ByteArray();
}

void ByteArray::reset(const ByteArray& other) {
  changeTo(other.shared_data_);
}

const ByteArray::ByteType& ByteArray::operator [](ByteArray::SizeType index) const {

  if (shared_data_) {

    if (index < shared_data_->buffer.size())
      return shared_data_->buffer[index];

  }

  static const ByteType dummy = 0;
  return dummy;
}

ByteArray::ByteType& ByteArray::operator [](SizeType index) {

  if (shared_data_) {

    if (index < shared_data_->buffer.size()) {
      SharedData* my_data = acquire();
      return my_data->buffer[index];
    }
  }

  static ByteType dummy = 0;
  return dummy;
}

const ByteArray ByteArray::empty;

//////////////////////////////////////////////////////////////////////////

bool insert(ByteArray& array, ByteArray::SizeType index, const void* data, ByteArray::SizeType length) {
  return array.insert(index,  data, length);
}

bool replace(ByteArray& array, ByteArray::SizeType index, const void* data, ByteArray::SizeType length) {
  const char* ch_data = static_cast<const char*>(data);
  ByteArray::SizeType from = index;
  ByteArray::SizeType to = std::min(index + length, array.getSize());
  for (ByteArray::SizeType i = from; i < to; ++i)
    array[i] = ch_data[i - from];

  return true;
}

bool replace(ByteArray& array, ByteArray::SizeType index, const ByteArray& src) {
  return replace(array, index, src.getData(), src.getSize());
}

bool append(ByteArray& array, const void* data, ByteArray::SizeType length) {
  return insert(array, array.getLength(), data, length);
}

bool append(ByteArray& array, const ByteArray& other) {
  if (0 == array.getLength()) {
    array = other;
    return true;
  }

  return insert(array, array.getLength(), other.getData(), other.getLength());
}

bool prepend(ByteArray& array, const void* data, ByteArray::SizeType length) {
  return insert(array, 0, data, length);
}

bool prepend(ByteArray& array, const ByteArray& other) {
  if (0 == array.getLength()) {
    array = other;
    return true;
  }

  return insert(array, 0, other.getData(), other.getLength());
}

void reset(ByteArray& array, const char* data, ByteArray::SizeType len) {
  array.reset(ByteArray(data, len));
}

bool append(ByteArray& array, const std::string& str) {
  return append(array, str.c_str(), str.size());
}

bool append(ByteArray& array, const char* str) {
  return append(array, str, strlen(str));
}

bool prepend(ByteArray& array, const std::string& str) {
  return prepend(array, str.c_str(), str.size());
}

bool prepend(ByteArray& array, const char* str) {
  return prepend(array, str, strlen(str));
}

bool compare(const ByteArray& array1,
             ByteArray::SizeType from1,
             const ByteArray& array2,
             ByteArray::SizeType from2,
             ByteArray::SizeType count) {
  if ((array1.getSize() < from1 + count) ||
      (array2.getSize() < from2 + count))
    return false;

  if (array1.getData() == array2.getData() && from1 == from2)
    return true;

  for (ByteArray::SizeType i = 0; i < count; ++i) {
    if (array1[from1 + i] != array2[from2 + i])
      return false;
  }

  return true;
}

bool compare(const ByteArray& array1, const ByteArray& array2) {
  if (array1.getSize() != array2.getSize())
    return false;

  if (array1.getData() == array2.getData())
    return true;

  for (ByteArray::SizeType i = 0; i < array1.getSize(); ++i) {
    if (array1[i] != array2[i])
      return false;
  }

  return true;
}

bool chooseIterators(const ByteArray& array,
                     ByteArray::SizeType from,
                     ByteArray::SizeType max_count,
                     ByteArray::const_iterator& it,
                     ByteArray::const_iterator& it_end) {
  if (array.isEmpty())
    return false;

  it = array.begin() + from;
  it_end = array.end();

  if (it_end - it < 0)
    return false;

  if (static_cast<ByteArray::SizeType>(it_end - it) > max_count)
    it_end = it + max_count;

  return true;
}

ByteArray::SizeType find(const ByteArray& array,
                         ByteArray::ByteType ch,
                         ByteArray::SizeType from,
                         ByteArray::SizeType max_count) {
  ByteArray::const_iterator it;
  ByteArray::const_iterator itEnd;
  if (!chooseIterators(array, from, max_count, it, itEnd))
    return ByteArray::npos;

  ByteArray::const_iterator itFound = std::find(it, itEnd, ch);
  if (itFound != itEnd)
    return itFound - array.begin();

  return ByteArray::npos;
}

ByteArray toByteArray(const std::string& str) {
  return ByteArray(str.c_str(), str.size());
}

std::string byteArray2string(const ByteArray& array, ByteArray::SizeType index, ByteArray::SizeType count, bool upto0) {
  if (index > array.getSize())
    return std::string();

  ByteArray::SizeType from = index;
  ByteArray::SizeType to = std::min(index + count, array.getSize());

  if (upto0) {
    ByteArray::SizeType found = find(array, '\0', index, count);
    if (found != std::string::npos)
      to = found;
  }

  return std::string(array.getData() + from, array.getData() + to);
}

std::ostream& operator<<(std::ostream& stream, const ByteArray& array) {
  const void* t_ptr = array.getData();
  stream.write(static_cast<const char*>(t_ptr), array.getSize());
  return stream;
}

std::istream& operator>>(std::istream& stream, ByteArray& array) {
  const std::size_t pos = static_cast<std::size_t>(stream.tellg());
  stream.seekg(0, std::ios::end);

  const std::size_t size = static_cast<std::size_t>(stream.tellg());
  stream.seekg(pos, std::ios::beg);

  stream.read(reinterpret_cast<char*>(array.askBuffer(size)), size);
  return stream;
}
}
