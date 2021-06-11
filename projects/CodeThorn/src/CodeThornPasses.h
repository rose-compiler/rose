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
  void normalization(CodeThornOptions& ctOpt, SgProject* root, TimingCollector& tc);
  CodeThorn::VariableIdMappingExtended* createVariableIdMapping(CodeThornOptions& ctOpt, SgProject* root, TimingCollector& tc);
  CodeThorn::Labeler* createLabeler(CodeThornOptions& ctOpt, SgProject* root, TimingCollector& tc, VariableIdMapping* variableIdMapping);
  } // namespace Pass
} // namespace CodeThorn

#endif
