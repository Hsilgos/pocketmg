#pragma once

#include <string>
#include <vector>
#include <map>
#include <ostream>

#include "testBenchmark.h"

namespace test {
class Report;
class ReportOutput: public IBenchmarkOutput {
  const std::string row_;
  const std::string column_;

  Report *const report_;

  ReportOutput(const std::string &row, const std::string &column, Report *report);
  friend class Report;
public:
  PATTERN_IMPL_CLONE(ReportOutput);

  virtual void started(int count);
  virtual void finished(boost::int64_t milliseconds);
};

class Report {
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
  void setDescription(const std::string &desc);

  void addInfo(const std::string &row, const std::string &column, const std::string &info);

  void printTable(std::ostream &out);

  ReportOutput output(const std::string &row, const std::string &column);
};
}

