
#include "CodeThornOptions.h"
#include "CodeThornException.h"

bool CodeThornOptions::activeOptionsRequireZ3Library() {
  return z3BasedReachabilityAnalysis;
}

CodeThornOptions::AnalysisListType CodeThornOptions::analysisList() const {
  CodeThornOptions::AnalysisListType analysisNames={
    {CodeThorn::ANALYSIS_NULL_POINTER,"null-pointer"},
    {CodeThorn::ANALYSIS_OUT_OF_BOUNDS,"out-of-bounds"},
    {CodeThorn::ANALYSIS_UNINITIALIZED,"uninitialized"}
  };
  return analysisNames;
}

bool CodeThornOptions::getAnalysisSelectionFlag(CodeThorn::AnalysisSelector asel) {
  switch(asel) {
  case CodeThorn::ANALYSIS_NULL_POINTER: return nullPointerAnalysis;
  case CodeThorn::ANALYSIS_OUT_OF_BOUNDS: return outOfBoundsAnalysis;
  case CodeThorn::ANALYSIS_UNINITIALIZED: return uninitializedMemoryAnalysis;
  default:
    throw CodeThorn::Exception("getAnalysisSelectionFlag: unknown analysis selector.");
  }
}

std::string CodeThornOptions::getAnalysisReportFileName(CodeThorn::AnalysisSelector asel) {
  switch(asel) {
  case CodeThorn::ANALYSIS_NULL_POINTER: return nullPointerAnalysisFileName;
  case CodeThorn::ANALYSIS_OUT_OF_BOUNDS: return outOfBoundsAnalysisFileName;
  case CodeThorn::ANALYSIS_UNINITIALIZED: return uninitializedMemoryAnalysisFileName;
  default:
    throw CodeThorn::Exception("getAnalysisReportFileName: unknown analysis selector.");
  }
}
