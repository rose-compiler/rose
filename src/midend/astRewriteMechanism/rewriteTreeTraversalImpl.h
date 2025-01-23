#ifndef AST_REWRITE_TREE_TRAVERSAL_C
#define AST_REWRITE_TREE_TRAVERSAL_C

#include "roseInternal.h"
#include "rewrite.h"

template<class RewriteInheritedAttribute, class RewriteSynthesizedAttribute>
RewriteInheritedAttribute
HighLevelRewrite::RewriteTreeTraversal<RewriteInheritedAttribute,RewriteSynthesizedAttribute>::
evaluateInheritedAttribute (
     SgNode* /*astNode*/,
     RewriteInheritedAttribute inheritedValue )
   {
     return inheritedValue;
   }

template<class RewriteInheritedAttribute, class RewriteSynthesizedAttribute>
RewriteSynthesizedAttribute
HighLevelRewrite::RewriteTreeTraversal<RewriteInheritedAttribute,RewriteSynthesizedAttribute>::
evaluateSynthesizedAttribute (
     SgNode* astNode,
     RewriteInheritedAttribute inheritedValue,
     typename HighLevelRewrite::RewriteTreeTraversal<RewriteInheritedAttribute,RewriteSynthesizedAttribute>::SubTreeSynthesizedAttributes synthesizedAttributeList )
   {
     ASSERT_not_null(astNode);
     RewriteSynthesizedAttribute returnSynthesizedAttribute;

     returnSynthesizedAttribute.mergeChildSynthesizedAttributes(synthesizedAttributeList);
     returnSynthesizedAttribute.consistancyCheck("After mergeChildSynthesizedAttributes");

  // Call the user's version of the evaluateRewriteSynthesizedAttribute function here
     returnSynthesizedAttribute += evaluateRewriteSynthesizedAttribute (astNode,inheritedValue,synthesizedAttributeList);
     returnSynthesizedAttribute.consistancyCheck("After += operator");

  // Now call the rewrite mechanism to generate AST fragements from the relavant 
  // strings (at this node) and patch them into the original application's AST.
  // Pass the accumulated attribute containing all declarations.
     returnSynthesizedAttribute.rewriteAST(astNode);
     returnSynthesizedAttribute.consistancyCheck("After returnSynthesizedAttribute.rewriteAST(project,astNode)");

     return returnSynthesizedAttribute;
   }

// endif for AST_REWRITE_TREE_TRAVERSAL_C
#endif












