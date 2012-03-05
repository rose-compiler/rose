%{
#include "hir/irstatements.h"
#include "hir/irinlineasm.h"
#include "hir/irexpressions.h"
#include "hir/irsimdexpr.h"
#include "hir/irdspexpressions.h"
#include "hir/irmatchtest.h"
#include "ir/irbase.h"
#include "ir/irbasefields.h"
    
typedef IrStatement *PIrStatement;
typedef IrExpr *PIrExpr;
typedef IrNodelistTmpl<IrExprField> *LIST_PIrExpr;
typedef IrMultiAssign::Assign *PIrMultiAssign_Assign;
typedef IrNodelistTmpl<IrMultiAssign::Assign> *LIST_PIrMultiAssign_Assign;
typedef IrControlStatement *PIrControlStatement;
typedef PIrControlStatement N_IrControlStatement;
typedef IrIf *PIrIf;
typedef PIrIf N_IrIf;
typedef IrUnary *PIrUnary;
typedef PIrUnary N_IrUnary;
typedef IrBinary *PIrBinary;
typedef PIrBinary N_IrBinary;
typedef IrCall::Argument *PIrCall_Argument;
typedef IrNodelistTmpl<IrCall::Argument> *LIST_PIrCall_Argument;
typedef IrAsm::Info *PIrAsm_Info;
typedef IrNodelistTmpl<IrAsm::Info> *LIST_PIrAsm_Info;
typedef IrAsm::Arg *PIrAsm_Arg;
typedef IrNodelistTmpl<IrAsm::Arg> *LIST_PIrAsm_Arg;
typedef IrRelation *PIrRelation;
typedef PIrRelation N_IrRelation;
typedef IrParallelBinary *PIrParallelBinary;
typedef PIrParallelBinary N_IrParallelBinary;
typedef const char *CString;
typedef int Int32;
%}
%{
#include "syntree.h"
%}

%%
get:head(NODE, TYPE, CONSTR, _, FIELD, _, FTYPE)
%{
FTYPE TYPE##_##CONSTR##_get_##FIELD(TYPE NODE)
%}

get:body(NODE, TYPE, _, _, "node", _, FTYPE)
%{
    return dynamic_cast<FTYPE>(NODE);
%}

get:body(NODE, TYPE, "N_IrIf" | "N_IrUnary" | "N_IrBinary"
    | "N_IrRelation" | "N_IrParallelBinary", _, FIELD, _, _)
%{
    return (dynamic_cast<PIrIf>(NODE))->FIELD.get();
%}

get:body(NODE, _, "Assign" | "Argument" | "Arg" | "Info", _, FIELD, _, _)
%{
    return NODE->FIELD.get();
%}

get:body(NODE, _, CONSTR, _, "assigns", _, "LIST_PIrMultiAssign_Assign")
%{
    CONSTR *node = dynamic_cast<CONSTR *>(NODE);
    return new IrNodelistTmpl<IrMultiAssign::Assign>(node->FIELD);
%}

get:body(NODE, _, _, _, "arguments", _, "LIST_PIrCall_Argument")
%{
    CONSTR *node = dynamic_cast<CONSTR *>(NODE);
    return new IrNodelistTmpl<IrCall::Argument>(node->FIELD);
%}

get:body(NODE, _, _, _, "infos", _, "LIST_PIrAsm_Info")
%{
    CONSTR *node = dynamic_cast<CONSTR *>(NODE);
    return new IrNodelistTmpl<IrAsm::Info>(node->FIELD);
%}

get:body(NODE, _, _, _, "args", _, "LIST_PIrAsm_Arg")
%{
    CONSTR *node = dynamic_cast<CONSTR *>(NODE);
    return new IrNodelistTmpl<IrAsm::Arg>(node->FIELD);
%}

get:body(NODE, TYPE, CONSTR, _, FIELD, _, _)
%{
    return (dynamic_cast<CONSTR *>(NODE))->FIELD.get();
%}

%%
per_constructor is:head(NODE, TYPE, CONSTR, _, _, _, _)
%{
int is_op_##TYPE##_##CONSTR(TYPE NODE)
%}

is:body(NODE, TYPE, "N_IrControlStatement" | "N_IrIf" | "N_IrUnary"
    | "N_IrBinary" | "N_IrRelation" | "N_IrParallelBinary",
    _, FIELD, _, _)
%{
    return (dynamic_cast<PIrControlStatement>(NODE) != 0);
%}

is:body(NODE, _, "Assign" | "Argument" | "Arg" | "Info", _, FIELD, _, _)
%{
    return 1;
%}

is:body(NODE, _, CONSTR, _, _, _, _)
%{
    return (dynamic_cast<CONSTR *>(NODE) != 0);
%}

%%
per_constructor list empty:head(L, _, _, _, _, _, FTYPE)
%{
int LIST_##FTYPE##_empty(LIST_##FTYPE L)
%}

empty:body(L, _, _, _, _, _, _)
%{
    return (L->getFirst() == 0);
%}

%%
per_constructor list hd:head(L, _, _, _, _, _, FTYPE)
%{
FTYPE LIST_##FTYPE##_hd(LIST_##FTYPE L)
%}

hd:body(L, _, _, _, _, _, _)
%{
    return L->getFirst();
%}

%%
per_constructor list tl:head(L, _, _, _, _, _, FTYPE)
%{
LIST_##FTYPE LIST_##FTYPE##_tl(LIST_##FTYPE L)
%}

tl:body(L, _, _, _, _, _, _)
%{
    L->fetchFirst();
    return L;
%}

%{%}
%{%}
