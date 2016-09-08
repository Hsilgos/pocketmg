#include "report.h"

#include <algorithm>
#include <boost/lexical_cast.hpp>

#include <iostream>

namespace {
template<class InputIterator, class T>
T accumulate2(InputIterator first, InputIterator last, T init) {
  while (first != last)
    init = init + *first++;  // or: init=binary_op(init,*first++) for the binary_op version
  return init;
}

void printValue(std::ostream& out, const std::string& value, size_t size) {
  out << value;
  for (size_t i = value.size(); i < size; ++i)
    out << " ";
}

typedef std::vector<std::string> StringList;
typedef std::vector<size_t> IntList;
void printValueS(std::ostream& out, const StringList& values, const IntList& sizes) {
  out << "|";
  for (size_t i = 0; i < values.size(); ++i) {
    printValue(out, values[i], sizes[i]);
    out << "|";
  }
  out << std::endl;
}
}

namespace test {
ReportOutput::ReportOutput(const std::string& row, const std::string& column, Report* report)
  : row_(row), column_(column), report_(report) {}

void ReportOutput::started(int) {}

void ReportOutput::finished(boost::int64_t milliseconds) {
  std::string info = boost::lexical_cast<std::string>(milliseconds);
  report_->addInfo(row_, column_, info);

  std::cout << "Finished: " << row_ << "/" << column_ << " = " << info << std::endl;
}

//////////////////////////////////////////////////////////////////////////

void Report::addInfo(const std::string& row, const std::string& column, const std::string& info) {
  if (rows_.end() == std::find(rows_.begin(), rows_.end(), row))
    rows_.push_back(row);

  if (columns_.end() == std::find(columns_.begin(), columns_.end(), column))
    columns_.push_back(column);

  table_[row][column] = info;
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

void Report::printTable(std::ostream& out) {
  IntList widths;

  for (StringList::const_iterator it = columns_.begin(), itEnd = columns_.end(); it != itEnd; ++it) {
    const std::string& tab_name = *it;
    widths.push_back(tab_name.size());

    for (TableContent::const_iterator itTab = table_.begin(), itTbEnd = table_.end(); itTab != itTbEnd; ++itTab) {
      const ColumnMap& columns = itTab->second;
      ColumnMap::const_iterator itContent = columns.find(tab_name);
      if (itContent != columns.end())
        widths.back() = std::max(widths.back(), itContent->second.size());
    }
  }

  size_t first_column_size = 0;
  for (StringList::const_iterator it = rows_.begin(), itEnd = rows_.end(); it != itEnd; ++it)
    first_column_size = std::max(first_column_size, it->size());


  const size_t head_sep_len = accumulate2(widths.begin(), widths.end(), 0) + widths.size() + 1;
  const std::string head_indent(first_column_size + 1, ' ');
  const std::string head_row_sep(head_sep_len, '-');

  const size_t table_sep_len = head_sep_len + first_column_size + 1;
  const std::string table_row_sep(table_sep_len, '-');

  // description
  if (!description_.empty()) {
    out << description_ << std::endl;
  }

  // print header
  out << head_indent << head_row_sep << std::endl;
  out << head_indent;
  printValueS(out, columns_, widths);
  out << table_row_sep << std::endl;

  // print content
  for (StringList::const_iterator it = rows_.begin(), itEnd = rows_.end(); it != itEnd; ++it) {
    StringList row_with_name;
    row_with_name.push_back(*it);

    ColumnMap& column_map = table_[*it];
    for (StringList::const_iterator itCol = columns_.begin(), itColEnd = columns_.end(); itCol != itColEnd; ++itCol)
      row_with_name.push_back(column_map[*itCol]);

    IntList widths_with_name = widths;
    widths_with_name.insert(widths_with_name.begin(), first_column_size);

    printValueS(out, row_with_name, widths_with_name);
    out << table_row_sep << std::endl;
  }
}

ReportOutput Report::output(const std::string& row, const std::string& column) {
  return ReportOutput(row, column, this);
}

void Report::setDescription(const std::string& desc) {
  description_ = desc;
}
}
