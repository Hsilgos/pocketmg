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
		const std::string mRow;
		const std::string mColumn;

		Report *const mReport;

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
		StringList mColumns;

		// row name
		StringList mRows;

		// table content
		typedef std::map<std::string, std::string> ColumnMap;
		typedef std::map< std::string, ColumnMap > TableContent;
		TableContent mTable;

		std::string mDescription;
	public:
		void setDescription(const std::string &aDesc);

		void addInfo(const std::string &aRow, const std::string &aColumn, const std::string &aInfo);

		void printTable(std::ostream &aOut);

		ReportOutput output(const std::string &aRow, const std::string &aColumn);
	};
}

