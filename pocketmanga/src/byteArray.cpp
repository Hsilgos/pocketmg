#include "byteArray.h"

#include <algorithm>

#include <ostream>
#include <istream>

namespace tools
{
	AtomicInt::AtomicInt(int aInitValue)
		:mValue(aInitValue)
	{
	}

	AtomicInt::AtomicInt(const AtomicInt &aOther)
		:mValue(aOther.mValue)
	{
	}

	AtomicInt &AtomicInt::operator = (const AtomicInt &aOther)
	{
		mValue = aOther.mValue;
		return *this;
	}

	int AtomicInt::getValue() const
	{
		return mValue;
	}

	void AtomicInt::inc(int aStep)
	{
		mValue += aStep;
	}

	void AtomicInt::dec(int aStep)
	{
		mValue -= aStep;
	}

	AtomicInt::operator int () const
	{
		return getValue();
	}

	AtomicInt &AtomicInt::operator ++()
	{
		inc(1);
		return *this;
	}

	AtomicInt AtomicInt::operator ++(int)
	{
		AtomicInt t_ret = *this;
		inc(1);
		return t_ret;
	}

	AtomicInt &AtomicInt::operator --()
	{
		dec(1);
		return *this;
	}

	AtomicInt AtomicInt::operator --(int)
	{
		AtomicInt t_ret = *this;
		dec(1);
		return t_ret;
	}

	//////////////////////////////////////////////////////////////////////////

	ByteArray::ByteArray()
		:mSharedData(0)
	{
	}

	ByteArray::ByteArray(ByteArray::SizeType aLen)
		:mSharedData(0)
	{
		resize(aLen);
	}

	ByteArray::ByteArray(const void *aData, SizeType aLen)
		:mSharedData(0)
	{
		implByteArray(aData, aLen);
	}

	const ByteArray::SizeType ByteArray::npos = std::numeric_limits<ByteArray::SizeType>::max();

	void ByteArray::implByteArray(const void *aData, SizeType aLen)
	{
		if( aData && aLen > 0 )
		{
			mSharedData = new SharedData;

			const ByteType *tData = static_cast<const ByteType *>(aData);
			mSharedData->buffer.assign(tData, tData + aLen);
			mSharedData->ref.inc();
		}
	}

	ByteArray::ByteArray(const ByteArray &aOther)
		:mSharedData(0)
	{
		changeTo(aOther.mSharedData);
	}

	ByteArray &ByteArray::operator = (const ByteArray &aOther)
	{
		changeTo(aOther.mSharedData);
		return *this;
	}

	ByteArray::iterator ByteArray::begin()
	{
		if( mSharedData )
		{
			return mSharedData->buffer.begin();
		}
		return iterator();
	}

	ByteArray::iterator ByteArray::end()
	{
		if( mSharedData )
		{
			return mSharedData->buffer.end();
		}
		return iterator();
	}

	ByteArray::const_iterator ByteArray::begin() const
	{
		if( mSharedData )
		{
			return mSharedData->buffer.begin();
		}
		return iterator();
	}

	ByteArray::const_iterator ByteArray::end() const
	{
		if( mSharedData )
		{
			return mSharedData->buffer.end();
		}
		return iterator();
	}

	ByteArray::~ByteArray()
	{
		changeTo(0);
	}

	void ByteArray::changeTo(ByteArray::SharedData *aOther)
	{
		if( mSharedData == aOther )
			return;

		if( mSharedData )
		{
			if( 1 == mSharedData->ref.getValue() )
			{
				delete mSharedData;
			}
			else
			{
				mSharedData->ref.dec();
			}

			mSharedData = 0;
		}

		if( aOther )
		{
			mSharedData = aOther;
			mSharedData->ref.inc();
		}
	}

	ByteArray::SharedData *ByteArray::acquire()
	{
		if( mSharedData )
		{
			if( mSharedData->ref.getValue() > 1 )
			{
				SharedData *tOther		= mSharedData;

				mSharedData				= new SharedData;
				mSharedData->buffer		= tOther->buffer;

				mSharedData->ref.inc();
				tOther->ref.dec();
			}
		}
		return mSharedData;
	}

	ByteArray::SizeType ByteArray::getLength() const
	{
		if( mSharedData )
		{
			return mSharedData->buffer.size();
		}
		return 0;
	}

	ByteArray::SizeType ByteArray::getReservedSize() const
	{
		if( mSharedData )
		{
			return mSharedData->buffer.capacity();
		}
		return 0;
	}

	const ByteArray::ByteType *ByteArray::getData() const
	{
		if( mSharedData )
		{
			if( !mSharedData->buffer.empty() )
				return &mSharedData->buffer[0];
		}

		return 0;
	}

	ByteArray::ByteType *ByteArray::askBuffer(SizeType aNewLen)
	{
		resize(aNewLen);

		return &mSharedData->buffer[0];
	}

	const void *ByteArray::getBuffer() const
	{
		return getData();
	}

	bool ByteArray::isNull() const
	{
		return !mSharedData;
	}

	bool ByteArray::isEmpty() const
	{
		return isNull() || mSharedData->buffer.empty();
	}

	bool ByteArray::setChar(ByteArray::SizeType aIndex, char aChar)
	{
		if( mSharedData )
		{

			if( aIndex < mSharedData->buffer.size() )
			{
				SharedData *t_myData = acquire();
				if( t_myData )
				{
					t_myData->buffer[aIndex] = aChar;
					return true;
				}
			}
		}
		
		return false;
	}

	bool ByteArray::insert(SizeType aIndex, const void *aPtr, SizeType aLength)
	{
		// we already added data of length 0 :-)
		if( 0 == aLength )
			return true;

		if( 0 == aPtr )
			return false;

		if( isNull() )
		{
			// Buffer may be not initialized. So, if we add data at the beginning, we can 
			// init buffer
			reserve(aLength);
		}

		if( mSharedData )
		{
			if(aIndex <= mSharedData->buffer.size())
			{
				SharedData *tMyData = acquire();

				if( tMyData )
				{
					const char *tData = static_cast<const char *>(aPtr);
					mSharedData->buffer.insert(tMyData->buffer.begin() + aIndex, tData, tData + aLength );

					return true;
				}
			}
		}

		return false;
	}

	bool ByteArray::remove(SizeType aIndex, SizeType aCount)
	{
		if( mSharedData && aIndex <= mSharedData->buffer.size() )
		{
			SizeType tMyLen = getLength();
			// sum can me more than size_type.
			if( (aCount > tMyLen) || (aIndex > (tMyLen - aCount)) )
			{
				resize(aIndex);
				return true;
			}

			SharedData *tMyData = acquire();
			if( tMyData )
			{
				Buffer::iterator it1 = tMyData->buffer.begin() + aIndex;
				Buffer::iterator it2 = it1 + aCount;

				tMyData->buffer.erase(it1, it2);
				return true;
			}
		}
		return false;
	}

	void ByteArray::resize(ByteArray::SizeType aNewLen)
	{
		
		if( !mSharedData )
		{
			if( 0 == aNewLen )
				return;

			mSharedData = new SharedData;
			mSharedData->buffer.resize(aNewLen, 0);
			mSharedData->ref.inc();
		}
		else
		{
			SharedData *tMyData = acquire();
			if( tMyData )
				tMyData->buffer.resize(aNewLen, 0);
		}
	}

	void ByteArray::reserve(SizeType aNewLen)
	{

		if( !mSharedData )
		{
			if( 0 == aNewLen )
				return;

			mSharedData = new SharedData;
			mSharedData->buffer.reserve(aNewLen);
			mSharedData->ref.inc();
		}
		else
		{
			SharedData *tMyData = acquire();
			if( tMyData )
				tMyData->buffer.reserve(aNewLen);
		}
	}

	ByteArray ByteArray::copyPart(SizeType aIndex, SizeType aCount) const
	{

		if( mSharedData )
		{

			SizeType tMyLen = getSize();
			if( aIndex >= mSharedData->buffer.size() )
				return ByteArray();

			SizeType tCount = aCount;
			if( (aCount > tMyLen) || (aIndex > (tMyLen - aCount)) )
				tCount = tMyLen - aIndex;

			if( tCount > 0)
				return ByteArray(&mSharedData->buffer[aIndex], tCount);
		}

		return ByteArray();
	}

	void ByteArray::reset(const ByteArray &aOther)
	{
		changeTo(aOther.mSharedData);
	}

	const ByteArray::ByteType &ByteArray::operator [](ByteArray::SizeType aIndex) const
	{

		if( mSharedData )
		{

			if( aIndex < mSharedData->buffer.size() )
				return mSharedData->buffer[aIndex];

		}

		static const ByteType tDummy = 0;
		return tDummy;
	}

	ByteArray::ByteType &ByteArray::operator [](SizeType aIndex)
	{

		if( mSharedData )
		{

			if( aIndex < mSharedData->buffer.size() )
			{
				SharedData *tMyData = acquire();
				return tMyData->buffer[aIndex];
			}
		}
		
		static ByteType tDummy = 0;
		return tDummy;
	}

	const ByteArray ByteArray::empty;

	//////////////////////////////////////////////////////////////////////////

	bool insert(ByteArray &aArray, ByteArray::SizeType aIndex, const void *aData, ByteArray::SizeType aLength)
	{
		return aArray.insert(aIndex,  aData, aLength);
	}

	bool replace(ByteArray &aArray, ByteArray::SizeType aIndex, const void *aData, ByteArray::SizeType aLength)
	{
		const char *tChData = static_cast<const char *>(aData);
		ByteArray::SizeType tFrom	= aIndex;
		ByteArray::SizeType tTo	= std::min(aIndex + aLength, aArray.getSize());
		for ( ByteArray::SizeType i = tFrom; i < tTo; ++i )
			aArray[i] = tChData[i - tFrom];

		return true;
	}

	bool replace(ByteArray &aArray, ByteArray::SizeType aIndex, const ByteArray &aSrc)
	{
		return replace(aArray, aIndex, aSrc.getData(), aSrc.getSize());
	}

	bool append(ByteArray &aArray, const void *aData, ByteArray::SizeType aLength)
	{
		return insert(aArray, aArray.getLength(), aData, aLength );
	}

	bool append(ByteArray &aArray, const ByteArray &aOther)
	{
		if( 0 == aArray.getLength() )
		{
			aArray = aOther;
			return true;
		}

		return insert(aArray, aArray.getLength(), aOther.getData(), aOther.getLength() );
	}

	bool prepend(ByteArray &aArray, const void *aData, ByteArray::SizeType aLength)
	{
		return insert(aArray, 0, aData, aLength );
	}

	bool prepend(ByteArray &aArray, const ByteArray &aOther)
	{
		if( 0 == aArray.getLength() )
		{
			aArray = aOther;
			return true;
		}

		return insert(aArray, 0, aOther.getData(), aOther.getLength());
	}

	void reset(ByteArray &aArray, const char *aData, ByteArray::SizeType aLen)
	{
		aArray.reset(ByteArray(aData, aLen));
	}

	bool append(ByteArray &aArray, const std::string &aStr)
	{
		return append(aArray, aStr.c_str(), aStr.size());
	}

	bool append(ByteArray &aArray, const char *aStr)
	{
		return append(aArray, aStr, strlen(aStr));
	}

	bool prepend(ByteArray &aArray, const std::string &aStr)
	{
		return prepend(aArray, aStr.c_str(), aStr.size());
	}

	bool prepend(ByteArray &aArray, const char *aStr)
	{
		return prepend(aArray, aStr, strlen(aStr));
	}

	bool compare(
		const ByteArray &aArray1,
		ByteArray::SizeType aFrom1,
		const ByteArray &aArray2,
		ByteArray::SizeType aFrom2,
		ByteArray::SizeType aCount)
	{
		if( (aArray1.getSize() < aFrom1 + aCount) || 
			(aArray2.getSize() < aFrom2 + aCount))
			return false;

		if( aArray1.getData() == aArray2.getData() && aFrom1 == aFrom2 )
			return true;

		for ( ByteArray::SizeType i = 0; i < aCount; ++i )
		{
			if( aArray1[aFrom1 + i] != aArray2[aFrom2 + i] )
				return false;
		}

		return true;
	}

	bool compare(const ByteArray &aArray1, const ByteArray &aArray2)
	{
		if( aArray1.getSize() != aArray2.getSize() )
			return false;

		if( aArray1.getData() == aArray2.getData() )
			return true;

		for ( ByteArray::SizeType i = 0; i < aArray1.getSize(); ++i )
		{
			if( aArray1[i] != aArray2[i] )
				return false;
		}

		return true;
	}

	bool chooseIterators(
		const ByteArray &aArray,
		ByteArray::SizeType aFrom, 
		ByteArray::SizeType aMaxCount,
		ByteArray::const_iterator &aIt,
		ByteArray::const_iterator &aItEnd)
	{
		if( aArray.isEmpty() )
			return false;

		aIt = aArray.begin() + aFrom;
		aItEnd = aArray.end();

		if( aItEnd - aIt < 0 )
			return false;

		if( static_cast<ByteArray::SizeType>(aItEnd - aIt) > aMaxCount )
			aItEnd = aIt + aMaxCount;

		return true;
	}

	ByteArray::SizeType find(
		const ByteArray &aArray, 
		ByteArray::ByteType aCh, 
		ByteArray::SizeType aFrom, 
		ByteArray::SizeType aMaxCount)
	{
		ByteArray::const_iterator it;
		ByteArray::const_iterator itEnd;
		if( !chooseIterators(aArray, aFrom, aMaxCount, it, itEnd) )
			return ByteArray::npos;

		ByteArray::const_iterator itFound = std::find(it ,itEnd, aCh);
		if( itFound != itEnd )
			return itFound - aArray.begin();

		return ByteArray::npos;
	}

	ByteArray toByteArray(const std::string &aStr)
	{
		return ByteArray(aStr.c_str(), aStr.size());
	}

	std::string byteArray2string(const ByteArray &aArray, ByteArray::SizeType aIndex, ByteArray::SizeType aCount, bool aUpto0)
	{
		if( aIndex > aArray.getSize() )
			return std::string();

		ByteArray::SizeType tFrom	= aIndex;
		ByteArray::SizeType tTo	= std::min(aIndex + aCount, aArray.getSize());

		if( aUpto0 )
		{
			ByteArray::SizeType tFound = find(aArray, '\0', aIndex, aCount);
			if( tFound != std::string::npos )
				tTo	= tFound;
		}

		return std::string(aArray.getData() + tFrom, aArray.getData() + tTo);
	}

	std::ostream& operator<< (std::ostream& aStream, const ByteArray& aArray)
	{
		const void *t_ptr = aArray.getData();
		aStream.write(static_cast<const char *>(t_ptr), aArray.getSize() );
		return aStream;
	}

	std::istream& operator>> (std::istream& aStream, ByteArray& aArray)
	{
		const std::size_t tPos = static_cast<std::size_t>(aStream.tellg());
		aStream.seekg (0, std::ios::end);

		const std::size_t tSize = static_cast<std::size_t>(aStream.tellg());
		aStream.seekg(tPos, std::ios::beg);

		aStream.read( reinterpret_cast<char *>(aArray.askBuffer(tSize)), tSize );
		return aStream;
	}
}



