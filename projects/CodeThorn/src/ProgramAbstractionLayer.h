#ifndef PROGRAM_ABSTRACTION_LAYER_H
#define PROGRAM_ABSTRACTION_LAYER_H

#include "Labeler.h"
#include "CTIOLabeler.h"
#include "VariableIdMapping.h"
#include "FunctionCallMapping.h"
#include "CFAnalysis.h"
#include "Flow.h"
#include "VariableIdMappingExtended.h"
#include "CodeThornOptions.h"

class SgProject;
class ClassHierarchyWrapper;

namespace CodeThorn {

  class ProgramAbstractionLayer {
  public:
    ProgramAbstractionLayer();
    virtual ~ProgramAbstractionLayer();
    virtual void initialize(CodeThornOptions& ctOpt, SgProject* root);
    SgProject* getRoot();
    Labeler* getLabeler();
    VariableIdMappingExtended* getVariableIdMapping();
    FunctionCallMapping* getFunctionCallMapping();
    FunctionCallMapping2* getFunctionCallMapping2();
    void setNormalizationLevel(unsigned int level);
    unsigned int getNormalizationLevel();
    void setInliningOption(bool flag);
    bool getInliningOption();
    Flow* getFlow(bool backwardflow = false);
    InterFlow* getInterFlow();
    CFAnalysis* getCFAnalyzer(); 
    
    // PP (08/25/20) added to support working with unfolded ICFG
    void setForwardFlow(const Flow& fwdflow);
    void setLabeler(Labeler* labeler);
    void clearCFAnalyzer() { _cfanalyzer = nullptr; }
  private:
    Labeler* _labeler=nullptr;
    VariableIdMappingExtended* _variableIdMapping=nullptr;
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
    InterFlow   _interFlow;
  };

} // end of namespace CodeThorn

#endif
