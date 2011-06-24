#include "sage3basic.h"
#include "unparsePython.h"
#include "abstract_handle.h"

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

/*
 * Expects an SgGlobal scope node that includes a single function declaration
 * with name __main__. This allows us to wrap the program in a __main__ function
 * (because SageIII requires declaration statements in the global scope) and still
 * unparse code that resembles the original.
 */
void
Unparse_Python::unparseWrappedProgram(SgScopeStatement* scope, SgUnparse_Info& info) {

    AbstractHandle::abstract_handle* handle = SageBuilder::buildAbstractHandle(scope);
    SgNode* match = SageInterface::getSgNodeFromAbstractHandleString(handle->toString() +
        "::FunctionDeclaration<name," + ROSE_PYTHON_WRAPPER_FXN_NAME + ">" +
        "::FunctionDefinition<numbering,1>" +
        "::BasicBlock<numbering,1>");
    SgBasicBlock* program = isSgBasicBlock(match);
    ROSE_ASSERT(program != NULL);

    info.set_current_scope(program);
    unparseStatement(program, info);
}

void
Unparse_Python::unparseLanguageSpecificStatement(SgStatement* stmt,
                                                 SgUnparse_Info& info)
{
    switch (stmt->variantT()) {

#define CASE_DISPATCH_AND_BREAK(sg_t) \
  case V_Sg ##sg_t : unparse ##sg_t (isSg##sg_t (stmt),info); break;

        CASE_DISPATCH_AND_BREAK(BasicBlock);
        CASE_DISPATCH_AND_BREAK(ExprStatement);
        CASE_DISPATCH_AND_BREAK(FunctionCallExp);
        CASE_DISPATCH_AND_BREAK(FunctionDeclaration);
        CASE_DISPATCH_AND_BREAK(FunctionDefinition);
        CASE_DISPATCH_AND_BREAK(FunctionParameterList);
        CASE_DISPATCH_AND_BREAK(IfStmt);
        CASE_DISPATCH_AND_BREAK(LongIntVal);
        CASE_DISPATCH_AND_BREAK(ReturnStmt);
        CASE_DISPATCH_AND_BREAK(StringVal);
        CASE_DISPATCH_AND_BREAK(WhileStmt);
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
        CASE_DISPATCH_AND_BREAK(FunctionCallExp);
        CASE_DISPATCH_AND_BREAK(VarRefExp);

        case V_SgAddOp:
        case V_SgSubtractOp:
        case V_SgMultiplyOp:
        case V_SgDivideOp:
        case V_SgBitAndOp:
        case V_SgBitOrOp:
        case V_SgBitXorOp:
        case V_SgModOp:
        case V_SgIntegerDivideOp:
        case V_SgLshiftOp:
        case V_SgRshiftOp:
        case V_SgExponentiationOp:
        case V_SgPlusAssignOp:
    //  case V_SgBitAndAssignOp:
    //  case V_SgBitOrAssignOp:
    //  case V_SgBitXorAssignOp:
        case V_SgDivAssignOp:
    //  case V_SgExpAssignOp:
    //  case V_SgIntegerDivAssignOp:
        case V_SgLshiftAssignOp:
        case V_SgRshiftAssignOp:
        case V_SgModAssignOp:
        case V_SgMultAssignOp:
        case V_SgMinusAssignOp:
            unparseBinaryOp( isSgBinaryOp(stmt), info );
            break;

        case V_SgUnaryAddOp:
        case V_SgMinusOp:
        case V_SgBitComplementOp:
            unparseUnaryOp( isSgUnaryOp(stmt), info );
            break;

        default: {
            cerr << "unparse Expression (" << stmt->class_name()
                 << "*) is unimplemented." << endl;
            break;
        }
    }
}

void
Unparse_Python::unparseStringVal(SgExpression* str_exp, SgUnparse_Info& info)
{
    SgStringVal* str_val = isSgStringVal(str_exp);
    curprint(string("\"") + str_val->get_value() + string("\""));
    //TODO what about other types of python strings
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
        if (isSgExpression(child))
            unparseExpression(isSgExpression(child), info);
        else
            unparseStatement(child, info);
        curprint("\n");
    }
}

void
Unparse_Python::unparseBinaryOp(SgBinaryOp* bin_op,
                                SgUnparse_Info& info)
{
    unparseExpression(bin_op->get_lhs_operand(), info);
    curprint(" ");
    switch(bin_op->variantT()) {
        case V_SgAddOp:            curprint(ROSE_PYTHON_ADD_OP);        break;
        case V_SgAssignOp:         curprint(ROSE_PYTHON_ASSIGN_OP);     break;
        case V_SgBitAndOp:         curprint(ROSE_PYTHON_BITAND_OP);     break;
        case V_SgBitOrOp:          curprint(ROSE_PYTHON_BITOR_OP);      break;
        case V_SgBitXorOp:         curprint(ROSE_PYTHON_BITXOR_OP);     break;
        case V_SgDivideOp:         curprint(ROSE_PYTHON_DIV_OP);        break;
        case V_SgExponentiationOp: curprint(ROSE_PYTHON_EXP_OP);        break;
        case V_SgIntegerDivideOp:  curprint(ROSE_PYTHON_IDIV_OP);       break;
        case V_SgLshiftOp:         curprint(ROSE_PYTHON_LSHIFT_OP);     break;
        case V_SgModOp:            curprint(ROSE_PYTHON_MOD_OP);        break;
        case V_SgMultiplyOp:       curprint(ROSE_PYTHON_MULT_OP);       break;
        case V_SgRshiftOp:         curprint(ROSE_PYTHON_RSHIFT_OP);     break;
        case V_SgSubtractOp:       curprint(ROSE_PYTHON_SUB_OP);        break;
        case V_SgPlusAssignOp:     curprint(ROSE_PYTHON_AUG_ADD_OP);    break;
    //  case V_SgBitAndAssignOp:   curprint(ROSE_PYTHON_AUG_BITAND_OP); break;
    //  case V_SgBitOrAssignOp:    curprint(ROSE_PYTHON_AUG_BITOR_OP);  break;
    //  case V_SgBitXorAssignOp:   curprint(ROSE_PYTHON_AUG_BITXOR_OP); break;
        case V_SgDivAssignOp:      curprint(ROSE_PYTHON_AUG_DIV_OP);    break;
    //  case V_SgExpAssignOp:      curprint(ROSE_PYTHON_AUG_EXP_OP);    break;
    //  case V_SgIntegerDivAssignOp: curprint(ROSE_PYTHON_AUG_IDIV_OP); break;
        case V_SgLshiftAssignOp:   curprint(ROSE_PYTHON_AUG_LSHIFT_OP); break;
        case V_SgRshiftAssignOp:   curprint(ROSE_PYTHON_AUG_RSHIFT_OP); break;
        case V_SgModAssignOp:      curprint(ROSE_PYTHON_AUG_MOD_OP);    break;
        case V_SgMultAssignOp:     curprint(ROSE_PYTHON_AUG_MULT_OP);   break;
        case V_SgMinusAssignOp:    curprint(ROSE_PYTHON_AUG_SUB_OP);    break;
        default: { cerr << "Unhandled SgBinaryOp: " << bin_op->class_name() << endl;
            ROSE_ABORT();
        }
    }
    curprint(" ");
    unparseExpression(bin_op->get_rhs_operand(), info);
}

void
Unparse_Python::unparseExprStatement(SgExprStatement* expr_stmt,
                                     SgUnparse_Info& info)
{
    unparseExpression(expr_stmt->get_expression(), info);
}

void
Unparse_Python::unparseFunctionCallExp(SgFunctionCallExp* func_call,
                                       SgUnparse_Info& info)
{
    SgFunctionSymbol* func_sym = func_call->getAssociatedFunctionSymbol();
    string func_name = func_sym->get_name().getString();
    curprint(func_name + string("("));
    unparseExpression(func_call->get_args(), info);
    curprint(")");
}

void
Unparse_Python::unparseFunctionDeclaration(SgFunctionDeclaration* func_decl,
                                           SgUnparse_Info& info)
{
#if 0 // awaiting resolution of abstract handle bug
    SgExprListExp* decoratorList = func_decl->get_decoratorList();
    SgExpressionPtrList& decorators = decoratorList->get_expressions();
    SgExpressionPtrList::iterator dec_it;
    for(dec_it = decorators.begin(); dec_it != decorators.end(); dec_it++) {
        curprint("@");
        unparseExpression(*dec_it, info);
        curprint(string("\n") + ws_prefix(info.get_nestingLevel()));
    }
#endif

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

#if 0 // awaiting resolution of abstract handle bug
    curprint(string("\n") + ws_prefix(info.get_nestingLevel()));
#endif
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
Unparse_Python::unparseIfStmt(SgIfStmt* if_stmt,
                              SgUnparse_Info& info)
{
    curprint("if ");
    unparseStatement(if_stmt->get_conditional(), info);
    curprint(":\n");

    info.inc_nestingLevel();
    unparseStatement(if_stmt->get_true_body(), info);
    info.dec_nestingLevel();

    if (if_stmt->get_false_body() != NULL) {
        curprint(ws_prefix(info.get_nestingLevel()) + "else:\n");
        info.inc_nestingLevel();
        unparseStatement(if_stmt->get_false_body(), info);
        info.dec_nestingLevel();
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
Unparse_Python::unparseLongIntVal(SgLongIntVal* long_int_val,
                                  SgUnparse_Info& info)
{
    stringstream code;
    code << long_int_val->get_value();
    curprint( code.str() );
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

void
Unparse_Python::unparseUnaryOp(SgUnaryOp* unary_op,
                               SgUnparse_Info& info)
{
    switch(unary_op->variantT()) {
        case V_SgUnaryAddOp:      curprint(ROSE_PYTHON_UADD_OP);   break;
        case V_SgMinusOp:         curprint(ROSE_PYTHON_USUB_OP);   break;
        case V_SgBitComplementOp: curprint(ROSE_PYTHON_INVERT_OP); break;
        default: {
            cerr << "Unhandled SgUnaryOp: " << unary_op->class_name() << endl;
            ROSE_ABORT();
        }
    }
    unparseExpression(unary_op->get_operand(), info);
}

void
Unparse_Python::unparseVarRefExp(SgVarRefExp* var_ref_exp,
                                 SgUnparse_Info& info)
{
    SgVariableSymbol* symbol = var_ref_exp->get_symbol();

    stringstream code;
    code << " " << symbol->get_name().str();
    curprint( code.str() );
}

void
Unparse_Python::unparseWhileStmt(SgWhileStmt* while_stmt,
                                 SgUnparse_Info& info)
{
    curprint("while ");
    unparseStatement(while_stmt->get_condition(), info);
    curprint(":\n");

    info.inc_nestingLevel();
    unparseStatement(while_stmt->get_body(), info);
    info.dec_nestingLevel();
}

