#ifndef CODETHORN_PASSES_H
#define CODETHORN_PASSES_H

#include "CodeThornOptions.h"
#include "TimingCollector.h"
#include "Normalization.h"
#include "CTIOLabeler.h"
#include "CodeThornLib.h"
#include "ClassHierarchyAnalysis.h"
//~ #include "ClassHierarchyGraph.h"

namespace CodeThorn {
  namespace Pass {
    /// defines if extended normalized call matching (functions+ctors) is enabled
    extern bool WITH_EXTENDED_NORMALIZED_CALL;
    enum ICFGDirection { ICFG_forward, ICFG_backward };
    void normalization(CodeThornOptions& ctOpt, SgProject* root, TimingCollector& tc);
    CodeThorn::VariableIdMappingExtended* createVariableIdMapping(CodeThornOptions& ctOpt, SgProject* root, TimingCollector& tc);
    CodeThorn::Labeler* createLabeler(CodeThornOptions& ctOpt, SgProject* root, TimingCollector& tc, VariableIdMappingExtended* variableIdMapping);
    CFAnalysis* createForwardIcfg(CodeThornOptions& ctOpt, SgProject* root, TimingCollector& tc, Labeler* labeler, ClassAnalysis* classHierarchy, VirtualFunctionAnalysis* virtualFunctions);
    CFAnalysis* createBackwardIcfg(CodeThornOptions& ctOpt, SgProject* root, TimingCollector& tc, Labeler* labeler, ClassAnalysis* classHierarchy, VirtualFunctionAnalysis* virtualFunctions);
    CFAnalysis* createIcfg(CodeThornOptions& ctOpt, SgProject* root, TimingCollector& tc, Labeler* labeler, ClassAnalysis* classHierarchy, VirtualFunctionAnalysis* virtualFunctions, ICFGDirection icfgDirection);

    /// creates a class hierarchy representation for classes in \ref root
    /// \return a class hierarchy representation
    ///         nullptr if ctOpt.extendedNormalizedCppFunctionCalls is not set
    ClassAnalysis* createClassAnalysis(CodeThornOptions& ctOpt, SgProject* root, TimingCollector& tc);

    /// Analysis virtual functions and their overrides for classes in \ref classes
    /// \return virtual function analysis results
    ///         nullptr if ctOpt.extendedNormalizedCppFunctionCalls is not set
    VirtualFunctionAnalysis* createVirtualFunctionAnalysis(CodeThornOptions& ctOpt, ClassAnalysis* classes, TimingCollector& tc);
  } // namespace Pass
} // namespace CodeThorn

#endif
