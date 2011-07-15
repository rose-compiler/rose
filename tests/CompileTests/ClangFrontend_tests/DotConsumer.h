
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

#include "DotVisitor.h"

enum Language {
    C,
    CPLUSPLUS,
    OBJC,
    CUDA,
    OPENCL,
    unknown
};

class DotConsumer : public clang::ASTConsumer {
    protected:
        DotVisitor p_dot_visitor;

        void print(std::ostream & out);

    public:
        DotConsumer(const clang::SourceManager & src_mgr, const clang::LangOptions & lang_opts);

        virtual ~DotConsumer();

        virtual void HandleTranslationUnit(clang::ASTContext & ast_context);

        static int generateDot(std::string input, std::string output, Language language);
};

void traverseAST(clang::Decl * decl, std::string indent = std::string());
void traverseAST(clang::Stmt * stmt, std::string indent = std::string());

