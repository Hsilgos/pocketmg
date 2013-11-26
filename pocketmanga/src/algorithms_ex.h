#pragma once

#include <algorithm>
#include <map>
#include <set>
#include <vector>
#include <deque>
#include <list>

namespace utils
{
	template<class Collect, class Type>
	bool exists(Collect &aCollect, const Type &aType)
	{
		return std::find(aCollect.begin(), aCollect.end(), aType) != aCollect.end();
	}

	template<class KeyType, class ValueType>
	bool exists(std::map<KeyType, ValueType> &aCollect, const KeyType &aKey)
	{
		return aCollect.find(aKey) != aCollect.end();
	}

	template<class ValueType>
	bool exists(std::set<ValueType> &aCollect, const ValueType &aValue)
	{
		return aCollect.find(aValue) != aCollect.end();
	}

	template<class OutputIterator, class Map>
	void collectKeys(const Map &aMap, OutputIterator aIter)
	{
		typedef typename Map::const_iterator const_iterator;
		for( const_iterator it = aMap.begin(), itEnd = aMap.end(); it != itEnd; ++it )
			aIter++ = it->first;
	}

	template<class OutputIterator, class Map>
	void collectValues(const Map &aMap, OutputIterator aIter)
	{
		typedef typename Map::const_iterator const_iterator;
		for( const_iterator it = aMap.begin(), itEnd = aMap.end(); it != itEnd; ++it )
			aIter++ = it->second;
	}

	//////////////////////////////////////////////////////////////////////////

	template<class Collect, typename ValueType>
	const ValueType &fetch_element_tmpl(
		const Collect &aCollect, 
		size_t aIndex, 
		const ValueType &aDefValue = ValueType())
	{
		if( aIndex >= aCollect.size() )
			return aDefValue;

		typedef typename Collect::const_iterator Iterator;
		Iterator it = aCollect.begin();

		for ( size_t i = 0; i < aIndex; ++i, ++it )
			;

		return *it;		
	}

	template<class Collect, typename ValueType>
	const ValueType &fetch_element(
		const Collect &aCollect, 
		size_t aIndex, 
		const ValueType &aDefValue = ValueType())
	{
		return fetch_element_tmpl(aCollect, aIndex, aDefValue);
	}

	template<typename ValueType>
	const ValueType &fetch_element(
		const std::set<ValueType> &aCollect, 
		size_t aIndex, 
		const ValueType &aDefValue = ValueType())
	{
		return fetch_element_tmpl(aCollect, aIndex, aDefValue);
	}

	template<typename ValueType>
	const ValueType &fetch_element(
		const std::vector<ValueType> &aCollect, 
		size_t aIndex, 
		const ValueType &aDefValue = ValueType())
	{
		if( aIndex > aCollect.size() )
			return aDefValue;

		return aCollect[aIndex];
	}

	template<typename ValueType>
	const ValueType &fetch_element(
		const std::list<ValueType> &aCollect, 
		size_t aIndex, 
		const ValueType &aDefValue = ValueType())
	{
		return fetch_element_tmpl(aCollect, aIndex, aDefValue);
	}

	template<typename ValueType>
	const ValueType &fetch_element(
		const std::deque<ValueType> &aCollect, 
		size_t aIndex, 
		const ValueType &aDefValue = ValueType())
	{
		if( aIndex > aCollect.size() )
			return aDefValue;

		return aCollect[aIndex];
	}

	inline std::size_t get_stream_size(std::istream &aStream)
	{
		const std::size_t tPos = static_cast<std::size_t>(aStream.tellg());
		aStream.seekg (0, std::ios::end);

		const std::size_t tSize = static_cast<std::size_t>(aStream.tellg());
		aStream.seekg(tPos, std::ios::beg);

		return tSize;
	}

// 	template<class KeyType, class ValueType>
// 	const std::pair<KeyType, ValueType> &fetch_element(
// 		const std::map<KeyType, ValueType> &aCollect, 
// 		size_t aIndex, 
// 		const ValueType &aDefValue = ValueType())
// 	{
// 		return fetch_element_tmpl(aCollect, aIndex, aDefValue);
// 	}
}
