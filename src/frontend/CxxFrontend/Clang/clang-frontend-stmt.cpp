#include "sage3basic.h"
#include "clang-frontend-private.hpp"

SgNode * ClangToSageTranslator::Traverse(clang::Stmt * stmt) {
    if (stmt == NULL)
        return NULL;

    std::map<clang::Stmt *, SgNode *>::iterator it = p_stmt_translation_map.find(stmt);
    if (it != p_stmt_translation_map.end())
        return it->second; 

    SgNode * result = NULL;
    bool ret_status = false;

    switch (stmt->getStmtClass()) {
// no longer available in Clang 9
//        case clang::Stmt::AsmStmtClass:
//    //      ret_status = VisitAsmStmt((clang::AsmStmt *)stmt, &result);
//            break;
        case clang::Stmt::GCCAsmStmtClass:
    //      ret_status = VisitAsmStmt((clang::AsmStmt *)stmt, &result);
            break;
        case clang::Stmt::MSAsmStmtClass:
    //      ret_status = VisitAsmStmt((clang::AsmStmt *)stmt, &result);
            break;
        case clang::Stmt::BreakStmtClass:
            ret_status = VisitBreakStmt((clang::BreakStmt *)stmt, &result);
            break;
        case clang::Stmt::CompoundStmtClass:
            ret_status = VisitCompoundStmt((clang::CompoundStmt *)stmt, &result);
            break;
        case clang::Stmt::ContinueStmtClass:
            ret_status = VisitContinueStmt((clang::ContinueStmt *)stmt, &result);
            break;
        case clang::Stmt::CXXCatchStmtClass:
    //      ret_status = VisitCXXCatchStmt((clang::CXXCatchStmt *)stmt, &result);
            break;
        case clang::Stmt::CXXForRangeStmtClass:
    //      ret_status = VisitCXXForRangeStmt((clang::CXXForRangeStmt *)stmt, &result);
            break;
        case clang::Stmt::CXXTryStmtClass:
    //      ret_status = VisitCXXTryStmt((clang::CXXTryStmt *)stmt, &result);
            break;
        case clang::Stmt::DeclStmtClass:
            ret_status = VisitDeclStmt((clang::DeclStmt *)stmt, &result);
            break;
        case clang::Stmt::DoStmtClass:
            ret_status = VisitDoStmt((clang::DoStmt *)stmt, &result);
            break;
        case clang::Stmt::BinaryConditionalOperatorClass:
    //      ret_status = VisitBinaryConditionalOperator((clang::BinaryConditionalOperator *)stmt, &result);
            break;
        case clang::Stmt::ConditionalOperatorClass:
            ret_status = VisitConditionalOperator((clang::ConditionalOperator *)stmt, &result);
            break;
        case clang::Stmt::AddrLabelExprClass:
    //      ret_status = VisitAddrLabelExpr((clang::AddrLabelExpr *)stmt, &result);
            break;
        case clang::Stmt::ArraySubscriptExprClass:
            ret_status = VisitArraySubscriptExpr((clang::ArraySubscriptExpr *)stmt, &result);
            break;
        case clang::Stmt::ArrayTypeTraitExprClass:
    //      ret_status = VisitArrayTypeTraitExpr((clang::ArrayTypeTraitExpr *)stmt, &result);
            break;
        case clang::Stmt::AsTypeExprClass:
    //      ret_status = VisitAsTypeExpr((clang::AsTypeExpr *)stmt, &result);
            break;
        case clang::Stmt::AtomicExprClass:
    //      ret_status = VisitAtomicExpr((clang::AtomicExpr *)stmt, &result);
            break;
        case clang::Stmt::BinaryOperatorClass:
            ret_status = VisitBinaryOperator((clang::BinaryOperator *)stmt, &result);
            break;
        case clang::Stmt::CompoundAssignOperatorClass:
    //      ret_status = VisitCompoundAssignOperator((clang::CompoundAssignOperator *)stmt, &result);
            break;
// BinaryTypeTraitExpr is replaced by TypeTraitExprClass
        case clang::Stmt::TypeTraitExprClass:
    //      ret_status = VisitTypeTraitExpr((clang::BinaryTypeTraitExpr *)stmt, &result);
            break;
//        case clang::Stmt::BlockDeclRefExprClass:
//    //      ret_status = VisitBlockDeclRefExpr((clang::BlockDeclRefExpr *)stmt, &result);
//            break;
        case clang::Stmt::BlockExprClass:
    //      ret_status = VisitBlockExpr((clang::BlockExpr * )stmt, &result);
            break;
        case clang::Stmt::CallExprClass:
            ret_status = VisitCallExpr((clang::CallExpr *)stmt, &result);
            break;
        case clang::Stmt::CUDAKernelCallExprClass:
    //      ret_status = VisitCUDAKernelCallExpr((clang::CUDAKernelCallExpr *)stmt, &result);
            break;
        case clang::Stmt::CXXMemberCallExprClass:
    //      ret_status = VisitCXXMemberCallExpr((clang::CXXMemberCallExpr *)stmt, &result);
            break;
        case clang::Stmt::CXXOperatorCallExprClass:
    //      ret_status = VisitCXXOperatorCallExpr((clang::CXXOperatorCallExpr *)stmt, &result);
            break;
        case clang::Stmt::CStyleCastExprClass:
            ret_status = VisitCStyleCastExpr((clang::CStyleCastExpr *)stmt, &result);
            break;
        case clang::Stmt::CXXFunctionalCastExprClass:
    //      ret_status = VisitCXXFunctionalCastExpr((clang::CXXFunctionalCastExpr *)stmt, &result);
            break;
        case clang::Stmt::CXXConstCastExprClass:
    //      ret_status = VisitCXXConstCastExpr((clang::CXXConstCastExpr *)stmt, &result);
            break;
        case clang::Stmt::CXXDynamicCastExprClass:
    //      ret_status = VisitCXXDynamicCastExpr((clang::CXXDynamicCastExpr *)stmt, &result);
            break;
        case clang::Stmt::CXXReinterpretCastExprClass:
    //      ret_status = VisitCXXReinterpretCastExpr((clang::CXXReinterpretCastExpr *)stmt, &result);
            break;
        case clang::Stmt::CXXStaticCastExprClass:
    //      ret_status = VisitCXXStaticCastExpr((clang::CXXStaticCastExpr *)stmt, &result);
            break;
        case clang::Stmt::ImplicitCastExprClass:
            ret_status = VisitImplicitCastExpr((clang::ImplicitCastExpr *)stmt, &result);
            break;
        case clang::Stmt::CharacterLiteralClass:
            ret_status = VisitCharacterLiteral((clang::CharacterLiteral *)stmt, &result);
            break;
        case clang::Stmt::ChooseExprClass:
    //      ret_status = VisitChooseExpr((clang::ChooseExpr *)stmt, &result);
            break;
        case clang::Stmt::CompoundLiteralExprClass:
            ret_status = VisitCompoundLiteralExpr((clang::CompoundLiteralExpr *)stmt, &result);
            break;
        case clang::Stmt::CXXBindTemporaryExprClass:
    //      ret_status = VisitCXXBindTemporaryExpr((clang::CXXBindTemporaryExpr *)stmt, &result);
            break;
        case clang::Stmt::CXXBoolLiteralExprClass:
    //      ret_status = VisitCXXBoolLiteralExpr((clang::CXXBoolLiteralExpr *)stmt, &result);
            break;
        case clang::Stmt::CXXConstructExprClass:
    //      ret_status = VisitCXXConstructExpr((clang::CXXConstructExpr *)stmt, &result);
            break;
        case clang::Stmt::CXXTemporaryObjectExprClass:
    //      ret_status = VisitCXXTemporaryObjectExpr((clang::CXXTemporaryObjectExpr *)stmt, &result);
            break;
        case clang::Stmt::CXXDefaultArgExprClass:
    //      ret_status = VisitCXXDefaultArgExpr((clang::CXXDefaultArgExpr *)stmt, &result);
            break;
        case clang::Stmt::CXXDeleteExprClass:
    //      ret_status = VisitCXXDeleteExpr((clang::CXXDeleteExpr *)stmt, &result);
            break;
        case clang::Stmt::CXXDependentScopeMemberExprClass:
    //      ret_status = VisitCXXDependentScopeMemberExpr((clang::CXXDependentScopeMemberExpr *)stmt, &result);
            break;
        case clang::Stmt::CXXNewExprClass:
    //      ret_status = VisitCXXNewExpr((clang::CXXNewExpr *)stmt, &result);
            break;
        case clang::Stmt::CXXNoexceptExprClass:
    //      ret_status = VisitCXXNoexceptExpr((clang::CXXNoexceptExpr *)stmt, &result);
            break;
        case clang::Stmt::CXXNullPtrLiteralExprClass:
    //      ret_status = VisitCXXNullPtrLiteralExpr((clang::CXXNullPtrLiteralExpr *)stmt, &result);
            break;
        case clang::Stmt::CXXPseudoDestructorExprClass:
    //      ret_status = VisitCXXPseudoDestructorExpr((clang::CXXPseudoDestructorExpr *)stmt, &result);
            break;
        case clang::Stmt::CXXScalarValueInitExprClass:
    //      ret_status = VisitCXXScalarValueInitExpr((clang::CXXScalarValueInitExpr *)stmt, &result);
            break;
        case clang::Stmt::CXXThisExprClass:
    //      ret_status = VisitCXXThisExpr((clang::CXXThisExpr *)stmt, &result);
            break;
        case clang::Stmt::CXXThrowExprClass:
    //      ret_status = VisitCXXThrowExpr((clang::CXXThrowExpr *)stmt, &result);
            break;
        case clang::Stmt::CXXTypeidExprClass:
    //      ret_status = VisitCXXTypeidExpr((clang::CXXTypeidExpr *)stmt, &result);
            break;
        case clang::Stmt::CXXUnresolvedConstructExprClass:
    //      ret_status = VisitCXXUnresolvedConstructExpr((clang::CXXUnresolvedConstructExpr *)stmt, &result);
            break;
        case clang::Stmt::CXXUuidofExprClass:
    //      ret_status = VisitCXXUuidofExpr((clang::CXXUuidofExpr *)stmt, &result);
            break;
        case clang::Stmt::DeclRefExprClass:
            ret_status = VisitDeclRefExpr((clang::DeclRefExpr *)stmt, &result);
            break;
        case clang::Stmt::DependentScopeDeclRefExprClass:
    //      ret_status = VisitDependentScopeDeclRefExpr((clang::DependentScopeDeclRefExpr *)stmt, &result);
            break;
        case clang::Stmt::DesignatedInitExprClass:
            ret_status = VisitDesignatedInitExpr((clang::DesignatedInitExpr *)stmt, &result);
            break;
        case clang::Stmt::ExpressionTraitExprClass:
    //      ret_status = VisitExpressionTraitExpr((clang::ExpressionTraitExpr *)stmt, &result);
            break;
        case clang::Stmt::ExprWithCleanupsClass:
            ret_status = VisitExprWithCleanups((clang::ExprWithCleanups *)stmt, &result);
            break;
        case clang::Stmt::ExtVectorElementExprClass:
            ret_status = VisitExtVectorElementExpr((clang::ExtVectorElementExpr *)stmt, &result);
            break;
        case clang::Stmt::FloatingLiteralClass:
            ret_status = VisitFloatingLiteral((clang::FloatingLiteral *)stmt, &result);
            break;
        case clang::Stmt::GenericSelectionExprClass:
    //      ret_status = VisitGenericSelectionExpr((clang::GenericSelectionExpr *)stmt, &result);
            break;
        case clang::Stmt::GNUNullExprClass:
    //      ret_status = VisitGNUNullExpr((clang::GNUNullExpr *)stmt, &result);
            break;
        case clang::Stmt::ImaginaryLiteralClass:
            ret_status = VisitImaginaryLiteral((clang::ImaginaryLiteral *)stmt, &result);
            break;
        case clang::Stmt::ImplicitValueInitExprClass:
    //      ret_status = VisitImplicitValueInitExpr((clang::ImplicitValueInitExpr *)stmt, &result);
            break;
        case clang::Stmt::InitListExprClass:
            ret_status = VisitInitListExpr((clang::InitListExpr *)stmt, &result);
            break;
        case clang::Stmt::IntegerLiteralClass:
            ret_status = VisitIntegerLiteral((clang::IntegerLiteral *)stmt, &result);
            break;
        case clang::Stmt::MemberExprClass:
            ret_status = VisitMemberExpr((clang::MemberExpr *)stmt, &result);
            break;
        case clang::Stmt::OffsetOfExprClass:
    //      ret_status = VisitOffsetOfExpr((clang::OffsetOfExpr *)stmt, &result);
            break;
        case clang::Stmt::OpaqueValueExprClass:
    //      ret_status = VisitOpaqueValueExpr((clang::OpaqueValueExpr *)stmt, &result);
            break;
        case clang::Stmt::UnresolvedLookupExprClass:
    //      ret_status = VisitUnresolvedLookupExpr((clang::UnresolvedLookupExpr *)stmt, &result);
            break;
        case clang::Stmt::UnresolvedMemberExprClass:
    //      ret_status = VisitUnresolvedMemberExpr((clang::UnresolvedMemberExpr *)stmt, &result);
            break;
        case clang::Stmt::PackExpansionExprClass:
    //      ret_status = VisitPackExpansionExpr((clang::PackExpansionExpr *)stmt, &result);
            break;
        case clang::Stmt::ParenExprClass:
            ret_status = VisitParenExpr((clang::ParenExpr *)stmt, &result);
            break;
        case clang::Stmt::ParenListExprClass:
    //      ret_status = VisitParenListExpr((clang::ParenListExpr *)stmt, &result);
            break;
        case clang::Stmt::PredefinedExprClass:
            ret_status = VisitPredefinedExpr((clang::PredefinedExpr *)stmt, &result);
            break;
        case clang::Stmt::ShuffleVectorExprClass:
    //      ret_status = VisitShuffleVectorExpr((clang::ShuffleVectorExpr *)stmt, &result);
            break;
        case clang::Stmt::SizeOfPackExprClass:
    //      ret_status = VisitSizeOfPackExpr((clang::SizeOfPackExpr *)stmt, &result);
            break;
        case clang::Stmt::StmtExprClass:
            ret_status = VisitStmtExpr((clang::StmtExpr *)stmt, &result);
            break;
        case clang::Stmt::StringLiteralClass:
            ret_status = VisitStringLiteral((clang::StringLiteral *)stmt, &result);
            break;
        case clang::Stmt::SubstNonTypeTemplateParmPackExprClass:
    //      ret_status = VisitSubstNonTypeTemplateParmPackExpr((clang::SubstNonTypeTemplateParmPackExpr *)stmt, &result);
            break;
        case clang::Stmt::UnaryExprOrTypeTraitExprClass:
            ret_status = VisitUnaryExprOrTypeTraitExpr((clang::UnaryExprOrTypeTraitExpr *)stmt, &result);
            break;
        case clang::Stmt::UnaryOperatorClass:
            ret_status = VisitUnaryOperator((clang::UnaryOperator *)stmt, &result);
            break;
//        case clang::Stmt::UnaryTypeTraitExprClass:
//    //      ret_status = VisitUnaryTypeTraitExpr((clang::UnaryTypeTraitExpr *)stmt, &result);
//            break;
        case clang::Stmt::VAArgExprClass:
            ret_status = VisitVAArgExpr((clang::VAArgExpr *)stmt, &result);
            break;
        case clang::Stmt::ForStmtClass:
            ret_status = VisitForStmt((clang::ForStmt *)stmt, &result);
            break;
        case clang::Stmt::GotoStmtClass:
            ret_status = VisitGotoStmt((clang::GotoStmt *)stmt, &result);
            break;
        case clang::Stmt::IfStmtClass:
            ret_status = VisitIfStmt((clang::IfStmt *)stmt, &result);
            break;
        case clang::Stmt::IndirectGotoStmtClass:
    //      ret_status = VisitIndirectGotoStmt((clang::IndirectGotoStmt *)stmt, &result);
            break;
        case clang::Stmt::LabelStmtClass:
            ret_status = VisitLabelStmt((clang::LabelStmt *)stmt, &result);
            break;
        case clang::Stmt::NullStmtClass:
            ret_status = VisitNullStmt((clang::NullStmt *)stmt, &result);
            break;
        case clang::Stmt::ReturnStmtClass:
            ret_status = VisitReturnStmt((clang::ReturnStmt *)stmt, &result);
            break;
        case clang::Stmt::CaseStmtClass:
            ret_status = VisitCaseStmt((clang::CaseStmt *)stmt, &result);
            break;
        case clang::Stmt::DefaultStmtClass:
            ret_status = VisitDefaultStmt((clang::DefaultStmt *)stmt, &result);
            break;
        case clang::Stmt::SwitchStmtClass:
            ret_status = VisitSwitchStmt((clang::SwitchStmt *)stmt, &result);
            break;
        case clang::Stmt::WhileStmtClass:
            ret_status = VisitWhileStmt((clang::WhileStmt *)stmt, &result);
            break;
        default:
            std::cerr << "Unknown statement kind: " << stmt->getStmtClassName() << " !" << std::endl;
            ROSE_ASSERT(false);
    }

    ROSE_ASSERT(result != NULL);

    p_stmt_translation_map.insert(std::pair<clang::Stmt *, SgNode *>(stmt, result));

    return result;
}

/********************/
/* Visit Statements */
/********************/

bool ClangToSageTranslator::VisitStmt(clang::Stmt * stmt, SgNode ** node) {
#if DEBUG_VISIT_STMT
    std::cerr << "ClangToSageTranslator::VisitStmt" << std::endl;
#endif

    if (*node == NULL) {
        std::cerr << "Runtime error: No Sage node associated with the Statement..." << std::endl;
        return false;
    }

    // TODO Is there anything else todo?

    if (
        isSgLocatedNode(*node) != NULL &&
        (
            isSgLocatedNode(*node)->get_file_info() == NULL ||
            !(isSgLocatedNode(*node)->get_file_info()->isCompilerGenerated())
        )
    ) {
        applySourceRange(*node, stmt->getSourceRange());
    }

    return true;
}

// bool ClangToSageTranslator::VisitAsmStmt(clang::AsmStmt * asm_stmt);

bool ClangToSageTranslator::VisitBreakStmt(clang::BreakStmt * break_stmt, SgNode ** node) {
#if DEBUG_VISIT_STMT
    std::cerr << "ClangToSageTranslator::VisitBreakStmt" << std::endl;
#endif

    *node = SageBuilder::buildBreakStmt();
    return VisitStmt(break_stmt, node);
}

bool ClangToSageTranslator::VisitCompoundStmt(clang::CompoundStmt * compound_stmt, SgNode ** node) {
#if DEBUG_VISIT_STMT
    std::cerr << "ClangToSageTranslator::VisitCompoundStmt" << std::endl;
#endif

    bool res = true;

    SgBasicBlock * block = SageBuilder::buildBasicBlock();

    block->set_parent(SageBuilder::topScopeStack());

    SageBuilder::pushScopeStack(block);

    clang::CompoundStmt::body_iterator it;
    for (it = compound_stmt->body_begin(); it != compound_stmt->body_end(); it++) {
        SgNode * tmp_node = Traverse(*it);

#if DEBUG_VISIT_STMT
        if (tmp_node != NULL)
          std::cerr << "In VisitCompoundStmt : child is " << tmp_node->class_name() << std::endl;
        else
          std::cerr << "In VisitCompoundStmt : child is NULL" << std::endl;
#endif

        SgClassDeclaration * class_decl = isSgClassDeclaration(tmp_node);
        if (class_decl != NULL && (class_decl->get_name() == "" || class_decl->get_isUnNamed())) continue;
        SgEnumDeclaration * enum_decl = isSgEnumDeclaration(tmp_node);
        if (enum_decl != NULL && (enum_decl->get_name() == "" || enum_decl->get_isUnNamed())) continue;
#if DEBUG_VISIT_STMT
        else if (enum_decl != NULL)
          std::cerr << "enum_decl = " << enum_decl << " >> name: " << enum_decl->get_name() << std::endl;
#endif

        SgStatement * stmt  = isSgStatement(tmp_node);
        SgExpression * expr = isSgExpression(tmp_node);
        if (tmp_node != NULL && stmt == NULL && expr == NULL) {
            std::cerr << "Runtime error: tmp_node != NULL && stmt == NULL && expr == NULL" << std::endl;
            res = false;
        }
        else if (stmt != NULL) {
            block->append_statement(stmt);
        }
        else if (expr != NULL) {
            SgExprStatement * expr_stmt = SageBuilder::buildExprStatement(expr);
            block->append_statement(expr_stmt);
        }
    }

    SageBuilder::popScopeStack();

    *node = block;

    return VisitStmt(compound_stmt, node) && res;
}

bool ClangToSageTranslator::VisitContinueStmt(clang::ContinueStmt * continue_stmt, SgNode ** node) {
#if DEBUG_VISIT_STMT
    std::cerr << "ClangToSageTranslator::VisitContinueStmt" << std::endl;
#endif

    *node = SageBuilder::buildContinueStmt();
    return VisitStmt(continue_stmt, node);
}

// bool ClangToSageTranslator::VisitCXXCatchStmt(clang::CXXCatchStmt * cxx_catch_stmt);
// bool ClangToSageTranslator::VisitCXXForRangeStmt(clang::CXXForRangeStmt * cxx_for_range_stmt);
// bool ClangToSageTranslator::VisitCXXTryStmt(clang::CXXTryStmt * cxx_try_stmt);

bool ClangToSageTranslator::VisitDeclStmt(clang::DeclStmt * decl_stmt, SgNode ** node) {
#if DEBUG_VISIT_STMT
    std::cerr << "ClangToSageTranslator::VisitDeclStmt" << std::endl;
#endif

    bool res = true;

    if (decl_stmt->isSingleDecl()) {
        *node = Traverse(decl_stmt->getSingleDecl());
    }
    else {
        std::vector<SgNode *> tmp_decls;
        SgDeclarationStatement * decl;
        clang::DeclStmt::decl_iterator it;

        SgScopeStatement * scope = SageBuilder::topScopeStack();

        for (it = decl_stmt->decl_begin(); it != decl_stmt->decl_end(); it++)
            tmp_decls.push_back(Traverse(*it));
        for (unsigned i = 0; i < tmp_decls.size() - 1; i++) {
            decl = isSgDeclarationStatement(tmp_decls[i]);
            if (tmp_decls[i] != NULL && decl == NULL) {
                std::cerr << "Runtime error: tmp_decls[i] != NULL && decl == NULL" << std::endl;
                res = false;
                continue;
            }
            else {
              SgClassDeclaration * class_decl = isSgClassDeclaration(decl);
              if (class_decl != NULL && (class_decl->get_name() == "" || class_decl->get_isUnNamed())) continue;
              SgEnumDeclaration * enum_decl = isSgEnumDeclaration(decl);
              if (enum_decl != NULL && (enum_decl->get_name() == "" || enum_decl->get_isUnNamed())) continue;
            }
            scope->append_statement(decl);
            decl->set_parent(scope);
        }
        decl = isSgDeclarationStatement(tmp_decls[tmp_decls.size() - 1]);
        if (tmp_decls[tmp_decls.size() - 1] != NULL && decl == NULL) {
            std::cerr << "Runtime error: tmp_decls[tmp_decls.size() - 1] != NULL && decl == NULL" << std::endl;
            res = false;
        }
        *node = decl;
    }

    return res;
}

bool ClangToSageTranslator::VisitDoStmt(clang::DoStmt * do_stmt, SgNode ** node) {
#if DEBUG_VISIT_STMT
    std::cerr << "ClangToSageTranslator::VisitDoStmt" << std::endl;
#endif

    SgNode * tmp_cond = Traverse(do_stmt->getCond());
    SgExpression * cond = isSgExpression(tmp_cond);
    ROSE_ASSERT(cond != NULL);

    SgStatement * expr_stmt = SageBuilder::buildExprStatement(cond);

    ROSE_ASSERT(expr_stmt != NULL);

    SgDoWhileStmt * sg_do_stmt = SageBuilder::buildDoWhileStmt_nfi(expr_stmt, NULL);

    sg_do_stmt->set_condition(expr_stmt);

    cond->set_parent(expr_stmt);
    expr_stmt->set_parent(sg_do_stmt);

    SageBuilder::pushScopeStack(sg_do_stmt);

    SgNode * tmp_body = Traverse(do_stmt->getBody());
    SgStatement * body = isSgStatement(tmp_body);
    SgExpression * expr = isSgExpression(tmp_body);
    if (expr != NULL) {
        body =  SageBuilder::buildExprStatement(expr);
        applySourceRange(body, do_stmt->getBody()->getSourceRange());
    }
    ROSE_ASSERT(body != NULL);

    body->set_parent(sg_do_stmt);

    SageBuilder::popScopeStack();

    sg_do_stmt->set_body(body);

    *node = sg_do_stmt;

    return VisitStmt(do_stmt, node); 
}

bool ClangToSageTranslator::VisitExpr(clang::Expr * expr, SgNode ** node) {
#if DEBUG_VISIT_STMT
    std::cerr << "ClangToSageTranslator::VisitExpr" << std::endl;
#endif

     // TODO Is there anything to be done? (maybe in relation with typing?)

     return VisitStmt(expr, node);
}

bool ClangToSageTranslator::VisitAbstractConditionalOperator(clang::AbstractConditionalOperator * abstract_conditional_operator, SgNode ** node) {
#if DEBUG_VISIT_STMT
    std::cerr << "ClangToSageTranslator::VisitAbstractConditionalOperator" << std::endl;
#endif
     bool res = true;

     // TODO 

     return VisitStmt(abstract_conditional_operator, node) && res;
}

// bool ClangToSageTranslator::VisitBinaryConditionalOperator(clang::BinaryConditionalOperator * binary_conditionnal_operator);

bool ClangToSageTranslator::VisitConditionalOperator(clang::ConditionalOperator * conditional_operator, SgNode ** node) {
#if DEBUG_VISIT_STMT
    std::cerr << "ClangToSageTranslator::VisitConditionalOperator" << std::endl;
#endif

    bool res = true;

    SgNode * tmp_cond  = Traverse(conditional_operator->getCond());
    SgExpression * cond_expr = isSgExpression(tmp_cond);
    ROSE_ASSERT(cond_expr);
    SgNode * tmp_true  = Traverse(conditional_operator->getTrueExpr());
    SgExpression * true_expr = isSgExpression(tmp_true);
    ROSE_ASSERT(true_expr);
    SgNode * tmp_false = Traverse(conditional_operator->getFalseExpr());
    SgExpression * false_expr = isSgExpression(tmp_false);
    ROSE_ASSERT(false_expr);

    *node = SageBuilder::buildConditionalExp(cond_expr, true_expr, false_expr);

    return VisitAbstractConditionalOperator(conditional_operator, node) && res;
}

bool ClangToSageTranslator::VisitArraySubscriptExpr(clang::ArraySubscriptExpr * array_subscript_expr, SgNode ** node) {
#if DEBUG_VISIT_STMT
    std::cerr << "ClangToSageTranslator::VisitArraySubscriptExpr" << std::endl;
#endif

    bool res = true;

    SgNode * tmp_base = Traverse(array_subscript_expr->getBase());
    SgExpression * base = isSgExpression(tmp_base);
    if (tmp_base != NULL && base == NULL) {
        std::cerr << "Runtime error: tmp_base != NULL && base == NULL" << std::endl;
        res = false;
    }

    SgNode * tmp_idx = Traverse(array_subscript_expr->getIdx());
    SgExpression * idx = isSgExpression(tmp_idx);
    if (tmp_idx != NULL && idx == NULL) {
        std::cerr << "Runtime error: tmp_idx != NULL && idx == NULL" << std::endl;
        res = false;
    }

    *node = SageBuilder::buildPntrArrRefExp(base, idx);

    return VisitExpr(array_subscript_expr, node) && res;
}

// bool ClangToSageTranslator::VisitArrayTypeTraitExpr(clang::ArrayTypeTraitExpr * array_type_trait_expr);
// bool ClangToSageTranslator::VisitAsTypeExpr(clang::AsTypeExpr * as_type_expr);

bool ClangToSageTranslator::VisitBinaryOperator(clang::BinaryOperator * binary_operator, SgNode ** node) {
#if DEBUG_VISIT_STMT
    std::cerr << "ClangToSageTranslator::VisitBinaryOperator" << std::endl;
#endif

    bool res = true;

    SgNode * tmp_lhs = Traverse(binary_operator->getLHS());
    SgExpression * lhs = isSgExpression(tmp_lhs);
    if (tmp_lhs != NULL && lhs == NULL) {
        std::cerr << "Runtime error: tmp_lhs != NULL && lhs == NULL" << std::endl;
        res = false;
    }

    SgNode * tmp_rhs = Traverse(binary_operator->getRHS());
    SgExpression * rhs = isSgExpression(tmp_rhs);
    if (tmp_rhs != NULL && rhs == NULL) {
        std::cerr << "Runtime error: tmp_rhs != NULL && rhs == NULL" << std::endl;
        res = false;
    }

    switch (binary_operator->getOpcode()) {
        case clang::BO_PtrMemD:   ROSE_ASSERT(!"clang::BO_PtrMemD:");//*node = SageBuilder::build(lhs, rhs); break;
        case clang::BO_PtrMemI:   ROSE_ASSERT(!"clang::BO_PtrMemI:");//*node = SageBuilder::build(lhs, rhs); break;
        case clang::BO_Mul:       *node = SageBuilder::buildMultiplyOp(lhs, rhs); break;
        case clang::BO_Div:       *node = SageBuilder::buildDivideOp(lhs, rhs); break;
        case clang::BO_Rem:       *node = SageBuilder::buildModOp(lhs, rhs); break;
        case clang::BO_Add:       *node = SageBuilder::buildAddOp(lhs, rhs); break;
        case clang::BO_Sub:       *node = SageBuilder::buildSubtractOp(lhs, rhs); break;
        case clang::BO_Shl:       *node = SageBuilder::buildLshiftOp(lhs, rhs); break;
        case clang::BO_Shr:       *node = SageBuilder::buildRshiftOp(lhs, rhs); break;
        case clang::BO_LT:        *node = SageBuilder::buildLessThanOp(lhs, rhs); break;
        case clang::BO_GT:        *node = SageBuilder::buildGreaterThanOp(lhs, rhs); break;
        case clang::BO_LE:        *node = SageBuilder::buildLessOrEqualOp(lhs, rhs); break;
        case clang::BO_GE:        *node = SageBuilder::buildGreaterOrEqualOp(lhs, rhs); break;
        case clang::BO_EQ:        *node = SageBuilder::buildEqualityOp(lhs, rhs); break;
        case clang::BO_NE:        *node = SageBuilder::buildNotEqualOp(lhs, rhs); break;
        case clang::BO_And:       *node = SageBuilder::buildBitAndOp(lhs, rhs); break;
        case clang::BO_Xor:       *node = SageBuilder::buildBitXorOp(lhs, rhs); break;
        case clang::BO_Or:        *node = SageBuilder::buildBitOrOp(lhs, rhs); break;
        case clang::BO_LAnd:      *node = SageBuilder::buildAndOp(lhs, rhs); break;
        case clang::BO_LOr:       *node = SageBuilder::buildOrOp(lhs, rhs); break;
        case clang::BO_Assign:    *node = SageBuilder::buildAssignOp(lhs, rhs); break;
        case clang::BO_MulAssign: *node = SageBuilder::buildMultAssignOp(lhs, rhs); break;
        case clang::BO_DivAssign: *node = SageBuilder::buildDivAssignOp(lhs, rhs); break;
        case clang::BO_RemAssign: *node = SageBuilder::buildModAssignOp(lhs, rhs); break;
        case clang::BO_AddAssign: *node = SageBuilder::buildPlusAssignOp(lhs, rhs); break;
        case clang::BO_SubAssign: *node = SageBuilder::buildMinusAssignOp(lhs, rhs); break;
        case clang::BO_ShlAssign: *node = SageBuilder::buildLshiftAssignOp(lhs, rhs); break;
        case clang::BO_ShrAssign: *node = SageBuilder::buildRshiftAssignOp(lhs, rhs); break;
        case clang::BO_AndAssign: *node = SageBuilder::buildAndAssignOp(lhs, rhs); break;
        case clang::BO_XorAssign: *node = SageBuilder::buildXorAssignOp(lhs, rhs); break;
        case clang::BO_OrAssign:  *node = SageBuilder::buildIorAssignOp(lhs, rhs); break;
        case clang::BO_Comma:     *node = SageBuilder::buildCommaOpExp(lhs, rhs); break;
        default:
            std::cerr << "Unknown opcode for binary operator: " << binary_operator->getOpcodeStr().str() << std::endl;
            res = false;
    }

    return VisitExpr(binary_operator, node) && res;
}

// bool ClangToSageTranslator::VisitCompoundAssignOperator(clang::CompoundAssignOperator * compound_assign_operator, SgNode ** node);
// bool ClangToSageTranslator::VisitBinaryTypeTraitExpr(clang::BinaryTypeTraitExpr * binary_type_trait_expr);
// bool ClangToSageTranslator::VisitBlockDeclRefExpr(clang::BlockDeclRefExpr * block_decl_ref_expr);
// bool ClangToSageTranslator::VisitBlockExpr(clang::BlockExpr * block_expr);

bool ClangToSageTranslator::VisitCallExpr(clang::CallExpr * call_expr, SgNode ** node) {
#if DEBUG_VISIT_STMT
    std::cerr << "ClangToSageTranslator::VisitCallExpr" << std::endl;
#endif

    bool res = true;

    SgNode * tmp_expr = Traverse(call_expr->getCallee());
    SgExpression * expr = isSgExpression(tmp_expr);
    if (tmp_expr != NULL && expr == NULL) {
        std::cerr << "Runtime error: tmp_expr != NULL && expr == NULLL" << std::endl;
        res = false;
    }

    SgExprListExp * param_list = SageBuilder::buildExprListExp_nfi();
        applySourceRange(param_list, call_expr->getSourceRange());

    clang::CallExpr::arg_iterator it;
    for (it = call_expr->arg_begin(); it != call_expr->arg_end(); ++it) {
        SgNode * tmp_expr = Traverse(*it);
        SgExpression * expr = isSgExpression(tmp_expr);
        if (tmp_expr != NULL && expr == NULL) {
            std::cerr << "Runtime error: tmp_expr != NULL && expr == NULL" << std::endl;
            res = false;
            continue;
        }
        param_list->append_expression(expr);
    }

    *node = SageBuilder::buildFunctionCallExp_nfi(expr, param_list);

    return VisitExpr(call_expr, node) && res;
}

// bool ClangToSageTranslator::VisitCUDAKernelCallExpr(clang::CUDAKernelCallExpr * cuda_kernel_call_expr);
// bool ClangToSageTranslator::VisitCXXMemberCallExpr(clang::CXXMemberCallExpr * cxx_member_call_expr);
// bool ClangToSageTranslator::VisitCXXOperatorCallExpr(clang::CXXOperatorCallExpr * cxx_operator_call_expr);

bool ClangToSageTranslator::VisitCastExpr(clang::CastExpr * cast_expr, SgNode ** node) {
#if DEBUG_VISIT_STMT
    std::cerr << "ClangToSageTranslator::VisitCastExpr" << std::endl;
#endif
    bool res = true;

    // TODO

    return VisitExpr(cast_expr, node) && res;
}

bool ClangToSageTranslator::VisitExplicitCastExpr(clang::ExplicitCastExpr * explicit_cast_expr, SgNode ** node) {
#if DEBUG_VISIT_STMT
    std::cerr << "ClangToSageTranslator::VisitExplicitCastExpr" << std::endl;
#endif
    bool res = true;

    // TODO

    return VisitCastExpr(explicit_cast_expr, node) && res;
}
    
bool ClangToSageTranslator::VisitCStyleCastExpr(clang::CStyleCastExpr * c_style_cast, SgNode ** node) {
#if DEBUG_VISIT_STMT
    std::cerr << "ClangToSageTranslator::VisitCStyleCastExpr" << std::endl;
#endif

    bool res = true;

    SgNode * tmp_expr = Traverse(c_style_cast->getSubExpr());
    SgExpression * expr = isSgExpression(tmp_expr);

    ROSE_ASSERT(expr);

    SgType * type = buildTypeFromQualifiedType(c_style_cast->getTypeAsWritten());

    *node = SageBuilder::buildCastExp(expr, type, SgCastExp::e_C_style_cast);

    return VisitExplicitCastExpr(c_style_cast, node) && res;
}

// bool ClangToSageTranslator::VisitCXXFunctionalCastExpr(clang::CXXFunctionalCastExpr * cxx_functional_cast_expr);
// bool ClangToSageTranslator::VisitCXXNamedCastExpr(clang::CXXNamedCastExpr * cxx_named_cast_expr);
// bool ClangToSageTranslator::VisitCXXConstCastExpr(clang::CXXConstCastExpr * cxx_const_cast_expr);
// bool ClangToSageTranslator::VisitCXXDynamicCastExpr(clang::CXXDynamicCastExpr * cxx_dynamic_cast_expr);
// bool ClangToSageTranslator::VisitCXXReinterpretCastExpr(clang::CXXReinterpretCastExpr * cxx_reinterpret_cast_expr);
// bool ClangToSageTranslator::VisitCXXStaticCastExpr(clang::CXXStaticCastExpr * cxx_static_cast_expr);

bool ClangToSageTranslator::VisitImplicitCastExpr(clang::ImplicitCastExpr * implicit_cast_expr, SgNode ** node) {
#if DEBUG_VISIT_STMT
    std::cerr << "ClangToSageTranslator::VisitImplicitCastExpr" << std::endl;
#endif

    SgNode * tmp_expr = Traverse(implicit_cast_expr->getSubExpr());
    SgExpression * expr = isSgExpression(tmp_expr);
    
    ROSE_ASSERT(expr != NULL);
/*
    FIXME why not? I dont remember why i commented it... :)

    SgType * type = buildTypeFromQualifiedType(implicit_cast_expr->getType());
    SgCastExp * res = SageBuilder::buildCastExp(expr, type);
    setCompilerGeneratedFileInfo(res);

    *node = res;
*/

    *node = expr;

    return VisitCastExpr(implicit_cast_expr, node);
}

bool ClangToSageTranslator::VisitCharacterLiteral(clang::CharacterLiteral * character_literal, SgNode ** node) {
#if DEBUG_VISIT_STMT
    std::cerr << "ClangToSageTranslator::VisitCharacterLiteral" << std::endl;
#endif

    *node = SageBuilder::buildCharVal(character_literal->getValue());

    return VisitExpr(character_literal, node);
}

// bool ClangToSageTranslator::VisitChooseExpr(clang::ChooseExpr * choose_expr);

bool ClangToSageTranslator::VisitCompoundLiteralExpr(clang::CompoundLiteralExpr * compound_literal, SgNode ** node) {
#if DEBUG_VISIT_STMT
    std::cerr << "ClangToSageTranslator::VisitCompoundLiteralExpr" << std::endl;
#endif

    SgNode * tmp_node = Traverse(compound_literal->getInitializer());
    SgExprListExp * expr = isSgExprListExp(tmp_node);

    ROSE_ASSERT(expr != NULL);

    SgType * type = buildTypeFromQualifiedType(compound_literal->getType());

    ROSE_ASSERT(type != NULL);

    *node = SageBuilder::buildCompoundInitializer_nfi(expr, type);

    return VisitExpr(compound_literal, node);
}

// bool ClangToSageTranslator::VisitCXXBindTemporaryExpr(clang::CXXBindTemporaryExpr * cxx_bind_temporary_expr);
// bool ClangToSageTranslator::VisitCXXBoolLiteralExpr(clang::CXXBoolLiteralExpr * cxx_bool_literal_expr);
// bool ClangToSageTranslator::VisitCXXConstructExpr(clang::CXXConstructExpr * cxx_constructor_expr);
// bool ClangToSageTranslator::VisitCXXTemporaryObjectExpr(clang::CXXTemporaryObjectExpr * cxx_temporary_object_expr);
// bool ClangToSageTranslator::VisitCXXDefaultArgExpr(clang::CXXDefaultArgExpr * cxx_default_arg_expr);
// bool ClangToSageTranslator::VisitCXXDeleteExpr(clang::CXXDeleteExpr * cxx_delete_expr);
// bool ClangToSageTranslator::VisitCXXDependentScopeMemberExpr(clang::CXXDependentScopeMemberExpr * cxx_dependent_scope_member_expr);
// bool ClangToSageTranslator::VisitCXXNewExpr(clang::CXXNewExpr * cxx_new_expr);
// bool ClangToSageTranslator::VisitCXXNoexceptExpr(clang::CXXNoexceptExpr * cxx_noexecept_expr);
// bool ClangToSageTranslator::VisitCXXNullPtrLiteralExpr(clang::CXXNullPtrLiteralExpr * cxx_null_ptr_literal_expr);
// bool ClangToSageTranslator::VisitCXXPseudoDestructorExpr(clang::CXXPseudoDestructorExpr * cxx_pseudo_destructor_expr);
// bool ClangToSageTranslator::VisitCXXScalarValueInitExpr(clang::CXXScalarValueInitExpr * cxx_scalar_value_init_expr);
// bool ClangToSageTranslator::VisitCXXThisExpr(clang::CXXThisExpr * cxx_this_expr);
// bool ClangToSageTranslator::VisitCXXThrowExpr(clang::CXXThrowExpr * cxx_throw_expr);
// bool ClangToSageTranslator::VisitCXXTypeidExpr(clang::CXXTypeidExpr * cxx_typeid_expr);
// bool ClangToSageTranslator::VisitCXXUnresolvedConstructExpr(clang::CXXUnresolvedConstructExpr * cxx_unresolved_construct_expr);
// bool ClangToSageTranslator::VisitCXXUuidofExpr(clang::CXXUuidofExpr * cxx_uuidof_expr);

bool ClangToSageTranslator::VisitDeclRefExpr(clang::DeclRefExpr * decl_ref_expr, SgNode ** node) {
#if DEBUG_VISIT_STMT
    std::cerr << "ClangToSageTranslator::VisitDeclRefExpr" << std::endl;
#endif

    bool res = true;

    //SgNode * tmp_node = Traverse(decl_ref_expr->getDecl());
    // DONE: Do not use Traverse(...) as the declaration can not be complete (recursive functions)
    //       Instead use SymbolTable from ROSE as the symbol should be ready (cannot have a reference before the declaration)
    // FIXME: This fix will not work for C++ (methods/fields can be use before they are declared...)
    // FIXME: I feel like it could work now, we will see ....

    SgSymbol * sym = GetSymbolFromSymbolTable(decl_ref_expr->getDecl());

    if (sym == NULL) {
        SgNode * tmp_decl = Traverse(decl_ref_expr->getDecl());

        if (tmp_decl != NULL) {
            sym = GetSymbolFromSymbolTable(decl_ref_expr->getDecl());
        }
        // FIXME hack Traverse have added the symbol but we cannot find it (probably: problem with type and function lookup)
        if (sym == NULL && isSgFunctionDeclaration(tmp_decl) != NULL) {
            sym = new SgFunctionSymbol(isSgFunctionDeclaration(tmp_decl));
            sym->set_parent(tmp_decl);
        }
        
    }

    if (sym != NULL) { // Not else if it was NULL we have try to traverse it....
        SgVariableSymbol  * var_sym  = isSgVariableSymbol(sym);
        SgFunctionSymbol  * func_sym = isSgFunctionSymbol(sym);
        SgEnumFieldSymbol * enum_sym = isSgEnumFieldSymbol(sym);

        if (var_sym != NULL) {
            *node = SageBuilder::buildVarRefExp(var_sym);
        }
        else if (func_sym != NULL) {
            *node = SageBuilder::buildFunctionRefExp(func_sym);
        }
        else if (enum_sym != NULL) {
            SgEnumDeclaration * enum_decl = isSgEnumDeclaration(enum_sym->get_declaration()->get_parent());
            ROSE_ASSERT(enum_decl != NULL);
            SgName name = enum_sym->get_name();
            *node = SageBuilder::buildEnumVal_nfi(0, enum_decl, name);
        }
        else if (sym != NULL) {
            std::cerr << "Runtime error: Unknown type of symbol for a declaration reference." << std::endl;
            std::cerr << "    sym->class_name() = " << sym->class_name()  << std::endl;
            ROSE_ASSERT(false);
        }
    }
    else {
         std::cerr << "Runtime error: Cannot find the symbol for a declaration reference (even after trying to buil th declaration)" << std::endl;
         ROSE_ASSERT(false);
    }

    return VisitExpr(decl_ref_expr, node) && res;
}

// bool ClangToSageTranslator::VisitDependentScopeDeclRefExpr(clang::DependentScopeDeclRefExpr * dependent_scope_decl_ref_expr);

bool ClangToSageTranslator::VisitDesignatedInitExpr(clang::DesignatedInitExpr * designated_init_expr, SgNode ** node) {
#if DEBUG_VISIT_STMT
    std::cerr << "ClangToSageTranslator::VisitDesignatedInitExpr" << std::endl;
#endif

    SgInitializer * init = NULL;    
    {
        SgNode * tmp_expr = Traverse(designated_init_expr->getInit());
        SgExpression * expr = isSgExpression(tmp_expr);
        ROSE_ASSERT(expr != NULL);
        SgExprListExp * expr_list_exp = isSgExprListExp(expr);
        if (expr_list_exp != NULL) {
            // FIXME get the type right...
            init = SageBuilder::buildAggregateInitializer_nfi(expr_list_exp, NULL);
        }
        else {
            init = SageBuilder::buildAssignInitializer_nfi(expr, expr->get_type());
        }
        ROSE_ASSERT(init != NULL);
        applySourceRange(init, designated_init_expr->getInit()->getSourceRange());
    }

    SgExprListExp * expr_list_exp = SageBuilder::buildExprListExp_nfi();
    auto designatorSize = designated_init_expr->size();
    for (auto it=0; it < designatorSize; it++) {
        SgExpression * expr = NULL;
	clang::DesignatedInitExpr::Designator * D = designated_init_expr->getDesignator(it);
        if (D->isFieldDesignator()) {
            SgSymbol * symbol = GetSymbolFromSymbolTable(D->getField());
            SgVariableSymbol * var_sym = isSgVariableSymbol(symbol);
            ROSE_ASSERT(var_sym != NULL);
            expr = SageBuilder::buildVarRefExp_nfi(var_sym);
            applySourceRange(expr, D->getSourceRange());
        }
        else if (D->isArrayDesignator()) {
            SgNode * tmp_expr = Traverse(designated_init_expr->getArrayIndex(*D));
            expr = isSgExpression(tmp_expr);
            ROSE_ASSERT(expr != NULL);
        }
        else if (D->isArrayRangeDesignator()) {
            ROSE_ASSERT(!"I don't believe range designator initializer are supported by ROSE...");    
        }
        else ROSE_ASSERT(false);

        ROSE_ASSERT(expr != NULL);

        expr->set_parent(expr_list_exp);
        expr_list_exp->append_expression(expr);
    }

    applySourceRange(expr_list_exp, designated_init_expr->getDesignatorsSourceRange());

    SgDesignatedInitializer * design_init = new SgDesignatedInitializer(expr_list_exp, init);
    expr_list_exp->set_parent(design_init);
    init->set_parent(design_init);

    *node = design_init;

    return VisitExpr(designated_init_expr, node);
}

// bool ClangToSageTranslator::VisitExpressionTraitExpr(clang::ExpressionTraitExpr * expression_trait_expr);

bool ClangToSageTranslator::VisitExprWithCleanups(clang::ExprWithCleanups * expr_with_cleanups, SgNode ** node) {
#if DEBUG_VISIT_STMT
    std::cerr << "ClangToSageTranslator::VisitExprWithCleanups" << std::endl;
#endif
    bool res = true;

    // TODO

    return VisitExpr(expr_with_cleanups, node) && res;
}

bool ClangToSageTranslator::VisitExtVectorElementExpr(clang::ExtVectorElementExpr * ext_vector_element_expr, SgNode ** node) {
#if DEBUG_VISIT_STMT
    std::cerr << "ClangToSageTranslator::VisitExtVectorElementExpr" << std::endl;
#endif

    SgNode * tmp_base = Traverse(ext_vector_element_expr->getBase());
    SgExpression * base = isSgExpression(tmp_base);

    ROSE_ASSERT(base != NULL);

    SgType * type = buildTypeFromQualifiedType(ext_vector_element_expr->getType());

    clang::IdentifierInfo & ident_info = ext_vector_element_expr->getAccessor();
    std::string ident = ident_info.getName().str();

    SgScopeStatement * scope = SageBuilder::ScopeStack.front();
    SgGlobal * global = isSgGlobal(scope);
    ROSE_ASSERT(global != NULL);

  // Build Manually a SgVarRefExp to have the same Accessor (text version) TODO ExtVectorAccessor and ExtVectorType
    SgInitializedName * init_name = SageBuilder::buildInitializedName(ident, SageBuilder::buildVoidType(), NULL);
    setCompilerGeneratedFileInfo(init_name);
    init_name->set_scope(global);
    SgVariableSymbol * var_symbol = new SgVariableSymbol(init_name);
    SgVarRefExp * pseudo_field = new SgVarRefExp(var_symbol);
    setCompilerGeneratedFileInfo(pseudo_field, true);
    init_name->set_parent(pseudo_field);

    SgExpression * res = NULL;
    if (ext_vector_element_expr->isArrow())
        res = SageBuilder::buildArrowExp(base, pseudo_field);
    else
        res = SageBuilder::buildDotExp(base, pseudo_field);

    ROSE_ASSERT(res != NULL);

    *node = res;

   return VisitExpr(ext_vector_element_expr, node);
}

bool ClangToSageTranslator::VisitFloatingLiteral(clang::FloatingLiteral * floating_literal, SgNode ** node) {
#if DEBUG_VISIT_STMT
    std::cerr << "ClangToSageTranslator::VisitFloatingLiteral" << std::endl;
#endif

    unsigned int precision =  llvm::APFloat::semanticsPrecision(floating_literal->getValue().getSemantics());
    if (precision == 24)
        *node = SageBuilder::buildFloatVal(floating_literal->getValue().convertToFloat());
    else if (precision == 53)
        *node = SageBuilder::buildDoubleVal(floating_literal->getValue().convertToDouble());
    else
        ROSE_ASSERT(!"In VisitFloatingLiteral: Unsupported float size");

    return VisitExpr(floating_literal, node);
}

// bool ClangToSageTranslator::VisitGenericSelectionExpr(clang::GenericSelectionExpr * generic_selection_expr);
// bool ClangToSageTranslator::VisitGNUNullExpr(clang::GNUNullExpr * gnu_null_expr);

bool ClangToSageTranslator::VisitImaginaryLiteral(clang::ImaginaryLiteral * imaginary_literal, SgNode ** node) {
#if DEBUG_VISIT_STMT
    std::cerr << "ClangToSageTranslator::VisitImaginaryLiteral" << std::endl;
#endif

    SgNode * tmp_imag_val = Traverse(imaginary_literal->getSubExpr());
    SgValueExp * imag_val = isSgValueExp(tmp_imag_val);
    ROSE_ASSERT(imag_val != NULL);

    SgComplexVal * comp_val = new SgComplexVal(NULL, imag_val, imag_val->get_type(), "");

    *node = comp_val;

    return VisitExpr(imaginary_literal, node);
}

// bool ClangToSageTranslator::VisitImplicitValueInitExpr(clang::ImplicitValueInitExpr * implicit_value_init_expr);

bool ClangToSageTranslator::VisitInitListExpr(clang::InitListExpr * init_list_expr, SgNode ** node) {
#if DEBUG_VISIT_STMT
    std::cerr << "ClangToSageTranslator::VisitInitListExpr" << std::endl;
#endif

    // We use the syntactic version of the initializer if it exists
    if (init_list_expr->getSyntacticForm() != NULL) return VisitInitListExpr(init_list_expr->getSyntacticForm(), node);

    SgExprListExp * expr_list_expr = SageBuilder::buildExprListExp_nfi();

    clang::InitListExpr::iterator it;
    for (it = init_list_expr->begin(); it != init_list_expr->end(); it++) {
        SgNode * tmp_expr = Traverse(*it);
        SgExpression * expr = isSgExpression(tmp_expr);
        ROSE_ASSERT(expr != NULL);
        expr_list_expr->append_expression(expr);
    }

    *node = expr_list_expr;

    return VisitExpr(init_list_expr, node);
}

bool ClangToSageTranslator::VisitIntegerLiteral(clang::IntegerLiteral * integer_literal, SgNode ** node) {
#if DEBUG_VISIT_STMT
    std::cerr << "ClangToSageTranslator::VisitIntegerLiteral" << std::endl;
#endif

    *node = SageBuilder::buildIntVal(integer_literal->getValue().getSExtValue());

    return VisitExpr(integer_literal, node);
}

bool ClangToSageTranslator::VisitMemberExpr(clang::MemberExpr * member_expr, SgNode ** node) {
#if DEBUG_VISIT_STMT
    std::cerr << "ClangToSageTranslator::VisitMemberExpr" << std::endl;
#endif

    bool res = true;

    SgNode * tmp_base = Traverse(member_expr->getBase());
    SgExpression * base = isSgExpression(tmp_base);
    ROSE_ASSERT(base != NULL);

    SgSymbol * sym = GetSymbolFromSymbolTable(member_expr->getMemberDecl());

    SgVariableSymbol * var_sym  = isSgVariableSymbol(sym);
    SgMemberFunctionSymbol * func_sym = isSgMemberFunctionSymbol(sym);

    SgExpression * sg_member_expr = NULL;

    bool successful_cast = var_sym || func_sym;
    if (sym != NULL && !successful_cast) {
        std::cerr << "Runtime error: Unknown type of symbol for a member reference." << std::endl;
        std::cerr << "    sym->class_name() = " << sym->class_name()  << std::endl;
        res = false;
    }
    else if (var_sym != NULL) {
        sg_member_expr = SageBuilder::buildVarRefExp(var_sym);
    }
    else if (func_sym != NULL) { // C++
        sg_member_expr = SageBuilder::buildMemberFunctionRefExp_nfi(func_sym, false, false); // FIXME 2nd and 3rd params ?
    }

    ROSE_ASSERT(sg_member_expr != NULL);

    // TODO (C++) member_expr->getQualifier() : for 'a->Base::foo'

    if (member_expr->isArrow())
        *node = SageBuilder::buildArrowExp(base, sg_member_expr);
    else
        *node = SageBuilder::buildDotExp(base, sg_member_expr);

    return VisitExpr(member_expr, node) && res;
}

// bool ClangToSageTranslator::VisitOffsetOfExpr(clang::OffsetOfExpr * offset_of_expr);
// bool ClangToSageTranslator::VisitOpaqueValueExpr(clang::OpaqueValueExpr * opaque_value_expr);
// bool ClangToSageTranslator::VisitOverloadExpr(clang::OverloadExpr * overload_expr);
// bool ClangToSageTranslator::VisitUnresolvedLookupExpr(clang::UnresolvedLookupExpr * unresolved_lookup_expr);
// bool ClangToSageTranslator::VisitUnresolvedMemberExpr(clang::UnresolvedMemberExpr * unresolved_member_expr);
// bool ClangToSageTranslator::VisitPackExpansionExpr(clang::PackExpansionExpr * pack_expansion_expr);

bool ClangToSageTranslator::VisitParenExpr(clang::ParenExpr * paren_expr, SgNode ** node) {
#if DEBUG_VISIT_STMT
    std::cerr << "ClangToSageTranslator::VisitParenExpr" << std::endl;
#endif

    bool res = true;

    SgNode * tmp_subexpr = Traverse(paren_expr->getSubExpr());
    SgExpression * subexpr = isSgExpression(tmp_subexpr);
    if (tmp_subexpr != NULL && subexpr == NULL) {
        std::cerr << "Runtime error: tmp_subexpr != NULL && subexpr == NULL" << std::endl;
        res = false;
    }

    // bypass ParenExpr, their is nothing equivalent in SageIII
    *node = subexpr;

    return VisitExpr(paren_expr, node) && res;
}

// bool ClangToSageTranslator::VisitParenListExpr(clang::ParenListExpr * paren_list_expr);

bool ClangToSageTranslator::VisitPredefinedExpr(clang::PredefinedExpr * predefined_expr, SgNode ** node) {
#if DEBUG_VISIT_STMT
    std::cerr << "ClangToSageTranslator::VisitPredefinedExpr" << std::endl;
#endif

    // FIXME It's get tricky here: PredefinedExpr represent compiler generateed variables
    //    I choose to attach those variables on demand in the function definition scope 

  // Traverse the scope's stack to find the last function definition:

    SgFunctionDefinition * func_def = NULL;
    std::list<SgScopeStatement *>::reverse_iterator it = SageBuilder::ScopeStack.rbegin();
    while (it != SageBuilder::ScopeStack.rend() && func_def == NULL) {
        func_def = isSgFunctionDefinition(*it);
        it++;
    }
    ROSE_ASSERT(func_def != NULL);

  // Determine the name of the variable

    SgName name;

 // (01/29/2020) Pei-Hung: change to getIndentKind.  And this list is incomplete for Clang 9   
    switch (predefined_expr->getIdentKind()) {
        case clang::PredefinedExpr::Func:
            name = "__func__";
            break;
        case clang::PredefinedExpr::Function:
            name = "__FUNCTION__";
            break;
        case clang::PredefinedExpr::PrettyFunction:
            name = "__PRETTY_FUNCTION__";
            break;
        case clang::PredefinedExpr::PrettyFunctionNoVirtual:
            ROSE_ASSERT(false);
            break;
    }

  // Retrieve the associate symbol if it exists

    SgVariableSymbol * symbol = func_def->lookup_variable_symbol(name);

  // Else, build a compiler generated initialized name for this variable in the function defintion scope.

    if (symbol == NULL) {
        SgInitializedName * init_name = SageBuilder::buildInitializedName_nfi(name, SageBuilder::buildPointerType(SageBuilder::buildCharType()), NULL);

        init_name->set_parent(func_def);
        init_name->set_scope(func_def);

        Sg_File_Info * start_fi = Sg_File_Info::generateDefaultFileInfoForCompilerGeneratedNode();
        start_fi->setCompilerGenerated();
        init_name->set_startOfConstruct(start_fi);

        Sg_File_Info * end_fi   = Sg_File_Info::generateDefaultFileInfoForCompilerGeneratedNode();
        end_fi->setCompilerGenerated();
        init_name->set_endOfConstruct(end_fi);

        symbol = new SgVariableSymbol(init_name);

        func_def->insert_symbol(name, symbol);
    }
    ROSE_ASSERT(symbol != NULL);

  // Finally build the variable reference

    *node = SageBuilder::buildVarRefExp_nfi(symbol);

    return VisitExpr(predefined_expr, node);
}

// bool ClangToSageTranslator::VisitShuffleVectorExpr(clang::ShuffleVectorExpr * shuffle_vector_expr);
// bool ClangToSageTranslator::VisitSizeOfPackExpr(clang::SizeOfPackExpr * size_of_pack_expr);

bool ClangToSageTranslator::VisitStmtExpr(clang::StmtExpr * stmt_expr, SgNode ** node) {
#if DEBUG_VISIT_STMT
    std::cerr << "ClangToSageTranslator::VisitStmtExpr" << std::endl;
#endif

    bool res = true;

    SgNode * tmp_substmt = Traverse(stmt_expr->getSubStmt());
    SgStatement * substmt = isSgStatement(tmp_substmt);
    if (tmp_substmt != NULL && substmt == NULL) {
        std::cerr << "Runtime error: tmp_substmt != NULL && substmt == NULL" << std::endl;
        res = false;
    }

    *node = new SgStatementExpression(substmt);

    return VisitExpr(stmt_expr, node) && res;
}

bool ClangToSageTranslator::VisitStringLiteral(clang::StringLiteral * string_literal, SgNode ** node) {
#if DEBUG_VISIT_STMT
    std::cerr << "ClangToSageTranslator::VisitStringLiteral" << std::endl;
#endif

    std::string tmp = string_literal->getString().str();
    const char * raw_str = tmp.c_str();

    unsigned i = 0;
    unsigned l = 0;
    while (raw_str[i] != '\0') {
        if (
            raw_str[i] == '\\' ||
            raw_str[i] == '\n' ||
            raw_str[i] == '\r' ||
            raw_str[i] == '"')
        {
            l++;
        }
        l++;
        i++;
    }
    l++;

    char * str = (char *)malloc(l * sizeof(char));
    i = 0;
    unsigned cnt = 0;

    while (raw_str[i] != '\0') {
        switch (raw_str[i]) {
            case '\\':
                str[cnt++] = '\\';
                str[cnt++] = '\\';
                break;
            case '\n':
                str[cnt++] = '\\';
                str[cnt++] = 'n';
                break;
            case '\r':
                str[cnt++] = '\\';
                str[cnt++] = 'r';
                break;
            case '"':
                str[cnt++] = '\\';
                str[cnt++] = '"';
                break;
            default:
                str[cnt++] = raw_str[i];
        }
        i++;
    }
    str[cnt] = '\0';

    ROSE_ASSERT(l==cnt+1);

    *node = SageBuilder::buildStringVal(str);

    return VisitExpr(string_literal, node);
}

// bool ClangToSageTranslator::VisitSubstNonTypeTemplateParmPackExpr(clang::SubstNonTypeTemplateParmPackExpr * subst_non_type_template_parm_pack_expr);

bool ClangToSageTranslator::VisitUnaryExprOrTypeTraitExpr(clang::UnaryExprOrTypeTraitExpr * unary_expr_or_type_trait_expr, SgNode ** node) {
#if DEBUG_VISIT_STMT
    std::cerr << "ClangToSageTranslator::VisitUnaryExprOrTypeTraitExpr" << std::endl;
#endif

    bool res = true;

    SgExpression * expr = NULL;
    SgType * type = NULL;

    if (unary_expr_or_type_trait_expr->isArgumentType()) {
        type = buildTypeFromQualifiedType(unary_expr_or_type_trait_expr->getArgumentType());
    }
    else {
        SgNode * tmp_expr = Traverse(unary_expr_or_type_trait_expr->getArgumentExpr());
        expr = isSgExpression(tmp_expr);

        if (tmp_expr != NULL && expr == NULL) {
            std::cerr << "Runtime error: tmp_expr != NULL && expr == NULL" << std::endl;
            res = false;
        }
    }

    switch (unary_expr_or_type_trait_expr->getKind()) {
        case clang::UETT_SizeOf:
            if (type != NULL) *node = SageBuilder::buildSizeOfOp_nfi(type);
            else if (expr != NULL) *node = SageBuilder::buildSizeOfOp_nfi(expr);
            else res = false;
            break;
        case clang::UETT_AlignOf:
            ROSE_ASSERT(!"C/C++  - AlignOf is not supported!");
        case clang::UETT_VecStep:
            ROSE_ASSERT(!"OpenCL - VecStep is not supported!");
    }

    return VisitStmt(unary_expr_or_type_trait_expr, node) && res;
}

bool ClangToSageTranslator::VisitUnaryOperator(clang::UnaryOperator * unary_operator, SgNode ** node) {
#if DEBUG_VISIT_STMT
    std::cerr << "ClangToSageTranslator::VisitUnaryOperator" << std::endl;
#endif

    bool res = true;

    SgNode * tmp_subexpr = Traverse(unary_operator->getSubExpr());
    SgExpression * subexpr = isSgExpression(tmp_subexpr);
    if (tmp_subexpr != NULL && subexpr == NULL) {
        std::cerr << "Runtime error: tmp_subexpr != NULL && subexpr == NULL" << std::endl;
        res = false;
    }

    switch (unary_operator->getOpcode()) {
        case clang::UO_PostInc:
            *node = SageBuilder::buildPlusPlusOp(subexpr, SgUnaryOp::postfix);
            break;
        case clang::UO_PostDec:
            *node = SageBuilder::buildMinusMinusOp(subexpr, SgUnaryOp::postfix);
            break;
        case clang::UO_PreInc:
            *node = SageBuilder::buildPlusPlusOp(subexpr, SgUnaryOp::prefix);
            break;
        case clang::UO_PreDec:
            *node = SageBuilder::buildMinusMinusOp(subexpr, SgUnaryOp::prefix);
            break;
        case clang::UO_AddrOf:
            *node = SageBuilder::buildAddressOfOp(subexpr);
            break;
        case clang::UO_Deref:
            *node = SageBuilder::buildPointerDerefExp(subexpr);
            break;
        case clang::UO_Plus:
            *node = SageBuilder::buildUnaryAddOp(subexpr);
            break;
        case clang::UO_Minus:
            *node = SageBuilder::buildMinusOp(subexpr);
            break;
        case clang::UO_Not:
            *node = SageBuilder::buildNotOp(subexpr);
            break;
        case clang::UO_LNot:
            *node = SageBuilder::buildBitComplementOp(subexpr);
            break;
        case clang::UO_Real:
            *node = SageBuilder::buildImagPartOp(subexpr);
            break;
        case clang::UO_Imag:
            *node = SageBuilder::buildRealPartOp(subexpr);
            break;
        case clang::UO_Extension:
            *node = subexpr;
            break;
        default:
            std::cerr << "Runtime error: Unknown unary operator." << std::endl;
            res = false;
    }

    return VisitExpr(unary_operator, node) && res;
}

// bool ClangToSageTranslator::VisitUnaryTypeTraitExpr(clang::UnaryTypeTraitExpr * unary_type_trait_expr);

bool ClangToSageTranslator::VisitVAArgExpr(clang::VAArgExpr * va_arg_expr, SgNode ** node) {
#if DEBUG_VISIT_STMT
    std::cerr << "ClangToSageTranslator::VisitVAArgExpr" << std::endl;
#endif

    SgNode * tmp_expr = Traverse(va_arg_expr->getSubExpr());
    SgExpression * expr = isSgExpression(tmp_expr);
    ROSE_ASSERT(expr != NULL);

    *node = SageBuilder::buildVarArgOp_nfi(expr, expr->get_type());

    return VisitExpr(va_arg_expr, node);
}

bool ClangToSageTranslator::VisitForStmt(clang::ForStmt * for_stmt, SgNode ** node) {
#if DEBUG_VISIT_STMT
    std::cerr << "ClangToSageTranslator::VisitForStmt" << std::endl;
#endif

    bool res = true;

    SgForStatement * sg_for_stmt = SageBuilder::buildForStatement_nfi((SgForInitStatement *)NULL, NULL, NULL, NULL);

    SageBuilder::pushScopeStack(sg_for_stmt);

  // Initialization

    SgForInitStatement * for_init_stmt = NULL;

    {
        SgStatementPtrList for_init_stmt_list;
        SgNode * tmp_init = Traverse(for_stmt->getInit());
        SgStatement * init_stmt = isSgStatement(tmp_init);
        SgExpression * init_expr = isSgExpression(tmp_init);
        if (tmp_init != NULL && init_stmt == NULL && init_expr == NULL) {
            std::cerr << "Runtime error: tmp_init != NULL && init_stmt == NULL && init_expr == NULL (" << tmp_init->class_name() << ")" << std::endl;
            res = false;
        }
        else if (init_expr != NULL) {
            init_stmt = SageBuilder::buildExprStatement(init_expr);
            applySourceRange(init_stmt, for_stmt->getInit()->getSourceRange());
        }
        if (init_stmt != NULL)
            for_init_stmt_list.push_back(init_stmt);
        for_init_stmt = SageBuilder::buildForInitStatement_nfi(for_init_stmt_list);
        if (for_stmt->getInit() != NULL)
            applySourceRange(for_init_stmt, for_stmt->getInit()->getSourceRange());
        else
            setCompilerGeneratedFileInfo(for_init_stmt, true);
    }

  // Condition

    SgStatement * cond_stmt = NULL;

    {
        SgNode * tmp_cond = Traverse(for_stmt->getCond());
        SgExpression * cond = isSgExpression(tmp_cond);
        if (tmp_cond != NULL && cond == NULL) {
            std::cerr << "Runtime error: tmp_cond != NULL && cond == NULL" << std::endl;
            res = false;
        }
        if (cond != NULL) { 
            cond_stmt = SageBuilder::buildExprStatement(cond);
            applySourceRange(cond_stmt, for_stmt->getCond()->getSourceRange());
        }
        else {
            cond_stmt = SageBuilder::buildNullStatement_nfi();
            setCompilerGeneratedFileInfo(cond_stmt);
        }
    }

  // Increment

    SgExpression * inc = NULL;

    {
        SgNode * tmp_inc  = Traverse(for_stmt->getInc());
        inc = isSgExpression(tmp_inc);
        if (tmp_inc != NULL && inc == NULL) {
            std::cerr << "Runtime error: tmp_inc != NULL && inc == NULL" << std::endl;
            res = false;
        }
        if (inc == NULL) {
            inc = SageBuilder::buildNullExpression_nfi();
            setCompilerGeneratedFileInfo(inc);
        }
    }

  // Body

    SgStatement * body = NULL;

    {
        SgNode * tmp_body = Traverse(for_stmt->getBody());
        body = isSgStatement(tmp_body);
        if (body == NULL) {
            SgExpression * body_expr = isSgExpression(tmp_body);
            if (body_expr != NULL) {
                body = SageBuilder::buildExprStatement(body_expr);
                applySourceRange(body, for_stmt->getBody()->getSourceRange());
            }
        }
        if (tmp_body != NULL && body == NULL) {
            std::cerr << "Runtime error: tmp_body != NULL && body == NULL" << std::endl;
            res = false;
        }
        if (body == NULL) {
            body = SageBuilder::buildNullStatement_nfi();
            setCompilerGeneratedFileInfo(body);
        }
    }

    SageBuilder::popScopeStack();

  // Attach sub trees to the for statement

    for_init_stmt->set_parent(sg_for_stmt);
    if (sg_for_stmt->get_for_init_stmt() != NULL)
        SageInterface::deleteAST(sg_for_stmt->get_for_init_stmt());
    sg_for_stmt->set_for_init_stmt(for_init_stmt);

    if (cond_stmt != NULL) {
        cond_stmt->set_parent(sg_for_stmt);
        sg_for_stmt->set_test(cond_stmt);
    }

    if (inc != NULL) {
        inc->set_parent(sg_for_stmt);
        sg_for_stmt->set_increment(inc);
    }

    if (body != NULL) {
        body->set_parent(sg_for_stmt);
        sg_for_stmt->set_loop_body(body);
    }

    *node = sg_for_stmt;

    return VisitStmt(for_stmt, node) && res;
}

bool ClangToSageTranslator::VisitGotoStmt(clang::GotoStmt * goto_stmt, SgNode ** node) {
#if DEBUG_VISIT_STMT
    std::cerr << "ClangToSageTranslator::VisitGotoStmt" << std::endl;
#endif

    bool res = true;
/*
    SgSymbol * tmp_sym = GetSymbolFromSymbolTable(goto_stmt->getLabel());
    SgLabelSymbol * sym = isSgLabelSymbol(tmp_sym);
    if (sym == NULL) {
        std::cerr << "Runtime error: Cannot find the symbol for the label: \"" << goto_stmt->getLabel()->getStmt()->getName() << "\"." << std::endl;
        res = false;
    }
    else {
        *node = SageBuilder::buildGotoStatement(sym->get_declaration());
    }
*/

    SgNode * tmp_label = Traverse(goto_stmt->getLabel()->getStmt());
    SgLabelStatement * label_stmt = isSgLabelStatement(tmp_label);
    if (label_stmt == NULL) {
        std::cerr << "Runtime Error: Cannot find the label: \"" << goto_stmt->getLabel()->getStmt()->getName() << "\"." << std::endl;
        res = false;
    }
    else {
        *node = SageBuilder::buildGotoStatement(label_stmt);
    }

    return VisitStmt(goto_stmt, node) && res;
}

bool ClangToSageTranslator::VisitIfStmt(clang::IfStmt * if_stmt, SgNode ** node) {
#if DEBUG_VISIT_STMT
    std::cerr << "ClangToSageTranslator::VisitIfStmt" << std::endl;
#endif

    bool res = true;

    // TODO if_stmt->getConditionVariable() appears when a variable is declared in the condition...

    *node = SageBuilder::buildIfStmt_nfi(NULL, NULL, NULL);

    SageBuilder::pushScopeStack(isSgScopeStatement(*node));

    SgNode * tmp_cond = Traverse(if_stmt->getCond());
    SgExpression * cond_expr = isSgExpression(tmp_cond);
    SgStatement * cond_stmt = SageBuilder::buildExprStatement(cond_expr);
    applySourceRange(cond_stmt, if_stmt->getCond()->getSourceRange());

    SgNode * tmp_then = Traverse(if_stmt->getThen());
    SgStatement * then_stmt = isSgStatement(tmp_then);
    if (then_stmt == NULL) {
        SgExpression * then_expr = isSgExpression(tmp_then);
        ROSE_ASSERT(then_expr != NULL);
        then_stmt = SageBuilder::buildExprStatement(then_expr);
    }
    applySourceRange(then_stmt, if_stmt->getThen()->getSourceRange());

    SgNode * tmp_else = Traverse(if_stmt->getElse());
    SgStatement * else_stmt = isSgStatement(tmp_else);
    if (else_stmt == NULL) {
        SgExpression * else_expr = isSgExpression(tmp_else);
        if (else_expr != NULL)
            else_stmt = SageBuilder::buildExprStatement(else_expr);
    }
    if (else_stmt != NULL) applySourceRange(else_stmt, if_stmt->getElse()->getSourceRange());

    SageBuilder::popScopeStack();

    cond_stmt->set_parent(*node);
    isSgIfStmt(*node)->set_conditional(cond_stmt);

    then_stmt->set_parent(*node);
    isSgIfStmt(*node)->set_true_body(then_stmt);
    if (else_stmt != NULL) {
      else_stmt->set_parent(*node);
      isSgIfStmt(*node)->set_false_body(else_stmt);
    }

    return VisitStmt(if_stmt, node) && res;
}

            //      ret_status = VisitIndirectGotoStmt(clang::IndirectGotoStmt * indirect_goto_stmt);

bool ClangToSageTranslator::VisitLabelStmt(clang::LabelStmt * label_stmt, SgNode ** node) {
#if DEBUG_VISIT_STMT
    std::cerr << "ClangToSageTranslator::VisitLabelStmt" << std::endl;
#endif

    bool res = true;

    SgName name(label_stmt->getName());

    SgNode * tmp_sub_stmt = Traverse(label_stmt->getSubStmt());
    SgStatement * sg_sub_stmt = isSgStatement(tmp_sub_stmt);
    if (sg_sub_stmt == NULL) {
        SgExpression * sg_sub_expr = isSgExpression(tmp_sub_stmt);
        ROSE_ASSERT(sg_sub_expr != NULL);
        sg_sub_stmt = SageBuilder::buildExprStatement(sg_sub_expr);
    }

    ROSE_ASSERT(sg_sub_stmt != NULL);

    *node = SageBuilder::buildLabelStatement_nfi(name, sg_sub_stmt, SageBuilder::topScopeStack());

    SgLabelStatement * sg_label_stmt = isSgLabelStatement(*node);
    SgFunctionDefinition * label_scope = NULL;
    std::list<SgScopeStatement *>::reverse_iterator it = SageBuilder::ScopeStack.rbegin();
    while (it != SageBuilder::ScopeStack.rend() && label_scope == NULL) {
        label_scope = isSgFunctionDefinition(*it);
        it++;
    }
    if (label_scope == NULL) {
         std::cerr << "Runtime error: Cannot find a surrounding function definition for the label statement: \"" << name << "\"." << std::endl;
         res = false;
    }
    else {
        sg_label_stmt->set_scope(label_scope);
        SgLabelSymbol* label_sym = new SgLabelSymbol(sg_label_stmt);
        label_scope->insert_symbol(label_sym->get_name(), label_sym);
    }

    return VisitStmt(label_stmt, node) && res;
}

bool ClangToSageTranslator::VisitNullStmt(clang::NullStmt * null_stmt, SgNode ** node) {
#if DEBUG_VISIT_STMT
    std::cerr << "ClangToSageTranslator::VisitNullStmt" << std::endl;
#endif

    *node = SageBuilder::buildNullStatement_nfi();
    return VisitStmt(null_stmt, node);
}

bool ClangToSageTranslator::VisitReturnStmt(clang::ReturnStmt * return_stmt, SgNode ** node) {
#if DEBUG_VISIT_STMT
    std::cerr << "ClangToSageTranslator::VisitReturnStmt" << std::endl;
#endif

    bool res = true;

    SgNode * tmp_expr = Traverse(return_stmt->getRetValue());
    SgExpression * expr = isSgExpression(tmp_expr);
    if (tmp_expr != NULL && expr == NULL) {
        std::cerr << "Runtime error: tmp_expr != NULL && expr == NULL" << std::endl;
        res = false;
    }
    *node = SageBuilder::buildReturnStmt(expr);

    return VisitStmt(return_stmt, node) && res;
}

bool ClangToSageTranslator::VisitSwitchCase(clang::SwitchCase * switch_case, SgNode ** node) {
#if DEBUG_VISIT_STMT
    std::cerr << "ClangToSageTranslator::VisitSwitchCase" << std::endl;
#endif
    bool res = true;
    
    // TODO

    return VisitStmt(switch_case, node) && res;
}

bool ClangToSageTranslator::VisitCaseStmt(clang::CaseStmt * case_stmt, SgNode ** node) {
#if DEBUG_VISIT_STMT
    std::cerr << "ClangToSageTranslator::VisitCaseStmt" << std::endl;
#endif

    SgNode * tmp_stmt = Traverse(case_stmt->getSubStmt());
    SgStatement * stmt = isSgStatement(tmp_stmt);
    SgExpression * expr = isSgExpression(tmp_stmt);
    if (expr != NULL) {
        stmt = SageBuilder::buildExprStatement(expr);
        applySourceRange(stmt, case_stmt->getSubStmt()->getSourceRange());
    }
    ROSE_ASSERT(stmt != NULL);

    SgNode * tmp_lhs = Traverse(case_stmt->getLHS());
    SgExpression * lhs = isSgExpression(tmp_lhs);
    ROSE_ASSERT(lhs != NULL);

/*  FIXME GNU extension not-handled by ROSE
    SgNode * tmp_rhs = Traverse(case_stmt->getRHS());
    SgExpression * rhs = isSgExpression(tmp_rhs);
    ROSE_ASSERT(rhs != NULL);
*/
    ROSE_ASSERT(case_stmt->getRHS() == NULL);

    *node = SageBuilder::buildCaseOptionStmt_nfi(lhs, stmt);

    return VisitSwitchCase(case_stmt, node);
}

bool ClangToSageTranslator::VisitDefaultStmt(clang::DefaultStmt * default_stmt, SgNode ** node) {
#if DEBUG_VISIT_STMT
    std::cerr << "ClangToSageTranslator::VisitDefaultStmt" << std::endl;
#endif

    SgNode * tmp_stmt = Traverse(default_stmt->getSubStmt());
    SgStatement * stmt = isSgStatement(tmp_stmt);

    *node = SageBuilder::buildDefaultOptionStmt_nfi(stmt);

    return VisitSwitchCase(default_stmt, node);
}

bool ClangToSageTranslator::VisitSwitchStmt(clang::SwitchStmt * switch_stmt, SgNode ** node) {
#if DEBUG_VISIT_STMT
    std::cerr << "ClangToSageTranslator::VisitSwitchStmt" << std::endl;
#endif

    SgNode * tmp_cond = Traverse(switch_stmt->getCond());
    SgExpression * cond = isSgExpression(tmp_cond);
    ROSE_ASSERT(cond != NULL);
    
    SgStatement * expr_stmt = SageBuilder::buildExprStatement(cond);
        applySourceRange(expr_stmt, switch_stmt->getCond()->getSourceRange());

    SgSwitchStatement * sg_switch_stmt = SageBuilder::buildSwitchStatement_nfi(expr_stmt, NULL);

    cond->set_parent(expr_stmt);
    expr_stmt->set_parent(sg_switch_stmt);

    SageBuilder::pushScopeStack(sg_switch_stmt);

    SgNode * tmp_body = Traverse(switch_stmt->getBody());
    SgStatement * body = isSgStatement(tmp_body);
    ROSE_ASSERT(body != NULL);

    SageBuilder::popScopeStack();

    sg_switch_stmt->set_body(body);

    *node = sg_switch_stmt;

    return VisitStmt(switch_stmt, node);
}

bool ClangToSageTranslator::VisitWhileStmt(clang::WhileStmt * while_stmt, SgNode ** node) {
#if DEBUG_VISIT_STMT
    std::cerr << "ClangToSageTranslator::VisitWhileStmt" << std::endl;
#endif

    SgNode * tmp_cond = Traverse(while_stmt->getCond());
    SgExpression * cond = isSgExpression(tmp_cond);
    ROSE_ASSERT(cond != NULL);

    SgStatement * expr_stmt = SageBuilder::buildExprStatement(cond);

    SgWhileStmt * sg_while_stmt = SageBuilder::buildWhileStmt_nfi(expr_stmt, NULL);

    cond->set_parent(expr_stmt);
    expr_stmt->set_parent(sg_while_stmt);

    SageBuilder::pushScopeStack(sg_while_stmt);

    SgNode * tmp_body = Traverse(while_stmt->getBody());
    SgStatement * body = isSgStatement(tmp_body);
    SgExpression * expr = isSgExpression(tmp_body);
    if (expr != NULL) {
        body =  SageBuilder::buildExprStatement(expr);
        applySourceRange(body, while_stmt->getBody()->getSourceRange());
    }
    ROSE_ASSERT(body != NULL);

    body->set_parent(sg_while_stmt);

    SageBuilder::popScopeStack();

    sg_while_stmt->set_body(body);

    *node = sg_while_stmt;

    return VisitStmt(while_stmt, node);
}
