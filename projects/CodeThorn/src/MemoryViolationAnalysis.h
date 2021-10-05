#ifndef MEMORY_VIOLATION_ANALYSIS_H 
#define MEMORY_VIOLATION_ANALYSIS_H 

//#include "ReadWriteListener.h"
#include "EStateTransferFunctions.h"
#include <map>
#include "BoolLattice.h"

namespace CodeThorn {
class MemoryViolationAnalysis : public ReadWriteListener {
 public:
  // result is value after reading from memLoc in pstate at label lab
  MemoryViolationAnalysis();
  void readingFromMemoryLocation(Label lab, PStatePtr pstate, AbstractValue& memLoc, AbstractValue& result) override;
  void writingToMemoryLocation(Label lab, PStatePtr pstate, AbstractValue& memLoc, AbstractValue& newValue) override;
  typedef std::map <Label,BoolLattice> OutOfBoundsAccessMap;
  OutOfBoundsAccessMap* getResultMapPtr();
  virtual ~MemoryViolationAnalysis();
 private:
  OutOfBoundsAccessMap outOfBoundsAccessMap;
};
}
#endif
