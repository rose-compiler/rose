// Example ROSE Translator: used within ROSE/tutorial

#include "reverse.h"
#include <stack>

using namespace std;
using namespace boost;

#define ISZERO(value, ValType) \
    if (ValType* val = is##ValType(value)) \
return val->get_value() == 0;

bool isZero(SgValueExp* value)
{
    if (value) return true;

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
    // if this expression is a binary one
    if (SgBinaryOp* bin_op = isSgBinaryOp(exp))
    {
	// a binary operation contains two expressions which need to be processed first
	SgExpression *fwd_left, *fwd_right, *rev_left, *rev_right;
	tie(fwd_right, rev_right) = instrumentAndReverseExpression(bin_op->get_rhs_operand_i());
	tie(fwd_left, rev_left) = instrumentAndReverseExpression(bin_op->get_lhs_operand_i());

	// the default forward version. Unless state saving is needed, we use the following one.
	SgBinaryOp* fwd_exp = isSgBinaryOp(copyExpression(exp));
	fwd_exp->set_lhs_operand_i(fwd_left);
	fwd_exp->set_rhs_operand_i(fwd_right);

	// if the left-hand side of the assign-op is the state
	// (note that only in this situation do we consider to reverse this expression)
	if (SgExpression* var = isStateVar(bin_op->get_lhs_operand_i()))
	{
	    // the operations +=, -=, and *= can be reversed without state saving,
	    // if the rhs operand is a value or state variable.
	    SgExpression* rhs_operand = bin_op->get_rhs_operand_i();
	    if (isStateVar(rhs_operand) || isSgValueExp(rhs_operand))
	    {
		if (isSgPlusAssignOp(exp))
		{
		    return ExpPair(
			    fwd_exp,
			    buildBinaryExpression<SgMinusAssignOp>(
				var, rhs_operand));
		}

		if (isSgMinusAssignOp(exp))
		{
		    return ExpPair(
			    fwd_exp,
			    buildBinaryExpression<SgPlusAssignOp>(
				var, rhs_operand));
		}

		// we must ensure that the rhs operand of *= is not ZERO
		if (isSgMultAssignOp(exp) && !isZero(isSgValueExp(rhs_operand)))
		{
		    // if the rhs_operand is a value and the value is not 0
		    return ExpPair(
			    fwd_exp,
			    buildBinaryExpression<SgDivAssignOp>(
				var, rhs_operand));
		}
	    }

	    // the following operations which alter the value of the lhs operand
	    // can be reversed by state saving
	    if (isSgAssignOp(exp) ||
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
		ROSE_ASSERT(exp_copy);

		SgExpression *fwd_rhs_exp, *rev_rhs_exp;
		tie(fwd_rhs_exp, rev_rhs_exp) = 
		    instrumentAndReverseExpression(exp_copy->get_rhs_operand_i());
		exp_copy->set_rhs_operand_i(fwd_rhs_exp);

		SgExpression* state_var = getStateVar(var);

		// Save the state (we cannot use the default forward expression any more).
		// Here we use comma operator expression to implement state saving.
		SgExpression* fwd_exp = buildBinaryExpression<SgCommaOpExp>(
			buildBinaryExpression<SgAssignOp>(
			    state_var,
			    copyExpression(var)),
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

	// The following is the default reverse expression which is a combination
	// of reverse expressions of both sides.
	SgExpression* rev_exp = NULL;
	if (rev_right && rev_left)
	    rev_exp = buildBinaryExpression<SgCommaOpExp>(rev_right, rev_left);
	else if (rev_left)
	    rev_exp = rev_left;
	else if (rev_right)
	    rev_exp = rev_right;

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
	SgExpression *rev_cond_exp, *rev_true_exp, *rev_false_exp;
	SgExpression *fwd_cond_exp, *fwd_true_exp, *fwd_false_exp;

	tie(rev_cond_exp, fwd_cond_exp) = instrumentAndReverseExpression(cond_exp->get_conditional_exp());
	tie(rev_true_exp, fwd_true_exp) = instrumentAndReverseExpression(cond_exp->get_true_exp());
	tie(rev_false_exp, fwd_false_exp) = instrumentAndReverseExpression(cond_exp->get_false_exp());
	return ExpPair(
		buildConditionalExp(rev_cond_exp, rev_true_exp, rev_false_exp),
		buildConditionalExp(fwd_cond_exp, fwd_true_exp, fwd_false_exp));
    }

    // process the function call expression
    if (SgFunctionCallExp* func_exp = isSgFunctionCallExp(exp))
    {
	// When there is a function call in the event function, we should consider to
	// instrument and reverse this function.
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

    return ExpPair(copyExpression(exp), NULL);
    //return ExpPair(copyExpression(exp), copyExpression(exp));
}


StmtPair EventReverser::instrumentAndReverseStatement(SgStatement* stmt)
{
    if (stmt == NULL)
	return NULL_STMT_PAIR;

    if (SgExprStatement* exp_stmt = isSgExprStatement(stmt))
    {
	SgExpression* exp = exp_stmt->get_expression();
	ExpPair exp_pair = instrumentAndReverseExpression(exp);
	if (exp_pair.first && exp_pair.second)
	    return StmtPair(
		    buildExprStatement(exp_pair.first), 
		    buildExprStatement(exp_pair.second));
	else if (exp_pair.first)
	    return StmtPair(
		    buildExprStatement(exp_pair.first), 
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
	    else if (isSgVariableDeclaration(rev_stmt))
		rev_forward_stmts.push_back(rev_stmt);
	    else if (rev_stmt)
		rev_body->prepend_statement(rev_stmt);
	}
	for (vector<SgStatement*>::reverse_iterator it = rev_forward_stmts.rbegin();
		it != rev_forward_stmts.rend(); ++it)
	    rev_body->prepend_statement(*it);

	return StmtPair(fwd_body, rev_body);
    }

    // if it's a if statement, process it according to the rule
    if (SgIfStmt* if_stmt = isSgIfStmt(stmt))
    {
	SgStatement *fwd_true_body, *fwd_false_body;
	SgStatement *rev_true_body, *rev_false_body;

	SgStatement* true_body = if_stmt->get_true_body();
	SgStatement* false_body = if_stmt->get_false_body();

	tie(fwd_true_body, rev_true_body) = instrumentAndReverseStatement(true_body);
	tie(fwd_false_body, rev_false_body) = instrumentAndReverseStatement(false_body);

	if (SgBasicBlock* body = isSgBasicBlock(fwd_true_body))
	    body->append_statement(putBranchFlag(true));
	else if (fwd_true_body)
	    fwd_true_body = buildBasicBlock(fwd_true_body, putBranchFlag(true));

	if (SgBasicBlock* body = isSgBasicBlock(fwd_false_body))
	    body->append_statement(putBranchFlag(false));
	else if (fwd_false_body)
	    fwd_false_body = buildBasicBlock(fwd_false_body, putBranchFlag(false));

	SgStatement* cond = if_stmt->get_conditional();

	SgStatement *fwd_cond_exp, *rev_post_exp;
	SgStatement* rev_cond_exp = checkBranchFlag();

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

	/* 
	   foreach(SgStatement* stmt, init->get_init_stmt())
	   isSgBasicBlock(fwd_body)->append_statement(stmt);
	   isSgBasicBlock(fwd_body)->append_statement(init);
	   */
	rev_init = copyStatement(init);
	rev_test = copyStatement(test);
	rev_incr = copyExpression(incr);
	SgForStatement* fwd_for = buildForStatement(fwd_init, fwd_test, fwd_incr, fwd_body);
	SgForStatement* rev_for = buildForStatement(NULL, rev_test, rev_incr, rev_body);
	//SgForInitStatement for_init = rev_for->get_
	//fwd_for->set_for_init_stmt(init);
	rev_for->set_for_init_stmt(isSgForInitStatement(rev_init));

	return StmtPair(
		fwd_for,
		//buildForStatement(fwd_init, fwd_test, fwd_incr, fwd_body),
		rev_for);
	//buildForStatement(init, test, incr, rev_body));
    }

    if (SgForInitStatement* for_init_stmt = isSgForInitStatement(stmt))
    {
	if (for_init_stmt->get_init_stmt().size() == 1)
	    return instrumentAndReverseStatement(*(for_init_stmt->get_init_stmt().begin()));
    }

    if (SgWhileStmt* while_stmt = isSgWhileStmt(stmt))
    {
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

		fwd_case_body->prepend_statement(putBranchFlag());

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

		    fwd_default_body->prepend_statement(putBranchFlag());
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
	virtual void visit(SgNode* n);
	vector<SgFunctionDeclaration*> funcs;
	vector<SgStatement*> var_decls;
};


void visitorTraversal::visit(SgNode* n)
{

    if (SgFunctionDeclaration* func_decl = isSgFunctionDeclaration(n))
    {
	if (func_decl->get_name() != "event") return;
	EventReverser reverser(func_decl);
	vector<FuncDeclPair> func_pairs = reverser.outputFunctions();
	foreach(const FuncDeclPair& func_pair, func_pairs)
	{
	    funcs.push_back(func_pair.first);
	    funcs.push_back(func_pair.second);
	}

	vector<SgStatement*> vars = reverser.getVarDeclarations();
	foreach(SgStatement* stmt, vars)
	    var_decls.push_back(stmt);

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


SgFunctionDeclaration* buildMainFunction()
{
    SgFunctionDeclaration* func_decl = 
	buildDefiningFunctionDeclaration(
		"main",
		buildIntType(),
		buildFunctionParameterList());
    pushScopeStack(isSgScopeStatement(func_decl->get_definition()->get_body()));

    /////////////////////////////////////////////////////////////////////////////////////////
    // There are two tests: one is testing event and event_fwd get the same value of the model,
    // the other is performing event_fwd and event_reverse to see if the value of the model changes
    
    SgType* model_type = buildStructDeclaration("model")->get_type();

    // Declare two variables 
    SgVariableDeclaration* var1 = buildVariableDeclaration("m1", model_type);
    SgVariableDeclaration* var2 = buildVariableDeclaration("m2", model_type);

    // Initialize them
    SgExprListExp* para1 = buildExprListExp(
	    buildUnaryExpression<SgAddressOfOp>(buildVarRefExp("m1")));
    SgExprStatement* init1 = buildFunctionCallStmt("initialize", buildVoidType(), para1);
    SgExprListExp* para2 = buildExprListExp(
	    buildUnaryExpression<SgAddressOfOp>(buildVarRefExp("m2")));
    SgExprStatement* init2 = buildFunctionCallStmt("initialize", buildVoidType(), para2);

    // Output PASS or FAIL information
    SgExprListExp* para_pass = buildExprListExp(buildStringVal("PASS!\\n"));
    SgExprListExp* para_fail = buildExprListExp(buildStringVal("FAIL!\\n"));
    SgExprStatement* test_pass = buildFunctionCallStmt("printf", buildVoidType(), para_pass);
    SgExprStatement* test_fail = buildFunctionCallStmt("printf", buildVoidType(), para_fail);


    // Call event and event_forward then check two models' values
    SgExprStatement* call_event = buildFunctionCallStmt("event", buildVoidType(), para1);
    SgExprStatement* call_event_fwd = buildFunctionCallStmt("event_forward", buildVoidType(), para2);
    SgExprListExp* para_comp = buildExprListExp(
	    buildUnaryExpression<SgAddressOfOp>(buildVarRefExp("m1")),
	    buildUnaryExpression<SgAddressOfOp>(buildVarRefExp("m2")));
    SgExprStatement* call_compare = buildFunctionCallStmt("compare", buildVoidType(), para_comp);
    SgIfStmt* test_compare = buildIfStmt(call_compare, test_fail, NULL);

    // Call event_forward and event_reverse then check if the model's value changes
    SgExprStatement* call_event_rev = buildFunctionCallStmt("event_reverse", buildVoidType(), para2);

    appendStatement(var1);
    appendStatement(var2);
    appendStatement(init1);
    appendStatement(init2);
    appendStatement(call_event);
    appendStatement(call_event_fwd);
    appendStatement(test_compare);
    appendStatement(copyStatement(init1));
    appendStatement(copyStatement(init2));
    appendStatement(copyStatement(call_event_fwd));
    appendStatement(call_event_rev);
    appendStatement(copyStatement(test_compare));
    appendStatement(test_pass);

    /////////////////////////////////////////////////////////////////////////////////////////

    popScopeStack();
    return func_decl;
}

// Typical main function for ROSE translator
int main( int argc, char * argv[] )
{
    SgProject* project = frontend(argc,argv);
    visitorTraversal traversal;

    SgGlobal *globalScope = getFirstGlobalScope (project);
    pushScopeStack (isSgScopeStatement (globalScope));

    traversal.traverseInputFiles(project,preorder);

    for (size_t i = 0; i < traversal.var_decls.size(); ++i)
	appendStatement(traversal.var_decls[i], globalScope);
    for (size_t i = 0; i < traversal.funcs.size(); ++i)
	appendStatement(traversal.funcs[i], globalScope);

    appendStatement(buildMainFunction());

    popScopeStack();

    return backend(project);
}

