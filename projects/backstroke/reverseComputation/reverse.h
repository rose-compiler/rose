#ifndef ROSE_PROJECT_REVERSE_COMPUTATION_H
#define ROSE_PROJECT_REVERSE_COMPUTATION_H

#include <rose.h>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/tuple/tuple.hpp>

#define foreach BOOST_FOREACH

//using std::vector;
//using std::pair;
//using std::string;
using namespace std;
using namespace boost;
using namespace SageBuilder;
using namespace SageInterface;

typedef std::pair<SgExpression*, SgExpression*> ExpPair;
typedef std::pair<SgStatement*, SgStatement*> StmtPair;
typedef std::pair<SgFunctionDeclaration*, SgFunctionDeclaration*> FuncDeclPair;

class EventReverser
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
    EventReverser(SgFunctionDeclaration* func_decl) 
	: func_decl_(func_decl), 
	function_name_(func_decl_->get_name()), 
	flag_(1), flag_saved_(0), counter_(0) 
    {
	flagstack_name_ = "flags";
    }
    vector<FuncDeclPair> outputFunctions();

    // Get all variables' declarations including all kinds of states
    vector<SgStatement*> getVarDeclarations()
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

	SgType* stack_type = buildPointerType(buildStructDeclaration("flag_stack")->get_type());
	decls.push_back(
		buildVariableDeclaration(flagstack_name_, stack_type));
	return decls;
    }

    vector<SgStatement*> getVarInitializers()
    {
	vector<SgStatement*> inits;
	// Initialize the flag stack object.
	SgType* stack_type = buildPointerType(buildStructDeclaration("flag_stack")->get_type());
	inits.push_back(buildAssignStatement(
		buildVarRefExp(flagstack_name_),
		buildFunctionCallExp("BuildFlagStack", stack_type)));
	return inits;
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
    SgExpression* getStateVar(SgExpression* var)
    {
	string name = function_name_ + "_state_" + lexical_cast<string>(counter_++);
	state_vars_[name] = var->get_type();
	return buildVarRefExp(name);
    }

    // Return the statement which saves the branch state
    SgStatement* putBranchFlag(bool flag = true)
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
    SgStatement* checkBranchFlag()
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

    SgStatement* newBranchFlag()
    {
	SgStatement* flag = buildFunctionCallStmt("Push", buildVoidType(), 
		buildExprListExp(buildVarRefExp(flagstack_name_)));
	return flag;
    }

    void saveBranchFlag() 
    {
	/* 
	   if (branch_flags_.empty())
	   branch_flags_.push_back(function_name_ + "_branch_flag" + lexical_cast<string>(counter_++));
	   flag_saved_ = flag_;
	   flag_name_saved_ = branch_flags_.back();
	   */
    }

    SgName generateVar(const string& name, SgType* type = buildIntType())
    {
	string var_name = function_name_ + "_" + name + "_" + lexical_cast<string>(counter_++);
	state_vars_[var_name] = type;
	return var_name;
    }

    SgStatement* initWhileCounter()
    {
	while_counters_.push_back(function_name_ + "_while_counter_" + lexical_cast<string>(counter_++));
	return buildExprStatement(
		buildBinaryExpression<SgAssignOp>(
		    buildVarRefExp(while_counters_.back()), 
		    buildIntVal(0)));
    }
};



#endif
