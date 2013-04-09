
#include "rose.h"
#include "rewrite.h"

// Extra headers for customizing the rewrite mechanism
#include "rewriteTreeTraversalImpl.h"
#include "rewriteSynthesizedAttributeTemplatesImpl.h"
#include "ASTFragmentCollectorTraversalImpl.h"
#include "prefixGenerationImpl.h"
#include "rewriteASTFragementStringTemplatesImpl.h"
#include "nodeCollectionTemplatesImpl.h"
#include "rewriteDebuggingSupportTemplatesImpl.h"

// Use new rewrite mechanism
#define USE_REWRITE_MECHANISM 1

// Notice that only the names of the evaluate functions change
// along with the derivation of the attributes from an AST_Rewrite nested class
#if USE_REWRITE_MECHANISM
#define EVALUATE_INHERITED_ATTRIBUTE_FUNCTION evaluateRewriteInheritedAttribute
#define EVALUATE_SYNTHESIZED_ATTRIBUTE_FUNCTION evaluateRewriteSynthesizedAttribute
#else
#define EVALUATE_INHERITED_ATTRIBUTE_FUNCTION evaluateInheritedAttribute
#define EVALUATE_SYNTHESIZED_ATTRIBUTE_FUNCTION evaluateSynthesizedAttribute
#endif

// Build an inherited attribute for the tree traversal to test the rewrite mechanism
class MyInheritedAttribute
   {
     public:
       // Note that any constructor is allowed
          MyInheritedAttribute () {};
   };

// Build a synthesized attribute for the tree traversal to test the rewrite mechanism
class MySynthesizedAttribute
#if USE_REWRITE_MECHANISM
   : public HighLevelRewrite::SynthesizedAttribute
#endif
   {
     public:
          MySynthesizedAttribute() {};
   };

// tree traversal to test the rewrite mechanism
#if USE_REWRITE_MECHANISM
 /*! A specific AST processing class is used (built from SgTopDownBottomUpProcessing)
  */
class MyTraversal
   : public HighLevelRewrite::RewriteTreeTraversal<MyInheritedAttribute,MySynthesizedAttribute>
#else
 /*! Any AST processing class may be used but the conversion 
    is trivial if SgTopDownBottomUpProcessing is used.
  */
class MyTraversal
   : public SgTopDownBottomUpProcessing<MyInheritedAttribute,MySynthesizedAttribute>
#endif
   {
     public:
          MyTraversal () {};

       // Functions required by the tree traversal mechanism
          MyInheritedAttribute EVALUATE_INHERITED_ATTRIBUTE_FUNCTION (
             SgNode* astNode,
             MyInheritedAttribute inheritedAttribute );

          MySynthesizedAttribute EVALUATE_SYNTHESIZED_ATTRIBUTE_FUNCTION (
             SgNode* astNode,
             MyInheritedAttribute inheritedAttribute,
             SubTreeSynthesizedAttributes synthesizedAttributeList );
   };

// Functions required by the tree traversal mechanism
MyInheritedAttribute
MyTraversal::EVALUATE_INHERITED_ATTRIBUTE_FUNCTION (
     SgNode* astNode,
     MyInheritedAttribute inheritedAttribute )
   {
  // Note that any constructor will do
     MyInheritedAttribute returnAttribute;

     return returnAttribute;
   }

MySynthesizedAttribute
MyTraversal::EVALUATE_SYNTHESIZED_ATTRIBUTE_FUNCTION (
     SgNode* astNode,
     MyInheritedAttribute inheritedAttribute,
     SubTreeSynthesizedAttributes synthesizedAttributeList )
   {
  // Note that any constructor will do
     MySynthesizedAttribute returnAttribute;

     return returnAttribute;
   }

int
main ( int argc, char** argv )
   {
  // Main Function for default example ROSE Preprocessor
  // This is an example of a preprocessor that can be built with ROSE
  // This example can be used to test the ROSE infrastructure

     SgProject* project = frontend(argc,argv);

     MyTraversal treeTraversal;

     MyInheritedAttribute inheritedAttribute;

  // Ignore the return value since we don't need it
     treeTraversal.traverseInputFiles(project,inheritedAttribute);

     return backend(project);
   }













