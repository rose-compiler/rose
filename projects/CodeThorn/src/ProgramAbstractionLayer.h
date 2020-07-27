#ifndef PROGRAM_ABSTRACTION_LAYER_H
#define PROGRAM_ABSTRACTION_LAYER_H

#include "Labeler.h"
#include "VariableIdMapping.h"
#include "FunctionIdMapping.h"
#include "FunctionCallMapping.h"
#include "CFAnalysis.h"
#include "Flow.h"
#include "VariableIdMappingExtended.h"

class SgProject;
class ClassHierarchyWrapper;

namespace CodeThorn {

  class ProgramAbstractionLayer {
  public:
    ProgramAbstractionLayer();
    virtual ~ProgramAbstractionLayer();
    virtual void initialize(SgProject* root);
    SgProject* getRoot();
    Labeler* getLabeler();
    VariableIdMappingExtended* getVariableIdMapping();
    FunctionIdMapping* getFunctionIdMapping();
    FunctionCallMapping* getFunctionCallMapping();
    FunctionCallMapping2* getFunctionCallMapping2();
    ClassHierarchyWrapper* getClassHierarchy();
    //    void setModeArrayElementVariableId(bool val);
    //bool getModeArrayElementVariableId();
    void setNormalizationLevel(unsigned int level);
    unsigned int getNormalizationLevel();
    void setInliningOption(bool flag);
    bool getInliningOption();
    Flow* getFlow(bool backwardflow = false);
    CFAnalysis* getCFAnalyzer();
  private:
    bool _modeArrayElementVariableId=false;
    Labeler* _labeler=nullptr;
    VariableIdMappingExtended* _variableIdMapping=nullptr;
    FunctionIdMapping* _functionIdMapping=nullptr;
    FunctionCallMapping* _functionCallMapping=nullptr;
    FunctionCallMapping2* _functionCallMapping2=nullptr;
    ClassHierarchyWrapper* _classHierarchy=nullptr;

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
