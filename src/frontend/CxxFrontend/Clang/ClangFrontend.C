
#include <iostream>

#include "ClangFrontend-private.h"

int clang_main(int argc, char* argv[], SgSourceFile& sageFile) {
    ClangToSageTranslator translator(std::vector<std::string>(argc, argv));

    return 1;
}

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

    if (located_node == NULL) {
        std::cerr << "Consistency error: try to apply a source range to a Sage node which are not a SgLocatedNode." << std::endl;
        exit(-1);
    }

    // TODO
}

/* Overload of ASTConsumer::HandleTranslationUnit, it is the "entry point" */

void ClangToSageTranslator::HandleTranslationUnit(clang::ASTContext & ast_context) {


    Traverse(ast_context.getTranslationUnitDecl());
}

/* Overload of Traverse{Decl|Stmt|Type} methods to prevent multiple traversal */

SgNode * ClangToSageTranslator::Traverse(clang::Decl * decl) {
    if (decl == NULL)
        return NULL;

    std::map<clang::Decl *, SgNode *>::iterator it = p_decl_translation_map.find(decl);
    if (it != p_decl_translation_map.end())
        return it->second;

    SgNode * result = NULL;

    switch (decl->getKind()) {
        // TODO cases
        default:
            std::cerr << "Unknown declacaration kind !" << std::endl;
            exit(-1);
    }

    return result;
}

SgNode * ClangToSageTranslator::Traverse(clang::Stmt * stmt) {
    if (stmt == NULL)
        return NULL;

    std::map<clang::Stmt *, SgNode *>::iterator it = p_stmt_translation_map.find(stmt);
    if (it != p_stmt_translation_map.end())
        return it->second; 

    SgNode * result = NULL;

    switch (stmt->getStmtClass()) {
        //TODO cases
        default:
            std::cerr << "Unknown statement kind !" << std::endl;
            exit(-1);
    }

    return result;
}

SgNode * ClangToSageTranslator::Traverse(const clang::Type * type) {
    if (type == NULL)
        return NULL;

    std::map<const clang::Type *, SgNode *>::iterator it = p_type_translation_map.find(type);
    if (it != p_type_translation_map.end()) 
         return it->second;

    SgNode * result = NULL;

    switch (type->getTypeClass()) {
        // TODO cases
        default:
            std::cerr << "Unknown type kind !" << std::endl;
            exit(-1);
    }

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

    applySourceRange(*node, decl->getSourceRange());

    // TODO attributes

    return true;
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

  // Traverse the child 

    clang::DeclContext * decl_context = (clang::DeclContext *)translation_unit_decl; // useless but more clear

    clang::DeclContext::decl_iterator it;
    for (it = decl_context->decls_begin(); it != decl_context->decls_end(); it++) {
        SgNode * child = Traverse(*it);
        SgDeclarationStatement * decl_stmt = isSgDeclarationStatement(child);
        if (decl_stmt == NULL) {
            std::cerr << "Runtime error: the node produce for a clang::Decl is not a SgDeclarationStatement !" << std::endl;
            return false;
        }
        p_global_scope->append_declaration(decl_stmt);
    }

  // Traverse the class hierarchy

    return VisitDecl(translation_unit_decl, node);
}

/********************/
/* Visit Statements */
/********************/

bool ClangToSageTranslator::VisitStmt(clang::Stmt * stmt, SgNode ** node) { return true; }

/***************/
/* Visit Types */
/***************/

bool ClangToSageTranslator::VisitType(clang::Type * type, SgNode ** node) { return true; }

