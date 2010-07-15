#include "storage.h"
#include "singleton.h"
#include "utilities/CPPDefinesAndNamespaces.h"


using namespace SageInterface;
using namespace SageBuilder;

// The object storing all stack variable declarations.
typedef singleton_default<std::map<SgFunctionDeclaration*, SgVariableDeclaration*> > AllStackDecl;

SgExpression* getStackVar(SgNode* node)
{
    SgFunctionDeclaration* func_decl = NULL;
    while (!(func_decl = isSgFunctionDeclaration(node)))
    {
        node = node->get_parent();
        ROSE_ASSERT(node);
    }

    std::map<SgFunctionDeclaration*, SgVariableDeclaration*>& all_var_decls = AllStackDecl::instance();

    if (all_var_decls.count(func_decl) == 0)
    {
        SgType* stack_type = buildPointerType(buildStructDeclaration("IntStack")->get_type());
        std::string stack_name = func_decl->get_name() + "_stack";
        all_var_decls[func_decl] = buildVariableDeclaration(stack_name, stack_type);
    }
    return buildVarRefExp(all_var_decls[func_decl]->get_variables()[0]);
}

std::vector<SgVariableDeclaration*> getAllStackDeclarations()
{
    vector<SgVariableDeclaration*> output;
    std::map<SgFunctionDeclaration*, SgVariableDeclaration*>& all_var_decls = AllStackDecl::instance();

    typedef std::pair<SgFunctionDeclaration*, SgVariableDeclaration*> pair_t;
    foreach (pair_t decl_pair, all_var_decls)
        output.push_back(decl_pair.second);
    return output;
}


// Currently, when calling the following function, make sure the expression passed in
// has a function declaration as parent.
SgExpression* pushVal(SgExpression* exp)
{
    return buildFunctionCallExp(
            "push", 
            buildIntType(), 
            buildExprListExp(
                getStackVar(exp),
                copyExpression(exp)));
}

// FIXME  This function should be versioned to deal with int and float values.
// Note that currently, we require that an expression which is assigned by the popped value
// should be passed to this function, then we can do type check and stack name generation.
SgExpression* popVal(SgExpression* exp)
{
    return buildFunctionCallExp("pop", buildIntType(), 
            buildExprListExp(getStackVar(exp)));
}

