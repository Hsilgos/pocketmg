#pragma once


namespace pattern
{
#define PATTERN_DEFINE_CLONE( IRetClass ) \
	virtual IRetClass* clone() const = 0;


#define PATTERN_IMPL_CLONE( RetClass )	\
	virtual RetClass* clone() const		\
	{									\
		return new RetClass( *this );	\
	}
}

