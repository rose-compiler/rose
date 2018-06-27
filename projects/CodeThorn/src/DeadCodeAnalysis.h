#ifndef DEAD_CODE_ANALYSIS_H
#define DEAD_CODE_ANALYSIS_H

#include <string>
#include "IntervalAnalysis.h"
#include "LVAnalysis.h"
#include <unordered_set>
class DeadCodeAnalysis {
 public:
  void writeUnreachableCodeResultFile(SPRAY::IntervalAnalysis* intervalAnalysis,
                                      std::string csvDeadCodeUnreachableFileName);
  void writeDeadAssignmentResultFile(SPRAY::LVAnalysis* lvAnalysis,
                                     std::string deadCodeCsvFileName);
  void setOptionTrace(bool flag);
  void setOptionSourceCode(bool flag);
  void setOptionSystemHeaders(bool flag);
  // checks whether fileId is one of the input files. Initiales data structures on first call to this function.
  bool isInputFileId(int fileId);
 private:
  bool option_trace=false;
  // generates the source of the dead statement as last entry in the csv file
  bool optionSourceCode=false;
  // generates deadcode reports also for system headers. If false
  // system headers are excluded from the dead code report.
  bool optionSystemHeaders=false;
  // exclude any dead code found in the following header files
  std::unordered_set<string> excludedHeaders={"bits/byteswap.h"};
};

#endif
