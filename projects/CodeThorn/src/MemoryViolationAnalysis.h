#ifndef MEMORY_VIOLATION_ANALYSIS_H 
#define MEMORY_VIOLATION_ANALYSIS_H 

//#include "ReadWriteListener.h"
#include "EStateTransferFunctions.h"
#include <map>
#include "ProgramLocationsReport.h"
#include "AbstractValue.h"

namespace CodeThorn {
class MemoryViolationAnalysis : public ReadWriteListener {
 public:
  enum MemoryAccessViolationType {ACCESS_ERROR,ACCESS_DEFINITELY_NP, ACCESS_DEFINITELY_INSIDE_BOUNDS, ACCESS_POTENTIALLY_OUTSIDE_BOUNDS, ACCESS_DEFINITELY_OUTSIDE_BOUNDS, ACCESS_NON_EXISTING};

  // result is value after reading from memLoc in pstate at label lab
  MemoryViolationAnalysis();
  void readingFromMemoryLocation(Label lab, PStatePtr pstate, AbstractValue& memLoc, AbstractValue& result) override;
  void writingToMemoryLocation(Label lab, PStatePtr pstate, AbstractValue& memLoc, AbstractValue& newValue) override;
  virtual ~MemoryViolationAnalysis();
 private:
  MemoryViolationAnalysis::MemoryAccessViolationType checkMemoryAddress(AbstractValue& memLoc);
  std::vector<ProgramLocationsReport> _violatingLocations;
};
}
#endif
