#pragma once

namespace utils
{
	inline bool specialDelete(...)
	{
		return false;
	}

	template <class T>
	inline void deletePtr(T *&ptr)
	{
		if( 0 == ptr )
			return;

		if( !specialDelete(ptr) )
			delete ptr;

		ptr = 0;
	}

	template <class T>
	inline void deleteArrPtr(T *&ptr)
	{
		if( ptr != 0 )
		{
			delete []ptr;
			ptr = 0;
		}
	}
}
