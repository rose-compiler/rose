#include "sage3basic.h"
#include "unparsePython.h"

#ifndef ROSE_USE_INTERNAL_FRONTEND_DEVELOPMENT
   #include "abstract_handle.h"
#endif

#include <iostream>

#include <boost/foreach.hpp>
#define foreach BOOST_FOREACH

#define CASE_DISPATCH_AND_BREAK(sg_t) \
  case V_Sg ##sg_t : unparse ##sg_t (isSg##sg_t (stmt),info); break;

using namespace std;


Unparse_Python::Unparse_Python(Unparser* unp, std::string fname) :
    UnparseLanguageIndependentConstructs(unp, fname)
{
}

Unparse_Python::~Unparse_Python()
{

}

void
Unparse_Python::curprint_indented(std::string txt, SgUnparse_Info& info)
{
    curprint( ws_prefix(info.get_nestingLevel()) );
    curprint( txt );
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

        CASE_DISPATCH_AND_BREAK(AssertStmt);
        CASE_DISPATCH_AND_BREAK(BasicBlock);
        CASE_DISPATCH_AND_BREAK(BreakStmt);
        CASE_DISPATCH_AND_BREAK(CatchOptionStmt);
        CASE_DISPATCH_AND_BREAK(ClassDeclaration);
        CASE_DISPATCH_AND_BREAK(ClassDefinition);
        CASE_DISPATCH_AND_BREAK(ContinueStmt);
        CASE_DISPATCH_AND_BREAK(ExprStatement);
        CASE_DISPATCH_AND_BREAK(ExecStatement);
        CASE_DISPATCH_AND_BREAK(FunctionDeclaration);
        CASE_DISPATCH_AND_BREAK(FunctionDefinition);
        CASE_DISPATCH_AND_BREAK(FunctionParameterList);
        CASE_DISPATCH_AND_BREAK(ForInitStatement);
        CASE_DISPATCH_AND_BREAK(ForStatement);
        CASE_DISPATCH_AND_BREAK(IfStmt);
        CASE_DISPATCH_AND_BREAK(ImportStatement);
        CASE_DISPATCH_AND_BREAK(PythonGlobalStmt);
        CASE_DISPATCH_AND_BREAK(PythonPrintStmt);
        CASE_DISPATCH_AND_BREAK(PassStatement);
        CASE_DISPATCH_AND_BREAK(ReturnStmt);
        CASE_DISPATCH_AND_BREAK(StmtDeclarationStatement);
        CASE_DISPATCH_AND_BREAK(TryStmt);
        CASE_DISPATCH_AND_BREAK(WhileStmt);
        CASE_DISPATCH_AND_BREAK(WithStatement);
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
        CASE_DISPATCH_AND_BREAK(ActualArgumentExpression);
        CASE_DISPATCH_AND_BREAK(AssignOp);
        CASE_DISPATCH_AND_BREAK(AssignInitializer);
        CASE_DISPATCH_AND_BREAK(CallExpression);
        CASE_DISPATCH_AND_BREAK(ComplexVal);
        CASE_DISPATCH_AND_BREAK(Comprehension);
        CASE_DISPATCH_AND_BREAK(ConditionalExp);
        CASE_DISPATCH_AND_BREAK(ClassNameRefExp);
        CASE_DISPATCH_AND_BREAK(DeleteExp);
        CASE_DISPATCH_AND_BREAK(DictionaryComprehension);
        CASE_DISPATCH_AND_BREAK(DoubleVal);
        CASE_DISPATCH_AND_BREAK(ExprListExp);
        CASE_DISPATCH_AND_BREAK(FunctionRefExp);
        CASE_DISPATCH_AND_BREAK(DictionaryExp);
        CASE_DISPATCH_AND_BREAK(KeyDatumPair);
        CASE_DISPATCH_AND_BREAK(LambdaRefExp);
        CASE_DISPATCH_AND_BREAK(ListComprehension);
        CASE_DISPATCH_AND_BREAK(ListExp);
        CASE_DISPATCH_AND_BREAK(LongLongIntVal);
        CASE_DISPATCH_AND_BREAK(PntrArrRefExp);
        CASE_DISPATCH_AND_BREAK(SetComprehension);
        CASE_DISPATCH_AND_BREAK(SubscriptExpression);
        CASE_DISPATCH_AND_BREAK(StringConversion);
        CASE_DISPATCH_AND_BREAK(StringVal);
        CASE_DISPATCH_AND_BREAK(TupleExp);
        CASE_DISPATCH_AND_BREAK(VarRefExp);
        CASE_DISPATCH_AND_BREAK(YieldExpression);

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
        case V_SgAndAssignOp:
        case V_SgIorAssignOp:
        case V_SgXorAssignOp:
        case V_SgDivAssignOp:
        case V_SgExponentiationAssignOp:
        case V_SgIntegerDivideAssignOp:
        case V_SgLshiftAssignOp:
        case V_SgRshiftAssignOp:
        case V_SgModAssignOp:
        case V_SgMultAssignOp:
        case V_SgMinusAssignOp:

        case V_SgLessThanOp:
        case V_SgLessOrEqualOp:
        case V_SgGreaterThanOp:
        case V_SgGreaterOrEqualOp:
        case V_SgEqualityOp:
        case V_SgNotEqualOp:
        case V_SgMembershipOp:
        case V_SgNonMembershipOp:
        case V_SgIsOp:
        case V_SgIsNotOp:
        case V_SgDotExp:
            unparseBinaryOp( isSgBinaryOp(stmt), info );
            break;

        case V_SgUnaryAddOp:
        case V_SgMinusOp:
        case V_SgBitComplementOp:
        case V_SgPointerDerefExp:
            unparseUnaryOp( isSgUnaryOp(stmt), info );
            break;

        case V_SgNaryBooleanOp:
        case V_SgNaryComparisonOp:
            unparseNaryOp( isSgNaryOp(stmt), info );
            break;

        default: {
            cout << "Error: unparseExpression(" << stmt->class_name() << "*) is unimplemented" << endl;
            break;
        }
    }
}

void
Unparse_Python::unparseExpression(SgExpression* expr, SgUnparse_Info& info) {
    bool parenthesize = requiresParentheses(expr, info);
    if (parenthesize) curprint("(");
    unparseLanguageSpecificExpression(expr, info);
    if (parenthesize) curprint(")");
}

void
Unparse_Python::unparseStatement(SgStatement* stmt, SgUnparse_Info& info) {
    unparseLanguageSpecificStatement(stmt, info);
}


void
Unparse_Python::unparseGlobalStmt(SgStatement* stmt, SgUnparse_Info& info) {
    SgGlobal* global = isSgGlobal(stmt);
    ASSERT_not_null(global);

    info.set_current_scope(global);
    foreach (SgDeclarationStatement* child, global->get_declarations()) {
        curprint( ws_prefix(info.get_nestingLevel()) );
        unparseStatement(child, info);
        curprint("\n");
    }
}

void
Unparse_Python::unparseAsSuite(SgStatement* stmt, SgUnparse_Info& info) {
    info.inc_nestingLevel();
    switch (stmt->variantT()) {
        case V_SgBasicBlock: {
            SgBasicBlock* basic_block = isSgBasicBlock(stmt);
            foreach (SgStatement* child, basic_block->get_statements()) {
                curprint( ws_prefix(info.get_nestingLevel()) );
                unparseStatement(child, info);
                curprint("\n");
            }
            break;
        }
        case V_SgClassDefinition: {
            SgClassDefinition* class_def = isSgClassDefinition(stmt);
            foreach (SgDeclarationStatement* child, class_def->get_members()) {
                curprint( ws_prefix(info.get_nestingLevel()) );
                unparseStatement(child, info);
                curprint("\n");
            }
            break;
        }
        case V_SgGlobal: {
            ROSE_ASSERT(!"Cannot unparseAsSuite(SgGlobal*,info). Use unparseGlobalStmt()");
            break;
        }
        default: {
            curprint( ws_prefix(info.get_nestingLevel()) );
            unparseStatement(stmt, info);
            curprint("\n");
            break;
        }
    }
    info.dec_nestingLevel();
}

void
Unparse_Python::unparseOperator(VariantT variant, bool pad) {
    if (pad) curprint(" ");
    switch(variant) {
        case V_SgAddOp:              curprint("+");      break;
        case V_SgAndOp:              curprint("and");    break;
        case V_SgAssignOp:           curprint("=");      break;
        case V_SgBitAndOp:           curprint("&");      break;
        case V_SgBitOrOp:            curprint("|");      break;
        case V_SgBitXorOp:           curprint("^");      break;
        case V_SgDivideOp:           curprint("/");      break;
        case V_SgExponentiationOp:   curprint("**");     break;
        case V_SgIntegerDivideOp:    curprint("//");     break;
        case V_SgLshiftOp:           curprint("<<");     break;
        case V_SgModOp:              curprint("%");      break;
        case V_SgMultiplyOp:         curprint("*");      break;
        case V_SgRshiftOp:           curprint(">>");     break;
        case V_SgSubtractOp:         curprint("-");      break;
        case V_SgPlusAssignOp:       curprint("+=");     break;
        case V_SgAndAssignOp:        curprint("&=");     break;
        case V_SgIorAssignOp:        curprint("|=");     break;
        case V_SgXorAssignOp:        curprint("^=");     break;
        case V_SgDivAssignOp:        curprint("/=");     break;
        case V_SgExponentiationAssignOp: curprint("**="); break;
        case V_SgIntegerDivideAssignOp:  curprint("//="); break;
        case V_SgLshiftAssignOp:     curprint("<<=");    break;
        case V_SgRshiftAssignOp:     curprint(">>=");    break;
        case V_SgModAssignOp:        curprint("%=");     break;
        case V_SgMultAssignOp:       curprint("*=");     break;
        case V_SgMinusAssignOp:      curprint("-=");     break;
        case V_SgNotOp:              curprint("not");    break;
        case V_SgOrOp:               curprint("or");     break;
        case V_SgLessThanOp:         curprint("<");      break;
        case V_SgLessOrEqualOp:      curprint("<=");     break;
        case V_SgGreaterThanOp:      curprint(">");      break;
        case V_SgGreaterOrEqualOp:   curprint(">=");     break;
        case V_SgEqualityOp:         curprint("==");     break;
        case V_SgNotEqualOp:         curprint("!=");     break;
        case V_SgMembershipOp:       curprint("in");     break;
        case V_SgNonMembershipOp:    curprint("not in"); break;
        case V_SgIsOp:               curprint("is");     break;
        case V_SgIsNotOp:            curprint("is not"); break;
        case V_SgDotExp:             curprint(".");      break;
        default: {
            cerr << "Unable to unparse operator: variant = " << variant << endl;
            ROSE_ABORT();
        }
    }
    if (pad) curprint(" ");
}

AssociativitySpecifier
Unparse_Python::getAssociativity(SgExpression* exp) {
    if (exp->variantT() == V_SgExponentiationOp)
        return e_assoc_right;
    else
        return e_assoc_left;
}

PrecedenceSpecifier
Unparse_Python::getPrecedence(SgExpression* exp) {
    switch (exp->variantT()) {
        case V_SgLambdaRefExp:         return 1;
        case V_SgConditionalExp:       return 2;
        case V_SgOrOp:                 return 3;
        case V_SgAndOp:                return 4;
        case V_SgNotOp:                return 5;
        case V_SgNaryBooleanOp:        return 5; // TODO actually depends on which ops

        case V_SgMembershipOp:         return 6;
        case V_SgNonMembershipOp:      return 6;
        case V_SgIsOp:                 return 6;
        case V_SgIsNotOp:              return 6;
        case V_SgLessThanOp:           return 6;
        case V_SgLessOrEqualOp:        return 6;
        case V_SgGreaterThanOp:        return 6;
        case V_SgGreaterOrEqualOp:     return 6;
        case V_SgNotEqualOp:           return 6;
        case V_SgEqualityOp:           return 6;
        case V_SgNaryComparisonOp:     return 6;

        case V_SgBitOrOp:              return 7;
        case V_SgBitAndOp:             return 8;
        case V_SgBitXorOp:             return 9;

        case V_SgLshiftOp:             return 10;
        case V_SgRshiftOp:             return 10;

        case V_SgAddOp:                return 11;
        case V_SgSubtractOp:           return 11;

        case V_SgMultiplyOp:           return 12;
        case V_SgDivideOp:             return 12;
        case V_SgIntegerDivideOp:      return 12;
        case V_SgModOp:                return 12;

        case V_SgUnaryAddOp:           return 13;
        case V_SgMinusOp:              return 13;
        case V_SgBitComplementOp:      return 13;

        case V_SgExponentiationOp:     return 14;

        case V_SgDotExp:               return 15; // attribute refs
        case V_SgCallExpression:       return 15;
        case V_SgSubscriptExpression:  return 15; // slices
        case V_SgPntrArrRefExp:        return 15; // subscript exps

        case V_SgTupleExp:             return 16;
        case V_SgListExp:              return 16;
        case V_SgListComprehension:    return 16;
        case V_SgSetComprehension:     return 16;
        case V_SgDictionaryComprehension: return 16;

     // DQ (9/5/2011): Added default case to avoid warnings generated to catch such issues.
        default:
           {
             return ROSE_UNPARSER_NO_PRECEDENCE;
           }
    }

 // DQ (9/5/2011): I will leave this here, but it is redundant.
    return ROSE_UNPARSER_NO_PRECEDENCE;
}

void
Unparse_Python::unparseComplexVal(SgExpression* exp, SgUnparse_Info& info)
{
    SgComplexVal* val = isSgComplexVal(exp);
    ASSERT_not_null(val);
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
Unparse_Python::unparseActualArgumentExpression(SgActualArgumentExpression* exp,
                                                SgUnparse_Info& info) {
    curprint(exp->get_argument_name().getString());
    curprint("=");
    unparseExpression(exp->get_expression(), info);
}

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
        unparseStatement(child, info);
        curprint("\n");
    }
}
void
Unparse_Python::unparseBaseClassPtrList(SgBaseClassPtrList& inheritances,
                                        SgUnparse_Info& info) {
    SgBaseClassPtrList::iterator base_it;
    for (base_it = inheritances.begin(); base_it != inheritances.end(); base_it++) {
        SgExpBaseClass* exp_base = isSgExpBaseClass(*base_it);
        ROSE_ASSERT(exp_base != NULL && "base class objects for python support must be of type SgExpBaseClass");

        if (base_it != inheritances.begin())
            curprint(", ");
        ASSERT_not_null(exp_base->get_base_class_exp());
        unparseExpression(exp_base->get_base_class_exp(), info);
    }
}

void
Unparse_Python::unparseBinaryOp(SgBinaryOp* bin_op,
                                SgUnparse_Info& info)
{
    unparseExpression(bin_op->get_lhs_operand(), info);
    unparseOperator(bin_op->variantT());
    unparseExpression(bin_op->get_rhs_operand(), info);
}

void
Unparse_Python::unparseBreakStmt(SgBreakStmt* break_stmt,
                                 SgUnparse_Info& info)
{
    curprint("break");
}

void
Unparse_Python::unparseCallExpression(SgCallExpression* func_call,
                                       SgUnparse_Info& info)
{
    unparseExpression(func_call->get_function(), info);
    curprint("(");
    unparseExpression(func_call->get_args(), info);
    curprint(")");
}

void
Unparse_Python::unparseCatchOptionStmt(SgCatchOptionStmt* catch_stmt,
                                       SgUnparse_Info& info)
{
    if (catch_stmt->get_condition() != NULL) {
        curprint_indented("except ", info);
        unparseStatement(catch_stmt->get_condition(), info);
        curprint(":\n");
    } else {
        curprint_indented("except:\n", info);
    }

    unparseAsSuite(catch_stmt->get_body(), info);
}

void
Unparse_Python::unparseClassDeclaration(SgClassDeclaration* class_decl,
                                        SgUnparse_Info& info)
{
    SgExprListExp* decoratorList = class_decl->get_decoratorList();
    if (decoratorList != NULL) {
        foreach (SgExpression* expr, decoratorList->get_expressions()) {
            curprint_indented("@", info);
            unparseExpression(expr, info);
            curprint("\n");
        }
    }

    curprint_indented("class ", info);
    curprint(class_decl->get_name().getString());

    curprint("(");
    ASSERT_not_null(class_decl->get_definition());
    SgBaseClassPtrList& inheritances = class_decl->get_definition()->get_inheritances();
    unparseBaseClassPtrList(inheritances, info);
    curprint("):\n");

    unparseStatement(class_decl->get_definition(), info);
}

void
Unparse_Python::unparseClassDefinition(SgClassDefinition* class_def,
                                       SgUnparse_Info& info)
{
    unparseAsSuite(class_def, info);
}

void
Unparse_Python::unparseClassNameRefExp(SgClassNameRefExp* class_ref,
                                       SgUnparse_Info& info)
{
    curprint( class_ref->get_symbol()->get_name().getString() );
}

void
Unparse_Python::unparseComprehension(SgComprehension* comp, SgUnparse_Info& info)
{
    curprint(" for ");
    unparseExpression(comp->get_target(), info);
    curprint(" in ");
    unparseExpression(comp->get_iter(), info);

    SgExprListExp* filters = comp->get_filters();
    if (filters != NULL) {
        foreach (SgExpression* if_exp, filters->get_expressions()) {
            curprint(" if ");
            unparseExpression(if_exp, info);
        }
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

// DQ (3/28/2017): This should take a SgExpression* to match the base class virtual function signature.
// Fixing this eliminates a warning in Clang.
// void Unparse_Python::unparseDoubleVal(SgDoubleVal* d_val, SgUnparse_Info& info)
void
Unparse_Python::unparseDoubleVal(SgExpression* e, SgUnparse_Info& info)
{
 // DQ (3/28/2017): Adding this here so that the function parameter can be SgExpression*.
    SgDoubleVal* d_val = isSgDoubleVal(e);
    ASSERT_not_null(d_val);

    stringstream code;
    code.setf( ios::showpoint );
    code << d_val->get_value();
    curprint( code.str() );
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
Unparse_Python::unparseConditionalExp(SgConditionalExp* cond_exp,
                                      SgUnparse_Info& info)
{
    unparseExpression(cond_exp->get_true_exp(), info);
    curprint(" if ");
    unparseExpression(cond_exp->get_conditional_exp(), info);
    curprint(" else ");
    unparseExpression(cond_exp->get_false_exp(), info);
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
Unparse_Python::unparseExecStatement(SgExecStatement* exec,
                                     SgUnparse_Info& info)
{
    curprint("exec ");
    unparseExpression(exec->get_executable(), info);

    if (exec->get_globals() != NULL) {
        curprint(" in ");
        unparseExpression(exec->get_globals(), info);

        if (exec->get_locals() != NULL) {
            curprint(", ");
            unparseExpression(exec->get_locals(), info);
        }
    } else {
        ROSE_ASSERT(exec->get_locals() == NULL);
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

    unparseAsSuite(for_stmt->get_loop_body(), info);

    if (for_stmt->get_else_body()) {
        curprint_indented("else:\n", info);
        unparseAsSuite(for_stmt->get_else_body(), info);
    }
}

void
Unparse_Python::unparseFunctionDeclaration(SgFunctionDeclaration* func_decl,
                                           SgUnparse_Info& info)
{
    SgExprListExp* decoratorList = func_decl->get_decoratorList();
    if (decoratorList != NULL) {
        foreach (SgExpression* expr, decoratorList->get_expressions()) {
            curprint_indented("@", info);
            unparseExpression(expr, info);
            curprint("\n");
        }
    }

    curprint("def ");
    curprint(func_decl->get_name().getString());
    curprint("(");
    unparseStatement(func_decl->get_parameterList(), info);
    curprint("):\n");
    unparseStatement(func_decl->get_definition(), info);
}

void
Unparse_Python::unparseFunctionDefinition(SgFunctionDefinition* func_decl,
                                          SgUnparse_Info& info)
{
    unparseAsSuite(func_decl->get_body(), info);
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
Unparse_Python::unparseFunctionRefExp(SgFunctionRefExp* func_ref,
                                      SgUnparse_Info& info)
{
    curprint( func_ref->get_symbol()->get_name().str() );
}

void
Unparse_Python::unparseIfStmt(SgIfStmt* if_stmt,
                              SgUnparse_Info& info)
{
    curprint("if ");
    unparseStatement(if_stmt->get_conditional(), info);
    curprint(":\n");

    unparseAsSuite(if_stmt->get_true_body(), info);

    if (if_stmt->get_false_body() != NULL) {
        curprint_indented("else:\n", info);
        unparseAsSuite(if_stmt->get_false_body(), info);
    }
}

void
Unparse_Python::unparseImportStatement(SgImportStatement* import,
                                       SgUnparse_Info& info)
{
    curprint("import ");
    SgExpressionPtrList& exps = import->get_import_list();
    SgExpressionPtrList::iterator exp_it = exps.begin();
    for(exp_it = exps.begin(); exp_it != exps.end(); exp_it++) {
        if (exp_it != exps.begin())
            curprint(", ");
        unparseExpression(*exp_it, info);
    }
}

void
Unparse_Python::unparseInitializedName(SgInitializedName* init_name,
                                       SgUnparse_Info& info)
{
    if (init_name->get_excess_specifier() == SgInitializedName::e_excess_specifier_positionals)
        curprint("*");
    if (init_name->get_excess_specifier() == SgInitializedName::e_excess_specifier_keywords)
        curprint("**");

    curprint(init_name->get_name().str());
    if (init_name->get_initializer() != NULL) {
        unparseExpression(init_name->get_initializer(), info);
    }
}

void
Unparse_Python::unparseDictionaryExp(SgDictionaryExp* kd_list,
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
Unparse_Python::unparseListExp(SgListExp* list_exp,
                               SgUnparse_Info& info)
{
    curprint("[");
    unparseExprListExp(list_exp, info);
    curprint("]");
}

// DQ (3/28/2017): This should take a SgExpression* to match the base class virtual function signature.
// Fixing this eliminates a warning in Clang.
// void Unparse_Python::unparseLongLongIntVal(SgLongLongIntVal* lli_val, SgUnparse_Info& info)
void
Unparse_Python::unparseLongLongIntVal(SgExpression* e, SgUnparse_Info& info)
{
 // DQ (3/28/2017): Adding this here so that the function parameter can be SgExpression*.
    SgLongLongIntVal* lli_val = isSgLongLongIntVal(e);
    ASSERT_not_null(lli_val);

    stringstream code;
    code << lli_val->get_value();
    curprint( code.str() );
}

void
Unparse_Python::unparseNaryOp(SgNaryOp* op,
                              SgUnparse_Info& info)
{
    SgExpressionPtrList& operands = op->get_operands();
    VariantTList& operators = op->get_operators();

    SgExpressionPtrList::iterator exp_it = operands.begin();
    VariantTList::iterator op_it = operators.begin();

    unparseExpression(*exp_it++, info);
    for ( ; exp_it != operands.end(); exp_it++, op_it++) {
        unparseOperator(*op_it);
        unparseExpression(*exp_it, info);
    }
}

void
Unparse_Python::unparsePntrArrRefExp(SgPntrArrRefExp* ref,
                                     SgUnparse_Info& info)
{
    unparseExpression(ref->get_lhs_operand(), info);
    curprint("[");
    unparseExpression(ref->get_rhs_operand(), info);
    curprint("]");
}

void
Unparse_Python::unparsePointerDerefExp(SgPointerDerefExp* exp,
                                     SgUnparse_Info& info)
{
    curprint("*");
    unparseExpression(exp->get_operand(), info);
}

void
Unparse_Python::unparsePassStatement(SgPassStatement* pass_stmt,
                                     SgUnparse_Info& info)
{
    curprint("pass");
}

void
Unparse_Python::unparsePythonGlobalStmt(SgPythonGlobalStmt* stmt,
                                        SgUnparse_Info& info)
{
    curprint("global ");

    SgInitializedNamePtrList& names = stmt->get_names();
    SgInitializedNamePtrList::iterator name_it;
    for (name_it = names.begin(); name_it != names.end(); name_it++) {
        if (name_it != names.begin())
            curprint(", ");
        unparseInitializedName(*name_it, info);
    }
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
Unparse_Python::unparseStmtDeclarationStatement(SgStmtDeclarationStatement* stmt,
                                                SgUnparse_Info& info)
{
    unparseStatement(stmt->get_statement(), info);
}

void
Unparse_Python::unparseStringConversion(SgStringConversion* str_conv,
                                        SgUnparse_Info& info)
{
    curprint("`");
    unparseExpression(str_conv->get_expression(), info);
    curprint("`");
}

void
Unparse_Python::unparseStringVal(SgExpression* exp,
                                 SgUnparse_Info& info)
{
    SgStringVal* str = isSgStringVal(exp);
    ASSERT_not_null(str);
    unparseStringVal(str, info);
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
Unparse_Python::unparseSubscriptExpression(SgSubscriptExpression* slice,
                                           SgUnparse_Info& info)
{
    if (slice->get_lowerBound() != NULL)
        unparseExpression(slice->get_lowerBound(), info);

    curprint(":");

    if (slice->get_upperBound() != NULL)
        unparseExpression(slice->get_upperBound(), info);

    if (slice->get_stride() != NULL) {
        curprint(":");
        unparseExpression(slice->get_stride(), info);
    }
}


void
Unparse_Python::unparseTryStmt(SgTryStmt* try_stmt,
                               SgUnparse_Info& info)
{
    curprint("try:\n");
    unparseAsSuite(try_stmt->get_body(), info);

    foreach(SgStatement* stmt, try_stmt->get_catch_statement_seq())
        unparseStatement(stmt, info);

    if (try_stmt->get_else_body() != NULL) {
        curprint_indented("else:\n", info);
        unparseAsSuite(try_stmt->get_else_body(), info);
    }

    if (try_stmt->get_finally_body() != NULL) {
        curprint_indented("finally:\n", info);
        unparseAsSuite(try_stmt->get_finally_body(), info);
    }
}

void
Unparse_Python::unparseTupleExp(SgTupleExp* tuple,
                                SgUnparse_Info& info)
{
    curprint("(");
    unparseExprListExp(tuple, info);
    curprint(")");
}

void
Unparse_Python::unparseUnaryOp(SgUnaryOp* unary_op,
                               SgUnparse_Info& info)
{
    switch(unary_op->variantT()) {
        case V_SgUnaryAddOp:      curprint("+"); break;
        case V_SgMinusOp:         curprint("-"); break;
        case V_SgBitComplementOp: curprint("~"); break;
        case V_SgPointerDerefExp: curprint("*"); break;
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

    unparseAsSuite(while_stmt->get_body(), info);

    if (while_stmt->get_else_body() != NULL) {
        curprint_indented("else:\n", info);
        unparseAsSuite(while_stmt->get_else_body(), info);
    }
}

void
Unparse_Python::unparseWithStatement(SgWithStatement* with_stmt,
                                     SgUnparse_Info& info)
{
    curprint("with ");

    // special handling for targetted with stmts ('with X as Y: ...')
    // in targetting with expressions, SgWithStatement::p_expression is
    // an SgAssignOp with lhs=Y and rhs=X.
    SgAssignOp* assign_op = isSgAssignOp(with_stmt->get_expression());
    if (assign_op != NULL) {
        unparseExpression(assign_op->get_rhs_operand(), info);
        curprint(" as ");
        unparseExpression(assign_op->get_lhs_operand(), info);
    } else {
        unparseExpression(with_stmt->get_expression(), info);
    }

    curprint(":\n");

    unparseAsSuite(with_stmt->get_body(), info);
}

void
Unparse_Python::unparseYieldExpression(SgYieldExpression* yield_exp,
                                      SgUnparse_Info& info)
{
    bool atom = isSgExpression(yield_exp->get_parent()) != NULL;
    if (atom) curprint("(");
    curprint("yield ");
    unparseExpression(yield_exp->get_value(), info);
    if (atom) curprint(")");
}

bool
Unparse_Python::requiresParentheses(SgExpression* expr, SgUnparse_Info& info) {
    SgExpression* parent = isSgExpression(expr->get_parent());
    if (parent == NULL) return false;

    PrecedenceSpecifier exprPrecedence = getPrecedence(expr);
    PrecedenceSpecifier parentPrecedence = getPrecedence(parent);
    AssociativitySpecifier assoc = getAssociativity(parent);
    if      (exprPrecedence == ROSE_UNPARSER_NO_PRECEDENCE)      return false;
    else if (parentPrecedence == ROSE_UNPARSER_NO_PRECEDENCE)    return false;
    else if (assoc == e_assoc_right && exprPrecedence >  parentPrecedence) return false;
    else if (assoc == e_assoc_left  && exprPrecedence >= parentPrecedence) return false;
    else if (assoc == e_assoc_left  && exprPrecedence <  parentPrecedence) return true;
    else if (assoc == e_assoc_right && exprPrecedence <= parentPrecedence) return true;
    else {
        ROSE_ASSERT(!"cannot determine if parens are required in UnparseLanguageIndependentConstructs::requiresParentheses");
        abort(); // in case ROSE_ASSERT is disabled
    }
}
