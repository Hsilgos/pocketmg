
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

namespace test
{	

	class TestLogFormatter: public btest::unit_test_log_formatter
	{
		unsigned int mTestCount;
		unsigned int mErrorCount;
		unsigned int mFatalErrorCount;
		unsigned int mWarningCount;

		unsigned int mCheckCount;

		unsigned int mColor;
		bool mDoPrint;
		bool mIsMessage;
		bool mNoErrors;

		boost::posix_time::ptime mUnitTime;
		boost::posix_time::ptime mTotalTime;

		// Destructor
		virtual ~TestLogFormatter() {}

		class Output
		{
			unsigned int	mColor;
			std::stringstream	mStream;
		public:
			Output(unsigned int aColor = utils::CCWhite)
				:mColor(aColor)
			{
			}

			~Output()
			{
				utils::setConsoleColor(mColor);
				utils::ideOutput(mStream.str().c_str(), false);
				std::cout << mStream.str();
				// return back
				utils::setConsoleColor(utils::CCWhite);
			}

			template <typename T>
			std::stringstream & operator << (const T &aValue) 
			{
				mStream << aValue;
				return mStream;
			}

			// this is the type of std::cout
			typedef std::basic_ostream<char, std::char_traits<char> > CoutType;

			// this is the function signature of std::endl
			typedef CoutType& (*StandardEndLine)(CoutType&);

			// define an operator<< to take in std::endl
			std::stringstream & operator<<(StandardEndLine manip)
			{
				mStream << manip;
				return mStream;
			}
		};

		std::string duration2str(const boost::posix_time::time_duration &aDiffTime)
		{
			std::stringstream tStream;

			if( aDiffTime.hours() > 0 )
				tStream << " " << aDiffTime.hours() << "h";
			if( aDiffTime.minutes() > 0 )
				tStream << " " << aDiffTime.minutes() << "m";
			if( aDiffTime.seconds() > 0 )
				tStream << " " << aDiffTime.seconds() << "s";

			boost::posix_time::time_duration::tick_type tMillis = 
				aDiffTime.total_milliseconds() - aDiffTime.total_seconds() * 1000;

			if( tMillis > 0 )
				tStream << " " << tMillis << "ms";

			return tStream.str();
		}

		// Formatter interface
		virtual void log_start( std::ostream&, btest::counter_t aTestCasesCount )
		{
			mTotalTime = boost::posix_time::microsec_clock::universal_time();
			mTestCount = aTestCasesCount;
			Output() << "Test started, "<< aTestCasesCount << " entries found" << std::endl;
		}

		virtual void log_finish( std::ostream&)
		{
			boost::posix_time::ptime tNow = boost::posix_time::microsec_clock::universal_time();
			boost::posix_time::time_duration tDiffTime = tNow - mTotalTime;

			Output() << "***************************" << std::endl;
			Output() << "Tests finished" << std::endl;
			Output() << "Total checks:   "<< mCheckCount << std::endl;
			Output() << "Total time:    " << duration2str(tDiffTime) << std::endl;
			Output(mFatalErrorCount>0?utils::CCRed:utils::CCWhite)				<< "Fatal errors:   "<< mFatalErrorCount << std::endl;
			Output(mErrorCount>0?utils::CCRed:utils::CCWhite)					<< "Errors:         "<< mErrorCount << std::endl;
			Output(mWarningCount>0?(utils::CCGreen|utils::CCRed):utils::CCWhite)<< "Warnings:       "<< mWarningCount << std::endl;
			//Output(FindMemoryLeaks::getCount()>0?utils::CCRed:utils::CCWhite)	<< "Memory leaks:   "<< FindMemoryLeaks::getCount() << std::endl;
			Output() << "***************************" << std::endl;
		}

		virtual void log_build_info( std::ostream& )
		{
		}

		virtual void test_unit_start( std::ostream&, btest::test_unit const& tu )
		{
			if(btest::tut_suite == tu.p_type)
			{
				Output() << "Suit \""<< tu.p_name << "\"is started." << std::endl;
			}
			if( btest::tut_case == tu.p_type )
			{
				mUnitTime = boost::posix_time::microsec_clock::universal_time();
				Output() << "  Test \""<< tu.p_name << "\"...";// << std::endl;
				mNoErrors = true;
			}

		}

		virtual void test_unit_finish( std::ostream&, btest::test_unit const& tu, unsigned long /*elapsed*/ )
		{
			if(btest::tut_suite == tu.p_type)
			{
				Output() << "Suit \""<< tu.p_name << "\"is finished." << std::endl;
			}
			if( btest::tut_case == tu.p_type && mNoErrors )
			{
				boost::posix_time::ptime tNow = boost::posix_time::microsec_clock::universal_time();
				boost::posix_time::time_duration tDiffTime = tNow - mUnitTime;
				Output() << duration2str(tDiffTime) << std::endl;
			}
		}

		virtual void test_unit_skipped( std::ostream&, btest::test_unit const& )
		{

		}

		virtual void log_exception( std::ostream&, btest::log_checkpoint_data const& aLogEntry, btest::const_string aExplanation )
		{
			mErrorCount++;
			mCheckCount++;
			Output(utils::CCRed) << aLogEntry.m_file_name <<"("<< aLogEntry.m_line_num<<") :" << aExplanation<< std::endl;
			mColor = utils::CCWhite;
			mDoPrint = false;
		}

		virtual void log_entry_start( std::ostream&, btest::log_entry_data const& aLogEntry, log_entry_types aType )
		{
			switch(aType)
			{
			case BOOST_UTL_ET_ERROR:
				mErrorCount++;
				mCheckCount++;
				mColor = utils::CCRed;
				if( mNoErrors )
					Output(mColor) << std::endl;
				mNoErrors = false;
				Output(mColor) << aLogEntry.m_file_name << "(" << aLogEntry.m_line_num<<") :";
				mDoPrint = true;
				break;
			case BOOST_UTL_ET_FATAL_ERROR:
				mFatalErrorCount++;
				mCheckCount++;
				mColor = utils::CCRed;
				if( mNoErrors )
					Output(mColor) << std::endl;
				mNoErrors = false;
				Output(mColor) << aLogEntry.m_file_name << "(" << aLogEntry.m_line_num<<") :";
				mDoPrint = true;
				break;
			case BOOST_UTL_ET_WARNING:
				mWarningCount++;
				mCheckCount++;
				mColor = utils::CCYellow;
				if( mNoErrors )
					Output(mColor) << std::endl;
				mNoErrors = false;
				Output(mColor) << aLogEntry.m_file_name << "(" << aLogEntry.m_line_num<<") :";
				mDoPrint = true;
				break;
			case BOOST_UTL_ET_MESSAGE:
				mColor = utils::CCWhite;
				mDoPrint = true;
				mIsMessage = true;
				break;
			case BOOST_UTL_ET_INFO:
				mCheckCount++;
				mColor = utils::CCWhite;
				mDoPrint = false;
			default:
				mColor = utils::CCWhite;
				mDoPrint = false;
			}
		}

		virtual void log_entry_value( std::ostream&, btest::const_string aValue )
		{
			if ( mDoPrint )
				Output(mColor) << aValue;
		}
		//virtual void        log_entry_value( std::ostream&, lazy_ostream const& value ); // there is a default impl
		virtual void log_entry_finish( std::ostream&)
		{
			if ( mDoPrint )
			{
				Output() << std::endl;
				mDoPrint = false;
				mIsMessage = false;
			}
		}
	public:
		TestLogFormatter()
			:mTestCount(0),
			mErrorCount(0),
			mFatalErrorCount(0),
			mWarningCount(0),
			mCheckCount(0),
			mColor(utils::CCWhite),
			mDoPrint(false),
			mIsMessage(false),
			mNoErrors(true)
		{
		}
	};

	class TestConfigData: public utils::SingletonStatic<TestConfigData>
	{
		LogFormatter * mFormatter;
	public:
		TestConfigData()
			:mFormatter(0)
		{
		}

		void setFormatter(LogFormatter *aFormatter)
		{
			mFormatter = aFormatter;
		}

		LogFormatter *getFormatter()
		{
			if( mFormatter )
				return mFormatter;

			return new test::TestLogFormatter; 
		}
	};

	void setDefTestFormatter(LogFormatter *aFormatter)
	{
		test::TestConfigData::getInstance().setFormatter(aFormatter);
	}

	int DoExternalRunStr(InitTestFunc aInitFunc, const std::string &aArgs, LogFormatter *aForm)
	{
		std::vector<char> tBuff(aArgs.begin(), aArgs.end());
		tBuff.push_back('\0');
		char *tArgs[1000];
		int tArgc = 0;
		char tEmpty[] = "";

		tArgs[tArgc++] = tEmpty;

		bool tPrevSpace = true;

		for ( size_t i = 0; i < tBuff.size(); ++i )
		{
			if(  tBuff[i] == ' ' )
			{
				tBuff[i] = 0;
				tPrevSpace = true;
			}
			else if( tBuff[i] != ' ' && tPrevSpace )
			{
				tArgs[tArgc++] = &tBuff[i];
				tPrevSpace = false;
			}
		}

		return DoExternalRunMain(aInitFunc, tArgc, tArgs, aForm);
	}

	int DoExternalRunMain(InitTestFunc aInitFunc, int aArgc, char* aArgv[], LogFormatter *aForm)
	{
		//test::TestConfigData::getInstance().setFormatter(aForm);
		test::setDefTestFormatter(aForm);
		return boost::unit_test::unit_test_main(aInitFunc, aArgc, aArgv);
	}
}

TestGlobalFixture::TestGlobalFixture()
{
	boost::unit_test::unit_test_log.set_threshold_level( boost::unit_test::log_successful_tests );
	boost::unit_test::unit_test_log.set_formatter( test::TestConfigData::getInstance().getFormatter() );
}

TestGlobalFixture::~TestGlobalFixture()
{
}

