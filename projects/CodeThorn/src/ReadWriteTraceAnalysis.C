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
    AbstractValue val=pstate->readFromMemoryLocation(memLoc);
    cout<<"RW TRACE: @"<<lab.toString()<<": READ: @"<< memLoc.toString(AbstractValue::_variableIdMapping)<<" => "<<val.toString(AbstractValue::_variableIdMapping)<<" LOC:"<<getLabeler()->sourceLocationToString(lab,30,30)<<" AST:"<<AstTerm::astTermWithNullValuesToString(getLabeler()->getNode(lab))<<endl;
    string blanks(14,' ');
    cout<<"RW TRACE:"<<blanks<<pstate->toString(AbstractValue::getVariableIdMapping())<<endl;
  }

  void ReadWriteTraceAnalysis::writingToMemoryLocation(Label lab, PState* pstate, AbstractValue& memLoc, AbstractValue& newValue) {
    ROSE_ASSERT(AbstractValue::_variableIdMapping);
    cout<<"RW TRACE: @"<<lab.toString()<<": WRITE: @"<< memLoc.toString(AbstractValue::_variableIdMapping)<<" = "<<newValue<<" LOC:"<<getLabeler()->sourceLocationToString(lab,30,30)<<endl;
  }

}
