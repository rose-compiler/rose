#ifndef FI_POINTER_ANALYSIS_H
#define FI_POINTER_ANALYSIS_H

#include "sage3basic.h"
#include "VariableIdMapping.h"
#include "addressTakenAnalysis.h"
#include "FunctionIdMapping.h"
#include "PointerAnalysisInterface.h"

namespace SPRAY {

  class FIPointerAnalysis : public PointerAnalysisInterface {
 public:
  FIPointerAnalysis(VariableIdMapping*, FunctionIdMapping*, SgProject*);
  ~FIPointerAnalysis();
  void initialize();
  void run();
  VariableIdSet getModByPointer();
  SPRAY::FlowInsensitivePointerInfo* getFIPointerInfo();
 private:
  VariableIdMapping* _variableIdMapping;
  FunctionIdMapping* _functionIdMapping;
  SgProject* _astRoot;
  SPRAY::FlowInsensitivePointerInfo* _fipi;
  VariableIdSet _modVariableIds;
};

} // end of namespace SPRAY
#endif
