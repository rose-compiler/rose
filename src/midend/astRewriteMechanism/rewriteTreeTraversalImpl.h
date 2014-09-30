#ifndef AST_REWRITE_TREE_TRAVERSAL_C
#define AST_REWRITE_TREE_TRAVERSAL_C

//#include "rose.h"
//#include "sage3.h"
#include "roseInternal.h"
#include "rewrite.h"


template<class RewriteInheritedAttribute, class RewriteSynthesizedAttribute>
RewriteInheritedAttribute
HighLevelRewrite::RewriteTreeTraversal<RewriteInheritedAttribute,RewriteSynthesizedAttribute>::
evaluateInheritedAttribute (
     SgNode* astNode,
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
   // Increment this data member first so that it will be correct if it is access subsequently
//    traversalNodeCounter++;

  // Error checking
     ROSE_ASSERT (astNode != NULL);

//   printf ("^^^^^ TOP of AST_Rewrite::RewriteTreeTraversal::evaluateSynthesizedAttribute: ddd \n");
//   ROSE_ABORT();

#if 0
     printf ("^^^^^ TOP of AST_Rewrite::RewriteTreeTraversal::evaluateSynthesizedAttribute: \n");
     printf ("    (traversalNodeCounter=%d) (astNode = %s) (synthesizedAttributeList.size() = %" PRIuPTR ") \n",
          traversalNodeCounter,astNode->sage_class_name(),synthesizedAttributeList.size());
     printf ("    inheritedValue.declarationContainer->size() = %" PRIuPTR " \n",inheritedValue.declarationContainer->size());
     printf ("astNode->unparseToString() = %s \n",astNode->unparseToString().c_str());
#endif

#if 0
  // Error checking:
  // There must at least be a global scope if this is not a SgFile or SgProject node
     if ( (dynamic_cast<SgFile*>(astNode) != NULL) || (dynamic_cast<SgProject*>(astNode) != NULL) )
          ROSE_ASSERT (inheritedValue.declarationContainer->size() == 0);
       else
          ROSE_ASSERT (inheritedValue.declarationContainer->size() > 0);
#endif

  // RewriteSynthesizedAttribute returnSynthesizedAttribute(astNode);
     RewriteSynthesizedAttribute returnSynthesizedAttribute;

  // printf ("    Test 1: inheritedValue.declarationContainer->size() = %" PRIuPTR " \n",inheritedValue.declarationContainer->size());

  // Accumulate the child attributes into the parent (uses user's operator+= defined for 
  // RewriteSynthesizedAttribute).
     returnSynthesizedAttribute.mergeChildSynthesizedAttributes(synthesizedAttributeList);

  // printf ("    Test 2: inheritedValue.declarationContainer->size() = %" PRIuPTR " \n",inheritedValue.declarationContainer->size());

     returnSynthesizedAttribute.consistancyCheck("After mergeChildSynthesizedAttributes");

  // returnSynthesizedAttribute.display("After mergeChildSynthesizedAttributes(synthesizedAttributeList)");

  // Call the user's version of the evaluateRewriteSynthesizedAttribute function here
     returnSynthesizedAttribute += evaluateRewriteSynthesizedAttribute (astNode,inheritedValue,synthesizedAttributeList);

     returnSynthesizedAttribute.consistancyCheck("After += operator");

  // printf ("    Test 3: inheritedValue.declarationContainer->size() = %" PRIuPTR " \n",inheritedValue.declarationContainer->size());

  // returnSynthesizedAttribute.display("Called from BASE of AST_Rewrite::RewriteTreeTraversal<>::evaluateSynthesizedAttribute()");

  // printf ("In AST_Rewrite::RewriteTreeTraversal::evaluateSynthesizedAttribute(): inheritedValue.declarations.size() = %" PRIuPTR " \n",inheritedValue.declarationContainer->size());
  // ROSE_ASSERT (inheritedValue.declarationContainer->size() > 0);

  // Now call the rewrite mechanism to generate AST fragements from the relavant 
  // strings (at this node) and patch them into the original application's AST.
  // Pass the accumulated attribute containing all declarations.
  // returnSynthesizedAttribute.rewriteAST(project,astNode,*inheritedValue.declarationContainer);
     returnSynthesizedAttribute.rewriteAST(astNode);

  // Make sure this is still set
//   ROSE_ASSERT (returnSynthesizedAttribute.associated_AST_Node == astNode);

  // returnSynthesizedAttribute.display("Called from BASE of AST_Rewrite::RewriteTreeTraversal<>::evaluateSynthesizedAttribute()");

#if 0
  // Pop the stack of scopes if we are leaving a scope statement
     if (dynamic_cast<SgScopeStatement*>(astNode) != NULL)
        {
          ROSE_ASSERT (inheritedValue.declarationContainer->size() > 0);
          inheritedValue.declarationContainer->popScope();
        }
#endif

     returnSynthesizedAttribute.consistancyCheck("After returnSynthesizedAttribute.rewriteAST(project,astNode)");

     return returnSynthesizedAttribute;
   }

// endif for AST_REWRITE_TREE_TRAVERSAL_C
#endif












