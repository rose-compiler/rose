#ifndef ROSE_PROGRAM_TRANSFORMATION_H
#define ROSE_PROGRAM_TRANSFORMATION_H

// This header file defines the outermost use of the tree traversal
// mechanism to traverse the AST and calls nested traversals specific
// to array statement transformation (only array expression statements
// are targeted for transformation).  The transformation can be any of
// several different types and are handled through nested tree
// traversals.

// Note: since each nested traversal will use the AST rewrite
// mechanism which requires the use of the
// SourceLocationInheritedAttribute and this inherited attribute must
// be used within the parent of any nested traversal we must use this
// inherited attribute at this top level tree traversal.

// #include "rewrite.h"

// Classes required for template parameters to the global tree traversal mechanisms
// typedef AST_Rewrite::InheritedAttribute   InheritedAttributeBaseClassType;
// typedef ArrayStatementQueryInheritedAttributeType InheritedAttributeBaseClassType;
// typedef AST_Rewrite::SynthesizedAttribute         SynthesizedAttributeBaseClassType;

// The inherited attribute does not need a new class derived from the
// InheritedAttributeBaseClassType (so we use reuse the existing class).
// typedef InheritedAttributeBaseClassType           ProgramTransformationInheritedAttributeType;
typedef ArrayStatementQueryInheritedAttributeType ProgramTransformationInheritedAttributeType;


#if 1
// typedef SynthesizedAttributeBaseClassType ProgramTransformationSynthesizedAttributeType;
typedef SynthesizedAttributeBaseClassType ProgramTransformationSynthesizedAttributeType;
#else
class ProgramTransformationSynthesizedAttributeType
   : public SynthesizedAttributeBaseClassType
   {
     public:
       // Assignment of the base class data
          ProgramTransformationSynthesizedAttributeType & operator= 
             ( const SynthesizedAttributeBaseClassType & X );

       // Copy constructor taking with initialization from the base class
          ProgramTransformationSynthesizedAttributeType
             ( const SynthesizedAttributeBaseClassType & X );

       // Assignment of the derived class data (using the base class operator=)
          ProgramTransformationSynthesizedAttributeType & operator=
             ( const ProgramTransformationSynthesizedAttributeType & X );

       // Function used accumulate variable declarations
          ProgramTransformationSynthesizedAttributeType & operator+=
             ( const ProgramTransformationSynthesizedAttributeType & X );
   };
#endif

class ArrayStatementTraversal
   : public HighLevelRewrite::RewriteTreeTraversal<ProgramTransformationInheritedAttributeType,
                                                   ProgramTransformationSynthesizedAttributeType>
   {
     public:
          ArrayStatementTraversal ();

       // Functions required by the AST Rewrite Tree Traversal mechanism
          ProgramTransformationInheritedAttributeType
               evaluateRewriteInheritedAttribute (
                    SgNode* astNode,
                    ProgramTransformationInheritedAttributeType inheritedValue );

          ProgramTransformationSynthesizedAttributeType
               evaluateRewriteSynthesizedAttribute (
                    SgNode* astNode,
                    ProgramTransformationInheritedAttributeType inheritedValue,
                    SubTreeSynthesizedAttributes attributList );

       // Simple array statement transformations
          ProgramTransformationSynthesizedAttributeType
               arrayAssignmentTransformation (
                    const ProgramTransformationInheritedAttributeType & inheritedAttribute,
                    SgNode* astNode );

       // Transformations on scalar indexing expressions in statements
          ProgramTransformationSynthesizedAttributeType
               arrayScalarIndexingTransformation (
                    const ProgramTransformationInheritedAttributeType & inheritedAttribute,
                    SgNode* astNode );
   };

// endif for ROSE_PROGRAM_TRANSFORMATION_H
#endif












