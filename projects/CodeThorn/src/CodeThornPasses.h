#ifndef CODETHORN_PASSES_H
#define CODETHORN_PASSES_H

#include "CodeThornOptions.h"
#include "TimingCollector.h"
#include "Normalization.h"
#include "CTIOLabeler.h"
#include "CodeThornLib.h"
#include "ClassHierarchyGraph.h"

namespace CodeThorn {
  namespace Pass {
    /// defines if extended normalized call matching (functions+ctors) is enabled
    extern bool WITH_EXTENDED_NORMALIZED_CALL;
    enum ICFGDirection { ICFG_forward, ICFG_backward };
    void normalization(CodeThornOptions& ctOpt, SgProject* root, TimingCollector& tc);
    CodeThorn::VariableIdMappingExtended* createVariableIdMapping(CodeThornOptions& ctOpt, SgProject* root, TimingCollector& tc);
    CodeThorn::Labeler* createLabeler(CodeThornOptions& ctOpt, SgProject* root, TimingCollector& tc, VariableIdMappingExtended* variableIdMapping);
    ClassHierarchyWrapper* createClassHierarchy(CodeThornOptions& ctOpt, SgProject* root, TimingCollector& tc);
    CFAnalysis* createForwardIcfg(CodeThornOptions& ctOpt, SgProject* root, TimingCollector& tc, Labeler* labeler);
    CFAnalysis* createBackwardIcfg(CodeThornOptions& ctOpt, SgProject* root, TimingCollector& tc, Labeler* labeler);
    CFAnalysis* createIcfg(CodeThornOptions& ctOpt, SgProject* root, TimingCollector& tc, Labeler* labeler, ICFGDirection icfgDirection);
  } // namespace Pass
} // namespace CodeThorn

#endif
