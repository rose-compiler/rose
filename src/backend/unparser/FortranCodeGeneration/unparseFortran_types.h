#ifndef UNPARSE_FORTRAN_TYPE
#define UNPARSE_FORTRAN_TYPE

#include "unparser.h"

class UnparseFortran_type
   {
     private:
          Unparser* unp;

     public:
          UnparseFortran_type(Unparser* unp):unp(unp){};
          virtual ~UnparseFortran_type() {};

          void curprint (const std::string & str) const;

          /**
           * @param printAttrs - true means print the type attributes such as dimension or length on the left of ::
           * for the given type; false means not.
           * the default is set to true so that all extant callers of this method are unaffected.
           */
          virtual void unparseType(SgType* type, SgUnparse_Info& info, bool printAttrs = true);

       // virtual void unparseTypeForConstExprs(SgType* type, SgUnparse_Info& info);

      //! unparse type functions implemented in unparse_type.C
       // DQ (4/25/2005): Made this virtual so that Gabriel could build a specialized unparser.

          /**
           * @param printAttrs - true means print the type attributes such as dimension or length on the left of ::
           * for the given type; false means not.
           * the default is set to true so that all extant callers of this method are unaffected.
           */
          virtual void unparsePointerType(SgType* type, SgUnparse_Info& info, bool printAttrs = true);

          virtual void unparseReferenceType(SgType* type, SgUnparse_Info& info);

       // This is for unparing the Fortran derived types
          virtual void unparseClassType(SgType* type, SgUnparse_Info& info);

          virtual void unparseModifierType(SgType* type, SgUnparse_Info& info);
          virtual void unparseFunctionType(SgType* type, SgUnparse_Info& info);

          /**
           * @param printDim - true means print the dimension attribute on the left of ::
           * for the given type; false means not.
           * the default is set to true so that all extant callers of this method are unaffected.
           */
          virtual void unparseArrayType(SgType* type, SgUnparse_Info& info, bool printDim = true);

          /**
           * @param printAttrs - true means print the type attributes such as dimension or length on the left of ::
           * for the given type; false means not.
           * the default is set to true so that all extant callers of this method are unaffected.
           */
          virtual void unparseStringType(SgType* type, SgUnparse_Info& info, bool printAttrs = true);

       // These are C++ specific code generation functions that are not likely required for Fortran
       // virtual void unparseMemberPointerType(SgType* type, SgUnparse_Info& info);
       // void unparseNameType(SgType* type, SgUnparse_Info& info);
       // virtual void unparseEnumType(SgType* type, SgUnparse_Info& info);
       // virtual void unparseTypedefType(SgType* type, SgUnparse_Info& info);
       // virtual void unparseMemberFunctionType(SgType* type, SgUnparse_Info& info);
       // virtual void unparseQualifiedNameType(SgType* type, SgUnparse_Info& info );

       // CI
       // virtual void unparseStructType(SgType* type, SgUnparse_Info& info);
       // virtual void unparseUnionType(SgType* type, SgUnparse_Info& info);

       // DQ (6/6/2007): Test for if types should be elaborated
          bool generateElaboratedType(SgDeclarationStatement* declarationStatement, const SgUnparse_Info & info );

          bool isCharType(SgType* type);

       // DQ (10/5/2010): Added unparse support for types with fortran kind parameters.
          void unparseTypeKind(SgType* type, SgUnparse_Info & info);
          void unparseTypeLengthAndKind(SgType* type, SgExpression* lengthExpression, SgUnparse_Info & info);
          void unparseBaseType(SgType* type, const std::string & nameOfType, SgUnparse_Info & info);
   };

#endif
