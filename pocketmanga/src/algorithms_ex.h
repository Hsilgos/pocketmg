#pragma once

#include <algorithm>
#include <map>
#include <set>
#include <vector>
#include <deque>
#include <list>

namespace utils {
template<class Collect, class Type>
bool exists(Collect& collect, const Type& type) {
  return std::find(collect.begin(), collect.end(), type) != collect.end();
}

template<class KeyType, class ValueType>
bool exists(std::map<KeyType, ValueType>& collect, const KeyType& key) {
  return collect.find(key) != collect.end();
}

template<class ValueType>
bool exists(std::set<ValueType>& collect, const ValueType& value) {
  return collect.find(value) != collect.end();
}

template<class OutputIterator, class Map>
void collectKeys(const Map& map, OutputIterator iter) {
  typedef typename Map::const_iterator const_iterator;
  for (const_iterator it = map.begin(), itEnd = map.end(); it != itEnd; ++it)
    iter++ = it->first;
}

template<class OutputIterator, class Map>
void collectValues(const Map& map, OutputIterator iter) {
  typedef typename Map::const_iterator const_iterator;
  for (const_iterator it = map.begin(), itEnd = map.end(); it != itEnd; ++it)
    iter++ = it->second;
}

//////////////////////////////////////////////////////////////////////////

template<class Collect, typename ValueType>
const ValueType& fetch_element_tmpl(const Collect& collect,
                                    size_t index,
                                    const ValueType& def_value = ValueType()) {
  if (index >= collect.size())
    return def_value;

  typedef typename Collect::const_iterator Iterator;
  Iterator it = collect.begin();

  for (size_t i = 0; i < index; ++i, ++it)
    ;

  return *it;
}

template<class Collect, typename ValueType>
const ValueType& fetch_element(const Collect& collect,
                               size_t index,
                               const ValueType& def_value = ValueType()) {
  return fetch_element_tmpl(collect, index, def_value);
}

template<typename ValueType>
const ValueType& fetch_element(const std::set<ValueType>& collect,
                               size_t index,
                               const ValueType& def_value = ValueType()) {
  return fetch_element_tmpl(collect, index, def_value);
}

template<typename ValueType>
const ValueType& fetch_element(const std::vector<ValueType>& collect,
                               size_t index,
                               const ValueType& def_value = ValueType()) {
  if (index > collect.size())
    return def_value;

  return collect[index];
}

template<typename ValueType>
const ValueType& fetch_element(const std::list<ValueType>& collect,
                               size_t index,
                               const ValueType& def_value = ValueType()) {
  return fetch_element_tmpl(collect, index, def_value);
}

template<typename ValueType>
const ValueType& fetch_element(const std::deque<ValueType>& collect,
                               size_t index,
                               const ValueType& def_value = ValueType()) {
  if (index > collect.size())
    return def_value;

  return collect[index];
}

inline std::size_t get_stream_size(std::istream& stream) {
  const std::size_t pos = static_cast<std::size_t>(stream.tellg());
  stream.seekg(0, std::ios::end);

  const std::size_t size = static_cast<std::size_t>(stream.tellg());
  stream.seekg(pos, std::ios::beg);

  return size;
}

//  template<class KeyType, class ValueType>
//  const std::pair<KeyType, ValueType> &fetch_element(
//   const std::map<KeyType, ValueType> &collect,
//   size_t index,
//   const ValueType &def_value = ValueType())
//  {
//   return fetch_element_tmpl(collect, index, def_value);
//  }
}
