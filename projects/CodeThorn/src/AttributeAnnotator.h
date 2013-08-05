// Author: Markus Schordan, 2013.

#ifndef ATTRIBUTEANNOTATIONMECHANISM_H
#define ATTRIBUTEANNOTATIONMECHANISM_H

#include <string>
using std::string;

#include "AnalysisAstAttribute.h"

class AnalysisResultAnnotator {
 public:
  // annotates attributes of Type AnalysisResultAttribute of name 'attributeName' as comment for all nodes in the AST subtree of  node 'node'.
  void annotateAnalysisResultAttributesAsComments(SgNode* node, string attributeName);
  //MS: planned: void annotateAttributeAsPragma(string attributeName);
 private:
  void insertComment(std::string comment, PreprocessingInfo::RelativePositionType posSpecifier, SgStatement* node);
};

#endif
