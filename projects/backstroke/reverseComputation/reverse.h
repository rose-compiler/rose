#ifndef ROSE_REVERSE_COMPUTATION_REVERSE_H
#define ROSE_REVERSE_COMPUTATION_REVERSE_H

#include <rose.h>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
//#include <boost/tuple/tuple.hpp>

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
    //vector<string> branch_flags_;
    vector<string> loop_counters_;
    map<string, SgType*> state_vars_;
    int flag_;
    int flag_saved_;
    string flagstack_name_;
    int counter_;
    vector<FuncDeclPair> output_func_pairs_;

    static set<SgFunctionDeclaration*> func_processed_;

    public:
    EventReverser(SgFunctionDeclaration* func_decl) 
        : func_decl_(func_decl), 
        function_name_(func_decl_->get_name()), 
        flagstack_name_(function_name_ + "_flag"),
        flag_(1), 
        flag_saved_(0), 
        counter_(0)
    {
    }

    // All functions generated. If the reverser meets a function call, it may reverse 
    // that function then add the resulted function pair into the function pair collection. 
    vector<FuncDeclPair> outputFunctions();

    // Get all variables' declarations including all kinds of states
    vector<SgStatement*> getVarDeclarations()
    {
        vector<SgStatement*> decls;
        //foreach(const string& flag, branch_flags_)
        //    decls.push_back(buildVariableDeclaration(flag, buildIntType()));

        foreach(const string& counter, loop_counters_)
            decls.push_back(buildVariableDeclaration(
                        counter, 
                        buildIntType(), 
                        buildAssignInitializer(buildIntVal(0))));

        pair<string, SgType*> state_var;
        foreach(state_var, state_vars_)
        {
            decls.push_back(buildVariableDeclaration(state_var.first, state_var.second));
        }

        SgType* stack_type = buildPointerType(buildStructDeclaration("FlagStack")->get_type());
        decls.push_back(
                buildVariableDeclaration(flagstack_name_, stack_type));
        return decls;
    }

    vector<SgStatement*> getVarInitializers()
    {
        vector<SgStatement*> inits;
        // Initialize the flag stack object.
        SgType* stack_type = buildPointerType(buildStructDeclaration("FlagStack")->get_type());
        inits.push_back(buildAssignStatement(
                    buildVarRefExp(flagstack_name_),
                    buildFunctionCallExp("buildFlagStack", stack_type)));
        return inits;
    }

    private:
    static const ExpPair NULL_EXP_PAIR;
    static const StmtPair NULL_STMT_PAIR;

    // **********************************************************************************
    // Main functions, which process expressions and statements
    // ==================================================================================

    // Just reverse an expression
    SgExpression* reverseExpression(SgExpression* exp);

    // Get the forward and reverse version of an expression
    ExpPair instrumentAndReverseExpression(SgExpression* exp);

    // Get the forward and reverse version of an statement 
    StmtPair instrumentAndReverseStatement(SgStatement* stmt);

    // **********************************************************************************
    // Auxiliary functions
    // ==================================================================================

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

    // Generate a name containing the function name and counter
    SgName generateVar(const string& name, SgType* type = buildIntType())
    {
        string var_name = function_name_ + "_" + name + "_" + lexical_cast<string>(counter_++);
        state_vars_[var_name] = type;
        return var_name;
    }

    // **********************************************************************************
    // The following function is for state saving
    // ==================================================================================

    // Get the variable name which saves the branch state
    SgExpression* getStateVar(SgExpression* var)
    {
        string name = function_name_ + "_state_" + lexical_cast<string>(counter_++);
        state_vars_[name] = var->get_type();
        return buildVarRefExp(name);
    }

    // **********************************************************************************
    // The following functions are for if statement
    // ==================================================================================

    // Return the statement which saves the branch state
    SgExpression* putBranchFlagExp(bool flag = true)
    {
        return buildFunctionCallExp("pushFlag", buildIntType(), 
                buildExprListExp(buildVarRefExp(flagstack_name_), buildIntVal(flag)));
    }

    SgExpression* putBranchFlagExp(SgExpression* exp)
    {
        return buildFunctionCallExp("pushFlag", buildIntType(), 
                buildExprListExp(buildVarRefExp(flagstack_name_), exp));
    }

    SgStatement* putBranchFlagStmt(bool flag = true)
    {
        return buildExprStatement(putBranchFlagExp(flag));
    }

    // Return the statement which checks the branch flag
    SgExpression* checkBranchFlagExp()
    {
        return buildFunctionCallExp("popFlag", buildVoidType(), 
                buildExprListExp(buildVarRefExp(flagstack_name_)));
    }

    SgStatement* checkBranchFlagStmt()
    {
        return buildExprStatement(checkBranchFlagExp());
    }

#if 0
    SgExpression* newBranchFlag()
    {
        return buildFunctionCallExp("Push", buildVoidType(), 
                buildExprListExp(buildVarRefExp(flagstack_name_)));
    }
#endif

    // **********************************************************************************

    SgStatement* initLoopCounter()
    {
        loop_counters_.push_back(function_name_ + "_loop_counter_" + lexical_cast<string>(counter_++));
        return buildExprStatement(
                buildBinaryExpression<SgAssignOp>(
                    buildVarRefExp(loop_counters_.back()), 
                    buildIntVal(0)));
    }

    SgExpression* getLoopCounter()
    {
        string counter_name = function_name_ + "_loop_counter_" + lexical_cast<string>(counter_++);
        loop_counters_.push_back(counter_name);
        return buildVarRefExp(counter_name);
    }
};



#endif
