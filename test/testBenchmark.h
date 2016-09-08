#pragma once

#include "debugUtils.h"
#include <boost/cstdint.hpp>
#include <boost/format.hpp>

#include "clone.h"

namespace test {
class IBenchmarkOutput {
public:
  PATTERN_DEFINE_CLONE(IBenchmarkOutput)

  virtual ~IBenchmarkOutput();

  virtual void started(int count)   = 0;
  virtual void finished(boost::int64_t milliseconds) = 0;
};

class Printout : public IBenchmarkOutput {
  //const std::string name_;
  boost::format format_;

  PATTERN_IMPL_CLONE(Printout)

  virtual void started(int count);
  virtual void finished(boost::int64_t milliseconds);
public:
  Printout(const char* name);

  template<typename T>
  Printout& operator %(const T& value) {
    format_ % value;

    return *this;
  }
};

class TestBenchmark {
public:
  TestBenchmark(const char* name, int count);
  TestBenchmark(IBenchmarkOutput const& output, int count);
  ~TestBenchmark();
  void resetTimer();
  bool isDone();
  void next();

private:
  TestBenchmark(const TestBenchmark&);
  TestBenchmark& operator =(const TestBenchmark&);

  void init(IBenchmarkOutput const& output, int count);
  struct Private;
  Private* private_;
};
}

#define BENCHMARK_REPEAT(Name, CountDebug, CountRelease) \
  for (test::TestBenchmark benchmark(Name, (utils::isDebugging() ? (CountDebug) : (CountRelease))); !benchmark.isDone(); benchmark.next())

#define BENCHMARK(Name) BENCHMARK_REPEAT(Name, 1, 1)
#define GET_LOCAL_BECHMARK() benchmark
#define BENCHMARK_RESET_TIMER() GET_LOCAL_BECHMARK().resetTimer();
