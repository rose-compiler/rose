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
    //vector<string> loop_counters_;
    //map<string, SgType*> state_vars_;

    string flag_stack_name_;
    string int_stack_name_;
    string float_stack_name_;
    string counter_stack_name_;

    int counter_;
    vector<FuncDeclPair> output_func_pairs_;

    // This mark is for and/or and conditional expression, in which the special stack
    // interfaces are used.
    int branch_mark_;

    static set<SgFunctionDeclaration*> func_processed_;

    public:
    EventReverser(SgFunctionDeclaration* func_decl) 
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

    // All functions generated. If the reverser meets a function call, it may reverse 
    // that function then add the resulted function pair into the function pair collection. 
    vector<FuncDeclPair> outputFunctions();

    // Get all variables' declarations including all kinds of states
    vector<SgStatement*> getVarDeclarations()
    {
        vector<SgStatement*> decls;
        //foreach(const string& flag, branch_flags_)
        //    decls.push_back(buildVariableDeclaration(flag, buildIntType()));

#if 0
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
#endif

        SgType* stack_type = buildPointerType(buildStructDeclaration("IntStack")->get_type());
        decls.push_back(buildVariableDeclaration(flag_stack_name_, stack_type));
        decls.push_back(buildVariableDeclaration(int_stack_name_, stack_type));
        decls.push_back(buildVariableDeclaration(counter_stack_name_, stack_type));
        
        // Float type stack
        decls.push_back(buildVariableDeclaration(float_stack_name_, stack_type));

        return decls;
    }

    vector<SgStatement*> getVarInitializers()
    {
        vector<SgStatement*> inits;
        SgType* stack_type = buildPointerType(buildStructDeclaration("IntStack")->get_type());

        // Initialize the flag stack object.
        inits.push_back(buildAssignStatement(
                    buildVarRefExp(flag_stack_name_),
                    buildFunctionCallExp("buildIntStack", stack_type)));

        // Initialize the integer stack object.
        inits.push_back(buildAssignStatement(
                    buildVarRefExp(int_stack_name_),
                    buildFunctionCallExp("buildIntStack", stack_type)));

        // Initialize the integer stack object.
        inits.push_back(buildAssignStatement(
                    buildVarRefExp(float_stack_name_),
                    buildFunctionCallExp("buildIntStack", stack_type)));

        // Initialize the loop counter stack object.
        inits.push_back(buildAssignStatement(
                    buildVarRefExp(counter_stack_name_),
                    buildFunctionCallExp("buildIntStack", stack_type)));

        return inits;
    }

    private:
    static const ExpPair NULL_EXP_PAIR;
    static const StmtPair NULL_STMT_PAIR;

    // **********************************************************************************
    // Main functions, which process expressions and statements
    // ==================================================================================

    // Just reverse an expression
    //SgExpression* reverseExpression(SgExpression* exp);

    // Get the forward and reverse version of an expression
    ExpPair instrumentAndReverseExpression(SgExpression* exp);

    // Get the forward and reverse version of an statement 
    StmtPair instrumentAndReverseStatement(SgStatement* stmt);

    ExpPair processUnaryOp(SgUnaryOp* unary_op);
    ExpPair processBinaryOp(SgBinaryOp* bin_op);
    ExpPair processConditionalExp(SgConditionalExp* cond_exp);
    ExpPair processFunctionCallExp(SgFunctionCallExp* func_exp);

    StmtPair processExprStatement(SgExprStatement* exp_stmt);
    StmtPair processBasicBlock(SgBasicBlock* body);
    StmtPair processVariableDeclaration(SgVariableDeclaration* var_decl);
    StmtPair processIfStmt(SgIfStmt* if_stmt);
    StmtPair processForStatement(SgForStatement* for_stmt);
    StmtPair processForInitStatement(SgForInitStatement* for_init_stmt);
    StmtPair processWhileStmt(SgWhileStmt* while_stmt);
    StmtPair processDoWhileStmt(SgDoWhileStmt* do_while_stmt);
    StmtPair processSwitchStatement(SgSwitchStatement* switch_stmt);

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
            if (isSgArrowExp(ref_exp->get_lhs_operand()))
                return ref_exp;
        }
        return NULL;
    }


#if 0
    // Generate a name containing the function name and counter
    SgName generateVar(const string& name, SgType* type = buildIntType())
    {
        string var_name = function_name_ + "_" + name + "_" + lexical_cast<string>(counter_++);
        state_vars_[var_name] = type;
        return var_name;
    }
#endif

    // **********************************************************************************
    // The following function is for state saving
    // ==================================================================================

#if 0
    // Get the variable name which saves the branch state
    SgExpression* getStateVar(SgExpression* var)
    {
        string name = function_name_ + "_state_" + lexical_cast<string>(counter_++);
        state_vars_[name] = var->get_type();
        return buildVarRefExp(name);
    }
#endif

    // Push an integer into integer stack. Can be used to save states. 
    SgExpression* pushIntVal(SgExpression* var)
    {
        if (branch_mark_ < 0)
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
    SgExpression* popIntVal()
    {
        return buildFunctionCallExp("pop", buildVoidType(), 
                buildExprListExp(buildVarRefExp(int_stack_name_)));
    }

    // Push a float into float stack. Can be used to save states. 
    SgExpression* pushFloatVal(SgExpression* var)
    {
        return buildFunctionCallExp(
                "push", 
                buildIntType(), 
                buildExprListExp(
                    buildVarRefExp(float_stack_name_), 
                    var)); 
    }

    // Pop the stack and get the value.
    SgExpression* popFloatVal()
    {
        return buildFunctionCallExp("pop", buildVoidType(), 
                buildExprListExp(buildVarRefExp(float_stack_name_)));
    }

    // For a local variable, return two statements to store its value and 
    // declare and assign the retrieved value to it.
    StmtPair pushAndPopLocalVar(SgVariableDeclaration* var_decl)
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

    // **********************************************************************************
    // The following functions are for logic and/or expression, conditional expression, 
    // and if statement
    // ==================================================================================

    // For and/or & conditional expressions. The value pushed is either 0 or 1. 
    SgExpression* putBranchFlagExp(SgExpression* exp)
    {
        return buildFunctionCallExp(
                "push", 
                buildIntType(), 
                buildExprListExp(
                    buildVarRefExp(flag_stack_name_), 
                    exp, 
                    buildIntVal(branch_mark_)));
    }

    SgStatement* putBranchFlagStmt(bool flag = true)
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
    SgExpression* checkBranchFlagExp()
    {
        return buildFunctionCallExp("pop", buildVoidType(), 
                buildExprListExp(buildVarRefExp(flag_stack_name_)));
    }

    SgStatement* checkBranchFlagStmt()
    {
        return buildExprStatement(checkBranchFlagExp());
    }


    // **********************************************************************************
    // The following functions are for "for", "while", and "do-while" statement
    // ==================================================================================

    // This function add the loop counter related statements (counter declaration, increase, and store)
    // to a forward loop statement. 
    SgStatement* assembleLoopCounter(SgStatement* loop_stmt);

    // This function wrap the loop body with a for statement. Note that we retrieve the loop count
    // from the stack and assign it to the counter in the initilization part in for.
    SgStatement* buildForLoop(SgStatement* loop_body);

    SgExpression* popLoopCounter()
    {
        return buildFunctionCallExp("pop", buildVoidType(), 
                buildExprListExp(buildVarRefExp(counter_stack_name_)));
    }

#if 0
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
#endif

    // **********************************************************************************
    // The following two functions are used to temporarily turn part of a stack
    // into a queue since the flags are FIFO for and/or and conditional operators.
    // ==================================================================================
    void beginFIFO() { ++branch_mark_; }
    void endFIFO() { --branch_mark_; }
};



#endif
