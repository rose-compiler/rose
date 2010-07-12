#include "eventReverser.h"
#include "utilities/Utilities.h"

#include <stack>
#include <boost/algorithm/string.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>

#define foreach BOOST_FOREACH

using namespace std;
using namespace boost;
using namespace SageBuilder;
using namespace SageInterface;

/** A list of all the expression handlers that we will try first.
  * If these handlers can't reverse an expression, we fall back to state saving. */
/*static*/ std::vector <boost::function<ExpPair (SgExpression*) > > EventReverser::expressionHandlers;

set<SgFunctionDeclaration*> EventReverser::func_processed_;
const ExpPair EventReverser::NULL_EXP_PAIR = ExpPair(NULL, NULL);
const StmtPair EventReverser::NULL_STMT_PAIR = StmtPair(NULL, NULL);

EventReverser::EventReverser(SgFunctionDeclaration* func_decl, DFAnalysis* analysis) 
: func_decl_(func_decl), 
    function_name_(func_decl_->get_name()), 
    flag_stack_name_(function_name_ + "_branch_flags"),
    int_stack_name_(function_name_ + "_int_values"),
    float_stack_name_(function_name_ + "_float_values"),
    counter_stack_name_(function_name_ + "_loop_counters"),
    counter_(0),
    branch_mark_(-1)
{
}

vector<SgVariableDeclaration*> EventReverser::getVarDeclarations()
{
    vector<SgVariableDeclaration*> decls;

    SgType* stack_type = buildPointerType(buildStructDeclaration("IntStack")->get_type());
    decls.push_back(buildVariableDeclaration(flag_stack_name_, stack_type));
    decls.push_back(buildVariableDeclaration(int_stack_name_, stack_type));
    decls.push_back(buildVariableDeclaration(counter_stack_name_, stack_type));

    // Float type stack
    decls.push_back(buildVariableDeclaration(float_stack_name_, stack_type));

    return decls;
}

vector<SgAssignOp*> EventReverser::getVarInitializers()
{
    vector<SgAssignOp*> inits;
    SgType* stack_type = buildPointerType(buildStructDeclaration("IntStack")->get_type());

    // Initialize the flag stack object.
    inits.push_back(buildAssignOp(
                buildVarRefExp(flag_stack_name_),
                buildFunctionCallExp("buildIntStack", stack_type)));

    // Initialize the integer stack object.
    inits.push_back(buildAssignOp(
                buildVarRefExp(int_stack_name_),
                buildFunctionCallExp("buildIntStack", stack_type)));

    // Initialize the integer stack object.
    inits.push_back(buildAssignOp(
                buildVarRefExp(float_stack_name_),
                buildFunctionCallExp("buildIntStack", stack_type)));

    // Initialize the loop counter stack object.
    inits.push_back(buildAssignOp(
                buildVarRefExp(counter_stack_name_),
                buildFunctionCallExp("buildIntStack", stack_type)));

    return inits;
}

ExpPair EventReverser::instrumentAndReverseExpression(SgExpression* exp)
{
	foreach (function<ExpPair (SgExpression*)> expressionHandler, expressionHandlers)
	{
		ExpPair result = expressionHandler(exp);
		if (result != NULL_EXP_PAIR)
		{
			return result;
		}
	}

    // if this expression is a binary one
    if (SgBinaryOp* bin_op = isSgBinaryOp(exp))
        return processBinaryOp(bin_op);

    // if the expression is a unary one
    if (SgUnaryOp* unary_op = isSgUnaryOp(exp))
        return processUnaryOp(unary_op);

    // process the conditional expression (?:).
    if (SgConditionalExp* cond_exp = isSgConditionalExp(exp))
        return processConditionalExp(cond_exp);

    // process the function call expression
    if (SgFunctionCallExp* func_exp = isSgFunctionCallExp(exp))
        return processFunctionCallExp(func_exp);

    if (isSgVarRefExp(exp) || isSgValueExp(exp) || isSgSizeOfOp(exp))
		return ExpPair(copyExpression(exp), copyExpression(exp));

	printf("WARNING: The following expression of type %s was not handled in the reversal: %s\n",
			exp->class_name().c_str(), exp->unparseToString().c_str());

    return ExpPair(copyExpression(exp), copyExpression(exp));
}


StmtPair EventReverser::instrumentAndReverseStatement(SgStatement* stmt)
{
    if (stmt == NULL)
        return NULL_STMT_PAIR;

    if (SgExprStatement* exp_stmt = isSgExprStatement(stmt))
        return processExprStatement(exp_stmt);

    // if it's a block, process each statement inside
    if (SgBasicBlock* body = isSgBasicBlock(stmt))
        return processBasicBlock(body);

    // if it's a local variable declaration
    if (SgVariableDeclaration* var_decl = isSgVariableDeclaration(stmt))
        return processVariableDeclaration(var_decl);

    // if it's a if statement, process it according to the rule
    if (SgIfStmt* if_stmt = isSgIfStmt(stmt))
        return processIfStmt(if_stmt);

    if (SgForStatement* for_stmt = isSgForStatement(stmt))
        return processForStatement(for_stmt);

    if (SgForInitStatement* for_init_stmt = isSgForInitStatement(stmt))
        return processForInitStatement(for_init_stmt);

    if (SgWhileStmt* while_stmt = isSgWhileStmt(stmt))
        return processWhileStmt(while_stmt);

    if (SgDoWhileStmt* do_while_stmt = isSgDoWhileStmt(stmt))
        return processDoWhileStmt(do_while_stmt);

    if (SgSwitchStatement* switch_stmt = isSgSwitchStatement(stmt))
        return processSwitchStatement(switch_stmt);

#if 0
    if (SgBreakStmt* break_stmt = isSgBreakStmt(stmt))
        return copyStatement(break_stmt);

    if (SgContinueStmt* continue_stmt = isSgContinueStmt(stmt))
        return copyStatement(continue_stmt);

    if (SgReturnStmt* return_stmt = isSgReturnStmt(stmt))
        return copyStatement(return_stmt);
#endif

    // The following output should include break, continue and other ones.
    return StmtPair(
            copyStatement(stmt),
            copyStatement(stmt));
}

// This function add the loop counter related statements (counter declaration, increase, and store)
// to a forward for statement. 
SgStatement* EventReverser::assembleLoopCounter(SgStatement* loop_stmt)
{
    string counter_name = function_name_ + "_loop_counter_" + lexical_cast<string>(counter_++);
    backstroke_util::validateName(counter_name, loop_stmt);

    SgStatement* counter_decl = buildVariableDeclaration(
            counter_name, 
            buildIntType(), 
            buildAssignInitializer(buildIntVal(0)));

#if 1
    SgStatement* incr_counter = buildExprStatement(
            buildPlusPlusOp(buildVarRefExp(counter_name), SgUnaryOp::prefix));

    if (SgForStatement* for_stmt = isSgForStatement(loop_stmt))
    {
        SgStatement* loop_body = for_stmt->get_loop_body();
        if (SgBasicBlock* block_body = isSgBasicBlock(loop_body))
            block_body->append_statement(incr_counter);
        else
        {
            block_body = buildBasicBlock(loop_body, incr_counter);
            for_stmt->set_loop_body(block_body);
            block_body->set_parent(for_stmt);
        }
    }
    else if (SgWhileStmt* while_stmt = isSgWhileStmt(loop_stmt))
    {
        SgStatement* loop_body = while_stmt->get_body();
        if (SgBasicBlock* block_body = isSgBasicBlock(loop_body))
            block_body->append_statement(incr_counter);
        else
        {
            block_body = buildBasicBlock(loop_body, incr_counter);
            while_stmt->set_body(block_body);
            block_body->set_parent(while_stmt);
        }
    }
    else if (SgDoWhileStmt* do_while_stmt = isSgDoWhileStmt(loop_stmt))
    {
        SgStatement* loop_body = do_while_stmt->get_body();
        if (SgBasicBlock* block_body = isSgBasicBlock(loop_body))
            block_body->append_statement(incr_counter);
        else
        {
            block_body = buildBasicBlock(loop_body, incr_counter);
            do_while_stmt->set_body(block_body);
            block_body->set_parent(do_while_stmt);
        }
    }
#endif

    SgStatement* push_counter = buildFunctionCallStmt(
            "push", 
            buildIntType(), 
            buildExprListExp(
                buildVarRefExp(counter_stack_name_),
                buildVarRefExp(counter_name))); 

    return buildBasicBlock(counter_decl, loop_stmt, push_counter);
}

SgStatement* EventReverser::buildForLoop(SgStatement* loop_body)
{
    // build a simple for loop like: for (int i = N; i > 0; --i)

    string counter_name = "i";
    backstroke_util::validateName(counter_name, loop_body);

    SgStatement* init = buildVariableDeclaration(
            counter_name, buildIntType(), buildAssignInitializer(popLoopCounter()));
    SgStatement* test = buildExprStatement(
            buildBinaryExpression<SgGreaterThanOp>(
                buildVarRefExp(counter_name), 
                buildIntVal(0)));
    SgExpression* incr = buildMinusMinusOp(buildVarRefExp(counter_name), SgUnaryOp::prefix);

    SgStatement* for_stmt = buildForStatement(init, test, incr, loop_body);
    return for_stmt;
}

SgExpression* EventReverser::isStateVar(SgExpression* exp)
{
    // a->b
    if (SgArrowExp* arr_exp = isSgArrowExp(exp))
    {
        return arr_exp;
    }
    // a[b]
    else if (SgPntrArrRefExp* ref_exp = isSgPntrArrRefExp(exp))
    {
        if (isSgArrowExp(ref_exp->get_lhs_operand()))
            return ref_exp;
    }
    return NULL;
}

bool EventReverser::toSave(SgExpression* exp)
{
    return true;
}

map<SgFunctionDeclaration*, FuncDeclPair> EventReverser::outputFunctions()
{
    SgBasicBlock* body = func_decl_->get_definition()->get_body();
    // Function body is a basic block, which is a kind of statement.
    SgStatement *fwd_body, *rvs_body;
    tie(fwd_body, rvs_body) = instrumentAndReverseStatement(body);

	//Generate the forward function
    SgName func_name = func_decl_->get_name() + "_forward";
    SgFunctionDeclaration* fwd_func_decl = 
        buildDefiningFunctionDeclaration(func_name, func_decl_->get_orig_return_type(), 
                isSgFunctionParameterList(copyStatement(func_decl_->get_parameterList())));
    SgFunctionDefinition* fwd_func_def = fwd_func_decl->get_definition();
    fwd_func_def->set_body(isSgBasicBlock(fwd_body));
    fwd_body->set_parent(fwd_func_def);

	//Generate the reverse function
    func_name = func_decl_->get_name() + "_reverse";
    SgFunctionDeclaration* rvs_func_decl = 
        buildDefiningFunctionDeclaration(func_name, func_decl_->get_orig_return_type(), 
                isSgFunctionParameterList(copyStatement(func_decl_->get_parameterList()))); 
    SgFunctionDefinition* rvs_func_def = rvs_func_decl->get_definition();
    rvs_func_def->set_body(isSgBasicBlock(rvs_body));
    rvs_body->set_parent(rvs_func_def);

    output_func_pairs_[func_decl_] = FuncDeclPair(fwd_func_decl, rvs_func_decl);
    return output_func_pairs_;
}

// Push an integer into integer stack. Can be used to save states. 
SgExpression* EventReverser::pushIntVal(SgExpression* var)
{
    // Cannot use branch mark here!!!
    if (1)//(branch_mark_ < 0)
        return buildFunctionCallExp(
                "push", 
                buildIntType(), 
                buildExprListExp(
                    buildVarRefExp(int_stack_name_), 
                    var));
    else
        return buildFunctionCallExp(
                "push", 
                buildIntType(), 
                buildExprListExp(
                    buildVarRefExp(int_stack_name_), 
                    var,
                    buildIntVal(branch_mark_)));
}

// Pop the stack and get the value.
SgExpression* EventReverser::popIntVal()
{
    return buildFunctionCallExp("pop", buildVoidType(), 
            buildExprListExp(buildVarRefExp(int_stack_name_)));
}

// Push a float into float stack. Can be used to save states. 
SgExpression* EventReverser::pushFloatVal(SgExpression* var)
{
    return buildFunctionCallExp(
            "push", 
            buildIntType(), 
            buildExprListExp(
                buildVarRefExp(float_stack_name_), 
                var)); 
}

// Pop the stack and get the value.
SgExpression* EventReverser::popFloatVal()
{
    return buildFunctionCallExp("pop", buildVoidType(), 
            buildExprListExp(buildVarRefExp(float_stack_name_)));
}

// For a local variable, return two statements to store its value and 
// declare and assign the retrieved value to it.
StmtPair EventReverser::pushAndPopLocalVar(SgVariableDeclaration* var_decl)
{
    const SgInitializedNamePtrList& names = var_decl->get_variables();
    ROSE_ASSERT(names.size() == 1);
    SgInitializedName* init_name = names[0];

    SgStatement* store_var = buildExprStatement(
            pushIntVal(buildVarRefExp(init_name)));

    SgStatement* decl_var = buildVariableDeclaration(
            init_name->get_name(),
            init_name->get_type(),
            buildAssignInitializer(popIntVal()));
    return StmtPair(store_var, decl_var);
}

// For and/or & conditional expressions. The value pushed is either 0 or 1. 
SgExpression* EventReverser::putBranchFlagExp(SgExpression* exp)
{
    return buildFunctionCallExp(
            "push", 
            buildIntType(), 
            buildExprListExp(
                buildVarRefExp(flag_stack_name_), 
                exp, 
                buildIntVal(branch_mark_)));
}

SgStatement* EventReverser::putBranchFlagStmt(bool flag)
{
    return buildExprStatement(
            buildFunctionCallExp(
                "push", 
                buildIntType(), 
                buildExprListExp(
                    buildVarRefExp(flag_stack_name_), 
                    buildIntVal(flag))));
}

// Return the statement which checks the branch flag
SgExpression* EventReverser::checkBranchFlagExp()
{
    return buildFunctionCallExp("pop", buildVoidType(), 
            buildExprListExp(buildVarRefExp(flag_stack_name_)));
}

SgStatement* EventReverser::checkBranchFlagStmt()
{
    return buildExprStatement(checkBranchFlagExp());
}


SgExpression* EventReverser::popLoopCounter()
{
    return buildFunctionCallExp("pop", buildVoidType(), 
            buildExprListExp(buildVarRefExp(counter_stack_name_)));
}


/**********************************************************
 * End of definitions of member funtions of EventReverser *
 **********************************************************/

