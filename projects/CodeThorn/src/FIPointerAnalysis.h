#ifndef FI_POINTER_ANALYSIS_H
#define FI_POINTER_ANALYSIS_H

#include "sage3basic.h"
#include "VariableIdMapping.h"
#include "addressTakenAnalysis.h"
#include "FunctionIdMapping.h"
#include "PointerAnalysisInterface.h"

namespace CodeThorn {

  class FIPointerAnalysis : public PointerAnalysisInterface {
 public:
  FIPointerAnalysis(VariableIdMapping*, FunctionIdMapping*, SgProject*);
  ~FIPointerAnalysis();
  void initialize();
  void run();
  // schroder3: TODO: replace calls of getModByPointer by calls
  //  of getAddressTakenVariables
  VariableIdSet getModByPointer();
  VariableIdSet getAddressTakenVariables();
  FunctionIdSet getAddressTakenFunctions();
  CodeThorn::FlowInsensitivePointerInfo* getFIPointerInfo();
 private:
  VariableIdMapping* _variableIdMapping;
  FunctionIdMapping* _functionIdMapping;
  SgProject* _astRoot;
  CodeThorn::FlowInsensitivePointerInfo* _fipi;
  VariableIdSet _modVariableIds;
  FunctionIdSet _modFunctionIds;
};

} // end of namespace CodeThorn
#endif
