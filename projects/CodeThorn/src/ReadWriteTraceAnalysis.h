#ifndef READ_WRITE_TRACE_ANALYSIS_H 
#define READ_WRITE_TRACE_ANALYSIS_H 

//#include "ReadWriteListener.h"
#include "EStateTransferFunctions.h"
#include <map>
#include "BoolLattice.h"

namespace CodeThorn {
class ReadWriteTraceAnalysis : public ReadWriteListener {
 public:
  // result is value after reading from memLoc in pstate at label lab
  ReadWriteTraceAnalysis();
  void readingFromMemoryLocation(Label lab, PStatePtr pstate, AbstractValue& memLoc) override;
  void writingToMemoryLocation(Label lab, PState* pstate, AbstractValue& memLoc, AbstractValue& newValue) override;
  typedef std::map <Label,BoolLattice> OutOfBoundsAccessMap;
  OutOfBoundsAccessMap* getResultMapPtr();
  virtual ~ReadWriteTraceAnalysis();
 private:
  OutOfBoundsAccessMap outOfBoundsAccessMap;
};
}
#endif
