#ifndef BACKSTROKE_EVENTPROCESSOR_H
#define	BACKSTROKE_EVENTPROCESSOR_H

#include <rose.h>
#include <utilities/types.h>
#include <boost/foreach.hpp>
#include "variableVersionTable.h"
#include "costModel.h"

#define foreach BOOST_FOREACH

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
};

class EvaluationResult
{
    // Variable version table
    VariableVersionTable var_table_;
    // Cost model
    SimpleCostModel cost_;

    std::vector<ExpressionProcessor*> exp_processors_;
    std::vector<StatementProcessor*> stmt_processors_;

public:

    EvaluationResult(const VariableVersionTable& table,
            const SimpleCostModel& cost_model = SimpleCostModel())
        : var_table_(table), cost_(cost_model) {}

    // In this update function, update every possible item in this structure.
    // Note the order!
    void update(const EvaluationResult& result)
    {
        var_table_ = result.var_table_;
        cost_ += result.cost_;

        exp_processors_.insert(exp_processors_.end(), result.exp_processors_.begin(), result.exp_processors_.end());
        stmt_processors_.insert(stmt_processors_.end(), result.stmt_processors_.begin(), result.stmt_processors_.end());
    }

    void addExpressionProcessor(ExpressionProcessor* exp_processor)
    { exp_processors_.push_back(exp_processor); }

    void addStatementProcessor(StatementProcessor* stmt_processor)
    { stmt_processors_.push_back(stmt_processor); }

    const VariableVersionTable& getVarTable() const
    { return var_table_; }

    VariableVersionTable& getVarTable() 
    { return var_table_; }

    const SimpleCostModel& getCost() const
    { return cost_; }

    const std::vector<ExpressionProcessor*>& getExpressionProcessors() const
    { return exp_processors_; }

    const std::vector<StatementProcessor*>& getStatementProcessors() const
    { return stmt_processors_; }
};

struct ExpressionReversal
{
    ExpressionReversal(SgExpression* fwd, SgExpression* rvs)
        : fwd_exp(fwd), rvs_exp(rvs) {}

    SgExpression* fwd_exp;
    SgExpression* rvs_exp;
};

struct StatementReversal
{
    StatementReversal(SgStatement* fwd, SgStatement* rvs)
        : fwd_stmt(fwd), rvs_stmt(rvs) {}

    SgStatement* fwd_stmt;
    SgStatement* rvs_stmt;
};


//! Comparison functions for structure InstrumentedStatement and InstrumentedExpression.

inline bool operator < (const EvaluationResult& r1, const EvaluationResult& r2)
{ return r1.getCost().getCost() < r2.getCost().getCost(); }




// Forward declaration of the class EventProcessor.
class EventProcessor;

class ProcessorBase
{
    EventProcessor* event_processor_;

protected:
    std::string name_;

    ExpressionReversal processExpression(SgExpression* exp);
    StatementReversal processStatement(SgStatement* stmt);

    std::vector<EvaluationResult> evaluateExpression(SgExpression* exp, const VariableVersionTable& var_table, bool is_value_used = false);
    std::vector<EvaluationResult> evaluateStatement(SgStatement* stmt, const VariableVersionTable& var_table);

    SgExpression* pushVal(SgExpression* exp, SgType* type);
    SgExpression* popVal(SgType* type);

    //! Return if the given variable is a state variable (currently, it should be the parameter of event function).
    bool isStateVariable(SgExpression* exp);

public:

    ProcessorBase() : event_processor_(NULL) {}
    ProcessorBase(const std::string& name) : event_processor_(NULL), name_(name) {}
    
    std::string getName() const { return name_; }

    void setEventProcessor(EventProcessor* processor)
    {
        event_processor_ = processor;
    }
};

class ExpressionProcessor : public ProcessorBase
{
public:

    virtual ExpressionReversal process(SgExpression* exp) = 0;
    virtual std::vector<EvaluationResult> evaluate(SgExpression* exp, const VariableVersionTable& var_table, bool is_value_used) = 0;
    //virtual void getCost() = 0;

    // Note this function is a wrapper which is called by event processor.
    std::vector<EvaluationResult> evaluate_(SgExpression* exp, const VariableVersionTable& var_table, bool is_value_used)
    {
        std::vector<EvaluationResult> results = evaluate(exp, var_table, is_value_used);
        foreach (EvaluationResult& result, results)
        {
            result.addExpressionProcessor(this);
            std::cout << "Processor added: " << typeid(this).name() << std::endl;
        }
        return results;
    }
};


class StatementProcessor : public ProcessorBase
{
public:

    virtual StatementReversal process(SgStatement* stmt) = 0; 
    virtual std::vector<EvaluationResult> evaluate(SgStatement* stmt, const VariableVersionTable& var_table) = 0;

    // Note this function is a wrapper which is called by event processor.
    std::vector<EvaluationResult> evaluate_(SgStatement* stmt, const VariableVersionTable& var_table)
    {
        std::vector<EvaluationResult> results = evaluate(stmt, var_table);
        foreach (EvaluationResult& result, results)
        {
            result.addStatementProcessor(this);
            std::cout << "Processor added:" << typeid(this).name() << std::endl;
        }
        return results;
    }


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

    /*! The following processors are different from the ones above, and they 
     are for generating code. */
    std::vector<ExpressionProcessor*> exp_processors;
    std::vector<StatementProcessor*> stmt_processors;

    //! All declarations of stacks which store values of different types.
    std::map<std::string, SgVariableDeclaration*> stack_decls_;

    //! The variable renaming analysis object.
    VariableRenaming* var_renaming_;

    //! The variable version table which record final version of all variables in the event.

    //! Make those two classes the friends to let them use some private methods.
    friend class ProcessorBase;

private:

    ExpressionReversal processExpression(SgExpression* exp);
    StatementReversal processStatement(SgStatement* stmt);
    StatementReversal processStatement(SgStatement* stmt, const EvaluationResult& result);

    //! Given an expression, return all evaluation results using all expression processors.
    std::vector<EvaluationResult> evaluateExpression(SgExpression* exp, const VariableVersionTable& var_table, bool is_value_used);

    //! Given a statement, return all evaluation results using all statement processors.
    std::vector<EvaluationResult> evaluateStatement(SgStatement* stmt, const VariableVersionTable& var_table);

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

