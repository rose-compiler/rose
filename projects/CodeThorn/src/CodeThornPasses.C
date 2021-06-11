
#include "sage3basic.h"
#include "CodeThornPasses.h"
#include <iostream>

namespace CodeThorn {

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
    tc.stopTimer(TimingCollector::variableIdMapping);
    return CodeThorn::createVariableIdMapping(ctOpt,root);
  }

  CodeThorn::Labeler* Pass::createLabeler(CodeThornOptions& ctOpt, SgProject* root, TimingCollector& tc, VariableIdMapping* variableIdMapping) {
    tc.startTimer();
    if(ctOpt.status) cout<<"Phase: program location labeling"<<endl;
    Labeler* labeler=createLabeler(root,variableIdMapping);
    tc.stopTimer(TimingCollector::labeler);
  }
}

} // namespace CodeThorn
