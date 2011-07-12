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
    if ( isSgExpression(stmt) != NULL ) {
        unparseExpression( isSgExpression(stmt), info );
        return;
    }

    switch (stmt->variantT()) {

#define CASE_DISPATCH_AND_BREAK(sg_t) \
  case V_Sg ##sg_t : unparse ##sg_t (isSg##sg_t (stmt),info); break;

        CASE_DISPATCH_AND_BREAK(AssertStmt);
        CASE_DISPATCH_AND_BREAK(BasicBlock);
        CASE_DISPATCH_AND_BREAK(BreakStmt);
        CASE_DISPATCH_AND_BREAK(ContinueStmt);
        CASE_DISPATCH_AND_BREAK(ExprStatement);
        CASE_DISPATCH_AND_BREAK(FunctionCallExp);
        CASE_DISPATCH_AND_BREAK(FunctionDeclaration);
        CASE_DISPATCH_AND_BREAK(FunctionDefinition);
        CASE_DISPATCH_AND_BREAK(FunctionParameterList);
        CASE_DISPATCH_AND_BREAK(ForInitStatement);
        CASE_DISPATCH_AND_BREAK(ForStatement);
        CASE_DISPATCH_AND_BREAK(IfStmt);
        CASE_DISPATCH_AND_BREAK(LongIntVal);
        CASE_DISPATCH_AND_BREAK(PythonPrintStmt);
        CASE_DISPATCH_AND_BREAK(PassStatement);
        CASE_DISPATCH_AND_BREAK(ReturnStmt);
        CASE_DISPATCH_AND_BREAK(StringVal);
        CASE_DISPATCH_AND_BREAK(WhileStmt);
        CASE_DISPATCH_AND_BREAK(YieldStatement);
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
        CASE_DISPATCH_AND_BREAK(AssignOp);
        CASE_DISPATCH_AND_BREAK(AssignInitializer);
        CASE_DISPATCH_AND_BREAK(ComplexVal);
        CASE_DISPATCH_AND_BREAK(Comprehension);
        CASE_DISPATCH_AND_BREAK(DeleteExp);
        CASE_DISPATCH_AND_BREAK(DictionaryComprehension);
        CASE_DISPATCH_AND_BREAK(ExprListExp);
        CASE_DISPATCH_AND_BREAK(FunctionCallExp);
        CASE_DISPATCH_AND_BREAK(KeyDatumList);
        CASE_DISPATCH_AND_BREAK(KeyDatumPair);
        CASE_DISPATCH_AND_BREAK(LambdaRefExp);
        CASE_DISPATCH_AND_BREAK(ListComprehension);
        CASE_DISPATCH_AND_BREAK(ListExp);
        CASE_DISPATCH_AND_BREAK(SetComprehension);
        CASE_DISPATCH_AND_BREAK(TupleExp);
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

void
Unparse_Python::unparseComplexVal(SgExpression* exp, SgUnparse_Info& info)
{
    SgComplexVal* val = isSgComplexVal(exp);
    ROSE_ASSERT(val != NULL);
    unparseComplexVal(val, info);
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
Unparse_Python::unparseAssertStmt(SgAssertStmt* sg_assert,
                                  SgUnparse_Info& info)
{
    curprint("assert ");
    unparseExpression(sg_assert->get_test(), info);
}

void
Unparse_Python::unparseAssignOp(SgAssignOp* sg_assign_op,
                                SgUnparse_Info& info)
{
    unparseExpression(sg_assign_op->get_lhs_operand(), info);
    curprint(" = ");
    unparseExpression(sg_assign_op->get_rhs_operand(), info);
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
Unparse_Python::unparseBreakStmt(SgBreakStmt* break_stmt,
                                 SgUnparse_Info& info)
{
    curprint("break");
}

void
Unparse_Python::unparseComprehension(SgComprehension* comp, SgUnparse_Info& info)
{
    curprint(" for ");
    unparseExpression(comp->get_target(), info);
    curprint(" in ");
    unparseExpression(comp->get_iter(), info);

    SgExprListExp* filters = comp->get_filters();
    foreach (SgExpression* if_exp, filters->get_expressions()) {
        curprint(" if ");
        unparseExpression(if_exp, info);
    }
}


void
Unparse_Python::unparseDeleteExp(SgDeleteExp* delete_exp, SgUnparse_Info& info)
{
    curprint("del ");
    unparseExpression(delete_exp->get_variable(), info);
}

void
Unparse_Python::unparseDictionaryComprehension(SgDictionaryComprehension* dict_comp, SgUnparse_Info& info)
{
    curprint("{");
    unparseExpression(dict_comp->get_element(), info);

    SgExprListExp* generators = dict_comp->get_generators();
    foreach (SgExpression* generator, generators->get_expressions())
        unparseExpression(generator, info);

    curprint("}");
}


void
Unparse_Python::unparseComplexVal(SgComplexVal* value,
                                  SgUnparse_Info& info)
{
    unparseExpression(value->get_real_value(), info);
    curprint("+");
    unparseExpression(value->get_imaginary_value(), info);
    curprint("j");
}

void
Unparse_Python::unparseContinueStmt(SgContinueStmt* continue_stmt,
                                    SgUnparse_Info& info)
{
    curprint("continue");
}

void
Unparse_Python::unparseExprListExp(SgExprListExp* expr_list_exp,
                                   SgUnparse_Info& info)
{
    SgExpressionPtrList& exps = expr_list_exp->get_expressions();
    SgExpressionPtrList::iterator exp_it = exps.begin();
    for(exp_it = exps.begin(); exp_it != exps.end(); exp_it++) {
        if (exp_it != exps.begin())
            curprint(", ");
        unparseExpression(*exp_it, info);
    }
}

void
Unparse_Python::unparseExprStatement(SgExprStatement* expr_stmt,
                                     SgUnparse_Info& info)
{
    unparseExpression(expr_stmt->get_expression(), info);
}

void
Unparse_Python::unparseForInitStatement(SgForInitStatement* for_init_stmt,
                                        SgUnparse_Info& info)
{
    foreach (SgStatement* stmt, for_init_stmt->get_init_stmt()) {
        unparseStatement(stmt, info);
    }
}

void
Unparse_Python::unparseForStatement(SgForStatement* for_stmt,
                                    SgUnparse_Info& info)
{
    curprint("for ");
    unparseStatement(for_stmt->get_for_init_stmt(), info);
    curprint(" in ");
    unparseExpression(for_stmt->get_increment(), info);
    curprint(":\n");

    info.inc_nestingLevel();
    unparseStatement(for_stmt->get_loop_body(), info);
    info.dec_nestingLevel();

    if (for_stmt->get_else_body()) {
        curprint(ws_prefix(info.get_nestingLevel()) + "else:\n");
        info.inc_nestingLevel();
        unparseStatement(for_stmt->get_else_body(), info);
        info.dec_nestingLevel();
    }
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
Unparse_Python::unparseKeyDatumList(SgKeyDatumList* kd_list,
                                    SgUnparse_Info& info)
{
    SgKeyDatumPairPtrList pairs = kd_list->get_key_datum_pairs();
    SgKeyDatumPairPtrList::iterator pair_it;

    curprint("{");
    for (pair_it = pairs.begin(); pair_it != pairs.end(); pair_it++) {
        if (pair_it != pairs.begin())
            curprint(", ");
        unparseExpression(*pair_it, info);
    }
    curprint("}");
}

void
Unparse_Python::unparseKeyDatumPair(SgKeyDatumPair* kd_pair,
                                    SgUnparse_Info& info)
{
    unparseExpression(kd_pair->get_key(), info);
    curprint(": ");
    unparseExpression(kd_pair->get_datum(), info);
}

void
Unparse_Python::unparseLambdaRefExp(SgLambdaRefExp* lambda,
                                 SgUnparse_Info& info)
{
    curprint( "lambda " );
    unparseStatement( lambda->get_functionDeclaration()->get_parameterList(), info );
    curprint( ": " );
    SgStatement* lambda_body = lambda->get_body();
    if (isSgBasicBlock(lambda_body)) {
        lambda_body = isSgBasicBlock(lambda_body)->get_statements().front();
    }
    SgExpression* lambda_expr = isSgExpression(lambda_body);
    if (lambda_expr != NULL) {
        unparseExpression( lambda_expr, info );
    } else {
        cout << "Python lambda bodies must contain one SgExpression. (found " <<
            lambda_body->class_name() << " instead)" << endl;
        ROSE_ASSERT(!"lambda body missing SgExpression");
    }
}

void
Unparse_Python::unparseListComprehension(SgListComprehension* list_comp, SgUnparse_Info& info)
{
    SgExprListExp* generators = list_comp->get_generators();

    curprint("[");
    unparseExpression(list_comp->get_element(), info);

    /* SgExprListExps unparse with commas separating elements, so override that behavior here */
    foreach (SgExpression* exp, generators->get_expressions())
        unparseExpression(exp, info);

    curprint("]");
}

void
Unparse_Python::unparseListExp(SgListExp* tuple,
                                SgUnparse_Info& info)
{
    curprint("[");
    SgExpressionPtrList& elts = tuple->get_elements();
    SgExpressionPtrList::iterator elt_it = elts.begin();
    for(elt_it = elts.begin(); elt_it != elts.end(); elt_it++) {
        if (elt_it != elts.begin())
            curprint(", ");
        unparseExpression(*elt_it, info);
    }
    curprint("]");
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
Unparse_Python::unparsePassStatement(SgPassStatement* pass_stmt,
                                     SgUnparse_Info& info)
{
    curprint("pass");
}

void
Unparse_Python::unparsePythonPrintStmt(SgPythonPrintStmt* print_stmt,
                                       SgUnparse_Info& info)
{
    curprint("print ");
    if (print_stmt->get_destination() != NULL) {
        curprint(">>");
        unparseExpression(print_stmt->get_destination(), info);
        curprint(", ");
    }

    if (print_stmt->get_values() != NULL) {
        unparseExpression(print_stmt->get_values(), info);
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
Unparse_Python::unparseSetComprehension(SgSetComprehension* set_comp,
                                        SgUnparse_Info& info)
{
    SgExprListExp* generators = set_comp->get_generators();

    curprint("{");
    unparseExpression(set_comp->get_element(), info);

    /* SgExprListExps unparse with commas separating elements, so override that behavior here */
    foreach (SgExpression* exp, generators->get_expressions())
        unparseExpression(exp, info);

    curprint("}");
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
Unparse_Python::unparseTupleExp(SgTupleExp* tuple,
                                SgUnparse_Info& info)
{
    curprint("(");
    SgExpressionPtrList& elts = tuple->get_elements();
    SgExpressionPtrList::iterator elt_it = elts.begin();
    for(elt_it = elts.begin(); elt_it != elts.end(); elt_it++) {
        if (elt_it != elts.begin())
            curprint(", ");
        unparseExpression(*elt_it, info);

        /* tuples with one item require a trailing comma */
        if (elts.size() == 1)
            curprint(",");
    }
    curprint(")");
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
    curprint( symbol->get_name().str() );
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

void
Unparse_Python::unparseYieldStatement(SgYieldStatement* yield_stmt,
                                      SgUnparse_Info& info)
{
    curprint("yield ");
    unparseExpression(yield_stmt->get_value(), info);
}
