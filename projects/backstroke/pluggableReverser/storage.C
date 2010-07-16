#include "storage.h"
#include "singleton.h"
#include "utilities/CPPDefinesAndNamespaces.h"


using namespace SageInterface;
using namespace SageBuilder;

// The object storing all stack variable declarations.
typedef singleton_default<std::map<std::string, SgVariableDeclaration*> > AllStackDecl;

SgExpression* getStackVar(SgType* type, SgNode* node)
{
    string type_name;

    if (isSgTypeInt(type))
        type_name = "int";
    if (isSgTypeBool(type))
        type_name = "bool";
    if (isSgTypeFloat(type))
        type_name = "float";

    AllStackDecl::object_type& all_var_decls = AllStackDecl::instance();

    SgFunctionDeclaration* func_decl = NULL;
    while (!(func_decl = isSgFunctionDeclaration(node)))
    {
        node = node->get_parent();
        ROSE_ASSERT(node);
    }

    string stack_name = func_decl->get_name() + "_" + type_name + "_stack";
    if (all_var_decls.count(stack_name) == 0)
    {
        SgType* stack_type = buildStructDeclaration("std::stack<" + type_name + ">")->get_type();
        ROSE_ASSERT(stack_type);
        all_var_decls[stack_name] = buildVariableDeclaration(stack_name, stack_type);
    }

    return buildVarRefExp(all_var_decls[stack_name]->get_variables()[0]);
}

std::vector<SgVariableDeclaration*> getAllStackDeclarations()
{
    vector<SgVariableDeclaration*> output;
    typedef AllStackDecl::object_type::value_type pair_t;
    foreach (pair_t decl_pair, AllStackDecl::instance())
        output.push_back(decl_pair.second);
    return output;
}


// Currently, when calling the following function, make sure the expression passed in
// has a function declaration as parent.

//SgExpression* pushVal(SgExpression* exp)
//{
//    return buildFunctionCallExp(
//            "push",
//            buildIntType(),
//            buildExprListExp(
//            getStackVar(exp),
//            copyExpression(exp)));
//}

SgExpression* pushVal(SgExpression* exp, SgType* type, SgNode* node)
{
    return buildFunctionCallExp(
            "push", type,
            buildExprListExp(
                getStackVar(type, node),
                copyExpression(exp)));
}

SgExpression* popVal(SgType* type, SgNode* node)
{
    return buildFunctionCallExp("pop", type,
            buildExprListExp(getStackVar(type, node)));
}

// FIXME  This function should be versioned to deal with int and float values.
// Note that currently, we require that an expression which is assigned by the popped value
// should be passed to this function, then we can do type check and stack name generation.

//SgExpression* popVal(SgExpression* exp)
//{
//    return buildFunctionCallExp("pop", buildIntType(),
//            buildExprListExp(getStackVar(exp)));
//}

