
#include "DotConsumer.h"

#include <string>

#include "ClangFrontend.h"

#include "clang/AST/ParentMap.h"

#include "clang/Basic/Builtins.h"
#include "clang/Basic/IdentifierTable.h"
#include "clang/Basic/FileManager.h"
#include "clang/Basic/TargetInfo.h"
#include "clang/Basic/TargetOptions.h"

#include "clang/Lex/HeaderSearch.h"
#include "clang/Lex/Preprocessor.h"

#include "clang/Parse/ParseAST.h"

#include "llvm/Config/config.h"

// Module's function

int generateDot(std::string input, std::string output) {
    clang::FileSystemOptions file_options;

    clang::FileManager file_manager(file_options);

    clang::DiagnosticIDs * diag_id = new clang::DiagnosticIDs();

    const llvm::IntrusiveRefCntPtr<clang::DiagnosticIDs> diag_id_ptr(diag_id);

    clang::Diagnostic diagnostic(diag_id_ptr);
        diagnostic.setSuppressAllDiagnostics(true);

    clang::SourceManager source_manager(diagnostic, file_manager);

    clang::HeaderSearch header_search(file_manager);

    clang::LangOptions lang_options;

    clang::TargetOptions target_options;
        target_options.Triple = LLVM_HOSTTRIPLE;
        target_options.ABI = "";
        target_options.CPU = "";
        target_options.Features.clear();

    clang::TargetInfo * target_info = clang::TargetInfo::CreateTargetInfo(diagnostic, target_options);

    clang::Preprocessor preprocessor(
        diagnostic,
        lang_options,
        *target_info,
        source_manager,
        header_search
    );

    std::string errors;
    llvm::raw_fd_ostream * out_stream = new llvm::raw_fd_ostream(output.c_str(), errors);


    DotConsumer * consumer = new DotConsumer(input, out_stream, diagnostic, lang_options);

    clang::IdentifierTable identifier_table(lang_options);

    clang::SelectorTable selector_table;

    clang::Builtin::Context builtin_context(*target_info);

    clang::ASTContext context(
        lang_options,
        source_manager,
        *target_info,
        identifier_table,
        selector_table,
        builtin_context,
        0
    );

    const clang::FileEntry * input_file_id = file_manager.getFile(input);

    source_manager.createMainFileID(input_file_id);

    clang::ParseAST(preprocessor, consumer, context, false, true, NULL);

    delete out_stream;
    delete consumer;
}

// Parse DeclContext: It is an infinite loop ...

void traverseASTusingDeclContext(clang::DeclContext * decl_context, std::string indent = std::string()) {
    std::cout << indent << decl_context->getDeclKindName() << std::endl;
    clang::DeclContext::decl_iterator decl_it;
    for (decl_it = decl_context->decls_begin(); decl_it != decl_context->decls_end(); decl_it++) {
        traverseASTusingDeclContext((*decl_it)->getDeclContext(), indent + "\t");
    }
}

// Traverse AST:

void traverseAST(clang::Stmt * stmt, std::string indent) {
    if (clang::Expr * expr = dyn_cast<clang::Expr>(stmt)) {
        std::cout << indent << "NYI: Expr traversal." << std::endl;
    }
    // FIXME do I need a IF for clang::SwitchCase (have only 2 inherited class)
    else {
        switch (stmt->getStmtClass()) {
            case clang::Stmt::DeclStmtClass:
            {
                clang::DeclStmt * decl_stmt = dyn_cast<clang::DeclStmt>(stmt);
                std::cout << indent << "Declaration Statement:" << std::endl;
                clang::DeclStmt::decl_iterator decl_it;
                for (decl_it = decl_stmt->decl_begin(); decl_it != decl_stmt->decl_end(); decl_it++)
                    traverseAST(*decl_it, indent + "\t\t");
                break;
            }
            case clang::Stmt::CompoundStmtClass:
            {
                 clang::CompoundStmt * compound_stmt = dyn_cast<clang::CompoundStmt>(stmt);
                 if (compound_stmt == NULL) exit(0);
                 std::cout << indent << "Compound Statement:" << std::endl;
                 clang::CompoundStmt::body_iterator body_it;
                 for (body_it = compound_stmt->body_begin(); body_it != compound_stmt->body_end(); body_it++)
                     traverseAST(*body_it, indent + "\t\t");
                break;
            }
            default:
                std::cout << indent << "Unknown statement: " << stmt->getStmtClassName() << std::endl;
        }
    }
}

void traverseAST(clang::Decl * decl, std::string indent) {
    if (clang::NamedDecl * named_decl = dyn_cast<clang::NamedDecl>(decl)) {
        if (clang::ValueDecl * value_decl = dyn_cast<clang::ValueDecl>(named_decl)) {
            if (clang::DeclaratorDecl * declarator_decl = dyn_cast<clang::DeclaratorDecl>(value_decl)) {
                if (clang::FunctionDecl * function_decl = dyn_cast<clang::FunctionDecl>(declarator_decl)) {
                    std::cout << indent << "Function declaration:" << std::endl;
                        std::cout << indent << ">\t" << "Parameters:" << std::endl;
                        clang::FunctionDecl::param_iterator param_it;
                        for (param_it = function_decl->param_begin(); param_it != function_decl->param_end(); param_it++) {
                            clang::ParmVarDecl * param = *param_it;
                            traverseAST(param, indent + "\t\t");
                        }
                        if (function_decl->hasBody()) {
                            clang::Stmt * body = function_decl->getBody();
                            std::cout << indent << ">\t" << "Body:" << std::endl;
                            traverseAST(body, indent + "\t\t");
                        }
                }
                else if (clang::VarDecl * var_decl = dyn_cast<clang::VarDecl>(declarator_decl)) {
                    if (clang::ParmVarDecl * param_var_decl = dyn_cast<clang::ParmVarDecl>(var_decl)) {
                        std::cout << indent << "Parameter Variable Declaration:" << std::endl;
                        // TODO print info from clang::ParmVarDecl
                    }
                    else if (clang::ImplicitParamDecl * implicit_param_decl = dyn_cast<clang::ImplicitParamDecl>(var_decl)) {
                       std::cout << indent << "Unsupported VarDecl (only ParmVarDecl): " << value_decl->getDeclKindName() << std::endl;
                    }
                    else {
                        std::cout << indent << "Variable Declaration:" << std::endl;
                    }
                    if (var_decl->hasInit()) {
                        std::cout << indent << ">\t" << "Initializer:" << std::endl;
                        traverseAST(var_decl->getInit(), indent + "\t\t");
                    }
                }
                else {
                    std::cout << indent << "Unknown DeclaratorDecl (only FunctionDecl and VarDecl): " << value_decl->getDeclKindName() << std::endl;
                }
                // TODO print info from clang::DeclaratorDecl
            }
            else {
                std::cout << indent << "Unknown ValueDecl (only DeclaratorDecl): " << value_decl->getDeclKindName() << std::endl;
            }
            std::cout << indent << ">\t" << "Type: " << clang::QualType::getAsString(value_decl->getType().split()) << std::endl;
        }
        else if (clang::TypeDecl * type_decl = dyn_cast<clang::TypeDecl>(named_decl)) {
            // TODO
        }
        else {
            std::cout << indent << "Unknown NamedDecl (only ValueDecl and TypeDecl): " << named_decl->getDeclKindName() << std::endl;
        }
        std::cout << indent << ">\t" << "Name: " << named_decl->getName().str() << " (" << named_decl->getNameAsString() << ")" << std::endl;
    }
    else {
        std::cout << indent << "Unknown Decl (only NamedDecl): " << decl->getDeclKindName() << std::endl;
    }
}

// AST Consumer generating a GraphViz file

DotConsumer::DotConsumer(std::string & input, llvm::raw_ostream * output, clang::Diagnostic & diagnostic, const clang::LangOptions & lang_options) :
    clang::ASTConsumer(),
    p_diagnostic(diagnostic),
    p_lang_options(lang_options),
    p_input_file(input),
    p_output_file(output)
{}

DotConsumer::~DotConsumer() {}

void DotConsumer::Initialize(clang::ASTContext & context) {
    p_ast_context = &context;
    p_source_management = &(p_ast_context->getSourceManager());
    p_translation_unit_decl = p_ast_context->getTranslationUnitDecl();

    p_main_file_id = p_source_management->getMainFileID();

    const llvm::MemoryBuffer * main_buffer = p_source_management->getBuffer(p_main_file_id);

    p_main_file_start = main_buffer->getBufferStart();
    p_main_file_end   = main_buffer->getBufferEnd();

    // FIXME: Do we need a rewriter?
}

void DotConsumer::HandleTopLevelDecl(clang::DeclGroupRef decl_grp_ref) { 
    std::cout << "Enter DotConsumer::HandleTopLevelDecl..." << std::endl;

    clang::DeclGroupRef::iterator decl_it;
    for (decl_it = decl_grp_ref.begin(); decl_it != decl_grp_ref.end(); decl_it++)
        traverseAST(*(decl_it));

    std::cout << "Leave DotConsumer::HandleTopLevelDecl." << std::endl;
}

void DotConsumer::HandleTranslationUnit(clang::ASTContext & ast_context) {
    std::cout << "Enter DotConsumer::HandleTranslationUnit..." << std::endl;

//    traverseASTusingDeclContext(clang::TranslationUnitDecl::castToDeclContext(ast_context.getTranslationUnitDecl()));

    std::cout << "Leave DotConsumer::HandleTranslationUnit." << std::endl;
}

