#include "snippetTests.h"

using namespace rose;

namespace SnippetTests {

std::string
findSnippetFile(const std::string &fileName)
{
    assert(!fileName.empty());
    std::string fullName = '/'==fileName[0] ? fileName :
                           ROSE_AUTOMAKE_TOP_SRCDIR + "/tests/roseTests/astSnippetTests/" + fileName;
    if (fileName.size()<2 || fileName.substr(fileName.size()-2)!=".c")
        fullName += ".c";
    if (-1 != access(fullName.c_str(), R_OK))
        return fullName;
    std::cerr <<"snippet file does not exist or is not readable: " <<fullName <<"\n";
    return "";
}

SgFunctionDefinition *
findFunctionDefinition(SgNode *ast, std::string function_name)
{
    assert(ast!=NULL);
    if (function_name.substr(0, 2)!="::")
        function_name = "::" + function_name;

    struct FindFunctionDef: AstSimpleProcessing {
        const std::string &name;
        FindFunctionDef(const std::string &name): name(name) {}

        void visit(SgNode *node) {
            if (SgFunctionDefinition *fdef = isSgFunctionDefinition(node)) {
                if (fdef->get_declaration()->get_qualified_name() == name)
                    throw fdef;
            }
        }
    } t1(function_name);

    try {
        t1.traverse(ast, preorder);
    } catch (SgFunctionDefinition *def) {
        return def;
    }
    return NULL;
}

SgStatement *
findLastAppendableStatement(SgFunctionDefinition *fdef)
{
    assert(fdef!=NULL);
    std::vector<SgStatement*> stmts = SageInterface::querySubTree<SgStatement>(fdef->get_body());
    while (!stmts.empty() && (isSgScopeStatement(stmts.back()) || isSgReturnStmt(stmts.back())))
        stmts.pop_back();
    return stmts.empty() ? NULL : stmts.back();
}

SgStatement *
findLastStatement(SgFunctionDefinition *fdef)
{
    assert(fdef!=NULL);
    struct T1: AstSimpleProcessing {
        SgStatement *last_stmt;
        T1(): last_stmt(NULL) {}
        void visit(SgNode *node) {
            if (SgStatement *stmt = isSgStatement(node))
                last_stmt = stmt;
        }
    } t1;
    t1.traverse(fdef, preorder);
    return t1.last_stmt;
}


SgInitializedName *
findVariableDeclaration(SgNode *ast, const std::string &var_name)
{
    struct FindVariable: AstSimpleProcessing {
        const std::string &name;
        FindVariable(const std::string &name): name(name) {}

        void visit(SgNode *node) {
            if (SgInitializedName *iname = isSgInitializedName(node)) {
                if (iname->get_name()==name)
                    throw iname;
            }
        }
    } t1(var_name);

    try {
        t1.traverse(ast, preorder);
    } catch (SgInitializedName *iname) {
        return iname;
    }
    return NULL;
}

std::vector<SgInitializedName*>
findFunctionVariables(SgFunctionDefinition *fdef)
{
    SgFunctionDeclaration *fdecl = fdef->get_declaration();
    return SageInterface::querySubTree<SgInitializedName>(fdecl);
}

} // namespace
