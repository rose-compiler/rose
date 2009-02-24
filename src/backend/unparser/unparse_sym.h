
/* unparser.h
 * This header file contains the class declaration for the newest unparser. Six
 * C files include this header file: unparser.C, modified_sage.C, unparse_stmt.C, 
 * unparse_expr.C, unparse_type.C, and unparse_sym.C.
 */

#ifndef UNPARSER_SYM
#define UNPARSER_SYM


#include "unparser.h"

class Unparser;

class Unparse_Sym {
 private:
  Unparser* unp;
 public:
  Unparse_Sym(Unparser* unp):unp(unp){};
  virtual ~Unparse_Sym() {};

      //! unparse symbol functions implemented in unparse_sym.C
       // DQ (4/25/2005): Made this virtual so that Gabriel could build a specialized unparser.
          virtual void unparseSymbol(SgSymbol* sym, SgUnparse_Info& info);

          virtual void unparseVarSymbol(SgSymbol* sym, SgUnparse_Info& info);
          virtual void unparseFunctionSymbol(SgSymbol* sym, SgUnparse_Info& info);
          virtual void unparseFuncTypeSymbol(SgSymbol* sym, SgUnparse_Info& info);
          virtual void unparseClassSymbol(SgSymbol* sym, SgUnparse_Info& info);
       // virtual void unparseUnionSymbol(SgSymbol* sym, SgUnparse_Info& info);
       // virtual void unparseStructSymbol(SgSymbol* sym, SgUnparse_Info& info);
          virtual void unparseEnumSymbol(SgSymbol* sym, SgUnparse_Info& info);
          virtual void unparseFieldSymbol(SgSymbol* sym, SgUnparse_Info& info);
          virtual void unparseTypedefSymbol(SgSymbol* sym, SgUnparse_Info& info);
          virtual void unparseMFunctionSymbol(SgSymbol* sym, SgUnparse_Info& info);
          virtual void unparseLabelSymbol(SgSymbol* sym, SgUnparse_Info& info);
       // virtual void unparseConstructSymbol(SgSymbol* sym, SgUnparse_Info& info);
          virtual void unparseTemplateSymbol(SgSymbol* sym, SgUnparse_Info& info);
          virtual void unparseNamespaceSymbol(SgSymbol* sym, SgUnparse_Info& info);

};

#endif


