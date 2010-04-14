// Example ROSE Translator: used within ROSE/tutorial

#include "rose.h"
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/tuple/tuple.hpp>
#include <stack>

using namespace std;
using namespace boost;
using namespace SageBuilder;
using namespace SageInterface;

#define foreach BOOST_FOREACH


SgProject* project1;

class visitorTraversal : public AstSimpleProcessing
{
    public:
	virtual void visit(SgNode* n);
	vector<SgFunctionDeclaration*> funcs;
	vector<SgStatement*> var_decls;
};

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

/* 
   class IfStmtHandler
   {
   int flag_;
   SgVarRefExp* state_var_;
   std::map<SgStatement*, int> flag_table_;

   public:
   IfStmtHandler()
   : flag_(0)
   {
   state_var_ = buildVarRefExp("saved_state_b");
   }

   std::pair<SgIfStmt*, SgIfStmt*>
   instrumentAndReverseIfStmt(SgIfStmt* if_stmt)
   {
   SgTreeCopy tree_copy;
   int counter = flag_;

// deep copy the statement
SgIfStmt* fwd_if_stmt = dynamic_cast<SgIfStmt*>(if_stmt->copy(tree_copy));
SgStatement* true_body = fwd_if_stmt->get_true_body();
fwd_if_stmt->set_true_body(instrumentBody(true_body));
SgStatement* false_body = fwd_if_stmt->get_false_body();
fwd_if_stmt->set_false_body(instrumentBody(false_body));

SgIfStmt* rev_if_stmt = dynamic_cast<SgIfStmt*>(if_stmt->copy(tree_copy));
SgEqualityOp* eq_op = buildBinaryExpression<SgEqualityOp>(state_var_, buildIntVal(counter));
rev_if_stmt->set_conditional(buildExprStatement(eq_op));

// may invert the statements in the body

return std::make_pair(fwd_if_stmt, rev_if_stmt);
}

void instrumentIfStmt(SgIfStmt* if_stmt)
{
SgStatement* true_body = if_stmt->get_true_body();
if_stmt->set_true_body(instrumentBody(true_body));

SgStatement* false_body = if_stmt->get_false_body();
if_stmt->set_false_body(instrumentBody(false_body));

if (SgIfStmt* stmt = isSgIfStmt(false_body))
instrumentIfStmt(stmt);
}

void reverseIfStmt(SgIfStmt* if_stmt)
{
SgStatement* true_body = if_stmt->get_true_body();
int counter = flag_table_[true_body];
SgEqualityOp* eq_op = buildBinaryExpression<SgEqualityOp>(state_var_, buildIntVal(counter));
if_stmt->set_conditional(buildExprStatement(eq_op));

SgStatement* false_body = if_stmt->get_false_body();
if (SgIfStmt* stmt = isSgIfStmt(false_body))
reverseIfStmt(stmt);
}

private:
SgStatement* instrumentBody(SgStatement* body)
{
if (SgBasicBlock* block_body = isSgBasicBlock(body))
{
flag_table_[isSgStatement(block_body)] = flag_;

pushScopeStack(isSgScopeStatement(block_body));
SgAssignOp* ass_op = buildBinaryExpression<SgAssignOp>(state_var_, buildIntVal(flag_++));
SgExprStatement* stmt = buildExprStatement(ass_op);
appendStatement(stmt);
popScopeStack();
return block_body;
}

if (SgExprStatement* exp_stmt = isSgExprStatement(body))
{
    flag_table_[isSgStatement(exp_stmt)] = flag_;

    SgAssignOp* ass_op = buildBinaryExpression<SgAssignOp>(state_var_, buildIntVal(flag_++));
    SgExprStatement* stmt = buildExprStatement(ass_op);
    SgBasicBlock* block_body = buildBasicBlock(exp_stmt, stmt);
    return block_body;
}

if (body == NULL)
{
    SgAssignOp* ass_op = buildBinaryExpression<SgAssignOp>(state_var_, buildIntVal(flag_++));
    SgExprStatement* stmt = buildExprStatement(ass_op);
    return stmt;
}


return body;
}

};
*/

/* 
   inline ExpPair makeExpPair(SgExpression* first, SgExpression* second)
   {
   return std::pair<SgExpression*, SgExpression*>(first, second);
   }

   inline StmtPair makeStmtPair(SgStatement* first, SgStatement* second)
   {
   return std::pair<SgStatement*, SgStatement*>(first, second);
   }
   */


typedef std::pair<SgExpression*, SgExpression*> ExpPair;
typedef std::pair<SgStatement*, SgStatement*> StmtPair;
typedef std::pair<SgFunctionDeclaration*, SgFunctionDeclaration*> FuncDeclPair;

class FunctionRC
{
    SgFunctionDeclaration* func_decl_;
    string function_name_;
    vector<string> branch_flags_;
    vector<string> while_counters_;
    map<string, SgType*> state_vars_;
    int flag_;
    int flag_saved_;
    string flag_name_saved_;
    string flagstack_name_;
    int counter_;
    vector<FuncDeclPair> output_func_pairs_;
    static set<SgFunctionDeclaration*> func_processed_;

    public:
    FunctionRC(SgFunctionDeclaration* func_decl) 
	: func_decl_(func_decl), 
	function_name_(func_decl_->get_name()), 
	flag_(1), flag_saved_(0), counter_(0) 
    {
	flagstack_name_ = "flags";
    }
    vector<FuncDeclPair> OutputFunctions();

    // Get all variables' declarations including all kinds of states
    vector<SgStatement*> GetVarDeclarations()
    {
	vector<SgStatement*> decls;
	foreach(const string& flag, branch_flags_)
	    decls.push_back(buildVariableDeclaration(flag, buildIntType()));

	foreach(const string& counter, while_counters_)
	    decls.push_back(buildVariableDeclaration(counter, buildIntType()));

	pair<string, SgType*> state_var;
	foreach(state_var, state_vars_)
	{
	    decls.push_back(buildVariableDeclaration(state_var.first, state_var.second));
	}

	// how to build a class type?
	//decls.push_back(buildVariableDeclaration(flagstack_name_, build
	return decls;
    }

    private:
    static const ExpPair NULL_EXP_PAIR;
    static const StmtPair NULL_STMT_PAIR;

    // Just reverse an expression
    SgExpression* reverseExpression(SgExpression* exp);

    // Get the forward and reverse version of an expression
    ExpPair instrumentAndReverseExpression(SgExpression* exp);

    // Get the forward and reverse version of an statement 
    StmtPair instrumentAndReverseStatement(SgStatement* stmt);

    // Tell whether an expression is a state variable. We only reverse the "states".
    // Normally the state is passed into functions as arguments, or they are class members
    // in C++. 
    SgExpression* isStateVar(SgExpression* exp)
    {
	if (SgArrowExp* arr_exp = isSgArrowExp(exp))
	{
	    return arr_exp;
	}
	else if (SgPntrArrRefExp* ref_exp = isSgPntrArrRefExp(exp))
	{
	    if (isSgArrowExp(ref_exp->get_lhs_operand_i()))
		return ref_exp;
	}
	return NULL;
    }

    // Get the variable name which saves the branch state
    SgExpression* GetStateVar(SgExpression* var)
    {
	string name = function_name_ + "_state_" + lexical_cast<string>(counter_++);
	state_vars_[name] = var->get_type();
	return buildVarRefExp(name);
    }

    // Return the statement which saves the branch state
    SgStatement* PutBranchFlag(bool flag = true)
    {
	/* 
	if (branch_flags_.empty())
	    branch_flags_.push_back(function_name_ + "_branch_flag_" + lexical_cast<string>(counter_++));
	SgVarRefExp* flag_var = buildVarRefExp(branch_flags_.back());
	return buildExprStatement(buildBinaryExpression<SgIorAssignOp>(flag_var, buildIntVal(flag_)));
	*/
	return buildFunctionCallStmt("Push", buildVoidType(), 
		buildExprListExp(buildVarRefExp("flags"), buildIntVal(flag)));
    }

    // Return the statement which checks the branch flag
    SgStatement* CheckBranchFlag()
    {
	/* 
	SgVarRefExp* flag_var = buildVarRefExp(branch_flags_.back());
	SgStatement* flag_check = buildExprStatement(buildBinaryExpression<SgBitAndOp>(flag_var, buildIntVal(flag_)));

	flag_ <<= 1;
	if (flag_ == 0)
	{
	    // the current flag has been used up, we need a new one.
	    flag_ = 1;
	    branch_flags_.push_back(function_name_ + "_branch_flag_" + lexical_cast<string>(counter_++));
	}

	return flag_check;
	*/
	SgStatement* flag_check = buildFunctionCallStmt("Pop", buildVoidType(), 
		buildExprListExp(buildVarRefExp(flagstack_name_)));
	return flag_check;
    }

    SgStatement* NewBranchFlag()
    {
	SgStatement* flag = buildFunctionCallStmt("Push", buildVoidType(), 
		buildExprListExp(buildVarRefExp(flagstack_name_)));
	return flag;
    }

    void SaveBranchFlag() 
    {
	/* 
	if (branch_flags_.empty())
	    branch_flags_.push_back(function_name_ + "_branch_flag" + lexical_cast<string>(counter_++));
	flag_saved_ = flag_;
	flag_name_saved_ = branch_flags_.back();
	*/
    }

    SgName GenerateVar(const string& name, SgType* type = buildIntType())
    {
	string var_name = function_name_ + "_" + name + "_" + lexical_cast<string>(counter_++);
	state_vars_[var_name] = type;
	return var_name;
    }

    SgStatement* InitWhileCounter()
    {
	while_counters_.push_back(function_name_ + "_while_counter_" + lexical_cast<string>(counter_++));
	return buildExprStatement(
		buildBinaryExpression<SgAssignOp>(
		    buildVarRefExp(while_counters_.back()), 
		    buildIntVal(0)));
    }
};

set<SgFunctionDeclaration*> FunctionRC::func_processed_;
const ExpPair FunctionRC::NULL_EXP_PAIR = ExpPair(NULL, NULL);
const StmtPair FunctionRC::NULL_STMT_PAIR = StmtPair(NULL, NULL);


SgExpression* FunctionRC::reverseExpression(SgExpression* exp)
{

    return NULL;
}


ExpPair FunctionRC::instrumentAndReverseExpression(SgExpression* exp)
{
    // if this expression is a binary one
    if (SgBinaryOp* bin_op = isSgBinaryOp(exp))
    {
	SgExpression *fwd_left, *fwd_right, *rev_left, *rev_right;
	tie(fwd_right, rev_right) = instrumentAndReverseExpression(bin_op->get_rhs_operand_i());
	tie(fwd_left, rev_left) = instrumentAndReverseExpression(bin_op->get_lhs_operand_i());

	SgBinaryOp* fwd_exp = isSgBinaryOp(copyExpression(exp));
	fwd_exp->set_lhs_operand_i(fwd_left);
	fwd_exp->set_rhs_operand_i(fwd_right);

	// if the left-hand side of the assign-op is the state
	if (SgExpression* var = isStateVar(bin_op->get_lhs_operand_i()))
	{
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

		if (isSgMultAssignOp(exp) && !isZero(isSgValueExp(rhs_operand)))
		{
		    // if the rhs_operand is a value and the value is not 0
		    return ExpPair(
			    fwd_exp,
			    buildBinaryExpression<SgDivAssignOp>(
				var, rhs_operand));
		}
	    }

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
		SgExpression *fwd_rhs_exp, *rev_rhs_exp;
		tie(fwd_rhs_exp, rev_rhs_exp) = 
		    instrumentAndReverseExpression(exp_copy->get_rhs_operand_i());
		exp_copy->set_rhs_operand_i(fwd_rhs_exp);

		SgExpression* state_var = GetStateVar(var);
		// save the state
		SgExpression* fwd_exp = buildBinaryExpression<SgCommaOpExp>(
			buildBinaryExpression<SgAssignOp>(
			    state_var,
			    copyExpression(var)),
			exp_copy);

		// retrieve the state
		SgExpression* rev_exp = buildBinaryExpression<SgAssignOp>(
			copyExpression(var),
			copyExpression(state_var));
		if (rev_rhs_exp)
		    rev_exp = buildBinaryExpression<SgCommaOpExp>(rev_rhs_exp, rev_exp);
		return ExpPair(fwd_exp, rev_exp);
	    }
	}

	SgExpression* rev_exp = NULL;
	if (rev_right && rev_left)
	    rev_exp = buildBinaryExpression<SgCommaOpExp>(rev_right, rev_left);
	else if (rev_left)
	    rev_exp = rev_left;
	else if (rev_right)
	    rev_exp = rev_right;

	return ExpPair(fwd_exp, rev_exp);
    }

    if (SgUnaryOp* unary_op = isSgUnaryOp(exp))
    {
	SgExpression *fwd_operand_exp, *rev_operand_exp;
	tie(fwd_operand_exp, rev_operand_exp) = instrumentAndReverseExpression(unary_op->get_operand());
	// instrument its operand
	SgUnaryOp* fwd_exp = isSgUnaryOp(copyExpression(exp));
	fwd_exp->set_operand_i(fwd_operand_exp);

	if (SgExpression* var = isStateVar(unary_op->get_operand_i()))
	{
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

    if (SgFunctionCallExp* func_exp = isSgFunctionCallExp(exp))
    {
	SgFunctionDeclaration* func_decl = func_exp->getAssociatedFunctionDeclaration();
	FunctionRC func_generator(func_decl);
	vector<FuncDeclPair> func_pairs = func_generator.OutputFunctions();
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


StmtPair FunctionRC::instrumentAndReverseStatement(SgStatement* stmt)
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
	    body->append_statement(PutBranchFlag(true));
	else if (fwd_true_body)
	    fwd_true_body = buildBasicBlock(fwd_true_body, PutBranchFlag(true));

	if (SgBasicBlock* body = isSgBasicBlock(fwd_false_body))
	    body->append_statement(PutBranchFlag(false));
	else if (fwd_false_body)
	    fwd_false_body = buildBasicBlock(fwd_false_body, PutBranchFlag(false));

	SgStatement* cond = if_stmt->get_conditional();

	SgStatement *fwd_cond_exp, *rev_post_exp;
	SgStatement* rev_cond_exp = CheckBranchFlag();

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

	SgName counter_name = GenerateVar("while_counter");
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

	SgName counter_name = GenerateVar("while_counter");
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

		fwd_case_body->prepend_statement(PutBranchFlag());

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

		    fwd_default_body->prepend_statement(PutBranchFlag());
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


vector<FuncDeclPair> FunctionRC::OutputFunctions()
{
    SgBasicBlock* body = func_decl_->get_definition()->get_body();
    StmtPair bodies = instrumentAndReverseStatement(body);

    SgName func_name = func_decl_->get_name() + "_instrumented";
    SgFunctionDeclaration* fwd_func_decl = 
	buildDefiningFunctionDeclaration(func_name, func_decl_->get_orig_return_type(), 
		isSgFunctionParameterList(copyStatement(func_decl_->get_parameterList()))); 


    pushScopeStack(isSgScopeStatement(fwd_func_decl->get_definition()->get_body()));
    SgStatementPtrList fwd_stmt_list = isSgBasicBlock(bodies.first)->get_statements();
#if !((__GNUC__ == 4) && (__GNUC_MINOR__ == 3))
 // This fails to link when this is include with the 4.3.2 compiler.
 // error: /usr/bin/ld: final link failed: Nonrepresentable section on output
    for_each(fwd_stmt_list.begin(), fwd_stmt_list.end(), appendStatement);
#endif
    popScopeStack();

    func_name = func_decl_->get_name() + "_reverse";
    SgFunctionDeclaration* rev_func_decl = 
	buildDefiningFunctionDeclaration(func_name, func_decl_->get_orig_return_type(), 
		isSgFunctionParameterList(copyStatement(func_decl_->get_parameterList()))); 

    pushScopeStack(isSgScopeStatement(rev_func_decl->get_definition()->get_body()));
    SgStatementPtrList rev_stmt_list = isSgBasicBlock(bodies.second)->get_statements();
#if !((__GNUC__ == 4) && (__GNUC_MINOR__ == 3))
 // This fails to link when this is include with the 4.3.2 compiler.
 // error: /usr/bin/ld: final link failed: Nonrepresentable section on output
    for_each(rev_stmt_list.begin(), rev_stmt_list.end(), appendStatement);
#endif
    popScopeStack();

    output_func_pairs_.push_back(FuncDeclPair(fwd_func_decl, rev_func_decl));
    return output_func_pairs_;
}


void visitorTraversal::visit(SgNode* n)
{

    if (SgFunctionDeclaration* func_decl = isSgFunctionDeclaration(n))
    {
	if (func_decl->get_name() != "event") return;
	FunctionRC func_rc(func_decl);
	vector<FuncDeclPair> func_pairs = func_rc.OutputFunctions();
	foreach(const FuncDeclPair& func_pair, func_pairs)
	{
	    funcs.push_back(func_pair.first);
	    funcs.push_back(func_pair.second);
	}

	vector<SgStatement*> vars = func_rc.GetVarDeclarations();
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

// Typical main function for ROSE translator
int main( int argc, char * argv[] )
{
    SgProject* project = frontend(argc,argv);
    visitorTraversal traversal;

    project1 = project;

    SgGlobal *globalScope = getFirstGlobalScope (project);
    pushScopeStack (isSgScopeStatement (globalScope));

    traversal.traverseInputFiles(project,preorder);

    for (size_t i = 0; i < traversal.var_decls.size(); ++i)
	appendStatement(traversal.var_decls[i], globalScope);
    for (size_t i = 0; i < traversal.funcs.size(); ++i)
	appendStatement(traversal.funcs[i], globalScope);

    popScopeStack();

    return backend(project);
}

