
#ifndef NEW_AST_PROCESSING_H
#define NEW_AST_PROCESSING_H

#include "sage3.h"
#include "AstSuccessorsSelectors.h"
#include "StackFrameVector.h"

// autotools failure!
// TODO: make it compile again!
bool inFileToTraverse(SgNode* node, bool traversalConstraint, SgFile* fileToVisit);

namespace AstProcessing {
   // ============================================================================
   // Traversal Classes
   //
   // Three seperate traversal classes to provide the different traversals.
   // Each of them takes two types of template parameters:
   //   1. Evaluator:
   //           Acts as a callback class. Has to implement the evaluate* Functions
   //   2. (Inherited|Synthesized)Attribute:
   //           Types for the Inherited or Synthesized Attribute
   //
   // By having three base classes, the actual traversal can be done with less
   // if statements
   //
   // By supplying the Base class as a template paramater, one can get rid of
   // virtual function calls completely.
   //
   // The general idea behind the redesign is, that the evaluate functions don't
   // have references as formal parameters. With this new design, the only restriction
   // is, that the return and formal paramaeters must match by basic type (i.e.
   // they can be by reference, by const reference, or by value).
   // However, during the tests it was found out that const references are nto always
   // the best choices (especially the StackFrameVector copy ctor is faster than passing
   // by reference.
   // ============================================================================

   // TopDown Traversal
   // Evaluator needs:
   //   - InheritedAttribute evaluateInheritedAttribute( SgNode*, InheritedAttribute )
   template< typename Evaluator,
             typename InheritedAttribute >
   class TopDown
      : public Evaluator
   {
      public:
         TopDown()
            : traversalConstraint( false ),
              fileToVisit( NULL )
         {}
         TopDown( const TopDown& t )
            : traversalConstraint( t.traversalConstraint ),
              fileToVisit( NULL )
         {}
         virtual ~TopDown() {}

         TopDown& operator=( const TopDown& t )
         {
            traversalConstraint = t.traversalConstraint;
            fileToVisit = t.fileToVisit;
            return *this;
         }

         void traverse          (       SgNode              *astNode,
                                  const InheritedAttribute& inheritedValue );
         void traverseWithinFile(       SgNode              *astNode,
                                  const InheritedAttribute& inheritedValue );
         void traverseInputFiles(       SgProject           *project,
                                  const InheritedAttribute& inheritedValue );

      protected:
      private:
         void performTraverse   ( SgNode             *astNode,
                                         InheritedAttribute inheritedValue );

         bool traversalConstraint;
         SgFile *fileToVisit;
   };

   // BottomUp Traversal
   // Evaluator needs:
   //   - SynthesizedAttribute evaluateSynthesizedAttribute( SgNode*, SynthesizedAttribute )
   template< typename Evaluator,
             typename SynthesizedAttribute >
   class BottomUp
      : public Evaluator
   {
      public:
         typedef StackFrameVector<SynthesizedAttribute> SynthesizedAttributesList;

         BottomUp()
            : traversalConstraint( false ),
              fileToVisit( NULL ),
              synthesizedAttributes( new SynthesizedAttributesList() )
         {}
         BottomUp( const BottomUp& b )
            : traversalConstraint( b.traversalConstraint ),
              fileToVisit( NULL ),
              synthesizedAttributes( b.synthesizedAttributes->deepCopy() )
         {}
         virtual ~BottomUp()
         {
            ROSE_ASSERT( synthesizedAttributes );
            delete synthesizedAttributes;
            synthesizedAttributes = NULL;
         }

         BottomUp& operator=( const BottomUp& b )
         {
            traversalConstraint = b.traversalConstraint;
            fileToVisit = b.fileToVisit;
            ROSE_ASSERT( synthesizedAttributes );
            delete synthesizedAttributes;
            synthesizedAttributes = b.synthesizedAttributes->deepCopy();

            return *this;
         }

         SynthesizedAttribute traverse          ( SgNode    *astNode );
         SynthesizedAttribute traverseWithinFile( SgNode    *astNode );
         void                 traverseInputFiles( SgProject *project );

      protected:

      private:
         SynthesizedAttribute performTraverse   ( SgNode *astNode );

         bool traversalConstraint;
         SgFile *fileToVisit;

         SynthesizedAttributesList *synthesizedAttributes;
         //std::list<SynthesizedAttribute> synthesizedAttributes;
   };

   // TopDownBottomUp Traversal
   // Evaluator needs:
   //   - InheritedAttribute evaluateInheritedAttribute( SgNode*, InheritedAttribute )
   //   - SynthesizedAttribute evaluateSynthesizedAttribute( SgNode*, InheritedAttribute, SynthesizedAttribute )
   template< typename Evaluator,
             typename InheritedAttribute,
             typename SynthesizedAttribute >
   class TopDownBottomUp
      : public Evaluator
   {
      public:
         typedef StackFrameVector<SynthesizedAttribute> SynthesizedAttributesList;

         TopDownBottomUp()
            : traversalConstraint( false ),
              fileToVisit( NULL ),
              synthesizedAttributes( new SynthesizedAttributesList() )
         {}
         TopDownBottomUp( const TopDownBottomUp& b )
            : traversalConstraint( b.traversalConstraint ),
              fileToVisit( NULL ),
              synthesizedAttributes( b.synthesizedAttributes->deepCopy() )
         {}
         virtual ~TopDownBottomUp()
         {
            ROSE_ASSERT( synthesizedAttributes );
            delete synthesizedAttributes;
            synthesizedAttributes = NULL;
         }

         TopDownBottomUp& operator=( const TopDownBottomUp& b )
         {
            traversalConstraint = b.traversalConstraint;
            fileToVisit = b.fileToVisit;
            ROSE_ASSERT( synthesizedAttributes );
            delete synthesizedAttributes;
            synthesizedAttributes = b.synthesizedAttributes->deepCopy();

            return *this;
         }

         SynthesizedAttribute traverse          (       SgNode              *astNode,
                                                  const InheritedAttribute& inheritedValue );
         SynthesizedAttribute traverseWithinFile(       SgNode              *astNode,
                                                  const InheritedAttribute& inheritedValue );
         void                 traverseInputFiles(       SgProject           *project,
                                                  const InheritedAttribute& inheritedValue );
      protected:
      private:
         SynthesizedAttribute performTraverse   ( SgNode             *astNode,
                                         InheritedAttribute inheritedValue );

         bool traversalConstraint;
         SgFile *fileToVisit;

         SynthesizedAttributesList *synthesizedAttributes;
   };
   // ============================================================================
} // namespace AstProcessing
// ============================================================================

// ============================================================================
// Old Style Wrapper Classes
//
// To Emulate the old design, one evaluator is needed to provide the pure
// virtual evaluate function, and a Wrapper which acts as the interface to the
// traversals. Typedefs are not an option here, because they can"t be templated.
// ============================================================================

//-----------------------------------------------------------------------------
// AstTopDownWrapper
template< typename InheritedAttribute >
class AstTopDownBaseEvaluator
{
   public:
      virtual ~AstTopDownBaseEvaluator() {}
   protected:
      virtual InheritedAttribute evaluateInheritedAttribute( SgNode*, InheritedAttribute ) = 0;
};

template< typename InheritedAttribute >
class AstTopDownProcessingNew
   : public AstProcessing::TopDown<AstTopDownBaseEvaluator<InheritedAttribute>,
                                   InheritedAttribute>
{
   public:
      virtual ~AstTopDownProcessingNew() {}
};
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// AstBottomUpWrapper
template< typename SynthesizedAttribute >
class AstBottomUpBaseEvaluator
{
   public:
      typedef StackFrameVector<SynthesizedAttribute> SynthesizedAttributesList;

      virtual ~AstBottomUpBaseEvaluator() {}
   protected:
      virtual SynthesizedAttribute evaluateSynthesizedAttribute( SgNode*, SynthesizedAttributesList ) = 0;
};

template< typename SynthesizedAttribute >
class AstBottomUpProcessingNew
   : public AstProcessing::BottomUp<AstBottomUpBaseEvaluator<SynthesizedAttribute>,
                                    SynthesizedAttribute>
{
   public:
      virtual ~AstBottomUpProcessingNew() {}
};
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// AstTopDownBottomUpWrapper
template< typename InheritedAttribute,
          typename SynthesizedAttribute >
class AstTopDownBottomUpBaseEvaluator
{
   public:
      typedef StackFrameVector<SynthesizedAttribute> SynthesizedAttributesList;
      typedef StackFrameVector<SynthesizedAttribute> SubTreeSynthesizedAttributes;

      virtual ~AstTopDownBottomUpBaseEvaluator() {}
   protected:
      virtual InheritedAttribute evaluateInheritedAttribute( SgNode*, InheritedAttribute ) = 0;
      virtual SynthesizedAttribute evaluateSynthesizedAttribute( SgNode*, InheritedAttribute, SynthesizedAttributesList ) = 0;

      virtual SynthesizedAttribute defaultSynthesizedAttribute( InheritedAttribute inheritedValue )
      {
         return SynthesizedAttribute();
      }
};

template< typename InheritedAttribute,
          typename SynthesizedAttribute >
class AstTopDownBottomUpProcessingNew
   : public AstProcessing::TopDownBottomUp<AstTopDownBottomUpBaseEvaluator<InheritedAttribute,
                                                                           SynthesizedAttribute>,
                                           InheritedAttribute,
                                           SynthesizedAttribute>
{
   public:
      virtual ~AstTopDownBottomUpProcessingNew() {}
};
// ============================================================================

// ============================================================================
// Traversal Implementations
// ============================================================================
#include "AstProcessing_TopDown_impl.h"
#include "AstProcessing_BottomUp_impl.h"
#include "AstProcessing_TopDownBottomUp_impl.h"
// ============================================================================
#endif
