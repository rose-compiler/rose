#ifndef PROGRAM_ABSTRACTION_LAYER_H
#define PROGRAM_ABSTRACTION_LAYER_H

#include "FunctionIdMapping.h"
#include "Labeler.h"
#include "VariableIdMapping.h"

class SgProject;

namespace CodeThorn {

  class ProgramAbstractionLayer {
  public:
    ProgramAbstractionLayer();
    virtual ~ProgramAbstractionLayer();
    virtual void initialize(SgProject* root);
    SgProject* getRoot();
    Labeler* getLabeler();
    VariableIdMapping* getVariableIdMapping();
    FunctionIdMapping* getFunctionIdMapping();
    void setModeArrayElementVariableId(bool val);
    bool getModeArrayElementVariableId();
    void setNormalizationLevel(unsigned int level);
    bool getNormalizationLevel();
    void setInliningOption(bool flag);
    bool getInliningOption();
  private:
    bool _modeArrayElementVariableId=false;
    Labeler* _labeler=nullptr;
    VariableIdMapping* _variableIdMapping=nullptr;
    FunctionIdMapping* _functionIdMapping=nullptr;
    bool _normalizationLevel=1;
    bool _inliningOption=false;
    SgProject* _root=nullptr;
  };

} // end of namespace CodeThorn

#endif
