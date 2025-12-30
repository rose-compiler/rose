
#include "sage3basic.h"
#include "CodeThornPasses.h"
#include <iostream>

using namespace std;
using namespace CodeThorn;
using namespace CodeThorn::CodeThornLib;

namespace CodeThorn {

  // set to true for matching C++ ctor calls
  //~ bool Pass::WITH_EXTENDED_NORMALIZED_CALL = false;

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

#if 0
  ClassHierarchyWrapper*
  Pass::createClassHierarchy(CodeThornOptions& ctOpt, SgProject* root, TimingCollector& tc) {
    tc.startTimer();
    if(ctOpt.status) cout<<"Phase: class hierarchy analysis"<<endl;
    ClassHierarchyWrapper* classHierarchy=new ClassHierarchyWrapper(root);
    tc.stopTimer(TimingCollector::classHierarchyAnalysis);
    return classHierarchy;
  }
#endif


  ClassAnalysis*
  Pass::createClassAnalysis(CodeThornOptions& ctOpt, SgProject* root, TimingCollector& tc) {
    if (!ctOpt.extendedNormalizedCppFunctionCalls)
      return nullptr;

    ASSERT_not_null(root);
    tc.startTimer();
    if(ctOpt.status) cout<<"Phase: class hierarchy analysis"<<endl;

    ClassAnalysis* res=new ClassAnalysis(std::move(analyzeClasses(root)));

    tc.stopTimer(TimingCollector::classHierarchyAnalysis);
    return res;
  }

  VirtualFunctionAnalysis*
  Pass::createVirtualFunctionAnalysis(CodeThornOptions& ctOpt, ClassAnalysis* classes, TimingCollector& tc) {
    if (!ctOpt.extendedNormalizedCppFunctionCalls)
      return nullptr;

    ASSERT_not_null(classes);

    tc.startTimer();
    if(ctOpt.status) cout<<"Phase: virtual function analysis"<<endl;

    VirtualFunctionAnalysis* res=new VirtualFunctionAnalysis(std::move(analyzeVirtualFunctions(*classes)));

    tc.stopTimer(TimingCollector::virtualFunctionAnalysis);
    return res;
  }


  CFAnalysis* Pass::createForwardIcfg(CodeThornOptions& ctOpt, SgProject* root, TimingCollector& tc, Labeler* labeler, ClassAnalysis* classAnalysis, VirtualFunctionAnalysis* virtualFunctions) {
    // bool isForardIcfg=true;
    return Pass::createIcfg(ctOpt,root,tc,labeler,classAnalysis,virtualFunctions,ICFG_forward);
  }

  CFAnalysis* Pass::createBackwardIcfg(CodeThornOptions& ctOpt, SgProject* root, TimingCollector& tc, Labeler* labeler, ClassAnalysis* classAnalysis, VirtualFunctionAnalysis* virtualFunctions) {
    return Pass::createIcfg(ctOpt,root,tc,labeler,classAnalysis,virtualFunctions,ICFG_backward);
  }

  CFAnalysis* Pass::createIcfg(CodeThornOptions& ctOpt, SgProject* root, TimingCollector& tc, Labeler* labeler, ClassAnalysis* classAnalysis, VirtualFunctionAnalysis* virtualFunctions, ICFGDirection /*icfgDirection*/) {
    tc.startTimer();

    CFAnalysis* cfanalyzer=new CFAnalysis(labeler);
    cfanalyzer->setInterProcedural(ctOpt.getInterProceduralFlag());
    if(!ctOpt.extendedNormalizedCppFunctionCalls) {
      if(ctOpt.status) cout<<"Phase: C ICFG construction"<<endl;
      cfanalyzer->createCICFG(root);
    } else {
      cfanalyzer->useCplusplus(true);
      if(ctOpt.status) cout<<"Phase: C++ ICFG construction"<<endl;
      FunctionCallMapping2* funMap2 = new FunctionCallMapping2(labeler, classAnalysis, virtualFunctions);
      ASSERT_not_null(funMap2);

      funMap2->computeFunctionCallMapping(root);
      cfanalyzer->createCppICFG(root, funMap2);
    }
    cfanalyzer->getIcfgFlow()->setDotOptionDisplayPassThroughLabel(ctOpt.visualization.displayPassThroughLabel);
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
