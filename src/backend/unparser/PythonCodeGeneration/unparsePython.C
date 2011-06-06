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

        CASE_DISPATCH_AND_BREAK(BasicBlock);
        CASE_DISPATCH_AND_BREAK(FunctionDeclaration);
        CASE_DISPATCH_AND_BREAK(FunctionDefinition);
        CASE_DISPATCH_AND_BREAK(StringVal);
        CASE_DISPATCH_AND_BREAK(AddOp);
        default: {
            cerr << "unparse(" << stmt->class_name()
                 << "*) is unimplemented." << endl;
            break;
        }
    }
}

void
Unparse_Python::unparseLanguageSpecificExpression(SgExpression*, SgUnparse_Info&)
{
    cout << "Unparse_Python language specific expression" << endl;
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
    stringstream code;
    string func_name = func_decl->get_name().getString();
    code << "def " << func_name << "():" << endl; //TODO: param list
    curprint (code.str());

    info.set_nestingLevel( info.get_nestingLevel()+1 );
    unparseStatement(func_decl->get_definition(), info);
    info.set_nestingLevel( info.get_nestingLevel()-1 );
}

void
Unparse_Python::unparseFunctionDefinition(SgFunctionDefinition* func_decl,
                                          SgUnparse_Info& info)
{
    unparseStatement(func_decl->get_body(), info);
}

void
Unparse_Python::unparseStringVal(SgStringVal* str,
                                 SgUnparse_Info& info)
{
    stringstream code;
    code << "\"" << str->get_value() << "\"";
    curprint( code.str() );
}

