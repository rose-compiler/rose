#ifndef ROSE_BOOLEAN_QUERY
#define ROSE_BOOLEAN_QUERY

#include "AstProcessing.h"

// *************************************
// Prototypes for Variable Boolean Query
// *************************************

// forward declaration
class BooleanQueryInheritedAttributeType;

typedef bool BooleanQuerySynthesizedAttributeType;

/*!
     \defgroup booleanQueryLib Boolean Query Library
     \brief This class simplifies the development of queries on the AST resulting in a 
            single boolean value.
     \authors Quinlan
     \ingroup subtreeQueryLib

     This class represents a library of queries.. Basically it will support a large number of
     different types of queries that can be ask of an AST and that return a boolean value.
 */

/*! \brief This class simplifies the development of queries on the AST resulting in a 
           single boolean value.
    \ingroup booleanQueryLib

    This class used several static member function within it's interface.  Each member function
    takes an AST node pointer (any subtree of the AST).
 */
class BooleanQuery
   : public SgTopDownBottomUpProcessing<BooleanQueryInheritedAttributeType,BooleanQuerySynthesizedAttributeType>
   {
     public:

      /*! \if documentDevelopmentVersionUsingDoxygen 
               \brief Specify return of variable names or type names (list\<string\> is not specific enough so
                      we can't handle this).
          \endif
       */
          enum TypeOfQueryType
             {
               UnknownListElementType        =  0,
               VariableDeclaration           =  1,
               Type                          =  2,
               FunctionDeclaration           =  3,
               MemberFunctionDeclaration     =  4,
               ClassDeclaration              =  5,
               Argument                      =  6,
               Field                         =  7,
               UnionedField                  =  8,
               Struct                        =  9,
               ContainedInSubtreeOfType      = 10,
               END_OF_BOOLEAN_QUERY_TYPE
             };

      /*! @{ */
      /*! \if documentDevelopmentVersionUsingDoxygen 
               \brief default constructor, destructor, copy constructor and assignment operator.

                These are required only for internal use and are not part of the public interface.
          \endif
      */
         ~BooleanQuery();
          BooleanQuery();

          BooleanQuery( const BooleanQuery & X );
          BooleanQuery & operator= ( const BooleanQuery & X );
      /*! @} */

      /*! @{ */
      /*! @name Boolean Query Library Interface
          \brief Member functions called on each node.

          These function return true if any part of the subtree can be evaluated to be true.

          \internal A static interface function. We would like to have this be a static function, but as a
                    static function it can't call virtual functions which makes the design less attractive
                    since we would want to define the interface in a abstract base class.

          \internal The names of these functions don't communicate that they report containment instead of "is a" behavior.
       */
      //! does the AST contain a struct?
          static bool isStruct ( SgNode* astNode );
      //! does the AST contain a union?
          static bool isUnion  ( SgNode* astNode );

      //! checks chain of parents from current node to root of AST for node of targetVariant.
          static bool isContainedInSubtreeOfType ( SgNode* astNode, VariantT targetNodeVariant );

#if 0
          static nodeListType isTypeQuery                ( SgNode* astNode );
          static nodeListType isClassDeclarationQuery    ( SgNode* astNode );
          static nodeListType isFunctionDeclarationQuery ( SgNode* astNode );
          static nodeListType isFunctionReferenceQuery   ( SgNode* astNode );
          static nodeListType isMemberFunctionDeclarationQuery
             ( SgNode* astNode );
#endif
      /*! @} */

     private:
      //! internal support function
          static bool internalBooleanQuery (
               SgNode* astNode,
               TypeOfQueryType elementReturnType );

      //! Functions required by the global tree traversal mechanism
          BooleanQueryInheritedAttributeType
          evaluateInheritedAttribute (
             SgNode* astNode,
             BooleanQueryInheritedAttributeType inheritedValue );

      //! Functions required by the global tree traversal mechanism
          BooleanQuerySynthesizedAttributeType
          evaluateSynthesizedAttribute (
             SgNode* astNode,
             BooleanQueryInheritedAttributeType inheritedValue,
             SubTreeSynthesizedAttributes attributList );
   };




// *************************************************************
//       Boolean Traversal using TraverseToRoot mechanism
// *************************************************************

// Inherited Attribute
class BooleanQueryReverseTraversalInheritedAttribute
   {
     public:
       // Save the input parameter (the enum value for what we are checking the parent list to find).
          VariantT targetVariant;

          BooleanQueryReverseTraversalInheritedAttribute() : targetVariant(VariantT(0)) {}
   };

// Synthesized Attribute
class BooleanQueryReverseTraversalSynthesizedAttribute
   {
     public:
       // If found in the chain of parents to the AST root then we the synthesized attribute will be 
       // used to carry the boolean value back to the location from which the traversal begain!
          bool value;

          BooleanQueryReverseTraversalSynthesizedAttribute() : value(false) {}
   };

#if 0
// BooleanQueryReverseTraversal is derived from ReverseTraversal
class BooleanQueryReverseTraversal : public TraverseToRoot<BooleanQueryReverseTraversalInheritedAttribute,
                                                           BooleanQueryReverseTraversalSynthesizedAttribute >
   {
     public:
          BooleanQueryReverseTraversalInheritedAttribute evaluateInheritedAttribute (
               SgNode* node,
               BooleanQueryReverseTraversalInheritedAttribute inputInheritedAttribute );

          BooleanQueryReverseTraversalSynthesizedAttribute evaluateSynthesizedAttribute (
               SgNode* node,
               BooleanQueryReverseTraversalInheritedAttribute inputInheritedAttribute,
               BooleanQueryReverseTraversalSynthesizedAttribute inputSynthesizedAttribute );
   };
#endif

// endif for ROSE_BOOLEAN_QUERY
#endif















