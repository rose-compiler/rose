
/* unparser.h
 * This header file contains the class declaration for the newest unparser. Six
 * C files include this header file: unparser.C, modified_sage.C, unparse_stmt.C, 
 * unparse_expr.C, unparse_type.C, and unparse_sym.C.
 */

#ifndef UNPARSER_TYPE
#define UNPARSER_TYPE

#include "unparser.h"

class SgType;
class SgDeclarationStatement;
// class Unparser;

class Unparse_Type
   {
     private:
          Unparser* unp;

     public:
       // GB (09/20/2007): Moved definitions to source file.
       // Unparse_Type(Unparser* unp):unp(unp){};
       // virtual ~Unparse_Type() {};
          Unparse_Type(Unparser* unp);
          virtual ~Unparse_Type();

          void curprint (std::string str);
          virtual void unparseType(SgType* type, SgUnparse_Info& info);

      //! unparse type functions implemented in unparse_type.C
       // DQ (4/25/2005): Made this virtual so that Gabriel could build a specialized unparser.

          virtual void unparsePointerType(SgType* type, SgUnparse_Info& info);
          virtual void unparseMemberPointerType(SgType* type, SgUnparse_Info& info);
          virtual void unparseReferenceType(SgType* type, SgUnparse_Info& info);
       // void unparseNameType(SgType* type, SgUnparse_Info& info);
          virtual void unparseClassType(SgType* type, SgUnparse_Info& info);
       // CI
       // virtual void unparseStructType(SgType* type, SgUnparse_Info& info);
          virtual void unparseEnumType(SgType* type, SgUnparse_Info& info);
       // CI
       // virtual void unparseUnionType(SgType* type, SgUnparse_Info& info);
          virtual void unparseTypedefType(SgType* type, SgUnparse_Info& info);
          virtual void unparseModifierType(SgType* type, SgUnparse_Info& info);
          virtual void unparseFunctionType(SgType* type, SgUnparse_Info& info);
          virtual void unparseMemberFunctionType(SgType* type, SgUnparse_Info& info);
          virtual void unparseArrayType(SgType* type, SgUnparse_Info& info);
          virtual void unparseQualifiedNameType(SgType* type, SgUnparse_Info& info );

       // DQ (6/6/2007): Test for if types should be elaborated
          bool generateElaboratedType(SgDeclarationStatement* declarationStatement, const SgUnparse_Info & info );
   };

#endif
