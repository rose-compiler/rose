// Author: Markus Schordan, 2013.

#include "sage3basic.h"

#include <iostream>

#include "AstAnnotator.h"
#include "RoseAst.h"
#include <iostream>

using namespace std;
using namespace CodeThorn;

AstAnnotator::AstAnnotator(Labeler* labeler):_labeler(labeler),_variableIdMapping(0) {
}
AstAnnotator::AstAnnotator(Labeler* labeler, VariableIdMapping* variableIdMapping):_labeler(labeler),_variableIdMapping(variableIdMapping) {
  ROSE_ASSERT(_variableIdMapping);
}

void AstAnnotator::annotateAstAttributesAsCommentsBeforeStatements(SgNode* node, string attributeName) {
  string analysisInfoTypeDescription="in ";
  annotateAstAttributesAsComments(node,attributeName,PreprocessingInfo::before,analysisInfoTypeDescription);
}
void AstAnnotator::annotateAstAttributesAsCommentsAfterStatements(SgNode* node, string attributeName) {
  string analysisInfoTypeDescription="out";
  annotateAstAttributesAsComments(node,attributeName,PreprocessingInfo::after,analysisInfoTypeDescription);
}

void AstAnnotator::annotateAstAttributesAsComments(SgNode* node, string attributeName,PreprocessingInfo::RelativePositionType posSpecifier, string analysisInfoTypeDescription) {
  RoseAst ast(node);
  for(RoseAst::iterator i=ast.begin(); i!=ast.end();++i) {
    if(SgStatement* stmt=dynamic_cast<SgStatement*>(*i)) {
      if(isSgCtorInitializerList(*i)) {
        //std::cerr << "WARNING: attaching comments to AST nodes of type SgCtorInitializerList not possible. We are skipping this annotation and continue."<<std::endl;
        continue;
      }
      DFAstAttribute* artAttribute=dynamic_cast<DFAstAttribute*>(stmt->getAttribute(attributeName));
      // if this fails, no attribute was attached
      if(artAttribute) {
        ROSE_ASSERT(_labeler);
        //cout << "@"<<stmt<<" "<<stmt->class_name()<<" FOUND LABEL: "<<_labeler->getLabel(stmt)<<endl;
        string labelString=_labeler->labelToString(_labeler->getLabel(stmt));
        string commentStart="// ";
        string artAttributeString;
        if(!_variableIdMapping) {
          artAttributeString=artAttribute->toString();
        } else {
          stringstream ss;
          artAttribute->toStream(ss,_variableIdMapping);
          artAttributeString=ss.str();
        }
        insertComment(commentStart+labelString+" "+analysisInfoTypeDescription+": "+artAttributeString,posSpecifier,stmt);
      }
    }
  }
}

// posSpecifier: PreprocessingInfo::before, PreprocessingInfo::after
void AstAnnotator::insertComment(std::string comment, PreprocessingInfo::RelativePositionType posSpecifier, SgStatement* node) {
  assert(posSpecifier==PreprocessingInfo::before || posSpecifier==PreprocessingInfo::after);
  PreprocessingInfo* commentInfo = 
    new PreprocessingInfo(PreprocessingInfo::CplusplusStyleComment, 
                          comment,
                          "user-generated",0, 0, 0, 
                          posSpecifier // e.g. PreprocessingInfo::before
                          );
  node->addToAttachedPreprocessingInfo(commentInfo);
}

