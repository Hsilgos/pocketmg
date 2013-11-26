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

namespace tools
{
	class AtomicInt
	{
		volatile int mValue;
	public:
		AtomicInt(int aInitValue = 0);

		AtomicInt(const AtomicInt &aOther);
		AtomicInt &operator = (const AtomicInt &aOther);

		int getValue() const;

		void inc(int aStep = 1);
		void dec(int aStep = 1);

		operator int () const;
		AtomicInt &operator ++();
		AtomicInt operator ++(int);

		AtomicInt &operator --();
		AtomicInt operator --(int);
	};

	class ByteArray
	{
	public:
		typedef unsigned char				ByteType;
		typedef std::vector<ByteType>		Buffer;
		typedef Buffer::size_type			SizeType;
		typedef Buffer::iterator			iterator;
		typedef Buffer::const_iterator		const_iterator;

		static const SizeType npos;
	private:
		struct SharedData
		{
			AtomicInt			ref;
			Buffer				buffer;
		};

		SharedData *mSharedData;

		void changeTo(SharedData *aOther);
		void implByteArray(const void *aData, SizeType aLen);

		// get data for edit.
		SharedData *acquire();
	public:
		ByteArray();
		explicit ByteArray(SizeType aLen);

		// CopyBuffer and ReadOnlyBuffer
		explicit ByteArray(const void *aData, SizeType aLen);

		ByteArray(const ByteArray &aOther);
		ByteArray &operator = (const ByteArray &aOther);

		iterator begin();
		iterator end();
		const_iterator begin() const;
		const_iterator end() const;

		void reset(const ByteArray &aOther = ByteArray() );

		SizeType getLength() const;
		SizeType getSize() const
		{
			return getLength();
		}
		SizeType getReservedSize() const;
		const void *getBuffer() const;
		const ByteType *getData() const;
		// return pointer to data for update.
		// use it carefully.
		ByteType *askBuffer(SizeType aNewLen);

		bool isNull() const;
		bool isEmpty() const;

		bool setChar(SizeType aIndex, char aChar);
		
		bool insert(SizeType aIndex, const void *aPtr, SizeType aLength);
		bool remove(SizeType aIndex, SizeType aCount = std::numeric_limits<SizeType>::max());

		void resize(SizeType aNewLen);
		void reserve(SizeType aNewLen);

		ByteArray copyPart(SizeType aIndex, SizeType aCount = std::numeric_limits<SizeType>::max()) const;

		const ByteType &operator [](SizeType aIndex) const;
		ByteType &operator [](SizeType aIndex);

		virtual ~ByteArray();

		static const ByteArray empty;
	};

	bool insert(ByteArray &aArray, ByteArray::SizeType aIndex, const void *aData, ByteArray::SizeType aLength);
	bool replace(ByteArray &aArray, ByteArray::SizeType aIndex, const void *aData, ByteArray::SizeType aLength);
	bool replace(ByteArray &aArray, ByteArray::SizeType aIndex, const ByteArray &aSrc);

	bool append(ByteArray &aArray, const void *aData, ByteArray::SizeType aLength);
	bool append(ByteArray &aArray, const ByteArray &aOther);

	bool prepend(ByteArray &aArray, const void *aData, ByteArray::SizeType aLength);
	bool prepend(ByteArray &aArray, const ByteArray &aOther);

	void reset(ByteArray &aArray, const char *aData, ByteArray::SizeType aLen);

	template <class T>
	void reset(ByteArray &aArray, const T &aValue)
	{
		char tBuffer[sizeof(T)];
		memcpy(&tBuffer[0], &aValue, tBuffer.size());
		reset(aArray, &tBuffer[0], tBuffer.size());
	}

	inline bool fromByteArray(const ByteArray &aArray, ByteArray::SizeType aIndex, void *aPtr, ByteArray::SizeType aCount)
	{
		if( !aPtr || aIndex + aCount > aArray.getLength() )
			return false;

		memcpy(aPtr, &aArray[aIndex], aCount);
		return true;
	}

	template <class T>
	inline bool fromByteArray(const ByteArray &aArray, ByteArray::SizeType aIndex, T &aValue)
	{
		return fromByteArray(aArray, aIndex, &aValue, sizeof(T));
	}

	template <class T>
	inline bool fromByteArray(const ByteArray &aArray, T &aValue)
	{
		return fromByteArray(aArray, 0, aValue);
	}

	template <class T>
	bool insert(ByteArray &aArray, ByteArray::SizeType aIndex, const T &aValue)
	{
		char tBuffer[sizeof(T)];
		memcpy(&tBuffer[0], &aValue, tBuffer.size());
		return insert(aArray, aIndex, &tBuffer[0], tBuffer.size());
	}

	template <class T>
	bool append(ByteArray &aArray, const T &aValue)
	{
		return insert(aArray, aArray.getLength(), aValue);
	}

	template <class T>
	bool prepend(ByteArray &aArray, const T &aValue)
	{
		return insert(aArray, 0, aValue);
	}

	template <class T>
	bool replace(ByteArray &aArray, ByteArray::SizeType aIndex, const T &aValue)
	{
		return replace(aArray, aIndex, &aValue, sizeof(T));
	}

	bool append(ByteArray &aArray, const std::string &aStr);
	bool append(ByteArray &aArray, const char *aStr);

	bool prepend(ByteArray &aArray, const std::string &aStr);
	bool prepend(ByteArray &aArray, const char *aStr);

	bool compare(
		const ByteArray &aArray1,
		ByteArray::SizeType aFrom1,
		const ByteArray &aArray2,
		ByteArray::SizeType aFrom2,
		ByteArray::SizeType aCount);

	bool compare(const ByteArray &aArray1, const ByteArray &aArray2);

	ByteArray::SizeType find(
		const ByteArray &aArray, 
		ByteArray::ByteType a_ch, 
		ByteArray::SizeType a_from = 0, 
		ByteArray::SizeType a_maxCount = std::numeric_limits<ByteArray::SizeType>::max());

	ByteArray toByteArray(const std::string &aStr);
	std::string byteArray2string(
		const ByteArray &aArray, 
		ByteArray::SizeType aIndex, 
		ByteArray::SizeType aCount, 
		bool aUpto0 = false);
	inline std::string byteArray2string(const ByteArray &aArray, bool aUpto0 = false)
	{
		return byteArray2string(aArray, 0, aArray.getSize(), aUpto0);
	}

	std::ostream& operator<< (std::ostream& aStream, const ByteArray& aArray);
	std::istream& operator>> (std::istream& aStream, ByteArray& aArray);
}

//std::ostream& operator<< (std::ostream& aStream, const tools::ByteArray& aArray);
