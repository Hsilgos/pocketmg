#pragma once

#include <string>
#include <vector>
#include <map>
#include <ostream>

#include "testBenchmark.h"

namespace test
{
	class Report;
	class ReportOutput: public IBenchmarkOutput
	{
		const std::string row_;
		const std::string column_;

		Report *const report_;

		ReportOutput(const std::string &aRow, const std::string &aColumn, Report *aReport);
		friend class Report;
	public:
		PATTERN_IMPL_CLONE(ReportOutput);

		virtual void started(int aCount);
		virtual void finished(boost::int64_t aMilliseconds);
	};

	class Report
	{
		typedef std::vector< std::string > StringList;
		// column names
		StringList columns_;

		// row name
		StringList rows_;

		// table content
		typedef std::map<std::string, std::string> ColumnMap;
		typedef std::map< std::string, ColumnMap > TableContent;
		TableContent table_;

		std::string description_;
	public:
		void setDescription(const std::string &aDesc);

		void addInfo(const std::string &aRow, const std::string &aColumn, const std::string &aInfo);

		void printTable(std::ostream &aOut);

		ReportOutput output(const std::string &aRow, const std::string &aColumn);
	};
}

