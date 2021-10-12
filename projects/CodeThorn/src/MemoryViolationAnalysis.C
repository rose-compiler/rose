#include "sage3basic.h"
#include "MemoryViolationAnalysis.h"
#include <iostream>
#include "EStateTransferFunctions.h"
#include "AbstractValue.h"

using namespace std;

namespace CodeThorn {

  MemoryViolationAnalysis::MemoryViolationAnalysis() {
  }

  MemoryViolationAnalysis::~MemoryViolationAnalysis() {
  }

  void MemoryViolationAnalysis::setEStateTransferFunctions(EStateTransferFunctions* tf) {
    _estateTransferFunctions=tf;
  }
  
  void MemoryViolationAnalysis::readingFromMemoryLocation(Label lab, PStatePtr pstate, AbstractValue& memLoc, AbstractValue& result) {
    //cout<<"MEM CHECK: @"<<lab.toString()<<": READ: @"<< memLoc.toString()<<endl;
    MemoryAccessViolationType violation=checkMemoryAddress(memLoc);
    recordViolation(violation,lab);
  }

  void MemoryViolationAnalysis::writingToMemoryLocation(Label lab, PStatePtr pstate, AbstractValue& memLoc, AbstractValue& newValue) {
    //cout<<"MEM CHECK: @"<<lab.toString()<<": WRITE: @"<< memLoc.toString()<<" = "<<newValue<<endl;
    MemoryAccessViolationType violation=checkMemoryAddress(memLoc);
    recordViolation(violation,lab);
  }

  MemoryViolationAnalysis::MemoryAccessViolationType MemoryViolationAnalysis::checkMemoryAddress(AbstractValue& address) {

    // MemoryViolationAnalysis::MemoryAccessViolationType::ACCESS_ERROR;
    // check memLoc w.r.t. AbstractValue::getVariableIdMapping()
    ROSE_ASSERT(AbstractValue::_variableIdMapping);
    if(address.isTop()) {
      return ACCESS_POTENTIALLY_OUTSIDE_BOUNDS;
    } if(address.isBot()) {
      return ACCESS_NON_EXISTING;
    } if(address.isNullPtr()) {
      return ACCESS_DEFINITELY_NP;
    } else {
      AbstractValue offset=address.getIndexValue();
      if(offset.isTop()) {
	return ACCESS_POTENTIALLY_OUTSIDE_BOUNDS;
      } else if(offset.isBot()) {
	return ACCESS_NON_EXISTING;
      } else {
	VariableId memId=address.getVariableId();
	// this must be the only remaining case
	if(offset.isConstInt()) {
	  // check array bounds
	  int memRegionSize=AbstractValue::_variableIdMapping->getNumberOfElements(memId);
	  if(memRegionSize==0)
	    return ACCESS_POTENTIALLY_OUTSIDE_BOUNDS; // will become ACCESS_DEFINITELY_OUTSIDE_BOUNDS;
	  int accessIndex=offset.getIntValue();
	  if(!(accessIndex<0||accessIndex>=memRegionSize)) {
	    return ACCESS_DEFINITELY_INSIDE_BOUNDS;
	  } else {
	    return ACCESS_DEFINITELY_OUTSIDE_BOUNDS;
	  }
	} else {
	  return ACCESS_POTENTIALLY_OUTSIDE_BOUNDS;
	}
      }
    }
  }    

  void MemoryViolationAnalysis::recordViolation(MemoryAccessViolationType violation, Label label) {
    ROSE_ASSERT(_estateTransferFunctions);
    switch(violation) {
    case ACCESS_DEFINITELY_NP:
      _estateTransferFunctions->recordDefinitiveViolatingLocation2(ANALYSIS_NULL_POINTER,label);
      break;
    case ACCESS_POTENTIALLY_NP:
      _estateTransferFunctions->recordPotentialViolatingLocation2(ANALYSIS_NULL_POINTER,label);
      break;
    case ACCESS_DEFINITELY_OUTSIDE_BOUNDS:
      _estateTransferFunctions->recordDefinitiveViolatingLocation2(ANALYSIS_OUT_OF_BOUNDS,label);
      break;
    case ACCESS_POTENTIALLY_OUTSIDE_BOUNDS:
      _estateTransferFunctions->recordPotentialViolatingLocation2(ANALYSIS_OUT_OF_BOUNDS,label);
      break;
    default: // ignore others
      ;
    }
  }
}
