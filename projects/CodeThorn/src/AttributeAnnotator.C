// Author: Markus Schordan, 2013.

#include "sage3basic.h"

#include <iostream>

#include "AttributeAnnotator.h"
#include "RoseAst.h"

// default behavior
string AnalysisResultAttribute::getPreInfoString() { return "pre-info: none";}
string AnalysisResultAttribute::getPostInfoString() { return "post-info: none";}

AnalysisResultAttribute::~AnalysisResultAttribute() {
  // virtual destructor intentionally empty
}

void AnalysisResultAnnotator::annotateAnalysisResultAttributesAsComments(SgNode* node, string attributeName) {
  RoseAst ast(node);
  for(RoseAst::iterator i=ast.begin(); i!=ast.end();++i) {
    if(SgStatement* stmt=dynamic_cast<SgStatement*>(*i)) {
      if(isSgCtorInitializerList(*i)) {
        std::cerr << "WARNING: attaching comments to AST nodes of type SgCtorInitializerList not possible. We are skipping this annotation and continue."<<std::endl;
        continue;
      }
      AnalysisResultAttribute* artAttribute=dynamic_cast<AnalysisResultAttribute*>(stmt->getAttribute(attributeName));
      // if this fails, no attribute was attached
      if(artAttribute) {
        insertComment(artAttribute->getPreInfoString(),PreprocessingInfo::before,stmt);
        insertComment(artAttribute->getPostInfoString(),PreprocessingInfo::after,stmt);
      }
    }
  }
}

// posSpecifier: PreprocessingInfo::before, PreprocessingInfo::after
void AnalysisResultAnnotator::insertComment(std::string comment, PreprocessingInfo::RelativePositionType posSpecifier, SgStatement* node) {
  assert(posSpecifier==PreprocessingInfo::before || posSpecifier==PreprocessingInfo::after);
  PreprocessingInfo* commentInfo = 
    new PreprocessingInfo(PreprocessingInfo::CplusplusStyleComment, 
                          comment,
                          "user-generated",0, 0, 0, 
                          posSpecifier // e.g. PreprocessingInfo::before
                          );
  node->addToAttachedPreprocessingInfo(commentInfo);
}

