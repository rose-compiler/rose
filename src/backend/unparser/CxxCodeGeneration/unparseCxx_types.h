
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
          virtual void unparseRvalueReferenceType(SgType* type, SgUnparse_Info& info);
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

       // DQ (11/20/2011): Adding support for template declarations in the AST.
          virtual void unparseTemplateType(SgType* type, SgUnparse_Info& info);

          virtual void unparseNonrealType(SgType* type, SgUnparse_Info& info, bool is_first_in_nonreal_chain = true);
          virtual void unparseAutoType(SgType* type, SgUnparse_Info& info);

       // DQ (7/31/2014): Adding support for nullptr constant expression and its associated type.
          virtual void unparseNullptrType(SgType* type, SgUnparse_Info& info);

       // DQ (8/2/2014): Adding support for C++11 decltype.
          virtual void unparseDeclType(SgType* type, SgUnparse_Info& info);

       // DQ (3/28/2015): Adding support for GNU C typeof language extension.
          virtual void unparseTypeOfType(SgType* type, SgUnparse_Info& info);

       // DQ (5/3/2013): This approach is no longer supported, as I recall.
       // virtual void unparseQualifiedNameType(SgType* type, SgUnparse_Info& info );

       // DQ (6/6/2007): Test for if types should be elaborated
          bool generateElaboratedType(SgDeclarationStatement* declarationStatement, const SgUnparse_Info & info );

       // DQ (12/11/2012): Refactored function to control output of "restrict" keyword (different for several backend compilers).
          static std::string unparseRestrictKeyword();

       // DQ (5/5/2013): Refactored code to support unparsing name qualified types.
          template <class T>
          void outputType(T* referenceNode, SgType* referenceNodeType, SgUnparse_Info & info);

       // DQ (4/15/2018): Adding support for output of template instantiations from template typedefs with template arguments with name qualification.
          void unparseTemplateTypedefName(SgTemplateInstantiationTypedefDeclaration* templateInstantiationTypedefDeclaration, SgUnparse_Info& info);

       // DQ (4/15/2018): This was part of a test (it can be removed now).
       // void foobar( SgUnparse_Info & info );

   };

#endif
