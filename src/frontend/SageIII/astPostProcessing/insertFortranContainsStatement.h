
#ifndef INSERT_FORTRAN_CONTAINS_STATEMENT_H
#define INSERT_FORTRAN_CONTAINS_STATEMENT_H

// DQ (11/24/2007):
/*! \brief Insert Fortran "CONTAINS" statement.

   OFP does not currently process a rule for the CONTAINS statement.
   Handling it can be automatic and we we have a fixup for the AST
   to introduce the CONTAINS statement automatically.

    \implementation This may not be required if OFP is fixed.
 */
void
insertFortranContainsStatement ( SgNode* node );

class InsertFortranContainsStatement : public AstSimpleProcessing
   {
     public:
          void visit ( SgNode* node );
   };

// endif for INSERT_FORTRAN_CONTAINS_STATEMENT_H
#endif
