
// inherited attribute for source location context information
#include "sourceLocationInheritedAttribute.h"
#include "rewrite.h"
#include "transformationSupport.h"

// Header file defining classes and member function 
// definitions for testing the AST Rewrite Mechanism

// Use new rewrite mechanism
#define USE_NEW_REWRITE_MECHANISM 1

// Build an inherited attribute for the tree traversal to test the rewrite mechanism
class MyInheritedAttributeType
   {
     public:
       // Support for transformation specification (just for the test code)
          HighLevelRewrite::ScopeIdentifierEnum   scope;
          HighLevelInterfaceNodeCollection::PlacementPositionEnum location;
          bool buildInNewScope;

          string targetCodeString;
          string newCodeString;
          bool removeTargetCode;

//        SgProject* projectPointer;

//   private:
       // Hide the default constructor
          MyInheritedAttributeType ();

     public:
//        MyInheritedAttributeType ( SgNode* astNode );
//        MyInheritedAttributeType ( const MyInheritedAttributeType & X, SgNode* astNode );
   };

// Build a synthesized attribute for the tree traversal to test the rewrite mechanism
class MySynthesizedAttributeType
   : public HighLevelRewrite::SynthesizedAttribute
   {
     public:
//        MySynthesizedAttributeType( SgNode* node );

  // private:
          MySynthesizedAttributeType();
   };

// tree traversal to test the rewrite mechanism
class MyTraversal
#if USE_NEW_REWRITE_MECHANISM
   : public HighLevelRewrite::RewriteTreeTraversal<MyInheritedAttributeType,MySynthesizedAttributeType>
#else
   : public SgTopDownBottomUpProcessing<MyInheritedAttributeType,MySynthesizedAttributeType>
#endif
   {
     public:
//        SgProject & project;

         ~MyTraversal ();
          MyTraversal ();

//        MyTraversal ( SgNode* astNode );
//        MyTraversal ( SgProject & project );

#if USE_NEW_REWRITE_MECHANISM
       // Functions required by the global tree traversal mechanism
          MyInheritedAttributeType evaluateRewriteInheritedAttribute (
             SgNode* astNode,
             MyInheritedAttributeType inheritedAttribute );

          MySynthesizedAttributeType evaluateRewriteSynthesizedAttribute (
             SgNode* astNode,
             MyInheritedAttributeType inheritedAttribute,
             SubTreeSynthesizedAttributes synthesizedAttributeList );
#else
       // Functions required by the global tree traversal mechanism
          MyInheritedAttributeType
          evaluateInheritedAttribute (
             SgNode* astNode,
             MyInheritedAttributeType inheritedAttribute );

          MySynthesizedAttributeType evaluateSynthesizedAttribute (
             SgNode* astNode,
             MyInheritedAttributeType inheritedAttribute,
             SubTreeSynthesizedAttributes synthesizedAttributeList );
#endif
   };

// Test function which calls the tree traversal function
void testPermutation( SgProject* project, MyInheritedAttributeType & inheritedAttribute );

