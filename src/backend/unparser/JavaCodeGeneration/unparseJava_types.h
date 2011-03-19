#ifndef UNPARSER_JAVA_TYPE
#define UNPARSER_JAVA_TYPE

/* unparser.h
 * This header file contains the class declaration for the newest unparser. Six
 * C files include this header file: unparser.C, modified_sage.C, unparse_stmt.C, 
 * unparse_expr.C, unparse_type.C, and unparse_sym.C.
 */

#include "unparser.h"

class SgType;
class SgDeclarationStatement;

class UnparseJava_type
   {
     private:
          Unparser* unp;

     public:
          UnparseJava_type(Unparser* unp); // :unp(unp){};
          virtual ~UnparseJava_type(); // {};

          void curprint (std::string str);
          virtual void unparseType(SgType* type, SgUnparse_Info& info);

      //! unparse type functions implemented in unparse_type.C (virtual to support GUI-based specialized unparser for Gabriel).
          virtual void unparsePointerType(SgType* type, SgUnparse_Info& info);
          virtual void unparseMemberPointerType(SgType* type, SgUnparse_Info& info);
          virtual void unparseReferenceType(SgType* type, SgUnparse_Info& info);
          virtual void unparseClassType(SgType* type, SgUnparse_Info& info);
          virtual void unparseEnumType(SgType* type, SgUnparse_Info& info);
          virtual void unparseTypedefType(SgType* type, SgUnparse_Info& info);
          virtual void unparseModifierType(SgType* type, SgUnparse_Info& info);
          virtual void unparseFunctionType(SgType* type, SgUnparse_Info& info);
          virtual void unparseMemberFunctionType(SgType* type, SgUnparse_Info& info);
          virtual void unparseArrayType(SgType* type, SgUnparse_Info& info);
          virtual void unparseQualifiedNameType(SgType* type, SgUnparse_Info& info );
   };

#endif
