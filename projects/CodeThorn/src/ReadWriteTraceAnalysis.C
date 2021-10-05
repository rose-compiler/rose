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

  void ReadWriteTraceAnalysis::readingFromMemoryLocation(Label lab, PStatePtr pstate, AbstractValue& memLoc, AbstractValue& result) {
    cout<<"RW TRACE: @"<<lab.toString()<<": READ: @"<< memLoc.toString()<<endl;
  }

  void ReadWriteTraceAnalysis::writingToMemoryLocation(Label lab, PStatePtr pstate, AbstractValue& memLoc, AbstractValue& newValue) {
    cout<<"RW TRACE: @"<<lab.toString()<<": WRITE: @"<< memLoc.toString()<<" = "<<newValue<<endl;
  }

}
