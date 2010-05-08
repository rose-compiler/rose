// Example ROSE Translator: used within ROSE/tutorial

#include "reverse.h"
#include "facilityBuilder.h"
#include <stack>
#include <boost/algorithm/string.hpp>
#include <boost/tuple/tuple.hpp>

using namespace std;
using namespace boost;

#define ISZERO(value, ValType) \
    if (ValType* val = is##ValType(value)) \
return val->get_value() == 0;

bool isZero(SgValueExp* value)
{
    if (!value)
	ROSE_ASSERT(false);

    ISZERO(value, SgBoolValExp);
    ISZERO(value, SgCharVal);
    ISZERO(value, SgDoubleVal);
    ISZERO(value, SgEnumVal);
    ISZERO(value, SgFloatVal);
    ISZERO(value, SgIntVal);
    ISZERO(value, SgLongDoubleVal);
    ISZERO(value, SgLongIntVal);
    ISZERO(value, SgLongLongIntVal);
    ISZERO(value, SgShortVal);
    ISZERO(value, SgUnsignedCharVal);
    ISZERO(value, SgUnsignedIntVal);
    ISZERO(value, SgUnsignedLongLongIntVal);
    ISZERO(value, SgUnsignedLongVal);
    ISZERO(value, SgUnsignedShortVal);

    ROSE_ASSERT(false);
    return true;
}

set<SgFunctionDeclaration*> EventReverser::func_processed_;
const ExpPair EventReverser::NULL_EXP_PAIR = ExpPair(NULL, NULL);
const StmtPair EventReverser::NULL_STMT_PAIR = StmtPair(NULL, NULL);


SgExpression* EventReverser::reverseExpression(SgExpression* exp)
{
    return NULL;
}


ExpPair EventReverser::instrumentAndReverseExpression(SgExpression* exp)
{
    // We can just make copy of statements.
    // Before processing this expression, we replace it with its copy. This can avoid
    // to modify the expression passed in, or the same node appears in AST more than once.
    //exp = copyExpression(exp);

    // if this expression is a binary one
    if (SgBinaryOp* bin_op = isSgBinaryOp(exp))
    {
	// a binary operation contains two expressions which need to be processed first
	SgExpression *fwd_lhs_exp, *fwd_rhs_exp, *rev_lhs_exp, *rev_rhs_exp;
	tie(fwd_rhs_exp, rev_rhs_exp) = instrumentAndReverseExpression(bin_op->get_rhs_operand_i());
	tie(fwd_lhs_exp, rev_lhs_exp) = instrumentAndReverseExpression(bin_op->get_lhs_operand_i());

	// the default forward version. Unless state saving is needed, we use the following one.
	SgBinaryOp* fwd_exp = isSgBinaryOp(copyExpression(exp));
	fwd_exp->set_lhs_operand_i(fwd_lhs_exp);
	fwd_exp->set_rhs_operand_i(fwd_rhs_exp);

	// if the left-hand side of the assign-op is the state
	// (note that only in this situation do we consider to reverse this expression)
	if (SgExpression* var = isStateVar(bin_op->get_lhs_operand_i()))
	{
	    // The operators +=, -=, *=, <<=, ^= can be reversed without state saving,
	    // if the rhs operand is a constant value or state variable.
	    // (*= and <<= may be optional, since the possibility of overflow).
	    // Also note that the expression in the 'sizeof' operator will not be evaluated,
	    // which we don't have to process. For example, i += sizeof(++i) equals to i += sizeof(i)
	    SgExpression* rhs_operand = bin_op->get_rhs_operand_i();
	    if (isStateVar(rhs_operand) || 
		    isSgValueExp(rhs_operand) ||
		    isSgSizeOfOp(rhs_operand))
	    {
		if (isSgPlusAssignOp(exp))
		    return ExpPair(
			    fwd_exp,
			    buildBinaryExpression<SgMinusAssignOp>(
				var, rhs_operand));

		if (isSgMinusAssignOp(exp))
		    return ExpPair(
			    fwd_exp,
			    buildBinaryExpression<SgPlusAssignOp>(
				var, rhs_operand));

		// we must ensure that the rhs operand of *= is not ZERO
		if (isSgMultAssignOp(exp) && !isZero(isSgValueExp(rhs_operand)))
		    // if the rhs_operand is a value and the value is not 0
		    return ExpPair(
			    fwd_exp,
			    buildBinaryExpression<SgDivAssignOp>(
				var, rhs_operand));

		if (isSgLshiftAssignOp(exp))
		    return ExpPair(
			    fwd_exp,
			    buildBinaryExpression<SgRshiftAssignOp>(
				var, rhs_operand));

		if (isSgXorAssignOp(exp))
		    return ExpPair(
			    fwd_exp,
			    buildBinaryExpression<SgXorAssignOp>(
				var, rhs_operand));
	    }

	    // the following operations which alter the value of the lhs operand
	    // can be reversed by state saving
	    if (isSgAssignOp(exp) ||
		    isSgPlusAssignOp(exp) ||
		    isSgMinusAssignOp(exp) ||
		    isSgMultAssignOp(exp) ||
		    isSgDivAssignOp(exp) ||
		    isSgModAssignOp(exp) ||
		    isSgIorAssignOp(exp) ||
		    isSgAndAssignOp(exp) ||
		    isSgXorAssignOp(exp) ||
		    //isSgPointerAssignOp(exp) ||          // what is this?
		    isSgLshiftAssignOp(exp) ||
		    isSgRshiftAssignOp(exp))
	    {
		SgBinaryOp* exp_copy = isSgBinaryOp(copyExpression(exp));
		exp_copy->set_rhs_operand_i(fwd_rhs_exp);

		SgExpression* state_var = getStateVar(var);

		// Save the state (we cannot use the default forward expression any more).
		// Here we use comma operator expression to implement state saving.
		SgExpression* fwd_exp = buildBinaryExpression<SgCommaOpExp>(
			buildBinaryExpression<SgAssignOp>(state_var, var),
			exp_copy);

		// retrieve the state
		SgExpression* rev_exp = buildBinaryExpression<SgAssignOp>(
			copyExpression(var),
			copyExpression(state_var));

		// If the rhs operand expression can be reversed, we have to deal with 
		// both sides at the same time. For example: b = ++a, where a and b are 
		// both state variables.
		if (rev_rhs_exp)
		    rev_exp = buildBinaryExpression<SgCommaOpExp>(rev_rhs_exp, rev_exp);
		return ExpPair(fwd_exp, rev_exp);
	    }
	}
	// The arrow expression should be regarded as a variable.
	// FIXME What if the rhs operand is a member function?
	if (isSgArrowExp(exp))
	    return ExpPair(copyExpression(exp), copyExpression(exp));

	// logical 'and' and 'or' should be taken care of since short circuit may happen
	if (SgAndOp* and_op = isSgAndOp(exp))
	{
	    if (rev_rhs_exp == NULL)
		return ExpPair(fwd_exp, rev_lhs_exp);

	    // Forward event should record the lhs value of the or operator.
	    SgExpression* fwd_exp = buildBinaryExpression<SgAndOp>(
		    putBranchFlagExp(fwd_lhs_exp),
		    fwd_rhs_exp);

	    SgExpression* rev_exp;
	    // We also have to check if rev_lhs_exp == NULL
	    if (rev_lhs_exp)
		// Use the flag recorded to choose whether to reverse just lhs operand or both.
		rev_exp = buildConditionalExp(
			checkBranchFlagExp(),
			buildBinaryExpression<SgCommaOpExp>(rev_rhs_exp, rev_lhs_exp),
			rev_lhs_exp);
	    else
		rev_exp = buildConditionalExp(
			checkBranchFlagExp(),
			rev_rhs_exp,
			buildIntVal(0));

	    return ExpPair(fwd_exp, rev_exp);
	}

	if (SgOrOp* or_op = isSgOrOp(exp))
	{
	    if (rev_rhs_exp == NULL)
		return ExpPair(fwd_exp, rev_lhs_exp);

	    // Forward event should record the lhs value of the or operator.
	    SgExpression* fwd_exp = buildBinaryExpression<SgOrOp>(
		    putBranchFlagExp(fwd_lhs_exp),
		    fwd_rhs_exp);

	    SgExpression* rev_exp;
	    // We also have to check if rev_lhs_exp == NULL
	    if (rev_lhs_exp)
		// Use the flag recorded to choose whether to reverse just lhs operand or both.
		rev_exp = buildConditionalExp(
			checkBranchFlagExp(),
			rev_lhs_exp,
			buildBinaryExpression<SgCommaOpExp>(rev_rhs_exp, rev_lhs_exp));
	    else
		rev_exp = buildConditionalExp(
			checkBranchFlagExp(),
			buildIntVal(0),
			rev_rhs_exp);
	    return ExpPair(fwd_exp, rev_exp);
	}


// The reverse version of an expression should still be an expression, not NULL
#if 1
	// The following is the default reverse expression which is a combination
	// of reverse expressions of both sides.
	SgExpression* rev_exp = NULL;
	if (rev_rhs_exp && rev_lhs_exp)
	    rev_exp = buildBinaryExpression<SgCommaOpExp>(rev_rhs_exp, rev_lhs_exp);
	else if (rev_lhs_exp)
	    rev_exp = rev_lhs_exp;
	else if (rev_rhs_exp)
	    rev_exp = rev_rhs_exp;
#endif

	return ExpPair(fwd_exp, rev_exp);
    }

    // if the expression is a unary one
    if (SgUnaryOp* unary_op = isSgUnaryOp(exp))
    {
	SgExpression *fwd_operand_exp, *rev_operand_exp;
	tie(fwd_operand_exp, rev_operand_exp) = instrumentAndReverseExpression(unary_op->get_operand());

	// The default forward version should instrument its operand.
	// For example, ++(a = 5), where a is a state variable.
	// Its forward expression is like ++(s = a, a = 5), where s is for state saving.
	SgUnaryOp* fwd_exp = isSgUnaryOp(copyExpression(exp));
	ROSE_ASSERT(fwd_exp);
	fwd_exp->set_operand_i(fwd_operand_exp);

	// if the left-hand side of the assign-op is the state
	// (note that only in this situation do we consider to reverse this expression)
	if (SgExpression* var = isStateVar(unary_op->get_operand_i()))
	{
	    // ++ and -- can both be reversed without state saving
	    if (SgPlusPlusOp* pp_op = isSgPlusPlusOp(exp))
	    {
		SgUnaryOp::Sgop_mode pp_mode = pp_op->get_mode();
		if (pp_mode == SgUnaryOp::prefix) pp_mode = SgUnaryOp::postfix;
		else pp_mode = SgUnaryOp::prefix;
		return ExpPair(
			fwd_exp,
			buildMinusMinusOp(var, pp_mode));
	    }

	    if (SgMinusMinusOp* mm_op = isSgMinusMinusOp(exp))
	    {
		SgUnaryOp::Sgop_mode mm_mode = mm_op->get_mode();
		if (mm_mode == SgUnaryOp::prefix) mm_mode = SgUnaryOp::postfix;
		else mm_mode = SgUnaryOp::prefix;
		return ExpPair(
			fwd_exp,
			buildPlusPlusOp(var, mm_mode));
	    }
	}

	SgExpression* rev_exp = rev_operand_exp;
	return ExpPair(fwd_exp, rev_exp);
    }

    // process the conditional expression (?:).
    if (SgConditionalExp* cond_exp = isSgConditionalExp(exp))
    {
	// Since conditional expression is quite like a if statement,
	// we will reverse it using flags like reversing if statement
	SgExpression *fwd_cond_exp, *fwd_true_exp, *fwd_false_exp;
	SgExpression *rev_cond_exp, *rev_true_exp, *rev_false_exp;

	tie(fwd_cond_exp, rev_cond_exp) = instrumentAndReverseExpression(cond_exp->get_conditional_exp());
	tie(fwd_true_exp, rev_true_exp) = instrumentAndReverseExpression(cond_exp->get_true_exp());
	tie(fwd_false_exp, rev_false_exp) = instrumentAndReverseExpression(cond_exp->get_false_exp());

	fwd_true_exp = buildBinaryExpression<SgCommaOpExp>(putBranchFlagExp(true), fwd_true_exp);
	fwd_false_exp = buildBinaryExpression<SgCommaOpExp>(putBranchFlagExp(false),fwd_false_exp);

	// Since the conditional operator does not allow NULL expressions appear to its 2nd and 3rd
	// operand, we have to pull a place holder which does nothing if the reverse expression of that
	// operand is NULL. 0 can represents arithmetic or pointer types.
	// FIXME If the operand is of type void or structure type???
	// If its return value does not matter, can we return 0 always? 
	// (For example,     
	//        bool_val ? foo1(i) : foo2()        
	// where foo1 and foo2 return void. Then its reverse expression is
	//        flag? (foo1_rev(i), 0) : 0
	if (!rev_true_exp) rev_true_exp = buildIntVal(0);
	if (!rev_false_exp) rev_false_exp = buildIntVal(0);

	SgExpression* fwd_exp = buildConditionalExp(fwd_cond_exp, fwd_true_exp, fwd_false_exp); 
	SgExpression* rev_exp = buildConditionalExp(checkBranchFlagExp(), rev_true_exp, rev_false_exp);
	if (rev_cond_exp)
	    rev_exp = buildBinaryExpression<SgCommaOpExp>(rev_exp, rev_cond_exp);

	// FIXME:!!! the reverse version is not right now! Note the return value!

	return ExpPair(fwd_exp, rev_exp);
    }

    // process the function call expression
    if (SgFunctionCallExp* func_exp = isSgFunctionCallExp(exp))
    {
	// When there is a function call in the event function, we should consider to
	// instrument and reverse this function.
	// First, check if the parameters of this function call contain any model state.
	bool needs_reverse = false;
	foreach (SgExpression* exp, func_exp->get_args()->get_expressions())
	{
	    if (isStateVar(exp)) 
	    {
		needs_reverse = true;
		break;
	    }
	}
	if (!needs_reverse) 
	{
	    // If the function call does not modify model's state, we can 
	    // just reverse its parameters
	    SgExpression *fwd_exp = NULL, *rev_exp = NULL;
	    vector<SgExpression*> fwd_args;
	    foreach (SgExpression* exp, func_exp->get_args()->get_expressions())
	    {
		SgExpression *fwd_arg, *rev_arg;
		tie(fwd_arg, rev_arg) = instrumentAndReverseExpression(exp);
		fwd_args.push_back(fwd_arg);
		rev_exp = (rev_exp == NULL) ?
		    rev_arg :
		    buildBinaryExpression<SgCommaOpExp>(rev_arg, rev_exp);
	    }
	    // The forward expression is still a function call, while the reverse expression
	    // just reverse it's parameters. 
	    SgExprListExp* fwd_para_list = buildExprListExp_nfi(fwd_args);
	    fwd_exp = buildFunctionCallExp(
		    copyExpression(func_exp->get_function()), 
		    fwd_para_list);
	    return ExpPair(fwd_exp, rev_exp);
	}


	// Then we have to reverse the function called
	SgFunctionDeclaration* func_decl = func_exp->getAssociatedFunctionDeclaration();
	EventReverser func_generator(func_decl);
	vector<FuncDeclPair> func_pairs = func_generator.outputFunctions();
	if (func_processed_.count(func_decl) == 0)
	    foreach(const FuncDeclPair& func_pair, func_pairs)
		output_func_pairs_.push_back(func_pair);
	SgFunctionDeclaration* fwd_func = func_pairs.back().first;
	SgFunctionDeclaration* rev_func = func_pairs.back().second;

	SgExprListExp* fwd_args = buildExprListExp();
	SgExprListExp* rev_args = buildExprListExp();
	foreach (SgExpression* exp, func_exp->get_args()->get_expressions())
	{
	    SgExpression *fwd_exp, *rev_exp;
	    //tie(fwd_exp, rev_exp) = instrumentAndReverseExpression(exp);
	    fwd_exp = copyExpression(exp);
	    rev_exp = copyExpression(exp);
	    fwd_args->append_expression(fwd_exp);
	    rev_args->append_expression(rev_exp);
	}

	func_processed_.insert(func_decl);

	return ExpPair(
		buildFunctionCallExp(fwd_func->get_name(), fwd_func->get_orig_return_type(), fwd_args),
		buildFunctionCallExp(rev_func->get_name(), rev_func->get_orig_return_type(), rev_args));

    }

    //if (isSgVarRefExp(exp) || isSgValueExp(exp) || isSgSizeOfOp(exp))

    return ExpPair(copyExpression(exp), NULL);
    //return ExpPair(copyExpression(exp), copyExpression(exp));
}


StmtPair EventReverser::instrumentAndReverseStatement(SgStatement* stmt)
{
    if (stmt == NULL)
	return NULL_STMT_PAIR;

    if (SgExprStatement* exp_stmt = isSgExprStatement(stmt))
    {
	// For a simple expression statement, we just process its 
	// expression then build new statements for the expression pair.
	SgExpression* exp = exp_stmt->get_expression();
	SgExpression *fwd_exp, *rev_exp;
	tie(fwd_exp, rev_exp) = instrumentAndReverseExpression(exp);
	if (fwd_exp && rev_exp)
	    return StmtPair(
		    buildExprStatement(fwd_exp), 
		    buildExprStatement(rev_exp));
	else if (fwd_exp)
	    return StmtPair(
		    buildExprStatement(fwd_exp), 
		    NULL);	
	else
	    return NULL_STMT_PAIR;
    }

    /* 
       if (isSgVariableDeclaration(stmt))
       {
       return StmtPair(copyStatement(stmt), NULL);
       }
       */

    // if it's a block, process each statement inside
    if (SgBasicBlock* body = isSgBasicBlock(stmt))
    {
	// A basic block normally contains a group of statements.
	// We process them one by one. Note that the reverse version
	// of the basic block has its reverse statements in reverse order. 

	SgBasicBlock* fwd_body = buildBasicBlock();
	SgBasicBlock* rev_body = buildBasicBlock();

	vector<SgStatement*> rev_forward_stmts;
	foreach(SgStatement* s, body->get_statements())
	{
	    SgStatement *fwd_stmt, *rev_stmt;
	    tie(fwd_stmt, rev_stmt) = instrumentAndReverseStatement(s);
	    if (fwd_stmt)
		fwd_body->append_statement(fwd_stmt);
	    if (isSgReturnStmt(rev_stmt) ||
		    isSgBreakStmt(rev_stmt) ||
		    isSgContinueStmt(rev_stmt))
		rev_body->append_statement(rev_stmt);
	    //else if (isSgVariableDeclaration(rev_stmt))
	    //	rev_forward_stmts.push_back(rev_stmt);
	    else if (rev_stmt)
		rev_body->prepend_statement(rev_stmt);
	}
	//for (vector<SgStatement*>::reverse_iterator it = rev_forward_stmts.rbegin();
	//	it != rev_forward_stmts.rend(); ++it)
	//    rev_body->prepend_statement(*it);

	return StmtPair(fwd_body, rev_body);
    }

    // if it's a local variable declaration
    if (SgVariableDeclaration* var_decl = isSgVariableDeclaration(stmt))
    {
	// I found that the number of variables of each variable declaration is always 1,
	// since Rose will seperate multi-vars into several declarations.
	// For example,  
	//           int i, j, k;
	// will be turned into:
	//           int i;
	//           int j;
	//           int k;
	// Therefore, currently we just use the first element of its variables returned below.
	const SgInitializedNamePtrList& names = var_decl->get_variables();
	ROSE_ASSERT(names.size() == 1);
	SgInitializedName* init_name = names[0];

	SgVariableDefinition* var_def = var_decl->get_definition();
	//SgAssignInitializer* init = isSgAssignInitializer(var_def->get_vardefn()->get_initializer());
	SgAssignInitializer* init = isSgAssignInitializer(init_name->get_initializer());
	// If not defined
	if (!init)
	    return StmtPair(copyStatement(stmt), NULL);

	SgExpression *fwd_exp, *rev_exp;
	tie(fwd_exp, rev_exp) = instrumentAndReverseExpression(init->get_operand());

	SgStatement *fwd_stmt, *rev_stmt = NULL;
	fwd_stmt = buildVariableDeclaration(
		    init_name->get_name(),
		    init_name->get_type(),
		    buildAssignInitializer(fwd_exp));
	if (rev_exp)
	    rev_stmt = buildExprStatement(rev_exp);
	return StmtPair(fwd_stmt, rev_stmt);
    }

    // if it's a if statement, process it according to the rule
    if (SgIfStmt* if_stmt = isSgIfStmt(stmt))
    {
	// For if statement, we have to use the external flags to save which 
	// branch is taken. The reverse version check these flags to select
	// the proper branch.
	// Currently we use a stack to store those flags in case that the if statement is 
	// inside a loop. The stack is written in C. For "if()else if()... else",
	// the current strategy does not make the optimal space use.
	// FIXME: Note that sometimes we don't have to use flags. for example, 
	// if the condition is decided by model state.

	SgStatement *fwd_true_body, *fwd_false_body;
	SgStatement *rev_true_body, *rev_false_body;

	SgStatement* true_body = if_stmt->get_true_body();
	SgStatement* false_body = if_stmt->get_false_body();

	tie(fwd_true_body, rev_true_body) = instrumentAndReverseStatement(true_body);
	tie(fwd_false_body, rev_false_body) = instrumentAndReverseStatement(false_body);

	// putBranchFlag is used to store which branch is chosen
	if (SgBasicBlock* body = isSgBasicBlock(fwd_true_body))
	    body->append_statement(putBranchFlagStmt(true));
	else if (fwd_true_body)
	    fwd_true_body = buildBasicBlock(
		    fwd_true_body, 
		    putBranchFlagStmt(true));

	if (SgBasicBlock* body = isSgBasicBlock(fwd_false_body))
	    body->append_statement(putBranchFlagStmt(false));
	else if (fwd_false_body)
	    fwd_false_body = buildBasicBlock(
		    fwd_false_body, 
		    putBranchFlagStmt(false));
	else
	    fwd_false_body = putBranchFlagStmt(false);

	SgStatement* cond = if_stmt->get_conditional();

	SgStatement *fwd_cond_exp, *rev_post_exp;
	SgExpression* rev_cond_exp = checkBranchFlagExp();

	// do not switch the position of the following statement to the one above;
	// make sure the current flag is used before generating new statement
	tie(fwd_cond_exp, rev_post_exp) = instrumentAndReverseStatement(cond);

	return StmtPair(
		//buildBasicBlock(
	    buildIfStmt(fwd_cond_exp, fwd_true_body, fwd_false_body),
	    //NewBranchFlag()),
		buildBasicBlock(
		    buildIfStmt(rev_cond_exp, rev_true_body, rev_false_body),
		    rev_post_exp));
    }

    if (SgForStatement* for_stmt = isSgForStatement(stmt))
    {
	// Currently, we just process the normal simple "for" use:
	// for (int i = 0; i < n; ++i)
	// FIXME: the complicated use of for needs to be dealt with in future.

	SgForInitStatement* init = for_stmt->get_for_init_stmt();
	SgStatement* test = for_stmt->get_test();
	SgExpression* incr = for_stmt->get_increment();
	SgStatement* body = for_stmt->get_loop_body();
	
	SgStatement *fwd_init, *rev_init, *fwd_test, *rev_test;
	SgExpression *fwd_incr, *rev_incr;
	SgStatement *fwd_body, *rev_body;

	tie(fwd_init, rev_init) = instrumentAndReverseStatement(init);
	tie(fwd_test, rev_test) = instrumentAndReverseStatement(test);
	tie(fwd_incr, rev_incr) = instrumentAndReverseExpression(incr);
	tie(fwd_body, rev_body) = instrumentAndReverseStatement(body);

	// To process a for statement, we have to know the loop count. If the for statement
	// is a simple for (int i = 0; i < N; ++i) where N is an constant number, we don't have 
	// to save this count. However, if we don't know the count when reverse is, we have to 
	// record the count by ourselves.
	
	// if (isSimpleForLoop()) do ...
	
	SgExpression* counter = getLoopCounter();

#if 0
	// We decide not to initialize the counter here, but finish it before entering this function
	SgStatement* init_counter = buildExprStatement(
		buildBinaryExpression<SgAssignOp>(
		    copyExpression(counter),
		    buildIntVal(0)));
#endif
	//fwd_init->append_init_statement(init_counter);
	if (SgBasicBlock* fwd_block_body = isSgBasicBlock(fwd_body))
	    fwd_block_body->append_statement(buildExprStatement(
			buildPlusPlusOp(counter, SgUnaryOp::prefix)));
	else
	    fwd_body = buildBasicBlock(fwd_body, buildExprStatement(
			buildPlusPlusOp(counter, SgUnaryOp::prefix)));
	SgForStatement* fwd_for = buildForStatement(fwd_init, fwd_test, fwd_incr, fwd_body);
	//SgForStatement* fwd_for = buildForStatement(init, test, incr, body);

	// Reversing for loop is a litter more complicated. We have to disorder every statement including
	// those in for()
	
	// Make the reverse incr part the first statement of the body.
	if (rev_incr)
	{
	    SgStatement* rev_incr_stmt = buildExprStatement(rev_incr);
	    if (SgBasicBlock* rev_block_body = isSgBasicBlock(rev_body))
		rev_block_body->prepend_statement(rev_incr_stmt);
	    else
		rev_body = buildBasicBlock(rev_incr_stmt, rev_body);
	}
	// Make the reverse test part the last statement of the body.
	if (rev_test)
	{
	    if (SgBasicBlock* rev_block_body = isSgBasicBlock(rev_body))
		rev_block_body->append_statement(rev_test);
	    else
		rev_body = buildBasicBlock(rev_body, rev_test);
	}

	// build a simple for loop like: for (int i = 0; i < N; ++i)
	SgStatement* rev_for = buildForStatement(
		buildVariableDeclaration("i", buildIntType(), buildAssignInitializer(buildIntVal(0))),
		buildExprStatement(buildBinaryExpression<SgLessThanOp>
		    (buildVarRefExp("i"), copyExpression(counter))),
		buildPlusPlusOp(buildVarRefExp("i"), SgUnaryOp::prefix),
		rev_body);

	rev_for = buildBasicBlock(rev_test, rev_for, rev_init);

#if 0
	//fwd_for->append_init_stmt(init_counter);
	//fwd_for = buildBasicBlock();
	/* 
	   foreach(SgStatement* stmt, init->get_init_stmt())
	   isSgBasicBlock(fwd_body)->append_statement(stmt);
	   isSgBasicBlock(fwd_body)->append_statement(init);
	   */
	rev_init = copyStatement(init);
	rev_test = copyStatement(test);
	rev_incr = copyExpression(incr);
	//SgForStatement* fwd_for = buildForStatement(fwd_init, fwd_test, fwd_incr, fwd_body);
	SgForStatement* rev_for = buildForStatement(NULL, rev_test, rev_incr, rev_body);
	//SgForInitStatement for_init = rev_for->get_
	//fwd_for->set_for_init_stmt(init);
	rev_for->set_for_init_stmt(isSgForInitStatement(rev_init));
#endif

	return StmtPair(fwd_for, rev_for);
    }

    if (SgForInitStatement* for_init_stmt = isSgForInitStatement(stmt))
    {
	// In C standard, the initialization part in for loop can either be a /declaration/ or
	// /expression;/. However, it's only regarded as a statement in Rose.
	if (for_init_stmt->get_init_stmt().size() == 0)
	    return StmtPair(NULL, NULL);
	if (for_init_stmt->get_init_stmt().size() == 1)
	    return instrumentAndReverseStatement(for_init_stmt->get_init_stmt().front());
	else
	    return StmtPair(copyStatement(for_init_stmt), NULL);
    }

    if (SgWhileStmt* while_stmt = isSgWhileStmt(stmt))
    {
	// We use an external counter to record the loop number.
	// In the reverse version, we use "for" loop according to the 
	// counter.

	SgStatement* cond = while_stmt->get_condition();
	SgStatement* body = while_stmt->get_body();

	SgStatement *fwd_cond, *rev_cond, *fwd_body, *rev_body;
	tie(fwd_cond, rev_cond) = instrumentAndReverseStatement(cond);
	tie(fwd_body, rev_body) = instrumentAndReverseStatement(body);

	SgName counter_name = generateVar("while_counter");
	// initialize the counter
	SgStatement* fwd_pre_exp = buildExprStatement(
		buildBinaryExpression<SgAssignOp>(
		    buildVarRefExp(counter_name), 
		    buildIntVal(0)));
	// increase the counter by 1
	SgStatement* incr_counter = buildExprStatement(
		buildPlusPlusOp(
		    buildVarRefExp(counter_name), 
		    SgUnaryOp::prefix));

	if (SgBasicBlock* body = isSgBasicBlock(fwd_body))
	    body->append_statement(incr_counter);
	else if (fwd_body)
	    fwd_body = buildBasicBlock(fwd_body, incr_counter);

	if (rev_cond != NULL)
	{
	    if (SgBasicBlock* body = isSgBasicBlock(rev_body))
		body->append_statement(rev_cond);
	    else if (rev_body)
		rev_body = buildBasicBlock(rev_body, rev_cond);
	}

	// compare the counter to 0
	SgStatement* test = buildExprStatement(
		buildBinaryExpression<SgGreaterThanOp>(
		    buildVarRefExp(counter_name), 
		    buildIntVal(0))); 
	// decrease the counter by 1
	SgExpression* incr = buildMinusMinusOp(
		buildVarRefExp(counter_name), 
		SgUnaryOp::prefix);
	// for loop
	SgStatement* rev_for_body = buildForStatement(NULL, test, incr, rev_body);
	if (rev_cond != NULL)
	    rev_for_body = buildBasicBlock(rev_for_body, rev_cond);

	return StmtPair(
		buildBasicBlock(
		    fwd_pre_exp,
		    buildWhileStmt(fwd_cond, fwd_body)),
		rev_for_body);
    }

    if (SgDoWhileStmt* do_while_stmt = isSgDoWhileStmt(stmt))
    {
	// This is similar to while statement.

	SgStatement* cond = do_while_stmt->get_condition();
	SgStatement* body = do_while_stmt->get_body();

	SgStatement *fwd_cond, *rev_cond, *fwd_body, *rev_body;
	tie(fwd_cond, rev_cond) = instrumentAndReverseStatement(cond);
	tie(fwd_body, rev_body) = instrumentAndReverseStatement(body);

	SgName counter_name = generateVar("while_counter");
	// initialize the counter
	SgStatement* fwd_pre_exp = buildExprStatement(
		buildBinaryExpression<SgAssignOp>(
		    buildVarRefExp(counter_name), 
		    buildIntVal(0)));
	// increase the counter by 1
	SgStatement* incr_counter = buildExprStatement(
		buildPlusPlusOp(
		    buildVarRefExp(counter_name), 
		    SgUnaryOp::prefix));

	if (SgBasicBlock* body = isSgBasicBlock(fwd_body))
	    body->append_statement(incr_counter);
	else if (fwd_body)
	    fwd_body = buildBasicBlock(fwd_body, incr_counter);

	if (rev_cond != NULL)
	{
	    if (SgBasicBlock* body = isSgBasicBlock(rev_body))
		body->prepend_statement(rev_cond);
	    else if (rev_body)
		rev_body = buildBasicBlock(rev_cond, rev_body);
	}

	// compare the counter to 0
	SgStatement* test = buildExprStatement(
		buildBinaryExpression<SgGreaterThanOp>(
		    buildVarRefExp(counter_name), 
		    buildIntVal(0))); 
	// decrease the counter by 1
	SgExpression* incr = buildMinusMinusOp(
		buildVarRefExp(counter_name), 
		SgUnaryOp::prefix);
	// for loop
	SgStatement* rev_for_body = buildForStatement(NULL, test, incr, rev_body);

	return StmtPair(
		buildBasicBlock(
		    fwd_pre_exp,
		    buildDoWhileStmt(fwd_body, fwd_cond)),
		rev_for_body);

    }

    if (SgSwitchStatement* switch_stmt = isSgSwitchStatement(stmt))
    {
	SgStatement* item_selector = switch_stmt->get_item_selector();
	SgBasicBlock* body = isSgBasicBlock(switch_stmt->get_body());
	const SgStatementPtrList& stmts = body->get_statements();
	SgStatementPtrList fwd_stmts, rev_stmts;

	// this part should be refined in case that there is no 'break' in one case.
	foreach(SgStatement* s, stmts)
	{
	    if (SgCaseOptionStmt* case_opt_stmt = isSgCaseOptionStmt(s))
	    {
		SgBasicBlock* case_body = isSgBasicBlock(case_opt_stmt->get_body());		
		if (case_body == NULL)
		    case_body = buildBasicBlock();

		StmtPair body_pair = instrumentAndReverseStatement(case_body);
		SgBasicBlock* fwd_case_body = isSgBasicBlock(body_pair.first);
		SgBasicBlock* rev_case_body = isSgBasicBlock(body_pair.second);

		fwd_case_body->prepend_statement(putBranchFlagStmt());

		fwd_stmts.push_back(buildCaseOptionStmt(
			    case_opt_stmt->get_key(),
			    fwd_case_body));
		rev_stmts.push_back(buildCaseOptionStmt(
			    case_opt_stmt->get_key(),
			    //buildIntVal(0),
			    rev_case_body));
	    }

	    if (SgDefaultOptionStmt* default_opt_stmt = isSgDefaultOptionStmt(s))
	    { 
		SgBasicBlock* default_body = isSgBasicBlock(default_opt_stmt->get_body());		
		if (default_body != NULL)
		{
		    StmtPair body_pair = instrumentAndReverseStatement(default_body);
		    SgBasicBlock* fwd_default_body = isSgBasicBlock(body_pair.first);
		    SgBasicBlock* rev_default_body = isSgBasicBlock(body_pair.second);

		    fwd_default_body->prepend_statement(putBranchFlagStmt());
		    fwd_stmts.push_back(buildDefaultOptionStmt(fwd_default_body));
		    rev_stmts.push_back(buildCaseOptionStmt(
				buildIntVal(0),
				rev_default_body));
		}
	    }
	}

	SgBasicBlock* fwd_body = buildBasicBlock();
	foreach(SgStatement* s, fwd_stmts)
	    fwd_body->append_statement(s);

	SgBasicBlock* rev_body = buildBasicBlock();
	foreach(SgStatement* s, rev_stmts)
	    rev_body->append_statement(s); 

	return StmtPair(
		buildSwitchStatement(item_selector, fwd_body),
		buildSwitchStatement(item_selector, rev_body));
    }

    return StmtPair(
	    copyStatement(stmt),
	    copyStatement(stmt));
}


vector<FuncDeclPair> EventReverser::outputFunctions()
{
    SgBasicBlock* body = func_decl_->get_definition()->get_body();
    StmtPair bodies = instrumentAndReverseStatement(body);

    SgName func_name = func_decl_->get_name() + "_forward";
    SgFunctionDeclaration* fwd_func_decl = 
	buildDefiningFunctionDeclaration(func_name, func_decl_->get_orig_return_type(), 
		isSgFunctionParameterList(copyStatement(func_decl_->get_parameterList())));

    pushScopeStack(isSgScopeStatement(fwd_func_decl->get_definition()->get_body()));
    SgStatementPtrList fwd_stmt_list = isSgBasicBlock(bodies.first)->get_statements();
    foreach (SgStatement* stmt, fwd_stmt_list)
	appendStatement(stmt);
    popScopeStack();

    func_name = func_decl_->get_name() + "_reverse";
    SgFunctionDeclaration* rev_func_decl = 
	buildDefiningFunctionDeclaration(func_name, func_decl_->get_orig_return_type(), 
		isSgFunctionParameterList(copyStatement(func_decl_->get_parameterList()))); 

    pushScopeStack(isSgScopeStatement(rev_func_decl->get_definition()->get_body()));
    SgStatementPtrList rev_stmt_list = isSgBasicBlock(bodies.second)->get_statements();
    foreach (SgStatement* stmt, rev_stmt_list)
	appendStatement(stmt);
    popScopeStack();

    output_func_pairs_.push_back(FuncDeclPair(fwd_func_decl, rev_func_decl));
    return output_func_pairs_;
}


class visitorTraversal : public AstSimpleProcessing
{
    public:
	visitorTraversal() 
	    : events_num(0),  
	    AstSimpleProcessing()
	{}
	virtual void visit(SgNode* n);
	vector<SgFunctionDeclaration*> funcs_gen;
	vector<SgFunctionDeclaration*> all_funcs;
	vector<SgStatement*> var_decls;
	vector<SgStatement*> var_inits;
	int events_num;
};


void visitorTraversal::visit(SgNode* n)
{

    if (SgFunctionDeclaration* func_decl = isSgFunctionDeclaration(n))
    {
	all_funcs.push_back(func_decl);

	string func_name = func_decl->get_name();
	if (!istarts_with(func_name, "event") ||
		iends_with(func_name, "reverse") ||
		iends_with(func_name, "forward"))
	    return;

	cout << func_name << endl;


	EventReverser reverser(func_decl);
	vector<FuncDeclPair> func_pairs = reverser.outputFunctions();
	foreach(const FuncDeclPair& func_pair, func_pairs)
	{
#if 1
	    funcs_gen.push_back(func_pair.second);
	    funcs_gen.push_back(func_pair.first);
#else
	    // put functions just after the target function for readibility
	    insertStatementAfter(func_decl, func_pair.second);
	    insertStatementAfter(func_decl, func_pair.first);
#endif
	}

	vector<SgStatement*> decls = reverser.getVarDeclarations();
	vector<SgStatement*> inits = reverser.getVarInitializers();

	var_decls.insert(var_decls.end(), decls.begin(), decls.end());
	var_inits.insert(var_inits.end(), inits.begin(), inits.end());

	// increase the number of events
	++events_num;

	/* 
	   pair<SgFunctionDeclaration*, SgFunctionDeclaration*> 
	   func = reverseFunction(func_decl->get_definition());
	   if (func.first != NULL)
	   funcs.push_back(func.first);
	   if (func.second != NULL)
	   funcs.push_back(func.second);
	   */
    }
}


// Put the functions generated in place
void insertFunctionInPlace(SgFunctionDeclaration* func, const vector<SgFunctionDeclaration*> all_funcs)
{
    string func_name = func->get_name();
    replace_last(func_name, "_forward", "");
    replace_last(func_name, "_reverse", "");

    foreach (SgFunctionDeclaration* f, all_funcs)
	if (func_name == string(f->get_name()))
	    insertStatementAfter(f, func);
}

int main( int argc, char * argv[] )
{
    SgProject* project = frontend(argc,argv);
    visitorTraversal traversal;

    SgGlobal *globalScope = getFirstGlobalScope (project);
    string includes = "#include \"rctypes.h\"\n"
	"#include <stdio.h>\n"
	"#include <stdlib.h>\n"
	"#include <time.h>\n"
	"#include <assert.h>\n";
    addTextForUnparser(globalScope,includes,AstUnparseAttribute::e_before); 

    pushScopeStack (isSgScopeStatement (globalScope));

    traversal.traverseInputFiles(project,preorder);

    appendStatement(buildInitializationFunction());
    appendStatement(buildCompareFunction());

    for (size_t i = 0; i < traversal.var_decls.size(); ++i)
	prependStatement(traversal.var_decls[i]);
    for (size_t i = 0; i < traversal.funcs_gen.size(); ++i)
	insertFunctionInPlace(traversal.funcs_gen[i], traversal.all_funcs);
	//appendStatement(traversal.funcs[i]);

    appendStatement(buildMainFunction(traversal.var_inits, traversal.events_num));

    popScopeStack();

    return backend(project);
}

