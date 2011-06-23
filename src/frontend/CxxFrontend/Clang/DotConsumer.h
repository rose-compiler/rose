
#include <string>
#include <iostream>

#include "clang/AST/AST.h"
#include "clang/AST/ASTContext.h"
#include "clang/AST/ASTConsumer.h"
#include "clang/AST/Decl.h"
#include "clang/AST/DeclarationName.h"
#include "clang/AST/DeclGroup.h"
#include "clang/AST/DeclVisitor.h"
#include "clang/AST/StmtVisitor.h"
#include "clang/AST/TypeLocVisitor.h"

#include "clang/Basic/Diagnostic.h"
#include "clang/Basic/LangOptions.h"
#include "clang/Basic/SourceManager.h"

#include "llvm/Support/raw_ostream.h"

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
        DotConsumer(std::string & input, llvm::raw_ostream * output, clang::Diagnostic & diagnostic, const clang::LangOptions & lang_options);

        virtual ~DotConsumer();

        virtual void Initialize(clang::ASTContext & context);

        virtual void HandleTopLevelDecl(clang::DeclGroupRef D);
        virtual void HandleTranslationUnit(clang::ASTContext &C);
};

int generateDot(std::string input, std::string output);

void traverseAST(clang::Decl * decl, std::string indent = std::string());
void traverseAST(clang::Stmt * stmt, std::string indent = std::string());


