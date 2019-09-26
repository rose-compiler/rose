#ifndef PROGRAM_ABSTRACTION_LAYER_H
#define PROGRAM_ABSTRACTION_LAYER_H

#include "Labeler.h"
#include "VariableIdMapping.h"
#include "FunctionIdMapping.h"
#include "CFAnalysis.h"
#include "Flow.h"

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
    unsigned int getNormalizationLevel();
    void setInliningOption(bool flag);
    bool getInliningOption();
    Flow* getFlow(bool backwardflow = false);
    CFAnalysis* getCFAnalyzer();
  private:
    bool _modeArrayElementVariableId=false;
    Labeler* _labeler=nullptr;
    VariableIdMapping* _variableIdMapping=nullptr;
    FunctionIdMapping* _functionIdMapping=nullptr;
    unsigned int _normalizationLevel=2;
    bool _inliningOption=false;
    SgProject* _root=nullptr;

    // PP (07/15/19) moved flow generation from DFAnalysisBase
    CFAnalysis* _cfanalyzer = nullptr;
    Flow        _fwFlow;
    Flow        _bwFlow;
  };

} // end of namespace CodeThorn

#endif
