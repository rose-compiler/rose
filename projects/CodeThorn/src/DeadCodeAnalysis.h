#ifndef DEAD_CODE_ANALYSIS_H
#define DEAD_CODE_ANALYSIS_H

#include <string>
#include "IntervalAnalysis.h"
#include "LVAnalysis.h"

class DeadCodeAnalysis {
 public:
  void writeUnreachableCodeResultFile(SPRAY::IntervalAnalysis* intervalAnalysis,
                                      std::string csvDeadCodeUnreachableFileName);
  void writeDeadAssignmentResultFile(SPRAY::LVAnalysis* lvAnalysis,
                                     std::string deadCodeCsvFileName);
  void setOptionTrace(bool flag);
 private:
  bool option_trace;
};

#endif
