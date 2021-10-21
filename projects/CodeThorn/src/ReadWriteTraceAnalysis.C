#include "sage3basic.h"
#include "ReadWriteTraceAnalysis.h"
#include <iostream>

using namespace std;

namespace CodeThorn {

  ReadWriteTraceAnalysis::ReadWriteTraceAnalysis() {
  }

  ReadWriteTraceAnalysis::~ReadWriteTraceAnalysis() {
  }

  ReadWriteTraceAnalysis::OutOfBoundsAccessMap* ReadWriteTraceAnalysis::getResultMapPtr() {
    return &outOfBoundsAccessMap;
  }

  void ReadWriteTraceAnalysis::readingFromMemoryLocation(Label lab, PStatePtr pstate, AbstractValue& memLoc) {
    ROSE_ASSERT(AbstractValue::_variableIdMapping);
    cout<<"RW TRACE: @"<<lab.toString()<<": READ: @"<< memLoc.toString(AbstractValue::_variableIdMapping)<<endl;
  }

  void ReadWriteTraceAnalysis::writingToMemoryLocation(Label lab, PState* pstate, AbstractValue& memLoc, AbstractValue& newValue) {
    ROSE_ASSERT(AbstractValue::_variableIdMapping);
    cout<<"RW TRACE: @"<<lab.toString()<<": WRITE: @"<< memLoc.toString(AbstractValue::_variableIdMapping)<<" = "<<newValue<<endl;
  }

}
