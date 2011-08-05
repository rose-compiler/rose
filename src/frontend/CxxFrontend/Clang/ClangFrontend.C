
#include <iostream>

#include "ClangFrontend-private.h"

int clang_main(int argc, char* argv[], SgSourceFile& sageFile) {
    std::vector<std::string> args;
    for (int i = 0; i < argc; i++)
        args.push_back(std::string(argv[i]));

    ClangToSageTranslator translator(args);

    SgGlobal * global_scope = translator.getGlobalScope();

    if (sageFile.get_globalScope() != NULL) SageInterface::deleteAST(sageFile.get_globalScope());

    sageFile.set_globalScope(global_scope);

    global_scope->set_parent(&sageFile);

    Sg_File_Info * start_fi = new Sg_File_Info(args[args.size()-1], 0, 0);
    Sg_File_Info * end_fi   = new Sg_File_Info(args[args.size()-1], 0, 0);

    global_scope->set_startOfConstruct(start_fi);

    global_scope->set_endOfConstruct(end_fi);

    return 1;
}

SgGlobal * ClangToSageTranslator::getGlobalScope() { return p_global_scope; }

ClangToSageTranslator::ClangToSageTranslator(std::vector<std::string> & arg) :
    clang::ASTConsumer(),
    p_decl_translation_map(),
    p_stmt_translation_map(),
    p_type_translation_map(),
    p_global_scope(NULL),
    p_file_manager(NULL),
    p_diagnostic(NULL),
    p_source_manager(NULL),
    p_header_search(NULL),
    p_preprocessor(NULL)
{
  /* 1 - Analyse command line */

    /* Input file */
    std::string input = arg[arg.size() - 1];

    /* Language detection */
    Language language = unknown;
    size_t last_period = input.find_last_of(".");
    std::string extention(input.substr(last_period + 1));

    if (extention == "c") {
        language = C;
    }
    else if (extention == "C" || extention == "cxx" || extention == "cpp") {
        language = CPLUSPLUS;
    }
    else if (extention == "objc") {
        language = OBJC;
    }
    else if (extention == "cu") {
        language = CUDA;
    }
    else if (extention == "ocl" || extention == "cl") {
        language = OPENCL;
    }

  /* 2 - Call the parser... */

    clang::FileSystemOptions file_options;

    p_file_manager = new clang::FileManager(file_options);

    clang::DiagnosticIDs * diag_id = new clang::DiagnosticIDs();

    const llvm::IntrusiveRefCntPtr<clang::DiagnosticIDs> diag_id_ptr(diag_id);

    llvm::raw_os_ostream output_stream(std::cout);

    clang::DiagnosticOptions diag_opts;

    clang::TextDiagnosticPrinter * diag_printer = new clang::TextDiagnosticPrinter(output_stream, diag_opts);

    p_diagnostic = new clang::Diagnostic(diag_id_ptr, diag_printer);

    p_source_manager = new clang::SourceManager(*p_diagnostic, *p_file_manager);

    p_header_search = new clang::HeaderSearch(*p_file_manager);

    clang::LangOptions lang_options;
        switch (language) {
            case C: break;
            case CPLUSPLUS: lang_options.CPlusPlus = 1; break;
            default:
                std::cerr << "Unsupported language..." << std::endl;
                exit(-1);
        }


    clang::TargetOptions target_options;
        target_options.Triple = LLVM_HOSTTRIPLE;
        target_options.ABI = "";
        target_options.CPU = "";
        target_options.Features.clear();

    clang::TargetInfo * target_info = clang::TargetInfo::CreateTargetInfo(*p_diagnostic, target_options);

    p_preprocessor = new clang::Preprocessor(*p_diagnostic, lang_options, *target_info, *p_source_manager, *p_header_search);

    clang::IdentifierTable identifier_table(lang_options);

    clang::SelectorTable selector_table;

    clang::Builtin::Context builtin_context(*target_info);

    clang::ASTContext context(
        lang_options,
        *p_source_manager,
        *target_info,
        identifier_table,
        selector_table,
        builtin_context,
        0
    );

    const clang::FileEntry * input_file_id = p_file_manager->getFile(input);

    p_source_manager->createMainFileID(input_file_id);

    clang::ParseAST(*p_preprocessor, this, context, false, true, NULL);
}


ClangToSageTranslator::~ClangToSageTranslator() {
    delete p_preprocessor;
    delete p_header_search;
    delete p_source_manager;
    delete p_diagnostic;
    delete p_file_manager;
}

/* (protected) Helper methods */

void ClangToSageTranslator::applySourceRange(SgNode * node, clang::SourceRange source_range) {
    SgLocatedNode * located_node = isSgLocatedNode(node);
    SgInitializedName * init_name = isSgInitializedName(node);

    if (located_node == NULL && init_name == NULL) {
        std::cerr << "Consistency error: try to apply a source range to a Sage node which are not a SgLocatedNode or a SgInitializedName." << std::endl;
        exit(-1);
    }
    else if (located_node != NULL) {
        Sg_File_Info * fi = located_node->get_startOfConstruct();
        if (fi != NULL) delete fi;
        fi = located_node->get_endOfConstruct();
        if (fi != NULL) delete fi;
    }
    else if (init_name != NULL) {
        Sg_File_Info * fi = init_name->get_startOfConstruct();
        if (fi != NULL) delete fi;
        fi = init_name->get_endOfConstruct();
        if (fi != NULL) delete fi;
    }

    clang::SourceLocation begin  = source_range.getBegin();
    clang::SourceLocation end    = source_range.getEnd();

    clang::FileID file_begin = p_source_manager->getFileID(begin);
    clang::FileID file_end   = p_source_manager->getFileID(end);

    bool inv_begin_line;
    bool inv_begin_col;
    bool inv_end_line;
    bool inv_end_col;

    unsigned ls = p_source_manager->getSpellingLineNumber(begin, &inv_begin_line);
    unsigned cs = p_source_manager->getSpellingColumnNumber(begin, &inv_begin_col);
    unsigned le = p_source_manager->getSpellingLineNumber(end, &inv_end_line);
    unsigned ce = p_source_manager->getSpellingColumnNumber(end, &inv_end_col);

    std::string file = p_source_manager->getFileEntryForID(file_begin)->getName();

    Sg_File_Info * start_fi = new Sg_File_Info(file, ls, cs);
    Sg_File_Info * end_fi   = new Sg_File_Info(file, le, ce);

    if (located_node != NULL) {
        located_node->set_startOfConstruct(start_fi);
        located_node->set_endOfConstruct(end_fi);
    }
    else if (init_name != NULL) {
        init_name->set_startOfConstruct(start_fi);
        init_name->set_endOfConstruct(end_fi);
    }

}

/* Overload of ASTConsumer::HandleTranslationUnit, it is the "entry point" */

void ClangToSageTranslator::HandleTranslationUnit(clang::ASTContext & ast_context) {
    Traverse(ast_context.getTranslationUnitDecl());
}

/* Traverse({Decl|Stmt|Type} *) methods */

SgNode * ClangToSageTranslator::Traverse(clang::Decl * decl) {
    if (decl == NULL)
        return NULL;

    if (decl->isImplicit()) {
        std::cerr << "Warning: skip implicit declaration." << std::endl;
        return NULL;
    }

    std::map<clang::Decl *, SgNode *>::iterator it = p_decl_translation_map.find(decl);
    if (it != p_decl_translation_map.end())
        return it->second;

    SgNode * result = NULL;
    bool ret_status = false;

    switch (decl->getKind()) {
        case clang::Decl::TranslationUnit:
            ret_status = VisitTranslationUnitDecl((clang::TranslationUnitDecl *)decl, &result);
            break;
        case clang::Decl::Typedef:
            // TODO
//          ret_status = VisitTypedef((clang::TypedefDecl *)decl, &result);
            break;
        case clang::Decl::Var:
            ret_status = VisitVarDecl((clang::VarDecl *)decl, &result);
            break;
        case clang::Decl::Function:
            ret_status = VisitFunctionDecl((clang::FunctionDecl *)decl, &result);
            break;
        case clang::Decl::ParmVar:
            ret_status = VisitParmVarDecl((clang::ParmVarDecl *)decl, &result);
            break;
        // TODO cases
        default:
            std::cerr << "Unknown declacaration kind: " << decl->getDeclKindName() << " !" << std::endl;
            ROSE_ASSERT(false);
    }

    // FIXME this assertion need to be activate (lock by typedef)
//  ROSE_ASSERT(result != NULL);

    p_decl_translation_map.insert(std::pair<clang::Decl *, SgNode *>(decl, result));

    return result;
}

SgNode * ClangToSageTranslator::Traverse(clang::Stmt * stmt) {
    if (stmt == NULL)
        return NULL;

    std::map<clang::Stmt *, SgNode *>::iterator it = p_stmt_translation_map.find(stmt);
    if (it != p_stmt_translation_map.end())
        return it->second; 

    SgNode * result = NULL;
    bool ret_status = false;

    switch (stmt->getStmtClass()) {
        case clang::Stmt::IntegerLiteralClass:
            ret_status = VisitIntegerLiteral((clang::IntegerLiteral *)stmt, &result);
            break;
        case clang::Stmt::ImplicitCastExprClass:
            ret_status = VisitImplicitCastExpr((clang::ImplicitCastExpr *)stmt, &result);
            break;
        case clang::Stmt::CharacterLiteralClass:
            ret_status = VisitCharacterLiteral((clang::CharacterLiteral *)stmt, &result);
            break;
        case clang::Stmt::StringLiteralClass:
            ret_status = VisitStringLiteral((clang::StringLiteral *)stmt, &result);
            break;
        case clang::Stmt::CompoundStmtClass:
            ret_status = VisitCompoundStmt((clang::CompoundStmt *)stmt, &result);
            break;
        case clang::Stmt::DeclStmtClass:
            ret_status = VisitDeclStmt((clang::DeclStmt *)stmt, &result);
            break;
        case clang::Stmt::CallExprClass:
            ret_status = VisitCallExpr((clang::CallExpr *)stmt, &result);
            break;
        case clang::Stmt::DeclRefExprClass:
            ret_status = VisitDeclRefExpr((clang::DeclRefExpr *)stmt, &result);
            break;
        case clang::Stmt::ReturnStmtClass:
            ret_status = VisitReturnStmt((clang::ReturnStmt *)stmt, &result);
            break;
        case clang::Stmt::BinaryOperatorClass:
            ret_status = VisitBinaryOperator((clang::BinaryOperator *)stmt, &result);
            break;

        // TODO
        default:
            std::cerr << "Unknown statement kind: " << stmt->getStmtClassName() << " !" << std::endl;
            ROSE_ASSERT(false);
    }

    ROSE_ASSERT(result != NULL);

    p_stmt_translation_map.insert(std::pair<clang::Stmt *, SgNode *>(stmt, result));

    return result;
}

SgNode * ClangToSageTranslator::Traverse(const clang::Type * type) {
    if (type == NULL)
        return NULL;

    std::map<const clang::Type *, SgNode *>::iterator it = p_type_translation_map.find(type);
    if (it != p_type_translation_map.end()) 
         return it->second;

    SgNode * result = NULL;
    bool ret_status = false;

    switch (type->getTypeClass()) {
        case clang::Type::Builtin:
            ret_status = VisitBuiltinType((clang::BuiltinType *)type, &result);
            break;
        case clang::Type::Pointer:
            ret_status = VisitPointerType((clang::PointerType *)type, &result);
            break;
        case clang::Type::ConstantArray:
            ret_status = VisitConstantArrayType((clang::ConstantArrayType *)type, &result);
            break;
        case clang::Type::Paren:
            ret_status = VisitParenType((clang::ParenType *)type, &result);
            break;
        case clang::Type::FunctionProto:
            ret_status = VisitFunctionProtoType((clang::FunctionProtoType *)type, &result);
            break;
        case clang::Type::FunctionNoProto:
            ret_status = VisitFunctionNoProtoType((clang::FunctionNoProtoType *)type, &result);
            break;
        // TODO cases
        default:
            std::cerr << "Unknown type kind " << type->getTypeClassName() << " !" << std::endl;
            ROSE_ASSERT(false);
    }

    ROSE_ASSERT(result != NULL);

    p_type_translation_map.insert(std::pair<const clang::Type *, SgNode *>(type, result));

    return result;
}

SgNode * ClangToSageTranslator::TraverseForDeclContext(clang::DeclContext * decl_context) {
    return Traverse((clang::Decl*)decl_context);
}

/**********************/
/* Visit Declarations */
/**********************/

bool ClangToSageTranslator::VisitDecl(clang::Decl * decl, SgNode ** node) { 
    if (*node == NULL) {
        std::cerr << "Runtime error: No Sage node associated with the declaration..." << std::endl;
        return false;
    }

    if (!isSgGlobal(*node))
        applySourceRange(*node, decl->getSourceRange());

    // TODO attributes

    return true;
}

bool ClangToSageTranslator::VisitFunctionDecl(clang::FunctionDecl * function_decl, SgNode ** node) {
    bool res = true;

    SgName name(function_decl->getNameAsString());

    clang::QualType qual_ret_type = function_decl->getResultType();
    SgNode * tmp_ret_type = Traverse(qual_ret_type.getTypePtr());
    SgType * ret_type = isSgType(tmp_ret_type);
    if (tmp_ret_type != NULL && ret_type == NULL) {
        std::cerr << "Runtime error: tmp_type != NULL && type == NULL" << std::endl;
        res = false;
    }
    else if (ret_type != NULL) {
        // TODO qualifiers
    }

    SgFunctionParameterList * param_list = SageBuilder::buildFunctionParameterList_nfi();
      applySourceRange(param_list, function_decl->getSourceRange()); // FIXME

    for (unsigned i = 0; i < function_decl->getNumParams(); i++) {
        SgNode * tmp_init_name = Traverse(function_decl->getParamDecl(i));
        SgInitializedName * init_name = isSgInitializedName(tmp_init_name);
        if (tmp_init_name != NULL && init_name == NULL) {
            std::cerr << "Runtime error: tmp_init_name != NULL && init_name == NULL" << std::endl;
            res = false;
            continue;
        }

        param_list->append_arg(init_name);
    }

    SgFunctionDeclaration * sg_function_decl;

    if (function_decl->isThisDeclarationADefinition()) {
        sg_function_decl = SageBuilder::buildDefiningFunctionDeclaration(name, ret_type, param_list, NULL);
        sg_function_decl->set_definingDeclaration(sg_function_decl);

        if (!function_decl->hasBody()) {
            std::cerr << "Defining function declaration without body..." << std::endl;
            res = false;
        }

        if (sg_function_decl->get_definition() != NULL) SageInterface::deleteAST(sg_function_decl->get_definition());

        SgFunctionDefinition * function_definition = new SgFunctionDefinition(sg_function_decl, NULL);

        SageBuilder::pushScopeStack(function_definition);

        SgNode * tmp_body = Traverse(function_decl->getBody());
        SgBasicBlock * body = isSgBasicBlock(tmp_body);

        SageBuilder::popScopeStack();

        if (tmp_body != NULL && body == NULL) {
            std::cerr << "Runtime error: tmp_body != NULL && body == NULL" << std::endl;
            res = false;
        }
        else {
            function_definition->set_body(body);
            body->set_parent(function_definition);
            applySourceRange(function_definition, function_decl->getSourceRange());
        }

        sg_function_decl->set_definition(function_definition);
        function_definition->set_parent(sg_function_decl);

        SgInitializedNamePtrList & init_names = param_list->get_args();
        SgInitializedNamePtrList::iterator it;
        for (it = init_names.begin(); it != init_names.end(); it++) {
             (*it)->set_scope(function_definition);
             SgSymbolTable * st = function_definition->get_symbol_table();
             // TODO insert symbol
        }

        SgFunctionDeclaration * first_decl;
        if (function_decl->getFirstDeclaration() == function_decl) {
//          SgFunctionParameterList * param_list_ = SageBuilder::buildFunctionParameterList_nfi();
  //           applySourceRange(param_list_, function_decl->getSourceRange()); // FIXME
    //      for (it = init_names.begin(); it != init_names.end(); it++)
      //        param_list_->append_arg(*it);

            first_decl = SageBuilder::buildNondefiningFunctionDeclaration(name, ret_type, SageInterface::deepCopy(param_list), NULL);
//          first_decl = SageBuilder::buildNondefiningFunctionDeclaration(name, ret_type, param_list_), NULL);
            first_decl->setCompilerGenerated();
            first_decl->set_parent(SageBuilder::topScopeStack());
        }
        else {
            SgNode * tmp_first_decl = Traverse(function_decl->getFirstDeclaration());
            first_decl = isSgFunctionDeclaration(tmp_first_decl);
            if (tmp_first_decl != NULL && first_decl == NULL) {
                std::cerr << "Runtime error: tmp_first_decl != NULL && first_decl == NULL" << std::endl;
                res = false;
            }
        }

        sg_function_decl->set_firstNondefiningDeclaration(first_decl);
    }
    else {
        sg_function_decl = SageBuilder::buildNondefiningFunctionDeclaration(name, ret_type, param_list, NULL);

        SgInitializedNamePtrList & init_names = param_list->get_args();
        SgInitializedNamePtrList::iterator it;
        for (it = init_names.begin(); it != init_names.end(); it++) {
             (*it)->set_scope(SageBuilder::topScopeStack());
        }

        if (function_decl->getFirstDeclaration() != function_decl) {
            SgNode * tmp_first_decl = Traverse(function_decl->getFirstDeclaration());
            SgFunctionDeclaration * first_decl = isSgFunctionDeclaration(tmp_first_decl);
            if (tmp_first_decl != NULL && first_decl == NULL) {
                std::cerr << "Runtime error: tmp_first_decl != NULL && first_decl == NULL" << std::endl;
                res = false;
            }
            else if (first_decl != NULL) {
                if (first_decl->get_firstNondefiningDeclaration() != NULL)
                    sg_function_decl->set_firstNondefiningDeclaration(first_decl->get_firstNondefiningDeclaration());
                else
                    sg_function_decl->set_firstNondefiningDeclaration(first_decl);
            }
        }
        else {
            sg_function_decl->set_firstNondefiningDeclaration(sg_function_decl);
        }
    }

    *node = sg_function_decl;

    return VisitDecl(function_decl, node) && res;
}

bool ClangToSageTranslator::VisitVarDecl(clang::VarDecl * var_decl, SgNode ** node) {
     bool res = true;

  // Create the SAGE node: SgVariableDeclaration

    SgName name(var_decl->getNameAsString());

    clang::QualType qual_type = var_decl->getType();
    SgNode * tmp_type = Traverse(qual_type.getTypePtr());
    SgType * type = isSgType(tmp_type);
    if (tmp_type != NULL && type == NULL) {
        std::cerr << "Runtime error: not a SgType..." << std::endl; // TODO
        res = false;
    }
    else if (type != NULL) {
        // TODO qualifiers
    }

    clang::Expr * init_expr = var_decl->getInit();
    SgNode * tmp_init = Traverse(init_expr);
    SgExpression * expr = isSgExpression(tmp_init);
    if (tmp_init != NULL && expr == NULL) {
        std::cerr << "Runtime error: not a SgInitializer..." << std::endl; // TODO
        res = false;
    }
    SgInitializer * init = expr != NULL ? SageBuilder::buildAssignInitializer_nfi(expr, expr->get_type()) : NULL;
    if (init != NULL)
        applySourceRange(init, init_expr->getSourceRange());

    SgVariableDeclaration * sg_var_decl = SageBuilder::buildVariableDeclaration_nfi(name, type, init, NULL); // scope: obtain from the scope stack.

    applySourceRange(sg_var_decl->get_variables()[0], var_decl->getSourceRange());
    applySourceRange(sg_var_decl->get_variables()[0]->get_definition(), var_decl->getSourceRange());

    *node = sg_var_decl;

    return VisitDecl(var_decl, node) && res;

}

bool ClangToSageTranslator::VisitParmVarDecl(clang::ParmVarDecl * param_var_decl, SgNode ** node) {
    bool res = true;

    SgName name(param_var_decl->getNameAsString());

    clang::QualType qual_type = param_var_decl->getType();
    SgNode * tmp_type = Traverse(qual_type.getTypePtr());
    SgType * type = isSgType(tmp_type);
    if (tmp_type != NULL && type == NULL) {
        std::cerr << "Runtime error: tmp_type != NULL && type == NULL" << std::endl; // TODO
        res = false;
    }
    else if (type != NULL) {
        // TODO qualifiers
    }

    SgInitializer * init = NULL;

    if (param_var_decl->hasDefaultArg()) {
        SgNode * tmp_expr = Traverse(param_var_decl->getDefaultArg());
        SgExpression * expr = isSgExpression(tmp_expr);
        if (tmp_expr != NULL && expr == NULL) {
            std::cerr << "Runtime error: tmp_expr != NULL && expr == NULL" << std::endl;
            res = false;
        }
        else {
            applySourceRange(expr, param_var_decl->getDefaultArgRange());
            init = SageBuilder::buildAssignInitializer_nfi(expr, expr->get_type());
            applySourceRange(init, param_var_decl->getDefaultArgRange());
        }
    }

    *node = SageBuilder::buildInitializedName(name, type, init);

    return VisitDecl(param_var_decl, node) && res;
}

bool ClangToSageTranslator::VisitTranslationUnitDecl(clang::TranslationUnitDecl * translation_unit_decl, SgNode ** node) {
    if (*node != NULL) {
        std::cerr << "Runtime error: The TranslationUnitDecl is already associated to a SAGE node." << std::endl;
        return false;
    }

  // Create the SAGE node: SgGlobal

    if (p_global_scope != NULL) {
        std::cerr << "Runtime error: Global Scope have already been set !" << std::endl;
        return false;
    }

    *node = p_global_scope = new SgGlobal();

    p_decl_translation_map.insert(std::pair<clang::Decl *, SgNode *>(translation_unit_decl, p_global_scope));

  // Traverse the children

    SageBuilder::pushScopeStack(*node);

    clang::DeclContext * decl_context = (clang::DeclContext *)translation_unit_decl; // useless but more clear

    bool res = true;
    clang::DeclContext::decl_iterator it;
    for (it = decl_context->decls_begin(); it != decl_context->decls_end(); it++) {
        if (*it == NULL) continue;
        SgNode * child = Traverse(*it);

        SgDeclarationStatement * decl_stmt = isSgDeclarationStatement(child);
        if (decl_stmt == NULL) {
            std::cerr << "Runtime error: the node produce for a clang::Decl is not a SgDeclarationStatement !" << std::endl;
            res = false;
        }
        else
            p_global_scope->append_declaration(decl_stmt);
    }

    SageBuilder::popScopeStack();

  // Traverse the class hierarchy

    return VisitDecl(translation_unit_decl, node) && res;
}

/********************/
/* Visit Statements */
/********************/

bool ClangToSageTranslator::VisitStmt(clang::Stmt * stmt, SgNode ** node) {
    if (*node == NULL) {
        std::cerr << "Runtime error: No Sage node associated with the type..." << std::endl;
        return false;
    }

    // TODO

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

bool ClangToSageTranslator::VisitCompoundStmt(clang::CompoundStmt * compound_stmt, SgNode ** node) {
    bool res = true;

    SgBasicBlock * block = SageBuilder::buildBasicBlock();

    SageBuilder::pushScopeStack(block);

    clang::CompoundStmt::body_iterator it;
    for (it = compound_stmt->body_begin(); it != compound_stmt->body_end(); it++) {
        SgNode * tmp_node = Traverse(*it);
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

bool ClangToSageTranslator::VisitDeclStmt(clang::DeclStmt * decl_stmt, SgNode ** node) {
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

bool ClangToSageTranslator::VisitExpr(clang::Expr * expr, SgNode ** node) {
     // TODO

     return VisitStmt(expr, node);
}

bool ClangToSageTranslator::VisitBinaryOperator(clang::BinaryOperator * binary_operator, SgNode ** node) {
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
//      case clang::BO_PtrMemD:   *node = SageBuilder::build(lhs, rhs); break;
//      case clang::BO_PtrMemI:   *node = SageBuilder::build(lhs, rhs); break;
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
//`     case clang::BO_Comma:     *node = SageBuilder::build(lhs, rhs); break;
        default:
            std::cerr << "Unknown opcode for binary operator..." << std::endl;
            res = false;
    }

    return VisitExpr(binary_operator, node) && res;
}

bool ClangToSageTranslator::VisitCallExpr(clang::CallExpr * call_expr, SgNode ** node) {
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

bool ClangToSageTranslator::VisitImplicitCastExpr(clang::ImplicitCastExpr * implicit_cast_expr, SgNode ** node) {
    SgNode * tmp_expr = Traverse(implicit_cast_expr->getSubExpr());
    SgExpression * expr = isSgExpression(tmp_expr);
    
    ROSE_ASSERT(expr != NULL);

    clang::QualType qual_type = implicit_cast_expr->getType();

    SgNode * tmp_type = Traverse(qual_type.getTypePtr());
    SgType * type = isSgType(tmp_type);

    ROSE_ASSERT(type != NULL);
/*
    SgCastExp * res = SageBuilder::buildCastExp(expr, type);
        res->setCompilerGenerated();

    *node = res;
*/

    *node = expr;

    return VisitExpr(implicit_cast_expr, node);
}

bool ClangToSageTranslator::VisitCharacterLiteral(clang::CharacterLiteral * character_literal, SgNode ** node) {
/*
    char val[4];
        val[0] = '\'';
        val[1] = character_literal->getValue();
        val[2] = '\'';
        val[3] = '\0';
    std::string str(val);
    *node = SageBuilder::buildCharVal_nfi(val[0], str);
*/
    *node = SageBuilder::buildCharVal(character_literal->getValue());

    return VisitExpr(character_literal, node);
}

bool ClangToSageTranslator::VisitDeclRefExpr(clang::DeclRefExpr * decl_ref_expr, SgNode ** node) {
    bool res = true;

    SgNode * tmp_node = Traverse(decl_ref_expr->getDecl());
    SgVariableDeclaration * var_decl = isSgVariableDeclaration(tmp_node);
    SgFunctionDeclaration * fun_decl = isSgFunctionDeclaration(tmp_node);
    SgInitializedName * init_name    = isSgInitializedName(tmp_node);
    
    bool successful_cast = var_decl || fun_decl || init_name;
    if (tmp_node != NULL && !successful_cast) {
        std::cerr << "Runtime error: tmp_node != NULL && !successful_cast" << std::endl;
        std::cerr << "tmp_node->class_name() = " << tmp_node->class_name()  << std::endl;
        res = false;
    }
    else if (var_decl != NULL) {
        *node = SageBuilder::buildVarRefExp(var_decl);
    }
    else if (fun_decl != NULL) {
        *node = SageBuilder::buildFunctionRefExp(fun_decl);
    }
    else if (init_name != NULL) {
        *node = SageBuilder::buildVarRefExp(init_name, init_name->get_scope());
    }

    return VisitExpr(decl_ref_expr, node) && res;
}

bool ClangToSageTranslator::VisitIntegerLiteral(clang::IntegerLiteral * integer_literal, SgNode ** node) {
//  *node = SageBuilder::buildIntVal_nfi(integer_literal->getValue().getSExtValue());
    *node = SageBuilder::buildIntVal(integer_literal->getValue().getSExtValue());

    return VisitExpr(integer_literal, node);
}

bool ClangToSageTranslator::VisitStringLiteral(clang::StringLiteral * string_literal, SgNode ** node) {
//  *node = SageBuilder::buildStringVal_nfi(string_literal->getString().str());
    *node = SageBuilder::buildStringVal(string_literal->getString().str());

    return VisitExpr(string_literal, node);
}

bool ClangToSageTranslator::VisitReturnStmt(clang::ReturnStmt * return_stmt, SgNode ** node) {
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

/***************/
/* Visit Types */
/***************/

bool ClangToSageTranslator::VisitType(clang::Type * type, SgNode ** node) {
    if (*node == NULL) {
        std::cerr << "Runtime error: No Sage node associated with the type..." << std::endl;
        return false;
    }

    // TODO

    return true;
}

bool ClangToSageTranslator::VisitConstantArrayType(clang::ConstantArrayType * constant_array_type, SgNode ** node) {
    clang::QualType qual_type = constant_array_type->getElementType();

    SgNode * tmp_type = Traverse(qual_type.getTypePtr());
    SgType * type = isSgType(tmp_type);

    ROSE_ASSERT(type != NULL);

    // TODO clang::ArrayType::ArraySizeModifier
    // TODO Qualifiers

    SgExpression * expr = SageBuilder::buildIntVal(constant_array_type->getSize().getSExtValue());

    *node = SageBuilder::buildArrayType(type, expr);

    return VisitType(constant_array_type, node);
}

bool ClangToSageTranslator::VisitBuiltinType(clang::BuiltinType * builtin_type, SgNode ** node) {
    switch (builtin_type->getKind()) {
        case clang::BuiltinType::Void:       *node = SageBuilder::buildVoidType();        break;
        case clang::BuiltinType::Bool:       *node = SageBuilder::buildBoolType();        break;
        case clang::BuiltinType::Short:      *node = SageBuilder::buildShortType();       break;
        case clang::BuiltinType::Int:        *node = SageBuilder::buildIntType();         break;
        case clang::BuiltinType::Long:       *node = SageBuilder::buildLongType();        break;
        case clang::BuiltinType::LongLong:   *node = SageBuilder::buildLongLongType();    break;
        case clang::BuiltinType::Float:      *node = SageBuilder::buildFloatType();       break;
        case clang::BuiltinType::Double:     *node = SageBuilder::buildDoubleType();      break;
        case clang::BuiltinType::LongDouble: *node = SageBuilder::buildLongDoubleType();  break;

        case clang::BuiltinType::Char_S:     *node = SageBuilder::buildCharType();        break;

        case clang::BuiltinType::UInt:       *node = SageBuilder::buildUnsignedIntType(); break;

/*
        case clang::BuiltinType::NullPtr:    *node = SageBuilder::build(); break;
*/
 
        case clang::BuiltinType::Char_U:    std::cerr << "Char_U    -> "; break;
        case clang::BuiltinType::UChar:     std::cerr << "UChar     -> "; break;
        case clang::BuiltinType::WChar_U:   std::cerr << "WChar_U   -> "; break;
        case clang::BuiltinType::Char16:    std::cerr << "Char16    -> "; break;
        case clang::BuiltinType::Char32:    std::cerr << "Char32    -> "; break;
        case clang::BuiltinType::SChar:     std::cerr << "SChar     -> "; break;
        case clang::BuiltinType::WChar_S:   std::cerr << "WChar_S   -> "; break;

        case clang::BuiltinType::UShort:    std::cerr << "UShort    -> "; break;
        case clang::BuiltinType::ULong:     std::cerr << "ULong     -> "; break;
        case clang::BuiltinType::ULongLong: std::cerr << "ULongLong -> "; break;
        case clang::BuiltinType::UInt128:   std::cerr << "UInt128   -> "; break;
        case clang::BuiltinType::Int128:    std::cerr << "Int128    -> "; break;

        case clang::BuiltinType::ObjCId:
        case clang::BuiltinType::ObjCClass:
        case clang::BuiltinType::ObjCSel:
        case clang::BuiltinType::Dependent:
        case clang::BuiltinType::Overload:
        case clang::BuiltinType::BoundMember:
        case clang::BuiltinType::UnknownAny:
        default:
            std::cerr << "Unknown builtin type: " << builtin_type->getName(p_preprocessor->getLangOptions()) << " !" << std::endl;
            exit(-1);
    }

    return VisitType(builtin_type, node);
}

bool ClangToSageTranslator::VisitFunctionNoProtoType(clang::FunctionNoProtoType * function_no_proto_type, SgNode ** node) {
    bool res = true;

    SgFunctionParameterTypeList * param_type_list = new SgFunctionParameterTypeList();

    clang::QualType qual_ret_type = function_no_proto_type->getResultType();
    SgNode * tmp_ret_type = Traverse(qual_ret_type.getTypePtr());
    SgType * ret_type = isSgType(tmp_ret_type);
    if (tmp_ret_type != NULL && ret_type == NULL) {
        std::cerr << "Runtime Error: tmp_ret_type != NULL && ret_type == NULL..." << std::endl;
        res = false;
    }
    else if (ret_type != NULL) {
        // TODO qualifiers
    }

    *node = SageBuilder::buildFunctionType(ret_type, param_type_list);

    return VisitType(function_no_proto_type, node) && res;
}

bool ClangToSageTranslator::VisitFunctionProtoType(clang::FunctionProtoType * function_proto_type, SgNode ** node) {
    bool res = true;
    SgFunctionParameterTypeList * param_type_list = new SgFunctionParameterTypeList();
    for (unsigned i = 0; i < function_proto_type->getNumArgs(); i++) {
        clang::QualType qual_param_type = function_proto_type->getArgType(i);
        SgNode * tmp_param_type = Traverse(qual_param_type.getTypePtr());
        SgType * param_type = isSgType(tmp_param_type);
        if (tmp_param_type != NULL && param_type == NULL) {
            std::cerr << "Runtime Error: tmp_param_type != NULL && param_type == NULL..." << std::endl;
            res = false;
            continue;
        }
        else if (param_type != NULL) {
            // TODO qualifiers
        }

        param_type_list->append_argument(param_type);
    }

    clang::QualType qual_ret_type = function_proto_type->getResultType();
    SgNode * tmp_ret_type = Traverse(qual_ret_type.getTypePtr());
    SgType * ret_type = isSgType(tmp_ret_type);
    if (tmp_ret_type != NULL && ret_type == NULL) {
        std::cerr << "Runtime Error: tmp_ret_type != NULL && ret_type == NULL..." << std::endl;
        res = false;
    }
    else if (ret_type != NULL) {
        // TODO qualifiers
    }

    *node = SageBuilder::buildFunctionType(ret_type, param_type_list);

    return VisitType(function_proto_type, node) && res;
}

bool ClangToSageTranslator::VisitParenType(clang::ParenType * paren_type, SgNode ** node) {
    clang::QualType qual_type = paren_type->getInnerType();

    const clang::Type * type = qual_type.getTypePtr();

    *node = Traverse(type);

    // TODO qualifiers

    return true;
}

bool ClangToSageTranslator::VisitPointerType(clang::PointerType * pointer_type, SgNode ** node) {
    clang::QualType qual_type = pointer_type->getPointeeType();

    SgNode * tmp_type = Traverse(qual_type.getTypePtr());
    SgType * type = isSgType(tmp_type);

    ROSE_ASSERT(type != NULL);

    *node = SageBuilder::buildPointerType(type);

    return VisitType(pointer_type, node);
}

