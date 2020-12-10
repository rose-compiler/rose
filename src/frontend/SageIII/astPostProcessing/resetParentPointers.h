#ifndef RESET_PARENT_POINTERS_H
#define RESET_PARENT_POINTERS_H

 // DQ (3/4/2003)
 /*! \brief Interface function to reset parent pointers.

     Interface for resetting parent pointers (called by temporaryAstFixes()
     function, but also required to reset parent pointers after any addition
     of new AST fragments to the AST).

     \internal This function can be called directly as well.
  */

 void resetParentPointers ( SgNode* node, SgNode* parent = NULL );

#if 0
 // DQ (3/4/2003)
 /*! \brief Interface function to reset parent pointers (for SgFile* only).

     Interface for resetting parent pointers (called by temporaryAstFixes()
     function, but also required to reset parent pointers after any addition
     of new AST fragements to the AST).

     \deprecated This function will be removed in favor of the more general 
                 version which takes any SgNode* (above).
  */
void resetParentPointers ( SgFile* node ) ROSE_DEPRECATED_FUNCTION;
#endif

/*
  All fixes implemented in this file are temporary.
  Changes necessary to remove it are:
  - ROSE/ROSETTA/Grammar/Support.code : remove: temporaryAstFixes(this);
  - ROSE/SAGE/Makefile.am : remove AstFixes.h and AstFixes.C
  - ROSE/SAGE/AstFixes.[hC] : remove files
*/

// *******************************************************************************************
// DQ (3/5/2003): Need to have this in the header file so that the static member data 
// modifiedNodeInformationList can be accessed within ROSE translators (this might change later).
// *******************************************************************************************
#include "AstNodeVisitMapping.h"

//! Inherited attribute required for ResetParentPointers class.
class ResetParentPointersInheritedAttribute
   {
     public:

      //! Default constructor
          ResetParentPointersInheritedAttribute(): parentNode(NULL) {}

       // DQ (8/1/2019): Copy constructor.
          ResetParentPointersInheritedAttribute(const ResetParentPointersInheritedAttribute & X ) { parentNode = X.parentNode; }

      //! Store previous node for reference and internal testing and output of debugging information
          SgNode* parentNode;
   };

/*! \brief This traversal implements the mechanism to reset all parent pointers (back edges in the AST graph)

    This traversal traverses the whole AST except types and symbols (future versions of the traversal 
    will traverse type and symbols)

    \internal This traversal is a demonstration of how to traverse the full AST including islands of code not 
    yet handled in the default traversal.  Nested traversals are used to implement the traversal of source code 
    (typically class definitions) hidden in types.
 */
class ResetParentPointers : public SgTopDownProcessing<ResetParentPointersInheritedAttribute>
   {
     public:
      /*! \brief This stores debugging output.

          This list of strings can be output as debugging information (in the future) 
          about what nodes had to be reset from unexpected values.
       */
       // static std::list<std::string> modifiedNodeInformationList;

      //! Required traversal function
          ResetParentPointersInheritedAttribute
               evaluateInheritedAttribute(SgNode* node, ResetParentPointersInheritedAttribute inheritedAttribute);

      //! Test function to test parent pointers (from any point back to the root)
          void traceBackToRoot ( SgNode* node );

      //! resets pointers in islands of AST code not currently traversed (hidden in types or arrays of types)
          void resetParentPointersInType (SgType* typeNode, SgNode* previousNode );

      /*! \brief Reset parents of referenced defining and first non-defining declaration.

          SgDeclarationStatement objects contain references to the defining and first non-defining
          declarations, both should have their parents set.  In general this will be sufficient to
          set all the associated declarations used internally is they are shared.  Declaration
          statements that are explicit forward declarations are however not all referencing the
          first non-defining declaration (since this would violate the rule of uniqueness of
          statements (only enforced within a single scope).
       */
          void resetParentPointersInDeclaration (SgDeclarationStatement* declaration, SgNode* inputParent);

      /*! \brief Reset parent pointers appearing in subtrees represnting the template arguments

          This function traverses the list of template arguments and looks for SgNameTypes 
          and reset the parents in their associated declarations.

          \internal This could be eliminated if we were to traverse the template arguments (not clear if that is a good idea).
       */
          void resetParentPointersInTemplateArgumentList ( const SgTemplateArgumentPtrList& templateArgList );
   };


/*! \brief This is a top level function not called recursively.

   This function calls resetParentPointer() which is a recursively 
   called function which would be difficult to obtain performance 
   information from directly.
 */
void topLevelResetParentPointer ( SgNode* node );


/*! \brief This is a top level function not called recursively.

   This function call the traversal to reset the parent pointers of 
   data members in class definitions, namespace definitions and global 
   scope.
 */
void resetParentPointersOfClassOrNamespaceDeclarations ( SgNode* node );

/*! \brief This traversal implements the mechanism to reset all parent pointers (back edges in the AST graph)

    This traversal traverses the more of the AST (but is not a memory pool traversal) except types and symbols 
    (future versions of the traversal will traverse type and symbols).

    \internal This traversal is a demonstration of how to traverse the full AST including islands of code not 
    yet handled in the default traversal.  Nested traversals are used to implement the traversal of source code 
    (typically class definitions) hidden in types.
 */
class ResetParentPointersOfClassAndNamespaceDeclarations : public SgSimpleProcessing
   {
     public:
      //! Required traversal function
          void visit (SgNode* node);
   };


/*! \brief This traversal calls ResetFileInfoParentPointersInMemoryPool Memory Pool traversal.
 */
void resetFileInfoParentPointersInMemoryPool();

/*! \brief This traversal uses the Memory Pool traversal to fixup remaining parent pointers in Sg_File_Info objects.
 */
class ResetFileInfoParentPointersInMemoryPool : public ROSE_VisitTraversal
   {
     public:
          virtual ~ResetFileInfoParentPointersInMemoryPool() {};

      //! Required traversal function
          void visit (SgNode* node);
   };

/*! \brief This traversal calles ResetParentPointersInMemoryPool Memory Pool traversal.
 */
// DQ (8/23/2012): Modified to take a SgNode so that we could compute the global scope for use in setting 
// parents of template instantiations that have not be placed into the AST but exist in the memory pool.
// void resetParentPointersInMemoryPool();
void resetParentPointersInMemoryPool(SgNode* node);

/*! \brief This traversal uses the Memory Pool traversal to fixup remaining parent pointers.

    This traversal uses the Memory Pool traversal to fixup remaining parent pointers 
    (e.g. declarations that are hidden from the AST traversal). This traversal traverses the 
    whole AST using the memory pool traversal.
 */
class ResetParentPointersInMemoryPool : public ROSE_VisitTraversal
   {
     public:
          SgGlobal* globalScope;

       // DQ (8/23/2012): Added a constructor to take SgGlobal so that we will have it available to use 
       // for setting parents of SgTemplateInstatiations that are not in the AST but in the memory pool.
          ResetParentPointersInMemoryPool(SgGlobal* n) : globalScope(n) {};

          virtual ~ResetParentPointersInMemoryPool() {};
      //! Required traversal function
          void visit (SgNode* node);
   };

// endif for RESET_PARENT_POINTERS_H
#endif


