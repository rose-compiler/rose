#ifndef ANALYSIS_AST_ANNOTATOR_H
#define ANALYSIS_AST_ANNOTATOR_H

#include "AstAnnotator.h"
#include "DFAnalysisBase.h"

class AnalysisAstAnnotator : public CodeThorn::AstAnnotator {
public:
  AnalysisAstAnnotator(CodeThorn::Labeler* labeler);
  AnalysisAstAnnotator(CodeThorn::Labeler* labeler, CodeThorn::VariableIdMapping* variableIdMapping);
  void annotateAnalysisPrePostInfoAsComments(SgNode* root,  string analysisInfoTypeDescription, CodeThorn::DFAnalysisBase* analysis);
};

#endif
