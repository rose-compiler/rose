
#include <iostream>
#include <cstdint>
#include <limits>
#include "CodeThornOptions.h"
#include "CodeThornException.h"
#include "CppStdUtilities.h"

void CodeThornOptions::configurePrecisionOption() {
  switch(precisionLevel) {
  case 0:
    /* keep default configuration */
    break;
  case 1:
    intraProcedural=true;
    contextSensitive=false;
    abstractionMode=1;
    if(arrayAbstractionIndex==-1)
      arrayAbstractionIndex=0;
    break;
  case 2:
    intraProcedural=false;
    contextSensitive=true;
    abstractionMode=1;
    if(arrayAbstractionIndex==-1)
      arrayAbstractionIndex=0;
    break;
  case 3:
    intraProcedural=false;
    contextSensitive=true;
    abstractionMode=0;
    if(solver!=5) {
      std::cout<<"STATUS: overriding solver "<<solver<<" with 5 (required by precision 3)"<<std::endl;
      solver=5; // override solver, L3 requires solver 5
    }
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
  reportFilePath="./";
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

void CodeThornOptions::setAnalysisSelectionFlag(CodeThorn::AnalysisSelector asel, bool flag) {
  switch(asel) {
  case CodeThorn::ANALYSIS_NULL_POINTER: nullPointerAnalysis=flag;break;
  case CodeThorn::ANALYSIS_OUT_OF_BOUNDS: outOfBoundsAnalysis=flag;break;
  case CodeThorn::ANALYSIS_UNINITIALIZED: uninitializedMemoryAnalysis=flag;break;
  case CodeThorn::ANALYSIS_DEAD_CODE: deadCodeAnalysis=flag;break;
  case CodeThorn::ANALYSIS_OPAQUE_PREDICATE: constantConditionAnalysis=flag;break;
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

uint64_t CodeThornOptions::getRegisterRangeStart() {
  return CppStdUtilities::convertStringToNumber(registerAddressStartString);
}

uint64_t CodeThornOptions::getRegisterRangeEnd() {
  if(registerAddressEndString.size()==0) {
    return std::numeric_limits<uint64_t>::max();
  }
  return CppStdUtilities::convertStringToNumber(registerAddressEndString);
}
