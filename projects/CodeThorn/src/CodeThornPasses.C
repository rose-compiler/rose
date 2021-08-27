
#include "sage3basic.h"
#include "CodeThornPasses.h"
#include <iostream>
using namespace CodeThorn::CodeThornLib;

namespace CodeThorn {

  // set to true for matching C++ ctor calls
  bool Pass::WITH_EXTENDED_NORMALIZED_CALL = false;

  void Pass::normalization(CodeThornOptions& ctOpt, SgProject* root, TimingCollector& tc) {
    tc.startTimer();
    if(ctOpt.status) cout<<"Phase: normalization";
    if(ctOpt.status && ctOpt.normalizeLevel==0) cout<<" (skipped)";
    if(ctOpt.status) cout<<endl;
    normalizationPass(ctOpt,root);
    tc.stopTimer(TimingCollector::normalization);
  }

  CodeThorn::VariableIdMappingExtended* Pass::createVariableIdMapping(CodeThornOptions& ctOpt, SgProject* root, TimingCollector& tc) {
    tc.startTimer();
    if(ctOpt.status) cout<<"Phase: variable-id mapping"<<endl;
    CodeThorn::VariableIdMappingExtended* vim=CodeThorn::CodeThornLib::createVariableIdMapping(ctOpt,root);
    tc.stopTimer(TimingCollector::variableIdMapping);
    return vim;
  }

  CodeThorn::Labeler* Pass::createLabeler(CodeThornOptions& ctOpt, SgProject* root, TimingCollector& tc, VariableIdMappingExtended* variableIdMapping) {
    tc.startTimer();
    if(ctOpt.status) cout<<"Phase: program location labeling"<<endl;

    const bool withCplusplus = ctOpt.extendedNormalizedCppFunctionCalls;
    Labeler* labeler=CodeThorn::CodeThornLib::createLabeler(root,variableIdMapping,withCplusplus);
    tc.stopTimer(TimingCollector::labeler);
    return labeler;
  }

  // not used
  ClassHierarchyWrapper* Pass::createClassHierarchy(CodeThornOptions& ctOpt, SgProject* root, TimingCollector& tc) {
    tc.startTimer();
    if(ctOpt.status) cout<<"Phase: class hierarchy analysis"<<endl;
    auto classHierarchy=new ClassHierarchyWrapper(root);
    tc.stopTimer(TimingCollector::classHierarchyAnalysis);
    return classHierarchy;
  }

  CFAnalysis* Pass::createForwardIcfg(CodeThornOptions& ctOpt, SgProject* root, TimingCollector& tc, Labeler* labeler, ClassHierarchyWrapper* classHierarchy) {
    // bool isForardIcfg=true;
    return Pass::createIcfg(ctOpt,root,tc,labeler,classHierarchy,ICFG_forward);
  }

  CFAnalysis* Pass::createBackwardIcfg(CodeThornOptions& ctOpt, SgProject* root, TimingCollector& tc, Labeler* labeler, ClassHierarchyWrapper* classHierarchy) {
    return Pass::createIcfg(ctOpt,root,tc,labeler,classHierarchy,ICFG_backward);
  }

  CFAnalysis* Pass::createIcfg(CodeThornOptions& ctOpt, SgProject* root, TimingCollector& tc, Labeler* labeler, ClassHierarchyWrapper* classHierarchy, ICFGDirection icfgDirection) {
    tc.startTimer();

    CodeThorn::Pass::WITH_EXTENDED_NORMALIZED_CALL=ctOpt.extendedNormalizedCppFunctionCalls; // to be used without global var

    CFAnalysis* cfanalyzer=new CFAnalysis(labeler);
    cfanalyzer->setInterProcedural(ctOpt.getInterProceduralFlag());
    if(!ctOpt.extendedNormalizedCppFunctionCalls) {
      if(ctOpt.status) cout<<"Phase: C ICFG construction"<<endl;
      cfanalyzer->createCICFG(root);
    } else {
      if(ctOpt.status) cout<<"Phase: C++ ICFG construction"<<endl;
      cfanalyzer->createCppICFG(root);
    }
    if(ctOpt.status) {
      cout<<"Phase: ICFG construction"<<endl;
      size_t icfgSize=cfanalyzer->getIcfgFlow()->size();
      size_t interSize=cfanalyzer->getInterFlow()->size();
      size_t intraSize=icfgSize-interSize;
      cout<<"    intra-procedural edges: " << intraSize<<endl;
      cout<<"    inter-procedural edges: " << interSize <<endl;
      cout<<"    ICFG total       edges: " << icfgSize <<endl;
    }
    tc.stopTimer(TimingCollector::icfgConstruction);
    return cfanalyzer;
  }

} // namespace CodeThorn
