
#include <iostream>

#include "ClangFrontend-private.h"

#define DEBUG_VISITOR 1

int clang_main(int argc, char ** argv, SgSourceFile& sageFile) {
  // 0 - Analyse Cmd Line

    std::vector<std::string> inc_dirs_list;
    std::vector<std::string> define_list;
    std::string input_file;

    for (int i = 0; i < argc; i++) {
        std::string current_arg(argv[i]);
        if (current_arg.find("-I") == 0) {
            if (current_arg.length() > 2) {
                inc_dirs_list.push_back(current_arg.substr(2));
            }
            else {
                i++;
                if (i < argc)
                    inc_dirs_list.push_back(current_arg);
                else
                    break;
            }
        }
        else if (current_arg.find("-D") == 0) {
            if (current_arg.length() > 2) {
                define_list.push_back(current_arg.substr(2));
            }
            else {
                i++;
                if (i < argc)
                    define_list.push_back(current_arg);
                else
                    break;
            }
        }
        else if (current_arg.find("-c") == 0) {}
        else if (current_arg.find("-o") == 0) {
            if (current_arg.length() == 2) {
                i++;
                if (i >= argc) break;
            }
        }
        else {
            std::cerr << "argv[" << i << "] = " << current_arg << " is neither define or include dir. Use it as input file."  << std::endl;
            input_file = current_arg;
        }
    }

  // 1 - Create a compiler instance

    clang::CompilerInstance * compiler_instance = new clang::CompilerInstance();

  // 2 - Fill the compiler instance

    clang::TextDiagnosticPrinter * diag_printer = new clang::TextDiagnosticPrinter(llvm::errs(), clang::DiagnosticOptions());
    compiler_instance->createDiagnostics(argc, argv, diag_printer, true, false);

    clang::TargetOptions target_options;
    target_options.Triple = llvm::sys::getDefaultTargetTriple();
    clang::TargetInfo * target_info = clang::TargetInfo::CreateTargetInfo(compiler_instance->getDiagnostics(), target_options);
    compiler_instance->setTarget(target_info);

    compiler_instance->createFileManager();
    compiler_instance->createSourceManager(compiler_instance->getFileManager());

    unsigned cnt = define_list.size() + inc_dirs_list.size();
    char ** args = new char*[cnt];
    std::vector<std::string>::iterator it_str;
    unsigned i = 0;
    for (it_str = define_list.begin(); it_str != define_list.end(); it_str++) {
        args[i] = new char[it_str->size() + 3];
        args[i][0] = '-';
        args[i][0] = 'D';
        strcpy(&(args[i][2]), it_str->c_str());
        i++;
    }
    for (it_str = inc_dirs_list.begin(); it_str != inc_dirs_list.end(); it_str++) {
        args[i] = new char[it_str->size() + 3];
        args[i][0] = '-';
        args[i][1] = 'I';
        strcpy(&(args[i][2]), it_str->c_str());
        i++;
    }
    clang::CompilerInvocation::CreateFromArgs(compiler_instance->getInvocation(), args, &(args[cnt]), compiler_instance->getDiagnostics());

//    clang::PreprocessorOptions & preproc_opts = compiler_instance->getPreprocessorOpts();
    // TODO sys dir
    // ?TODO? fill 'preproc_opts' from 'define_list'

//    clang::HeaderSearchOptions & header_opts = compiler_instance->getHeaderSearchOpts();
    // ?TODO? add include dirs 'inc_dirs_list'
/*
    clang::LangOptions & lang_opts = compiler_instance->getLangOpts();
    size_t last_period = input_file.find_last_of(".");
    std::string extention(input_file.substr(last_period + 1));

    if (extention == "c") {
    }
    else if (extention == "C" || extention == "cxx" || extention == "cpp") {
        lang_opts.CPlusPlus = 1;
    }
    else if (extention == "objc") {
        ROSE_ASSERT(!"Objective-C is not supported by ROSE Compiler.");
    }
    else if (extention == "cu") {
        lang_opts.CUDA = 1;
        lang_opts.CPlusPlus = 1;
    }
    else if (extention == "ocl" || extention == "cl") {
        lang_opts.OpenCL = 1;
    }
*/

    const clang::FileEntry * input_file_entry = compiler_instance->getFileManager().getFile(input_file);
    compiler_instance->getSourceManager().createMainFileID(input_file_entry);

    compiler_instance->createPreprocessor();

    compiler_instance->createASTContext();


    ClangToSageTranslator translator(compiler_instance);
    compiler_instance->setASTConsumer(&translator);   

    compiler_instance->createSema(clang::TU_Complete, NULL);

    ROSE_ASSERT (compiler_instance->hasDiagnostics());
    ROSE_ASSERT (compiler_instance->hasTarget());
    ROSE_ASSERT (compiler_instance->hasFileManager());
    ROSE_ASSERT (compiler_instance->hasSourceManager());
    ROSE_ASSERT (compiler_instance->hasPreprocessor());
    ROSE_ASSERT (compiler_instance->hasASTContext());
    ROSE_ASSERT (compiler_instance->hasSema());

  // 3 - Translate

//    compiler_instance->getDiagnosticClient().BeginSourceFile(compiler_instance->getLangOpts(), &(compiler_instance->getPreprocessor()));
    clang::ParseAST(compiler_instance->getPreprocessor(), &translator, compiler_instance->getASTContext());
//    compiler_instance->getDiagnosticClient().EndSourceFile();

    SgGlobal * global_scope = translator.getGlobalScope();

  // 4 - Attach to the file

    if (sageFile.get_globalScope() != NULL) SageInterface::deleteAST(sageFile.get_globalScope());

    sageFile.set_globalScope(global_scope);

    global_scope->set_parent(&sageFile);

    size_t last_slash = input_file.find_last_of("/");
    //std::string file_name(input_file.substr(last_slash + 1));
    std::string file_name(input_file);

    Sg_File_Info * start_fi = new Sg_File_Info(file_name, 0, 0);
    Sg_File_Info * end_fi   = new Sg_File_Info(file_name, 0, 0);

    global_scope->set_startOfConstruct(start_fi);

    global_scope->set_endOfConstruct(end_fi);

  // 5 - Finish the AST (fixup phase)

    finishSageAST(translator);

    return 1;
}

void finishSageAST(ClangToSageTranslator & translator) {
    SgGlobal * global_scope = translator.getGlobalScope();

 // 1 - Label Statements: Move sub-statement after the label statement.

    std::vector<SgLabelStatement *> label_stmts = SageInterface::querySubTree<SgLabelStatement>(global_scope, V_SgLabelStatement);
    std::vector<SgLabelStatement *>::iterator label_stmt_it;
    for (label_stmt_it = label_stmts.begin(); label_stmt_it != label_stmts.end(); label_stmt_it++) {
        SgStatement * sub_stmt = (*label_stmt_it)->get_statement();
        if (!isSgNullStatement(sub_stmt)) {
            SgNullStatement * null_stmt = SageBuilder::buildNullStatement();
            (*label_stmt_it)->set_statement(null_stmt);
            null_stmt->set_parent(*label_stmt_it);
            SageInterface::insertStatementAfter(*label_stmt_it, sub_stmt);
        }
    }

 // 2 - Place Preprocessor informations
}

SgGlobal * ClangToSageTranslator::getGlobalScope() { return p_global_scope; }

ClangToSageTranslator::ClangToSageTranslator(clang::CompilerInstance * compiler_instance) :
    clang::ASTConsumer(),
    p_decl_translation_map(),
    p_stmt_translation_map(),
    p_type_translation_map(),
    p_global_scope(NULL),
    p_compiler_instance(compiler_instance),
    p_sage_preprocessor_recorder(new SagePreprocessorRecord(&(p_compiler_instance->getSourceManager())))
{}

ClangToSageTranslator::~ClangToSageTranslator() {
    delete p_sage_preprocessor_recorder;
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

    clang::FileID file_begin = p_compiler_instance->getSourceManager().getFileID(begin);
    clang::FileID file_end   = p_compiler_instance->getSourceManager().getFileID(end);

    bool inv_begin_line;
    bool inv_begin_col;
    bool inv_end_line;
    bool inv_end_col;

    unsigned ls = p_compiler_instance->getSourceManager().getSpellingLineNumber(begin, &inv_begin_line);
    unsigned cs = p_compiler_instance->getSourceManager().getSpellingColumnNumber(begin, &inv_begin_col);
    unsigned le = p_compiler_instance->getSourceManager().getSpellingLineNumber(end, &inv_end_line);
    unsigned ce = p_compiler_instance->getSourceManager().getSpellingColumnNumber(end, &inv_end_col);

    Sg_File_Info * start_fi;
    Sg_File_Info * end_fi;

    if (file_begin.isInvalid()) {
        std::string file = p_compiler_instance->getSourceManager().getFileEntryForID(file_begin)->getName();

        start_fi = new Sg_File_Info(file, ls, cs);
        end_fi   = new Sg_File_Info(file, le, ce);

        std::cerr << "Create FI for node in " << file << std::endl;
    }
    else {
        start_fi = Sg_File_Info::generateDefaultFileInfoForCompilerGeneratedNode();
        end_fi   = Sg_File_Info::generateDefaultFileInfoForCompilerGeneratedNode();

        start_fi->setCompilerGenerated();
        end_fi->setCompilerGenerated();

        std::cerr << "Create FI for compiler generated node" << std::endl;
    }

    if (located_node != NULL) {
        located_node->set_startOfConstruct(start_fi);
        located_node->set_endOfConstruct(end_fi);
    }
    else if (init_name != NULL) {
        init_name->set_startOfConstruct(start_fi);
        init_name->set_endOfConstruct(end_fi);
    }

}

SgSymbol * ClangToSageTranslator::GetSymbolFromSymbolTable(clang::NamedDecl * decl) {
    SgScopeStatement * scope = SageBuilder::topScopeStack();

    SgName name(decl->getNameAsString());

    std::list<SgScopeStatement *>::reverse_iterator it;
    SgSymbol * sym = NULL;
    switch (decl->getKind()) {
        case clang::Decl::Typedef:
        {
            it = SageBuilder::ScopeStack.rbegin();
            while (it != SageBuilder::ScopeStack.rend() && sym == NULL) {
                 sym = (*it)->lookup_typedef_symbol(name);
                 it++;
            }
            break;
        }
        case clang::Decl::Var:
        case clang::Decl::ParmVar:
        {
            it = SageBuilder::ScopeStack.rbegin();
            while (it != SageBuilder::ScopeStack.rend() && sym == NULL) {
                sym = (*it)->lookup_variable_symbol(name);
                it++;
            }
            break;
        }
        case clang::Decl::Function:
        {
            SgNode * tmp_type = Traverse(((clang::FunctionDecl *)decl)->getType().getTypePtr());
            SgFunctionType * type = isSgFunctionType(tmp_type);
            it = SageBuilder::ScopeStack.rbegin();
            while (it != SageBuilder::ScopeStack.rend() && sym == NULL) {
                sym = (*it)->lookup_function_symbol(name, type);
                it++;
            }
            break;
        }
        case clang::Decl::Field:
        {
            SgClassDeclaration * sg_class_decl = isSgClassDeclaration(Traverse(((clang::FieldDecl *)decl)->getParent()));
            ROSE_ASSERT(sg_class_decl != NULL);
            if (sg_class_decl->get_definingDeclaration() == NULL)
                std::cerr << "Runtime Error: cannot find the definition of the class/struct associate to the field: " << name << std::endl;
            else {
                scope = isSgClassDeclaration(sg_class_decl->get_definingDeclaration())->get_definition();
                // TODO: for C++, if 'scope' is in 'SageBuilder::ScopeStack': problem!!!
                while (scope != NULL && sym == NULL) {
                    sym = scope->lookup_variable_symbol(name);
                    scope = scope->get_scope();
                }
            }
            break;
        }
        case clang::Decl::Record:
        {
            it = SageBuilder::ScopeStack.rbegin();
            while (it != SageBuilder::ScopeStack.rend() && sym == NULL) {
                sym = (*it)->lookup_class_symbol(name);
                it++;
            }
            break;
        }
        case clang::Decl::Label:
        {
            // Should not be reach as we use Traverse to retrieve Label (they are "terminal" statements) (it avoids the problem of forward use of label: goto before declaration)
            name = SgName(((clang::LabelDecl *)decl)->getStmt()->getName());
            it = SageBuilder::ScopeStack.rbegin();
            while (it != SageBuilder::ScopeStack.rend() && sym == NULL) {
                sym = (*it)->lookup_label_symbol(name);
                it++;
            }
            break;
        }
        default:
            std::cerr << "Runtime Error: Unknown type of Decl." << std::endl;
    }

    return sym;
}

/*
SgSymbol * ClangToSageTranslator::GetSymbolFromSymbolTable(clang::TypeDecl * decl) {
    SgSymbol * sym = NULL;

    // TODO

    return sym;
}
*/

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
            ret_status = VisitTypedefDecl((clang::TypedefDecl *)decl, &result);
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
        case clang::Decl::Record:
            ret_status = VisitRecordDecl((clang::RecordDecl *)decl, &result);
            break;
        case clang::Decl::Field:
            ret_status = VisitFieldDecl((clang::FieldDecl *)decl, &result);
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
        case clang::Stmt::UnaryOperatorClass:
            ret_status = VisitUnaryOperator((clang::UnaryOperator *)stmt, &result);
            break;
        case clang::Stmt::ForStmtClass:
            ret_status = VisitForStmt((clang::ForStmt *)stmt, &result);
            break;
        case clang::Stmt::IfStmtClass:
            ret_status = VisitIfStmt((clang::IfStmt *)stmt, &result);
            break;
        case clang::Stmt::ReturnStmtClass:
            ret_status = VisitReturnStmt((clang::ReturnStmt *)stmt, &result);
            break;
        case clang::Stmt::BinaryOperatorClass:
        case clang::Stmt::CompoundAssignOperatorClass:
            ret_status = VisitBinaryOperator((clang::BinaryOperator *)stmt, &result);
            break;
        case clang::Stmt::ConditionalOperatorClass:
            ret_status = VisitConditionalOperator((clang::ConditionalOperator *)stmt, &result);
            break;
        case clang::Stmt::ArraySubscriptExprClass:
            ret_status = VisitArraySubscriptExpr((clang::ArraySubscriptExpr *)stmt, &result);
            break;
        case clang::Stmt::MemberExprClass:
            ret_status = VisitMemberExpr((clang::MemberExpr *)stmt, &result);
            break;
        case clang::Stmt::LabelStmtClass:
            ret_status = VisitLabelStmt((clang::LabelStmt *)stmt, &result);
            break;
        case clang::Stmt::NullStmtClass:
            ret_status = VisitNullStmt((clang::NullStmt *)stmt, &result);
            break;
        case clang::Stmt::GotoStmtClass:
            ret_status = VisitGotoStmt((clang::GotoStmt *)stmt, &result);
            break;
//        case clang::Stmt::ImplicitValueInitExprClass: break; // FIXME
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
        case clang::Type::Elaborated:
            ret_status = VisitElaboratedType((clang::ElaboratedType *)type, &result);
            break;
        case clang::Type::Record:
            ret_status = VisitRecordType((clang::RecordType *)type, &result);
            break;
        case clang::Type::Typedef:
            ret_status = VisitTypedefType((clang::TypedefType *)type, &result);
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
#if DEBUG_VISITOR
    std::cerr << "ClangToSageTranslator::VisitDecl" << std::endl;
#endif    
    if (*node == NULL) {
        std::cerr << "Runtime error: No Sage node associated with the declaration..." << std::endl;
        return false;
    }

    if (!isSgGlobal(*node))
        applySourceRange(*node, decl->getSourceRange());

    // TODO attributes

    return true;
}

bool ClangToSageTranslator::VisitRecordDecl(clang::RecordDecl * record_decl, SgNode ** node) {
#if DEBUG_VISITOR
    std::cerr << "ClangToSageTranslator::VisitRecordDecl" << std::endl;
#endif
    bool res = true;

    SgClassDeclaration * sg_class_decl = NULL;

    clang::RecordDecl * prev_record_decl = record_decl->getPreviousDecl();
    SgClassDeclaration * sg_prev_class_decl = isSgClassDeclaration(Traverse(prev_record_decl));

    SgClassDeclaration * sg_first_class_decl = sg_prev_class_decl == NULL ? NULL : isSgClassDeclaration(sg_prev_class_decl->get_firstNondefiningDeclaration());
    SgClassDeclaration * sg_def_class_decl = sg_prev_class_decl == NULL ? NULL : isSgClassDeclaration(sg_prev_class_decl->get_definingDeclaration());

    ROSE_ASSERT(sg_first_class_decl != NULL || sg_def_class_decl == NULL);

    SgName name(record_decl->getNameAsString());

    if (sg_first_class_decl == NULL || record_decl->field_empty()) {
        *node = SageBuilder::buildNondefiningClassDeclaration(name, SageBuilder::topScopeStack());
        sg_class_decl = isSgClassDeclaration(*node);
        if (record_decl->isAnonymousStructOrUnion()) sg_class_decl->set_isUnNamed(true);
        sg_class_decl->set_parent(SageBuilder::topScopeStack());
        if (sg_first_class_decl == NULL) {
            sg_first_class_decl = sg_class_decl;
            sg_first_class_decl->set_firstNondefiningDeclaration(sg_first_class_decl);
        }
        else {
            sg_class_decl->set_firstNondefiningDeclaration(sg_first_class_decl);
            sg_class_decl->set_definingDeclaration(sg_def_class_decl);
        }
    }
    else {
        *node = SageBuilder::buildDefiningClassDeclaration(name, SageBuilder::topScopeStack());
        sg_class_decl = isSgClassDeclaration(*node);
        if (record_decl->isAnonymousStructOrUnion()) sg_class_decl->set_isUnNamed(true);
        sg_class_decl->set_parent(SageBuilder::topScopeStack());
        sg_class_decl->set_firstNondefiningDeclaration(sg_first_class_decl);
        if (sg_def_class_decl != NULL) {
            // FIXME second definition
            sg_class_decl->set_definingDeclaration(sg_def_class_decl);
        }
        else {
            sg_def_class_decl = sg_class_decl;
            sg_first_class_decl->set_definingDeclaration(sg_def_class_decl);
        }
    }

    switch (record_decl->getTagKind()) {
        case clang::TTK_Struct:
            sg_class_decl->set_class_type(SgClassDeclaration::e_struct);
            break;
        case clang::TTK_Class:
            sg_class_decl->set_class_type(SgClassDeclaration::e_class);
            break;
        case clang::TTK_Union:
            sg_class_decl->set_class_type(SgClassDeclaration::e_union);
            break;
        default:
            std::cerr << "Runtime error: RecordDecl can only be a struct/class/union." << std::endl;
            res = false;
    }

    if (!record_decl->field_empty()) {
        if (sg_first_class_decl == sg_class_decl) {
            *node = SageBuilder::buildDefiningClassDeclaration(name, SageBuilder::topScopeStack());
            sg_class_decl = isSgClassDeclaration(*node);
            if (record_decl->isAnonymousStructOrUnion()) sg_class_decl->set_isUnNamed(true);
            sg_class_decl->set_parent(SageBuilder::topScopeStack());
            sg_class_decl->set_firstNondefiningDeclaration(sg_first_class_decl);
            sg_class_decl->set_class_type(sg_first_class_decl->get_class_type());
            sg_def_class_decl = sg_class_decl;
            sg_first_class_decl->set_definingDeclaration(sg_def_class_decl);
            sg_first_class_decl->setCompilerGenerated();
        }

        SgClassDefinition * sg_class_def = isSgClassDefinition(sg_class_decl->get_definition());
        if (sg_class_def == NULL) {
            sg_class_def = SageBuilder::buildClassDefinition(sg_class_decl);
            sg_class_def->set_parent(sg_class_decl);
        }

        SageBuilder::pushScopeStack(sg_class_def);

        clang::RecordDecl::field_iterator it;
        for (it = record_decl->field_begin(); it != record_decl->field_end(); it++) {
            SgNode * tmp_field = Traverse(*it);
            SgDeclarationStatement * field_decl = isSgDeclarationStatement(tmp_field);
            ROSE_ASSERT(field_decl != NULL);
            sg_class_def->append_member(field_decl);
        }

        SageBuilder::popScopeStack();
    }

    return VisitDecl(record_decl, node) && res;
}

bool ClangToSageTranslator::VisitTypedefDecl(clang::TypedefDecl * typedef_decl, SgNode ** node) {
#if DEBUG_VISITOR
    std::cerr << "ClangToSageTranslator::VisitTypedefDecl" << std::endl;
#endif
    bool res = true;

    SgName name(typedef_decl->getNameAsString());

    SgNode * tmp_type = Traverse(typedef_decl->getUnderlyingType().getTypePtr());
    SgType * type = isSgType(tmp_type);
    if (type == NULL) {
        std::cerr << "Runtime Error: Cannot retrieve the base type of the typedef: " << name << std::endl;
        res = false;
    }

    *node = SageBuilder::buildTypedefDeclaration_nfi(name, type, SageBuilder::topScopeStack());

    return VisitDecl(typedef_decl, node) && res;
}

bool ClangToSageTranslator::VisitFieldDecl(clang::FieldDecl * field_decl, SgNode ** node) {
#if DEBUG_VISITOR
    std::cerr << "ClangToSageTranslator::VisitFieldDecl" << std::endl;
#endif  
    bool res = true;
    
    SgName name(field_decl->getNameAsString());

    clang::QualType qual_type = field_decl->getType();
    SgNode * tmp_type = Traverse(qual_type.getTypePtr());
    SgType * type = isSgType(tmp_type);
    if (tmp_type != NULL && type == NULL) {
        std::cerr << "Runtime error: tmp_type != NULL && type == NULL" << std::endl;
        res = false;
    }
    else if (type != NULL) {
        // TODO qualifiers
    }

    clang::Expr * init_expr = field_decl->getInClassInitializer();
    SgNode * tmp_init = Traverse(init_expr);
    SgExpression * expr = isSgExpression(tmp_init);
    if (tmp_init != NULL && expr == NULL) {
        std::cerr << "Runtime error: not a SgInitializer..." << std::endl; // TODO
        res = false;
    }
    SgInitializer * init = expr != NULL ? SageBuilder::buildAssignInitializer_nfi(expr, expr->get_type()) : NULL;
    if (init != NULL)
        applySourceRange(init, init_expr->getSourceRange());

    *node = SageBuilder::buildVariableDeclaration(name, type, init, SageBuilder::topScopeStack());
    (*node)->set_parent(SageBuilder::topScopeStack());

    return VisitDecl(field_decl, node) && res; 
}

bool ClangToSageTranslator::VisitFunctionDecl(clang::FunctionDecl * function_decl, SgNode ** node) {
#if DEBUG_VISITOR
    std::cerr << "ClangToSageTranslator::VisitFunctionDecl" << std::endl;
#endif
    bool res = true;

    // FIXME: There is something weird here when try to Traverse a function reference in a recursive function (when first Traverse is not complete)
    //        It seems that it tries to instantiate the decl inside the function...
    //        It may be faster to recode from scratch...

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

        SgInitializedNamePtrList & init_names = param_list->get_args();
        SgInitializedNamePtrList::iterator it;
        for (it = init_names.begin(); it != init_names.end(); it++) {
            (*it)->set_scope(function_definition);
            SgSymbolTable * st = function_definition->get_symbol_table();
            ROSE_ASSERT(st != NULL);
            SgVariableSymbol * tmp_sym  = new SgVariableSymbol(*it);
            st->insert((*it)->get_name(), tmp_sym);
            tmp_sym->set_parent(st);
        }


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
#if DEBUG_VISITOR
    std::cerr << "ClangToSageTranslator::VisitVarDecl" << std::endl;
#endif
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
#if DEBUG_VISITOR
    std::cerr << "ClangToSageTranslator::VisitParmVarDecl" << std::endl;
#endif
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
#if DEBUG_VISITOR
    std::cerr << "ClangToSageTranslator::VisitTranslationUnitDecl" << std::endl;
#endif
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
            std::cerr << "    class = " << child->class_name() << std::endl;
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

bool ClangToSageTranslator::VisitConditionalOperator(clang::ConditionalOperator * conditional_operator, SgNode ** node) {
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

    return VisitExpr(conditional_operator, node) && res;
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

bool ClangToSageTranslator::VisitArraySubscriptExpr(clang::ArraySubscriptExpr * array_subscript_expr, SgNode ** node) {
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

    //SgNode * tmp_node = Traverse(decl_ref_expr->getDecl());
    // DONE: Do not use Traverse(...) as the declaration can not be complete (recursive functions)
    //       Instead use SymbolTable from ROSE as the symbol should be ready (cannot have a reference before the declaration)
    // FIXME: This fix will not work for C++ (methods/fields can be use before they are declared...)

    SgSymbol * sym = GetSymbolFromSymbolTable(decl_ref_expr->getDecl());

    if (sym == NULL) {
        std::cerr << "Runtime error: Cannot find the symbol for a declaration reference." << std::endl;
        res = false;
    }

    SgVariableSymbol * var_sym  = isSgVariableSymbol(sym);
    SgFunctionSymbol * func_sym = isSgFunctionSymbol(sym);

    bool successful_cast = var_sym || func_sym;
    if (sym != NULL && !successful_cast) {
        std::cerr << "Runtime error: Unknown type of symbol for a declaration reference." << std::endl;
        std::cerr << "    sym->class_name() = " << sym->class_name()  << std::endl;
        res = false;
    }
    else if (var_sym != NULL) {
        *node = SageBuilder::buildVarRefExp(var_sym);
    }
    else if (func_sym != NULL) {
        *node = SageBuilder::buildFunctionRefExp(func_sym);
    }

    return VisitExpr(decl_ref_expr, node) && res;
}

bool ClangToSageTranslator::VisitIntegerLiteral(clang::IntegerLiteral * integer_literal, SgNode ** node) {
//  *node = SageBuilder::buildIntVal_nfi(integer_literal->getValue().getSExtValue());
    *node = SageBuilder::buildIntVal(integer_literal->getValue().getSExtValue());

    return VisitExpr(integer_literal, node);
}

bool ClangToSageTranslator::VisitMemberExpr(clang::MemberExpr * member_expr, SgNode ** node) {
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

bool ClangToSageTranslator::VisitStringLiteral(clang::StringLiteral * string_literal, SgNode ** node) {
//  *node = SageBuilder::buildStringVal_nfi(string_literal->getString().str());
    *node = SageBuilder::buildStringVal(string_literal->getString().str());

    return VisitExpr(string_literal, node);
}

bool ClangToSageTranslator::VisitUnaryOperator(clang::UnaryOperator * unary_operator, SgNode ** node) {
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
        case clang::UO_Imag:
        case clang::UO_Extension:
        default:
            std::cerr << "Runtime error: Unknown unary operator." << std::endl;
            res = false;
    }

    return VisitStmt(unary_operator, node) && res;
}

bool ClangToSageTranslator::VisitForStmt(clang::ForStmt * for_stmt, SgNode ** node) {
    bool res = true;

//    *node = SageBuilder::buildForStatement_nfi(init_stmt, SageBuilder::buildExprStatement(cond), inc, NULL);
    *node = SageBuilder::buildForStatement_nfi((SgForInitStatement *)NULL, NULL, NULL, NULL);

    SageBuilder::pushScopeStack(isSgScopeStatement(*node));

    SgNode * tmp_init = Traverse(for_stmt->getInit());
    SgStatement * init_stmt = isSgStatement(tmp_init);
    SgExpression * init_expr = isSgExpression(tmp_init);
    if (tmp_init != NULL && init_stmt == NULL) {
        if (init_expr == NULL) {
            std::cerr << "Runtime error: tmp_init != NULL && init_stmt == NULL && init_expr == NULL (" << tmp_init->class_name() << ")" << std::endl;
            res = false;
        }
        else {
            init_stmt = SageBuilder::buildExprStatement(init_expr);
            applySourceRange(init_stmt, for_stmt->getInit()->getSourceRange());
        }
    }

    SgNode * tmp_cond = Traverse(for_stmt->getCond());
    SgExpression * cond = isSgExpression(tmp_cond);
    if (tmp_cond != NULL && cond == NULL) {
        std::cerr << "Runtime error: tmp_cond != NULL && cond == NULL" << std::endl;
        res = false;
    }
    SgStatement * cond_stmt = SageBuilder::buildExprStatement(cond);

    SgNode * tmp_inc  = Traverse(for_stmt->getInc());
    SgExpression * inc = isSgExpression(tmp_inc);
    if (tmp_inc != NULL && inc == NULL) {
        std::cerr << "Runtime error: tmp_inc != NULL && inc == NULL" << std::endl;
        res = false;
    }

    SgNode * tmp_body = Traverse(for_stmt->getBody());
    SgStatement * body = isSgStatement(tmp_body);
    if (body == NULL) {
        SgExpression * body_expr = isSgExpression(tmp_body);
        if (body_expr != NULL)
            body = SageBuilder::buildExprStatement(body_expr);
    }
    if (tmp_body != NULL && body == NULL) {
        std::cerr << "Runtime error: tmp_body != NULL && body == NULL" << std::endl;
        res = false;
    }

    SageBuilder::popScopeStack();

    SgStatementPtrList for_init_stmt_list;
    for_init_stmt_list.push_back(init_stmt);
    SgForInitStatement * for_init_stmt = SageBuilder::buildForInitStatement_nfi(for_init_stmt_list);
//    applySourceRange(for_init_stmt, for_stmt->getInit()->getSourceRange());

    for_init_stmt->set_parent(*node);
    if (isSgForStatement(*node)->get_for_init_stmt() != NULL)
        SageInterface::deleteAST(isSgForStatement(*node)->get_for_init_stmt());
    isSgForStatement(*node)->set_for_init_stmt(for_init_stmt);

    cond_stmt->set_parent(*node);
    isSgForStatement(*node)->set_test(cond_stmt);

    inc->set_parent(*node);
    isSgForStatement(*node)->set_increment(inc);

    body->set_parent(*node);
    isSgForStatement(*node)->set_loop_body(body);

    applySourceRange(isSgForStatement(*node)->get_for_init_stmt(), for_stmt->getInit()->getSourceRange());
    applySourceRange(isSgForStatement(*node)->get_test(), for_stmt->getCond()->getSourceRange());

    return VisitStmt(for_stmt, node) && res;
}

bool ClangToSageTranslator::VisitGotoStmt(clang::GotoStmt * goto_stmt, SgNode ** node) {
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
    bool res = true;

    // TODO if_stmt->getConditionVariable()

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

bool ClangToSageTranslator::VisitLabelStmt(clang::LabelStmt * label_stmt, SgNode ** node) {
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
    *node = SageBuilder::buildNullStatement_nfi();
    return VisitStmt(null_stmt, node);
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
            std::cerr << "Unknown builtin type: " << builtin_type->getName(p_compiler_instance->getLangOpts()) << " !" << std::endl;
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

    return VisitType(paren_type, node);
}

bool ClangToSageTranslator::VisitPointerType(clang::PointerType * pointer_type, SgNode ** node) {
    clang::QualType qual_type = pointer_type->getPointeeType();

    SgNode * tmp_type = Traverse(qual_type.getTypePtr());
    SgType * type = isSgType(tmp_type);

    ROSE_ASSERT(type != NULL);

    *node = SageBuilder::buildPointerType(type);

    return VisitType(pointer_type, node);
}

bool ClangToSageTranslator::VisitRecordType(clang::RecordType * record_type, SgNode ** node) {
    bool res = true;

    SgSymbol * sym = GetSymbolFromSymbolTable(record_type->getDecl());

    SgClassSymbol * class_sym = isSgClassSymbol(sym);

    if (class_sym == NULL) {
        std::cerr << "Runtime Error: Cannot find a class symbol for the RecordType." << std::endl;
        res = false;
    }

    *node = class_sym->get_type();

    return VisitType(record_type, node) && res;
}

bool ClangToSageTranslator::VisitTypedefType(clang::TypedefType * typedef_type, SgNode ** node) {
    bool res = true;

    SgSymbol * sym = GetSymbolFromSymbolTable(typedef_type->getDecl());
    SgTypedefSymbol * tdef_sym = isSgTypedefSymbol(sym);

    if (tdef_sym == NULL) {
        std::cerr << "Runtime Error: Cannot find a typedef symbol for the TypedefType." << std::endl;
        res = false;
    }

    *node = tdef_sym->get_type();

   return VisitType(typedef_type, node) && res;
}

bool ClangToSageTranslator::VisitElaboratedType(clang::ElaboratedType * elaborated_type, SgNode ** node) {
    clang::QualType qual_type = elaborated_type->getNamedType();

    SgNode * tmp_type = Traverse(qual_type.getTypePtr());
    SgType * type = isSgType(tmp_type);

    ROSE_ASSERT(type != NULL);

    // TODO clang::ElaboratedType contains the "sugar" of a type reference (eg, "struct A" or "M::N::A"), it should be pass down to ROSE

    *node = type;

    // TODO qualifiers

    return VisitType(elaborated_type, node);
}

std::pair<Sg_File_Info *, PreprocessingInfo *> ClangToSageTranslator::preprocessor_top() {
    return p_sage_preprocessor_recorder->top();
}

bool ClangToSageTranslator::preprocessor_pop() {
    return p_sage_preprocessor_recorder->pop();
}

// struct NextPreprocessorToInsert

NextPreprocessorToInsert::NextPreprocessorToInsert(ClangToSageTranslator & translator_) :
  cursor(NULL),
  candidat(NULL),
  next_to_insert(NULL),
  translator(translator_)
{}

NextPreprocessorToInsert * NextPreprocessorToInsert::next() {
    if (!translator.preprocessor_pop()) return NULL;

    NextPreprocessorToInsert * res = new NextPreprocessorToInsert(translator);

    std::pair<Sg_File_Info *, PreprocessingInfo *> next = translator.preprocessor_top();
    res->cursor = next.first;
    res->next_to_insert = next.second;
    res->candidat = candidat;
}

// class

NextPreprocessorToInsert * PreprocessorInserter::evaluateInheritedAttribute(SgNode * astNode, NextPreprocessorToInsert * inheritedValue) {
    SgLocatedNode * loc_node = isSgLocatedNode(astNode);
    if (loc_node == NULL) return inheritedValue;

    Sg_File_Info * current_pos = loc_node->get_startOfConstruct();

    bool passed_cursor = *current_pos > *(inheritedValue->cursor);

    if (passed_cursor) {
        // TODO insert on inheritedValue->candidat
        return inheritedValue->next();
    }

    
}

// class SagePreprocessorRecord

SagePreprocessorRecord::SagePreprocessorRecord(clang::SourceManager * source_manager) :
  p_source_manager(source_manager),
  p_preprocessor_record_list()
{}

void SagePreprocessorRecord::InclusionDirective(clang::SourceLocation HashLoc, const clang::Token & IncludeTok, llvm::StringRef FileName, bool IsAngled,
                                                const clang::FileEntry * File, clang::SourceLocation EndLoc, llvm::StringRef SearchPath, llvm::StringRef RelativePath) {
    std::cerr << "InclusionDirective" << std::endl;

    bool inv_begin_line;
    bool inv_begin_col;

    unsigned ls = p_source_manager->getSpellingLineNumber(HashLoc, &inv_begin_line);
    unsigned cs = p_source_manager->getSpellingColumnNumber(HashLoc, &inv_begin_col);

    std::string file = p_source_manager->getFileEntryForID(p_source_manager->getFileID(HashLoc))->getName();

    std::cerr << "    In file  : " << file << std::endl;
    std::cerr << "    From     : " << ls << ":" << cs << std::endl;
    std::cerr << "    Included : " << FileName.str() << std::endl;
    std::cerr << "    Is angled: " << (IsAngled ? "T" : "F") << std::endl;

    Sg_File_Info * file_info = new Sg_File_Info(file, ls, cs);
    PreprocessingInfo * preproc_info = new PreprocessingInfo(
                                                  PreprocessingInfo::CpreprocessorIncludeDeclaration,
                                                  FileName.str(),
                                                  file,
                                                  ls,
                                                  cs,
                                                  0,
                                                  PreprocessingInfo::before
                                           );

    p_preprocessor_record_list.push_back(std::pair<Sg_File_Info *, PreprocessingInfo *>(file_info, preproc_info));
}

void SagePreprocessorRecord::EndOfMainFile() {
    std::cerr << "EndOfMainFile" << std::endl;
    ROSE_ASSERT(false);
}

void SagePreprocessorRecord::Ident(clang::SourceLocation Loc, const std::string & str) {
    std::cerr << "Ident" << std::endl;
    ROSE_ASSERT(false);
}

void SagePreprocessorRecord::PragmaComment(clang::SourceLocation Loc, const clang::IdentifierInfo * Kind, const std::string & Str) {
    std::cerr << "PragmaComment" << std::endl;
    ROSE_ASSERT(false);
}

void SagePreprocessorRecord::PragmaMessage(clang::SourceLocation Loc, llvm::StringRef Str) {
    std::cerr << "PragmaMessage" << std::endl;
    ROSE_ASSERT(false);
}

void SagePreprocessorRecord::PragmaDiagnosticPush(clang::SourceLocation Loc, llvm::StringRef Namespace) {
    std::cerr << "PragmaDiagnosticPush" << std::endl;
    ROSE_ASSERT(false);
}

void SagePreprocessorRecord::PragmaDiagnosticPop(clang::SourceLocation Loc, llvm::StringRef Namespace) {
    std::cerr << "PragmaDiagnosticPop" << std::endl;
    ROSE_ASSERT(false);
}

void SagePreprocessorRecord::PragmaDiagnostic(clang::SourceLocation Loc, llvm::StringRef Namespace, clang::diag::Mapping mapping, llvm::StringRef Str) {
    std::cerr << "PragmaDiagnostic" << std::endl;
    ROSE_ASSERT(false);
}

void SagePreprocessorRecord::MacroExpands(const clang::Token & MacroNameTok, const clang::MacroInfo * MI, clang::SourceRange Range) {
    std::cerr << "MacroExpands" << std::endl;
    ROSE_ASSERT(false);
}

void SagePreprocessorRecord::MacroDefined(const clang::Token & MacroNameTok, const clang::MacroInfo * MI) {
    std::cerr << "" << std::endl;
    ROSE_ASSERT(false);
}

void SagePreprocessorRecord::MacroUndefined(const clang::Token & MacroNameTok, const clang::MacroInfo * MI) {
    std::cerr << "MacroUndefined" << std::endl;
    ROSE_ASSERT(false);
}

void SagePreprocessorRecord::Defined(const clang::Token & MacroNameTok) {
    std::cerr << "Defined" << std::endl;
    ROSE_ASSERT(false);
}

void SagePreprocessorRecord::SourceRangeSkipped(clang::SourceRange Range) {
    std::cerr << "SourceRangeSkipped" << std::endl;
    ROSE_ASSERT(false);
}

void SagePreprocessorRecord::If(clang::SourceRange Range) {
    std::cerr << "If" << std::endl;
    ROSE_ASSERT(false);
}

void SagePreprocessorRecord::Elif(clang::SourceRange Range) {
    std::cerr << "Elif" << std::endl;
    ROSE_ASSERT(false);
}

void SagePreprocessorRecord::Ifdef(const clang::Token & MacroNameTok) {
    std::cerr << "Ifdef" << std::endl;
    ROSE_ASSERT(false);
}

void SagePreprocessorRecord::Ifndef(const clang::Token & MacroNameTok) {
    std::cerr << "Ifndef" << std::endl;
    ROSE_ASSERT(false);
}

void SagePreprocessorRecord::Else() {
    std::cerr << "Else" << std::endl;
    ROSE_ASSERT(false);
}

void SagePreprocessorRecord::Endif() {
    std::cerr << "Endif" << std::endl;
    ROSE_ASSERT(false);
}

std::pair<Sg_File_Info *, PreprocessingInfo *> SagePreprocessorRecord::top() {
    return p_preprocessor_record_list.front();
}

bool SagePreprocessorRecord::pop() {
    p_preprocessor_record_list.erase(p_preprocessor_record_list.begin());
    return !p_preprocessor_record_list.empty();
}

