#include <iostream>

#include <string>

#include "ClangFrontend.h"

#include "clang/AST/ASTContext.h"

#include "clang/AST/Decl.h"
#include "clang/AST/DeclGroup.h"
#include "clang/AST/DeclarationName.h"
#include "clang/AST/StmtVisitor.h"
#include "clang/AST/DeclVisitor.h"
#include "clang/AST/TypeLocVisitor.h"

#include "clang/AST/AST.h"
#include "clang/AST/ASTConsumer.h"
#include "clang/AST/ParentMap.h"

#include "clang/Basic/Builtins.h"
#include "clang/Basic/IdentifierTable.h"
#include "clang/Basic/Diagnostic.h"
#include "clang/Basic/FileManager.h"
#include "clang/Basic/SourceManager.h"
#include "clang/Basic/TargetInfo.h"
#include "clang/Basic/TargetOptions.h"
#include "clang/Basic/LangOptions.h"

#include "clang/Lex/HeaderSearch.h"
#include "clang/Lex/Preprocessor.h"

#include "clang/Parse/ParseAST.h"

#include "llvm/Config/config.h"

#include <llvm/Support/raw_ostream.h>

class DotConsumer : public clang::ASTConsumer {
	protected:
		clang::Diagnostic & p_diagnostic;
		const clang::LangOptions & p_lang_options;

		std::string p_input_file;
		llvm::raw_ostream * p_output_file;

		clang::ASTContext * p_ast_context;
		clang::SourceManager * p_source_management;
		clang::TranslationUnitDecl * p_translation_unit_decl;

		clang::FileID p_main_file_id;
		const char * p_main_file_start;
		const char * p_main_file_end;

	public:
		DotConsumer(std::string & input, llvm::raw_ostream * output, clang::Diagnostic & diagnostic, const clang::LangOptions & lang_options) :
			clang::ASTConsumer(),
			p_diagnostic(diagnostic),
			p_lang_options(lang_options),
			p_input_file(input),
			p_output_file(output)
		{}

		virtual ~DotConsumer() {}

		virtual void Initialize(clang::ASTContext & context) {
			p_ast_context = &context;
			p_source_management = &(p_ast_context->getSourceManager());
			p_translation_unit_decl = p_ast_context->getTranslationUnitDecl();

			p_main_file_id = p_source_management->getMainFileID();

			const llvm::MemoryBuffer * main_buffer = p_source_management->getBuffer(p_main_file_id);

			p_main_file_start = main_buffer->getBufferStart();
			p_main_file_end   = main_buffer->getBufferEnd();

			// FIXME: Do we need a rewriter?
		}

		virtual void HandleTopLevelDecl(clang::DeclGroupRef D) {}

		virtual void HandleTranslationUnit(clang::ASTContext &C) {}
};

int clang_main(int argc, char* argv[], SgSourceFile& sageFile) {

    std::cout << "Enter clang_main..." << std::endl;

    std::string in_file("../../test-codes/c/small/test_0.c");

    clang::FileSystemOptions file_options;

    clang::FileManager file_manager(file_options);

    clang::DiagnosticIDs diag_id;

    const llvm::IntrusiveRefCntPtr<clang::DiagnosticIDs> diag_id_ptr(&diag_id);

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
    llvm::raw_fd_ostream * out_stream = new llvm::raw_fd_ostream((in_file + ".dot").c_str(), errors);

    DotConsumer * consumer = new DotConsumer(in_file, out_stream, diagnostic, lang_options);

    clang::IdentifierTable identifier_table(lang_options);

    clang::SelectorTable selector_table;

    clang::Builtin::Context builtin_context(*target_info);

    clang::ASTContext contex(
        lang_options,
        source_manager,
        *target_info,
        identifier_table,
        selector_table,
        builtin_context,
        0
    );

    clang::ParseAST(preprocessor, consumer, contex, false, true, NULL);

    delete out_stream;
    delete consumer;

    std::cout << "Leave clang_main." << std::endl;
    return 1;
}
