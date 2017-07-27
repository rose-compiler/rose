#ifndef UNTYPED_CONVERTER_H
#define UNTYPED_CONVERTER_H

//-----------------------------------------------------------------------------------
// The SgUntyped::UntypedConverter class functions are used to convert
// SgUntypedNodes to regular SgNodes.
//-----------------------------------------------------------------------------------

#include "FASTNodes.hpp"
#include "FortranBuilderInterface.h"

namespace Fortran {
namespace Untyped {

class UntypedConverter
  {
    public:

      UntypedConverter(FortranBuilderInterface* builder) : pBuilder(builder)
        {
        }

      void  setSourcePositionFrom      ( SgLocatedNode* toNode, SgLocatedNode* fromNode );
      void  setSourcePositionIncluding ( SgLocatedNode* toNode, SgLocatedNode* startNode, SgLocatedNode* endNode );

      static void              setSourcePositionUnknown( SgLocatedNode* locatedNode );
      static SgScopeStatement* initialize_global_scope ( SgSourceFile* file );

             void              buildProcedureSupport (SgUntypedFunctionDeclaration* ut_function,
                                                      SgProcedureHeaderStatement* procedureDeclaration, SgScopeStatement* scope);


      static void convertLabel(SgUntypedStatement* ut_stmt, SgStatement* sg_stmt,
                               SgLabelSymbol::label_type_enum label_type = SgLabelSymbol::e_start_label_type, SgScopeStatement* label_scope = NULL);

      SgInitializedName*     convertSgUntypedInitializedName    (SgUntypedInitializedName* ut_name, SgType* sg_type, SgInitializer* sg_init);

             SgType*                convertSgUntypedType               (SgUntypedType* ut_type,                SgScopeStatement* scope);
      static SgGlobal*              convertSgUntypedGlobalScope        (SgUntypedGlobalScope* ut_scope,        SgScopeStatement* scope);

             SgModuleStatement*          convertSgUntypedModuleDeclaration        (SgUntypedModuleDeclaration*        ut_decl, SgScopeStatement* scope);
             SgProgramHeaderStatement*   convertSgUntypedProgramHeaderDeclaration (SgUntypedProgramHeaderDeclaration* ut_decl, SgScopeStatement* scope);
             SgProcedureHeaderStatement* convertSgUntypedSubroutineDeclaration    (SgUntypedSubroutineDeclaration*    ut_decl, SgScopeStatement* scope);
      static SgProcedureHeaderStatement* convertSgUntypedFunctionDeclaration      (SgUntypedFunctionDeclaration*      ut_decl, SgScopeStatement* scope);
      static void                        convertSgUntypedFunctionDeclarationList  (SgUntypedFunctionDeclarationList*  ut_list, SgScopeStatement* scope);

   // Declaration statements
   //
      SgImplicitStatement*   convertSgUntypedImplicitDeclaration (SgUntypedImplicitDeclaration* ut_decl, SgScopeStatement* scope);
      SgVariableDeclaration* convertSgUntypedVariableDeclaration (SgUntypedVariableDeclaration* ut_decl, SgScopeStatement* scope);

   // Executable statements
   //
      SgExprStatement* convertSgUntypedAssignmentStatement (SgUntypedAssignmentStatement* ut_stmt, SgExpressionPtrList& children, SgScopeStatement* scope);
      SgStatement*     convertSgUntypedExpressionStatement (SgUntypedExpressionStatement* ut_stmt, SgExpressionPtrList& children, SgScopeStatement* scope);
      SgStatement*     convertSgUntypedOtherStatement      (SgUntypedOtherStatement* ut_stmt, SgScopeStatement* scope);

   // Expressions
   //
      SgExpression* convertSgUntypedExpression      (SgUntypedExpression* ut_expr, SgExpressionPtrList& children, SgScopeStatement* scope);

      SgValueExp*   convertSgUntypedValueExpression (SgUntypedValueExpression* ut_expr);
      SgUnaryOp*    convertSgUntypedUnaryOperator   (SgUntypedUnaryOperator* untyped_operator, SgExpression* expr);
      SgBinaryOp*   convertSgUntypedBinaryOperator  (SgUntypedBinaryOperator* untyped_operator, SgExpression* lhs, SgExpression* rhs);

    private:
      FortranBuilderInterface* pBuilder;  // pointer to controlling builder class; DO NOT delete/free
  };

} // namespace Fortran
} // namespace Untyped

// endif for UNTYPED_CONVERTER_H
#endif

