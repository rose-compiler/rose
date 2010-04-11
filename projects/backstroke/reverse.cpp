// Example ROSE Translator: used within ROSE/tutorial

#include "rose.h"
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/tuple/tuple.hpp>
#include <stack>
#include "boost/date_time/local_time/local_time.hpp"
#include <boost/date_time.hpp>

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
   instrumentAndInverseIfStmt(SgIfStmt* if_stmt)
   {
   SgTreeCopy tree_copy;
   int counter = flag_;

// deep copy the statement
SgIfStmt* ins_if_stmt = dynamic_cast<SgIfStmt*>(if_stmt->copy(tree_copy));
SgStatement* true_body = ins_if_stmt->get_true_body();
ins_if_stmt->set_true_body(instrumentBody(true_body));
SgStatement* false_body = ins_if_stmt->get_false_body();
ins_if_stmt->set_false_body(instrumentBody(false_body));

SgIfStmt* inv_if_stmt = dynamic_cast<SgIfStmt*>(if_stmt->copy(tree_copy));
SgEqualityOp* eq_op = buildBinaryExpression<SgEqualityOp>(state_var_, buildIntVal(counter));
inv_if_stmt->set_conditional(buildExprStatement(eq_op));

// may invert the statements in the body

return std::make_pair(ins_if_stmt, inv_if_stmt);
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

void inverseIfStmt(SgIfStmt* if_stmt)
{
SgStatement* true_body = if_stmt->get_true_body();
int counter = flag_table_[true_body];
SgEqualityOp* eq_op = buildBinaryExpression<SgEqualityOp>(state_var_, buildIntVal(counter));
if_stmt->set_conditional(buildExprStatement(eq_op));

SgStatement* false_body = if_stmt->get_false_body();
if (SgIfStmt* stmt = isSgIfStmt(false_body))
inverseIfStmt(stmt);
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

    SgExpression* inverseExpression(SgExpression* exp);
    ExpPair instrumentAndInverseExpression(SgExpression* exp);
    StmtPair instrumentAndInverseStatement(SgStatement* stmt);

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

    SgExpression* GetStateVar(SgExpression* var)
    {
	string name = function_name_ + "_state_" + lexical_cast<string>(counter_++);
	state_vars_[name] = var->get_type();
	return buildVarRefExp(name);
    }

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


SgExpression* FunctionRC::inverseExpression(SgExpression* exp)
{

    return NULL;
}


ExpPair FunctionRC::instrumentAndInverseExpression(SgExpression* exp)
{
    if (SgBinaryOp* bin_op = isSgBinaryOp(exp))
    {
	SgExpression *ins_left, *ins_right, *inv_left, *inv_right;
	tie(ins_right, inv_right) = instrumentAndInverseExpression(bin_op->get_rhs_operand_i());
	tie(ins_left, inv_left) = instrumentAndInverseExpression(bin_op->get_lhs_operand_i());

	SgBinaryOp* ins_exp = isSgBinaryOp(copyExpression(exp));
	ins_exp->set_lhs_operand_i(ins_left);
	ins_exp->set_rhs_operand_i(ins_right);

	// if the left-hand side of the assign-op is the state
	if (SgExpression* var = isStateVar(bin_op->get_lhs_operand_i()))
	{
	    SgExpression* rhs_operand = bin_op->get_rhs_operand_i();
	    if (isStateVar(rhs_operand) || isSgValueExp(rhs_operand))
	    {
		if (isSgPlusAssignOp(exp))
		{
		    return ExpPair(
			    ins_exp,
			    buildBinaryExpression<SgMinusAssignOp>(
				var, rhs_operand));
		}

		if (isSgMinusAssignOp(exp))
		{
		    return ExpPair(
			    ins_exp,
			    buildBinaryExpression<SgPlusAssignOp>(
				var, rhs_operand));
		}

		if (isSgMultAssignOp(exp) && !isZero(isSgValueExp(rhs_operand)))
		{
		    // if the rhs_operand is a value and the value is not 0
		    return ExpPair(
			    ins_exp,
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
		SgExpression *ins_rhs_exp, *inv_rhs_exp;
		tie(ins_rhs_exp, inv_rhs_exp) = 
		    instrumentAndInverseExpression(exp_copy->get_rhs_operand_i());
		exp_copy->set_rhs_operand_i(ins_rhs_exp);

		SgExpression* state_var = GetStateVar(var);
		// save the state
		SgExpression* ins_exp = buildBinaryExpression<SgCommaOpExp>(
			buildBinaryExpression<SgAssignOp>(
			    state_var,
			    copyExpression(var)),
			exp_copy);

		// retrieve the state
		SgExpression* inv_exp = buildBinaryExpression<SgAssignOp>(
			copyExpression(var),
			copyExpression(state_var));
		if (inv_rhs_exp)
		    inv_exp = buildBinaryExpression<SgCommaOpExp>(inv_rhs_exp, inv_exp);
		return ExpPair(ins_exp, inv_exp);
	    }
	}

	SgExpression* inv_exp = NULL;
	if (inv_right && inv_left)
	    inv_exp = buildBinaryExpression<SgCommaOpExp>(inv_right, inv_left);
	else if (inv_left)
	    inv_exp = inv_left;
	else if (inv_right)
	    inv_exp = inv_right;

	return ExpPair(ins_exp, inv_exp);
    }

    if (SgUnaryOp* unary_op = isSgUnaryOp(exp))
    {
	SgExpression *ins_operand_exp, *inv_operand_exp;
	tie(ins_operand_exp, inv_operand_exp) = instrumentAndInverseExpression(unary_op->get_operand());
	// instrument its operand
	SgUnaryOp* ins_exp = isSgUnaryOp(copyExpression(exp));
	ins_exp->set_operand_i(ins_operand_exp);

	if (SgExpression* var = isStateVar(unary_op->get_operand_i()))
	{
	    if (SgPlusPlusOp* pp_op = isSgPlusPlusOp(exp))
	    {
		SgUnaryOp::Sgop_mode pp_mode = pp_op->get_mode();
		if (pp_mode == SgUnaryOp::prefix) pp_mode = SgUnaryOp::postfix;
		else pp_mode = SgUnaryOp::prefix;
		return ExpPair(
			ins_exp,
			buildMinusMinusOp(var, pp_mode));
	    }

	    if (SgMinusMinusOp* mm_op = isSgMinusMinusOp(exp))
	    {
		SgUnaryOp::Sgop_mode mm_mode = mm_op->get_mode();
		if (mm_mode == SgUnaryOp::prefix) mm_mode = SgUnaryOp::postfix;
		else mm_mode = SgUnaryOp::prefix;
		return ExpPair(
			ins_exp,
			buildPlusPlusOp(var, mm_mode));
	    }
	}

	SgExpression* inv_exp = inv_operand_exp;
	return ExpPair(ins_exp, inv_exp);
    }

    if (SgConditionalExp* cond_exp = isSgConditionalExp(exp))
    {
	SgExpression *inv_cond_exp, *inv_true_exp, *inv_false_exp;
	SgExpression *ins_cond_exp, *ins_true_exp, *ins_false_exp;

	tie(inv_cond_exp, ins_cond_exp) = instrumentAndInverseExpression(cond_exp->get_conditional_exp());
	tie(inv_true_exp, ins_true_exp) = instrumentAndInverseExpression(cond_exp->get_true_exp());
	tie(inv_false_exp, ins_false_exp) = instrumentAndInverseExpression(cond_exp->get_false_exp());
	return ExpPair(
		buildConditionalExp(inv_cond_exp, inv_true_exp, inv_false_exp),
		buildConditionalExp(ins_cond_exp, ins_true_exp, ins_false_exp));
    }

    if (SgFunctionCallExp* func_exp = isSgFunctionCallExp(exp))
    {
	SgFunctionDeclaration* func_decl = func_exp->getAssociatedFunctionDeclaration();
	FunctionRC func_generator(func_decl);
	vector<FuncDeclPair> func_pairs = func_generator.OutputFunctions();
	if (func_processed_.count(func_decl) == 0)
	    foreach(const FuncDeclPair& func_pair, func_pairs)
		output_func_pairs_.push_back(func_pair);
	SgFunctionDeclaration* ins_func = func_pairs.back().first;
	SgFunctionDeclaration* inv_func = func_pairs.back().second;
	
	SgExprListExp* ins_args = buildExprListExp();
	SgExprListExp* inv_args = buildExprListExp();
	foreach (SgExpression* exp, func_exp->get_args()->get_expressions())
	{
	    SgExpression *ins_exp, *inv_exp;
	    //tie(ins_exp, inv_exp) = instrumentAndInverseExpression(exp);
	    ins_exp = copyExpression(exp);
	    inv_exp = copyExpression(exp);
	    ins_args->append_expression(ins_exp);
	    inv_args->append_expression(inv_exp);
	}

	func_processed_.insert(func_decl);

	return ExpPair(
		buildFunctionCallExp(ins_func->get_name(), ins_func->get_orig_return_type(), ins_args),
		buildFunctionCallExp(inv_func->get_name(), inv_func->get_orig_return_type(), inv_args));

    }

    return ExpPair(copyExpression(exp), NULL);
    //return ExpPair(copyExpression(exp), copyExpression(exp));
}



StmtPair FunctionRC::instrumentAndInverseStatement(SgStatement* stmt)
{
    if (stmt == NULL)
	return NULL_STMT_PAIR;

    if (SgExprStatement* exp_stmt = isSgExprStatement(stmt))
    {
	SgExpression* exp = exp_stmt->get_expression();
	ExpPair exp_pair = instrumentAndInverseExpression(exp);
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
	SgBasicBlock* ins_body = buildBasicBlock();
	SgBasicBlock* inv_body = buildBasicBlock();

	vector<SgStatement*> inv_forward_stmts;
	foreach(SgStatement* s, body->get_statements())
	{
	    SgStatement *ins_stmt, *inv_stmt;
	    tie(ins_stmt, inv_stmt) = instrumentAndInverseStatement(s);
	    if (ins_stmt)
		ins_body->append_statement(ins_stmt);
	    if (isSgReturnStmt(inv_stmt) ||
		    isSgBreakStmt(inv_stmt) ||
		    isSgContinueStmt(inv_stmt))
		inv_body->append_statement(inv_stmt);
	    else if (isSgVariableDeclaration(inv_stmt))
		inv_forward_stmts.push_back(inv_stmt);
	    else if (inv_stmt)
		inv_body->prepend_statement(inv_stmt);
	}
	for (vector<SgStatement*>::reverse_iterator it = inv_forward_stmts.rbegin();
		it != inv_forward_stmts.rend(); ++it)
	    inv_body->prepend_statement(*it);

	return StmtPair(ins_body, inv_body);
    }

    // if it's a if statement, process it according to the rule
    if (SgIfStmt* if_stmt = isSgIfStmt(stmt))
    {
	SgStatement *ins_true_body, *ins_false_body;
	SgStatement *inv_true_body, *inv_false_body;

	SgStatement* true_body = if_stmt->get_true_body();
	SgStatement* false_body = if_stmt->get_false_body();

	tie(ins_true_body, inv_true_body) = instrumentAndInverseStatement(true_body);
	tie(ins_false_body, inv_false_body) = instrumentAndInverseStatement(false_body);

	if (SgBasicBlock* body = isSgBasicBlock(ins_true_body))
	    body->append_statement(PutBranchFlag(true));
	else if (ins_true_body)
	    ins_true_body = buildBasicBlock(ins_true_body, PutBranchFlag(true));

	if (SgBasicBlock* body = isSgBasicBlock(ins_false_body))
	    body->append_statement(PutBranchFlag(false));
	else if (ins_false_body)
	    ins_false_body = buildBasicBlock(ins_false_body, PutBranchFlag(false));

	SgStatement* cond = if_stmt->get_conditional();

	SgStatement *ins_cond_exp, *inv_post_exp;
	SgStatement* inv_cond_exp = CheckBranchFlag();

	// do not switch the position of the following statement to the one above;
	// make sure the current flag is used before generating new statement
	tie(ins_cond_exp, inv_post_exp) = instrumentAndInverseStatement(cond);

	return StmtPair(
		//buildBasicBlock(
		    buildIfStmt(ins_cond_exp, ins_true_body, ins_false_body),
		    //NewBranchFlag()),
		buildBasicBlock(
		    buildIfStmt(inv_cond_exp, inv_true_body, inv_false_body),
		    inv_post_exp));
    }

    if (SgForStatement* for_stmt = isSgForStatement(stmt))
    {
	SgForInitStatement* init = for_stmt->get_for_init_stmt();
	SgStatement* test = for_stmt->get_test();
	SgExpression* incr = for_stmt->get_increment();
	SgStatement* body = for_stmt->get_loop_body();

	SgStatement *ins_init, *inv_init, *ins_test, *inv_test;
	SgExpression *ins_incr, *inv_incr;
	SgStatement *ins_body, *inv_body;

	tie(ins_init, inv_init) = instrumentAndInverseStatement(init);
	tie(ins_test, inv_test) = instrumentAndInverseStatement(test);
	tie(ins_incr, inv_incr) = instrumentAndInverseExpression(incr);
	tie(ins_body, inv_body) = instrumentAndInverseStatement(body);

	/* 
	foreach(SgStatement* stmt, init->get_init_stmt())
	    isSgBasicBlock(ins_body)->append_statement(stmt);
	isSgBasicBlock(ins_body)->append_statement(init);
	*/
	inv_init = copyStatement(init);
	inv_test = copyStatement(test);
	inv_incr = copyExpression(incr);
	SgForStatement* ins_for = buildForStatement(ins_init, ins_test, ins_incr, ins_body);
	SgForStatement* inv_for = buildForStatement(NULL, inv_test, inv_incr, inv_body);
	//SgForInitStatement for_init = inv_for->get_
	//ins_for->set_for_init_stmt(init);
	inv_for->set_for_init_stmt(isSgForInitStatement(inv_init));

	return StmtPair(
		ins_for,
		//buildForStatement(ins_init, ins_test, ins_incr, ins_body),
		inv_for);
		//buildForStatement(init, test, incr, inv_body));
    }
    
    if (SgForInitStatement* for_init_stmt = isSgForInitStatement(stmt))
    {
	if (for_init_stmt->get_init_stmt().size() == 1)
	    return instrumentAndInverseStatement(*(for_init_stmt->get_init_stmt().begin()));
    }

    if (SgWhileStmt* while_stmt = isSgWhileStmt(stmt))
    {
	SgStatement* cond = while_stmt->get_condition();
	SgStatement* body = while_stmt->get_body();

	SgStatement *ins_cond, *inv_cond, *ins_body, *inv_body;
	tie(ins_cond, inv_cond) = instrumentAndInverseStatement(cond);
	tie(ins_body, inv_body) = instrumentAndInverseStatement(body);

	SgName counter_name = GenerateVar("while_counter");
	// initialize the counter
	SgStatement* ins_pre_exp = buildExprStatement(
		buildBinaryExpression<SgAssignOp>(
		    buildVarRefExp(counter_name), 
		    buildIntVal(0)));
	// increase the counter by 1
	SgStatement* incr_counter = buildExprStatement(
		buildPlusPlusOp(
		    buildVarRefExp(counter_name), 
		    SgUnaryOp::prefix));

	if (SgBasicBlock* body = isSgBasicBlock(ins_body))
	    body->append_statement(incr_counter);
	else if (ins_body)
	    ins_body = buildBasicBlock(ins_body, incr_counter);

	if (inv_cond != NULL)
	{
	    if (SgBasicBlock* body = isSgBasicBlock(inv_body))
		body->append_statement(inv_cond);
	    else if (inv_body)
		inv_body = buildBasicBlock(inv_body, inv_cond);
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
	SgStatement* inv_for_body = buildForStatement(NULL, test, incr, inv_body);
	if (inv_cond != NULL)
	    inv_for_body = buildBasicBlock(inv_for_body, inv_cond);

	return StmtPair(
		buildBasicBlock(
		    ins_pre_exp,
		    buildWhileStmt(ins_cond, ins_body)),
		inv_for_body);
    }
      
    if (SgDoWhileStmt* do_while_stmt = isSgDoWhileStmt(stmt))
    {
	SgStatement* cond = do_while_stmt->get_condition();
	SgStatement* body = do_while_stmt->get_body();

	SgStatement *ins_cond, *inv_cond, *ins_body, *inv_body;
	tie(ins_cond, inv_cond) = instrumentAndInverseStatement(cond);
	tie(ins_body, inv_body) = instrumentAndInverseStatement(body);

	SgName counter_name = GenerateVar("while_counter");
	// initialize the counter
	SgStatement* ins_pre_exp = buildExprStatement(
		buildBinaryExpression<SgAssignOp>(
		    buildVarRefExp(counter_name), 
		    buildIntVal(0)));
	// increase the counter by 1
	SgStatement* incr_counter = buildExprStatement(
		buildPlusPlusOp(
		    buildVarRefExp(counter_name), 
		    SgUnaryOp::prefix));

	if (SgBasicBlock* body = isSgBasicBlock(ins_body))
	    body->append_statement(incr_counter);
	else if (ins_body)
	    ins_body = buildBasicBlock(ins_body, incr_counter);

	if (inv_cond != NULL)
	{
	    if (SgBasicBlock* body = isSgBasicBlock(inv_body))
		body->prepend_statement(inv_cond);
	    else if (inv_body)
		inv_body = buildBasicBlock(inv_cond, inv_body);
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
	SgStatement* inv_for_body = buildForStatement(NULL, test, incr, inv_body);

	return StmtPair(
		buildBasicBlock(
		    ins_pre_exp,
		    buildDoWhileStmt(ins_body, ins_cond)),
		inv_for_body);

    }

    if (SgSwitchStatement* switch_stmt = isSgSwitchStatement(stmt))
    {
	SgStatement* item_selector = switch_stmt->get_item_selector();
	SgBasicBlock* body = isSgBasicBlock(switch_stmt->get_body());
	const SgStatementPtrList& stmts = body->get_statements();
	SgStatementPtrList ins_stmts, inv_stmts;

	// this part should be refined in case that there is no 'break' in one case.
	foreach(SgStatement* s, stmts)
	{
	    if (SgCaseOptionStmt* case_opt_stmt = isSgCaseOptionStmt(s))
	    {
		SgBasicBlock* case_body = isSgBasicBlock(case_opt_stmt->get_body());		
		if (case_body == NULL)
		    case_body = buildBasicBlock();

		StmtPair body_pair = instrumentAndInverseStatement(case_body);
		SgBasicBlock* ins_case_body = isSgBasicBlock(body_pair.first);
		SgBasicBlock* inv_case_body = isSgBasicBlock(body_pair.second);

		ins_case_body->prepend_statement(PutBranchFlag());

		ins_stmts.push_back(buildCaseOptionStmt(
			    case_opt_stmt->get_key(),
			    ins_case_body));
		inv_stmts.push_back(buildCaseOptionStmt(
			    case_opt_stmt->get_key(),
			    //buildIntVal(0),
			    inv_case_body));
	    }

	    if (SgDefaultOptionStmt* default_opt_stmt = isSgDefaultOptionStmt(s))
	    {
		SgBasicBlock* default_body = isSgBasicBlock(default_opt_stmt->get_body());		
		if (default_body != NULL)
		{
		    StmtPair body_pair = instrumentAndInverseStatement(default_body);
		    SgBasicBlock* ins_default_body = isSgBasicBlock(body_pair.first);
		    SgBasicBlock* inv_default_body = isSgBasicBlock(body_pair.second);

		    ins_default_body->prepend_statement(PutBranchFlag());
		    ins_stmts.push_back(buildDefaultOptionStmt(ins_default_body));
		    inv_stmts.push_back(buildCaseOptionStmt(
				buildIntVal(0),
				inv_default_body));
		}
	    }
	}

	SgBasicBlock* ins_body = buildBasicBlock();
	foreach(SgStatement* s, ins_stmts)
	    ins_body->append_statement(s);

	SgBasicBlock* inv_body = buildBasicBlock();
	foreach(SgStatement* s, inv_stmts)
	    inv_body->append_statement(s);

	return StmtPair(
		buildSwitchStatement(item_selector, ins_body),
		buildSwitchStatement(item_selector, inv_body));
    }

    return StmtPair(
	    copyStatement(stmt),
	    copyStatement(stmt));
}


vector<FuncDeclPair> FunctionRC::OutputFunctions()
{
    SgBasicBlock* body = func_decl_->get_definition()->get_body();
    StmtPair bodies = instrumentAndInverseStatement(body);

    SgName func_name = func_decl_->get_name() + "_instrumented";
    SgFunctionDeclaration* ins_func_decl = 
	buildDefiningFunctionDeclaration(func_name, func_decl_->get_orig_return_type(), 
		isSgFunctionParameterList(copyStatement(func_decl_->get_parameterList()))); 


    pushScopeStack(isSgScopeStatement(ins_func_decl->get_definition()->get_body()));
    SgStatementPtrList ins_stmt_list = isSgBasicBlock(bodies.first)->get_statements();
#if (__GNUC__ == 4) && (__GNUC_MINOR__ == 3)
 // This fails to link when this is include with the 4.3.2 compiler.
 // error: /usr/bin/ld: final link failed: Nonrepresentable section on output
    for_each(ins_stmt_list.begin(), ins_stmt_list.end(), appendStatement);
#endif
    popScopeStack();

    func_name = func_decl_->get_name() + "_inverse";
    SgFunctionDeclaration* inv_func_decl = 
	buildDefiningFunctionDeclaration(func_name, func_decl_->get_orig_return_type(), 
		isSgFunctionParameterList(copyStatement(func_decl_->get_parameterList()))); 

    pushScopeStack(isSgScopeStatement(inv_func_decl->get_definition()->get_body()));
    SgStatementPtrList inv_stmt_list = isSgBasicBlock(bodies.second)->get_statements();
#if (__GNUC__ == 4) && (__GNUC_MINOR__ == 3)
 // This fails to link when this is include with the 4.3.2 compiler.
 // error: /usr/bin/ld: final link failed: Nonrepresentable section on output
    for_each(inv_stmt_list.begin(), inv_stmt_list.end(), appendStatement);
#endif
    popScopeStack();

    output_func_pairs_.push_back(FuncDeclPair(ins_func_decl, inv_func_decl));
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
	   func = inverseFunction(func_decl->get_definition());
	   if (func.first != NULL)
	   funcs.push_back(func.first);
	   if (func.second != NULL)
	   funcs.push_back(func.second);
	   */
    }
}

class A{};
class B:public A{};
struct abc : public std::binary_function<int, vector<B*>*, void>
{
    void operator () (first_argument_type a, vector<B*>* b) const
    {return;}
};

void foo(vector<A*>* b) {};
// Typical main function for ROSE translator
int main( int argc, char * argv[] )
{
    vector<A*> a;
    foo(&a);
    abc c;
    bind2nd(c, &a)(0);

    timeval tv;
    gettimeofday(&tv, NULL);

    //using namespace boost::date_time;
    using namespace boost::local_time;
 //local_microsec_clock::local_time();
    //cout << tv.tv_usec << ' ' << local_microsec_clock::local_time() << endl;

time_zone_ptr zone(
  new posix_time_zone("MST-07")
);
local_date_time ldt = 
  local_microsec_clock::local_time(
    zone);

    cout << tv.tv_usec << ' ' << ldt << endl;

 ldt = 
  local_sec_clock::local_time(zone);

    cout << tv.tv_sec << ' ' << ldt.local_time().time_of_day() << endl;

    return 0;

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

