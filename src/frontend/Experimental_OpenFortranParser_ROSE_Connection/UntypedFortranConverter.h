#ifndef UNTYPED_FORTRAN_CONVERTER_H
#define UNTYPED_FORTRAN_CONVERTER_H

//-----------------------------------------------------------------------------------
// The SgUntyped::UntypedConverter class functions are used to convert
// SgUntypedNodes to regular SgNodes.
//-----------------------------------------------------------------------------------

#include "UntypedConverter.h"

namespace Untyped {

   class UntypedFortranConverter : public UntypedConverter
  {
    public:

      UntypedFortranConverter() : UntypedConverter(true /* is case insensitive */)
         {
         }

      virtual bool
      convertLabel(SgUntypedStatement* ut_stmt, SgStatement* sg_stmt, SgScopeStatement* label_scope = NULL);

      virtual bool
      convertLabel(SgUntypedStatement* ut_stmt, SgStatement* sg_stmt,
                   SgLabelSymbol::label_type_enum label_type, SgScopeStatement* label_scope=NULL);

      void
      setFortranNumericLabel(SgStatement* stmt, int label_value, SgLabelSymbol::label_type_enum label_type, SgScopeStatement* label_scope = NULL);

      virtual SgProcedureHeaderStatement* convertUntypedSubroutineDeclaration   (SgUntypedSubroutineDeclaration*   ut_decl, SgScopeStatement* scope);
      virtual SgProcedureHeaderStatement* convertUntypedBlockDataDeclaration    (SgUntypedBlockDataDeclaration*    ut_decl, SgScopeStatement* scope);

      virtual SgProcedureHeaderStatement* convertUntypedFunctionDeclaration     (SgUntypedFunctionDeclaration*     ut_decl, SgScopeStatement* scope);
      virtual void                        convertUntypedFunctionDeclarationList (SgUntypedFunctionDeclarationList* ut_list, SgScopeStatement* scope);

   // Declaration statements
   //
      virtual SgDeclarationStatement* convertUntypedNameListDeclaration  (SgUntypedNameListDeclaration*  ut_decl, SgScopeStatement* scope);
      virtual SgImplicitStatement*    convertUntypedImplicitDeclaration  (SgUntypedImplicitDeclaration*  ut_decl, SgScopeStatement* scope);
      virtual SgVariableDeclaration*  convertUntypedVariableDeclaration  (SgUntypedVariableDeclaration*  ut_decl, SgScopeStatement* scope);
      virtual SgDeclarationStatement* convertUntypedStructureDeclaration (SgUntypedStructureDeclaration* ut_decl, SgScopeStatement* scope);

   // Executable statements
   //
      virtual SgStatement* convertUntypedExpressionStatement (SgUntypedExpressionStatement* ut_stmt, SgNodePtrList& children, SgScopeStatement* scope);
      virtual SgStatement* convertUntypedForStatement        (SgUntypedForStatement*        ut_stmt, SgNodePtrList& children, SgScopeStatement* scope);
      virtual SgStatement* convertUntypedOtherStatement      (SgUntypedOtherStatement* ut_stmt, SgScopeStatement* scope);

   // Fortran image control statements
      virtual SgImageControlStatement* convertUntypedImageControlStatement (SgUntypedImageControlStatement* ut_stmt, SgScopeStatement* scope);
      virtual SgImageControlStatement* convertUntypedImageControlStatement (SgUntypedImageControlStatement* ut_stmt,
                                                                            SgNodePtrList& children, SgScopeStatement* scope);
   // Expressions
   //
  };

} // namespace Untyped

// endif for UNTYPED_FORTRAN_CONVERTER_H
#endif

