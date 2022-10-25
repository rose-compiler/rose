#ifndef POINTER_ANALYSIS_INTERFACE_H
#define POINTER_ANALYSIS_INTERFACE_H

#include "sage3basic.h"
#include "VariableIdMapping.h"

namespace CodeThorn {

  class PointerAnalysisInterface {
  public:
    PointerAnalysisInterface();
    virtual void initialize()=0;
    virtual void run()=0;
    virtual VariableIdSet getModByPointer()=0;
    virtual ~PointerAnalysisInterface();
    bool hasDereferenceOperation(SgExpression* exp);
    bool hasAddressOfOperation(SgExpression* exp);
  };
  
  class PointerAnalysisEmptyImplementation : public PointerAnalysisInterface {
  public:
    PointerAnalysisEmptyImplementation(VariableIdMapping* vim);
    virtual void initialize();
    virtual void run();
    virtual VariableIdSet getModByPointer();
  private:
    VariableIdMapping* _variableIdMapping;
  };

}

#endif
