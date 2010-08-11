#ifndef BACKSTROKE_EVENTPROCESSOR_H
#define	BACKSTROKE_EVENTPROCESSOR_H

#include <rose.h>
#include <utilities/types.h>
#include "variableVersionTable.h"
#include "costModel.h"

class ExpressionProcessor;
class StatementProcessor;

struct ExpressionPackage
{
    ExpressionPackage(
            SgExpression* e,
            const VariableVersionTable& table,
            bool is_rvs_val_used = false)
    : exp(e), var_table(table), is_value_used(is_rvs_val_used)
    {}

    SgExpression* exp;
    VariableVersionTable var_table;
    bool is_value_used;

    // The following two stacks are for tracking what kind of expression
    // or statement processors are used in order to make the correct transformation
    // finally.
    std::stack<ExpressionProcessor*> exp_processors;
    std::stack<StatementProcessor*> stmt_processors;
};

struct StatementPackage
{
    StatementPackage(
            SgStatement* s,
            const VariableVersionTable& table)
    : stmt(s), var_table(table)
    {}

    SgStatement* stmt;
    VariableVersionTable var_table;

    // The following two stacks are for tracking what kind of expression
    // or statement processors are used in order to make the correct transformation
    // finally.
    std::stack<ExpressionProcessor*> exp_processors;
    std::stack<StatementProcessor*> stmt_processors;
};

struct EvaluationResult
{
    EvaluationResult(const VariableVersionTable& table,
            const SimpleCostModel& cost_model = SimpleCostModel())
        : var_table(table), cost(cost_model) {}

    // Variable version table
    VariableVersionTable var_table;
    // Cost model
    SimpleCostModel cost;

    // The following two stacks are for tracking what kind of expression
    // or statement processors are used in order to make the correct transformation
    // finally.
    std::stack<ExpressionProcessor*> exp_processors;
    std::stack<StatementProcessor*> stmt_processors;
};

struct ProcessedExpression
{
    ProcessedExpression(SgExpression* fwd, SgExpression* rvs)
        : fwd_exp(fwd), rvs_exp(rvs) {}

    SgExpression* fwd_exp;
    SgExpression* rvs_exp;
};

struct ProcessedStatement
{
    ProcessedStatement(SgStatement* fwd, SgStatement* rvs)
        : fwd_stmt(fwd), rvs_stmt(rvs) {}

    SgStatement* fwd_stmt;
    SgStatement* rvs_stmt;
};


//! Comparison functions for structure InstrumentedStatement and InstrumentedExpression.

inline bool operator < (const EvaluationResult& r1, const EvaluationResult& r2)
{ return r1.cost.getCost() < r2.cost.getCost(); }




// Forward declaration of the class EventProcessor.
class EventProcessor;

class ProcessorBase
{
    EventProcessor* event_processor_;

protected:

    std::vector<ProcessedExpression*> processExpression(const ExpressionPackage& exp_pkg);
    std::vector<ProcessedStatement*>  processStatement(const StatementPackage& stmt_pkg);

    std::vector<EvaluationResult> evaluateExpression(const ExpressionPackage& exp_pkg);
    std::vector<EvaluationResult> evaluateStatement(const StatementPackage& stmt_pkg);

    SgExpression* pushVal(SgExpression* exp, SgType* type);
    SgExpression* popVal(SgType* type);

    //! Return if the given variable is a state variable (currently, it should be the parameter of event function).
    bool isStateVariable(SgExpression* exp);

public:

    ProcessorBase() : event_processor_(NULL) {}
    
    void setEventProcessor(EventProcessor* processor)
    {
        event_processor_ = processor;
    }
};

class ExpressionProcessor : public ProcessorBase
{
public:

    virtual ProcessedExpression process(SgExpression* exp) = 0;
    virtual std::vector<EvaluationResult> evaluate(const ExpressionPackage& exp_pkg) = 0;
    //virtual void getCost() = 0;

    EvaluationResult makeEvaluationResult(
            const ExpressionPackage& exp_pkg,
            const VariableVersionTable& var_table,
            const SimpleCostModel& cost = SimpleCostModel())
    {
        EvaluationResult result(var_table, cost);
        result.exp_processors = exp_pkg.exp_processors;
        result.exp_processors.push(this);
        return result;
    }

    EvaluationResult makeEvaluationResult(
            const ExpressionPackage& exp_pkg,
            const SimpleCostModel& cost = SimpleCostModel())
    {
        EvaluationResult result(exp_pkg.var_table, cost);
        result.exp_processors = exp_pkg.exp_processors;
        result.exp_processors.push(this);
        return result;
    }
};


class StatementProcessor : public ProcessorBase
{
public:

    virtual ProcessedStatement process(
            SgStatement* stmt, 
            std::stack<ExpressionProcessor*>& exp_processors,
            std::stack<StatementProcessor*>& stmt_processors) = 0;
    virtual std::vector<EvaluationResult> evaluate(const StatementPackage& stmt_pkg) = 0;

    //virtual S

    //virtual void getCost() = 0;
};


class VariableRenaming;

class EventProcessor
{
    //! This is the current event function to handle.
    SgFunctionDeclaration* event_;

    //! All expression processors which are added by the user.
    std::vector<ExpressionProcessor*> exp_processors_;

    //! All statement processors which are added by the user.
    std::vector<StatementProcessor*> stmt_processors_;

    //! All declarations of stacks which store values of different types.
    std::map<std::string, SgVariableDeclaration*> stack_decls_;

    //! The variable renaming analysis object.
    VariableRenaming* var_renaming_;

    //! The variable version table which record final version of all variables in the event.

    //! Make those two classes the friends to let them use some private methods.
    friend class ProcessorBase;

private:

    ProcessedStatement processStatement(SgStatement* stmt, EvaluationResult& result);

    //! Given an expression, return all evaluation results using all expression processors.
    std::vector<EvaluationResult> evaluateExpression(const ExpressionPackage& exp_pkg);

    //! Given a statement, return all evaluation results using all statement processors.
    std::vector<EvaluationResult> evaluateStatement(const StatementPackage& stmt_pkg);

    //! The following methods are for expression and statement processors for store and restore.
    SgExpression* getStackVar(SgType* type);
    SgExpression* pushVal(SgExpression* exp, SgType* type);
    SgExpression* popVal(SgType* type);


public:
    
    EventProcessor(SgFunctionDeclaration* func_decl = NULL, VariableRenaming* var_renaming = NULL)
    : event_(func_decl), var_renaming_(var_renaming) {}

    void addExpressionProcessor(ExpressionProcessor* exp_processor)
    {
        exp_processor->setEventProcessor(this);
        exp_processors_.push_back(exp_processor);
    }

    void addStatementProcessor(StatementProcessor* stmt_processor)
    {
        stmt_processor->setEventProcessor(this);
        stmt_processors_.push_back(stmt_processor);
    }

    FuncDeclPairs processEvent();

    FuncDeclPairs processEvent(SgFunctionDeclaration* event)
    {
        event_ = event;
        //stack_decls_.clear();
        return processEvent();
    }

    //! Return if the given variable is a state variable (currently, it should be the parameter of event function).
    bool isStateVariable(SgExpression* exp);

    //! Get all declarations of stacks which store values of different types.
    std::vector<SgVariableDeclaration*> getAllStackDeclarations() const;
};


#endif	/* BACKSTROKE_EVENTPROCESSOR_H */

