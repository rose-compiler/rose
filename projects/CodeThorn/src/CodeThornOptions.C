
#include "CodeThornOptions.h"
#include "CodeThornException.h"
#include <iostream>

void CodeThornOptions::configurePrecisionOption() {
  switch(precisionLevel) {
  case 0:
    /* keep default configuration */
    break;
  case 1:
    intraProcedural=true;
    contextSensitive=false;
    abstractionMode=1;
    solver=16;
    if(arrayAbstractionIndex==-1)
      arrayAbstractionIndex=1;
    break;
  case 2:
    intraProcedural=false;
    contextSensitive=true;
    if(callStringLength<1)
      callStringLength=3;
    abstractionMode=1;
    solver=16;
    if(arrayAbstractionIndex==-1)
      arrayAbstractionIndex=1;
    
    break;
  case 3:
    intraProcedural=false;
    contextSensitive=true;
    abstractionMode=0;
    solver=5;
    arrayAbstractionIndex=-1;
    if(callStringLength<1)
      callStringLength=5;
    break;
  default:
    std::cerr<<"Unknown precision level selection (level="<<precisionLevel<<")"<<std::endl;
    exit(1);
  }
}

CodeThornOptions::CodeThornOptions() {
  CodeThornOptions::AnalysisListType analysisList={
    {CodeThorn::ANALYSIS_NULL_POINTER,"null-pointer"},
    {CodeThorn::ANALYSIS_OUT_OF_BOUNDS,"out-of-bounds"},
    {CodeThorn::ANALYSIS_UNINITIALIZED,"uninitialized"},
    {CodeThorn::ANALYSIS_DEAD_CODE,"dead-code"},
    {CodeThorn::ANALYSIS_OPAQUE_PREDICATE,"opaque-predicate"}
  };
  _analysisList=analysisList;
}

bool CodeThornOptions::getInterProceduralFlag() {
  return !getIntraProceduralFlag();
}

bool CodeThornOptions::getIntraProceduralFlag() {
  return intraProcedural;
}

bool CodeThornOptions::activeOptionsRequireZ3Library() {
  return z3BasedReachabilityAnalysis;
}

void CodeThornOptions::setAnalysisList(CodeThornOptions::AnalysisListType list) {
  _analysisList=list;
}

CodeThornOptions::AnalysisListType CodeThornOptions::analysisList() const {
  return _analysisList;
}

bool CodeThornOptions::getAnalysisSelectionFlag(CodeThorn::AnalysisSelector asel) {
  switch(asel) {
  case CodeThorn::ANALYSIS_NULL_POINTER: return nullPointerAnalysis;
  case CodeThorn::ANALYSIS_OUT_OF_BOUNDS: return outOfBoundsAnalysis;
  case CodeThorn::ANALYSIS_UNINITIALIZED: return uninitializedMemoryAnalysis;
  case CodeThorn::ANALYSIS_DEAD_CODE: return deadCodeAnalysis;
  case CodeThorn::ANALYSIS_OPAQUE_PREDICATE: return constantConditionAnalysis;
  default:
    throw CodeThorn::Exception("getAnalysisSelectionFlag: unknown analysis selector.");
  }
}

std::string CodeThornOptions::getAnalysisReportFileName(CodeThorn::AnalysisSelector asel) {
  switch(asel) {
  case CodeThorn::ANALYSIS_NULL_POINTER: return nullPointerAnalysisFileName;
  case CodeThorn::ANALYSIS_OUT_OF_BOUNDS: return outOfBoundsAnalysisFileName;
  case CodeThorn::ANALYSIS_UNINITIALIZED: return uninitializedMemoryAnalysisFileName;
  case CodeThorn::ANALYSIS_DEAD_CODE: return deadCodeAnalysisFileName;
  case CodeThorn::ANALYSIS_OPAQUE_PREDICATE: return constantConditionAnalysisFileName;
  default:
    throw CodeThorn::Exception("getAnalysisReportFileName: unknown analysis selector.");
  }
}
