#pragma once

#include <boost/date_time.hpp>

namespace tools
{
	typedef boost::posix_time::ptime system_time;
	inline system_time get_system_time()
	{
#if defined(BOOST_DATE_TIME_HAS_HIGH_PRECISION_CLOCK)
		return boost::date_time::microsec_clock<system_time>::universal_time();
#else // defined(BOOST_DATE_TIME_HAS_HIGH_PRECISION_CLOCK)
		return boost::date_time::second_clock<system_time>::universal_time();
#endif // defined(BOOST_DATE_TIME_HAS_HIGH_PRECISION_CLOCK)
	}
}
