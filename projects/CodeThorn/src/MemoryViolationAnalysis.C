#include "sage3basic.h"
#include "MemoryViolationAnalysis.h"
#include <iostream>

using namespace std;

namespace CodeThorn {

  MemoryViolationAnalysis::MemoryViolationAnalysis() {
  }

  MemoryViolationAnalysis::~MemoryViolationAnalysis() {
  }

  MemoryViolationAnalysis::OutOfBoundsAccessMap* MemoryViolationAnalysis::getResultMapPtr() {
    return &outOfBoundsAccessMap;
  }

  void MemoryViolationAnalysis::readingFromMemoryLocation(Label lab, PStatePtr pstate, AbstractValue& memLoc, AbstractValue& result) {
    cout<<"AOB ANALYSIS: @"<<lab.toString()<<": READ: @"<< memLoc.toString()<<endl;
  }

  void MemoryViolationAnalysis::writingToMemoryLocation(Label lab, PStatePtr pstate, AbstractValue& memLoc, AbstractValue& newValue) {
    cout<<"AOB ANALYSIS: @"<<lab.toString()<<": WRITE: @"<< memLoc.toString()<<" = "<<newValue<<endl;
  }

}
