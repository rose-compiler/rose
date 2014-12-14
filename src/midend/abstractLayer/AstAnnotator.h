// Author: Markus Schordan, 2013.

#ifndef ASTANNOTATOR_H
#define ASTANNOTATOR_H


#include <string>
using std::string;

#include "DFAstAttribute.h"
#include "Labeler.h"

/*! 
  * \author Markus Schordan
  * \date 2013.
 */
class AstAnnotator {
 public:
  AstAnnotator(Labeler* labeler);
  AstAnnotator(Labeler* labeler, VariableIdMapping* variableIdMapping);
  // annotates attributes of Type DFAstAttribute of name 'attributeName' as comment for all nodes in the AST subtree of  node 'node'.
  void annotateAstAttributesAsCommentsBeforeStatements(SgNode* node, string attributeName);
  void annotateAstAttributesAsCommentsAfterStatements(SgNode* node, string attributeName);
  //MS: planned: void annotateAttributeAsPragma(string attributeName);
 private:
  void annotateAstAttributesAsComments(SgNode* node, string attributeName, PreprocessingInfo::RelativePositionType posSpecifier,string analysisInfoTypeDescription);
  void insertComment(std::string comment, PreprocessingInfo::RelativePositionType posSpecifier, SgStatement* node);
  Labeler* _labeler;
  VariableIdMapping* _variableIdMapping;
};

#endif
