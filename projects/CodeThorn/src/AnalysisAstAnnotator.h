#ifndef ANALYSIS_AST_ANNOTATOR_H
#define ANALYSIS_AST_ANNOTATOR_H

#include "AstAnnotator.h"
#include "DFAnalysisBase.h"

class AnalysisAstAnnotator : public SPRAY::AstAnnotator {
public:
  AnalysisAstAnnotator(SPRAY::Labeler* labeler);
  AnalysisAstAnnotator(SPRAY::Labeler* labeler, SPRAY::VariableIdMapping* variableIdMapping);
  void annotateAnalysisPrePostInfoAsComments(SgNode* root,  string analysisInfoTypeDescription, SPRAY::DFAnalysisBase* analysis);
};

#endif
