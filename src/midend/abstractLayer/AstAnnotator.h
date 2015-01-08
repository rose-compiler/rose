// Author: Markus Schordan, 2013.

#ifndef ASTANNOTATOR_H
#define ASTANNOTATOR_H


#include <string>

#include "DFAstAttribute.h"
#include "Labeler.h"

/*! 
  * \author Markus Schordan
  * \date 2013.
 */
class AstAnnotator {
 public:
  AstAnnotator(SPRAY::Labeler* labeler);
  AstAnnotator(SPRAY::Labeler* labeler, VariableIdMapping* variableIdMapping);
  // annotates attributes of Type DFAstAttribute of name 'attributeName' as comment for all nodes in the AST subtree of  node 'node'.
  void annotateAstAttributesAsCommentsBeforeStatements(SgNode* node, std::string attributeName);
  void annotateAstAttributesAsCommentsAfterStatements(SgNode* node, std::string attributeName);
  //MS: planned: void annotateAttributeAsPragma(std::string attributeName);
 private:
  void annotateAstAttributesAsComments(SgNode* node, std::string attributeName, PreprocessingInfo::RelativePositionType posSpecifier,std::string analysisInfoTypeDescription);
  void insertComment(std::string comment, PreprocessingInfo::RelativePositionType posSpecifier, SgStatement* node);
  SPRAY::Labeler* _labeler;
  VariableIdMapping* _variableIdMapping;
};

#endif
