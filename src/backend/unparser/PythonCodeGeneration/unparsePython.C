#include "sage3basic.h"
#include "unparsePython.h"

#include <iostream>

#include <boost/foreach.hpp>
#define foreach BOOST_FOREACH

using namespace std;

Unparse_Python::Unparse_Python(Unparser* unp, std::string fname) :
    UnparseLanguageIndependentConstructs(unp, fname)
{
}

Unparse_Python::~Unparse_Python()
{
}

void
Unparse_Python::unparseLanguageSpecificStatement(SgStatement* stmt,
                                                 SgUnparse_Info& info)
{
    switch (stmt->variantT()) {

#define CASE_DISPATCH_AND_BREAK(sg_t) \
  case V_Sg ##sg_t : unparse ##sg_t (isSg##sg_t (stmt),info); break;

        CASE_DISPATCH_AND_BREAK(AddOp);
        CASE_DISPATCH_AND_BREAK(BasicBlock);
        CASE_DISPATCH_AND_BREAK(FunctionDeclaration);
        CASE_DISPATCH_AND_BREAK(FunctionDefinition);
        CASE_DISPATCH_AND_BREAK(FunctionParameterList);
        CASE_DISPATCH_AND_BREAK(ReturnStmt);
        CASE_DISPATCH_AND_BREAK(StringVal);
        default: {
            cerr << "unparse Statement (" << stmt->class_name()
                 << "*) is unimplemented." << endl;
            break;
        }
    }
}

void
Unparse_Python::unparseLanguageSpecificExpression(SgExpression* stmt,
                                                  SgUnparse_Info& info)
{
    switch (stmt->variantT()) {
        CASE_DISPATCH_AND_BREAK(AssignInitializer);
        default: {
            cerr << "unparse Expression (" << stmt->class_name()
                 << "*) is unimplemented." << endl;
            break;
        }
    }
}

void
Unparse_Python::unparseStringVal(SgExpression*, SgUnparse_Info&)
{
    cout << "Unparse_Python string val" << endl;
}

std::string
Unparse_Python::ws_prefix(int nesting_level) {
    stringstream code;
    for(int i = 0; i < nesting_level; i++)
        code << "    ";
    return code.str();
}

/* ================== Node-specific unparsing functions ===================== */

void
Unparse_Python::unparseAddOp(SgAddOp* sg_add_op,
                             SgUnparse_Info& info)
{
    unparseExpression(sg_add_op->get_lhs_operand(), info);
    curprint(" + ");
    unparseExpression(sg_add_op->get_rhs_operand(), info);
}

void
Unparse_Python::unparseAssignInitializer(SgAssignInitializer* sg_assign_init,
                             SgUnparse_Info& info)
{
    curprint("=");
    unparseExpression(sg_assign_init->get_operand(), info);
}

void
Unparse_Python::unparseBasicBlock(SgBasicBlock* bblock,
                                  SgUnparse_Info& info)
{
    foreach (SgStatement* child, bblock->get_statements()) {
        curprint( ws_prefix(info.get_nestingLevel()) );
        unparseStatement(child, info);

        //TODO easier way to print newline
        stringstream code;
        code << endl;
        curprint( code.str() );
    }
}

void
Unparse_Python::unparseFunctionDeclaration(SgFunctionDeclaration* func_decl,
                                           SgUnparse_Info& info)
{
    stringstream code0;
    string func_name = func_decl->get_name().getString();
    code0 << "def " << func_name << "(";
    curprint (code0.str());

    unparseStatement(func_decl->get_parameterList(), info);

    stringstream code1;
    code1 << "):" << endl;
    curprint (code1.str());

    info.inc_nestingLevel();
    unparseStatement(func_decl->get_definition(), info);
    info.dec_nestingLevel();
}

void
Unparse_Python::unparseFunctionDefinition(SgFunctionDefinition* func_decl,
                                          SgUnparse_Info& info)
{
    unparseStatement(func_decl->get_body(), info);
}

void
Unparse_Python::unparseFunctionParameterList(SgFunctionParameterList* param_list,
                                             SgUnparse_Info& info)
{
    SgInitializedNamePtrList& arg_list = param_list->get_args();
    SgInitializedNamePtrList::iterator name_iter;
    SgInitializedName* init_name;
    for (name_iter = arg_list.begin(); name_iter != arg_list.end(); name_iter++) {
        if (name_iter != arg_list.begin())
            curprint(", ");
        unparseInitializedName(*name_iter, info);
    }
}

void
Unparse_Python::unparseInitializedName(SgInitializedName* init_name,
                                       SgUnparse_Info& info)
{
    curprint(init_name->get_name().str());
    if (init_name->get_initializer() != NULL) {
        unparseExpression(init_name->get_initializer(), info);
    }
}

void
Unparse_Python::unparseReturnStmt(SgReturnStmt* return_stmt,
                                  SgUnparse_Info& info)
{
    curprint("return ");
    unparseExpression(return_stmt->get_expression(), info);
}

void
Unparse_Python::unparseStringVal(SgStringVal* str,
                                 SgUnparse_Info& info)
{
    stringstream code;
    code << "\"" << str->get_value() << "\"";
    curprint( code.str() );
}

