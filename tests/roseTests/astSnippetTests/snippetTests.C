#include "snippetTests.h"
#include <boost/foreach.hpp>

using namespace rose;

namespace SnippetTests {

std::string
findSnippetFile(const std::string &fileName)
{
    if (fileName.empty())
        throw std::runtime_error("empty file name");

    if ('/'==fileName[0]) {
        if (0 != access(fileName.c_str(), R_OK))
            throw std::runtime_error("snippet file does not exist or is not readable: " + fileName);
        return fileName;
    }

    std::vector<std::string> directories;
    directories.push_back(".");
    directories.push_back(ROSE_AUTOMAKE_TOP_SRCDIR + "/tests/roseTests/astSnippetTests");
    directories.push_back(ROSE_AUTOMAKE_TOP_SRCDIR + "/tests/roseTests/astSnippetTests/SmallSpecimensC");
    directories.push_back(ROSE_AUTOMAKE_TOP_SRCDIR + "/tests/roseTests/astSnippetTests/SmallSpecimensJava");
    BOOST_FOREACH (const std::string &directory, directories) {
        std::string fullName = directory + "/" + fileName;
        if (0 == access(fullName.c_str(), R_OK))
            return fullName;
    }

    throw std::runtime_error("snippet file does not exist or is not readable: " + fileName);
}

SnippetPtr
findSnippetInAst(SgProject *project, const std::string &snippetFileName, const std::string &snippetName)
{
    SgFunctionDefinition *snippetDefn = findFunctionDefinition(project, snippetName);
    ROSE_ASSERT(snippetDefn || !"cannot find snippet in existing AST");
    SgSourceFile *snippetFileAst = isSgSourceFile(SageInterface::getEnclosingFileNode(snippetDefn));
    ROSE_ASSERT(snippetFileAst || !"snippet has no source file");

    SnippetFilePtr snippetFile = SnippetFile::instance(snippetFileName, snippetFileAst);
    return snippetFile->findSnippet(snippetName);
}

SgFunctionDefinition *
findFunctionDefinition(SgNode *ast, std::string function_name)
{
    assert(ast!=NULL);
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

std::vector<SgFunctionDefinition*>
findFunctionDefinitions(SgNode *ast, std::string function_name)
{
    assert(ast!=NULL);
    struct FindFunctionDef: AstSimpleProcessing {
        const std::string &name;
        std::vector<SgFunctionDefinition*> found;
        FindFunctionDef(const std::string &name): name(name) {}

        void visit(SgNode *node) {
            if (SgFunctionDefinition *fdef = isSgFunctionDefinition(node)) {
                if (fdef->get_declaration()->get_qualified_name() == name)
                    found.push_back(fdef);
            }
        }
    } t1(function_name);
    t1.traverse(ast, preorder);
    return t1.found;
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

SgStatement *
findInsertHere(SgFunctionDefinition *fdef)
{
    struct T1: AstSimpleProcessing {
        void visit(SgNode *node) {
            if (SgVarRefExp *vref = isSgVarRefExp(node)) {
                if (vref->get_symbol()->get_name() == "INSERT_HERE")
                    throw SageInterface::getEnclosingNode<SgStatement>(vref);
            }
        }
    } t1;
    try {
        t1.traverse(fdef, preorder);
    } catch (SgStatement *stmt) {
        return stmt;
    }
    return NULL;
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

SgInitializedName *
findArgumentDeclaration(SgNode *ast, const std::string &varName)
{
    SgFunctionDeclaration *fdecl = SageInterface::getEnclosingNode<SgFunctionDeclaration>(ast);
    ROSE_ASSERT(fdecl || !"cannot find enclosing function declaration");
    return findVariableDeclaration(fdecl->get_parameterList(), varName);
}

std::vector<SgInitializedName*>
findFunctionVariables(SgFunctionDefinition *fdef)
{
    SgFunctionDeclaration *fdecl = fdef->get_declaration();
    return SageInterface::querySubTree<SgInitializedName>(fdecl);
}

} // namespace
