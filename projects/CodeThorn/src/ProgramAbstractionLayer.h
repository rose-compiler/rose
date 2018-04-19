#ifndef PROGRAM_ABSTRACTION_LAYER_H
#define PROGRAM_ABSTRACTION_LAYER_H

#include "FunctionIdMapping.h"
#include "Labeler.h"
#include "VariableIdMapping.h"

class SgProject;

namespace SPRAY {

  class ProgramAbstractionLayer {
  public:
    ProgramAbstractionLayer();
    virtual ~ProgramAbstractionLayer();
    virtual void initialize(SgProject* root);
    Labeler* getLabeler();
    VariableIdMapping* getVariableIdMapping();
    FunctionIdMapping* getFunctionIdMapping();
    void setModeArrayElementVariableId(bool val);
    bool getModeArrayElementVariableId();
    void setLoweringOption(bool flag);
    bool getLoweringOption();
    void setInliningOption(bool flag);
    bool getInliningOption();
  private:
    bool _modeArrayElementVariableId;;
    Labeler* _labeler;
    VariableIdMapping* _variableIdMapping;
    FunctionIdMapping* _functionIdMapping;
    bool _loweringOption=false;
    bool _inliningOption=false;
  };

} // end of namespace SPRAY

#endif
