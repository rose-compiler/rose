#ifndef CTIO_LABELER_H
#define CTIO_LABELER_H

#include "Labeler.h"
#include "VariableIdMapping.h"

namespace CodeThorn {
  class CTIOLabeler : public SPRAY::IOLabeler {
  public:
    CTIOLabeler(SgNode* start, SPRAY::VariableIdMapping* variableIdMapping);
    virtual bool isStdIOLabel(SPRAY::Label label);
    virtual bool isStdInLabel(SPRAY::Label label, SPRAY::VariableId* id);
    bool isNonDetIntFunctionCall(SPRAY::Label lab,SPRAY::VariableId* varIdPtr);
    bool isNonDetLongFunctionCall(SPRAY::Label lab,SPRAY::VariableId* varIdPtr);
    bool isFunctionCallWithName(SPRAY::Label lab,SPRAY::VariableId* varIdPtr, std::string name);
    ~CTIOLabeler();
    void setExternalNonDetIntFunctionName(std::string);
    void setExternalNonDetLongFunctionName(std::string);
  private:
    std::string _externalNonDetIntFunctionName;
    std::string _externalNonDetLongFunctionName;
  };
} // end of namespace CodeThorn

#endif
