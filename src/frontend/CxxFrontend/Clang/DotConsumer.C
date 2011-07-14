
#include "DotConsumer.h"

#include <string>

#include "ClangFrontend.h"

#include "clang/AST/ParentMap.h"

#include "clang/Basic/Builtins.h"
#include "clang/Basic/IdentifierTable.h"
#include "clang/Basic/FileManager.h"
#include "clang/Basic/TargetInfo.h"
#include "clang/Basic/TargetOptions.h"

#include "clang/Frontend/DiagnosticOptions.h"
#include "clang/Frontend/TextDiagnosticPrinter.h"

#include "clang/Lex/HeaderSearch.h"
#include "clang/Lex/Preprocessor.h"

#include "clang/Parse/ParseAST.h"

#include "llvm/Config/config.h"

#include "llvm/Support/raw_os_ostream.h"

// Module's function

int DotConsumer::generateDot(std::string input, std::string output, Language language) {
    clang::FileSystemOptions file_options;

    clang::FileManager file_manager(file_options);

    clang::DiagnosticIDs * diag_id = new clang::DiagnosticIDs();

    const llvm::IntrusiveRefCntPtr<clang::DiagnosticIDs> diag_id_ptr(diag_id);

    llvm::raw_os_ostream output_stream(std::cout);

    clang::DiagnosticOptions diag_opts;

    clang::TextDiagnosticPrinter * diag_printer = new clang::TextDiagnosticPrinter(output_stream, diag_opts);

    clang::Diagnostic diagnostic(diag_id_ptr, diag_printer);
        diagnostic.setSuppressAllDiagnostics(true);

    clang::SourceManager source_manager(diagnostic, file_manager);

    clang::HeaderSearch header_search(file_manager);

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

    clang::TargetInfo * target_info = clang::TargetInfo::CreateTargetInfo(diagnostic, target_options);

    clang::Preprocessor preprocessor(
        diagnostic,
        lang_options,
        *target_info,
        source_manager,
        header_search
    );

    DotConsumer * consumer = new DotConsumer(source_manager, lang_options);

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

    std::ofstream output_file_stream;

    output_file_stream.open(output.c_str());

    if (!output_file_stream.is_open()) {
        std::cerr << "Impossible to open " << output << " in write mode." << std::endl;
        exit(1);
    }

    consumer->print(output_file_stream);

    output_file_stream.close();

    // FIXME this delete cause a "double free or memory corruption" error.
//    delete consumer;

    return 1;
}

// AST Consumer generating a GraphViz file

void DotConsumer::print(std::ostream & out) {
    p_dot_visitor.toDot(out);
}

DotConsumer::DotConsumer(const clang::SourceManager & src_mgr, const clang::LangOptions & lang_opts) :
    clang::ASTConsumer(),
    p_dot_visitor(src_mgr, lang_opts)
{}

DotConsumer::~DotConsumer() {}

void DotConsumer::HandleTranslationUnit(clang::ASTContext & ast_context) {
    p_dot_visitor.TraverseDecl(ast_context.getTranslationUnitDecl());

    p_dot_visitor.finalyse();
}

