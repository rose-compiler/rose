#ifndef BACKSTROKE_EVENTPROCESSOR_H
#define	BACKSTROKE_EVENTPROCESSOR_H

#include <rose.h>
#include <utilities/types.h>
#include "variableVersionTable.h"
#include "costModel.h"

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



struct InstrumentedExpression
{
    InstrumentedExpression() {}
    InstrumentedExpression(
        SgExpression* exp1,
        SgExpression* exp2,
        const VariableVersionTable& table,
        const SimpleCostModel& cst = SimpleCostModel())
            : fwd_exp(exp1), rvs_exp(exp2), var_table(table), cost(cst)
    {}

    InstrumentedExpression clone()
    {
        return InstrumentedExpression(
                SageInterface::copyExpression(fwd_exp),
                SageInterface::copyExpression(rvs_exp),
                var_table, cost);
    }
    
    SgExpression* fwd_exp;
    SgExpression* rvs_exp;
    
    // Variable version table
    VariableVersionTable var_table;
    // Cost model
    SimpleCostModel cost;
};

struct InstrumentedStatement
{
    InstrumentedStatement() {}
    InstrumentedStatement(
        SgStatement* stmt1,
        SgStatement* stmt2,
        const VariableVersionTable& table,
        const SimpleCostModel& cst = SimpleCostModel())
            : fwd_stmt(stmt1), rvs_stmt(stmt2), var_table(table), cost(cst)
    {}

    InstrumentedStatement clone()
    {
        return InstrumentedStatement(
                SageInterface::copyStatement(fwd_stmt),
                SageInterface::copyStatement(rvs_stmt),
                var_table, cost);
    }
    
    SgStatement* fwd_stmt;
    SgStatement* rvs_stmt;

    // Variable version table
    VariableVersionTable var_table;
    // Cost model;
    SimpleCostModel cost;
};

//! Comparison functions for structure InstrumentedStatement and InstrumentedExpression.

inline bool operator < (const InstrumentedExpression& e1, const InstrumentedExpression& e2)
{ return e1.cost.getCost() < e2.cost.getCost(); }

inline bool operator < (const InstrumentedStatement& s1, const InstrumentedStatement& s2)
{ return s1.cost.getCost() < s2.cost.getCost(); }


typedef std::vector<InstrumentedExpression> InstrumentedExpressionVec;
typedef std::vector<InstrumentedStatement> InstrumentedStatementVec;







// Forward declaration of the class EventProcessor.
class EventProcessor;

class ProcessorBase
{
    EventProcessor* event_processor_;

protected:

    InstrumentedExpressionVec processExpression(SgExpression* exp, const VariableVersionTable& table, bool isReverseValueUsed);
    InstrumentedStatementVec processStatement(const StatementPackage& stmt_pkg);

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

    virtual InstrumentedExpressionVec process(SgExpression* exp, const VariableVersionTable& table, bool isReverseValueUsed) = 0;
    //virtual void getCost() = 0;

};


class StatementProcessor : public ProcessorBase
{
public:

    virtual InstrumentedStatementVec process(const StatementPackage& stmt_pkg) = 0;

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

    //! Given an expression, return all transformations using all expression processors.
    InstrumentedExpressionVec processExpression(SgExpression* exp, const VariableVersionTable& table, bool isReverseValueUsed);

    //! Given a statement, return all transformations using all statement processors.
    InstrumentedStatementVec processStatement(const StatementPackage& stmt_pkg);

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

