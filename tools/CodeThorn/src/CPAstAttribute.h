#ifndef CPASTATTRIBUTE_H
#define CPASTATTRIBUTE_H

#include "Labeler.h"
#include "VariableIdMapping.h"
#include "CPAstAttributeInterface.h"
#include "AbstractValue.h"
#include "FIConstAnalysis.h"

class SgNode;

namespace CodeThorn {
  class CPAstAttribute : public CodeThorn::CPAstAttributeInterface {
  public:
    virtual bool isConstantInteger(CodeThorn::VariableId varId);
    virtual CPAstAttributeInterface::ConstantInteger getConstantInteger(CodeThorn::VariableId varId);
    //virtual ~CPAstAttribute();
    CPAstAttribute(CodeThorn::VariableConstInfo* elem, SgNode* node, CodeThorn::VariableIdMapping* variableIdMapping);
    void toStream(std::ostream& os, CodeThorn::VariableIdMapping* vim);
    std::string toString();
  private:
    CodeThorn::VariableConstInfo* _elem;
    SgNode* _node;
    CodeThorn::VariableIdMapping* _variableIdMapping;
  };
}

#endif
