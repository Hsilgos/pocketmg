#pragma once

#include "debugUtils.h"
#include <boost/cstdint.hpp>
#include <boost/format.hpp>

#include "clone.h"

namespace test {
class IBenchmarkOutput {
public:
  PATTERN_DEFINE_CLONE(IBenchmarkOutput);

  virtual ~IBenchmarkOutput();

  virtual void started(int count)			= 0;
  virtual void finished(boost::int64_t milliseconds)	= 0;
};

class Printout: public IBenchmarkOutput {
  //const std::string name_;
  boost::format	format_;

  PATTERN_IMPL_CLONE(Printout);

  virtual void started(int count);
  virtual void finished(boost::int64_t milliseconds);
public:
  Printout(const char *name);

  template <typename T>
  Printout& operator %(const T &value) {
    format_ % value;

    return *this;
  }
};

class TestBenchmark {
  struct Private;
  Private *private_;

  TestBenchmark(const TestBenchmark &);
  TestBenchmark &operator = (const TestBenchmark &);

  void init(IBenchmarkOutput &output, int count);
public:
  TestBenchmark(const char *name, int count);
  TestBenchmark(IBenchmarkOutput &output, int count);
  ~TestBenchmark();
  bool isDone();
  void next();
};
}

#define BENCHMARK_REPEAT( Name, CountDebug, CountRelease ) \
	for(test::TestBenchmark bench(Name, (utils::isDebugging()?(CountDebug):(CountRelease))); !bench.isDone(); bench.next() )

#define BENCHMARK( Name ) BENCHMARK_REPEAT( Name, 1, 1 )
