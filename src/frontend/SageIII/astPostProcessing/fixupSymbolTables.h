#ifndef FIXUP_SYMBOL_TABLES_H
#define FIXUP_SYMBOL_TABLES_H

// DQ (6/26/2005):
/*! \brief Fixup the names used build the global function symbol table.

    EDG builds function types using the return type and parameter types of the functions (but not the function name).
    Functions of the same type (function type) resolve to the same funtion type symbol.  Since the names of the types 
    are used, the qualified type names are required.  Within the initial translation of the EDG AST to the Sage III AST, 
    the template names are not prperly set (this is a fixup phase after done after the translation).  At the end of the 
    EDG/Sage III, we have a valid global function type symbol table, but it is hashed using the wrong names (that is the 
    names of the types used to build each symbol have been modified).  So the global function type symbol table must be 
    rebuilt).  Actually it might no longer really be required unless we are building more functions!  But we should 
    rebuild it.  Since the symbols are already correct, all we want to do is rename the symbols, but the names are not
    stored in the symbols, so perhaps we don't have to do anything, except:  
      -# Suppress the error messages that complain about template names being used before they are reset (since they are 
         used in the generation of mangled names of types).
      -# Rebuild the global table since the symbols names used to hask them into place in the table have changed and 
         now the symbols can not be found using there new names.

    \implementation 
 */
void 
fixupGlobalFunctionSymbolTable (SgFunctionTypeTable* globalFunctionTypeSymbolTable);


class FixUpGlobalFunctionTypeTable : public ROSE_VisitTraversal
   {
     public:
     virtual ~FixUpGlobalFunctionTypeTable() {};
      //! Required traversal function
          void visit (SgNode* node);
   };


// DQ (6/27/2005):
/*! \brief Fixup global and local symbol tables.

    The global symbol table needs to be rebuilt (since it was originally constructed using EDG names 
    and we have changed the names of templates to (more readable) Sage III names (e.g. converting 
    "_A___L1234" to "A < int >").

    Local symbol tables are fixed as well, but all that apepars required is to give all scopes 
    (e.g. empty scopes) a valid symbol table pointer.

    \implementation the global function type symbol table is not rebuilt yet.
 */
void
fixupAstSymbolTables ( SgNode* node );

class FixupAstSymbolTables : public AstSimpleProcessing
   {
  // This class uses a traversal to test the values of the definingDeclaration and
  // firstNondefiningDeclaration pointers in each SgDeclarationStatement.  See code for
  // details, since both of these pointers are not always set.

     public:
          void visit ( SgNode* node );
   };

// endif for FIXUP_SYMBOL_TABLES_H
#endif
