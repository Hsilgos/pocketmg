#include "testBenchmark.h"

#include <boost/test/unit_test.hpp>

#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include "timeInfo.h"

namespace btime = boost::posix_time;


#ifdef WIN32

#include <Windows.h>

#undef min
#undef max

namespace
{
	class HighTime
	{
		ULONG     mPrevious;

		typedef LONG (__stdcall  *NtQueryTimerResolutionFun) (PULONG, PULONG, PULONG);
		typedef LONG (__stdcall  *NtSetTimerResolutionFun) (ULONG,BOOLEAN,PULONG);

		NtSetTimerResolutionFun SetFun;
		NtQueryTimerResolutionFun QueryFun;
		HMODULE mNtDll;

		static const ULONG DesiredDelay = 10000;//ns
	public:
		HighTime()
			:SetFun(0), QueryFun(0), mNtDll(0)
		{
			mNtDll = LoadLibrary(L"ntdll.dll");

			if( mNtDll )
			{
				QueryFun = (NtQueryTimerResolutionFun)GetProcAddress (mNtDll, "NtQueryTimerResolution");
				SetFun = (NtSetTimerResolutionFun)GetProcAddress (mNtDll, "NtSetTimerResolution");

				if( QueryFun && SetFun )
				{
					ULONG tMaxDelay = 0, tMinDelay = 0, tCurrDelay = 0;
					QueryFun(&tMaxDelay, &tMinDelay, &tCurrDelay);

					tCurrDelay = std::min(std::max(DesiredDelay, tMinDelay), tMaxDelay);

					SetFun(tCurrDelay, TRUE, &mPrevious);
				}
			}
		}

		~HighTime()
		{
			if( QueryFun && SetFun )
				SetFun(mPrevious, TRUE, &mPrevious);
		}
	};
}

#else

namespace
{
	class HighTime
	{
	}
}

#endif

namespace
{
	boost::shared_ptr<HighTime> initHighTiming()
	{
		static boost::weak_ptr<HighTime> tTime;
		boost::shared_ptr<HighTime> tResult = tTime.lock();
		if( !tResult )
		{
			tResult.reset(new HighTime);
			tTime = tResult;
		}

		return tResult;
	}
}

namespace test
{
	IBenchmarkOutput::~IBenchmarkOutput()
	{
	}

		
	void Printout::started(int aCount)
	{
		std::stringstream tOut;

		if( aCount > 1 )
			tOut << "-->Start benchmark '" << mFormat.str() << "', repeate " << aCount << " times" << " ...";
		else
			tOut << "-->Start benchmark '" << mFormat.str() << "' ...";

		BOOST_TEST_MESSAGE( tOut.str() );
	}
	void Printout::finished(boost::int64_t aMilliseconds)
	{
		std::stringstream tOut;

		tOut 
			<< "<--Benchmark '"
			<< mFormat.str() 
			<<"' finished, total milliseconds: "
			<< aMilliseconds;

		BOOST_TEST_MESSAGE( tOut.str() );
	}

	Printout::Printout(const char *aName)
		:mFormat(aName)
	{
	}

	//////////////////////////////////////////////////////////////////////////

	struct TestBenchmark::Private
	{
		size_t count;
		boost::shared_ptr<HighTime> hightTiming;

		boost::posix_time::ptime startTime;
		boost::posix_time::ptime finishTime;
		//std::string name;
		std::auto_ptr<IBenchmarkOutput> output;
	};

	void TestBenchmark::init(IBenchmarkOutput &aOutput, int aCount)
	{
		mPrivate = new Private;

		mPrivate->hightTiming = initHighTiming();

		mPrivate->output.reset(aOutput.clone());
		mPrivate->count		= aCount;
		mPrivate->startTime	= tools::get_system_time();

		mPrivate->output->started(aCount);
	}

	TestBenchmark::TestBenchmark(const char *aName, int aCount)
	{
		init(Printout(aName), aCount);
	}

	TestBenchmark::TestBenchmark(IBenchmarkOutput &aOutput, int aCount)
	{
		init(aOutput, aCount);
	}

	TestBenchmark::~TestBenchmark()
	{
		mPrivate->finishTime = tools::get_system_time();

		btime::time_duration tDuration 
			= mPrivate->finishTime - mPrivate->startTime;

		btime::time_duration::tick_type tTicks 
			= tDuration.total_milliseconds();

		mPrivate->output->finished(tTicks);

		delete mPrivate;
	}

	bool TestBenchmark::isDone()
	{
		return 0 == mPrivate->count;
	}

	void TestBenchmark::next()
	{
		mPrivate->count -- ; 
	}
}
