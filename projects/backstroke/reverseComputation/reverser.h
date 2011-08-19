#ifndef ROSE_REVERSE_COMPUTATION_REVERSE_H
#define ROSE_REVERSE_COMPUTATION_REVERSE_H

#include <rose.h>
#include <boost/function.hpp>
#include <string>

typedef std::pair<SgExpression*, SgExpression*> ExpPair;
typedef std::pair<SgStatement*, SgStatement*> StmtPair;
typedef std::pair<SgFunctionDeclaration*, SgFunctionDeclaration*> FuncDeclPair;

class EventReverser
{
	/** Defining declaration of the function being reversed by this reverser. */
    SgFunctionDeclaration* func_decl_;

    std::string function_name_;
    //std::vector<string> branch_flags_;
    //std::vector<string> loop_counters_;
    //std::map<string, SgType*> state_vars_;

    std::string flag_stack_name_;
    std::string int_stack_name_;
    std::string float_stack_name_;
    std::string counter_stack_name_;

    int counter_;

	/** Map from the original function declaration to the forward-reverse pair. */
    std::map<SgFunctionDeclaration*, FuncDeclPair> output_func_pairs_;

    // This mark is for and/or and conditional expression, in which the special stack
    // interfaces are used.
    int branch_mark_;

    static std::set<SgFunctionDeclaration*> func_processed_;

    /** A list of all the expression handlers that we will try first.
     * If these handlers can't reverse an expression, we fall back to state saving. */
    static std::vector <boost::function<ExpPair (SgExpression*) > > expressionHandlers;

    public:
    EventReverser(SgFunctionDeclaration* func_decl,
            DFAnalysis* analysis = NULL);

    /** All functions generated. If the reverser meets a function call, it may reverse
     * that function then add the resulted function pair into the function pair collection.
     * The reverse of the original function is the very last function in this list. */
    std::map<SgFunctionDeclaration*, FuncDeclPair> outputFunctions();

    /** Get the variables that need to be declared for use in the reverse methods.
     * Make sure these declarations are inserted in a scope reachable by the generated functions. */
    std::vector<SgVariableDeclaration*> getVarDeclarations();

    /** Get the initialization statements of all the variables accessed by the reverse methods. */
    std::vector<SgAssignOp*> getVarInitializers();

    /** Add a method that can generate forward and reverse expressions from the given expression. */
    static void registerExpressionHandler(boost::function<ExpPair (SgExpression*)> handler)
    {
        expressionHandlers.push_back(handler);
    }

    static const ExpPair NULL_EXP_PAIR;
    static const StmtPair NULL_STMT_PAIR;

    private:


    // **********************************************************************************
    // Main functions, which process expressions and statements
    // ==================================================================================

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
    SgExpression* isStateVar(SgExpression* exp);

    bool toSave(SgExpression* exp);


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
    SgExpression* pushIntVal(SgExpression* var);

    // Pop the stack and get the value.
    SgExpression* popIntVal();

    // Push a float into float stack. Can be used to save states. 
    SgExpression* pushFloatVal(SgExpression* var);

    // Pop the stack and get the value.
    SgExpression* popFloatVal();

    // For a local variable, return two statements to store its value and 
    // declare and assign the retrieved value to it.
    StmtPair pushAndPopLocalVar(SgVariableDeclaration* var_decl);

    // **********************************************************************************
    // The following functions are for logic and/or expression, conditional expression, 
    // and if statement
    // ==================================================================================

    // For and/or & conditional expressions. The value pushed is either 0 or 1. 
    SgExpression* putBranchFlagExp(SgExpression* exp);

    SgStatement* putBranchFlagStmt(bool flag = true);

    // Return the statement which checks the branch flag
    SgExpression* checkBranchFlagExp();

    SgStatement* checkBranchFlagStmt();

    // **********************************************************************************
    // The following functions are for "for", "while", and "do-while" statement
    // ==================================================================================

    // This function add the loop counter related statements (counter declaration, increase, and store)
    // to a forward loop statement. 
    SgStatement* assembleLoopCounter(SgStatement* loop_stmt);

    // This function wrap the loop body with a for statement. Note that we retrieve the loop count
    // from the stack and assign it to the counter in the initilization part in for.
    SgStatement* buildForLoop(SgStatement* loop_body);

    SgExpression* popLoopCounter();

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
