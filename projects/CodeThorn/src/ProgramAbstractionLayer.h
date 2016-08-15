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
    virtual void initialize(SgProject* root, bool computeFunctionIdMapping = true);
    Labeler* getLabeler();
    VariableIdMapping* getVariableIdMapping();
    FunctionIdMapping* getFunctionIdMapping();
    void setModeArrayElementVariableId(bool val);
    bool getModeArrayElementVariableId();
  private:
    bool _modeArrayElementVariableId;;
    Labeler* _labeler;
    VariableIdMapping* _variableIdMapping;
    FunctionIdMapping* _functionIdMapping;
  };

} // end of namespace SPRAY

#endif
