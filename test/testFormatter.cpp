
#include "testFormatter.h"

#include <boost/date_time.hpp>
#include <boost/test/unit_test_log_formatter.hpp>
#include <boost/test/unit_test.hpp>
#include <boost/filesystem.hpp>

#include <iostream>
#include <string>

#include "testUtils.h"
#include "singleton.h"

namespace btest = boost::unit_test;

namespace test {

class TestLogFormatter: public btest::unit_test_log_formatter {
  unsigned int test_count_;
  unsigned int error_count_;
  unsigned int fatal_error_count_;
  unsigned int warning_count_;

  unsigned int check_count_;

  unsigned int color_;
  bool do_print_;
  bool is_message_;
  bool no_errors_;

  boost::posix_time::ptime unit_time_;
  boost::posix_time::ptime total_time_;

  // Destructor
  virtual ~TestLogFormatter() {}

  class Output {
    unsigned int	color_;
    std::stringstream	stream_;
  public:
    Output(unsigned int color = utils::CCWhite)
      :color_(color) {
    }

    ~Output() {
      utils::setConsoleColor(color_);
      utils::ideOutput(stream_.str().c_str(), false);
      std::cout << stream_.str();
      // return back
      utils::setConsoleColor(utils::CCWhite);
    }

    template <typename T>
    std::stringstream & operator << (const T &value) {
      stream_ << value;
      return stream_;
    }

    // this is the type of std::cout
    typedef std::basic_ostream<char, std::char_traits<char> > CoutType;

    // this is the function signature of std::endl
    typedef CoutType& (*StandardEndLine)(CoutType&);

    // define an operator<< to take in std::endl
    std::stringstream & operator<<(StandardEndLine manip) {
      stream_ << manip;
      return stream_;
    }
  };

  std::string duration2str(const boost::posix_time::time_duration &diff_time) {
    std::stringstream stream;

    if( diff_time.hours() > 0 )
      stream << " " << diff_time.hours() << "h";
    if( diff_time.minutes() > 0 )
      stream << " " << diff_time.minutes() << "m";
    if( diff_time.seconds() > 0 )
      stream << " " << diff_time.seconds() << "s";

    boost::posix_time::time_duration::tick_type millis =
      diff_time.total_milliseconds() - diff_time.total_seconds() * 1000;

    if( millis > 0 )
      stream << " " << millis << "ms";

    return stream.str();
  }

  // Formatter interface
  virtual void log_start( std::ostream&, btest::counter_t test_cases_count ) {
    total_time_ = boost::posix_time::microsec_clock::universal_time();
    test_count_ = test_cases_count;
    Output() << "Test started, "<< test_cases_count << " entries found" << std::endl;
  }

  virtual void log_finish( std::ostream&) {
    boost::posix_time::ptime now = boost::posix_time::microsec_clock::universal_time();
    boost::posix_time::time_duration diff_time = now - total_time_;

    Output() << "***************************" << std::endl;
    Output() << "Tests finished" << std::endl;
    Output() << "Total checks:   "<< check_count_ << std::endl;
    Output() << "Total time:    " << duration2str(diff_time) << std::endl;
    Output(fatal_error_count_>0?utils::CCRed:utils::CCWhite)				<< "Fatal errors:   "<< fatal_error_count_ << std::endl;
    Output(error_count_>0?utils::CCRed:utils::CCWhite)					<< "Errors:         "<< error_count_ << std::endl;
    Output(warning_count_>0?(utils::CCGreen|utils::CCRed):utils::CCWhite)<< "Warnings:       "<< warning_count_ << std::endl;
    //Output(FindMemoryLeaks::getCount()>0?utils::CCRed:utils::CCWhite)	<< "Memory leaks:   "<< FindMemoryLeaks::getCount() << std::endl;
    Output() << "***************************" << std::endl;
  }

  virtual void log_build_info( std::ostream& ) {
  }

  virtual void test_unit_start( std::ostream&, btest::test_unit const& tu ) {
    if(btest::tut_suite == tu.p_type) {
      Output() << "Suit \""<< tu.p_name << "\"is started." << std::endl;
    }
    if( btest::tut_case == tu.p_type ) {
      unit_time_ = boost::posix_time::microsec_clock::universal_time();
      Output() << "  Test \""<< tu.p_name << "\"...";// << std::endl;
      no_errors_ = true;
    }

  }

  virtual void test_unit_finish( std::ostream&, btest::test_unit const& tu, unsigned long /*elapsed*/ ) {
    if(btest::tut_suite == tu.p_type) {
      Output() << "Suit \""<< tu.p_name << "\"is finished." << std::endl;
    }
    if( btest::tut_case == tu.p_type && no_errors_ ) {
      boost::posix_time::ptime now = boost::posix_time::microsec_clock::universal_time();
      boost::posix_time::time_duration diff_time = now - unit_time_;
      Output() << duration2str(diff_time) << std::endl;
    }
  }

  virtual void test_unit_skipped( std::ostream&, btest::test_unit const& ) {

  }

  virtual void log_exception( std::ostream&, btest::log_checkpoint_data const& log_entry, btest::const_string explanation ) {
    error_count_++;
    check_count_++;
    Output(utils::CCRed) << log_entry.m_file_name <<"("<< log_entry.m_line_num<<") :" << explanation<< std::endl;
    color_ = utils::CCWhite;
    do_print_ = false;
  }

  virtual void log_entry_start( std::ostream&, btest::log_entry_data const& log_entry, log_entry_types type ) {
    switch(type) {
    case BOOST_UTL_ET_ERROR:
      error_count_++;
      check_count_++;
      color_ = utils::CCRed;
      if( no_errors_ )
        Output(color_) << std::endl;
      no_errors_ = false;
      Output(color_) << log_entry.m_file_name << "(" << log_entry.m_line_num<<") :";
      do_print_ = true;
      break;
    case BOOST_UTL_ET_FATAL_ERROR:
      fatal_error_count_++;
      check_count_++;
      color_ = utils::CCRed;
      if( no_errors_ )
        Output(color_) << std::endl;
      no_errors_ = false;
      Output(color_) << log_entry.m_file_name << "(" << log_entry.m_line_num<<") :";
      do_print_ = true;
      break;
    case BOOST_UTL_ET_WARNING:
      warning_count_++;
      check_count_++;
      color_ = utils::CCYellow;
      if( no_errors_ )
        Output(color_) << std::endl;
      no_errors_ = false;
      Output(color_) << log_entry.m_file_name << "(" << log_entry.m_line_num<<") :";
      do_print_ = true;
      break;
    case BOOST_UTL_ET_MESSAGE:
      color_ = utils::CCWhite;
      do_print_ = true;
      is_message_ = true;
      break;
    case BOOST_UTL_ET_INFO:
      check_count_++;
      color_ = utils::CCWhite;
      do_print_ = false;
    default:
      color_ = utils::CCWhite;
      do_print_ = false;
    }
  }

  virtual void log_entry_value( std::ostream&, btest::const_string value ) {
    if ( do_print_ )
      Output(color_) << value;
  }
  //virtual void        log_entry_value( std::ostream&, lazy_ostream const& value ); // there is a default impl
  virtual void log_entry_finish( std::ostream&) {
    if ( do_print_ ) {
      Output() << std::endl;
      do_print_ = false;
      is_message_ = false;
    }
  }
public:
  TestLogFormatter()
    :test_count_(0),
     error_count_(0),
     fatal_error_count_(0),
     warning_count_(0),
     check_count_(0),
     color_(utils::CCWhite),
     do_print_(false),
     is_message_(false),
     no_errors_(true) {
  }
};

class TestConfigData: public utils::SingletonStatic<TestConfigData> {
  LogFormatter * formatter_;
public:
  TestConfigData()
    :formatter_(0) {
  }

  void setFormatter(LogFormatter *formatter) {
    formatter_ = formatter;
  }

  LogFormatter *getFormatter() {
    if( formatter_ )
      return formatter_;

    return new test::TestLogFormatter;
  }
};

void setDefTestFormatter(LogFormatter *formatter) {
  test::TestConfigData::getInstance().setFormatter(formatter);
}

int DoExternalRunStr(InitTestFunc init_func, const std::string &args, LogFormatter *form) {
  std::vector<char> buff(args.begin(), args.end());
  buff.push_back('\0');
  char *args_parsed[1000];
  int argc_parsed = 0;
  char empty[] = "";

  args_parsed[argc_parsed++] = empty;

  bool prev_space = true;

  for ( size_t i = 0; i < buff.size(); ++i ) {
    if(  buff[i] == ' ' ) {
      buff[i] = 0;
      prev_space = true;
    } else if( buff[i] != ' ' && prev_space ) {
      args_parsed[argc_parsed++] = &buff[i];
      prev_space = false;
    }
  }

  return DoExternalRunMain(init_func, argc_parsed, args_parsed, form);
}

int DoExternalRunMain(InitTestFunc init_func, int argc, char* argv[], LogFormatter *form) {
  //test::TestConfigData::getInstance().setFormatter(form);
  test::setDefTestFormatter(form);
  return boost::unit_test::unit_test_main(init_func, argc, argv);
}
}

TestGlobalFixture::TestGlobalFixture() {
  boost::unit_test::unit_test_log.set_threshold_level( boost::unit_test::log_successful_tests );
  boost::unit_test::unit_test_log.set_formatter( test::TestConfigData::getInstance().getFormatter() );
}

TestGlobalFixture::~TestGlobalFixture() {
}

