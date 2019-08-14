#ifndef DEAD_CODE_ANALYSIS_H
#define DEAD_CODE_ANALYSIS_H

#include <string>
#include "IntervalAnalysis.h"
#include "LVAnalysis.h"
#include <unordered_set>
class DeadCodeAnalysis {
 public:
  void writeUnreachableCodeResultFile(CodeThorn::IntervalAnalysis* intervalAnalysis,
                                      std::string csvDeadCodeUnreachableFileName);
  void writeDeadAssignmentResultFile(CodeThorn::LVAnalysis* lvAnalysis,
                                     std::string deadCodeCsvFileName);
  void setOptionTrace(bool flag);
  void setOptionSourceCode(bool flag);
  void setOptionSystemHeaders(bool flag);
  void setOptionFilePath(bool flag);
 private:
  bool option_trace=false;
  // generates the source of the dead statement as last entry in the csv file
  bool optionSourceCode=false;
  // generates deadcode reports also for system headers. If false
  // system headers are excluded from the dead code report.
  bool optionSystemHeaders=false;
  // report the file's path in the deadcode report
  bool optionFilePath=false;
  std::unordered_set<string> excludedHeaders={"bits/byteswap.h"};
};

#endif
