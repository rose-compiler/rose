#include "sage3basic.h"
#include "AnalysisAstAnnotator.h"

#include <iostream>

using namespace std;
using namespace CodeThorn;

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
      // exclude branch/loop statement root nodes because they are not
      // included in the ICFG (instead conditions and blocks are
      // included)
      if(SgNodeHelper::isCondStmt(stmt)) {
        continue;
      }
      ROSE_ASSERT(_labeler);
      //cout << "@"<<stmt<<" "<<stmt->class_name()<<" FOUND LABEL: "<<_labeler->getLabel(stmt)<<endl;
      Label label=_labeler->getLabel(stmt);
      if(label!=Labeler::NO_LABEL && !_labeler->isBlockBeginLabel(label) && !_labeler->isBlockEndLabel(label)) {
        string labelString=_labeler->labelToString(label);
        string preLabelString=labelString;
        string postLabelString=labelString; // default (only different for nodes with 2 or more associated labels)
        string commentStart="// ";
        string preInfoString;
        string postInfoString;
        if(!_variableIdMapping) {
          preInfoString=analysis->getPreInfo(label)->toString();
          postInfoString=analysis->getPostInfo(label)->toString();
        } else {
          if(analysis->getLabeler()->isFunctionCallLabel(label)) {
            // special case of function call annotation. Since the
            // function call is represented by two node in the ICFG
            // that are both associated with the AST function call
            // node, the annotation is generated for the call node
            // before the call (pre+post using the preInfoString), and
            // for the callreturn node after the call (pre+post using
            // the postInfoString).
            stringstream ss1;
            preLabelString=_labeler->labelToString(label);
            analysis->getPreInfo(label)->toStream(ss1,_variableIdMapping);
            ss1<<", ";
            analysis->getPostInfo(label)->toStream(ss1,_variableIdMapping);
            preInfoString=ss1.str();
            Label fcReturnLabel=analysis->getLabeler()->functionCallReturnLabel(stmt);
            labelString+=(", "+_labeler->labelToString(fcReturnLabel));
            stringstream ss2;
            postLabelString=_labeler->labelToString(fcReturnLabel);
            analysis->getPreInfo(fcReturnLabel)->toStream(ss2,_variableIdMapping);
            ss2<<", ";
            analysis->getPostInfo(fcReturnLabel)->toStream(ss2,_variableIdMapping);
            postInfoString=ss2.str();
          } else {
            stringstream ss1;
            analysis->getPreInfo(label)->toStream(ss1,_variableIdMapping);
            preInfoString=ss1.str();
            stringstream ss2;
            analysis->getPostInfo(label)->toStream(ss2,_variableIdMapping);
            postInfoString=ss2.str();
          }
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
        string commentString;
        bool isFunCall=analysis->getLabeler()->isFunctionCallLabel(label);
        insertComment(commentStart+preLabelString+" "+analysisInfoTypeDescription+"-"+(isFunCall?"in/out":"in ")+": "+preInfoString,ppInfo1,stmt);

        // TODO: workaround for ROSE's wrong comment unparsing of "case x:" (unparses after the block, not right after the case)
        if(!isSgCaseOptionStmt(stmt)&&!isSgDefaultOptionStmt(stmt)) {
          insertComment(commentStart+postLabelString+" "+analysisInfoTypeDescription+"-"+(isFunCall?"in/out":"out ")+": "+postInfoString,ppInfo2,stmt);
        }

      }
    }
  }
}
