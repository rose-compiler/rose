#ifndef INITIALIZE_EXPLICIT_SCOPE_DATA_H
#define INITIALIZE_EXPLICIT_SCOPE_DATA_H

// DQ (11/29/2004): Added support for the initialization of explicitly stored scope information.

/*! \brief Initialize explicit scopes (located only at specific IR nodes).
 */
void initializeExplicitScopes( SgNode* node );

/*! \brief Initialize explicit scopes (support function should not be called).
 */
void initializeExplicitScopeSupport( SgNode* node );

/*! \brief Support for initialization of explicit scopes (located only at specific IR nodes).

    This function initializes any uninitialized explicit scope data members. Some IR nodes have
    explicit scope information where the parent pointer is insufficient to identify the scope
    because of structural flexability in the layout of the C++ source code.

    \internal The explicitly stored scope information is called by the get_scope() function 
              on those IR nodes where the scope is explicitly stored.  Else the get_scope() 
              function computes the scope using either the paranet information or the results 
              of other get_scope() member function calls.
 */

// DQ (7/26/2007): Modified to reflect use of memory pool as an alternative traversal.
// class InitializeExplicitScopes : public SgSimpleProcessing
class InitializeExplicitScopes : public ROSE_VisitTraversal
   {
     public:
     virtual ~InitializeExplicitScopes() {};
      //! Function to support traversal of types (where islands can hide)
          void visitType ( SgType* typeNode );

      //! Required traversal function
          void visit (SgNode* node);
   };

// endif for INITIALIZE_EXPLICIT_SCOPE_DATA_H
#endif
