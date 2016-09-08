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

namespace {
class HighTime {
  ULONG previous_;

  typedef LONG (__stdcall  * NtQueryTimerResolutionFun)(PULONG, PULONG, PULONG);
  typedef LONG (__stdcall  * NtSetTimerResolutionFun)(ULONG, BOOLEAN, PULONG);

  NtSetTimerResolutionFun SetFun;
  NtQueryTimerResolutionFun QueryFun;
  HMODULE nt_dll_;

  static const ULONG DesiredDelay = 10000; //ns
public:
  HighTime()
    : SetFun(0), QueryFun(0), nt_dll_(0) {
    nt_dll_ = LoadLibrary(L"ntdll.dll");

    if (nt_dll_) {
      QueryFun = (NtQueryTimerResolutionFun)GetProcAddress(nt_dll_, "NtQueryTimerResolution");
      SetFun = (NtSetTimerResolutionFun)GetProcAddress(nt_dll_, "NtSetTimerResolution");

      if (QueryFun && SetFun) {
        ULONG max_delay = 0, min_delay = 0, curr_delay = 0;
        QueryFun(&max_delay, &min_delay, &curr_delay);

        curr_delay = std::min(std::max(DesiredDelay, min_delay), max_delay);

        SetFun(curr_delay, TRUE, &previous_);
      }
    }
  }

  ~HighTime() {
    if (QueryFun && SetFun)
      SetFun(previous_, TRUE, &previous_);
  }
};
}

#else

namespace {
class HighTime {};
}

#endif

namespace {
boost::shared_ptr<HighTime> initHighTiming() {
  static boost::weak_ptr<HighTime> time;
  boost::shared_ptr<HighTime> result = time.lock();
  if (!result) {
    result.reset(new HighTime);
    time = result;
  }

  return result;
}
}

namespace test {
IBenchmarkOutput::~IBenchmarkOutput() {}

void Printout::started(int count) {
  std::stringstream out;

  if (count > 1)
    out << "-->Start benchmark '" << format_.str() << "', repeate " << count << " times" << " ...";
  else
    out << "-->Start benchmark '" << format_.str() << "' ...";

  BOOST_TEST_MESSAGE(out.str());
}

void Printout::finished(boost::int64_t milliseconds) {
  std::stringstream out;

  out
  << "<--Benchmark '"
  << format_.str()
  << "' finished, total milliseconds: "
  << milliseconds;

  BOOST_TEST_MESSAGE(out.str());
}

Printout::Printout(const char* name)
  : format_(name) {}

//////////////////////////////////////////////////////////////////////////

struct TestBenchmark::Private {
  size_t count;
  boost::shared_ptr<HighTime> hightTiming;

  boost::posix_time::ptime startTime;
  boost::posix_time::ptime finishTime;
  //std::string name;
  std::auto_ptr<IBenchmarkOutput> output;
};

void TestBenchmark::init(IBenchmarkOutput const& output, int count) {
  private_ = new Private;

  private_->hightTiming = initHighTiming();

  private_->output.reset(output.clone());
  private_->count  = count;
  private_->startTime = tools::get_system_time();

  private_->output->started(count);
}

void TestBenchmark::resetTimer() {
  private_->startTime = tools::get_system_time();
}

TestBenchmark::TestBenchmark(const char* name, int count) {
  Printout printout(name);
  init(printout, count);
}

TestBenchmark::TestBenchmark(IBenchmarkOutput const& output, int count) {
  init(output, count);
}

TestBenchmark::~TestBenchmark() {
  private_->finishTime = tools::get_system_time();

  btime::time_duration duration
    = private_->finishTime - private_->startTime;

  btime::time_duration::tick_type ticks
    = duration.total_milliseconds();

  private_->output->finished(ticks);

  delete private_;
}

bool TestBenchmark::isDone() {
  return 0 == private_->count;
}

void TestBenchmark::next() {
  private_->count--;
}
}
