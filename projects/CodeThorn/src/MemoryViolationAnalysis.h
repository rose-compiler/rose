#ifndef MEMORY_VIOLATION_ANALYSIS_H 
#define MEMORY_VIOLATION_ANALYSIS_H 

//#include "ReadWriteListener.h"
#include <map>
#include "ProgramLocationsReport.h"
#include "ReadWriteListener.h"
#include <list>

namespace CodeThorn {

  class EStateTransferFunctions;
  class AbstractValue;
  
  class MemoryViolationAnalysis : public ReadWriteListener {
 public:
    enum MemoryAccessViolationType {ACCESS_ERROR,ACCESS_DEFINITELY_NP, ACCESS_POTENTIALLY_NP, ACCESS_DEFINITELY_INSIDE_BOUNDS, ACCESS_POTENTIALLY_OUTSIDE_BOUNDS, ACCESS_DEFINITELY_OUTSIDE_BOUNDS, ACCESS_DEFINITELY_UNINIT, ACCESS_POTENTIALLY_UNINIT,ACCESS_NON_EXISTING};

    // result is value after reading from memLoc in pstate at label lab
    MemoryViolationAnalysis();
    void readingFromMemoryLocation(Label lab, PStatePtr pstate, AbstractValue& memLoc) override;
    void writingToMemoryLocation(Label lab, PState* pstate, AbstractValue& memLoc, AbstractValue& newValue) override;
    virtual ~MemoryViolationAnalysis();
    void setEStateTransferFunctions(EStateTransferFunctions* tf);
  private:
    typedef std::set<MemoryAccessViolationType> MemoryAccessViolationSet;
    MemoryAccessViolationSet checkMemoryAddress(AbstractValue& memLoc);
    void recordViolation(MemoryAccessViolationSet, Label lab);
    std::vector<ProgramLocationsReport> _violatingLocations; // not used yet
    EStateTransferFunctions* _estateTransferFunctions=nullptr; // temporary, until Reports are moved
  };
}
#endif
