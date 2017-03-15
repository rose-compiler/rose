#ifndef UNTYPED_CONVERTER_H
#define UNTYPED_CONVERTER_H

//-----------------------------------------------------------------------------------
// The SgUntyped::UntypedConverter class functions are used to convert
// SgUntypedNodes to regular SgNodes.
//-----------------------------------------------------------------------------------

namespace Fortran {
namespace Untyped {

class UntypedConverter
  {
    public:

      static void              setSourcePositionUnknown( SgLocatedNode* locatedNode );
      static SgScopeStatement* initialize_global_scope ( SgSourceFile* file );

      static void              buildProcedureSupport (SgUntypedFunctionDeclaration* ut_function,
                                                      SgProcedureHeaderStatement* procedureDeclaration, SgScopeStatement* scope);


      static void convertLabel(SgUntypedStatement* ut_stmt, SgStatement* sg_stmt,
                               SgLabelSymbol::label_type_enum label_type = SgLabelSymbol::e_start_label_type, SgScopeStatement* label_scope = NULL);

      static SgInitializedName*     convertSgUntypedInitializedName    (SgUntypedInitializedName* ut_name, SgType* sg_type, SgInitializer* sg_init);

      static SgType*                convertSgUntypedType               (SgUntypedType* ut_type,                SgScopeStatement* scope);
      static SgGlobal*              convertSgUntypedGlobalScope        (SgUntypedGlobalScope* ut_scope,        SgScopeStatement* scope);

      static void                        convertSgUntypedFunctionDeclarationList  (SgUntypedFunctionDeclarationList*  ut_list, SgScopeStatement* scope);
      static SgProgramHeaderStatement*   convertSgUntypedProgramHeaderDeclaration (SgUntypedProgramHeaderDeclaration* ut_decl, SgScopeStatement* scope);
      static SgProcedureHeaderStatement* convertSgUntypedSubroutineDeclaration    (SgUntypedSubroutineDeclaration*    ut_decl, SgScopeStatement* scope);
      static SgProcedureHeaderStatement* convertSgUntypedFunctionDeclaration      (SgUntypedFunctionDeclaration*      ut_decl, SgScopeStatement* scope);

   // Declaration statements
   //
      static SgImplicitStatement*   convertSgUntypedImplicitDeclaration (SgUntypedImplicitDeclaration* ut_decl, SgScopeStatement* scope);
      static SgVariableDeclaration* convertSgUntypedVariableDeclaration (SgUntypedVariableDeclaration* ut_decl, SgScopeStatement* scope);

   // Executable statements
   //
      static SgExprStatement* convertSgUntypedAssignmentStatement (SgUntypedAssignmentStatement* ut_stmt, SgExpressionPtrList& children, SgScopeStatement* scope);

   // Expressions
   //
      static SgExpression* convertSgUntypedExpression      (SgUntypedExpression* ut_expr, SgExpressionPtrList& children, SgScopeStatement* scope);

      static SgValueExp*   convertSgUntypedValueExpression (SgUntypedValueExpression* ut_expr);
      static SgUnaryOp*    convertSgUntypedUnaryOperator   (SgUntypedUnaryOperator* untyped_operator, SgExpression* expr);
      static SgBinaryOp*   convertSgUntypedBinaryOperator  (SgUntypedBinaryOperator* untyped_operator, SgExpression* lhs, SgExpression* rhs);

  };

} // namespace Fortran
} // namespace Untyped

// endif for UNTYPED_CONVERTER_H
#endif

