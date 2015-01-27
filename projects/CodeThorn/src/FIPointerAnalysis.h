#ifndef FI_POINTER_ANALYSIS_H
#define FI_POINTER_ANALYSIS_H

#include "sage3basic.h"
#include "VariableIdMapping.h"
#include "addressTakenAnalysis.h"
#include "PointerAnalysisInterface.h"

using namespace CodeThorn;

namespace SPRAY {

  class FIPointerAnalysis : public PointerAnalysisInterface {
 public:
  FIPointerAnalysis(VariableIdMapping*, SgProject*);
  ~FIPointerAnalysis();
  void initialize();
  void run();
  VariableIdSet getModByPointer();
 private:
  VariableIdMapping* _variableIdMapping;
  SgProject* _astRoot;
  SPRAY::FlowInsensitivePointerInfo* _fipi;
  VariableIdSet _modVariableIds;
};

} // end of namespace SPRAY
#endif
