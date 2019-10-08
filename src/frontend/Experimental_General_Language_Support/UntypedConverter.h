#ifndef UNTYPED_CONVERTER_H

#define UNTYPED_CONVERTER_H

//-----------------------------------------------------------------------------------
// The SgUntyped::UntypedConverter class functions are used to convert
// SgUntypedNodes to regular SgNodes.
//-----------------------------------------------------------------------------------

namespace Untyped {

typedef std::vector<SgLocatedNode*> SgNodePtrList;

class UntypedConverter
  {
    public:

      UntypedConverter(bool isCaseInsensitive = false)
         {
            pCaseInsensitive = isCaseInsensitive;

         // not currently converting a function declaration
            isConvertingFunctionDecl = false;
            isDefiningDeclaration    = false;
         }

      void  setSourcePositionFrom      ( SgLocatedNode* toNode, SgLocatedNode* fromNode );
      void  setSourcePositionIncluding ( SgLocatedNode* toNode, SgLocatedNode* startNode, SgLocatedNode* endNode );

      void  setSourcePositionUnknown   ( SgLocatedNode* locatedNode );

      SgScopeStatement* initialize_global_scope ( SgSourceFile* file );

      void buildProcedureSupport (SgUntypedFunctionDeclaration* ut_function,
                                  SgProcedureHeaderStatement* procedureDeclaration, SgScopeStatement* scope);

   // Replacement for API removed from SageBuilder
   // WARNING: This should be removed in a redesign using class member variables to save state.
      SgFunctionRefExp* buildFunctionRefExp(const SgName& name, SgScopeStatement* scope /*=NULL*/);

      virtual bool
      convertLabel(SgUntypedStatement* ut_stmt, SgStatement* sg_stmt, SgScopeStatement* label_scope=NULL) = 0;

      virtual bool
      convertLabel(SgUntypedStatement* ut_stmt, SgStatement* sg_stmt,
                   SgLabelSymbol::label_type_enum label_type, SgScopeStatement* scope=NULL) = 0;

      virtual SgInitializedName*        convertSgUntypedInitializedName     (SgUntypedInitializedName*     ut_name,      SgType* sg_base_type);
      virtual SgInitializedNamePtrList* convertSgUntypedInitializedNameList (SgUntypedInitializedNameList* ut_name_list, SgType* sg_base_type);

      virtual void convertFunctionPrefix (SgUntypedExprListExpression* prefix_list, SgFunctionDeclaration* function_decl);

      virtual void setDeclarationModifiers (SgDeclarationStatement* decl, SgUntypedExprListExpression* mod_list);

      virtual SgType*   convertUntypedType   (SgUntypedType* ut_type,            SgScopeStatement* scope);
      virtual SgType*   convertSgUntypedType (SgUntypedInitializedName* ut_name, SgScopeStatement* scope, bool delete_ut_type=false);

      virtual SgGlobal* convertUntypedGlobalScope (SgUntypedGlobalScope* ut_scope, SgScopeStatement* scope);

      virtual SgDeclarationStatement*     convertUntypedDirectiveDeclaration     (SgUntypedDirectiveDeclaration*     ut_decl, SgScopeStatement* scope);
      virtual SgEnumDeclaration*          convertUntypedEnumDeclaration          (SgUntypedEnumDeclaration*          ut_decl, SgScopeStatement* scope);

      virtual SgDeclarationStatement*     convertUntypedStructureDeclaration     (SgUntypedStructureDeclaration*     ut_decl, SgScopeStatement* scope) = 0;
      virtual SgTypedefDeclaration*       convertUntypedTypedefDeclaration       (SgUntypedTypedefDeclaration*       ut_decl, SgScopeStatement* scope);

      virtual SgModuleStatement*          convertUntypedModuleDeclaration        (SgUntypedModuleDeclaration*        ut_decl, SgScopeStatement* scope);
      virtual SgProgramHeaderStatement*   convertUntypedProgramHeaderDeclaration (SgUntypedProgramHeaderDeclaration* ut_decl, SgScopeStatement* scope);
      virtual SgProcedureHeaderStatement* convertUntypedSubroutineDeclaration    (SgUntypedSubroutineDeclaration*    ut_decl, SgScopeStatement* scope);
      virtual SgProcedureHeaderStatement* convertUntypedBlockDataDeclaration     (SgUntypedBlockDataDeclaration*     ut_decl, SgScopeStatement* scope);

      virtual SgFunctionDeclaration* convertUntypedFunctionDeclaration (SgUntypedFunctionDeclaration* ut_decl, SgScopeStatement* scope);
      virtual SgFunctionDeclaration* convertUntypedFunctionDeclaration (SgUntypedFunctionDeclaration* ut_decl, SgNodePtrList& children, SgScopeStatement* scope);

      virtual void convertUntypedFunctionDeclarationList (SgUntypedFunctionDeclarationList* ut_list, SgScopeStatement* scope);

   // Declaration statements
   //
      virtual SgDeclarationStatement* convertUntypedNameListDeclaration   (SgUntypedNameListDeclaration* ut_decl, SgScopeStatement* scope);
      virtual SgImplicitStatement*    convertSgUntypedImplicitDeclaration (SgUntypedImplicitDeclaration* ut_decl, SgScopeStatement* scope);
      virtual SgVariableDeclaration*  convertSgUntypedVariableDeclaration (SgUntypedVariableDeclaration* ut_decl, SgScopeStatement* scope);
      virtual SgBasicBlock*           convertSgUntypedBlockStatement      (SgUntypedBlockStatement*      ut_stmt, SgScopeStatement* scope);
      virtual SgUseStatement*         convertUntypedUseStatement          (SgUntypedUseStatement*        ut_stmt, SgScopeStatement* scope);

   // Used by dimension, codimension, ... statements
      virtual SgDeclarationStatement* convertSgUntypedInitializedNameListDeclaration (SgUntypedInitializedNameListDeclaration* ut_decl,
                                                                                      SgScopeStatement* scope);
   // Executable statements
   //
      virtual SgExprStatement* convertSgUntypedFunctionCallStatement (SgUntypedFunctionCallStatement* ut_stmt,
                                                                      SgNodePtrList& children, SgScopeStatement* scope);

      virtual SgExprStatement* convertSgUntypedAssignmentStatement (SgUntypedAssignmentStatement* ut_stmt, SgNodePtrList& children, SgScopeStatement* scope);
      virtual SgStatement*     convertUntypedCaseStatement         (SgUntypedCaseStatement*       ut_stmt, SgNodePtrList& children, SgScopeStatement* scope);
      virtual SgStatement*     convertSgUntypedExpressionStatement (SgUntypedExpressionStatement* ut_stmt, SgNodePtrList& children, SgScopeStatement* scope);
      virtual SgStatement*     convertUntypedForStatement          (SgUntypedForStatement*        ut_stmt, SgNodePtrList& children, SgScopeStatement* scope);
      virtual SgIfStmt*        convertSgUntypedIfStatement         (SgUntypedIfStatement*         ut_stmt, SgNodePtrList& children, SgScopeStatement* scope);
      virtual SgReturnStmt*    convertSgUntypedReturnStatement     (SgUntypedReturnStatement*     ut_stmt, SgNodePtrList& children, SgScopeStatement* scope);
      virtual SgStatement*     convertSgUntypedStopStatement       (SgUntypedStopStatement*       ut_stmt, SgNodePtrList& children, SgScopeStatement* scope);

      virtual SgStatement*     convertSgUntypedAbortStatement      (SgUntypedAbortStatement*  ut_stmt, SgScopeStatement* scope);
      virtual SgStatement*     convertSgUntypedExitStatement       (SgUntypedExitStatement*   ut_stmt, SgScopeStatement* scope);
      virtual SgStatement*     convertSgUntypedForAllStatement     (SgUntypedForAllStatement* ut_stmt, SgScopeStatement* scope);
      virtual SgStatement*     convertSgUntypedGotoStatement       (SgUntypedGotoStatement*   ut_stmt, SgScopeStatement* scope);
      virtual SgStatement*     convertSgUntypedLabelStatement_decl (SgUntypedLabelStatement*  ut_stmt, SgScopeStatement* scope);
      virtual SgStatement*     convertSgUntypedLabelStatement      (SgUntypedLabelStatement*  ut_stmt, SgStatement* stmt, SgScopeStatement* scope);
      virtual SgStatement*     convertUntypedNamedStatement        (SgUntypedNamedStatement*  ut_stmt, SgScopeStatement* scope);
      virtual SgNullStatement* convertSgUntypedNullStatement       (SgUntypedNullStatement*   ut_stmt, SgScopeStatement* scope);
      virtual SgStatement*     convertSgUntypedOtherStatement      (SgUntypedOtherStatement*  ut_stmt, SgScopeStatement* scope);
      virtual SgWhileStmt*     convertUntypedWhileStatement        (SgUntypedWhileStatement*  ut_stmt, SgNodePtrList& children, SgScopeStatement* scope);

   // Fortran image control statements
      virtual SgImageControlStatement* convertSgUntypedImageControlStatement (SgUntypedImageControlStatement* ut_stmt, SgScopeStatement* scope);
      virtual SgImageControlStatement* convertSgUntypedImageControlStatement (SgUntypedImageControlStatement* ut_stmt,
                                                                              SgNodePtrList& children, SgScopeStatement* scope);

   // Jovial specific statements
   //
      virtual SgDeclarationStatement* convertUntypedJovialCompoolStatement   (SgUntypedNameListDeclaration* ut_decl,
                                                                              SgScopeStatement* scope);

   // Expressions
   //
      virtual SgExpression* convertSgUntypedExpression      (SgUntypedExpression* ut_expr, bool delete_ut_expr=false);
      virtual SgExpression* convertSgUntypedExpression      (SgUntypedExpression* ut_expr, SgNodePtrList& children);
      virtual SgExpression* convertSgUntypedExpression      (SgUntypedExpression* ut_expr, SgNodePtrList& children, SgScopeStatement* scope);

      virtual SgValueExp*   convertSgUntypedValueExpression (SgUntypedValueExpression* ut_expr, bool delete_ut_expr=false);

      virtual SgExprListExp* convertSgUntypedExprListExpression  (SgUntypedExprListExpression * ut_expr_list, bool delete_ut_expr=false);
      virtual SgExprListExp* convertSgUntypedExprListExpression  (SgUntypedExprListExpression * ut_expr_list, SgNodePtrList& children);
      virtual SgExpression*  convertSgUntypedSubscriptExpression (SgUntypedSubscriptExpression* ut_expr, bool delete_ut_expr=false);
      virtual SgExpression*  convertSgUntypedSubscriptExpression (SgUntypedSubscriptExpression* ut_expr, SgNodePtrList& children);

      virtual SgExpression*    convertUntypedReferenceExpression (SgUntypedReferenceExpression* ut_expr, bool delete_ut_expr=false);
      virtual SgPntrArrRefExp* convertUntypedArrayReferenceExpression (SgUntypedArrayReferenceExpression* ut_expr,
                                                                       SgNodePtrList& children);

   // Operators
   //
      virtual SgUnaryOp*  convertUntypedUnaryOperator (SgUntypedUnaryOperator * untyped_operator, SgExpression* expr);
      virtual SgBinaryOp* convertUntypedBinaryOperator(SgUntypedBinaryOperator* untyped_operator, SgExpression* lhs, SgExpression* rhs);

    protected:

      bool pCaseInsensitive;

   // variables for converting functions declarations
      bool isConvertingFunctionDecl;
      bool isDefiningDeclaration;
  };

} // namespace Untyped

// endif for UNTYPED_CONVERTER_H
#endif

