#ifndef RESET_TEMPLATE_NAMES_H
#define RESET_TEMPLATE_NAMES_H

/*! \brief Sets names of template classes (calls custom traversal).
 */
void resetTemplateNames( SgNode* node );

/*! \brief Sets names of template classes.

    This function sets the names of template class instatiations to be of the form "ABC<int>" 
    instead of an internal form such as "ABC___L8".  In each case the old names are replaced 
    with the new names (overwritten).  The unparsed source code requires the new names, which
    the old names would only make since if ALL header files were to be unparsed.

    \internal This function calls the qualified name mechanism within Sage III and requires 
              all parent pointers to be set prior to being called.  Thus this traversal 
              MUST occur AFTER the post processing traversal to set the parents.  It the
              parent processing could be included (merged into the EDG/Sage III connection
              then it still might be that this traversal would have to be called within
              a post processing phase (because of the complexity of C and C++ some parents
              might not be set or known).
 */
class ResetTemplateNames 
   : public SgSimpleProcessing
   {
     public:
      //! Function to support traversal of types (where islands can hide)
          void visitType ( SgType* typeNode );

      //! Required traversal function
          void visit (SgNode* node);

      //! Function to support reset of types required for qualified names
          void resetTemplateNamesInAllScopes ( SgScopeStatement* scope );
   };

#if 1
// If this traveral is useful then the other one should be remove or commented out!
// DQ (2/10/2007): Converted this traversal to use the memory pool traversal
class ResetTemplateNamesOnMemoryPool
// : public SgSimpleProcessing
   : public ROSE_VisitTraversal
   {
     public:
      //! Function to support traversal of types (where islands can hide)
       // void visitType ( SgType* typeNode );

      //! Required traversal function
          void visit (SgNode* node);

      //! Function to support reset of types required for qualified names
       // void resetTemplateNamesInAllScopes ( SgScopeStatement* scope );

       // This avoids a warning by g++
         virtual ~ResetTemplateNamesOnMemoryPool() {};         
   };
#endif

// endif for RESET_TEMPLATE_NAMES_H
#endif
