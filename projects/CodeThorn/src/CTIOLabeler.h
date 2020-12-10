#ifndef CTIO_LABELER_H
#define CTIO_LABELER_H

#include "Labeler.h"
#include "VariableIdMapping.h"

namespace CodeThorn {
  class CTIOLabeler : public CodeThorn::IOLabeler {
  public:
    CTIOLabeler(SgNode* start, CodeThorn::VariableIdMapping* variableIdMapping);
    virtual bool isStdIOLabel(CodeThorn::Label label);
    virtual bool isStdInLabel(CodeThorn::Label label, CodeThorn::VariableId* id);
    bool isNonDetIntFunctionCall(CodeThorn::Label lab,CodeThorn::VariableId* varIdPtr);
    bool isNonDetLongFunctionCall(CodeThorn::Label lab,CodeThorn::VariableId* varIdPtr);
    bool isFunctionCallWithName(CodeThorn::Label lab,CodeThorn::VariableId* varIdPtr, std::string name);
    ~CTIOLabeler();
    void setExternalNonDetIntFunctionName(std::string);
    void setExternalNonDetLongFunctionName(std::string);
  private:
    std::string _externalNonDetIntFunctionName;
    std::string _externalNonDetLongFunctionName;
  };
} // end of namespace CodeThorn

#endif
