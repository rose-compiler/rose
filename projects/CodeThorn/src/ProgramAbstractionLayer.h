#ifndef PROGRAM_ABSTRACTION_LAYER_H
#define PROGRAM_ABSTRACTION_LAYER_H

#include "Labeler.h"
#include "VariableIdMapping.h"

class SgProject;

namespace SPRAY {

  class ProgramAbstractionLayer {
  public:
    ProgramAbstractionLayer();
    virtual void initialize(SgProject* root);
    Labeler* getLabeler();
    VariableIdMapping* getVariableIdMapping();
    void setModeArrayElementVariableId(bool val);
    bool getModeArrayElementVariableId();
  private:
    bool _modeArrayElementVariableId;;
    Labeler* _labeler;
    VariableIdMapping* _variableIdMapping;
  };

} // end of namespace SPRAY

#endif
