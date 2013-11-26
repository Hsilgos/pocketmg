#include "report.h"

#include <algorithm>
#include <boost/lexical_cast.hpp>

#include <iostream>

namespace
{
	template <class InputIterator, class T>
	T accumulate ( InputIterator first, InputIterator last, T init )
	{
		while ( first!=last )
			init = init + *first++;  // or: init=binary_op(init,*first++) for the binary_op version
		return init;
	}

	void printValue(std::ostream &aOut, const std::string &aValue, size_t aSize)
	{
		aOut << aValue;
		for( size_t i = aValue.size(); i < aSize; ++i )
			aOut << " ";
	}

	typedef std::vector< std::string > StringList;
	typedef std::vector< size_t > IntList;
	void printValueS(std::ostream &aOut, const StringList &aValues, const IntList &aSizes)
	{
		aOut << "|";
		for( size_t i = 0 ; i < aValues.size(); ++i )
		{
			printValue(aOut, aValues[i], aSizes[i]);
			aOut << "|";
		}
		aOut << std::endl;
	}
}

namespace test
{
	ReportOutput::ReportOutput(const std::string &aRow, const std::string &aColumn, Report *aReport)
		:row_(aRow), column_(aColumn), report_(aReport)
	{
	}

	void ReportOutput::started(int)
	{
	}

	void ReportOutput::finished(boost::int64_t aMilliseconds)
	{
		std::string tInfo = boost::lexical_cast<std::string>(aMilliseconds);
		report_->addInfo(row_, column_, tInfo);

		std::cout << "Finished: " << row_ << "/" << column_ << " = " << tInfo << std::endl;
	}

	//////////////////////////////////////////////////////////////////////////

	void Report::addInfo(const std::string &aRow, const std::string &aColumn, const std::string &aInfo)
	{
		if( rows_.end() == std::find(rows_.begin(), rows_.end(), aRow) )
			rows_.push_back(aRow);

		if( columns_.end() == std::find(columns_.begin(), columns_.end(), aColumn) )
			columns_.push_back(aColumn);

		table_[aRow][aColumn] = aInfo;
	}


	/*
	        ---------------------
	        |Head1|Head1  |Head3|
    -----------------------------
	|row1   |10   |20     |30   |
	-----------------------------
	|row 2  |1    |2      |3    |
	-----------------------------
	|row  30|301  |1234556|321  |
	-----------------------------
	*/

	void Report::printTable(std::ostream &aOut)
	{
		IntList tWidths;

		for( StringList::const_iterator it = columns_.begin(), itEnd = columns_.end(); it != itEnd; ++it )
		{
			const std::string &tTabName = *it;
			tWidths.push_back( tTabName.size() );

			for( TableContent::const_iterator itTab = table_.begin(), itTbEnd = table_.end(); itTab != itTbEnd; ++itTab )
			{
				const ColumnMap &tColumns = itTab->second;
				ColumnMap::const_iterator itContent = tColumns.find( tTabName );
				if( itContent != tColumns.end() )
					tWidths.back() = std::max(tWidths.back(), itContent->second.size() );
			}
		}

		size_t tFirstColumnSize = 0;
		for( StringList::const_iterator it = rows_.begin(), itEnd = rows_.end(); it != itEnd; ++it )
			tFirstColumnSize = std::max(tFirstColumnSize, it->size());

		
		const size_t tHeadSepLen = accumulate(tWidths.begin(), tWidths.end(), 0) + tWidths.size() + 1;
		const std::string tHeadIndent(tFirstColumnSize + 1, ' ');
		const std::string tHeadRowSep(tHeadSepLen, '-');

		const size_t tTableSepLen = tHeadSepLen + tFirstColumnSize + 1;
		const std::string tTableRowSep(tTableSepLen, '-');

		// description
		if( !description_.empty() )
		{
			aOut << description_ << std::endl;
		}

		// print header
		aOut << tHeadIndent << tHeadRowSep << std::endl;
		aOut << tHeadIndent;
		printValueS(aOut, columns_, tWidths);
		aOut << tTableRowSep << std::endl;

		// print content
		for( StringList::const_iterator it = rows_.begin(), itEnd = rows_.end(); it != itEnd; ++it )
		{
			StringList tRowWithName;
			tRowWithName.push_back(*it);

			ColumnMap &tColumnMap = table_[*it];
			for( StringList::const_iterator itCol = columns_.begin(), itColEnd = columns_.end(); itCol != itColEnd; ++itCol )
				tRowWithName.push_back(tColumnMap[*itCol]);

			IntList tWidthsWithName = tWidths;
			tWidthsWithName.insert(tWidthsWithName.begin(), tFirstColumnSize);

			printValueS(aOut, tRowWithName, tWidthsWithName);
			aOut << tTableRowSep << std::endl;
		}
	}

	ReportOutput Report::output(const std::string &aRow, const std::string &aColumn)
	{
		return ReportOutput(aRow, aColumn, this);
	}

	void Report::setDescription(const std::string &aDesc)
	{
		description_ = aDesc;
	}
}

