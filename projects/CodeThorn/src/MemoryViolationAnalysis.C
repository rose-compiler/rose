#include "sage3basic.h"
#include "MemoryViolationAnalysis.h"
#include <iostream>
#include "EStateTransferFunctions.h"
#include "AbstractValue.h"

using namespace std;
using namespace Sawyer::Message;

namespace CodeThorn {

  MemoryViolationAnalysis::MemoryViolationAnalysis() {
  }

  MemoryViolationAnalysis::~MemoryViolationAnalysis() {
  }

  void MemoryViolationAnalysis::setEStateTransferFunctions(EStateTransferFunctions* tf) {
    _estateTransferFunctions=tf;
  }
  
  void MemoryViolationAnalysis::readingFromMemoryLocation(Label lab, PStatePtr pstate, AbstractValue& memLoc) {
    //cout<<"MEM CHECK: @"<<lab.toString()<<": READ: @"<< memLoc.toString()<<endl;
    auto violation=checkMemoryAddress(memLoc);
    recordViolation(violation,lab);
  }

  void MemoryViolationAnalysis::writingToMemoryLocation(Label lab, PState* pstate, AbstractValue& memLoc, AbstractValue& newValue) {
    //cout<<"MEM CHECK: @"<<lab.toString()<<": WRITE: @"<< memLoc.toString()<<" = "<<newValue<<endl;
    auto violation=checkMemoryAddress(memLoc);
    recordViolation(violation,lab);
  }

  std::list<MemoryViolationAnalysis::MemoryAccessViolationType> MemoryViolationAnalysis::checkMemoryAddress(AbstractValue& address) {
    std::list<MemoryViolationAnalysis::MemoryAccessViolationType> resultList;
    // MemoryViolationAnalysis::MemoryAccessViolationType::ACCESS_ERROR;
    // check memLoc w.r.t. AbstractValue::getVariableIdMapping()
    ROSE_ASSERT(AbstractValue::_variableIdMapping);
    if(address.isTop()) {
      resultList.push_back(ACCESS_POTENTIALLY_NP);
      resultList.push_back(ACCESS_POTENTIALLY_OUTSIDE_BOUNDS);
    } if(address.isBot()) {
      resultList.push_back(ACCESS_NON_EXISTING);
    } if(address.isNullPtr()) {
      resultList.push_back(ACCESS_DEFINITELY_NP);
    } else {
      AbstractValue offset=address.getIndexValue();
      if(offset.isTop()) {
	resultList.push_back(ACCESS_POTENTIALLY_OUTSIDE_BOUNDS);
      } else if(offset.isBot()) {
	resultList.push_back(ACCESS_NON_EXISTING);
      } else {
	VariableId memId=address.getVariableId();
	// this must be the only remaining case
	if(offset.isConstInt()) {
	  // check array bounds
	  int memRegionSize=AbstractValue::_variableIdMapping->getTotalSize(memId);
	  if(memRegionSize==0) {
	    resultList.push_back(ACCESS_POTENTIALLY_OUTSIDE_BOUNDS); // will become ACCESS_DEFINITELY_OUTSIDE_BOUNDS;
          } else {
            int accessOffset=offset.getIntValue();
            if(!(accessOffset<0||accessOffset>=memRegionSize)) {
              resultList.push_back(ACCESS_DEFINITELY_INSIDE_BOUNDS);
            } else {
              if(memRegionSize!=-1) {
                resultList.push_back(ACCESS_DEFINITELY_OUTSIDE_BOUNDS);
              } else {
                SAWYER_MESG(CodeThorn::logger[WARN])<<"Memory violation check (unknown region size): "<<address.toString()<<":"
                  <<" offset:"<<accessOffset
                  <<" memregionsize:"<<memRegionSize
                  <<" numelemsize:"<<AbstractValue::_variableIdMapping->getElementSize(memId)
                  <<" numElems:"<<AbstractValue::_variableIdMapping->getNumberOfElements(memId)
                  <<endl;
              }
              
            }
          }
	} else {
	  resultList.push_back(ACCESS_POTENTIALLY_OUTSIDE_BOUNDS);
	}
      }
    }
    return resultList;
  }    

  void MemoryViolationAnalysis::recordViolation(std::list<MemoryAccessViolationType> violationList, Label label) {
    ROSE_ASSERT(_estateTransferFunctions);
    for(auto violation : violationList) {
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
}
