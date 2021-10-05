#include "sage3basic.h"
#include "MemoryViolationAnalysis.h"
#include <iostream>

using namespace std;

namespace CodeThorn {

  MemoryViolationAnalysis::MemoryViolationAnalysis() {
  }

  MemoryViolationAnalysis::~MemoryViolationAnalysis() {
  }

  void MemoryViolationAnalysis::readingFromMemoryLocation(Label lab, PStatePtr pstate, AbstractValue& memLoc, AbstractValue& result) {
    //cout<<"MEM CHECK: @"<<lab.toString()<<": READ: @"<< memLoc.toString()<<endl;
  }

  void MemoryViolationAnalysis::writingToMemoryLocation(Label lab, PStatePtr pstate, AbstractValue& memLoc, AbstractValue& newValue) {
    //cout<<"MEM CHECK: @"<<lab.toString()<<": WRITE: @"<< memLoc.toString()<<" = "<<newValue<<endl;
  }

  MemoryViolationAnalysis::MemoryAccessViolationType MemoryViolationAnalysis::checkMemoryAddress(AbstractValue& memLoc) {
    // check memLoc w.r.t. AbstractValue::getVariableIdMapping()
    return MemoryViolationAnalysis::MemoryAccessViolationType::ACCESS_POTENTIALLY_OUTSIDE_BOUNDS; // unknown
  }

}
