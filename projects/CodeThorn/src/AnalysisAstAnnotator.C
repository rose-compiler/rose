#include "sage3basic.h"
#include "AnalysisAstAnnotator.h"

#include <iostream>

using namespace std;
using namespace SPRAY;

AnalysisAstAnnotator::AnalysisAstAnnotator(Labeler* labeler):AstAnnotator(labeler) {
}

AnalysisAstAnnotator::AnalysisAstAnnotator(Labeler* labeler, VariableIdMapping* variableIdMapping):AstAnnotator(labeler,variableIdMapping) {
}

void AnalysisAstAnnotator::annotateAnalysisPrePostInfoAsComments(SgNode* node, string analysisInfoTypeDescription, DFAnalysisBase* analysis) {
  RoseAst ast(node);
  for(RoseAst::iterator i=ast.begin(); i!=ast.end();++i) {
    if(SgStatement* stmt=dynamic_cast<SgStatement*>(*i)) {
      if(isSgCtorInitializerList(*i)) {
        //std::cerr << "WARNING: attaching comments to AST nodes of type SgCtorInitializerList not possible. We are skipping this annotation and continue."<<std::endl;
        continue;
      }
      ROSE_ASSERT(_labeler);
      //cout << "@"<<stmt<<" "<<stmt->class_name()<<" FOUND LABEL: "<<_labeler->getLabel(stmt)<<endl;
      Label label=_labeler->getLabel(stmt);
      if(label!=Labeler::NO_LABEL) {
        string labelString=_labeler->labelToString(label);
        string commentStart="// ";
        string preInfoString;
        string postInfoString;
        if(!_variableIdMapping) {
          preInfoString=analysis->getPreInfo(label)->toString();
          postInfoString=analysis->getPostInfo(label)->toString();
        } else {
          stringstream ss1;
          analysis->getPreInfo(label)->toStream(ss1,_variableIdMapping);
          preInfoString=ss1.str();
          stringstream ss2;
          analysis->getPostInfo(label)->toStream(ss2,_variableIdMapping);
          postInfoString=ss2.str();
        }
        PreprocessingInfo::RelativePositionType ppInfo1;
        PreprocessingInfo::RelativePositionType ppInfo2;
        if(analysis->isForwardAnalysis()) {
          ppInfo1=PreprocessingInfo::before;
          ppInfo2=PreprocessingInfo::after;
        } else {
          ppInfo1=PreprocessingInfo::after;
          ppInfo2=PreprocessingInfo::before;
        }
        insertComment(commentStart+labelString+" "+analysisInfoTypeDescription+": "+preInfoString,ppInfo1,stmt);
        insertComment(commentStart+labelString+" "+analysisInfoTypeDescription+": "+postInfoString,ppInfo2,stmt);
      }
    }
  }
}
