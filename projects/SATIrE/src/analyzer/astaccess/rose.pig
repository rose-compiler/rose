%{ /* -*- c++ -*- */
#ifndef H_SYNTREE
#define H_SYNTREE

#include "syn_typedefs.h"
%}
%{
#ifndef NDEBUG
#   define NDEBUG
#   define NDEBUG_WAS_NOT_DEFINED
#endif

#include "syntree.h"

#include <satire_rose.h>

#include "pignodelist.h"
#include "IrCreation.h"
%}

%%
extern_c get:head(NODE, TYPE, CONSTR, _, FIELD, _, FTYPE)
%{
FTYPE TYPE##_##CONSTR##_get_##FIELD(TYPE NODE)
%}

/* do something for all nonterminals (marked NT) */
get:body(NODE, "ExpressionRootNT", _, _, FIELD, _, _)
%{
    return isSgExpressionRoot((SgNode *) NODE)->get_##FIELD();
%}

get:body(NODE, _, "UndeclareStmt", _, "vars", _, _)
%{
    return Ir::createNodeList(*((UndeclareStmt *) NODE)->get_vars());
%}
get:body(NODE, _, "DeclareStmt" | "UndeclareStmt", _, FIELD, _, _)
%{
    CONSTR *stmt = dynamic_cast<CONSTR *>((SgNode *) NODE);
    return (void *) stmt->get_##FIELD();
%}

get:body(NODE, _, "ExternalCall" | "ExternalReturn", _, "params", _, _)
%{
    CONSTR *stmt = dynamic_cast<CONSTR *>((SgNode *) NODE);
    return Ir::createNodeList(*stmt->get_##FIELD());
%}

get:body(NODE, _, "ConstructorCall" | "DestructorCall", _, "name", _, _)
%{
    CONSTR *stmt = dynamic_cast<CONSTR *>((SgNode *) NODE);
    return Ir::getCharPtr(stmt->get_##FIELD());
%}

get:body(NODE, _, "FunctionEntry" | "FunctionExit" | "FunctionCall" | "FunctionReturn", _, "funcname", _, _)
%{
    CONSTR *stmt = dynamic_cast<CONSTR *>((SgNode *) NODE);
    return Ir::getCharPtr(stmt->get_##FIELD());
%}

get:body(NODE, _, "ExternalCall" | "ExternalReturn" | "ConstructorCall" | "DestructorCall", _, FIELD, _, _)
%{
    CONSTR *stmt = dynamic_cast<CONSTR *>((SgNode *) NODE);
    return stmt->get_##FIELD();
%}

get:body(NODE, _, "FunctionExit"|"FunctionCall"|"FunctionReturn", _, "params", _, _)
%{
    if (((CallStmt *) NODE)->parent->get_##FIELD() != NULL)
        return Ir::createNodeList(*((CallStmt *) NODE)->parent->get_##FIELD());
    else
        return NULL;
%}

get:body(NODE, _, "FunctionEntry" | "FunctionExit" | "FunctionCall" | "FunctionReturn" , _, FIELD, _, _)
%{
    CONSTR *stmt = dynamic_cast<CONSTR *>((SgNode *) NODE);
    return stmt->get_##FIELD();
%}

get:body(NODE, _, "ArgumentAssignment" | "ParamAssignment" | "ReturnAssignment" | "LogicalIf" | "IfJoin" | "WhileJoin", _, FIELD, _, _)
%{
    CONSTR *assignment = dynamic_cast<CONSTR *>((SgNode *) NODE);
    return assignment->get_##FIELD();
%}

get:body(NODE, _, "ScopeStatement", _, "node", _, _)
%{
    return NODE;
%}

get:body(NODE, _, "VarRefExp" | "InitializedName", _, "name", _, _)
%{
    SgInitializedName *in = isSgInitializedName((SgNode *) NODE);
    if (in == NULL && isSgVarRefExp((SgNode *) NODE))
        in = isSgVarRefExp((SgNode *) NODE)->get_symbol()->get_declaration();
    if (in != NULL) {
         /* FIXME: check for global scope '::' and use non-qualified name for global scope
          *        find a better way to deal with qualified names */
         if ((in->get_scope() != NULL) && (in->get_scope()->get_qualified_name()!="::")) {
            return Ir::getCharPtr(Ir::getStrippedName(in));
         } else {
            /* don't print the global namespace (::) */
            return Ir::getCharPtr(in->get_name());
         }
    } else
        return NULL;
%}

get:body(NODE, _, "VariableSymbol", _, "name", _, _)
%{
    SgVariableSymbol *var = isSgVariableSymbol((SgNode *) NODE);
    /* FIXME: check for global scope '::' and use non-qualified name for global scope
     *        find a better way to deal with qualified names [MS08] */
    if ((var->get_declaration()->get_scope() != NULL) && (var->get_declaration()->get_scope()->get_qualified_name() != "::") )
        return Ir::getCharPtr(var->get_declaration()->get_qualified_name());
    else
        /* don't print the global namespace (::) */
        return Ir::getCharPtr(var->get_declaration()->get_name());
%}

get:body(NODE, _, "ArrayType", _, "size", _, _)
%{
    SgExpression *index = isSgArrayType((SgNode *) NODE)->get_index();
    if (SgUnsignedLongVal *v = isSgUnsignedLongVal(index))
        return v->get_value();
    else
        return -1;
%}

get:body(NODE, _, "BasicType", _, "typename", _, _)
%{
    return basic_type_name(NODE);
%}

get:body(NODE, _, "NamedType", _, "name", _, _)
%{
    SgNamedType *type = isSgNamedType((SgNode *) NODE);
    return Ir::getCharPtr(type->get_name());
%}

/* lists */
get:body(NODE, _, _, _, "declarations", _, FTYPE)
%{
    return Ir::createNodeList(
        isSg##CONSTR((SgNode *) NODE)->get_##FIELD());
%}

get:body(NODE, _, _, _, "args", _, FTYPE)
%{
    return Ir::createNodeList(
        isSg##CONSTR((SgNode *) NODE)->get_##FIELD());
%}

get:body(NODE, _, _, _, "arguments", _, FTYPE)
%{
    return Ir::createNodeList(
        isSg##CONSTR((SgNode *) NODE)->get_##FIELD());
%}

get:body(NODE, _, _, _, "ctors", _, FTYPE)
%{
    return Ir::createNodeList(
        isSg##CONSTR((SgNode *) NODE)->get_##FIELD());
%}

get:body(NODE, _, "ForInitStatement", _, "init_stmt", _, FTYPE)
%{
    return Ir::createNodeList(
        isSg##CONSTR((SgNode *) NODE)->get_##FIELD());
%}

get:body(NODE, _, "VariableDeclaration", _, "variables", _, FTYPE)
%{
    return Ir::createNodeList(
        isSg##CONSTR((SgNode *) NODE)->get_##FIELD());
%}

get:body(NODE, _, "ExprListExp", _, "expressions", _, FTYPE)
%{
    return Ir::createNodeList(
        isSg##CONSTR((SgNode *) NODE)->get_##FIELD());
%}

get:body(NODE, _, "AsmStmt", _, "operands", _, FTYPE)
%{
    return Ir::createNodeList(
        isSg##CONSTR((SgNode *) NODE)->get_##FIELD());
%}

get:body(NODE, _, "StringVal", _, "value", _, _)
%{
    return Ir::getCharPtr(isSg##CONSTR((SgNode *) NODE)->get_value());
%}

get:body(NODE, _, CONSTR, _, "value", _, _)
%{
    return isSg##CONSTR((SgNode *) NODE)->get_value();
%}

get:body(NODE, _, CONSTR, _, FIELD, _, _)
%{
    return (void *) isSg##CONSTR((SgNode *) NODE)->get_##FIELD();
%}

%%
extern_c per_constructor is:head(NODE, TYPE, CONSTR, _, _, _, _)
%{
int is_op_##TYPE##_##CONSTR(TYPE NODE)
%}

is:body(NODE, _, "DeclareStmt" | "UndeclareStmt", _, _, _, _)
%{
    return dynamic_cast<CONSTR *>(isSgStatement((SgNode *) NODE)) != NULL;
%}

is:body(NODE, _, "ExpressionRootNT", _, _, _, _)
%{
    return isSgExpressionRoot((SgNode *) NODE) != NULL;
%}

is:body(NODE, _, "ExternalCall" | "ExternalReturn" | "ConstructorCall" | "DestructorCall", _, _, _, _)
%{
    CONSTR *e = dynamic_cast<CONSTR *>(isSgStatement((SgNode *) NODE));
    return e != NULL;
%}

is:body(NODE, _, "FunctionEntry" | "FunctionExit" | "FunctionCall" | "FunctionReturn", _, _, _, _)
%{
    CONSTR *e = dynamic_cast<CONSTR *>(isSgStatement((SgNode *) NODE));
    return e != NULL && e->parent != NULL
        && e->parent->node_type == (KFG_NODE_TYPE) X_##CONSTR;
%}

is:body(NODE, _, "ArgumentAssignment" | "ParamAssignment" | "ReturnAssignment" | "LogicalIf" | "IfJoin" | "WhileJoin", _, _, _, _)
%{
    return dynamic_cast<CONSTR *>(isSgStatement((SgNode *) NODE)) != NULL;
%}

is:body(NODE, _, "BasicType", _, _, _, _)
%{
    return basic_type_name(NODE) != NULL;
%}

is:body(NODE, _, CONSTR, _, _, _, _)
%{
    return isSg##CONSTR((SgNode *) NODE) != NULL;
%}

%%
extern_c list empty:head(NODE, _, _, _, _, _, FTYPE)
%{
int LIST_##FTYPE##_empty(void **NODE)
%}

empty:body(NODE, _, _, _, _, _, _)
%{
    return *NODE == NULL;
%}

%%
extern_c list hd:head(NODE, _, _, _, _, _, FTYPE)
%{
void *LIST_##FTYPE##_hd(void **NODE)
%}

hd:body(NODE, _, _, _, _, _, _)
%{
    return *NODE;
%}

%%
extern_c list tl:head(NODE, _, _, _, _, _, FTYPE)
%{
void *LIST_##FTYPE##_tl(void **NODE)
%}

tl:body(NODE, _, _, _, _, _, _)
%{
    return NODE + 1;
%}


%{
#include "pag_support.h"

#endif
%}
%{
//#include "pag_support.c"

#ifdef NDEBUG_WAS_NOT_DEFINED
#   undef NDEBUG
#endif
%}
