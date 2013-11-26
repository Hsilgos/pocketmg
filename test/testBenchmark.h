#pragma once

#include "debugUtils.h"
#include <boost/cstdint.hpp>
#include <boost/format.hpp>

#include "clone.h"

namespace test
{
	class IBenchmarkOutput
	{
	public:
		PATTERN_DEFINE_CLONE(IBenchmarkOutput);

		virtual ~IBenchmarkOutput();

		virtual void started(int aCount)			= 0;
		virtual void finished(boost::int64_t aMilliseconds)	= 0;
	};

	class Printout: public IBenchmarkOutput
	{
		//const std::string mName;
		boost::format	format_;

		PATTERN_IMPL_CLONE(Printout);

		virtual void started(int aCount);
		virtual void finished(boost::int64_t aMilliseconds);
	public:
		Printout(const char *aName);

		template <typename T>
		Printout& operator %(const T &aValue) 
		{
			format_ % aValue;

			return *this;
		}
	};

	class TestBenchmark
	{
		struct Private;
		Private *private_;

		TestBenchmark(const TestBenchmark &);
		TestBenchmark &operator = (const TestBenchmark &);

		void init(IBenchmarkOutput &aOutput, int aCount);
	public:
		TestBenchmark(const char *aName, int aCount);
		TestBenchmark(IBenchmarkOutput &aOutput, int aCount);
		~TestBenchmark();
		bool isDone();
		void next();
	};
}

#define BENCHMARK_REPEAT( Name, CountDebug, CountRelease ) \
	for(test::TestBenchmark bench(Name, (utils::isDebugging()?(CountDebug):(CountRelease))); !bench.isDone(); bench.next() )

#define BENCHMARK( Name ) BENCHMARK_REPEAT( Name, 1, 1 ) 
