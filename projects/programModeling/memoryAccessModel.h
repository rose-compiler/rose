#ifndef MEMORY_ACCESS_MODEL_H
#define MEMORY_ACCESS_MODEL_H

#include "programModel.h"

/*! \brief This modeling attribute marks memory accesses (there will be different types of memory accesses)

    This forms an access attribute classes to be used in modeling of different sorts 
    of memory accesses.

    \internal This could just be a base class and each of the different types of memory access attributes 
    could be its own class.  This would however result in a lot of classes and so it is not clear if that 
    is helpful.  In the equations that we generate each configuration of this attribute could form a variable
    in the equation.  Loops could record two summaries, one for the first (n == 0) iteration (triggering 
    cache misses) and a second one for the (n > 0) iterations where there would be fewer cache misses.

    \todo There is a lot of design evaluation work to do here!
 */

class AccessModelAttribute : public ModelingAttribute
// class AccessModelAttribute : public AccessModelSummaryAttribute
   {
     public:

      //! Memory access can originate from different language constructs
          enum AccessTypeEnum 
             {
               unknownAccess          = 0,
               registerAccess         = 1, //! marks likely register access
               scalarAccess           = 2, //! marks likely scalar access
               arrayAccess            = 3, //! marks likely array access
               arrayBasePointerAccess = 4, //! marks likely array access
               summaryAccess          = 5, //! not defined because the attribute is a summary of numerous memory accesses
               LAST_ACCESS_TYPE
             };

      //! Memory access can be from different location in the memory hierarchy
          enum MemoryLocationEnum 
             {
               unknownLocation  = 0,
               registerLocation = 1, //! access is likely from register
               L1Cache          = 2, //! access is likely from L1 cache
               L2Cache          = 3, //! access is likely from L2 cache
               L3Cache          = 4, //! access is likely from L3 cache
               mainMemory       = 5, //! access is likely from main memory
               summaryLocation  = 6, //! access is likely from main memory
               LAST_MODEL
             };

      //! For accesses to main memory, we support a TLB model 
          enum TLBLocationEnum
             {
               unknownTLBLocation = 0,
               cachedPage         = 1, //! access is likely from a cached page
               nonCachedPage      = 2, //! access is likely from a non-cached page
               summaryTLBLocation = 3, //! access is likely from a non-cached page
               LAST_TLB_LOCATION
             };

          AccessTypeEnum    accessType;
          MemoryLocationEnum memoryLocation;
          TLBLocationEnum    tlbLocation;

       // Pointer to the relavant subtree in the AST
          SgNode* astNode;

       // This is the support for the cache locatity metric based on stack distances
          int cacheLocalityMetric;

       // Constructor with defaults
          AccessModelAttribute (AccessTypeEnum accessTypeParameter         = scalarAccess,
                                MemoryLocationEnum memoryLocationParameter = mainMemory,
                                TLBLocationEnum    tlbLocationParameter    = nonCachedPage ) 
             : accessType(accessTypeParameter),
               memoryLocation(memoryLocationParameter),
               tlbLocation(tlbLocationParameter)
             {
            // compute the locality metric (punt for now)
               cacheLocalityMetric = INT_MAX;
             }
   };

/*! \brief This modeling attribute summarizes memory accesses

    \internal This attribute is attached to scopes to summarize memory accesses.
It might be that the user-defined summary attributes should be derived from the user-defined 
modeling attributes!  This would allow attributes to be added and where appropriate the attributes
could be summerized.  But we would need to have to have a function to check if the modeling attribute
really was a summary attribute, but then th summary could be done into the modling attribute where
appropriate.  The problem is that a summary attibut can have a lot of data (more then a modeling attribute),
but a modeling attribute would have to have specific values for accessType etc., which might not make sense.
Order of ModelingAttributes is important since it defines the likely access order (trace).

    \todo There is a lot of design evaluation work to do here!
 */
class AccessModelSummaryAttribute : public ModelingAttribute
   {
     public:
         int    numberOfAccesses;
         std::string equationString;
         std::string simplifiedEquationString;

      // Not clear how to handle branches (perhaps a vector of vectors of 
      // AccessModelAttribute, so that we can handle different control branches)
         typedef std::vector<AccessModelAttribute*> AccessListType;
         typedef std::vector<AccessListType>        BranchListType;

     // list of pointers to AccessModelAttribute is the subtrees that are summarized
         BranchListType branchList;

      // Should be have a list of summaries?
         std::vector<AccessModelSummaryAttribute*> summaryList;
   };

/*! \brief Inherited attribute for the suppport of attaching memory access information to the AST.

    \internal No context information is presently used, but this will change.  Not certain what
    context information is most important here!
 */
class MemoryAccessAttributeAttachmentInheritedAttribute
   {
     public:
         bool memoryAccess;

      //! Specific constructors are required
          MemoryAccessAttributeAttachmentInheritedAttribute ();
          MemoryAccessAttributeAttachmentInheritedAttribute ( const MemoryAccessAttributeAttachmentInheritedAttribute & X );
   };


/*! Synthesized attribute for the suppport of attaching memory access information to the AST.

    This attribute carries memory access information from child AST nodes and deposits it on 
    parent AST nodes.

    \internal Attach summary memory access information to specific IR nodes that represnt aggregates
    of expressions and statements. For expressions, aggregate information onto parent expressions and 
    any statement which is the parent of an expression.  For statements, aggregate information at scope
    statements (blocks, function declarations, etc.).
 */
class MemoryAccessAttributeAttachmentSynthesizedAttribute
   {
     public:
         int numberOfAccesses;
         std::string equationString;
         std::string simplifiedEquationString;

         MemoryAccessAttributeAttachmentSynthesizedAttribute();
   };

// tree traversal to test the rewrite mechanism
 /*! A specific AST processing class is used (built from SgTopDownBottomUpProcessing)
  */
class MemoryAccessAttributeAttachmentTraversal
   : public SgTopDownBottomUpProcessing<MemoryAccessAttributeAttachmentInheritedAttribute,MemoryAccessAttributeAttachmentSynthesizedAttribute>
   {
     public:
          MemoryAccessAttributeAttachmentTraversal () {};

       // Functions required by the rewrite mechanism
          MemoryAccessAttributeAttachmentInheritedAttribute evaluateInheritedAttribute (
             SgNode* astNode,
             MemoryAccessAttributeAttachmentInheritedAttribute inheritedAttribute );

          MemoryAccessAttributeAttachmentSynthesizedAttribute evaluateSynthesizedAttribute (
             SgNode* astNode,
             MemoryAccessAttributeAttachmentInheritedAttribute inheritedAttribute,
             SubTreeSynthesizedAttributes synthesizedAttributeList );
   };






namespace AccessModeling
   {
  // Attach modeling attributes to the AST
     void attachedAcessAttributes ( SgProject* project );

  // Call to Maple
     static std::string simplifyEquation ( std::string inputEquation );

  // build equation to handle loop bases and bounds
     std::string generateBaseBoundEquation ( SgExpression* expression );

  // Functions to build the equations strings
     void buildEquation( SgExpression*      exp, MemoryAccessAttributeAttachmentSynthesizedAttribute & attribute );
     void buildEquation( SgVarRefExp*       exp, MemoryAccessAttributeAttachmentSynthesizedAttribute & attribute );
     void buildEquation( SgUnaryOp*         exp, MemoryAccessAttributeAttachmentSynthesizedAttribute & attribute );
     void buildEquation( SgBinaryOp*        exp, MemoryAccessAttributeAttachmentSynthesizedAttribute & attribute );
     void buildEquation( SgFunctionRefExp*  exp, MemoryAccessAttributeAttachmentSynthesizedAttribute & attribute );
     void buildEquation( SgFunctionCallExp* exp, MemoryAccessAttributeAttachmentSynthesizedAttribute & attribute );
     void buildEquation( SgExprListExp*     exp, MemoryAccessAttributeAttachmentSynthesizedAttribute & attribute );

     void buildEquation( SgExprStatement* exprStatement, MemoryAccessAttributeAttachmentSynthesizedAttribute & attribute );
   }

// endif for MEMORY_ACCESS_MODEL_H
#endif
